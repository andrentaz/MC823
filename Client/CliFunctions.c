/* Client.c --------------------------------------------------------------------
 * Andre Nakagaki Filliettaz 	- RA104595 -------------------------------------
 * Guilherme Alcarde Gallo 	- RA105008 -------------------------------------
 * -------------------------------------------------------------------------- */

/* Implementation of all the functions used on Client.c  */

/* Include all the stuff need to execute the program */
#include "Client.h"

int check_str(char str[], char alpha) {
	int it=0, count=0;

	/* Looping on the string */
	for(it=0; it < strlen(str); it++) {
		if(str[it] == alpha)	count++;

	}

	/* char didn't find */
	return count;
}

void logger(char option[], int time, int countR, int tOp) {
	FILE *logfile;
	char text[50], name[10];
	
	sprintf(name,"LOG%s", option);
	// Gravando opcao e tempo percorrido em foramto CSV
	sprintf(text,"%s,%d,%d,%d\n", option, time, countR, tOp);
	logfile = fopen(name,"a");
	fputs(text,logfile);
	fclose(logfile);
}

/* -------------------------------------------------------------------------- */

void showOptions() {
	printf("Welcome to the Library! Enter the option following the notation:\n");
	printf("[h]: Help 	  - Show this message again!\n");
	printf("[l]: List  	  - List all the ISBN and his respects Titles\n");
	printf("[d]: Description  - Show the description of a given ISBN\n");
	printf("[i]: Information  - Displays the infos from a given ISBN\n");
	printf("[a]: All Infos	  - Show all the infos from all the books\n");
	printf("[p]: Password	  - Authenticate the livraria account\n");
	printf("[c]: Changing	  - Change the numbers of the Stock **\n");
	printf("[n]: On Stock	  - Numbers on Stock\n");
	printf("[q]: Quit	  - Bye Bye!\n\n");

	printf("---------\n");
	printf("** Administrator Only!\n\n");

	printf("Make your choice: ");
}

/* -------------------------------------------------------------------------- */

void alterStock(int sockfd, char isbn[], char qtd[]) {
	char asw[5000];	// Response from server
	char *time;	// Operation Time from the Server
	int read_bytes,sig=0;
	typedef struct livro {
		char i[20];	// ISBN
		char q[4];	// Stock Quantity
	} Livro;

	Livro tt;
	strcpy(tt.i,isbn);
	strcpy(tt.q,qtd);

	long elapsed = 0; 	// Guarda intervalo de tempo
	int nRevcs = 0;		// Guarda numero de receives
	struct timeval t0, t1;	// Guarda tempo percorrido

	// Sending request for password to server
	if ( send(sockfd, "5", 2, 0) < 0) {
		printf("SEND FAILURE!\n");	// DEBUG 
		return;
	}

	// Sending the password string to server
	if ( send(sockfd, &tt, 30, 0) < 0) {
		printf("SEND FAILURE!\n");	// DEBUG 
		return;
	}

	// Receiving the answer of server authentication
	while(1) {

		/* Cleans the string */
		memset(asw,0,5000);

		gettimeofday(&t0, 0);		// Capturando tempo de inicio
		/* Read a maximum of 500 bytes from buffer */
		if ( read_bytes = recv(sockfd, asw, 5000, 0) < 0 ) {
			printf("Erro no receive!!\n\n");
			return;
		} else {
			gettimeofday(&t1, 0);	// Capturando tempo de termino
			nRevcs++;		// Atualizando contagem de receive
			// Calculando intervalo de tempo em microsegundos
			elapsed += (t1.tv_sec-t0.tv_sec)*1000000 + t1.tv_usec-t0.tv_usec;
			 /* Tests if received string contains the char
			  * '^D', which means TRANSMISSION OVER */
			 sig=check_str(asw, '');
			 
			 /* Testing here what is what... */
			 if (sig > 0) {
				 /* End Reading */
				 printf("%s", asw);
				 break;
			 } else  /* Continue Reading! */
				 printf("%s", asw);
			 
		}
	}

	printf("%s",asw);
	time = asw+strlen(asw)-8;
	printf("\n");
	printf("Tempo gasto: %lius || %li || %li", elapsed - (long)atoi(time), elapsed, (long)atoi(time));
	printf("\n");
	logger("5",elapsed - atoi(time),nRevcs,atoi(time));
	elapsed = 0;
}

void pass(int sockfd, char pwd[]) {
	char asw[5000];	// Response from server

	// Sending request for password to server
	if ( send(sockfd, "7", 2, 0) < 0) {
		printf("SEND FAILURE!\n");	// DEBUG 
		return;
	}

	// Sending the password string to server
	if ( send(sockfd, pwd, 50, 0) < 0) {
		printf("SEND FAILURE!\n");	// DEBUG 
		return;
	}

	// Receiving the answer of server authentication
	if ( recv(sockfd, asw, 200, 0) < 0 ) {
		printf("[1] RECEIVE FAILURE\nasw: %s", asw );	// DEBUG
		return;
	}

	printf("%s",asw);
}


/* -------------------------------------------------------------------------- */

int dataFetch(int sockfd, char *ISBN, char op[]) {
	char asw[5000];	// Answer from the Server
	char *time;	// Operation Time from the Server
	int read_bytes, sig=0, errc=0;

	long elapsed = 0; 	// Guarda intervalo de tempo
	int nRevcs = 0;		// Guarda numero de receives
	struct timeval t0, t1;	// Guarda tempo percorrido

	/* Formating output */
	printf("\n");
	
	/* Sends the Request to the Server and check errors*/
	if ( send(sockfd, op, 2, 0) < 0 ) {
		printf("Sending Error! Aborting!\n");
		return -1;
	}

	if ( op[0] != '1' && op[0] != '4' ) {
		/* Send the ISBN required to the operation in case
		 * of operations 2 and 3 */
		if ( send(sockfd, ISBN, strlen(ISBN), 0) < 0 ) {
			printf("Sending Error! Aborting!\n");
			return -1;
		}

	}


	/* Reading L00P! Read from the buffer as long there is data at the buffer,
	 * if receives the signal to stop (char '^D') then stop reading */
	elapsed = 0;
	while(1) {

		/* Cleans the string */
		memset(asw,0,5000);

		gettimeofday(&t0, 0);		// Capturando tempo de inicio
		/* Read a maximum of 500 bytes from buffer */
		if ( read_bytes = recv(sockfd, asw, 5000, 0) < 0 ) 
			 return -1;
		else {
			gettimeofday(&t1, 0);	// Capturando tempo de termino
			nRevcs++;		// Atualizando contagem de receive
			// Calculando intervalo de tempo em microsegundos
			elapsed += (t1.tv_sec-t0.tv_sec)*1000000 + t1.tv_usec-t0.tv_usec;
			 /* Tests if received string contains the char
			  * '^D', which means TRANSMISSION OVER */
			 sig=check_str(asw, '');
			 
			 /* Testing here what is what... */
			 if (sig > 0) {
				 /* End Reading */
				 printf("%s", asw);
				 break;
			 } else  /* Continue Reading! */
				 printf("%s", asw);
			 
		}
	}

	/* Formating the output! */
	time = asw+strlen(asw)-8;
	printf("\n");
	printf("Tempo gasto: %lius || %li || %li -- %d", elapsed - (long)atoi(time), elapsed, (long)atoi(time), nRevcs);
	printf("\n");
	printf("%s",time);
	printf("\n");
	// Guardando num log CSV
	logger(op,elapsed-atoi(time),nRevcs,atoi(time));
	elapsed = 0;
	return 0;
	
}

/* -------------------------------------------------------------------------- */
