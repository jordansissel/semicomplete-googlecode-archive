/**
 * BSShip.java
 *
 * Version:
 *   $Id$
 *
 * Revisions:
 *   $Log$
 *   Revision 1.9  2004/01/19 21:49:05  tristan
 *   converted to row/col/length
 *
 *   Revision 1.8  2004/01/19 04:27:36  njohnson
 *   All stubs written. Time to test.
 *
 *   Revision 1.7  2004/01/19 02:06:04  njohnson
 *   fixed some javadoc mistakes
 *
 *   Revision 1.6  2004/01/19 00:35:46  njohnson
 *   Committing Compilable code. Added a getGridArray() funtion for grids
 *   which allows the array to be accessed directly for game grids.
 *
 *   Revision 1.5  2004/01/18 23:02:48  njohnson
 *   added some important stubs that need to be implemented.
 *
 *   Revision 1.4  2004/01/13 02:14:24  njohnson
 *   I finished the BSGrid's generateGrid() method which is now quite huge.
 *   It might be better with some AI( or worse ) since it just tries to throw
 *   ships down randomly and checks for overlaps.
 *   I also added a default constructor to BSShip to ease my mind.
 *
 *   Revision 1.3  2004/01/12 23:33:02  njohnson
 *   Almost completed with BSShip. Just need to finish commenting.
 *
 *   Revision 1.2  2004/01/12 04:51:42  njohnson
 *   I made the code compilation error free. Still mostly stubs.
 *
 *   Revision 1.1  2004/01/12 03:29:02  njohnson
 *   initial commit
 *
 *
 */

/**
 * A class that contains the endpoints of a ship in 
 * a BattleShip fleet
 *
 * @author Nicholas R. Johnson
 */
public class BSShip {
    private byte row;
    private byte col;
    private byte length;

    /**
     *  A default constructor that makes a ship at A, 1.
     *
     *
     */
    public BSShip() {
        row = 0;
        col = 0;
        length = 0;
    } // constructor

    /**
     *
     * @param row
     * @param col
     * @param length
     */
    public BSShip( byte row, byte col, byte length ) {
        this.row = row;
        this.col = col;
        this.length = length;
    } // constructor

    /**
     *
     * @return
     */
    public byte getRow() {
        return row;
    }

    /**
     *
     * @param row
     */
    public void setRow( byte row ) {
        this.row = row;
    }

    /**
     *
     * @return
     */
    public byte getCol() {
        return col;
    }

    /**
     *
     * @param col
     */
    public void setCol( byte col ) {
        this.col = col;
    }

    /**
     *
     * @return
     */
    public byte getLength() {
        return length;
    }

    /**
     *
     * @param length
     */
    public void setLength( byte length ) {
        this.length = length;
    }
} //BSShip


