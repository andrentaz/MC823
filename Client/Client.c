/* Client.c - UDP Socket ------------------------------------------------------
 * Andre Nakagaki Filliettaz 	- RA104595 --------------------------------------
 * Guilerme Alcarde Gallo 	- RA105008 -----------------------------------------
 ----------------------------------------------------------------------------*/

/* This programs deals with the interface with the humans and requests to
 * to the server. Uses the standarts UDP sockets and SQLite3 librarys */

/* Include all the stuff need to execute the program */
#include "Client.h"
#define MYPORT "4950"
#define SERVIP "143.106.16.244"		// Ribeiro - IC301

/* Main function */
int main (int argc, char *argv[]) {
	/* Control Variables */
	char op, isbn[11], pwd[50], qtt[4], sIP[20];

	/* With the connection done, read to send requests to the server */
	int sockfd;
	struct addrinfo hints, *servinfo, *tmp;
	char message[1000] , server_reply[2000];
	struct timeval tv;

	tv.tv_sec = 5;  /* 30 Secs Timeout */
	tv.tv_usec = 0;  // Not init'ing this can cause strange errors


	/* Defining the IP */
	if (argv[1] == NULL)	strcpy(sIP, SERVIP);		/* Default */
	else if (argv[1]=="0")	strcpy(sIP, "127.0.0.1");	/* Host */
	else 			strcpy(sIP, argv[1]);		/* Passed */

	/* Setting hints to get the list of addrinfo struct */
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;	/* Any kind of IP */
	hints.ai_socktype = SOCK_DGRAM;	/* UDP */


	/* Create the list of structs */
	if (getaddrinfo (sIP,MYPORT, &hints, &servinfo) != 0) {
		printf("Erro na alocacao de Enderecos!\n");
		return -1;
	}

	/* Take the first of the addrinfo inside the list */
	for (tmp = servinfo; tmp != NULL; tmp = tmp->ai_next) {
		/* Get the Socket Descriptor */
		if ((sockfd = socket(tmp->ai_family, tmp->ai_socktype, tmp->ai_protocol)) == -1 ) {
			printf("Error on socket creation! Trying next address struct!\n");
			continue;
		}
		break;
	}

	if (tmp == NULL) {
		printf("Falha ao criar sockets! Abortando!\n");
		return -2;
	}


	/* Start the loop of requests to the server! */
	while(1) {
		showOptions();	// Explains the options to the User

		scanf(" %c", &op);	// Take the option from user


		switch(op) {
			case 'h':	// A little help
				break;


			case 'l':	// Looking at the Store
				if( dataFetch(sockfd, NULL, "1", tmp) < 0)
					printf("PROBLEMS!!!!!!!\n");
				break;


			case 'd':	// Searching for Description
				printf("Waiting for ISBN of the Book!\n");
				scanf(" %s", isbn);	// Getting ISBN

				/* Calling the fetching result function */
				dataFetch(sockfd, isbn, "2", tmp);
				break;


			case 'i':	// Searching for Info
				printf("Waiting for ISBN of the Book!\n");
				scanf(" %s", isbn);	// Getting ISBN

				/* Calling the fetching result function  */
				printf("%d",dataFetch(sockfd, isbn, "3", tmp));
				break;


			case 'a':	// All Infos
					/* Calling the fetching result function  */
				if( dataFetch(sockfd, NULL, "4", tmp) < 0)
					printf("PROBLEMS!!!!!!!\n");
				break;


			case 'c':	// Changing the stores numbers
				printf("Waiting for the new stock amount!\n");
				scanf(" %s", qtt);	// Getting Quantity
				printf("Waiting for ISBN of the Book!\n");
				scanf(" %s", isbn);	// Getting ISBN
				alterStock(sockfd, isbn, qtt, tmp);
				break;


			case 'n':	// Numbers on stock
				printf("Waiting for ISBN of the Book!\n");
				scanf("%s", isbn);	// Getting ISBN

				/* Calling the stocks numbers */
				dataFetch(sockfd, isbn, "6", tmp);
				break;


			case 'p':
				printf("Digite a senha para cliente livraria...\n");
				scanf(" %s", pwd);
				pass(sockfd, pwd, tmp);
				break;


			case 'q':	// Quiting the program!
				printf("Quiting now!\n");
				break;


			default:	// Unknow command
				printf("Bad instruction, try again!\n");
				break;
		} /* End Switch */

		if (op == 'q')
			break;

	}
	
	close(sockfd);
	return 0;	// Terminating program

}
