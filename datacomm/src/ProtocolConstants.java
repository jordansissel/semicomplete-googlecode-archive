/*
 * ProtocolConstants.java
 * 
 * Version:
 *   $Id$
 *
 * Revisions:
 *   $Log$
 *   Revision 1.6  2004/01/20 04:23:33  tristan
 *   updated comments for everything.
 *
 *   Revision 1.5  2004/01/20 02:36:19  tristan
 *   added fire and fire response constants
 *
 *   Revision 1.4  2004/01/20 02:25:30  tristan
 *   added first/second init game response
 *
 *   Revision 1.3  2004/01/20 02:07:40  tristan
 *   added init game
 *
 *   Revision 1.2  2004/01/19 23:21:16  tristan
 *   added player found constant
 *
 *   Revision 1.1  2004/01/19 20:58:21  tristan
 *   started class
 *
 */

/**
 * Manages the protocol constants.
 *
 * @author Tristan O'Tierney
 * @author Jordan Sissel
 * @author Nick Johnson
 */
public interface ProtocolConstants {
    // 100s: startup connection constants
    public static final String HELLO = "HELLO";
    public static final int HELLO_RESPONSE = 101;
    public static final String STARTGAME = "STARTGAME";
    public static final int STARTGAME_RESPONSE = 102;
    public static final int PLAYERFOUND = 103;
    public static final String INITGAME = "INITGAME";
    public static final int INITGAME_RESPONSE_FIRST = 104;
    public static final int INITGAME_RESPONSE_SECOND = 105;

    // 200s: game state constants
    public static final String FIRE = "FIRE";
    public static final int FIRE_RESPONSE_HIT = 201;
    public static final int FIRE_RESPONSE_MISS = 202;
    
}   // ProtocolConstants

