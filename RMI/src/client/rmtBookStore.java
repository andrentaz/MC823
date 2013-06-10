/* Andre Nakagaki Filliettaz --------------------------------------------------
 * rmtBookStore.java ----------------------------------------------------------
 * ----------------------------------------------------------------------------
 * Description: remote interface of the bookstore that will be implemented by -
 * by the server --------------------------------------------------------------
 * ------------------------------------------------------------------------- */

package client;

import java.rmi.Remote;
import java.rmi.RemoteException;
import java.sql.SQLException;
import java.util.ArrayList;
import server.Book;

public interface rmtBookStore extends Remote {
	/* Shows the content of the Store */
	public ArrayList<String> showStore() throws RemoteException;

	/* Look for and show the description of a given ISBN */
	public ArrayList<String> fetchDescription(String isbn) throws RemoteException;

	/* Search for info about an ISBN */
	public ArrayList<String> fetchInfos(String isbn) throws RemoteException;

	/* Get all the infos from all the books of the Store */
	public ArrayList<Book> fetchAll() throws RemoteException;

	/* Get the numbers of books on store */
	public int getNumber(String isbn) throws RemoteException;
	
	/* Set the numbers of books on store -- only if you are the livraria client */
	public String setNumber(int num, String isbn) throws RemoteException, SQLException;

	/* Get access to livraria client */
	public Boolean login(String pass) throws RemoteException;
}