/**
 * BSGrid.java
 *
 * Version:
 * 	$Id$
 *
 * Revisions:
 * 	$Log$
 * 	Revision 1.6  2004/01/13 02:14:23  njohnson
 * 	I finished the BSGrid's generateGrid() method which is now quite huge.
 * 	It might be better with some AI( or worse ) since it just tries to throw
 * 	ships down randomly and checks for overlaps.
 * 	I also added a default constructor to BSShip to ease my mind.
 *
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
		fleet = new BSShip[ 4 ];
		generateGrid();
	} // constructor

	public void generateGrid() {
		byte[][] newGrid;
		newGrid = new byte[10][10];
		int startRowValue = 0;
		int startColValue = 0;
		int endRowValue = 0;
		int endColValue = 0;
		boolean overlap = false; //true if there is a ship overlap
		int p = 0; 
		int shipL = 0; // the ship length;
		int dir = 0; // direction from start to end of ship
		            //
			    //		1
			    //	2	+	4	
			    //		3
			    //

		for( int i = 0; i < 5; i++ ) {

			//generate the random values for the ship's
			//start position
			startRowValue = (int)
			( Math.ceil( Math.random()*10 ) - 1.0 );	
			startColValue = (int)
			( Math.ceil( Math.random()*10 ) - 1.0 );

			//gotta set the ship's length
			switch( i ) {
				case 0:
					shipL = 2;	
					break;
				case 1:
					shipL = 3;
					break;
				case 2:
					shipL = 3;
					break;
				case 3:
					shipL = 4;
					break;
				case 4:
					shipL = 5;
					break;
			}


			//check grid bounds, reverse ship if
			//necessary
			dir = (int) Math.ceil( Math.random()*4 );
			if( dir == 1 && ( startColValue - shipL ) < 0  ) {
				dir = 3;
			} else if( dir == 2 && ( startRowValue - shipL ) < 0 ) {
				dir = 4;
			} else if( dir == 3 && ( startColValue + shipL ) > 9 ) {
				dir = 1;
			} else if( dir == 4 && ( startRowValue + shipL ) > 9 ) {
				dir = 2;
			}
			
			//initially check for ship overlap
			switch( dir ) {
				case 1:
				for( p = 0; p < shipL; p++ ) {
					if(grid[startRowValue+p][startColValue] != 0) {
						overlap = true;
					}
				}
				endRowValue=startRowValue+shipL;
				endColValue = startColValue;
				break;
				case 2:
				for( p = 0; p < shipL; p++ ) {
					if(grid[startRowValue][startColValue-p] != 0 ) {
						overlap = true;
					}
				}
				endRowValue = startRowValue;
				endColValue=startColValue-shipL;
				break;
				case 3:
				for( p = 0; p < shipL; p++ ) {
					if(grid[startRowValue-p][startColValue] != 0 ) {
						overlap = true;
					}
				}
				endRowValue=startRowValue-shipL;
				endColValue = startColValue;
				break;
				case 4:
				for( p = 0; p < shipL; p++ ) {
					if(grid[startRowValue][startColValue+p] != 0 ) {
						overlap = true;
					}
				}
				endRowValue = startRowValue;
				endColValue=startColValue+shipL;
				break;
				default:
					System.out.println("wrong!");
			} //switch

			//fill in ships and make sure they 
			//don't overlap or go off the grid
		 	while( overlap ) {
				startRowValue = (int)
				( Math.ceil( Math.random()*10 ) - 1.0 );	
				startColValue = (int)
				( Math.ceil( Math.random()*10 ) - 1.0 );

				overlap = false;

				if( dir == 1 && ( startColValue - shipL) < 0 ) {
					dir = 3;
				} else if( dir == 2 && 
				  ( startRowValue - shipL ) < 0 ) {
					dir = 4;
				} else if( dir == 3 && 
				  ( startColValue + shipL ) > 9 ) {
					dir = 1;
				} else if( dir == 4 && 
				  ( startRowValue + shipL ) > 9 ) {
					dir = 2;
				}
		
				//check for ship overlap again.
				switch( dir ) {
					case 1:
					for( p = 0; p < shipL; p++ ) {
						if(grid[startRowValue+p][startColValue] != 0) {
							overlap = true;
						}
					}
					endRowValue=startRowValue+shipL;
					endColValue = startColValue;
					break;
					case 2:
					for( p = 0; p < shipL; p++ ) {
						if(grid[startRowValue][startColValue-p] != 0 ) {
							overlap = true;
						}
					}
					endRowValue = startRowValue;
					endColValue=startColValue-shipL;
					break;
					case 3:
					for( p = 0; p < shipL; p++ ) {
						if(grid[startRowValue-p][startColValue] != 0 ) {
							overlap = true;
						}
					}
					endRowValue=startRowValue-shipL;
					endColValue = startColValue;
					break;
					case 4:
					for( p = 0; p < shipL; p++ ) {
						if(grid[startRowValue][startColValue+p] != 0 ) {
							overlap = true;
						}
					}
					endRowValue = startRowValue;
					endColValue=startColValue+shipL;
					break;
					default:
						System.out.println("wrong!");
				} //switch
			}

			//make the ship!
			fleet[ i ] = new BSShip((byte)startRowValue,
				(byte)startColValue, 
				(byte)endRowValue,
				(byte)endColValue );	

			//fill in the grid with the ship

			switch( dir ) {
				case 1:
				for( p = 0; p < shipL; p++ ) {
					grid[startRowValue+p][startColValue]=1; 
				}
				endRowValue=startRowValue+shipL;
				endColValue = startColValue;
				break;
				case 2:
				for( p = 0; p < shipL; p++ ) {
					grid[startRowValue][startColValue-p]=1;
				}
				endRowValue = startRowValue;
				endColValue=startColValue-shipL;
				break;
				case 3:
				for( p = 0; p < shipL; p++ ) {
					grid[startRowValue-p][startColValue]=1;
				}
				endRowValue=startRowValue-shipL;
				endColValue = startColValue;
				break;
				case 4:
				for( p = 0; p < shipL; p++ ) {
					grid[startRowValue][startColValue+p]=1;
				}
				endRowValue = startRowValue;
				endColValue=startColValue+shipL;
				break;
			} //switch
		}

} //generateGrid()

public void generateGrid( byte[][] newGrid ) {
	grid = newGrid;
} //generateGrid()

/**

		}
		
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


