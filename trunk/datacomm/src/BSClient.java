/*
 * BSClient.java
 * 
 * Version:
 *   $Id$
 *
 * Revisions:
 *   $Log$
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
    public BSClient( ClientConnection connection ) {
        this.connection = connection;
        this.game = new BSGame();
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
}   // BSClient