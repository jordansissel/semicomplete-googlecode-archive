/**
 * BSGrid.java
 *
 * Version:
 * 	$Id$
 *
 * Revisions:
 * 	$Log$
 * 	Revision 1.3  2004/01/12 04:51:41  njohnson
 * 	I made the code compilation error free. Still mostly stubs.
 *
 * 	Revision 1.1  2004/01/12 02:29:50  njohnson
 * 	initial commit
 * 	
 */


/**
 * The grid objects to represent oceans for each player in BattleShip
 *
 * @author Nicholas R. Johnson - nrj7604 
 */
public class BSGrid {

	BSShip[] fleet = new BSShip[ 5 ]; 

	public BSGrid( String id ) {


	} // constructor

	public byte[] generateGrid() {
		byte[] newGrid = new byte[10];

		return newGrid;
	} //generateGrid()

} // BSGrid


