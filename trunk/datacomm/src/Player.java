/*
 * .java
 *
 * Version:
 *    $Id$
 *
 * Revisions:
 *    $Log$
 *    Revision 1.3  2004/01/26 21:21:27  psionic
 *    - Updated commands framework, added game-starting ability
 *
 *    Revision 1.2  2004/01/20 04:23:33  tristan
 *    updated comments for everything.
 *
 *    Revision 1.1  2004/01/20 03:57:34  psionic
 *    *** empty log message ***
 *
 *
 */

/**
 * A player object for managing two players.
 *
 * @author Tristan O'Tierney
 * @author Jordan Sissel
 * @author Nick Johnson
 */
public class Player {
    private String name;
    private String whowanted;

    /**
     * Creates a player and initializes his name.
     * @param n
     */
    public Player(String n) {
        name = n;
    }

	 public String wanted() {
		 return whowanted;
	 }

    /**
     * Sets who this player wants to play against.
     * @param who
     */
    public void wants(String who) {
        whowanted = who;
    }

	 public String getName() {
		 return name;
	 }
}

