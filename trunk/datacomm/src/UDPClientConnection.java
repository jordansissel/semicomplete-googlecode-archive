/*
 * UDPClientConnection.java
 *
 * Version:
 *   $Id$
 *
 * Revisions:
 *   $Log$
 *   Revision 1.4  2004/01/13 14:55:00  tristan
 *   started connect method.
 *
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
import java.net.DatagramSocket;
import java.net.DatagramPacket;

/**
 * Manages a UDP based connection with the server
 *
 * @author tristan
 */
public class UDPClientConnection extends ClientConnection {
    // constants
    private static final long RETRY_DELAY = 5 * 1000;
    private static final int RETRY_COUNT = 3;
    private static final int BUFFER_SIZE = 1024;
    private static final int TIMEOUT = 30 * 1000;

    // member variables
    private DatagramSocket socket;

    /**
     * Creates a new UDPClientConnection
     */
    public UDPClientConnection( InetAddress server, int port ) {
        super( server, port );

        try {
            socket = new DatagramSocket();
        } catch ( Exception e ) {
            System.err.println( e.getMessage() );
            e.printStackTrace();
        }
    }

    /**
     * Reads and sends messages to the server.
     */
    public void run() {
        try {
            while ( isConnected() ) {
                // send alive msg
                // listen for response
                // sleep delay time
                sleep( RETRY_DELAY );
            }
        } catch ( Exception e ) {
            System.err.println( e.getMessage() );
            e.printStackTrace();
        }
    }

    /**
     * Disconnects from the server.
     */
    public synchronized void disconnect() {
        setConnected( false );
    }

    /**
     * Starts a UDP transaction with the server.
     */
    public synchronized boolean connect() {
        boolean retVal = false;

        // attempt to send a hello to the server
        socket.connect( getServerHost(), getServerPort() );
        byte[] buffer = new byte[ BUFFER_SIZE ];
        DatagramPacket packet = new DatagramPacket( buffer, buffer.length );
        try {
            // TODO create special hello packet
            socket.send( packet );
            socket.setSoTimeout( TIMEOUT );
            socket.receive( packet );
            retVal = true;
        } catch ( Exception e ) {
            System.err.println( e.getMessage() );
            e.printStackTrace();
        }

        // update status
        setConnected( retVal );

        return retVal;
    }
}   // UDPClientConnection
