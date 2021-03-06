/*
 * UDPClient.java
 *
 * Version:
 *   $Id$
 *
 * Revisions:
 *   $Log$
 *   Revision 1.3  2004/01/20 04:23:33  tristan
 *   updated comments for everything.
 *
 *   Revision 1.2  2004/01/19 21:11:01  tristan
 *   changed to use constants
 *
 *   Revision 1.1  2004/01/19 20:46:33  tristan
 *   refactored to a new name
 *
 *   Revision 1.6  2004/01/19 17:26:51  tristan
 *   implemented send/receive and part of command
 *
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
import java.net.SocketTimeoutException;
import java.io.ByteArrayOutputStream;
import java.io.DataOutputStream;
import java.io.DataInputStream;
import java.io.ByteArrayInputStream;

/**
 * Manages a UDP based connection with the server
 *
 * @author Tristan O'Tierney
 * @author Jordan Sissel
 * @author Nick Johnson
 */
public class UDPClient extends Client {
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
     * Creates a new UDPClient
     */
    public UDPClient( InetAddress server, int port ) {
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

        // send hello msg
        sendMessage( new Command( HELLO ) );

        // listen for response
        Response response = receiveResponse();
        if ( response != null && response.getId() == HELLO_RESPONSE ) {
            retVal = true;
        }
        setConnected( retVal );

        // update status
        setConnected( retVal );

        return retVal;
    }

    /**
     * Sends a command or response to the server.
     * @param msg The message to send.
     */
    public void sendMessage( Message msg ) {
        DatagramPacket packet = null;

        // make the packet
        try {
            // TODO include packet # maybe?
            buffer.reset();
            packet = new DatagramPacket( buffer.toByteArray(), buffer.size() );
            out.write( msg.toByteArray() );
        } catch ( Exception e ) {
        }

        // TODO add acknowledgement
        try {
            // send packet
            socket.send( packet );
        } catch ( Exception e ) {
            System.err.println( e.getMessage() );
            e.printStackTrace();
        }
    }

    /**
     * Receives a command object from the server.
     * @return A command object.
     */
    public Command receiveCommand() {
        Command retVal = null;
        DatagramPacket packet = null;

        // make the receive packet
        buffer.reset();
        packet = new DatagramPacket( buffer.toByteArray(), buffer.size() );

        // try and receive a packet
        for ( int i = 0; i < RETRY_COUNT && retVal == null; i++ ) {
            try {
                socket.setSoTimeout( TIMEOUT );
                socket.receive( packet );

                // received
                retVal = new Command( in.readUTF() );
                // TODO add way to parse args from bytes
            } catch ( Exception e ) {
            }
        }

        return retVal;
    }

    /**
     * Returns a response object from the server.
     * @return A response object from the server.
     */
    public Response receiveResponse() {
        Response retVal = null;
        DatagramPacket packet = null;

        // make the receive packet
        buffer.reset();
        packet = new DatagramPacket( buffer.toByteArray(), buffer.size() );

        // try and receive a packet
        for ( int i = 0; i < RETRY_COUNT && retVal == null; i++ ) {
            try {
                socket.setSoTimeout( TIMEOUT );
                socket.receive( packet );

                // received
                retVal = new Response( in.readInt(), in.readUTF() );
            } catch ( Exception e ) {
            }
        }

        return retVal;
    }
}   // UDPClient
