/*
 * StartgameResponse.java
 * 
 * Version:
 *   $Id$
 *
 * Revisions:
 *   $Log$
 *   Revision 1.2  2004/01/26 21:44:40  psionic
 *   *** empty log message ***
 *
 *   Revision 1.1  2004/01/26 21:21:28  psionic
 *   - Updated commands framework, added game-starting ability
 *
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
public class StartgameResponse extends Response {
    /**
     * Creates a new StartgameResponse
     */
    public StartgameResponse() {
        super( ProtocolConstants.STARTGAME_RESPONSE, "Start game" );
    }
}   // StartgameResponse
