/*
 * TCPClientConnection.java
 *
 * Version:
 *   $Id$
 *
 * Revisions:
 *   $Log$
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

        } catch ( Exception e ) {
            System.err.println( e.getMessage() );
            e.printStackTrace();
        }
    }

    /**
     * Starts a tcp connection with the server.
     */
    public void connect() {
        try {
            socket = new Socket( getServerHost(), getServerPort() );
            out = new PrintWriter( socket.getOutputStream(), true );
            in = new BufferedReader( new InputStreamReader(
                                     socket.getInputStream() ) );
        } catch ( Exception e ) {
            System.err.println( e.getMessage() );
            e.printStackTrace();
        }
    }

    /**
     * Disconnects from the server.
     */
    public void disconnect() {
        try {
            socket.close();
        } catch ( Exception e ) {
            System.err.println( e.getMessage() );
            e.printStackTrace();
        }
    }
}   // TCPClientConnection
