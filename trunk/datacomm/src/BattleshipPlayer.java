/*
 * BattleshipPlayer.java
 * 
 * Version:
 *   $Id$
 *
 * Revisions:
 *   $Log$
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
    // constants
    private static final int UDP = 0;
    private static final int TCP = 1;

    // member variables
    private InetAddress host;
    private int protocol;
    private int port;
    private String yourName;
    private String otherName;

    /**
     *
     * @param host The server we're connecting to.
     * @param protocol Which protocol to use for connections.
     * @param port The server's listening port.
     * @param yourName This client's name.
     * @param otherName The client's opposition.
     */
    public BattleshipPlayer( InetAddress host,
                             int protocol,
                             int port,
                             String yourName,
                             String otherName ) {
        this.host = host;
        this.protocol = protocol;
        this.port = port;
        this.yourName = yourName;
        this.otherName = otherName;
    }

    /**
     * Starts up the program.
     * @param args The command line arguments.
     */
    public static void main( String[] args ) {
        if ( args.length == 5 ) {
            try {
                InetAddress host = InetAddress.getByName( args[ 0 ] );
                int protocol = 0;

                // figure out the protocol
                if ( args[ 1 ].equals( "TCP" ) ) {
                    protocol = 1;
                } else if ( !args[ 1 ].equals( "UDP" ) ) {
                    System.err.println( "Protocol must be UDP or TCP" );
                    System.exit( 3 );
                }

                int port = Integer.parseInt( args[ 2 ] );

                // make the client
                BattleshipPlayer client = new BattleshipPlayer(
                    host,
                    protocol,
                    port,
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
    public void start() {

    }

    /**
     *
     * @return
     */
    public InetAddress getHost() {
        return host;
    }

    /**
     *
     * @param host
     */
    public void setHost( InetAddress host ) {
        this.host = host;
    }

    /**
     *
     * @return
     */
    public int getProtocol() {
        return protocol;
    }

    /**
     *
     * @param protocol
     */
    public void setProtocol( int protocol ) {
        this.protocol = protocol;
    }

    /**
     *
     * @return
     */
    public int getPort() {
        return port;
    }

    /**
     *
     * @param port
     */
    public void setPort( int port ) {
        this.port = port;
    }

    /**
     *
     * @return
     */
    public String getYourName() {
        return yourName;
    }

    /**
     *
     * @param yourName
     */
    public void setYourName( String yourName ) {
        this.yourName = yourName;
    }

    /**
     *
     * @return
     */
    public String getOtherName() {
        return otherName;
    }

    /**
     *
     * @param otherName
     */
    public void setOtherName( String otherName ) {
        this.otherName = otherName;
    }
}   // BattleshipPlayer
