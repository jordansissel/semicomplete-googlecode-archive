/*
 * TCPServer.java
 * 
 * Version:
 *   $Id$
 *
 * Revisions:
 *   $Log$
 *   Revision 1.10  2004/01/26 21:21:28  psionic
 *   - Updated commands framework, added game-starting ability
 *
 *   Revision 1.9  2004/01/26 20:34:57  psionic
 *   *** empty log message ***
 *
 *   Revision 1.8  2004/01/20 08:57:35  tristan
 *   fixed compile time errors.
 *
 *   Revision 1.7  2004/01/20 04:31:19  tristan
 *   fixed commenting.
 *
 *   Revision 1.6  2004/01/20 04:30:25  tristan
 *   fixed syntax errors.
 *
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

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;

/**
 * Manages a server's connection with the client
 *
 *' @author Tristan O'Tierney
 * @author Jordan Sissel
 * @author Nick Johnson
 */

public class TCPServer extends Server {
    private Socket socket;
    private ServerGame game;
    private BSServer server;
    private boolean error = false;
    private boolean hellodone = false;
    private boolean isready = false;
    private BufferedReader in;
    private PrintWriter out;

    /**
     * Creates a new tcp server.
     *
     * @param socket The socket to connect to.
     * @param server The battle ship server for this connection.
     */
    public TCPServer( Socket socket, BSServer server ) {
        super( server );

        this.socket = socket;

        try {
            in = new BufferedReader(
                new InputStreamReader( socket.getInputStream() )
            );
            out = new PrintWriter( socket.getOutputStream(), true );
        } catch ( Exception e ) {
            disconnect();
        }
    }

    /**
     * Sets the game for this tcp server.
     *
     * @param g The game shared by the clients.
     */
    public void setGame( ServerGame g ) {
        game = g;
    }

    /**
     * Runs the clients in a thread.
     */
    public void run() {


        try {
            String line = null;
            boolean started = false;

            // Game hasn't started yet.
            // Look for hello
            while ( !isready ) {
                // Look for hello.
                line = in.readLine();
                if ( line != null ) {
                    if ( !line.matches( "^\\s*$" ) ) {
                        try {
                            Command foo = Command.parseCommand( line );
                            System.out.println( "--> " + foo );
                            if ( foo instanceof HelloCommand ) {
                                if ( !hellodone ) {
                                    hellodone = true;
                                    Response resp = new HelloResponse();
                                    out.println( resp );
                                } else {
                                    Response error = new ErrorResponse(
                                        "You already said hello!"
                                    );
                                    out.println( error );
                                }
                            }

                            if ( foo instanceof StartgameCommand ) {
                                if ( !isready && hellodone ) {
                                    isready = true;
                                    Response resp = new StartGameResponse();
                                    out.println( resp );
												Player p = new TCPPlayer((String)foo.getArgs(0), socket);
												server.addPlayer(p);
												game = server.getGame(p);
												
                                }
                            }
                        } catch ( InvalidCommandArgumentsException e ) {
                            Response error = new ErrorResponse(
                                "Invalid arguments."
                            );
                            out.println( error );
                        }
                    }
                }
            }

            // Wait for PlayerFound
            //while ( playerWanted() ) {
            //   yield( 50 );
            //}

            while ( !error ) {
                line = in.readLine();
                if ( line != null ) {
                    if ( !line.matches( "^\\s*$" ) ) {
                        try {
                            Command foo = Command.parseCommand( line );
                            System.out.println( "--> " + foo );
                        } catch ( InvalidCommandArgumentsException e ) {
                            System.err.println( "* Invalid arguments sent. \"" +
                                                line + "\"" );
                            Response error = new ErrorResponse(
                                "Invalid arguments."
                            );
                            out.println( error );
                        }
                    }
                } else {
                    error = true;
                }
            }

            System.err.println( "There was an error on the socket, " +
                                "it is now closed." );
        } catch ( IOException e ) {
            System.out.println( e );
        }
    }

    /**
     * Disconnects the client.
     */
    public void disconnect() {

    }
} // TCPServer
