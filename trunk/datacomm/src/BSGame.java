/**
 * BSGame.java
 *
 * Version:
 *   $Id$
 * 
 * Revisions:
 *   $Log$
 *   Revision 1.7  2004/01/19 00:35:46  njohnson
 *   Committing Compilable code. Added a getGridArray() funtion for grids
 *   which allows the array to be accessed directly for game grids.
 *
 *   Revision 1.5  2004/01/18 23:02:48  njohnson
 *   added some important stubs that need to be implemented.
 *
 *   Revision 1.4  2004/01/12 23:20:51  njohnson
 *   Stubs almost complete for client to use
 *
 *   Revision 1.3  2004/01/12 04:51:41  njohnson
 *   I made the code compilation error free. Still mostly stubs.
 *
 *   Revision 1.2  2004/01/12 04:14:23  njohnson
 *   Finalized basic game model. BSGame will be the game for each client
 *   respective to their point of view. Therefore, the main method
 *   invoked to create the local game will instantiate two BSGame methods
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
    private BSGrid ownGrid;
    private BSGrid targetGrid;
    
    static char[] horz_coords = 
    { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J' };

    /**
     * Constructor of BSGame.
     *
     * @arg ownName -
     * @arg targetName -
     * @arg ownGrid -
     * @arg targetGrid -
     */
    public BSGame( String ownName, 
            String targetName,    
            BSGrid ownGrid,
            BSGrid targetGrid ) {
        this.ownName = ownName;
        this.targetName = targetName;
        this.ownGrid = ownGrid;
        this.targetGrid = targetGrid;
    } // BSGame()

    public void initializeGrid( BSGrid the_grid ) {
        the_grid.generateGrid();
    } //initializeGrid()

    /**
     * Clears the bytes of a grid all to zero.
     *
     * @arg the_grid - the grid that is cleared
     */
    public void clearGrid( BSGrid the_grid ) {
        byte[][] new_grid = new byte[10][10];
        the_grid.generateGrid( new_grid );
    } //clearGrid()

    /** 
     * Alters the byte at a point on the grid given
     *
     * @arg the_grid - The grid to be altered.
     * @arg x - letter A-J representing horizontal coord
     * @arg i - integer 1-10 representing vertical coord
     * @arg hit - true for hit, false for miss
     */
    public void alterGrid( BSGrid the_grid, char x, int i, boolean hit ) {
        int j = -1;
        byte new_byte = 0;

	//go from characters to numbers to work with array
	for( int k = 0; k < horz_coords.length; k++ ) {
            if( x == horz_coords[ k ] ) {
                  j = k; 
	    }
	}

	//decrement i once to work with the array
        i = i - 1; 


	//set new_byte to the correct value
	if( hit ) {
            new_byte = 4;
	} else {
            new_byte = 2;
	}

	//set the byte in the grid
        the_grid.getGridArray()[ j ][ i ] = new_byte;

    } //alterGrid()

    /**
     * Returns the player who owns this game's own grid.
     *
     * @return - ownGrid
     */
    public BSGrid getOwnGrid() {
        return ownGrid;
    } //getOwnGrid()

    /**
     * Returns the player who owns this game's target grid.
     *
     * @return - the target grid
     */
    public BSGrid getTargetGrid() {
        return targetGrid;
    } //getTargetGrid()

    /**
     * Determines whether a shot is a hit or miss
     * on ownGrid.
     *
     * @return    true - if hit
     *        false - if miss
     *
     * @throws AlreadyShotThereException
     */
    public boolean shot( char x, int i ) {
        boolean retVal = false;
        int j = 0;  
      
	//go from characters to numbers to work with array
	for( int k = 0; k < horz_coords.length; k++ ) {
            if( x == horz_coords[ k ] ) {
                  j = k; 
	    }
	}

        i = i - 1;

	//check for a hit
        if( ownGrid.getGridArray()[ j ][ i ] == 1 ) {
            retVal = true;
        } else if( ownGrid.getGridArray()[ j ][ i ] == 0 ) {
            retVal = false;
        } else {
            //throw the exception
	}

        return retVal;
    }

    /**
     * The main method the runs a game locally.
     *
     * @args - player one's name
     *         player two's name
     *       type of game?
     */
    public static void main( String[] args ) {
        String input_string = new String();

        //starts two games, one for each player

        //Generate the grids for each player    

        //play the game. player one gets the first shot


    } //main()

} // BSGame

