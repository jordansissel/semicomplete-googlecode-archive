/*
 * UDPClientConnection.java
 *
 * Version:
 *   $Id$
 *
 * Revisions:
 *   $Log$
 *   Revision 1.5  2004/01/13 18:03:12  tristan
 *   added packet generation to connect method. untested
 *
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
import java.io.ByteArrayOutputStream;
import java.io.DataOutputStream;
import java.io.DataInputStream;
import java.io.ByteArrayInputStream;

/**
 * Manages a UDP based connection with the server
 *
 * @author tristan
 */
public class UDPClientConnection extends ClientConnection {
    // constants
    private static final long RETRY_DELAY = 5 * 1000;
    private static final int RETRY_COUNT = 3;
    private static final int TIMEOUT = 30 * 1000;
    private static final int BUFFER_SIZE = 1024;

    // member variables
    private DatagramSocket socket;
    private DataOutputStream out;
    private DataInputStream in;
    private ByteArrayOutputStream buffer;

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

        // initialize byte streams
        buffer = new ByteArrayOutputStream( BUFFER_SIZE );
        out = new DataOutputStream( buffer );
        in = new DataInputStream( new ByteArrayInputStream(
                                  buffer.toByteArray() ) );
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
        DatagramPacket packet = new DatagramPacket( buffer.toByteArray(),
                                                    buffer.size() );

        for ( int i = 0; i < RETRY_COUNT && !retVal; i++ ) {
            try {
                // set msg
                buffer.reset();
                out.writeInt( HELLO_MSG );

                // send package
                socket.send( packet );
                socket.setSoTimeout( TIMEOUT );

                // test response
                socket.receive( packet );
                int response = in.readInt();
                if ( response == HELLO_RESPONSE ) {
                    retVal = true;
                }
            } catch ( Exception e ) {
                System.err.println( e.getMessage() );
                e.printStackTrace();
            }
        }

        // update status
        setConnected( retVal );

        return retVal;
    }
}   // UDPClientConnection
