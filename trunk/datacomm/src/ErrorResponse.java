/*
 * ErrorResponse.java
 *
 * Version:
 *    $Id$
 *
 * Revisions:
 *    $Log$
 *    Revision 1.3  2004/01/20 04:23:33  tristan
 *    updated comments for everything.
 *
 *    Revision 1.2  2004/01/20 00:05:21  psionic
 *    - Fixed missing default constructor.
 *
 *    Revision 1.1  2004/01/19 23:52:28  psionic
 *    - initial commit
 *
 *
 */

/**
 * A response for errors.
 *
 * @author Tristan O'Tierney
 * @author Jordan Sissel
 * @author Nick Johnson
 */
public class ErrorResponse extends Response {

    /**
     * Creates a new error response object
     */
	public ErrorResponse() {
        this( "" );
	}

    /**
     * Creates a new error response with message.
     * @param msg The message to set.
     */
	public ErrorResponse(String msg) {
		this.setMessage(msg);
	}
}
