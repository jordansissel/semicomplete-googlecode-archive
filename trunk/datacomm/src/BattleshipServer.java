/*
 * BattleshipServer.java
 *
 * Version:
 *   $Id$
 *
 * Revisions:
 *   $Log$
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

import java.net.*;
import java.io.IOException;
import java.util.HashMap;

/**
 * The main method that starts the battle ship server.
 *
 * @author Tristan O'Tierney
 * @author Jordan Sissel
 * @author Nick Johnson
 */
public class BattleshipServer extends Thread {
    private InetAddress host;
    private int tcpPort;
    private int udpPort;
	private ServerSocket tcplisten;
	private DatagramSocket udplisten;

	 private HashMap playerwants = new HashMap();

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

    /**
     * Initialize a battle ship server.
     * @param host The host to connect to.
     * @param tcpPort The TCP port to use.
     * @param udpPort The UDP port to use.
     * @exception IllegalArgumentException if tcp is the same as udp.
     */
    public BattleshipServer( InetAddress host,
									  int tcpPort,
									  int udpPort ) throws IllegalArgumentException,
									                       IOException {
		this.host = host;
		this.tcpPort = tcpPort;
		this.udpPort = udpPort;
		tcplisten = new ServerSocket(tcpPort);
		//udplisten = new DatagramSocket(udpPort);
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

	 public String playerWants() {
		 if (playerwants.containsKey(player1)) {
			 return playerwants.get(player1);
		 }
		 return null;
	 }

    /**
     * Starts the server's tcp and udp servers.
     */
    public void run() {
		 try {
			 tcplisten.setSoTimeout(1);
		 } catch (SocketException e) {
			 System.err.println("Error trying to set accept() timeout on server's tcp listener socket. " + e);
		 }

		 while (true) {
			 // Look for attempts to connect to tcp or udp
			 try {
				 Socket newconn = tcplisten.accept();
				 Thread newclient = new TCPServer(newconn, this);
				 newclient.start();
			 } catch (SocketTimeoutException e) {
				 // ignore
			 } catch (IOException e) {
				 System.err.println("Error trying to accept incoming connection on tcp port. " + e);
			 }

		 }

	 }

    /**
     * Shutdown tcp and udp connections.
     */
    public void finalize() {

    }

}   // BattleshipServer
