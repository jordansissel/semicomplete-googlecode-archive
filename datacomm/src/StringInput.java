/*
 * StringInput.java
 * 
 * Version:
 *   $Id$
 *
 * Revisions:
 *   $Log$
 *   Revision 1.2  2004/01/20 04:23:33  tristan
 *   updated comments for everything.
 *
 *   Revision 1.1  2004/01/19 03:13:35  tristan
 *   initial
 *
 */

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.IOException;

/**
 *  Receives input from the user.
 *
 * @author Tristan O'Tierney
 * @author Jordan Sissel
 * @author Nick Johnson
 */

public class StringInput {

    private static BufferedReader input = null;     // the input stream to read from.

    /**
     * Keeps users from creating an instance.
     */
    private StringInput() {
    }

    /**
     * Sends an output message and reads input.
     *
     * @param output The message to output first.
     * @return String the input given by the user.
     */
    public static synchronized String getInput( String output ) {
        if ( input == null ) {
            input = new BufferedReader( new InputStreamReader( System.in ) );
        }

        // output the message
        System.out.print( output );

        // receive the user's input
        String retVal = null;
        try {
            retVal = input.readLine();
        } catch ( IOException e ) {
            System.err.println( "Error: could not open stream to input keys" );
            System.err.println( e.getMessage() );
            retVal = "";
        }

        // send it to the function caller
        return ( retVal == null ) ? "" : retVal;
    }
}   // StringInput