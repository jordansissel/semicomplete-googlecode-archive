/**
 * BSGame.java
 *
 * Version:
 *   $Id$
 * 
 * Revisions:
 *   $Log$
 *   Revision 1.17  2004/01/20 04:23:33  tristan
 *   updated comments for everything.
 *
 *   Revision 1.16  2004/01/20 03:18:35  tristan
 *   very close to finished.
 *
 *   Revision 1.15  2004/01/20 02:58:13  tristan
 *   minor commenting changes.
 *
 *   Revision 1.14  2004/01/19 23:41:34  tristan
 *   fixed references to BSGrid
 *
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

/**
 * The BattleShip game, based of the Milton Bradley Rules.
 * This will be usable with the BattleshipPlayer and 
 * BattleShipServer for networked games.
 *
 * @author Tristan O'Tierney
 * @author Jordan Sissel
 * @author Nick Johnson
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

    /**
     * Constructor of BSGame.
     *
     */
    public BSGame() {
        ownGrid = new BSGrid();
        ownGrid.generateGrid(); 
        targetGrid = new BSGrid();
        shipSunk = new boolean[ BSGrid.SHIP_COUNT ];
        for ( int i = 0; i < shipSunk.length; i++ ) {
            shipSunk[ i ] = false;
        }
    } // BSGame()

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

            startX = the_ship.getStartX();
            startY = the_ship.getStartY();
            endX = the_ship.getEndX();
            endY = the_ship.getEndY();

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
                        if( ownGrid.getGrid()[startY+p][startX] != 3 ) {
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
                        if( ownGrid.getGrid()[startY][startX-p] != 3 ) {
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
                        if( ownGrid.getGrid()[startY-p][startX] != 3 ) {
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
                        if(  ownGrid.getGrid()[startY][startX+p] != 3 ) {
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
} // BSGame

