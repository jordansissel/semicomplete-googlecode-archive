/*
 * TCPClient.java
 *
 * Version:
 *   $Id$
 *
 * Revisions:
 *   $Log$
 *   Revision 1.3  2004/01/20 04:23:33  tristan
 *   updated comments for everything.
 *
 *   Revision 1.2  2004/01/19 21:10:09  tristan
 *   changed to use constants
 *
 *   Revision 1.1  2004/01/19 20:47:21  tristan
 *   refactored to a new name
 *
 *   Revision 1.5  2004/01/19 05:31:51  tristan
 *   implemented send/receive functions.
 *
 *   Revision 1.4  2004/01/13 18:14:54  tristan
 *   finished connect method. untested
 *
 *   Revision 1.3  2004/01/13 14:44:08  tristan
 *   updated connect and disconnect methods with status.
 *
 *   Revision 1.2  2004/01/13 02:26:36  tristan
 *   added some basic stuff to connect method
 *
 *   Revision 1.1  2004/01/13 01:34:41  tristan
 *   initial
 *
 */

import java.net.InetAddress;
import java.net.Socket;
import java.io.PrintWriter;
import java.io.BufferedReader;
import java.io.InputStreamReader;

/**
 * Manages a TCP based connection with the server.
 *
 * @author Tristan O'Tierney
 * @author Jordan Sissel
 * @author Nick Johnson
 */
public class TCPClient extends Client {
    private Socket socket;
    private PrintWriter out;
    private BufferedReader in;

    /**
     * Creates a new TCPClient
     */
    public TCPClient( InetAddress server, int port ) {
        super( server, port );
    }

    /**
     * Reads and sends messages to the server.
     */
    public void run() {
        try {
            String input = null;

            while ( ( input = in.readLine() ) != null ) {
                System.out.println( input );
            }
        } catch ( Exception e ) {
            System.err.println( e.getMessage() );
            e.printStackTrace();
        }
    }

    /**
     * Starts a tcp connection with the server.
     */
    public synchronized boolean connect() {
        boolean retVal = false;

        try {
            // connect to server
            socket = new Socket( getServerHost(), getServerPort() );
            out = new PrintWriter( socket.getOutputStream(), true );
            in = new BufferedReader( new InputStreamReader(
                                     socket.getInputStream() ) );

            // test for hello/response
            sendMessage( new Command( HELLO ) );
            if ( receiveResponse().getId() == HELLO_RESPONSE ) {
                retVal = true;
            }
        } catch ( Exception e ) {
            System.err.println( e.getMessage() );
            e.printStackTrace();
        }

        // change status var
        setConnected( retVal );

        return retVal;
    }

    /**
     * Disconnects from the server.
     */
    public synchronized void disconnect() {
        try {
            socket.close();
        } catch ( Exception e ) {
            System.err.println( e.getMessage() );
            e.printStackTrace();
        }

        // update status
        setConnected( false );
    }

    /**
     * Sends a message to the server.
     * @param msg The message to send.
     */
    public void sendMessage( Message msg ) {
        out.println( msg.toString() );
    }

    /**
     * Receves a command object from the server.
     * @return The command received.
     */
    public Command receiveCommand() {
        Command retVal = null;

        try {
            retVal = Command.parseCommand( in.readLine() );
        } catch ( Exception e ) {
            // error parsing?
        }

        return retVal;
    }

    /**
     * Receives a response object from the server.
     * @return The response object received.
     */
    public Response receiveResponse() {
        Response retVal = null;

        try {
            retVal = Response.parseResponse( in.readLine() );
        } catch ( Exception e ) {
        }

        return retVal;
    }
}   // TCPClient
