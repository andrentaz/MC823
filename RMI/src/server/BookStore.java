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
	private Boolean superuser;
	private String PWD = "numaPistacheCottapie";

	/* Constructor */
	public BookStore() throws ClassNotFoundException {
		/*
		 * Fazendo conexão com o banco de dados em SQLite3 a partir do conector
		 * JDBC. Todo o banco de dados estará acessível via atributo privado
		 * statement
		 */
		this.superuser = false;
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
		return (statement.executeQuery(query));
	}

	/* Remote Methods */
	@Override
	public ArrayList<String> showStore() throws RemoteException {
		ArrayList<String> store = new ArrayList<String>(); /* Return */
		String aux; /* SQL */

		try {
			ResultSet res = fetchData("SELECT ISBN10, titulo FROM livro");

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

		/* Returning the Array List */
		return store;
	}

	@Override
	public ArrayList<String> fetchDescription(String isbn)
			throws RemoteException {
		ArrayList<String> book = new ArrayList<String>();
		/* SQL */
		String aux = "SELECT titulo, descricao FROM livro WHERE ISBN10 = ";

		aux += isbn;

		/* DEBUG */
		System.out.println(aux);

		/* Working on it */
		try {
			ResultSet res = fetchData(aux);

			while (res.next()) {
				/* First Line is the ISBN string */
				aux = "ISBN: ";
				aux += isbn;
				book.add(aux);

				/* Second Line is the Title String */
				aux = "Titulo: ";
				aux += res.getString("titulo");
				book.add(aux);

				/* Third Line is the Description */
				book.add(res.getString("descricao"));
			}

			/* Case Result Set was an empty set */
			if (book.size() == 0)
				book.add("Esse ISBN nao consta em nossos"
						+ " estoques, desculpe!");
		} catch (Exception e) {
			System.out.println(e.getMessage());
		}

		return book;
	}

	@Override
	public ArrayList<String> fetchInfos(String isbn) throws RemoteException {
		ArrayList<String> book = new ArrayList<String>();
		String aux, query = "SELECT * FROM livro WHERE ISBN10 = "; /* SQL */

		query += isbn;

		/* DEBUG */
		System.out.println(query);

		try {
			ResultSet res = fetchData(query);

			while (res.next()) {
				/* First Line is the ISBN string */
				aux = "ISBN: ";
				aux += isbn;
				book.add(aux);

				/* Second Line is the Title String */
				aux = "Titulo: ";
				aux += res.getString("titulo");
				book.add(aux);

				/* Third Line is the Publishing House */
				aux = "Editora: ";
				aux += res.getString("editora");
				book.add(aux);

				/* Fourth Line is the year */
				aux = "Ano: ";
				aux += Integer.toString(res.getInt("ano"));
				book.add(aux);

				/* Fifth Line is the Number on Store */
				aux = "Estoque: ";
				aux += Integer.toString(res.getInt("estoque"));
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

	public ArrayList<Book> fetchAll() throws RemoteException {
		ArrayList<Book> store = new ArrayList<Book>();
		String query = "SELECT * FROM livro"; /* SQL */

		try {
			ResultSet res = fetchData(query), author;
			Book bk = new Book();
			ArrayList<String> tmp = new ArrayList<String>();

			while (res.next()) {
				/* Creating the object Book */
				bk.setIsbn(Integer.toString(res.getInt("ISBN10")));
				bk.setDesc(res.getString("descricao"));
				bk.setPubHouse(res.getString("editora"));
				bk.setRelDate(res.getInt("ano"));
				bk.setStore(res.getInt("estoque"));
				bk.setTitle("titulo");

				/* Getting author data */
				query = "SELECT * FROM autor WHERE a_id = "
						+ Integer.toString(res.getInt("autores"));

				/* DEBUG */
				System.out.println(query);

				author = fetchData(query);

				/* VERIFICAR */
				author.next();
				for (int i = 1; i < 5; i++) {
					if (author.getString(i) != null)
						tmp.add(author.getString(i));
				}
				bk.setAuthor(tmp);

				/* Add to the store array */
				store.add(bk);
			}

		} catch (Exception e) {
			System.out.println(e.getMessage());
		}
		return store;
	}

	public int getNumber(String isbn) throws RemoteException {
		String aux = "SELECT estoque FROM livro WHERE ISBN10 = "; /* SQL */

		aux += isbn;

		/* DEBUG */
		System.out.println();

		try {
			ResultSet res = fetchData(aux);

			while (res.next())
				return res.getInt("estoque");

			System.out.println("Esse ISBN nao consta em nossos "
					+ "estoques, desculpe!");

		} catch (Exception e) {
			System.out.println(e.getMessage());
		}

		return 0;
	}

	@Override
	public Boolean login(String pass) throws RemoteException {
		if (pass.equals(PWD)) {
			this.superuser = true;
			return true;
		}
		this.superuser = false;
		return false;
	}

	@Override
	public String setNumber(int num, String isbn) throws RemoteException,
			SQLException {
		// TODO Auto-generated method stub"update livro set estoque = "
		if (this.superuser) {
			String aux = "update livro set estoque = ";
			aux += num + " where ISBN10 = " + isbn;
			statement.executeUpdate(aux);
			return "Estoque alterado com sucesso!";
		}
		return "Voce nao tem permissoes para mudar estoques...";
	}
	
	public static void main(String args[]) {

		try {
			BookStore obj = new BookStore();
			rmtBookStore stub = (rmtBookStore) UnicastRemoteObject
					.exportObject(obj, 0);

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