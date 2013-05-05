/* Client.c --------------------------------------------------------------------
 * Andre Nakagaki Filliettaz 	- RA104595 -------------------------------------
 * Guilherme Alcarde Gallo 	- RA105008 -------------------------------------
 * -------------------------------------------------------------------------- */

/* HEADER - Support the program with the includes, typedefs and all king of
 * declaration and preprocessor problems */

/* Standart input and output libraries */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

/* Library to deal with the networks syscalls */
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <errno.h>
#include <netdb.h>

/* Typedefs */
typedef struct livro {
	char i[20];     // ISBN
	char q[4];      // Stock Quantity
} Livro;

/* Functions Declarations */
void showOptins();		// Explanation Function

/* Fetch the Description or Infos */
int dataFetch(int, char *, char [], struct addrinfo *); 

/* Change the Stock Number (Administrator Only)*/
void alterStock(int, char [], char [], struct addrinfo *);

/* Auxiliar Function */
int fetch(int, char **, int);
int check_str(char *, char);
void logger(char [], int, int, int);
void pass(int, char [], struct addrinfo *);
