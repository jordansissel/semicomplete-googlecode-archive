/*
 * Client.java
 *
 * Version:
 *   $Id$
 *
 * Revisions:
 *   $Log$
 *   Revision 1.1  2004/01/19 20:47:49  tristan
 *   refactored to a new name
 *
 *   Revision 1.9  2004/01/19 16:36:02  tristan
 *   switched to extend thread
 *
 *   Revision 1.8  2004/01/19 05:32:42  tristan
 *   implemented send/receive functions.
 *
 *   Revision 1.7  2004/01/19 05:17:03  tristan
 *   switched to use command and response for receiving
 *
 *   Revision 1.6  2004/01/19 05:13:39  tristan
 *   added send and receive msg commands
 *
 *   Revision 1.5  2004/01/13 18:03:32  tristan
 *   began adding constants for message identifiers
 *
 *   Revision 1.4  2004/01/13 14:43:27  tristan
 *   added connection status information
 *
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
public abstract class Client extends Thread {
    // constants
    public static final int HELLO_RESPONSE = 101;

    // member variables
    private InetAddress serverHost;
    private int serverPort;
    private boolean connected;

    /**
     * Initializes the server's inet address.
     * @param serverHost The server's inet address.
     */
    public Client( InetAddress serverHost, int serverPort ) {
        this.serverHost = serverHost;
        this.serverPort = serverPort;
        this.connected = false;
    }

    /**
     * Returns the server's inet address.
     * @return The current server's inet address.
     */
    public synchronized InetAddress getServerHost() {
        return serverHost;
    }

    /**
     * Sets the server's inet address.
     * @param serverHost The new server inet address.
     */
    public synchronized void setServerHost( InetAddress serverHost ) {
        this.serverHost = serverHost;
    }

    /**
     * Returns the server's port.
     * @return The server's port.
     */
    public synchronized int getServerPort() {
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
     * Sets the connected variable
     * @param connected Sets whether or not the client is connected.
     */
    public synchronized void setConnected( boolean connected ) {
        this.connected = connected;
    }

    /**
     * Returns whether or not the client is connected.
     * @return If the client is connected.
     */
    public synchronized boolean isConnected() {
        return connected;
    }

    /**
     * Disconnects the connection from the server
     */
    public void finalize() {
        disconnect();
    }

    // abstract methods
    public abstract boolean connect();
    public abstract void disconnect();
    public abstract void sendMessage( Message var );
    public abstract Command receiveCommand();
    public abstract Response receiveResponse();

}   // Client
