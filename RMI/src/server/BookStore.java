/* Andre Nakagaki Filliettaz --------------------------------------------------
 * BookStore.java -------------------------------------------------------------
 * ----------------------------------------------------------------------------
 * Description: class that implements the remote interface and will be used by
 * the server to send all the stuffs ------------------------------------------
 * ------------------------------------------------------------------------- */

package server;

import java.rmi.RemoteException;
import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;
import java.rmi.server.UnicastRemoteObject;
import java.util.ArrayList;
import client.rmtBookStore;
import java.sql.*;

public class BookStore implements rmtBookStore {
	private Statement statement;

	/* Constructor */
	public BookStore() throws ClassNotFoundException {
		/* Fazendo conexão com o banco de dados em SQLite3 a partir do conector
		 * JDBC.
		 * Todo o banco de dados estará acessível via atributo privado statement
		 */
		Class.forName("org.sqlite.JDBC");

		Connection connection = null;
		try {
			// create a database connection
			connection = DriverManager.getConnection("jdbc:sqlite:livraria.db");
			statement = connection.createStatement();
			statement.setQueryTimeout(30); // set timeout to 30 sec.

		} catch (SQLException e) {
			// if the error message is "out of memory",
			// it probably means no database file is found
			System.err.println(e.getMessage());
		} 
	}

	/* Methods */
	/* Auxiliar Methods */
	public ResultSet fetchData(String query) throws SQLException {

			//ResultSet rs = statement.executeQuery("select * from livro");
			return(statement.executeQuery("select * from livro"));
		/*	ResultSet res = rs;
			while (rs.next()) {
				// read the result set
				System.out.println("name = " + rs.getString("titulo"));
				System.out.println("ano = " + rs.getInt("ano"));
			}
			return res;*/

	}

	@Override
	public ArrayList<String> showStore() throws RemoteException {
		ArrayList<String> store = new ArrayList<String>(); /* Return */
		String aux = "", query = "SELECT ISBN10, titulo FROM livro"; /* SQL */

		try {
			ResultSet res = fetchData(query);

			/* Handling the Data */
			while (res.next()) {
				/*
				 * Each line of 'store' it's one book, aux is the tmp variable
				 * that holds each line of the result set
				 */
				System.out.println("isbn = " + res.getString("ISBN10"));
				aux = res.getString("ISBN10");
				aux = aux + " | ";
				aux += res.getString("titulo");

				/* DEBUG */
				System.out.println(aux);

				store.add(aux);
			}
		} catch (Exception e) {
			System.out.println(e.getMessage());
		}
		System.out.println("Enviando dados...");
		/* Returning the Array List */
		return store;
	}

	@Override
	public ArrayList<String> fetchDescription(String isbn)
			throws RemoteException {
		ArrayList<String> book = new ArrayList<String>();
		String aux, query = "SELECT titulo, descricao FROM livro "
				+ "WHERE ISBN10 = "; /* SQL */

		query.concat(isbn);

		/* DEBUG */
		System.out.println(query);

		/* Working on it */
		try {
			ResultSet res = fetchData(query);

			while (res.next()) {
				/* First Line is the ISBN string */
				aux = "ISBN: ";
				aux.concat(isbn);
				book.add(aux);

				/* Second Line is the Title String */
				aux = "Titulo: ";
				aux.concat(res.getString("titulo"));
				book.add(aux);

				/* Third Line is the Description */
				book.add(res.getString("descricao"));
			}

			/* Case Result Set was an empty set */
			if (book.size() == 0)
				book.add("Esse ISBN nao consta em nossos"
						+ "estoques, desculpe!");
		} catch (Exception e) {
			System.out.println(e.getMessage());
		}

		return book;
	}

	@Override
	public ArrayList<String> fetchInfos(String isbn) throws RemoteException {
		ArrayList<String> book = new ArrayList<String>();
		String aux, query = "SELECT * FROM livro WHERE ISBN10 = "; /* SQL */

		query.concat(isbn);

		/* DEBUG */
		System.out.println(query);

		try {
			ResultSet res = fetchData(query);

			while (res.next()) {
				/* First Line is the ISBN string */
				aux = "ISBN: ";
				aux.concat(isbn);
				book.add(aux);

				/* Second Line is the Title String */
				aux = "Titulo: ";
				aux.concat(res.getString("titulo"));
				book.add(aux);

				/* Third Line is the Publishing House */
				aux = "Editora: ";
				aux.concat(res.getString("editora"));
				book.add(aux);

				/* Fourth Line is the year */
				aux = "Ano: ";
				aux.concat(Integer.toString(res.getInt("ano")));
				book.add(aux);

				/* Fifth Line is the Number on Store */
				aux = "Estoque: ";
				aux.concat(Integer.toString(res.getInt("estoque")));
				book.add(aux);

				/* Descricao */
				book.add(res.getString("descricao"));
			}

			/* Result Set is Empty */
			if (book.size() == 0)
				book.add("Esse ISBN nao consta em nossos"
						+ "estoques, desculpe!");
		} catch (Exception e) {
			System.out.println(e.getMessage());
		}

		return book;
	}

	@Override
	public ArrayList<String> fetchAll() throws RemoteException {
		ArrayList<String> store = new ArrayList<String>();
		String aux, query = "SELECT * FROM livro"; /* SQL */

		try {
			ResultSet res = fetchData(query);

			/* Each book is separated by an '^D' */
			while (res.next()) {
				//store.add(res.getString());
			}

		} catch (Exception e) {
			System.out.println(e.getMessage());
		}
		return store;
	}

	@Override
	public int getNumber(String isbn) throws RemoteException {
		// TODO Auto-generated method stub
		return 0;
	}

	/* Remotes Methods */

	public static void main(String args[]) {

		try {
			BookStore obj = new BookStore();
			rmtBookStore stub = (rmtBookStore) UnicastRemoteObject.exportObject(obj, 0);

			// Bind the remote object's stub in the registry
			Registry registry = LocateRegistry.getRegistry();
			registry.bind("rmtBookStore", stub);

			System.err.println("Servidor ready");
		} catch (Exception e) {
			System.err.println("Servidor exception: " + e.toString());
			e.printStackTrace();
		}
	}
}
