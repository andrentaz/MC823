/* Andre Nakagaki Filliettaz --------------------------------------------------
 * BookStore.java -------------------------------------------------------------
 * ----------------------------------------------------------------------------
 * Description: class that implements the remote interface and will be used by
 * the server to send all the stuffs ------------------------------------------
 * ------------------------------------------------------------------------- */

package server;

import java.rmi.RemoteException;
import java.util.ArrayList;
import client.rmtBookStore;
import java.sql.*;

public class BookStore implements rmtBookStore {

	/* Constructor */
	public BookStore() {}


	/* Methods */
	/* Auxiliar Methods */
	public ResultSet fetchData(String query) 
			throws SQLException { return null; }


	@Override
	public ArrayList<String> showStore() throws RemoteException {
		ArrayList<String> store = new ArrayList<String>();		/* Return */
		String aux, query = "SELECT ISBN10, titulo FROM livro";	/* MySQL */
		
		try {
			ResultSet res = fetchData(query);
			
			/* Handling the Data */
			while (res.next()) {
				/* Each line of 'store' it's one book, aux is the tmp
				 * variable that holds each line of the result set */
				aux = Integer.toString(res.getInt("ISBN10"));
				aux.concat(" | ");
				aux.concat(res.getString("titulo"));
				
				/* DEBUG */
				System.out.println(aux);
				
				store.add(aux);
			}
		} catch (Exception e) {
			System.out.println(e.getMessage());
		} 
		
		/* Returning the Array List */
		return store;
	}


	@Override
	public ArrayList<String> fetchDescription(String isbn)
			throws RemoteException {
		ArrayList<String> book = new ArrayList<String>();
		String aux, query = "SELECT titulo, descricao FROM livro " +
				"WHERE ISBN10 = ";	/* MySQL */
		
		query.concat(isbn);
		
		/* Working on it */
		try {
			
		}
		
		return null;
	}


	@Override
	public ArrayList<String> fetchInfos(String isbn) throws RemoteException {
		// TODO Auto-generated method stub
		return null;
	}


	@Override
	public ArrayList<String> fetchAll() throws RemoteException {
		// TODO Auto-generated method stub
		return null;
	}


	@Override
	public int getNumber(String isbn) throws RemoteException {
		// TODO Auto-generated method stub
		return 0;
	}


	/* Remotes Methods */
	
}
