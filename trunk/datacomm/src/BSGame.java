/**
 * BSGame.java
 *
 * Version:
 *   $Id$
 * 
 * Revisions:
 *   $Log$
 *   Revision 1.13  2004/01/19 05:47:43  njohnson
 *   fixed some NullPointerExcpeptions. fixed shipLength problem is BSGrid.java
 *
 *   Revision 1.12  2004/01/19 04:27:36  njohnson
 *   All stubs written. Time to test.
 *
 *   Revision 1.10  2004/01/19 02:14:39  njohnson
 *   the constructor redone. BSGame does not keep track of names[
 *
 *   Revision 1.9  2004/01/19 02:09:14  njohnson
 *   fixed some replace mistakes
 *
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
    private BSGrid ownGrid;
    private BSGrid targetGrid;
    private boolean[] shipSunk; //true if the ship is sunk.
				// 0 - Destroyer
				// 1 - Submarine
				// 2 - Cruiser
				// 3 - Battleship
				// 4 - Carrier

    static char[] horz_coords = 
    { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J' };

    /**
     * Constructor of BSGame.
     *
     */
    public BSGame() {
        ownGrid = new BSGrid();
        ownGrid.generateGrid(); 
        targetGrid = new BSGrid();
	this.clearGrid( targetGrid );
        shipSunk = new boolean[ 5 ];
        for( int i = 0; i < shipSunk.length; i++ ) {
            shipSunk[ i ] = false;
	}

    } // BSGame()


    /**
     * Clears the bytes of a grid all to zero.
     *
     * @param the_grid - the grid that is cleared
     */
    public void clearGrid( BSGrid the_grid ) {
        byte[][] new_grid = new byte[10][10];
        the_grid.generateGrid( new_grid );
    } //clearGrid()

    /** 
     * Alters the byte at a point on the grid given
     *
     * @param the_grid - The grid to be altered.
     * @param x - letter A-J representing horizontal coord
     * @param i - integer 1-10 representing vertical coord
     * @param hit - true for hit, false for miss
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
    } //shot()

    /**
     * Checks whether a particular ship on ownGrid 
     * is sunk.
     *
     * @param i - the number mapping to a particular ship
     *          1 - Destroyer
     *          2 - Submarine
     *          3 - Cruiser
     *          4 - Battleship
     *          5 - Carrier
     */
    public boolean shipSunk( int i )  {
        boolean retVal = false;
        int dir = 0; //direction of ship

        //first check the shipSunk array
	if( shipSunk[ i - 1 ] ) {
            retVal = true;
	} else { //calculate whether the ship was sunk
            BSShip the_ship; // the ship to test
            int startX; //the letter coord
	    int startY; //the number coord
            int endX;
	    int endY;

            //get the ship from the fleet
            the_ship = ownGrid.getShip( i );

            int shipL = the_ship.getLength();

            startX = the_ship.getStart()[ 0 ];
	    startY = the_ship.getStart()[ 1 ];
	    endX = the_ship.getEnd()[ 0 ];
	    endY = the_ship.getEnd()[ 1 ];



	    //set the direction
            if( startY > endY ) { 
		dir = 1;
	    } else if( startX > endX ) {
                dir = 2;
	    } else if( startX < endX ) {
		dir = 4;
	    } else {
                dir = 3;
	    }

	    //a variable for the for loops
	    int p = 0;
            //variable to break out of switch
	    //and the ship is not Sunk!
            boolean notSunk = false;

	    //run through test
            switch( dir ) {
                case 1:
                    for( p = 0; p < shipL; p++ ) {
			if( ownGrid.getGridArray()[startY+p][startX] != 3 ) {
                           notSunk = true; 
			}
			if( notSunk ) {
                            break;
			}
                    }
		    if( notSunk ) {
                        break;
		    }
                    retVal = true;
                    break;
                case 2:
                    for( p = 0; p < shipL; p++ ) {
                        if( ownGrid.getGridArray()[startY][startX-p] != 3 ) {
                           notSunk = true;
			}
			if( notSunk ) {
                            break;
			}
                    }
		    if( notSunk ) {
                        break;
		    }
                    retVal = true;
                    break;
                case 3:
                    for( p = 0; p < shipL; p++ ) {
                        if( ownGrid.getGridArray()[startY-p][startX] != 3 ) {
                            notSunk = true;
			}
			if( notSunk ) {
			    break;
			}
                    }
		    if( notSunk ) {
                        break; 
		    }
		    retVal = true;
                    break;
                case 4:
                    for( p = 0; p < shipL; p++ ) {
                        if(  ownGrid.getGridArray()[startY][startX+p] != 3 ) {
                            notSunk = true;
			}
			if( notSunk ) {
                            break;
			}
                    }
		    if( notSunk ) {
                        break; 
		    }
		    retVal = true;
		    break;
            } //switch

        }

        return retVal;
    } //shipSunk()
     
    /**
     * The main method that tests stuff.
     *
     * @args - player one's name
     *         player two's name
     *       type of game?
     */
    public static void main( String[] args ) {
        //Generate a game
        BSGame test_game = new BSGame();


    } //main()

} // BSGame

