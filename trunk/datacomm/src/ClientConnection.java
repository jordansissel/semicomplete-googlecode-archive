/*
 * ClientConnection.java
 *
 * Version:
 *   $Id$
 *
 * Revisions:
 *   $Log$
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


}   // ClientConnection
