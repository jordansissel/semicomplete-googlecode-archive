/*
 * UDPClientConnection.java
 *
 * Version:
 *   $Id$
 *
 * Revisions:
 *   $Log$
 *   Revision 1.3  2004/01/13 02:26:52  tristan
 *   stubed out run method.
 *
 *   Revision 1.2  2004/01/13 01:36:54  tristan
 *   forgot something
 *
 *   Revision 1.1  2004/01/13 01:35:15  tristan
 *   initial
 *
 */

import java.net.InetAddress;

/**
 * Manages a UDP based connection with the server
 *
 * @author tristan
 */
public class UDPClientConnection extends ClientConnection {
    /**
     * Creates a new UDPClientConnection
     */
    public UDPClientConnection( InetAddress server, int port ) {
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
     * Disconnects from the server.
     */
    public void disconnect() {

    }

    /**
     * Starts a UDP transaction with the server.
     */
    public void connect() {

    }
}   // UDPClientConnection
