/*
 * BSClient.java
 * 
 * Version:
 *   $Id$
 *
 * Revisions:
 *   $Log$
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

/**
 * Abstract class for a BS Client object
 *
 * @author tristan
 */
public abstract class BSClient extends Thread {
    private ClientConnection connection;
    private String yourName;
    private String otherName;
    private BSGame game;

    /**
     * Initializes the battle ship client
     */
    public BSClient( ClientConnection connection,
                     String yourName,
                     String otherName ) {
        this.connection = connection;
        this.game = new BSGame();
        this.yourName = yourName;
        this.otherName = otherName;
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
    public ClientConnection getConnection() {
        return connection;
    }

    /**
     * Sets the connection for the server.
     * @param connection Changes the connection for the client.
     */
    public void setConnection( ClientConnection connection ) {
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
     * Starts the game.
     */
    public void startGame() {

    }

    /**
     * Finds an opponent.
     */
    public void findOpponent() {

    }

    /**
     * Determines if it's this client's turn or not.
     * @return Whether or not it's our turn.
     */
    public boolean isClientTurn() {
        return true;
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
    public void sendCall( char row, int col ) {

    }

    /**
     * Blocks until it's the user's turn
     */
    public void waitForTurn() {
        
    }
}   // BSClient