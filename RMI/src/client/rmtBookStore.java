/* Andre Nakagaki Filliettaz --------------------------------------------------
 * rmtBookStore.java ----------------------------------------------------------
 * ----------------------------------------------------------------------------
 * Description: remote interface of the bookstore that will be implemented by -
 * by the server --------------------------------------------------------------
 * ------------------------------------------------------------------------- */

package client;

import java.rmi.Remote;
import java.rmi.RemoteException;

public interface rmtBookStore extends Remote {
	/* Shows the content of the Store */
	public void showStore() throws RemoteException;

	/* Look for and show the description of a given ISBN */
	public void fetchDescription(String isbn) throws RemoteException;

	/* Search for info about an ISBN */
	public void fetchInfos(String isbn) throws RemoteException;

	/* Get all the infos from all the books of the Store */
	public void fetchAll() throws RemoteException;

	/* Get the numbers of books on store */
	public void getNumber(String isbn) throws RemoteException;

}