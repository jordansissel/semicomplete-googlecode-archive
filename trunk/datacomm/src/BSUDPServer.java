/*
 * BSUDPServer.java
 * 
 * Version:
 *   $Id$
 *
 * Revisions:
 *   $Log$
 *   Revision 1.2  2004/01/20 08:29:04  tristan
 *   set up basic layout.
 *
 *   Revision 1.1  2004/01/20 07:04:17  tristan
 *   initial
 *
 */

import java.net.InetAddress;
import java.net.DatagramSocket;

/**
 * Manages UDP connections for a battle ship server.
 *
 * @author Tristan O'Tierney
 * @author Jordan Sissel
 * @author Nick Johnson
 */
public class BSUDPServer extends BSProtocolServer {
    private DatagramSocket socket;

    /**
     * Starts the udp server and binds to a host/port.
     * @param server The main parent server.
     * @param host The host to bind to.
     * @param port The port to bind to.
     */
    public BSUDPServer( BSServer server, InetAddress host, int port ) {
        super( server, host, port );

        try {
            socket = new DatagramSocket( port, host );
        } catch ( Exception e ) {
            shutdown();
        }
    }

    /**
     * Starts the event loop for udp listening.
     */
    public void run() {
        while( isRunning() ) {
            // listen for connections.
        }
    }
}   // BSUDPServer