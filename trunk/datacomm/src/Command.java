/*
 * Command.java
 *
 * Version:
 *   $Id$
 *
 * Revisions:
 *   $Log$
 *   Revision 1.2  2004/01/14 05:53:33  tristan
 *   finished command parsing
 *
 *   Revision 1.1  2004/01/14 04:36:04  tristan
 *   initial
 *
 */

import java.util.List;
import java.util.Iterator;
import java.util.ArrayList;
import java.util.regex.Pattern;
import java.util.regex.Matcher;

/**
 * Command sent to a client from the server.
 *
 * @author tristan
 */
public class Command {
    // constants
    public static final Pattern commandPattern = Pattern.compile(
        "(.*?)\\s*"
    );
    public static final Pattern argsPattern = Pattern.compile(
        "\\s*(.*?)\\s*,{1}\\s*(.*)\\s*"
    );

    // member vars
    private String command;
    private List args;

    /**
     * Initializes a command with a command name and args.
     * @param command The command
     * @param args The args to set.
     */
    public Command( String command, List args ) {
        this.command = command;
        this.args = args;
    }

    /**
     * Initializes a command without args.
     * @param command
     */
    public Command( String command ) {
        this( command, null );
    }

    /**
     * Gets the command.
     * @return The command.
     */
    public String getCommand() {
        return command;
    }

    /**
     * Sets the command var.
     * @param command The command name.
     */
    public void setCommand( String command ) {
        this.command = command;
    }

    /**
     * Returns the args in this command.
     * @return The args for this command.
     */
    public List getArgs() {
        return args;
    }

    /**
     * Changes the args for this command.
     * @param args The args for this command.
     */
    public void setArgs( List args ) {
        this.args = args;
    }

    /**
     * Returns a string representation of Command
     * @return A string representation of Command
     */
    public String toString() {
        StringBuffer retVal = new StringBuffer( command.toUpperCase() );

        if ( args != null ) {
            Iterator i = args.iterator();

            while ( i.hasNext() ) {
                Object item = i.next();
                retVal.append( " " + item );

                if ( i.hasNext() ) {
                    retVal.append( "," );
                }
            }
        }

        return retVal.toString();
    }

    /**
     * Parses a command object in the format: COMMAND arg1, arg2, arg3
     * @param input Input text to parse.
     * @return Return the created command object.
     */
    public static Command parseCommand( String input ) {
        Command retVal = null;
        Matcher m = commandPattern.matcher( input );
        List args = null;

        // command found
        if ( m.matches() ) {
            retVal = new Command( m.group( 1 ) );

            // args found
            if ( m.groupCount() > 1 ) {
                String unparsedArgs = m.group( 2 );
                args = new ArrayList();
                retVal.setArgs( args );

                // parse each arg
                do {
                    m = argsPattern.matcher( unparsedArgs );

                    if ( m.matches() ) {
                        args.add( m.group( 1 ) );

                        if ( m.groupCount() == 1 ) {
                            unparsedArgs = null;
                            break;
                        } else {
                            unparsedArgs = m.group( 2 );
                        }
                    } else {
                        break;
                    }
                } while ( true );

                // add trailing arg
                if ( unparsedArgs != null ) {
                    args.add( unparsedArgs );
                }
            }
        }

        return retVal;
    }
}   // Command
