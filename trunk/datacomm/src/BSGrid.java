/**
 * BSGrid.java
 *
 * Version:
 *     $Id$
 *
 * Revisions:
 *     $Log$
 *     Revision 1.21  2004/01/19 23:20:06  tristan
 *     changed grid generation
 *
 *     Revision 1.20  2004/01/19 21:38:35  tristan
 *     fixed more constants
 *
 *     Revision 1.19  2004/01/19 21:32:04  tristan
 *     fixed constants
 *
 *     Revision 1.18  2004/01/19 07:25:46  njohnson
 *     still working on ArrayOutofBounds
 *
 *     Revision 1.11  2004/01/18 23:02:48  njohnson
 *     added some important stubs that need to be implemented.
 *
 *     Revision 1.10  2004/01/13 23:04:20  njohnson
 *     fixed a couple bugs
 *     
 *     Revision 1.8  2004/01/13 03:46:49  tristan
 *     removed useless import
 *     
 *     Revision 1.7  2004/01/13 02:34:33  tristan
 *     fixed up to string with string buffer.
 *     
 *     Revision 1.6  2004/01/13 02:14:23  njohnson
 *     I finished the BSGrid's generateGrid() method which is now quite huge.
 *     It might be better with some AI( or worse ) since it just tries to throw
 *     ships down randomly and checks for overlaps.
 *     I also added a default constructor to BSShip to ease my mind.
 *
 *     Revision 1.5  2004/01/12 23:21:38  njohnson
 *     completed the toString() method so that Grids can easily be
 *     printed to the terminal
 *
 *     Revision 1.4  2004/01/12 21:49:15  njohnson
 *     added a toString method, so we can print the grids and translate them
 *     easily to pass over the network.
 *
 *     Revision 1.3  2004/01/12 04:51:41  njohnson
 *     I made the code compilation error free. Still mostly stubs.
 *
 *     Revision 1.1  2004/01/12 02:29:50  njohnson
 *     initial commit
 *
 */

/**
 * The grid objects to represent oceans for each player in BattleShip
 *
 * @author Nicholas R. Johnson - nrj7604
 */
public class BSGrid {
    public static final int SHIP_COUNT = 5;
    public static final int ROW_COUNT = 10;
    public static final int COL_COUNT = 10;

    private BSShip[] fleet;
    private byte[][] grid;

    /**
     *
     */
    public BSGrid() {
        fleet = new BSShip[ SHIP_COUNT ];
        grid = new byte[ ROW_COUNT ][ COL_COUNT ];
    } // constructor

    /**
     *
     */
    public void generateGrid() {
        byte[] length = new byte[] { 2, 3, 3, 4, 5 };

        for( int i = 0; i < SHIP_COUNT; i++ ) {
            // keep looping until the overlap is gone
            BSShip ship = new BSShip();

            do {
                // generate start
                ship.setStartX( ( byte ) ( Math.random() * ROW_COUNT ) );
                ship.setStartY( ( byte ) ( Math.random() * COL_COUNT ) );

                // generate direction
                int xDir = ( ( Math.random() * 2 ) == 0 ) ? -1 : 1;
                int yDir = ( ( Math.random() * 2 ) == 0 ) ? -1 : 1;

                // check directions
                int tempY =  ship.getStartY() + ( length[ i ] * yDir );
                if ( tempY < 0 ) {
                    yDir = 1;
                } else if ( yDir > ROW_COUNT ) {
                    yDir = -1;
                }

                int tempX = ship.getStartX() + ( length[ i ] * xDir );
                if ( tempX < 0 ) {
                    xDir = 1;
                } else if ( tempX > COL_COUNT ) {
                    xDir = -1;
                }

                // calculate endx and endy
                ship.setEndX( ( byte ) (
                              ship.getStartX() + ( length[ i ] * xDir ) ) );
                ship.setEndY( ( byte )
                              ( ship.getStartY() + ( length[ i ] * yDir ) ) );
            } while ( findOverlap( ship ) );

            // set the ship
            fleet[ i ] = ship;
        }
    } // generateGrid()

    /**
     *
     * @param newGrid
     */
    public void setGrid( byte[][] newGrid ) {
        grid = newGrid;
    }

    /**
     *
     * @param x1
     * @param y1
     * @param x2
     * @param y2
     * @param x3
     * @param y3
     * @return
     */
    private boolean ccw( byte x1, byte y1,
                         byte x2, byte y2,
                         byte x3, byte y3 ) {
        float dx1, dx2, dy1, dy2;

        dx1 = x2 - x1;
        dy1 = y2 - y1;
        dx2 = x3 - x2;
        dy2 = y3 - y2;

        return dy1 / dx1 < dy2 / dx2;
    }

    /**
     *
     * @param x1
     * @param y1
     * @param x2
     * @param y2
     * @param x3
     * @param y3
     * @param x4
     * @param y4
     * @return
     */
    private boolean opposite( byte x1, byte y1,
                              byte x2, byte y2,
                              byte x3, byte y3,
                              byte x4, byte y4 ) {
        return ccw( x1, y1, x2, y2, x3, y3 ) !=
               ccw( x1, y1, x2, y2, x4, y4 );
    }

    /**
     *
     * @param x1
     * @param y1
     * @param x2
     * @param y2
     * @param x3
     * @param y3
     * @param x4
     * @param y4
     * @return
     */
    private boolean intersect( byte x1, byte y1,
                               byte x2, byte y2,
                               byte x3, byte y3,
                               byte x4, byte y4 ) {
        return opposite( x1, y1, x2, y2, x3, y3, x4, y4 ) &&
               opposite( x3, y3, x4, y4, x1, y1, x2, y2 );
    }

    /**
     *
     * @param ship
     * @return
     */
    private boolean findOverlap( BSShip ship ) {
        boolean retVal = false;

        for ( int i = 0; !retVal &&
                         i < SHIP_COUNT &&
                         fleet[ i ] != null; i++ ) {
            retVal = intersect( fleet[ i ].getStartX(),
                                fleet[ i ].getStartY(),
                                fleet[ i ].getEndX(),
                                fleet[ i ].getEndY(),
                                ship.getStartX(),
                                ship.getStartY(),
                                ship.getEndX(),
                                ship.getEndY() );
        }

        return retVal;
    }

    /**
     * Returns the array of the grid.
     *
     * @return grid
     */
    public byte[][] getGrid() {
        return grid;
    } //getGridArray()

    /**
     * Returns a particular ship from the grid.
     *
     * @param i - the number of the corresponding ship
     *          Destroyer - 1
     *          Submarine - 2 
     *          Cruiser - 3
     *          Battleship - 4
     *          Carrier - 5
     * @return  returns the particular ship from the grid
     */
    public BSShip getShip( int i ) {
        return fleet[ i - 1 ];
    } //getShip()


    /**
     * Creates a string that is the BattleShip grid in
     * human readable form.
     *
     * @return - readable grid
     */
    public String toString() {
        StringBuffer gridString = new StringBuffer();
        int j = 0;

        //add top area
        gridString.append( " 0123456789\n" );

        for( int i = 0; i < ROW_COUNT; i++ ) {
            gridString.append( ( char ) ( 'A' + i ) );
            for( j = 0; j < COL_COUNT; j++ ) {
                if( grid[ i ][ j ] == 0 ) { //empty
                    gridString.append( "." );
                } else if( grid[ i ][ j ] == 1 ) { //ship
                    gridString.append( "0" );
                } else if( grid[ i ][ j ] == 2 ) { //miss
                    gridString.append( "*" );
                } else { //hit
                     gridString.append( "X" );
                }
            }
            gridString.append( "\n" );
        }

        return gridString.toString();
    } //toString()
} // BSGrid


