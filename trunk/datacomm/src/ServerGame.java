/*
 * ServerGame.java
 *
 * Version:
 *    $Id$
 *
 * Revisions:
 *    $Log$
 *    Revision 1.3  2004/01/20 04:23:33  tristan
 *    updated comments for everything.
 *
 *    Revision 1.2  2004/01/20 03:57:34  psionic
 *    *** empty log message ***
 *
 *    Revision 1.1  2004/01/19 23:52:28  psionic
 *    - initial commit
 *
 *
 */

import java.net.*;

/**
 * Creates a game and associates two clients.
 *
 * @author Tristan O'Tierney
 * @author Jordan Sissel
 * @author Nick Johnson
 */
public class ServerGame {
	private String gameName;
	private Player player1;
	private Player player2;

	public void setPlayer1(String name) {
		player1 = new Player();

	}

	public void setPlayer2(String name) {

	}

	public ServerGame() {
		player1 = null;
		player2 = null;
    }
}

