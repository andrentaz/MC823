/*
 ** listener.c -- a datagram sockets "server" demo
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define MYPORT "4950"	// the port users will be connecting to
#define MAXBUFLEN 100

// SQLite3
#include <sqlite3.h>

#define PASSWORD "numaPistacheCottapie"
int sockfd;
struct sockaddr_storage their_addr;
socklen_t addr_len;
int numbytes;

typedef struct livro {
	char i[20];	// ISBN
	char q[4];	// Stock Quantity
} Livro;

int socket_desc, client_sock, c, read_size, connfd;

// Tempo
long elapsed = 0;			// Conta o tempo percorrido
struct timeval t0, t1;

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*) sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*) sa)->sin6_addr);
}

// Callback do SQLite3. Versao silenciosa, nao envia nada para o cliente
static int callbackSilent(void *NotUsed, int argc, char **ans,
		char **azColName) {
	int i, num;
	int *v;
	v = (int *) alloca(sizeof(int));
	// Aaaaaaah Moleque!!!!!
	v = NotUsed;
	*v = atoi(ans[0]);
	//	printf("\n--------\n%d\n--------\n",*v);
	//sendall(client_sock, aux, &num);
	return 0;
}

// Callback do SQLite3. Versao formatada para varios detalhes
static int callbackFmt(void *NotUsed, int argc, char **ans, char **azColName) {
	int i, num;
	char aux[20000];

	// Nome da coluna
	strcpy(aux, azColName[0]);
	strcat(aux, ": ");
	// Concatena valor
	strcat(aux, ans[0]);
	strcat(aux, "\n");
	// Formata
	for (i = 1; i < argc; i++) {		// Fazendo isso para o resto da tupla
		if (ans[i]) {
			strcat(aux, azColName[i]);
			strcat(aux, ": ");
			// Melhorando a visualizacao da descricao
			if (i == 3) {
				strcat(aux, "\n\t");
			}
			strcat(aux, ans[i]);
			strcat(aux, "\n");
		}
	}
	strcat(aux, "\n");
	// DEBUG
	printf("%s", aux);
	// Enviando para o cliente
	num = strlen(aux);
	//	sendall(client_sock, aux, &num);
	if ((numbytes = sendto(sockfd, aux, strlen(aux), 0,
					(struct sockaddr *) &their_addr, addr_len)) == -1) {
		perror("server: sendto");
		exit(1);
	}
	return 0;
}

// Callback formatado em tupla simples
static int callback(void *NotUsed, int argc, char **ans, char **azColName) {
	int i, num;
	char aux[20000];

	strcpy(aux, ans[0]);
	for (i = 0; i < argc; i++) {
		if (i && ans[i]) {
			strcat(aux, " | ");
			strcat(aux, ans[i]);
		}
	}
	strcat(aux, "\n");
	printf("Tamanho= %d\n%s", (int)strlen(aux), aux);
	num = strlen(aux);
	//sendall(client_sock, aux, &num);
	if ((numbytes = sendto(sockfd, aux, strlen(aux), 0,
					(struct sockaddr *) &their_addr, addr_len)) == -1) {
		perror("server: sendto");
		exit(1);
	}
	return 0;
}

int main(void) {
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char buf[MAXBUFLEN];
	char s[INET6_ADDRSTRLEN];


	char client_message[2000], query[2500], query2[2500];

	int opcao, length = 1;

	// SQLite3
	sqlite3 *db;
	char *zErrMsg = 0, *msg;
	int rc;
	void *existe; 				// Para requisicoes invalidas
	int superuser = 0; 			// Cliente Livraria
	existe = (void *) alloca(sizeof(int));	// Usado para saber se o ISBN existe
	Livro cm;

	// Timeout setado para imprevistos ...
	struct timeval tv;

	tv.tv_sec = 5; /* 30 Secs Timeout */
	tv.tv_usec = 0;  // Not init'ing this can cause strange errors

	setsockopt(connfd, SOL_SOCKET, SO_RCVTIMEO, (char *) &tv,
			sizeof(struct timeval));
	// ... Timeout setado.

	// Abrindo Banco de Dados do SQLite3
	rc = sqlite3_open("livraria2.db", &db);

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	// Servidor interativo
	while (1) {
		if ((rv = getaddrinfo(NULL, MYPORT, &hints, &servinfo)) != 0) {
			fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
			return 1;
		}

		// loop through all the results and bind to the first we can
		for (p = servinfo; p != NULL; p = p->ai_next) {
			if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol))
					== -1) {
				perror("listener: socket");
				continue;
			}

			if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
				close(sockfd);
				perror("listener: bind");
				continue;
			}

			break;
		}

		if (p == NULL) {
			fprintf(stderr, "listener: failed to bind socket\n");
			return 2;
		}

		freeaddrinfo(servinfo);

		printf("listener: waiting to recvfrom...\n");

		addr_len = sizeof their_addr;
		if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN - 1, 0,
						(struct sockaddr *) &their_addr, &addr_len)) == -1) {
			perror("recvfrom");
			exit(1);
		}

		printf("listener: got packet from %s\n",
				inet_ntop(their_addr.ss_family,
					get_in_addr((struct sockaddr *) &their_addr), s,
					sizeof s));

		printf("listener: packet is %d bytes long\n", numbytes);
		buf[numbytes] = '\0';
		printf("listener: packet contains \"%s\"\n", buf);

		/* Teste do envio
		   if ((numbytes = sendto(sockfd, "Chegou?", 8, 0, 			(struct sockaddr *) &their_addr, addr_len)) == -1) {
		   perror("server: sendto");
		   exit(1);
		   }
		   */

		opcao = atoi(buf);

		switch (opcao) {
			case 1:		// Lista de ISBN e titulo dos livros
				elapsed = 0;
				// Enviando tuplas de ISBN e titulo de todos os livros
				rc = sqlite3_exec(db, "select ISBN10,titulo from livro;", callback,
						0, &zErrMsg);
				// Tempo percorrido ate agora
				gettimeofday(&t1, 0);
				if (rc != SQLITE_OK) {
					sqlite3_free(zErrMsg);
				}

				// Finalizando a mensagem
				// Calculo do tempo de operacao
				elapsed = (t1.tv_sec - t0.tv_sec) * 1000000 + t1.tv_usec
					- t0.tv_usec;
				// Transformando em string com caracteres de "seguranca" para postumo atoi
				sprintf(query, "          %6li", elapsed);	// Caractere  e um identificador de fim da mensagem
				// DEBUG
				printf("\nOperation Time: %s\n\n", query);
				// Calculando o tamanho
				length = strlen(query);
				// Finalmente envia para o cliente
				//					sendall(client_sock, query, &length);
				if ((numbytes = sendto(sockfd, query, strlen(query), 0,	(struct sockaddr *) &their_addr, addr_len)) == -1) {
					perror("server: sendto");
					exit(1);
				}
				break;

				/***********************************/

			case 2:		// Descricao de um livro
				elapsed = 0;
				// Esperando o cliente mandar o ISBN desejado
				// Tempo percorrido ate agora
				gettimeofday(&t1, 0);
				// ReCalculo do tempo de operacao
				elapsed += (t1.tv_sec - t0.tv_sec) * 1000000 + t1.tv_usec
					- t0.tv_usec;

				printf("\nISBN1:%s\n", client_message);
				if ((numbytes = recvfrom(sockfd, client_message, MAXBUFLEN - 1, 0,
								(struct sockaddr *) &their_addr, &addr_len)) == -1) {
					perror("recvfrom");
					exit(1);
				}
				//							gettimeofday(&t0, 0);
				// Montando a query
				strcpy(query, "select descricao from livro where ISBN10 = ");
				strcpy(query2,
						"select count(descricao) from livro where ISBN10 = ");
				printf("\nISBN2:%s\n", client_message);
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
				if (*((int *) existe) == 0) {
					length = 41;
					//				sendall(client_sock, "\nEste ISBN nao consta na nossa livraria!\n",&length);
					if ((numbytes = sendto(sockfd,
									"\nEste ISBN nao consta na nossa livraria!\n", 42,
									0, (struct sockaddr *) &their_addr, addr_len))
							== -1) {
						perror("server: sendto");
						exit(1);
					}
				} else
					// Executando query - Callback já faz os sends
					rc = sqlite3_exec(db, query, callback, 0, &zErrMsg);
				length = 1;

				gettimeofday(&t1, 0);
				// Fim da mensagem
				// ReCalculo do tempo de operacao
				elapsed += (t1.tv_sec - t0.tv_sec) * 1000000 + t1.tv_usec
					- t0.tv_usec;
				// Transformando em string com chars de "seguranca" para postumo atoi
				sprintf(query, "          %6li", elapsed);
				// DEBUG
				printf("\nOperation Time: %s\n\n", query);
				// Calculando o tamanho
				length = strlen(query);
				// Finalmente envia para o cliente
				//sendall(client_sock, query, &length);
				if ((numbytes = sendto(sockfd, query, strlen(query), 0,
								(struct sockaddr *) &their_addr, addr_len)) == -1) {
					perror("server: sendto");
					exit(1);
				}
				break;

				/***********************************************/


			case 3:		// Todas as informacoes de um livro
				elapsed = 0;
				// Esperando o cliente mandar o ISBN desejado
				// Tempo percorrido ate agora
				gettimeofday(&t1, 0);
				// ReCalculo do tempo de operacao
				elapsed += (t1.tv_sec - t0.tv_sec) * 1000000 + t1.tv_usec
					- t0.tv_usec;
				// Esperando o cliente mandar o ISBN desejado
				if ((read_size = recvfrom(sockfd, client_message, 2000, 0,
								(struct sockaddr *) &their_addr, &addr_len)) == -1) {
				}
				//							gettimeofday(&t0, 0);
				// Montando a query
				//strcpy(query, "select * from livro where ISBN10 = ");

				strcpy(query,
						"select l.ISBN10, l.titulo, a.autor, a.autor2, a.autor3, a.autor4, l.descricao, l.editora, l.ano, l.estoque from livro l, autor a where l.autores=a.a_id and ISBN10 = ");
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
				if (*((int *) existe) == 0) {
					length = 41;
					//		sendall(client_sock, "\nEste ISBN nao consta na nossa livraria!\n",&length);
					if ((numbytes = sendto(sockfd,
									"\nEste ISBN nao consta na nossa livraria!\n", 42,
									0, (struct sockaddr *) &their_addr, addr_len))
							== -1) {
						perror("server: sendto");
						exit(1);
					}
					// Tempo percorrido ate agora
					gettimeofday(&t1, 0);
				} else {
					// Executando query - Callback já faz os sends
					rc = sqlite3_exec(db, query, callbackFmt, 0, &zErrMsg);
					// Tempo percorrido ate agora
					gettimeofday(&t1, 0);
				}

				// Fim da mensagem
				// ReCalculo do tempo de operacao
				elapsed += (t1.tv_sec - t0.tv_sec) * 1000000 + t1.tv_usec
					- t0.tv_usec;
				// Transformando em string com chars de "seguranca" para postumo atoi
				sprintf(query, "          %6li", elapsed);
				// DEBUG
				printf("\nOperation Time: %s\n\n", query);
				// Calculando o tamanho
				length = strlen(query);
				// Finalmente envia para o cliente
				//sendall(client_sock, query, &length);
				if ((numbytes = sendto(sockfd, query, strlen(query), 0,
								(struct sockaddr *) &their_addr, addr_len)) == -1) {
					perror("server: sendto");
					exit(1);
				}
				break;

			case 4:		// Todas as informacoes de todos os livros
				elapsed = 0;
				// Executando query
				rc =
					sqlite3_exec(db,
							"select l.ISBN10, l.titulo, a.autor, a.autor2, a.autor3, a.autor4, l.descricao, l.editora, l.ano, l.estoque from livro l, autor a where l.autores=a.a_id;",
							callbackFmt, 0, &zErrMsg);

				// Fim da mensagem
				// Tempo percorrido ate agora
				gettimeofday(&t1, 0);
				// Calculo do tempo de operacao
				elapsed = (t1.tv_sec - t0.tv_sec) * 1000000 + t1.tv_usec
					- t0.tv_usec;
				// Transformando em string com chars de "seguranca" para postumo atoi
				sprintf(query, "          %6li", elapsed);
				// DEBUG
				printf("\nOperation Time: %s\n\n", query);
				// Calculando o tamanho
				length = strlen(query);
				// Finalmente envia para o cliente
				//		sendall(client_sock, query, &length);
				if ((numbytes = sendto(sockfd, query, strlen(query), 0,
								(struct sockaddr *) &their_addr, addr_len)) == -1) {
					perror("server: sendto");
					exit(1);
				}

				break;

				/*******************************************/

			case 5:		// Atualizar estoque
				elapsed = 0;
				// Tempo percorrido ate agora
				gettimeofday(&t1, 0);
				// ReCalculo do tempo de operacao
				elapsed += (t1.tv_sec - t0.tv_sec) * 1000000 + t1.tv_usec
					- t0.tv_usec;
				if ((read_size = recvfrom(sockfd, &cm, 2000, 0,
								(struct sockaddr *) &their_addr, &addr_len)) == -1) {
					perror("server: sendto");
					exit(1);
				}
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
					printf("%s\n", query);
					// Executando query - Callback já faz os writes
					rc = sqlite3_exec(db, query, callback, 0, &zErrMsg);
				} else {
					length = 41;
					//				sendall(client_sock, "Sem permissoes para modificar estoque!\n", &length);
					if ((numbytes = sendto(sockfd, "Sem permissoes para modificar estoque!\n", 42, 0,
									(struct sockaddr *) &their_addr, addr_len)) == -1) {
						perror("server: sendto");
						exit(1);
					}

				}

				// Fim da mensagem
				// Tempo percorrido ate agora
				gettimeofday(&t1, 0);
				// ReCalculo do tempo de operacao
				elapsed += (t1.tv_sec - t0.tv_sec) * 1000000 + t1.tv_usec
					- t0.tv_usec;
				// Transformando em string com chars de "seguranca" para postumo atoi
				sprintf(query, "          %6li", elapsed);
				// DEBUG
				printf("\nTime: %s\n\n", query);
				// Calculando o tamanho
				length = strlen(query);
				// Finalmente envia para o cliente
				//		sendall(client_sock, query, &length);
				if ((numbytes = sendto(sockfd, query, strlen(query), 0,
								(struct sockaddr *) &their_addr, addr_len)) == -1) {
					perror("server: sendto");
					exit(1);
				}

				break;

			case 6:		// Mostra estoque de um livro
				elapsed = 0;
				// Tempo percorrido ate agora
				gettimeofday(&t1, 0);
				// ReCalculo do tempo de operacao
				elapsed += (t1.tv_sec-t0.tv_sec)*1000000 + t1.tv_usec-t0.tv_usec;
				// Esperando o cliente mandar o ISBN desejado
				if ((numbytes = recvfrom(sockfd, client_message, MAXBUFLEN - 1, 0,
								(struct sockaddr *) &their_addr, &addr_len)) == -1) {
					perror("recvfrom");
					exit(1);
				}
				//							gettimeofday(&t0, 0);
				// Montando a query
				strcpy(query, "select estoque from livro where ISBN10 = ");
				// Concatenando o ISBN
				strcat(query, client_message);
				// Fim do comando SQLite
				strcat(query, ";");
				printf("\n%s\n", query);
				// Executando query - Callback já faz os writes
				rc = sqlite3_exec(db, query, callbackFmt, 0, &zErrMsg);
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
				if ((numbytes = sendto(sockfd, query, strlen(query), 0,
								(struct sockaddr *) &their_addr, addr_len)) == -1) {
					perror("server: sendto");
					exit(1);
				}
				break;
			case 7:		// Autentica o cliente livraria
				// Recebe a senha
				if ((numbytes = recvfrom(sockfd, client_message, 50, 0,
								(struct sockaddr *) &their_addr, &addr_len)) == -1) {
					perror("recvfrom");
					exit(1);
				}
					length = 31;
					// Compara as senhas
					if( (strcmp(client_message,PASSWORD) ) == 0) {
						superuser = 1;		// Sessao de superusuario
						//		 sendall(client_sock, "Bem-vindo, Chuck Norris!\n\n",&length);
						if ((numbytes = sendto(sockfd, "Bem-vindo, Chuck Norris!\n\n", 31, 0,
										(struct sockaddr *) &their_addr, addr_len)) == -1) {
							perror("server: sendto");
							exit(1);
						}
					}
					else {
						superuser = 0;		// Usuario invalido
						//		 sendall(client_sock, "Senha Invalida!\n\n",&length);
						if ((numbytes = sendto(sockfd, "Senha Invalida!\n\n", 18, 0,
										(struct sockaddr *) &their_addr, addr_len)) == -1) {
							perror("server: sendto");
							exit(1);
						}
					}
					length = 1;
					memset(client_message,0,strlen(client_message));
				break;

		}
		close(sockfd);
	}
	return 0;
}
