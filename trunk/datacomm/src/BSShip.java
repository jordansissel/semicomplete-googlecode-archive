/**
 * BSShip.java
 *
 * Version:
 *   $Id$
 *
 * Revisions:
 *   $Log$
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
	private byte[] location = new byte[4];	

	public BSShip( byte startPointX, 
			byte startPointY,		
			byte endPointX,
			byte endPointY ) {

		location[ 0 ] = startPointX;
		location[ 1 ] = startPointY;
		location[ 2 ] = endPointX;
		location[ 3 ] = endPointY;

	} // constructor

	/**
	 * Returns a 2-byte byte array with
	 * the starting point of the ship.
	 * Format: { <row>, <column> }
	 * Where <row> is the number associated with the letter of 
	 * the grid's row ( 10 - A, 11 - B, ... , 19 - I, 20 - J )
	 * and <column> is the number of the grid's column. 
	 *
	 * @return 
	 */
	public byte[] getStart() {
		byte[]	startLocation = new byte[2];	
		startLocation[ 0 ] = location[ 0 ];
		startLocation[ 1 ] = location[ 1 ];
		return startLocation;
	} // getStart()

	/**
	 * Returns a 2-byte byte arrays with
	 * the ending point of the ship.
	 * Format: { <row>, <column> }
	 * Where <row> is the number associated with the letter of 
	 * the grid's row ( 10 - A, 11 - B, ... , 19 - I, 20 - J )
	 * and <column> is the number of the grid's column. 
	 *
	 * @return
	 */
	public byte[] getEnd() {
		byte[] endLocation = new byte[2];
		endLocation[ 0 ] = location[ 2 ];
		endLocation[ 1 ] = location[ 3 ];
		return endLocation;
	} //getEnd()

} //BSShip


