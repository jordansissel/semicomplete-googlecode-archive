/*
 * BattleshipPlayer.java
 * 
 * Version:
 *   $Id$
 *
 * Revisions:
 *   $Log$
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
public class BattleshipPlayer extends Thread {

    // member variables
    private ClientConnection connection;
    private String yourName;
    private String otherName;
    private BSGame game;

    /**
     * Initializes the battleship player
     * @param connection The client's connection to the server.
     * @param yourName This client's name.
     * @param otherName The client's opposition.
     */
    public BattleshipPlayer( ClientConnection connection,
                             String yourName,
                             String otherName ) {
        this.connection = connection;
        this.yourName = yourName;
        this.otherName = otherName;
        this.game = new BSGame();
    }

    /**
     * Starts up the program.
     * @param args The command line arguments.
     */
    public static void main( String[] args ) {
        if ( args.length == 5 ) {
            try {
                InetAddress host = InetAddress.getByName( args[ 0 ] );
                ClientConnection connection = null;
                int port = Integer.parseInt( args[ 2 ] );

                // figure out the protocol
                if ( args[ 1 ].equals( "TCP" ) ) {
                    connection = new TCPClientConnection( host, port );
                } else if ( args[ 1 ].equals( "UDP" ) ) {
                    connection = new UDPClientConnection( host, port );
                } else {
                    System.err.println( "Protocol must be UDP or TCP" );
                    System.exit( 3 );
                }

                // make the client
                BattleshipPlayer client = new BattleshipPlayer(
                    connection,
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

    /**
     * Starts up the battle ship client
     */
    public void run() {
        if ( connection.connect() ) {
            // send a start game message

            // block until server says opponent found


            while ( connection.isConnected() ) {

                // talk to server
            }
        } else {
            System.out.println( "Failing to connect to server" );
        }
    }

    /**
     * Returns the client connection for this player.
     * @return The player's connection to the server.
     */
    public ClientConnection getConnection() {
        return connection;
    }

    /**
     * Sets the connection for the server.
     * @param connection Changes the connection for the client.
     */
    public void setConnection( ClientConnection connection ) {
        this.connection = connection;
    }

    /**
     * Gets the name of this player.
     * @return The player's name.
     */
    public String getYourName() {
        return yourName;
    }

    /**
     * Sets the player's name.
     * @param yourName The name for this player.
     */
    public void setYourName( String yourName ) {
        this.yourName = yourName;
    }

    /**
     * Returns the other player's name.
     * @return The other player's name.
     */
    public String getOtherName() {
        return otherName;
    }
}   // BattleshipPlayer
