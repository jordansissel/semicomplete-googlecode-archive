/**
 * BSGame.java
 *
 * Version:
 *   $Id$
 * 
 * Revisions:
 *   $Log$
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
	private BSGrid ownGrid;
	private BSGrid targetGrid;
	private String firstName;
	private String secondName;

	public BSGame( String firstName, 
			String secondName,	
			BSGrid ownGrid;
			BSGrid targetGrid; ) {


	} // BSGame()

	public initializeGrid( BSGrid the_grid ) {

	} //initializeGrid()

	public getOwnGrid() {

	} //getOwnGrid()

	public getTargetGrid() {

	} //getTargetGrid

	/**
	 * The main method the runs a game locally.
	 *
	 *
	 */
	public static void main( String[] args ) {
		String input_string = new String();

		//prompt for the names of players
		System.out.println( "What is the name of player one: " );

		System.out.println( "What is the name of player two: " );


		//Generate the grids for each player	

		//play the game. player one gets the first shot


	} //main()

} // BSGame

