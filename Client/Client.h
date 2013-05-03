/* Client.c --------------------------------------------------------------------
 * Andre Nakagaki Filliettaz 	- RA104595 -------------------------------------
 * Guilherme Alcarde Gallo 	- RA105008 -------------------------------------
 * -------------------------------------------------------------------------- */

/* HEADER - Support the program with the includes, typedefs and all king of
 * declaration and preprocessor problems*/

/* Standarts input and output librarys */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* Library to deal with the networks syscalls */
#include <arpa/inet.h>
//#include <sys/types.h>
#include <sys/socket.h>

/* Defines */

#define PASSWRD "mc832"	// Password to some operations

//short int ACCESS = 0;	// Controls the mode of the Client (Livraria or Comum)

/* Functions Declarations */
void showOptins();		// Explanation Function
int listAll(int);		// List All the ISBN and Titles of the Books
int dataFetch(int, char *, char []);	// Fetch the Description or Infos 
int listAllInfos(int);		// List All the Books and respectivily infos
int changeStocks(int);		// Change the Numbers on stock (Admin ONLY)
int reportNumbers(int, char *);	// Show the numbers on stock
int autenticate(char *);	// Autenticate the change stock operation

/* Auxiliar Function */
int fetch(int, char **, int);
int check_str(char *, char); 
