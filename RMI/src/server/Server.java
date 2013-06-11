/* Andre Nakagaki Filliettaz --------------------------------------------------
 * Server.java ----------------------------------------------------------------
 * ----------------------------------------------------------------------------
 * Description: Server application that make all the comunication with the ----
 * client and available the RMI Object ----------------------------------------
 * ------------------------------------------------------------------------- */

package server;

import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;
import java.rmi.server.UnicastRemoteObject;
import client.RmtBookStore;

public class Server {
	/* Main */
	public static void main(String[] args) {
		
		try {
			BookStore obj = new BookStore();
			RmtBookStore stub = (RmtBookStore) UnicastRemoteObject.exportObject(obj, 0);
			
			/* Binds the remote object's stub in the registry */
			Registry registry = LocateRegistry.getRegistry();
			registry.bind("RmtBookStore", stub);
			
			System.out.println("Servidor Online");
			
		} catch (Exception e) {
			System.err.println("Servidor exception: " + e.toString());
			e.printStackTrace();
		}
	}
}
