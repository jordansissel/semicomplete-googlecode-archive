/*
 * BattleshipPlayer.java
 * 
 * Version:
 *   $Id$
 *
 * Revisions:
 *   $Log$
 *   Revision 1.7  2004/01/19 20:48:08  tristan
 *   changes due to refactoring
 *
 *   Revision 1.6  2004/01/19 02:34:42  tristan
 *   refactored to a text client
 *
 *   Revision 1.5  2004/01/19 02:18:56  tristan
 *   added game var
 *
 *   Revision 1.4  2004/01/19 00:13:00  tristan
 *   cleaned up and switched to thread
 *
 *   Revision 1.3  2004/01/19 00:05:51  tristan
 *   fixed protocol specification
 *
 *   Revision 1.2  2004/01/18 23:35:09  tristan
 *   basic usage parsing done
 *
 *   Revision 1.1  2004/01/10 22:39:24  tristan
 *   initial
 *
 */

import java.net.InetAddress;

/**
 * Starts a battleship player client.
 * 
 * @author tristan
 */
public class BattleshipPlayer {

    /**
     * Starts up the program.
     * @param args The command line arguments.
     */
    public static void main( String[] args ) {
        if ( args.length == 5 ) {
            try {
                InetAddress host = InetAddress.getByName( args[ 0 ] );
                Client connection = null;
                int port = Integer.parseInt( args[ 2 ] );

                // figure out the protocol
                if ( args[ 1 ].equals( "TCP" ) ) {
                    connection = new TCPClient( host, port );
                } else if ( args[ 1 ].equals( "UDP" ) ) {
                    connection = new UDPClient( host, port );
                } else {
                    System.err.println( "Protocol must be UDP or TCP" );
                    System.exit( 3 );
                }

                // make the client
                BSClient client = new TextClient( connection,
                                                  args[ 3 ],
                                                  args[ 4 ] );
                client.start();
            } catch ( NumberFormatException e ) {
                System.err.println( "TCP or UDP port must be a number" );
            } catch ( Exception e ) {
                System.err.println( e.getMessage() );
                System.exit( 2 );
            }
        } else {
            System.err.println( "Usage: java BattleshipPlayer <host> " +
                                "<protocol> <port> <yourName> " +
                                "<otherName>" );
            System.exit( 1 );
        }
    }
}   // BattleshipPlayer
