/*
 * ProtocolConstants.java
 * 
 * Version:
 *   $Id$
 *
 * Revisions:
 *   $Log$
 *   Revision 1.1  2004/01/19 20:58:21  tristan
 *   started class
 *
 */

/**
 * Manages the protocol constants.
 * 
 * @author Tristan O'Tierney
 */
public interface ProtocolConstants {
    public static final String HELLO = "HELLO";
    public static final int HELLO_RESPONSE = 101;

    public static final String STARTGAME = "STARTGAME";
    public static final int STARTGAME_RESPONSE = 102;
}   // ProtocolConstants

