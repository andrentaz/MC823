/* Andre Nakagaki Filliettaz --------------------------------------------------
 * client.java ----------------------------------------------------------------
 * ----------------------------------------------------------------------------
 * Description: client application that make all the comunication with the ----
 * server and deals with the user of the application --------------------------
 * ------------------------------------------------------------------------- */

package client;

import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;
import java.util.ArrayList;
import java.util.Scanner;
import server.Book;

public class Client {
	/* Constructor */
	private Client() {}

	static public void showOptions() {
		System.out
				.println("Welcome to the Library! Enter the option following the notation:");
		System.out.println("[1]: Help		- Show this message again!");
		System.out
				.println("[2]: List		- List all the ISBN and his respects Titles");
		System.out
				.println("[3]: Description- Show the description of a given ISBN");
		System.out
				.println("[4]: Information- Displays the infos from a given ISBN");
		System.out
				.println("[5]: All Infos	- Show all the infos from all the books");
		System.out
				.println("[6]: Changing	- Change the numbers of the Stock **");
		System.out.println("[7]: On Stock	- Numbers on Stock");
		System.out.println("[8]: Quit	  	- Bye Bye!\n");

		System.out.println("---------");
		System.out.println("** Administrator Only!\n");

		System.out.printf("Make your choice: ");
	}


	/* Main */
	public static void main(String[] args) {
		int qtd, op;
		Scanner scan = new Scanner(System.in);
		
		ArrayList<String> serv_str = new ArrayList<String>();
		ArrayList<Book> serv_bk = new ArrayList<Book>();

		/* The host can be passed by arguments */
		String pwd, isbn, host = (args.length < 1) ? null : args[0];	
		
		try {
			/* Getting the stub to do the Remote Calls */
			Registry registry = LocateRegistry.getRegistry(host);
			RmtBookStore stub = (RmtBookStore) registry.lookup("RmtBookStore");

			/* Here show the options to the client */
			while (true) {
				Client.showOptions(); // Explains the options to the User
				op = scan.nextInt(); // Take the option from user

				/* DEBUG */
				System.out.println(op);
				switch (op) {
				case 1: // A little help
					break;

				case 2: // Looking at the Store
					/* Remote Calling - Returning ArrayList<String> */
					serv_str = stub.showStore();
					
					/* Prints all the books from the server */
					for (int i=0; i<serv_str.size(); i++)
						System.out.println(serv_str.get(i));

					break;

				case 3: // Searching for Description
					System.out.println("Waiting for ISBN of the Book!");
					isbn = scan.nextLine(); // Getting ISBN

					/* Calling the fetching result function */
					serv_str = stub.fetchDescription(isbn);

					/* Printing the results */
					for (int i=0; i<serv_str.size(); i++) 
						System.out.println(serv_str.get(i));

					break;

				case 4: // Searching for Info
					System.out.println("Waiting for ISBN of the Book!");
					isbn = scan.nextLine(); // Getting ISBN

					/* Calling the fetching result function */
					serv_str = stub.fetchInfos(isbn);

					/* Printing the results */
					for (int i=0; i<serv_str.size(); i++)
						System.out.println(serv_str.get(i));

					break;

				case 5: // All Infos
					/* Calling the fetching result function */
					serv_bk = stub.fetchAll();

					/* Printing the Books */
					for (int i=0; i<serv_bk.size(); i++)
						serv_bk.get(i).print();

					break;

				case 6: // Changing the stores numbers				
					System.out.println("Waiting for the new stock amount!");
					qtd = scan.nextInt(); // Getting new Stock amount
					System.out.println("Waiting for ISBN of the Book!");
					isbn = scan.nextLine(); // Getting ISBN

					/* Calling the fetching result function */
					System.out.println("Password: ");
					pwd = scan.nextLine();
					stub.setNumber(qtd, isbn, pwd);

					break;

				case 7: // Numbers on stock
					System.out.println("Waiting for ISBN of the Book!");
					isbn = scan.nextLine(); // Getting ISBN
					qtd = stub.getNumber(isbn);
					
					/* Calling the stocks numbers */
					if (qtd != -1)
						System.out.println("Quantidade em Estoque: "
							+ qtd);
					else
						System.out.println("Esse ISBN nao consta em nossos " +
							"estoques, desculpe!");

					break;

				case 8: // Quiting the program!
					System.out.println("Quiting now!");
					break;

				default: // Unknow command
					System.out.println("Bad instruction, try again!");
					break;
				} /* End Switch */

				if (op == 8)
					break;

			}
			scan.close();

		} catch (Exception e) {
			System.err.println("Client Exception: " + e.toString());
			e.printStackTrace();
			return;
		}
		
		return;
	}
}