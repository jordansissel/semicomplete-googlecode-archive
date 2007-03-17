/*
 * BattleshipServer.java
 *
 * Version:
 *   $Id$
 *
 * Revisions:
 *   $Log$
 *   Revision 1.8  2004/01/20 08:34:00  tristan
 *   refactored stuff to BSServer
 *
 *   Revision 1.7  2004/01/20 04:23:33  tristan
 *   updated comments for everything.
 *
 *   Revision 1.6  2004/01/20 03:57:34  psionic
 *   *** empty log message ***
 *
 *   Revision 1.5  2004/01/19 23:48:52  psionic
 *   - Bulk commit: Command infrastructure improved. New commands can be added
 *     dynamically now.
 *
 *   Revision 1.4  2004/01/19 20:54:18  psionic
 *   - Updated server-side happyland framework for TCP
 *
 *   Revision 1.3  2004/01/18 23:10:31  tristan
 *   fixed count
 *
 *   Revision 1.2  2004/01/10 23:22:20  tristan
 *   finished basic template for starting up server
 *
 *   Revision 1.1  2004/01/10 22:38:21  tristan
 *   initial
 *
 */

import java.net.InetAddress;

/**
 * The main method that starts the battle ship server.
 *
 * @author Tristan O'Tierney
 * @author Jordan Sissel
 * @author Nick Johnson
 */
public class BattleshipServer extends Thread {
    /**
     * Starts up the program.
     *
     * @param args The command line arguments.
     */
    public static void main( String[] args ) {
        // make battleship server
        if ( args.length == 3 ) {
            try {
                // initialize the host
                InetAddress host = InetAddress.getByName( args[ 0 ] );

                // check the ports
                int tcp = Integer.parseInt( args[ 1 ] );
                int udp = Integer.parseInt( args[ 2 ] );

                // create the server
                BSServer server = new BSServer( host, tcp, udp );
                server.start();
            } catch ( NumberFormatException e ) {
                System.err.println( "TCPport and UDPport must be numbers" );
                System.exit( 2 );
            } catch ( Exception e ) {
                System.err.println( e.getMessage() );
                System.exit( 3 );
            }
        } else {
            System.err.println( "Usage: java BattleshipServer <host> " +
                    "<TCPport> <UDPport>" );
            System.exit( 1 );
        }
    }
}   // BattleshipServer
