/*
 * ServerGame.java
 *
 * Version:
 *    $Id$
 *
 * Revisions:
 *    $Log$
 *    Revision 1.2  2004/01/20 03:57:34  psionic
 *    *** empty log message ***
 *
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


	public ServerGame() {
		board = new BSGrid();
		player1 = null;
		player2 = null;
	}

	public setPlayer1(String name) {
		player1 = new Player();

	}

	public setPlayer2(String name) {

	}
}

public class Player {
	private String name;
	private String whowanted;
	
	public Player(String n) {
		name = n;
	}

	public void wants(String who) {
		whowanted = who;
	}

}

public class TCPPlayer extends Player {
	private Socket sock;
	public TCPPlayer(String n, sock s) {
		super(n);
		sock = s;
	}
}

public class UDPPlayer extends Player {
	private DatagramSocket sock;
	public UDPPlayer(String n, sock s) {
		super(n);
		sock = s;
	}
}
