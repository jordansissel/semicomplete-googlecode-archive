/*
 * TCPServer.java
 * 
 * Version:
 *   $Id$
 *
 * Revisions:
 *   $Log$
 *   Revision 1.3  2004/01/19 23:48:52  psionic
 *   - Bulk commit: Command infrastructure improved. New commands can be added
 *     dynamically now.
 *
 *   Revision 1.2  2004/01/19 20:54:19  psionic
 *   - Updated server-side happyland framework for TCP
 *
 *
 */

import java.io.*;
import java.net.*;

/**
 * Manages a server's connection with the client
 *
 * @author Jordan Sissel
 */

public class TCPServer extends Thread {
	private Socket sock;
	private ServerGame game;
	private BattleshipServer server;
	private boolean error;

	public TCPServer(Socket sock, BattleshipServer serv) {
		this.sock = sock;
		server = serv;
		error = false;
	}

	public void run() {

		try {
			String line = null;
			BufferedReader in = new BufferedReader( new InputStreamReader(sock.getInputStream()));
			PrintWriter out = new PrintWriter( sock.getOutputStream() );

			while (!error) {
				line = in.readLine();
				if (! line.matches("^\\s*$")) {
					if (line != null) {
						try {
							Command foo = Command.parseCommand(line);
							System.out.println("--> " + foo);
						} catch (InvalidCommandArgumentsException e) {
							System.err.println("* Invalid arguments sent. \"" + line + "\"");
							Response error = new ErrorResponse("Invalid arguments.");
							out.println(error);
						}
					} else {
						error = true;
					}
				}
			}

			System.err.println("There was an error on the socket, it is now closed.");

		} catch (IOException e) {
			System.out.println(e);
		}

	}
} // TCPServer
