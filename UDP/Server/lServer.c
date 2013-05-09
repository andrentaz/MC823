#include<stdio.h>
#include<stdlib.h>
#include<string.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<errno.h>
#include<sys/wait.h>
#include<signal.h>
#include<unistd.h>

// SQLite3
#include <sqlite3.h>

#define PASSWORD "numaPistacheCottapie"

typedef struct livro {
	char i[20];	// ISBN
	char q[4];	// Stock Quantity
} Livro;

int socket_desc , client_sock , c , read_size, connfd;

// Tempo
long elapsed=0;			// Conta o tempo percorrido
struct timeval t0, t1;

// Funcao que persiste no send ate toda a mensagem ser enviada
int sendall(int s, char *buf, int *len)
{
//	gettimeofday(&t0, 0);
	int total = 0;
	// how many bytes we've sent
	int bytesleft = *len; // how many we have left to send
	int n;
	while(total < *len) {
		gettimeofday(&t1, 0);
		elapsed += (t1.tv_sec-t0.tv_sec)*1000000 + t1.tv_usec-t0.tv_usec;
		// Ignorar tempo de send
		n = send(s, buf+total, bytesleft, 0);
		// Voltando a calcular o tempo de operacao
		gettimeofday(&t0, 0);
		if (n == -1) { break; }
		total += n;
		bytesleft -= n;
	}
	gettimeofday(&t1, 0);
	elapsed += (t1.tv_sec-t0.tv_sec)*1000000 + t1.tv_usec-t0.tv_usec;
	gettimeofday(&t0, 0);
	*len = total; // return number actually sent here
	return n==-1?-1:0; // return -1 on failure, 0 on success
}

// Callback do SQLite3. Versao silenciosa, nao envia nada para o cliente
static int callbackSilent(void *NotUsed, int argc, char **ans, char **azColName){
	int i, num;
	int *v;
	v = (int *)alloca(sizeof(int));
	// Aaaaaaah Moleque!!!!!
	v = NotUsed;
	*v = atoi(ans[0]);
//	printf("\n--------\n%d\n--------\n",*v);
	//sendall(client_sock, aux, &num);
	return 0;
}

// Callback do SQLite3. Versao formatada para varios detalhes
static int callbackFmt(void *NotUsed, int argc, char **ans, char **azColName){
	int i, num;
	char aux[20000];
	
	// Nome da coluna
	strcpy(aux,azColName[0]);
	strcat(aux,": ");
	// Concatena valor
	strcat(aux,ans[0]);
	strcat(aux,"\n");
	// Formata
	for(i=1; i<argc; i++) {		// Fazendo isso para o resto da tupla
		if (ans[i]) {
			strcat(aux,azColName[i]);
			strcat(aux,": ");
			// Melhorando a visualizacao da descricao
			if (i==3) {
				strcat(aux,"\n\t");
			}
			strcat(aux,ans[i]);
			strcat(aux,"\n");
		}
	}
	strcat(aux,"\n");
	// DEBUG
	printf("%s", aux);
	// Enviando para o cliente
	num = strlen(aux);
	sendall(client_sock, aux, &num);
	return 0;
}

// Callback formatado em tupla simples
static int callback(void *NotUsed, int argc, char **ans, char **azColName){
	int i, num;
	char aux[20000];
	
	strcpy(aux,ans[0]);
	for(i=0; i<argc; i++){
		if (i && ans[i]) {
			strcat(aux," | ");
			strcat(aux,ans[i]);
		}
	}
	strcat(aux,"\n");
	printf("%s", aux);
	num = strlen(aux);
	sendall(client_sock, aux, &num);
	return 0;
}

// Funcao que espera chamado dos filhos
void sigchld_handler(int s)
{
	// -1 	: chamadas de processos-filhos, apenas
	// NULL : nao precisa ler estado
	while(waitpid(-1, NULL, WNOHANG) > 0);
}

int main(int argc , char *argv[])
{
	struct sockaddr_in server, client;
	struct sigaction sa;
	pid_t childpid;
	socklen_t clilen;
	char client_message[2000], query[2500], query2[2500];

	int opcao, length=1;

	// SQLite3
	sqlite3 *db;
	char *zErrMsg = 0, *msg;
	int rc;
	void *existe; 				// Para requisicoes invalidas
	int superuser = 0; 			// Cliente Livraria
	existe = (void *)alloca(sizeof(int));	// Usado para saber se o ISBN existe
	Livro cm;

	// Timeout setado para imprevistos ...
	struct timeval tv;

	tv.tv_sec = 5;  /* 30 Secs Timeout */
	tv.tv_usec = 0;  // Not init'ing this can cause strange errors

	setsockopt(connfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv,sizeof(struct timeval));
	// ... Timeout setado.

	// Abrindo Banco de Dados do SQLite3
	rc = sqlite3_open("livraria2.db", &db);
	//Create socket
	socket_desc = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_desc == -1)
	{
		printf("Could not create socket");
	}
	puts("Socket created");

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons( 8888 );

	//Bind
	if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
	{
		//print the error message
		perror("bind failed. Error");
		return 1;
	}
	puts("bind done");

	//Listen
	listen(socket_desc , 3);

	// Matando todos os processos zumbis
	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}


	while (1) {
		//Accept and incoming connection
		puts("Waiting for incoming connections...");
		c = sizeof(struct sockaddr_in);

		//accept connection from an incoming client
		client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
		if (client_sock < 0)
		{
			perror("accept failed");
			if(errno == EINTR) {
				continue;
			}
			else {
				//	return 1;
				perror("Erro no accept!");
			}
		}
		puts("Connection accepted");

		// Criando um processo-filho para tratar a requisicao
		if ( (childpid = fork()) == 0 ) {
			// Processo-filho nao trata requisicoes para novas coneccoes
			close(socket_desc);

			// Servidor em espera da requisicao do cliente
			while( (read_size = recv(client_sock , client_message , 2000 , 0)) > 0 )
			{
				// Comecando a contar o tempo de operacao
				gettimeofday(&t0, 0);

				opcao = atoi(client_message);

				switch(opcao) {
					case 1:		// Lista de ISBN e titulo dos livros
						elapsed = 0;
						// Enviando tuplas de ISBN e titulo de todos os livros
						rc = sqlite3_exec(db, "select ISBN10,titulo from livro;", callback, 0, &zErrMsg);
						// Tempo percorrido ate agora
						gettimeofday(&t1, 0);
						if( rc!=SQLITE_OK ){
							sqlite3_free(zErrMsg);
						}

						// Finalizando a mensagem
						// Calculo do tempo de operacao
						elapsed = (t1.tv_sec-t0.tv_sec)*1000000 + t1.tv_usec-t0.tv_usec;
						// Transformando em string com caracteres de "seguranca" para postumo atoi
						sprintf(query,"          %6li",elapsed);	// Caractere  e um identificador de fim da mensagem
						// DEBUG
						printf("\nOperation Time: %s\n\n",query);
						// Calculando o tamanho
						length = strlen(query);
						// Finalmente envia para o cliente
						sendall(client_sock, query, &length);
						break;

					case 2:		// Descricao de um livro
						elapsed = 0;
						// Esperando o cliente mandar o ISBN desejado
						// Tempo percorrido ate agora
						gettimeofday(&t1, 0);
						// ReCalculo do tempo de operacao
						elapsed += (t1.tv_sec-t0.tv_sec)*1000000 + t1.tv_usec-t0.tv_usec;
						if ( (read_size = recv(client_sock , client_message , 2000 , 0)) > 0 ) {
//							gettimeofday(&t0, 0);
							// Montando a query
							strcpy(query, "select descricao from livro where ISBN10 = ");
							strcpy(query2, "select count(descricao) from livro where ISBN10 = ");
							// Concatenando o ISBN
							strcat(query, client_message);
							strcat(query2, client_message);
							// Fim do comando SQLite
							strcat(query, ";");

							// Verificando se ha livros
							// callbackSilent nao envia dados ao cliente
							// existe recebe o resultado de contagem de livros (0 ou 1)
							rc = sqlite3_exec(db, query2, callbackSilent, existe, &zErrMsg);
							// Tempo percorrido ate agora
						//	gettimeofday(&t1, 0);
							if ( *((int *)existe) == 0) {
								length = 41;
								sendall(client_sock, "\nEste ISBN nao consta na nossa livraria!\n",&length);
							}
							else
							// Executando query - Callback já faz os sends
								rc = sqlite3_exec(db, query, callback, 0, &zErrMsg);
						}
						length = 1;

						gettimeofday(&t1, 0);
						// Fim da mensagem
						// ReCalculo do tempo de operacao
						elapsed += (t1.tv_sec-t0.tv_sec)*1000000 + t1.tv_usec-t0.tv_usec;
						// Transformando em string com chars de "seguranca" para postumo atoi
						sprintf(query,"          %6li",elapsed);
						// DEBUG
						printf("\nOperation Time: %s\n\n",query);
						// Calculando o tamanho
						length = strlen(query);
						// Finalmente envia para o cliente
						sendall(client_sock, query, &length);
						break;

					case 3:		// Todas as informacoes de um livro
						elapsed = 0;
						// Esperando o cliente mandar o ISBN desejado
						// Tempo percorrido ate agora
						gettimeofday(&t1, 0);
						// ReCalculo do tempo de operacao
						elapsed += (t1.tv_sec-t0.tv_sec)*1000000 + t1.tv_usec-t0.tv_usec;
						// Esperando o cliente mandar o ISBN desejado
						if ( (read_size = recv(client_sock , client_message , 2000 , 0)) > 0 ) {
//							gettimeofday(&t0, 0);
							// Montando a query
							//strcpy(query, "select * from livro where ISBN10 = ");

							strcpy(query, "select l.ISBN10, l.titulo, a.autor, a.autor2, a.autor3, a.autor4, l.descricao, l.editora, l.ano, l.estoque from livro l, autor a where l.autores=a.a_id and ISBN10 = ");
							strcpy(query2, "select count(*) from livro where ISBN10 = ");
							// Concatenando o ISBN
							strcat(query, client_message);
							strcat(query2, client_message);
							// Fim do comando SQLite
							strcat(query, ";");

							// Verificando se ha livros
							// callbackSilent nao envia dados ao cliente
							// existe recebe o resultado de contagem de livros (0 ou 1)
							rc = sqlite3_exec(db, query2, callbackSilent, existe, &zErrMsg);
							if ( *((int *)existe) == 0) {
								length = 41;
								sendall(client_sock, "\nEste ISBN nao consta na nossa livraria!\n",&length);
								// Tempo percorrido ate agora
								gettimeofday(&t1, 0);
							}
							else {
								// Executando query - Callback já faz os sends
								rc = sqlite3_exec(db, query, callbackFmt, 0, &zErrMsg);
								// Tempo percorrido ate agora
								gettimeofday(&t1, 0);
							}
						}

						// Fim da mensagem
						// ReCalculo do tempo de operacao
						elapsed += (t1.tv_sec-t0.tv_sec)*1000000 + t1.tv_usec-t0.tv_usec;
						// Transformando em string com chars de "seguranca" para postumo atoi
						sprintf(query,"          %6li",elapsed);
						// DEBUG
						printf("\nOperation Time: %s\n\n",query);
						// Calculando o tamanho
						length = strlen(query);
						// Finalmente envia para o cliente
						sendall(client_sock, query, &length);
						break;

					case 4:		// Todas as informacoes de todos os livros
						elapsed = 0;
						// Executando query
						rc = sqlite3_exec(db, "select l.ISBN10, l.titulo, a.autor, a.autor2, a.autor3, a.autor4, l.descricao, l.editora, l.ano, l.estoque from livro l, autor a where l.autores=a.a_id;",
						callbackFmt, 0, &zErrMsg);

						// Fim da mensagem
						// Tempo percorrido ate agora
						gettimeofday(&t1, 0);
						// Calculo do tempo de operacao
						elapsed = (t1.tv_sec-t0.tv_sec)*1000000 + t1.tv_usec-t0.tv_usec;
						// Transformando em string com chars de "seguranca" para postumo atoi
						sprintf(query,"          %6li",elapsed);
						// DEBUG
						printf("\nOperation Time: %s\n\n",query);
						// Calculando o tamanho
						length = strlen(query);
						// Finalmente envia para o cliente
						sendall(client_sock, query, &length);
						break;

					case 5:		// Atualizar estoque
						elapsed = 0;
						// Tempo percorrido ate agora
						gettimeofday(&t1, 0);
						// ReCalculo do tempo de operacao
						elapsed += (t1.tv_sec-t0.tv_sec)*1000000 + t1.tv_usec-t0.tv_usec;
						// Esperando o cliente mandar o novo estoque do livro
						if ( (read_size = recv(client_sock , &cm , 2000 , 0)) > 0 ) {
//							gettimeofday(&t0, 0);
							if (superuser) {
								// Montando a query
								strcpy(query, "update livro set estoque = ");
								// Concatenando o nova quantidade do estoque
								strcat(query, cm.q);
								// Concatenando o ISBN
								strcat(query, " where ISBN10 = ");
								strcat(query, cm.i);
								// Fim do comando SQLite
								strcat(query, ";");
								printf("%s\n",query);
								// Executando query - Callback já faz os writes
								rc = sqlite3_exec(db, query, callback, 0, &zErrMsg);
							}
							else {
								length = 41;
								sendall(client_sock, "Sem permissoes para modificar estoque!\n", &length);
							}
						}

						// Fim da mensagem
						// Tempo percorrido ate agora
						gettimeofday(&t1, 0);
						// ReCalculo do tempo de operacao
						elapsed += (t1.tv_sec-t0.tv_sec)*1000000 + t1.tv_usec-t0.tv_usec;
						// Transformando em string com chars de "seguranca" para postumo atoi
						sprintf(query,"          %6li",elapsed);
						// DEBUG
						printf("\nTime: %s\n\n",query);
						// Calculando o tamanho
						length = strlen(query);
						// Finalmente envia para o cliente
						sendall(client_sock, query, &length);
						break;

					case 6:		// Mostra estoque de um livro
						elapsed = 0;
						// Tempo percorrido ate agora
						gettimeofday(&t1, 0);
						// ReCalculo do tempo de operacao
						elapsed += (t1.tv_sec-t0.tv_sec)*1000000 + t1.tv_usec-t0.tv_usec;
						// Esperando o cliente mandar o ISBN desejado
						if ( (read_size = recv(client_sock , client_message , 2000 , 0)) > 0 ) {
//							gettimeofday(&t0, 0);
							// Montando a query
							strcpy(query, "select estoque from livro where ISBN10 = ");
							// Concatenando o ISBN
							strcat(query, client_message);
							// Fim do comando SQLite
							strcat(query, ";");
							// Executando query - Callback já faz os writes
							rc = sqlite3_exec(db, query, callbackFmt, 0, &zErrMsg);
						}
						// Fim da mensagem
						// Tempo percorrido ate agora
						gettimeofday(&t1, 0);
						// ReCalculo do tempo de operacao
						elapsed += (t1.tv_sec-t0.tv_sec)*1000000 + t1.tv_usec-t0.tv_usec;
						// Transformando em string com chars de "seguranca" para postumo atoi
						sprintf(query,"          %6li",elapsed);
						// DEBUG
						printf("\nTime: %s\n\n",query);
						// Calculando o tamanho
						length = strlen(query);
						// Finalmente envia para o cliente
						sendall(client_sock, query, &length);
						break;
					case 7:		// Autentica o cliente livraria
						// Recebe a senha
						if ( (read_size = recv(client_sock , client_message , 50 , 0)) > 0 ) {
							length = 31;
							// Compara as senhas
							if( (strcmp(client_message,PASSWORD) ) == 0) {
								superuser = 1;		// Sessao de superusuario
								sendall(client_sock, "Bem-vindo, Chuck Norris!\n\n",&length);
							}
							else {
								superuser = 0;		// Usuario invalido
								sendall(client_sock, "Senha Invalida!\n\n",&length);
							}
							length = 1;
						}
						break;
				}

			}

			if(read_size == 0)
			{
				puts("Client disconnected");
				fflush(stdout);
				exit(0);
			}
		}
		close(connfd);
	}

	return 0;
}
