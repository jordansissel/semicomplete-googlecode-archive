/*
 * NewuserResponse.java
 * 
 * Version:
 *   $Id$
 *
 * Revisions:
 *   $Log$
 *   Revision 1.1  2004/02/19 06:33:19  psionic
 *   *** empty log message ***
 *
 *
 */

/**
 * Response for sending a newuser msg.
 *
 * @author Tristan O'Tierney
 * @author Jordan Sissel
 * @author Nick Johnson
 */
public class NewuserResponse extends Response {
    /**
     * Initializes a newuser response.
     */
    public NewuserResponse() {
        super( ProtocolConstants.ENTERPASSWD_NEWUSER, "Newuser" );
    }
}   // NewuserResponse
