/*
 * BSTCPServer.java
 * 
 * Version:
 *   $Id$
 *
 * Revisions:
 *   $Log$
 *   Revision 1.2  2004/01/20 08:18:36  tristan
 *   included the bs server too.
 *
 *   Revision 1.1  2004/01/20 07:03:17  tristan
 *   initial
 *
 */

import java.net.InetAddress;
import java.net.ServerSocket;
import java.net.Socket;

/**
 * Manages the TCP connections for a battleship server.
 *
 * @author Tristan O'Tierney
 * @author Jordan Sissel
 * @author Nick Johnson
 */
public class BSTCPServer extends BSProtocolServer {
    private ServerSocket socket;

    /**
     * Initialize the tcp protocol server.
     * @param server The main parent server.
     * @param host The host to bind to.
     * @param port The port to bind to.
     */
    public BSTCPServer( BSServer server, InetAddress host, int port ) {
        super( server, host, port );
        try {
            socket = new ServerSocket( port, 0, host );
        } catch ( Exception e ) {
            shutdown();
        }
    }

    /**
     * Run loop to listen for connections.
     */
    public void run() {
        while( isRunning() ) {
            try {
                Socket accept = socket.accept();
                Server connection = new TCPServer( accept, getServer() );

                getClients().add( connection );
                connection.start();
            } catch ( Exception e ) {
            }
        }
    }
}   // BSTCPServer