/*
 * StartGameResponse.java
 * 
 * Version:
 *   $Id$
 *
 * Revisions:
 *   $Log$
 *   Revision 1.1  2004/01/20 08:55:04  tristan
 *   wrote basic startgame response
 *
 */

/**
 * Start game response object.
 *
 * @author Tristan O'Tierney
 * @author Jordan Sissel
 * @author Nick Johnson
 */
public class StartGameResponse extends Response {
    /**
     * Creates a new StartGameResponse
     */
    public StartGameResponse() {
        super( ProtocolConstants.STARTGAME_RESPONSE, "Start game" );
    }
}   // StartGameResponse