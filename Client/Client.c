/* Client.c -------------------------------------------------------------------
 * Andre Nakagaki Filliettaz 	- RA104595 ------------------------------------
 * Guilerme Alcarde Gallo 	- RA105008 ------------------------------------
 ----------------------------------------------------------------------------*/

/* This programs deals with the interface with the humans and requests to
 * to the server. Uses the standarts TCP sockets and SQLite3 librarys */

/* Include all the stuff need to execute the program */
#include "Client.h"

/* Main function */
int main (int argc, char *argv[]) {
	/* Control Variables */
	char op, isbn[11], pwd[50], qtt[4];

	/* With the connection done, read to send requests to the server */
	int sockfd;
	struct sockaddr_in server;
	char message[1000] , server_reply[2000];
	struct timeval tv;

	tv.tv_sec = 5;  /* 30 Secs Timeout */
	tv.tv_usec = 0;  // Not init'ing this can cause strange errors

	sockfd = socket(AF_INET , SOCK_STREAM , 0);

	//Create socket
	if (sockfd == -1)
	{
		printf("Could not create socket");
	}
	puts("Socket created");
	setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv,sizeof(struct timeval));


	server.sin_addr.s_addr = inet_addr("127.0.0.1");
//	server.sin_addr.s_addr = inet_addr("187.106.45.71");
//	server.sin_addr.s_addr = inet_addr("143.106.174.65");
	server.sin_family = AF_INET;
	server.sin_port = htons( 8888 );

	//Connect to remote server
	if (connect(sockfd , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		perror("connect failed. Error");
		return 1;
	}
	while(1) {
		showOptions();	// Explains the options to the User

		scanf(" %c", &op);	// Take the option from user
		//printf("\n");
//		printf("\n--------\n%c\n--------\n",op);
		switch(op) {
			case 'h':	// A little help
				break;
			case 'l':	// Looking at the Store
				if( dataFetch(sockfd, NULL, "1") < 0)
					printf("PROBLEMS!!!!!!!\n");
				break;
			case 'd':	// Searching for Description
				printf("Waiting for ISBN of the Book!\n");
				scanf(" %s", isbn);	// Getting ISBN

				/* Calling the fetching result function */
				dataFetch(sockfd, isbn, "2");
				break;
			case 'i':	// Searching for Info
				printf("Waiting for ISBN of the Book!\n");
				scanf(" %s", isbn);	// Getting ISBN

				/* Calling the fetching result function  */
				printf("%d",dataFetch(sockfd, isbn, "3"));
				break;
//			case 'a':	// All Infos
//				/* Calling the fetching result function  */
//				listAllInfos(sockfd);
//				break;
			case 'a':	// All Infos
				/* Calling the fetching result function  */
				if( dataFetch(sockfd, NULL, "4") < 0)
					printf("PROBLEMS!!!!!!!\n");
				break;
			case 'c':	// Changing the stores numbers
//				printf("Enter the Library Password:");
//				scanf("%s", pwd);
//				if (!autenticate(pwd)) {
//					printf("Wrong password! \
//						Access Denied!\n");

				printf("Waiting for the new stock amount!\n");
				scanf(" %s", qtt);	// Getting Quantity
				printf("Waiting for ISBN of the Book!\n");
				scanf(" %s", isbn);	// Getting ISBN
				alterStock(sockfd, isbn, qtt);
				break;
//				}

//				changeStocks(sockfd);
//				break;
//			case 'n':	// Numbers on stock
//				printf("Waiting for ISBN of the Book!\n");
//				scanf("%s", isbn);	// Getting ISBN

				/* Calling the stocks numbers */
//				reportNumbers(sockfd, isbn);
			case 'n':	// Numbers on stock
				printf("Waiting for ISBN of the Book!\n");
				scanf("%s", isbn);	// Getting ISBN

				/* Calling the stocks numbers */
				dataFetch(sockfd, isbn, "6");
				break;
			case 'p':
				printf("Digite a senha para cliente livraria...\n");
				scanf(" %s", pwd);
				pass(sockfd, pwd);
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
