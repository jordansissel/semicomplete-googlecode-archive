/*
 * ErrorResponse.java
 *
 * Version:
 *    $Id$
 *
 * Revisions:
 *    $Log$
 *    Revision 1.2  2004/01/20 00:05:21  psionic
 *    - Fixed missing default constructor.
 *
 *    Revision 1.1  2004/01/19 23:52:28  psionic
 *    - initial commit
 *
 *
 */

public class ErrorResponse extends Response {

	public ErrorResponse() {
		super();
		this.setMessage(new String());
	}

	public ErrorResponse(String msg) {
		super();
		this.setMessage(msg);
	}

}
