/*
 * ServerGame.java
 *
 * Version:
 *    $Id$
 *
 * Revisions:
 *    $Log$
 *    Revision 1.1  2004/01/19 23:52:28  psionic
 *    - initial commit
 *
 *
 */

import java.net.*;

public class ServerGame {
	private String gameName;
	private Player player1;
	private Player player2;
	private BSGrid board;

	private class Player {
		private String name;
	}

	private class TCPPlayer extends Player {
		private Socket sock;
	}

	private class UDPPlayer extends Player {
		private DatagramSocket sock;
	}

	public ServerGame() {
		board = new BSGrid();
		player1 = null;
		player2 = null;
	}
}
