/*
 * BattleshipServer.java
 *
 * Version:
 *   $Id$
 *
 * Revisions:
 *   $Log$
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
 * @author tristan
 */
public class BattleshipServer {
    private InetAddress host;
    private int tcpPort;
    private int udpPort;

    /**
     * Starts up the program.
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
                BattleshipServer server = new BattleshipServer( host,
                                                                tcp,
                                                                udp );
                server.start();
            } catch ( NumberFormatException e ) {
                System.err.println( "TCPprt and UDPport must be numbers" );
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

    /**
     * Initialize a battle ship server.
     * @param host The host to connect to.
     * @param tcpPort The TCP port to use.
     * @param udpPort The UDP port to use.
     * @exception IllegalArgumentException if tcp is the same as udp.
     */
    public BattleshipServer( InetAddress host,
                             int tcpPort,
                             int udpPort ) throws IllegalArgumentException {
        if ( tcpPort == udpPort ) {
            throw new IllegalArgumentException( "TCP and UDP ports must " +
                                                "differ" );
        }

        // set vars
        this.host = host;
        this.tcpPort = tcpPort;
        this.udpPort = udpPort;
    }

    /**
     * Returns the inet address this battleship server is bound to.
     * @return The server's inet address.
     */
    public InetAddress getHost() {
        return host;
    }

    /**
     * Returns the tcp port for this server.
     * @return The tcp port.
     */
    public int getTcpPort() {
        return tcpPort;
    }

    /**
     * Sets the tcp port this server should use.
     * @param tcpPort The server's tcp port.
     */
    public void setTcpPort( int tcpPort ) {
        this.tcpPort = tcpPort;
    }

    /**
     * Returns the server's UDP port.
     * @return The server's udp port.
     */
    public int getUdpPort() {
        return udpPort;
    }

    /**
     * Changes the server's udp port.
     * @param udpPort The server's new udp port.
     */
    public void setUdpPort( int udpPort ) {
        this.udpPort = udpPort;
    }

    /**
     * Starts the server's tcp and udp servers.
     */
    public void start() {

    }

    /**
     * Shutdown tcp and udp connections.
     */
    public void finalize() {

    }
}   // BattleshipServer
