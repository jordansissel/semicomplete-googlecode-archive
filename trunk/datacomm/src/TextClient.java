/*
 * TextClient.java
 * 
 * Version:
 *   $Id$
 *
 * Revisions:
 *   $Log$
 *   Revision 1.8  2004/01/20 03:42:37  tristan
 *   wrote basic functions for print game finish
 *
 *   Revision 1.7  2004/01/20 03:31:13  tristan
 *   uncommented block.
 *
 *   Revision 1.6  2004/01/19 23:23:19  tristan
 *   included init game
 *
 *   Revision 1.5  2004/01/19 20:48:32  tristan
 *   changes due to refactoring
 *
 *   Revision 1.4  2004/01/19 05:35:38  tristan
 *   added print opponent's move function
 *
 *   Revision 1.3  2004/01/19 04:01:53  tristan
 *   implemented run method.
 *
 *   Revision 1.2  2004/01/19 02:35:25  tristan
 *   moved stuff into text client
 *
 *   Revision 1.1  2004/01/19 02:21:48  tristan
 *   initial
 *
 */

/**
 * Text based Battle ship client
 *  
 * @author tristan
 */
public class TextClient extends BSClient {
    /**
     * Initializes the text client.
     * @param connection The client connection.
     * @param yourName This player's name.
     * @param otherName The opponent's name
     */
    public TextClient( Client connection,
                       String yourName,
                       String otherName ) {
        super( connection, yourName, otherName );
    }

    /**
     * Runs the text client by talking to the server
     */
    public void run() {
        if ( getConnection().connect() ) {
            // initialize game
            startGame();
            findOpponent();
            initGame();

            // start game loop
            while ( !isGameOver() && getConnection().isConnected() ) {
                if ( !isClientTurn() ) {
                    waitForTurn();
                    printOpponentMove();
                } else {
                    doTurn();
                }
            }

            // print out who won etc
            printGameFinish();
        } else {
            System.out.println( "Failing to connect to server" );
        }
    }

    /**
     * Does the user's turn
     */
    public void doTurn() {
        String input = null;
        boolean validInput = false;
        char row = 'a';
        int col = 1;
        String errorMsg = "Input must be [A-J] [1-10]";

        // get the row and column
        do {
            input = StringInput.getInput( "Input row and column: " );
            String[] vars = input.split( "\\s" );

            // check length
            if ( vars.length != 2 ) {
                System.out.println( errorMsg );
                continue;
            }

            // check row
            if ( vars[ 0 ].length() > 1 ) {
                System.out.println( errorMsg );
                continue;
            }

            // make sure it's between a-j
            row = vars[ 0 ].toLowerCase().charAt( 0 );
            if ( row < 'a' || row > 'j' ) {
                System.out.println( errorMsg );
                continue;
            }

            // parse column
            try {
                col = Integer.parseInt( vars[ 1 ] );
            } catch ( NumberFormatException e ) {
                System.out.println( errorMsg );
                continue;
            }

            // make sure the number is between 1 and 10
            if ( col < 1 || col > 10 ) {
                System.out.println( errorMsg );
                continue;
            }

            // we actually parsed everything
            validInput = true;
        } while ( !validInput );

        // send a message to the server
        sendFire( row, col );
    }

    /**
     * Prints who won, etc.
     */
    public void printGameFinish() {
        if ( wonGame() ) {
            System.out.println( "Congradulations, you won!" );
        } else {
            System.out.println( "You Lose!" );
        }
    }

    /**
     * Prints the opponents move.
     */
    public void printOpponentMove() {
        String row = new String( new char[] { getLastRow() } ).toUpperCase();

        System.out.println( "Your opponent fired at " + row +
                            "-" + getLastCol() );

        if ( isLastHit() ) {
            System.out.println( "Your opponent hit one of your ships!" );
        } else {
            System.out.println( "Your opponent missed." );
        }
    }
}   // TextClient