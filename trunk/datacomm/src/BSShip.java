/**
 * BSShip.java
 *
 * Version:
 *   $Id$
 *
 * Revisions:
 *   $Log$
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

	public byte[] getStart() {
		byte[]	startLocation = new byte[2];	
		startLocation[ 0 ] = location[ 0 ];
		startLocation[ 1 ] = location[ 1 ];
		return startLocation;
	} // getStart()

	public byte[] getEnd() {
		byte[] endLocation = new byte[2];
		endLocation[ 0 ] = location[ 2 ];
		endLocation[ 1 ] = location[ 3 ];
		return endLocation;
	} //getEnd()

} //BSShip


