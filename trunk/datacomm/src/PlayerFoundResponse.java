/*
 * PlayerFoundResponse.java
 *
 * Version:
 *    $Id$
 *
 * Revisions:
 *    $Log$
 *    Revision 1.1  2004/01/20 04:10:32  psionic
 *    - initial commit
 *
 *
 */

public class PlayerFoundResponse extends Response {
	private String playername;

	public PlayerFoundResponse() {
		super();
		this.setMessage(new String());
	}

	public void setPlayerName(String p) {
		playername = p;
		this.setMessage(p);
	}
}
