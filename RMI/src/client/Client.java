/* Andre Nakagaki Filliettaz --------------------------------------------------
 * client.java ----------------------------------------------------------------
 * ----------------------------------------------------------------------------
 * Description: client object that make all the comunication with the server --
 * and deals with the user of the application ---------------------------------
 * ------------------------------------------------------------------------- */

package client;

import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;
import java.util.Scanner;

import com.sun.xml.internal.ws.client.Stub;

public class Client {
	/* Constructor */
	private Client() {}
	
	static public void showOptions() {
		System.out.println("Welcome to the Library! Enter the option following the notation:");
		System.out.println("[h]: Help		- Show this message again!");
		System.out.println("[l]: List		- List all the ISBN and his respects Titles");
		System.out.println("[d]: Description- Show the description of a given ISBN");
		System.out.println("[i]: Information- Displays the infos from a given ISBN");
		System.out.println("[a]: All Infos	- Show all the infos from all the books");
		System.out.println("[p]: Password	- Authenticate the livraria account");
		System.out.println("[c]: Changing	- Change the numbers of the Stock **");
		System.out.println("[n]: On Stock	- Numbers on Stock");
		System.out.println("[q]: Quit	  	- Bye Bye!\n");

		System.out.println("---------");
		System.out.println("** Administrator Only!\n");

		System.out.printf("Make your choice: ");
	}

	/* Main */
	public static void main(String[] args) {
		Scanner scan = new Scanner(System.in);
		char op;
		
		
		/* The host can be passed by arguments */
		String isbn, host = (args.length < 1) ? null : args[0];
		try {
			/* Getting the stub to do the Remote Calls */
			Registry registry = LocateRegistry.getRegistry(host);
			rmtBookStore stub = (rmtBookStore) registry.lookup("BkStore");
		} catch (Exception e) {
			System.err.println("Client Exception: " + e.toString());
			e.printStackTrace();
			return;
		}
		
		/* Here show the options to the client */
		while(true) {
			Client.showOptions();	// Explains the options to the User
			op = (char) scan.nextByte();	// Take the option from user

			switch(op) {
				case 'h':	// A little help
					break;
				
				case 'l':	// Looking at the Store
					
					break;
				
				case 'd':	// Searching for Description
					System.out.println("Waiting for ISBN of the Book!\n");
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

				case 'a':	// All Infos
					/* Calling the fetching result function  */
					if( dataFetch(sockfd, NULL, "4") < 0)
						printf("PROBLEMS!!!!!!!\n");
					break;

				case 'c':	// Changing the stores numbers
					printf("Waiting for the new stock amount!\n");
					scanf(" %s", qtt);	// Getting Quantity
					printf("Waiting for ISBN of the Book!\n");
					scanf(" %s", isbn);	// Getting ISBN
					alterStock(sockfd, isbn, qtt);
					break;

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
		
	}
}