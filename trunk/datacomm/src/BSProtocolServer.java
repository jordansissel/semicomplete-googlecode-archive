/*
 * BSProtocolServer.java
 * 
 * Version:
 *   $Id$
 *
 * Revisions:
 *   $Log$
 *   Revision 1.1  2004/01/20 07:13:08  tristan
 *   wrote basic class
 *
 */

/**
 * Abstract class for a common protocol based battle ship server.
 *
 * @author Tristan O'Tierney
 * @author Jordan Sissel
 * @author Nick Johnson
 */
public abstract class BSProtocolServer extends Thread {
    private BSServer server;

    /**
     * Initializes the bs protocol server.
     * @param server The main server that manages the various protocols.
     */
    public BSProtocolServer( BSServer server ) {
        this.server = server;
    }

    /**
     * Returns the bs server that manages this protocol.
     * @return The BS server for this protocol.
     */
    public BSServer getServer() {
        return server;
    }

    /**
     * Sets the bs server for this protocol.
     * @param server The bs server to set.
     */
    public void setServer( BSServer server ) {
        this.server = server;
    }

    public abstract void shutdown();
}   // BSProtocolServer