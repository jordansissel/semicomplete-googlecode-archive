/*
 * TCPServer.java
 * 
 * Version:
 *   $Id$
 *
 * Revisions:
 *   $Log$
 *   Revision 1.2  2004/01/19 20:54:19  psionic
 *   - Updated server-side happyland framework for TCP
 *
 *
 */

/**
 * Manages a server's connection with the client
 *
 * @author Jordan Sissel
 */

public class TCPServer extends Thread {
	private Socket sock;
	private String user;
	private boolean error;

	public TCPServer(Socket sock) {
		this.sock = sock;
		error = false;
	}

	public void run() {
		String line = null;
		BufferedReader in = new BufferedReader( new InputStreamReader(sock.getInputStream()));
		PrintWriter out = new PrintWriter( sock.getOutputStream() );

		while (!error) {
			
		}
	}
} // TCPServer
