/*
 * TextClient.java
 * 
 * Version:
 *   $Id$
 *
 * Revisions:
 *   $Log$
 *   Revision 1.2  2004/01/19 02:35:25  tristan
 *   moved stuff into text client
 *
 *   Revision 1.1  2004/01/19 02:21:48  tristan
 *   initial
 *
 */

/**
 * Text based Battle ship client
 *  
 * @author tristan
 */
public class TextClient extends BSClient {
    /**
     * Creates a new TextClient
     */
    public TextClient( ClientConnection connection,
                       String yourName,
                       String otherName ) {
        super( connection, yourName, otherName );
    }

    /**
     * Runs the text client by talking to the server
     */
    public void run() {
        if ( getConnection().connect() ) {
            // send a start game message

            // block until server says opponent found


            while ( getConnection().isConnected() ) {

                // talk to server
            }
        } else {
            System.out.println( "Failing to connect to server" );
        }
    }
}   // TextClient