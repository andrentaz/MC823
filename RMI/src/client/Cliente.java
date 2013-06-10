/* Andre Nakagaki Filliettaz --------------------------------------------------
 * client.java ----------------------------------------------------------------
 * ----------------------------------------------------------------------------
 * Description: client object that make all the comunication with the server 
 * and deals with the user of the application ---------------------------------
 * ------------------------------------------------------------------------- */


package client;

import java.rmi.registry.*;
import java.util.ArrayList;

import server.Book;

import client.rmtBookStore;;

public class Cliente {

	private Cliente() {
	}

	public static void main(String[] args) {

		String host = (args.length < 1) ? null : args[0];
		try {
			Registry registry = LocateRegistry.getRegistry(host);
			rmtBookStore stub = (rmtBookStore) registry.lookup("rmtBookStore");
			
			ArrayList<String> response = stub.fetchDescription("9112033453");
			for(int i=0; i < response.size(); i++) {
				System.out.println(response.get(i));
			}
			System.out.println("resposta: " + response.size());
			
			response = stub.showStore();
			for(int i=0; i < response.size(); i++) {
				System.out.println(response.get(i));
			}
			System.out.println(response.size());
			
			response = stub.fetchInfos("1234567890");
			for(int i=0; i < response.size(); i++) {
				System.out.println(response.get(i));
			}
			System.out.println(response.size());
			//TODO Consertar impressão
			/*
			ArrayList<Book> response2 = stub.fetchAll();
			for(int i=0; i < response2.size(); i++) {
				System.out.println(response2.get(i));
			}
			System.out.println(response2.size());
			*/
			System.out.println(stub.setNumber(42, "1234567890"));
			
			if(stub.login("numaPistacheCottapie") == true) {
				System.out.println("logado");
			}
			else {
				System.out.println("senha incorreta");
			}
			
			System.out.println(stub.setNumber(42, "1234567890"));
			
			System.out.println(stub.getNumber("1234567890"));
			
			System.out.println(stub.setNumber(25, "1234567890"));
			
			System.out.println(stub.getNumber("1234567890"));
			
		} catch (Exception e) {
			System.err.println("Cliente exception: " + e.toString());
			e.printStackTrace();
		}
	}
}
