/*
 * BSServer.java
 * 
 * Version:
 *   $Id$
 *
 * Revisions:
 *   $Log$
 *   Revision 1.2  2004/01/20 07:58:56  tristan
 *   finished most methods for bs server.
 *
 *   Revision 1.1  2004/01/20 06:54:32  tristan
 *   initial
 *
 */

import java.net.InetAddress;
import java.util.List;
import java.util.ArrayList;
import java.util.Iterator;

/**
 * Manages and listens for clients connecting.
 *
 * @author Tristan O'Tierney
 * @author Jordan Sissel
 * @author Nick Johnson
 */
public class BSServer {
    private boolean running;
    private List protocols;

    /**
     * Initializes the various protocol servers and starts them.
     * @param host The host to bind to.
     * @param tcpPort The tcp protocol port.
     * @param udpPort The udp protocol port.
     */
    public BSServer( InetAddress host, int tcpPort, int udpPort ) {
        protocols = new ArrayList();
        protocols.add( new BSTCPServer( this, host, tcpPort ) );
        protocols.add( new BSUDPServer( this, host, udpPort ) );

        running = false;
    }

    /**
     * Returns a list of the supported protocols for this server.
     * @return The protocols this server listens for.
     */
    public List getProtocols() {
        return protocols;
    }

    /**
     * Returns whether or not this server is running.
     * @return True if the server is started.
     */
    public boolean isRunning() {
        return running;
    }

    /**
     * Starts the servers.
     */
    public void start() {
        running = true;

        Iterator i = protocols.iterator();
        while ( i.hasNext() ) {
            ( ( BSProtocolServer ) i.next() ).start();
        }
    }

    /**
     * Shuts down protocol servers.
     */
    public void shutdown() {
        running = false;

        Iterator i = protocols.iterator();
        while ( i.hasNext() ) {
            ( ( BSProtocolServer ) i.next() ).shutdown();
        }
    }

    /**
     * Shuts down the protocol servers.
     */
    protected void finalize() {
        if ( running ) {
            shutdown();
        }
    }
}   // BSServer