/**
 * BSGrid.java
 *
 * Version:
 * 	$Id$
 *
 * Revisions:
 * 	$Log$
 * 	Revision 1.4  2004/01/12 21:49:15  njohnson
 * 	added a toString method, so we can print the grids and translate them
 * 	easily to pass over the network.
 *
 * 	Revision 1.3  2004/01/12 04:51:41  njohnson
 * 	I made the code compilation error free. Still mostly stubs.
 * 	
 * 	Revision 1.1  2004/01/12 02:29:50  njohnson
 * 	initial commit
 * 	
 */

import java.lang.Math;

/**
 * The grid objects to represent oceans for each player in BattleShip
 *
 * @author Nicholas R. Johnson - nrj7604 
 */
public class BSGrid {
	private BSShip[] fleet; 
	private byte[][] grid; 
	private String name;

	public BSGrid( String id ) {
		name = id;
		generateGrid();
	} // constructor

	public void generateGrid() {
		byte[][] newGrid;
		newGrid = new byte[10][10];
		int valueX;
		int valueY;

		valueX = (int)Math.ceil( Math.random()*10 );	

		

		return newGrid;
	} //generateGrid()
	
	public void generateGrid( byte[][] newGrid ) {
		grid = newGrid;
	} //generateGrid()

	public String toString() {

	} //toString()

} // BSGrid


