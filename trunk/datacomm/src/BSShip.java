/**
 * BSShip.java
 *
 * Version:
 *   $Id$
 *
 * Revisions:
 *   $Log$
 *   Revision 1.11  2004/01/20 02:07:05  tristan
 *   added stubs for comments
 *
 *   Revision 1.10  2004/01/19 23:19:47  tristan
 *   switched back and wrote getter/setters
 *
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
    private byte startX;
    private byte startY;
    private byte endX;
    private byte endY;

    /**
     *  A default constructor that makes a ship at A, 1.
     *
     *
     */
    public BSShip() {
        startX = 0;
        startY = 0;
        endX = 0;
        endY = 0;
    } // constructor

    /**
     *
     * @param startX
     * @param startY
     * @param endX
     * @param endY
     */
    public BSShip( byte startX, byte startY, byte endX, byte endY ) {
        this.startX = startX;
        this.startY = startY;
        this.endX = endX;
        this.endY = endY;
    }

    /**
     *
     * @return
     */
    public byte getLength() {
        byte retVal = 0;

        if ( startX == endX ) {
            retVal = ( byte )
                     ( Math.max( startY, endY ) - Math.min( startY, endY ) );
        } else {
           retVal = ( byte )
                     ( Math.max( startX, endX ) - Math.min( startX, endX ) );
        }

        return retVal;
    }

    /**
     *
     * @return
     */
    public byte getStartX() {
        return startX;
    }

    /**
     *
     * @param startX
     */
    public void setStartX( byte startX ) {
        this.startX = startX;
    }

    /**
     *
     * @return
     */
    public byte getStartY() {
        return startY;
    }

    /**
     *
     * @param startY
     */
    public void setStartY( byte startY ) {
        this.startY = startY;
    }

    /**
     *
     * @return
     */
    public byte getEndX() {
        return endX;
    }

    /**
     *
     * @param endX
     */
    public void setEndX( byte endX ) {
        this.endX = endX;
    }

    /**
     *
     * @return
     */
    public byte getEndY() {
        return endY;
    }

    /**
     *
     * @param endY
     */
    public void setEndY( byte endY ) {
        this.endY = endY;
    }
} // BSShip


