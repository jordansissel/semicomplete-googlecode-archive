/*
 * HelloResponse.java
 * 
 * Version:
 *   $Id$
 *
 * Revisions:
 *   $Log$
 *   Revision 1.1  2004/01/20 08:52:35  tristan
 *   finished writing.
 *
 */

/**
 * Response for sending a hello msg.
 *
 * @author Tristan O'Tierney
 * @author Jordan Sissel
 * @author Nick Johnson
 */
public class HelloResponse extends Response {
    /**
     * Initializes a hello response.
     */
    public HelloResponse() {
        super( ProtocolConstants.HELLO_RESPONSE, "Hello" );
    }
}   // HelloResponse