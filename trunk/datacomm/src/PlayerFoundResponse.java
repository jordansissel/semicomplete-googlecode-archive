/*
 * PlayerFoundResponse.java
 *
 * Version:
 *    $Id$
 *
 * Revisions:
 *    $Log$
 *    Revision 1.3  2004/01/20 08:51:42  tristan
 *    added constructor
 *
 *    Revision 1.2  2004/01/20 04:23:33  tristan
 *    updated comments for everything.
 *
 *    Revision 1.1  2004/01/20 04:10:32  psionic
 *    - initial commit
 *
 *
 */

/**
 * A response object for synchronizsing two players.
 *
 * @author Tristan O'Tierney
 * @author Jordan Sissel
 * @author Nick Johnson
 */
public class PlayerFoundResponse extends Response {
	private String playername;

    /**
     * Initializes the player found response id.
     */
    public PlayerFoundResponse() {
        super( ProtocolConstants.PLAYERFOUND );
    }

    /**
     * Sets the players name.
     * @param p The name of the player that the other is playing against.
     */
	public void setPlayerName( String p ) {
		playername = p;
		this.setMessage( p );
	}
}
