/**
 * BSGame.java
 *
 * Version:
 *   $Id$
 * 
 * Revisions:
 *   $Log$
 *   Revision 1.2  2004/01/12 04:14:23  njohnson
 *   Finalized basic game model. BSGame will be the game for each client
 *   respective to their point of view. Therefore, the main method
 *   invoked to create the local game with instantiate to BSGame methods
 *   for each player.
 *
 *   Revision 1.1  2004/01/12 02:01:46  njohnson
 *   Initial Commit.
 *
 *
 *
 */

import java.util.*;

/**
 * The BattleShip game, based of the Milton Bradley Rules.
 * This will be usable with the BattleshipPlayer and 
 * BattleShipServer for networked games.
 *
 * @author Nicholas Johnson - nrj7064
 */
public class BSGame {
	private String ownName;
	private String targetName;
	private byte[] misses;
	private BSGrid ownGrid;
	private BSGrid targetGrid;

	public BSGame( String ownName, 
			String targetName,	
			BSGrid ownGrid;
			BSGrid targetGrid; ) {


	} // BSGame()

	public void initializeGrid( BSGrid the_grid ) {

	} //initializeGrid()

	public BSGrid getOwnGrid() {
		return ownGrid;
	} //getOwnGrid()

	public BSGrid getTargetGrid() {
		return targetGrid;
	} //getTargetGrid()

	public void printGrid() {

	} //printGrid()

	/**
	 * The main method the runs a game locally.
	 *
	 * @args
	 *
	 */
	public static void main( String[] args ) {
		String input_string = new String();

		//starts two games, one for each player

		//Generate the grids for each player	

		//play the game. player one gets the first shot


	} //main()

} // BSGame

