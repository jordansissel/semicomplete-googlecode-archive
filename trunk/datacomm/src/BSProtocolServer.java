/*
 * BSProtocolServer.java
 * 
 * Version:
 *   $Id$
 *
 * Revisions:
 *   $Log$
 *   Revision 1.2  2004/01/20 07:37:53  tristan
 *   implemented many of the basic methods.
 *
 *   Revision 1.1  2004/01/20 07:13:08  tristan
 *   wrote basic class
 *
 */

import java.util.List;
import java.util.ArrayList;
import java.util.Iterator;
import java.net.InetAddress;

/**
 * Abstract class for a common protocol based battle ship server.
 *
 * @author Tristan O'Tierney
 * @author Jordan Sissel
 * @author Nick Johnson
 */
public abstract class BSProtocolServer extends Thread {
    private InetAddress host;
    private int port;
    private List clients;
    private boolean running;

    /**
     * Initializes the bs protocol server.
     * @param host The host this protocol binds to.
     * @param port The port to bind to.
     */
    public BSProtocolServer( InetAddress host, int port ) {
        this.host = host;
        this.port = port;
        this.clients = new ArrayList();
        this.running = false;
    }

    /**
     * Gets a list of the clients connected to this server.
     * @return A list of the connected clients.
     */
    public List getClients() {
        return clients;
    }

    /**
     * Sets the list of clients connected to the server.
     * @param clients
     */
    public void setClients( List clients ) {
        this.clients = clients;
    }

    /**
     * Returns whether or not this server's event loop should continue.
     * @return True if the event loop should be running.
     */
    public boolean isRunning() {
        return running;
    }

    /**
     * Changes this event loop's running status.
     * @param running Whether or not the event loop should continue.
     */
    public void setRunning( boolean running ) {
        this.running = running;
    }

    /**
     * Gets the port this protocol listens on.
     * @return The port this protocol listens on.
     */
    public int getPort() {
        return port;
    }

    /**
     * Sets the port this protocol listens on.
     * @param port The port this protocol listens on.
     */
    public void setPort( int port ) {
        this.port = port;
    }

    /**
     * Returns the host this protocol binds to.
     * @return The host for this protocol.
     */
    public InetAddress getHost() {
        return host;
    }

    /**
     * Sets the host this protocol binds to.
     * @param host The host to bind to.
     */
    public void setHost( InetAddress host ) {
        this.host = host;
    }

    /**
     * Starts the protocol server thread.
     */
    public void start() {
        running = true;

        super.start();
    }

    /**
     * Shuts down the server.
     */
    protected void finalize() {
        if ( running ) {
            shutdown();
        }
    }

    /**
     * Sets running to false and disconnects clients.
     */
    public void shutdown() {
        running = false;

        // shut down each
        Iterator i = clients.iterator();
        while ( i.hasNext() ) {
            Server server = ( Server ) i.next();

            server.disconnect();
        }
    }
}   // BSProtocolServer