/**
 * BSGrid.java
 *
 * Version:
 * 	$Id$
 *
 * Revisions:
 * 	$Log$
 * 	Revision 1.5  2004/01/12 23:21:38  njohnson
 * 	completed the toString() method so that Grids can easily be
 * 	printed to the terminal
 *
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

		
	} //generateGrid()
	
	public void generateGrid( byte[][] newGrid ) {
		grid = newGrid;
	} //generateGrid()

	/**
	 * Creates a string that is the BattleShip grid in 
	 * human readable form.
	 *
	 * @return - readable grid
	 */
	public String toString() {
		String gridString = new String();
		String[] letters = { "A", "B", "C", "D", "E", 
		"F", "G", "H", "I", "J" };
		int j = 0;

		//add top area
		gridString = gridString.concat(
		" 0123456789\n" );

		for( int i = 0; i < 10; i++ ) {
			gridString = 
			gridString.concat( letters[ i ] );	
			for( j = 0; j < 10; j++ ) {
				if( grid[ i ][ j ] == 0 ) { //empty
					gridString = gridString.concat( "." );
				} else if( grid[ i ][ j ] == 1 ) { //ship
					gridString = gridString.concat( "0" );
				} else if( grid[ i ][ j ] == 2 ) { //miss
					gridString = gridString.concat( "*" );
				} else { //hit
					gridString = gridString.concat( "X" );
				}

			}
			gridString = gridString.concat( "\n" );
		}

		return gridString;
	} //toString()

} // BSGrid


