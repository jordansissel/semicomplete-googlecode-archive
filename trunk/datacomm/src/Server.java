/*
 * Server.java
 * 
 * Version:
 *   $Id$
 *
 * Revisions:
 *   $Log$
 *   Revision 1.3  2004/01/20 08:21:27  tristan
 *   no message
 *
 *   Revision 1.2  2004/01/20 04:23:33  tristan
 *   updated comments for everything.
 *
 *   Revision 1.1  2004/01/19 20:40:58  tristan
 *   refactored to a new file name.
 *
 *   Revision 1.2  2004/01/18 23:49:26  tristan
 *   updated comments
 *
 *   Revision 1.1  2004/01/18 23:39:05  tristan
 *   initial
 *
 */

/**
 * Manages a server's connection with the client
 *
 * @author Tristan O'Tierney
 * @author Jordan Sissel
 * @author Nick Johnson
 */
public abstract class Server extends Thread {
    private BSServer server;

    /**
     * The server that manages this server connection.
     * @param server The parent server.
     */
    public Server( BSServer server ) {
        this.server = server;
    }

    /**
     * Returns the server object.
     * @return The parent server.
     */
    public BSServer getServer() {
        return server;
    }

    /**
     * Sets the parent server.
     * @param server The new parent server.
     */
    public void setServer( BSServer server ) {
        this.server = server;
    }

    /**
     * Forces a disconnect if this object is garbage collected.
     */
    protected void finalize() {
        disconnect();
    }

    // abstract methods.
    public abstract void disconnect();
}   // Server