/*
 * UDPServer.java
 * 
 * Version:
 *   $Id$
 *
 * Revisions:
 *   $Log$
 *   Revision 1.3  2004/01/20 08:30:36  tristan
 *   added basic disconnect method.
 *
 *   Revision 1.2  2004/01/20 04:23:33  tristan
 *   updated comments for everything.
 *
 *   Revision 1.1  2004/01/19 20:43:26  tristan
 *   refactored to a new name
 *
 *   Revision 1.2  2004/01/18 23:48:57  tristan
 *   updated comments
 *
 *   Revision 1.1  2004/01/18 23:42:38  tristan
 *   initial
 *
 */

/**
 * Implementation of a udp based connection with the client
 *
 * @author Tristan O'Tierney
 * @author Jordan Sissel
 * @author Nick Johnson
 */
public class UDPServer extends Server {

    /**
     * The server that manages this server.
     * @param server
     */
    public UDPServer( BSServer server ) {
        super( server );
    }

    /**
     * Disconnects the client from this connection.
     */
    public void disconnect() {

    }
}   // UDPServer