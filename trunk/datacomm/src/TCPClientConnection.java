/*
 * TCPClientConnection.java
 *
 * Version:
 *   $Id$
 *
 * Revisions:
 *   $Log$
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
 * @author tristan
 */
public class TCPClientConnection extends ClientConnection {
    private Socket socket;
    private PrintWriter out;
    private BufferedReader in;

    /**
     * Creates a new TCPClientConnection
     */
    public TCPClientConnection( InetAddress server, int port ) {
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
            out.println( HELLO_MSG + " Hello" );
            if ( parseMessage( in.readLine() ) == HELLO_RESPONSE ) {
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
     * Returns the integer corresponding to this msg.
     * @param msg The message to parse.
     * @return The integer value of the msg. -1 if error.
     */
    private int parseMessage( String msg ) {
        int retVal = -1;
        String[] parsed = msg.split( "\\s", 1 );

        if ( parsed.length >= 1 ) {
            try {
                retVal = Integer.parseInt( parsed[ 0 ] );
            } catch ( NumberFormatException e ) {
            }
        }

        return retVal;
    }
}   // TCPClientConnection
