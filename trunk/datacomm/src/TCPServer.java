/*
 * TCPServer.java
 * 
 * Version:
 *   $Id$
 *
 * Revisions:
 *   $Log$
 *   Revision 1.5  2004/01/20 04:23:33  tristan
 *   updated comments for everything.
 *
 *   Revision 1.4  2004/01/20 03:57:35  psionic
 *   *** empty log message ***
 *
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
 * @author Tristan O'Tierney
 * @author Jordan Sissel
 * @author Nick Johnson
 */

public class TCPServer extends Thread {
	private Socket sock;
	private ServerGame game;
	private BattleshipServer server;
	private boolean error = false;
	private boolean hellodone = false;
	private boolean isready = false;

    /**
     * Creates a new tcp server.
     * @param sock The socket to connect to.
     * @param serv The battle ship server for this connection.
     */
	public TCPServer(Socket sock, BattleshipServer serv) {
		this.sock = sock;
		server = serv;
	}

	public void setGame(ServerGame g) {
		game = g;
	}

    /**
     * Runs the clients in a thread.
     */
	public void run() {

		try {
			String line = null;
			BufferedReader in = new BufferedReader( new InputStreamReader(sock.getInputStream()));
			PrintWriter out = new PrintWriter( sock.getOutputStream() );
			bool started = false;

			// Game hasn't started yet.
			// Look for hello
			while (!isready) {
				// Look for hello.
				line = in.readLine();
				if (line != null) {
					if (! line.matches("^\\s*$")) {
						try {
							Command foo = Command.parseCommand(line);
							System.out.println("--> " + foo);
							if (foo instanceOf HelloCommand) {
								if (!hellodone) {
									hellodone = true;
									Response resp = new HelloResponse();
									out.println(resp);
								} else {
									Response error = new ErrorResponse("You already said hello!");
									out.println(error);
								}
							}

							if (foo instanceOf StartGameCommand) {
								if (!isready) && (hellodone) {
									isready = true;
									Response resp = new StartGameResponse();
									out.println(resp);
								}
							}
						} catch (InvalidCommandArgumentsException e) {
							Response error = new ErrorResponse("Invalid arguments.");
							out.println(error);
						}
					}
				}
			}

			// Wait for PlayerFound
			while (playerWanted() {
				yield(50);
			}

			while (!error) {
				line = in.readLine();
				if (line != null) {
					if (! line.matches("^\\s*$")) {
						try {
							Command foo = Command.parseCommand(line);
							System.out.println("--> " + foo);
						} catch (InvalidCommandArgumentsException e) {
							System.err.println("* Invalid arguments sent. \"" + line + "\"");
							Response error = new ErrorResponse("Invalid arguments.");
							out.println(error);
						}
					}
				} else {
					error = true;
				}
			}

			System.err.println("There was an error on the socket, it is now closed.");

		} catch (IOException e) {
			System.out.println(e);
		}
	}
} // TCPServer
