/*
 * ClientConnection.java
 *
 * Version:
 *   $Id$
 *
 * Revisions:
 *   $Log$
 *   Revision 1.3  2004/01/13 02:27:13  tristan
 *   added accessors and mutators.
 *
 *   Revision 1.2  2004/01/13 01:53:23  tristan
 *   added a var
 *
 *   Revision 1.1  2004/01/13 01:33:38  tristan
 *   initial
 *
 */

import java.net.InetAddress;

/**
 * Manages a client connection with the server.
 *
 * @author tristan
 */
public abstract class ClientConnection extends Thread {
    protected InetAddress serverHost;
    protected int serverPort;

    /**
     * Initializes the server's inet address.
     * @param serverHost The server's inet address.
     */
    public ClientConnection( InetAddress serverHost, int serverPort ) {
        this.serverHost = serverHost;
        this.serverPort = serverPort;
    }

    /**
     * Returns the server's inet address.
     * @return The current server's inet address.
     */
    public InetAddress getServerHost() {
        return serverHost;
    }

    /**
     * Sets the server's inet address.
     * @param serverHost The new server inet address.
     */
    public void setServerHost( InetAddress serverHost ) {
        this.serverHost = serverHost;
    }

    /**
     * Returns the server's port.
     * @return The server's port.
     */
    public int getServerPort() {
        return serverPort;
    }

    /**
     * Changes the server's port number.
     * @param serverPort The new server port number.
     */
    public void setServerPort( int serverPort ) {
        this.serverPort = serverPort;
    }

    /**
     * Disconnects the connection from the server
     */
    public void finalize() {
        disconnect();
    }

    // abstract methods
    public abstract void connect();
    public abstract void disconnect();

}   // ClientConnection
