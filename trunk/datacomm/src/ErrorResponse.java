/*
 * ErrorResponse.java
 *
 * Version:
 *    $Id$
 *
 * Revisions:
 *    $Log$
 *    Revision 1.1  2004/01/19 23:52:28  psionic
 *    - initial commit
 *
 *
 */

public class ErrorResponse extends Response {

	public ErrorResponse() {
		message = new String();
	}

	public ErrorResponse(String msg) {
		message = msg;
	}

}
