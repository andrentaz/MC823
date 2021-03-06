/* Andre Nakagaki Filliettaz --------------------------------------------------
 * BookStore.java -------------------------------------------------------------
 * ----------------------------------------------------------------------------
 * Description: class that implements the remote interface and will be used by
 * the server to send all the stuffs ------------------------------------------
 * ------------------------------------------------------------------------- */

package server;

import java.rmi.RemoteException;
import java.util.ArrayList;
import client.RmtBookStore;
import java.sql.*;

public class BookStore implements RmtBookStore {
	private Statement statement;
	private String pass;

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
		} finally {
			try {
				if (connection != null)
					connection.close();
			} catch (SQLException e) {
				// connection close failed.
				System.err.println(e);
			}
		}
	}
	
	/* Methods */
	/* Remote Methods */
	@Override
	public ArrayList<String> showStore() throws RemoteException {
		ArrayList<String> store = new ArrayList<String>();		/* Return */
		String aux;	/* SQL */

		try {
			ResultSet res = statement.executeQuery("SELECT ISBN10, " +
					"titulo FROM livro");

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
		/* SQL */
		String aux = "SELECT titulo, descricao FROM livro WHERE ISBN10 = ";

		aux.concat(isbn);

		/* DEBUG */
		System.out.println(aux);

		/* Working on it */
		try {
			ResultSet res = statement.executeQuery(aux);

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
			if(book.size() == 0)	book.add("Esse ISBN nao consta em nossos" +
					" estoques, desculpe!");
		} catch (Exception e) {
			System.out.println(e.getMessage());
		}

		return book;
	}


	@Override
	public ArrayList<String> fetchInfos(String isbn) throws RemoteException {
		ArrayList<String> book = new ArrayList<String>();
		String aux, query = "SELECT * FROM livro WHERE ISBN10 = ";	/* SQL */

		query.concat(isbn);

		/* DEBUG */
		System.out.println(query);

		try {
			ResultSet res = statement.executeQuery(query);

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
			if(book.size() == 0)	book.add("Esse ISBN nao consta em nossos" +
					"estoques, desculpe!");
		} catch (Exception e) {
			System.out.println(e.getMessage());
		}

		return book;
	}


	public ArrayList<Book> fetchAll() throws RemoteException {
		ArrayList<Book> store = new ArrayList<Book>();
		String query = "SELECT * FROM livro";	/* SQL */

		try {
			ResultSet res = statement.executeQuery(query), author;
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
				query = "SELECT * FROM author WHERE a_id = " +
						Integer.toString(res.getInt("autores"));

				/* DEBUG */
				System.out.println(query);

				author = statement.executeQuery(query);

				/* VERIFICAR */
				author.next();
				for (int i=1; i<5; i++) {
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
		String aux = "SELECT estoque FROM livro WHERE ISBN10 = ";	/* SQL */

		aux.concat(isbn);

		/* DEBUG */
		System.out.println();

		try {
			ResultSet res = statement.executeQuery(aux);

			while (res.next())
				return res.getInt("estoque");

			return -1;

		} catch (Exception e) {
			System.out.println(e.getMessage());
		}

		return 0;
	}

	
	@Override
	public String setNumber(int num, String isbn, String pwd)
			throws RemoteException, SQLException {
		
		if (pwd == this.pass) {
			String aux = "UPDATE livro SET estoque = ";
			aux += num + " WHERE ISBN10 = " + isbn;			
			statement.executeUpdate(aux);
			return "Estoque alterado com sucesso!";
		}
		else return "Incorrect Password! Permission denied!";
	}

}