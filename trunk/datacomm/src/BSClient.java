/*
 * BSClient.java
 * 
 * Version:
 *   $Id$
 *
 * Revisions:
 *   $Log$
 *   Revision 1.12  2004/01/20 04:23:32  tristan
 *   updated comments for everything.
 *
 *   Revision 1.11  2004/01/20 03:43:24  tristan
 *   added vars for last move.
 *
 *   Revision 1.10  2004/01/20 03:19:24  tristan
 *   very close to finished.
 *
 *   Revision 1.9  2004/01/20 02:40:44  tristan
 *   wrote part of send call function.
 *
 *   Revision 1.8  2004/01/20 02:33:23  tristan
 *   finished init game
 *
 *   Revision 1.7  2004/01/19 23:20:52  tristan
 *   wrote start game and find opponent
 *
 *   Revision 1.6  2004/01/19 21:00:50  tristan
 *   implements protocol constants now
 *
 *   Revision 1.5  2004/01/19 20:48:20  tristan
 *   changes due to refactoring
 *
 *   Revision 1.4  2004/01/19 05:33:46  tristan
 *   switched to use boolean for client turn
 *
 *   Revision 1.3  2004/01/19 04:00:35  tristan
 *   subbed out protocol methods.
 *
 *   Revision 1.2  2004/01/19 02:34:58  tristan
 *   added a new constructor
 *
 *   Revision 1.1  2004/01/19 02:28:17  tristan
 *   finished basic abstract client.
 *
 */

import java.util.ArrayList;
import java.util.List;

/**
 * Abstract class for a BS Client object
 *
 * @author Tristan O'Tierney
 * @author Jordan Sissel
 * @author Nick Johnson
 */
public abstract class BSClient extends Thread implements ProtocolConstants {
    private Client connection;
    private String yourName;
    private String otherName;
    private BSGame game;
    private boolean clientTurn;
    private char lastRow;
    private int lastCol;
    private boolean lastHit;
    private boolean wonGame;

    /**
     * Initializes the battle ship client
     */
    public BSClient( Client connection,
                     String yourName,
                     String otherName ) {
        this.connection = connection;
        this.game = new BSGame();
        this.yourName = yourName;
        this.otherName = otherName;
        this.clientTurn = false;
        this.wonGame = false;
        this.lastRow = 'a';
        this.lastCol = 1;
        this.lastHit = false;
    }

    /**
     * Returns the game for this client.
     * @return The game this client is playing.
     */
    public BSGame getGame() {
        return game;
    }

    /**
     * Changes the game for this client.
     * @param game The new game object.
     */
    public void setGame( BSGame game ) {
        this.game = game;
    }

    /**
     * Returns the client connection for this player.
     * @return The player's connection to the server.
     */
    public Client getConnection() {
        return connection;
    }

    /**
     * Sets the connection for the server.
     * @param connection Changes the connection for the client.
     */
    public void setConnection( Client connection ) {
        this.connection = connection;
    }

    /**
     * Gets the name of this player.
     * @return The player's name.
     */
    public String getYourName() {
        return yourName;
    }

    /**
     * Sets the player's name.
     * @param yourName The name for this player.
     */
    public void setYourName( String yourName ) {
        this.yourName = yourName;
    }

    /**
     * Sets the other player's name.
     * @param otherName The other player's name.
     */
    public void setOtherName( String otherName ) {
        this.otherName = otherName;
    }

    /**
     * Returns the other player's name.
     * @return The other player's name.
     */
    public String getOtherName() {
        return otherName;
    }

    /**
     * Sends a start game message to the server telling it that we
     * are looking for a specific opponent.
     */
    public void startGame() {
        List args = new ArrayList();
        args.add( yourName );
        args.add( otherName );
        connection.sendMessage( new Command( STARTGAME, args ) );

        // check for a receive msg
        waitForResponse( STARTGAME_RESPONSE );
    }

    /**
     * Waits until an opponent is found.
     */
    public void findOpponent() {
        waitForResponse( PLAYERFOUND );
    }

    /**
     * Initializes the game board from the server and does
     * other startup things.
     */
    public void initGame() {
        Command comm = new Command( INITGAME );
        List args = new ArrayList();

        // build the command
        BSGrid grid = game.getOwnGrid();
        for ( int i = 0; i < BSGrid.ROW_COUNT; i++ ) {
            BSShip ship = grid.getShip( i );
            args.add( new Integer( ship.getStartX() ) );
            args.add( new Integer( ship.getStartY() ) );
            args.add( new Integer( ship.getEndX() ) );
            args.add( new Integer( ship.getEndY() ) );
        }

        // send it
        try {
            comm.setArgs( args );
        } catch ( Exception e ) {
        }
        connection.sendMessage( comm );

        // check response
        Response response = null;
        boolean done = false;

        do {
            response = connection.receiveResponse();

            if ( response != null ) {
                if ( response.getId() == INITGAME_RESPONSE_FIRST ) {
                    clientTurn = true;
                    done = true;
                } else if ( response.getId() == INITGAME_RESPONSE_SECOND ) {
                    done = true;
                }
            }
        } while ( !done && connection.isConnected() );
    }

    /**
     * Determines if it's this client's turn or not.
     * @return Whether or not it's our turn.
     */
    public boolean isClientTurn() {
        return clientTurn;
    }

    /**
     * Changes the client turn.
     * @param clientTurn The new client turn var.
     */
    public void setClientTurn( boolean clientTurn ) {
        this.clientTurn = clientTurn;
    }

    /**
     * Determines if this game is over.
     * @return Whether or not the game is still in play.
     */
    public boolean isGameOver() {
        return false;
    }

    /**
     * Sends a call to the server
     * @param row Which row to hit.
     * @param col Which column to hit.
     */
    public void sendFire( char row, int col ) {
        Command fire = new Command( FIRE );

        // build args
        List args = new ArrayList();
        args.add( new Character( row ) );
        args.add( new Integer( col ) );

        // build command
        try {
            fire.setArgs( args );
        } catch ( Exception e ) {
        }
        connection.sendMessage( fire );

        // get response
        Response response = null;
        boolean done = false;
        boolean hit = false;

        do {
            response = connection.receiveResponse();

            if ( response != null ) {
                if ( response.getId() == FIRE_RESPONSE_HIT ) {
                    hit = true;
                    done = true;
                } else if ( response.getId() == FIRE_RESPONSE_MISS ) {
                    done = true;
                }
            }
        } while ( !done && connection.isConnected() );

        // do stuff based on hit/miss
        game.getTargetGrid().alterGrid( row, col, hit );
        setClientTurn( false );
    }

    /**
     * Receives a fire and processes it.
     */
    public void receiveFire() {
        Command fire = null;

        // get the fire command
        do {
            fire = connection.receiveCommand();
        } while ( connection.isConnected() &&
                  fire != null &&
                  fire.getCommand().equals( FIRE ) );

        // parse the row and col
        char row = 'a';
        int col = 1;
        boolean invalid = false;

        try {
            if ( fire.getArgs().size() == 2 ) {
                row = fire.getArg( 0 ).toString().toLowerCase().charAt( 0 );

                if ( row < 'a' || row > 'j' ) {
                    invalid = true;
                } else {
                    col = Integer.parseInt( fire.getArg( 1 ).toString() );

                    if ( col < 1 || col > 10 ) {
                        invalid = true;
                    }
                }
            } else {
                invalid = true;
            }
        } catch ( Exception e ) {
            invalid = true;
        }

        // if they sent us a correct fire, then process it
        if ( !invalid ) {
            boolean hit = game.getOwnGrid().shot( row, col );
            Response response = null;

            if ( hit ) {
                // send hit response
                response = new Response( FIRE_RESPONSE_HIT, "Succesful fire" );
                game.getOwnGrid().alterGrid( row, col, hit );
            } else {
                // send miss response
                response = new Response( FIRE_RESPONSE_HIT,
                                         "Unsuccesful fire" );
            }

            // set last vars
            lastRow = row;
            lastCol = col;
            lastHit = hit;

            // send it
            connection.sendMessage( response );
        }
    }

    /**
     * Blocks until it's the user's turn
     */
    public void waitForTurn() {
        if ( clientTurn != true ) {
            receiveFire();
        }
        clientTurn = true;
    }

    /**
     * Waits for a response from the server.
     * @param id The id to match.
     */
    protected void waitForResponse( int id ) {
        Response response = null;

        do {
            response = connection.receiveResponse();
        } while ( response != null &&
                  response.getId() != id &&
                  connection.isConnected() );
    }

    /**
     * Receives the last row targeted from the opponent.
     * @return The last row targeted.
     */
    public char getLastRow() {
        return lastRow;
    }

    /**
     * Get the last column targeted.
     * @return The last column targeted.
     */
    public int getLastCol() {
        return lastCol;
    }

    /**
     * Return whether or not the last target was a hit.
     * @return Returns if the last shot was a hit.
     */
    public boolean isLastHit() {
        return lastHit;
    }

    /**
     * Return whether or not you won the game.
     * @return True if this user won.
     */
    public boolean wonGame() {
        return wonGame;
    }
}   // BSClient