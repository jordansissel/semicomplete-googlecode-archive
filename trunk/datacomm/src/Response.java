/*
 * Response.java
 *
 * Version:
 *   $Id$
 *
 * Revisions:
 *   $Log$
 *   Revision 1.2  2004/01/14 06:05:25  tristan
 *   finished basic response class
 *
 *   Revision 1.1  2004/01/14 04:35:27  tristan
 *   initial
 *
 */

import java.util.regex.Pattern;
import java.util.regex.Matcher;

/**
 * Response to a command query.
 *
 * @author tristan
 */
public class Response {
    // constants
    public static final Pattern responsePattern = Pattern.compile(
        "(\\d*?)\\s*"
    );

    // member vars
    private int id;
    private String message;

    /**
     * Creates a new response with an id and message
     * @param id The response id.
     * @param message The response message.
     */
    public Response( int id, String message ) {
        this.id = id;
        this.message = message;
    }

    /**
     * Creates a new response with no message.
     * @param id The response id.
     */
    public Response( int id ) {
        this( id, null );
    }

    /**
     * Returns the id number.
     * @return the id number.
     */
    public int getId() {
        return id;
    }

    /**
     * Set the id number of this message.
     * @param id The id number of the message.
     */
    public void setId( int id ) {
        this.id = id;
    }

    /**
     * Returns the message for this response.
     * @return The response message.
     */
    public String getMessage() {
        return message;
    }

    /**
     * Changes the message for this response.
     * @param message The response's new message.
     */
    public void setMessage( String message ) {
        this.message = message;
    }

    /**
     * Returns a string representation of Response
     * @return A string representation of Response
     */
    public String toString() {
        StringBuffer retVal = new StringBuffer( String.valueOf( id ) );

        if ( message != null ) {
            retVal.append( " " + message );
        }

        return retVal.toString();
    }

    /**
     * Parses a response message.
     * @param input The input string to parse.
     * @return The response object.
     */
    public static Response parseResponse( String input ) {
        Response retVal = null;
        Matcher m = responsePattern.matcher( input );

        // id found
        if ( m.matches() ) {
            try {
                retVal = new Response( Integer.parseInt( m.group( 1 ) ) );

                // message found
                if ( m.groupCount() > 1 ) {
                    retVal.setMessage( m.group( 2 ) );
                }
            } catch ( NumberFormatException e ) {
            }
        }

        return retVal;
    }
}   // Response
