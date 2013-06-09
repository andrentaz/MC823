/* Andre Nakagaki Filliettaz --------------------------------------------------
 * client.java ----------------------------------------------------------------
 * ----------------------------------------------------------------------------
 * Description: client object that make all the comunication with the server 
 * and deals with the user of the application ---------------------------------
 * ------------------------------------------------------------------------- */


package client;

import java.rmi.registry.*;
import java.util.ArrayList;

import client.rmtBookStore;;

public class Cliente {

	private Cliente() {
	}

	public static void main(String[] args) {

		String host = (args.length < 1) ? null : args[0];
		try {
			Registry registry = LocateRegistry.getRegistry(host);
			rmtBookStore stub = (rmtBookStore) registry.lookup("rmtBookStore");
			ArrayList<String> response = stub.showStore();
			for(int i=0; i < response.size(); i++) {
				System.out.println("response: " + response.get(i));
			}
			System.out.println("resposta: " + response.size());
		} catch (Exception e) {
			System.err.println("Cliente exception: " + e.toString());
			e.printStackTrace();
		}
	}
}
