/*
 * Response.java
 *
 * Version:
 *   $Id$
 *
 * Revisions:
 *   $Log$
 *   Revision 1.8  2004/02/05 01:57:23  tristan
 *   fixed parse bug.
 *
 *   Revision 1.7  2004/01/20 04:23:33  tristan
 *   updated comments for everything.
 *
 *   Revision 1.6  2004/01/20 00:05:21  psionic
 *   - Fixed missing default constructor.
 *
 *   Revision 1.5  2004/01/19 23:48:52  psionic
 *   - Bulk commit: Command infrastructure improved. New commands can be added
 *     dynamically now.
 *
 *   Revision 1.4  2004/01/19 20:36:58  tristan
 *   implemented validate arguments method.
 *
 *   Revision 1.3  2004/01/19 05:44:43  tristan
 *   added to byte array function.
 *
 *   Revision 1.2  2004/01/14 06:05:25  tristan
 *   finished basic response class
 *
 *   Revision 1.1  2004/01/14 04:35:27  tristan
 *   initial
 *
 */

import java.util.regex.Pattern;
import java.util.regex.Matcher;
import java.util.List;
import java.io.ByteArrayOutputStream;
import java.io.DataOutputStream;

/**
 * Response to a command query.
 *
 * @author Tristan O'Tierney
 * @author Jordan Sissel
 * @author Nick Johnson
 */
public class Response implements Message {
    // member vars
    private int id;
    private String message;

	 /**
	  * Default Constructor
	  *
	  */
	 public Response() {
		 this.id = 0;
		 this.message = new String();
	 }

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
        String[] items = input.split( "\\s", 2 );
        Response retVal = null;

        if ( items.length > 0 ) {
            try {
                retVal = new Response( Integer.parseInt( items[ 0 ] ) );

                if ( items.length == 2 ) {
                    retVal.setMessage( items[ 1 ] );
                }
            } catch ( NumberFormatException e ) {
            }
        }

        return retVal;
    }

    /**
     * Returns the response in byte form.
     * @return The response in byte form
     */
    public byte[] toByteArray() {
        ByteArrayOutputStream out = new ByteArrayOutputStream();
        DataOutputStream data = new DataOutputStream( out );

        try {
            data.writeInt( id );
            data.writeUTF( message );
        } catch ( Exception e ) {
        }

        return out.toByteArray();
    }

    /**
     * Default impelementation for validate arguments
     * @return
     * @throws InvalidCommandArgumentsException
     */
    public boolean validateArguments(List args) 
	        throws InvalidCommandArgumentsException {
        return true;
    }
}   // Response
