/*
 * Command.java
 *
 * Version:
 *   $Id$
 *
 * Revisions:
 *   $Log$
 *   Revision 1.4  2004/01/17 18:22:08  tristan
 *   fixed parse bug
 *
 *   Revision 1.3  2004/01/14 06:02:14  tristan
 *   removed unneeded var
 *
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

    // regexes needed to parse commands
    public static final Pattern argsPattern = Pattern.compile(
        "\\s*(.*?)\\s*,{1}\\s*(.*)\\s*"
    );

	private String command;
	private List args;

	/**
	 * Initialize a default command.
	 */
	public Command() {
		command = null;
		args = null;
	}

	/**
	 * Initializes a new Command out of the command and it's arguments.
	 */
	public Command( String command, List args ) {
		setCommand( command );
		this.args = args;
	}

	/**
	 * Parses and initializes the command variable.
	 * @param command The command.
	 */
	public Command( String command ) {
        this( command, null );
	}

	/**
	 * Returns the arguments for this command.
	 * @return The comman arguments.
	 */
	public List getArgs() {
		return args;
	}

	/**
	 * Returns the arg at the specified index.
	 * @param i The index of the arg.
	 * @return The arg.
	 */
	public Object getArg( int i ) {
		return args.get( i );
	}

	/**
	 * Changes the command args.
	 * @param args The command args.
	 */
	public void setArgs( List args ) {
		this.args = args;
	}

	/**
	 * Returns the command name.
	 * @return The command name.
	 */
	public String getCommand() {
		return command;
	}

	/**
	 * Changes the command.
	 * @param command The command.
	 */
	public void setCommand( String command ) {
        if ( command != null ) {
		    this.command = command.toUpperCase();
        } else {
            this.command = null;
        }
	}

	/**
	 * Return the command as a string.
	 * @return The command as a string.
	 */
    public String toString() {
        StringBuffer retVal = new StringBuffer( command );

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
	 * Sets the command args and parses them.
	 * @param unparsedArgs The unparsed arguments.
	 */
	public static List parseArgs( String unparsedArgs ) {
        List retVal = new ArrayList();

        // parse each arg
        do {
            Matcher m = argsPattern.matcher( unparsedArgs );

            if ( m.matches() ) {
                retVal.add( m.group( 1 ) );

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
            retVal.add( unparsedArgs );
        }

        return retVal;
	}

	/**
	 * Parses a command and it's arguments.
	 * @param unparsedCommand The command to parse.
	 */
	public static Command parseCommand( String unparsedCommand ) {
        String[] items = unparsedCommand.split( "\\s", 2 );
        Command retVal = new Command();

        // if command was found
        if ( items.length > 0 ) {
            retVal.setCommand( items[ 0 ] );

            // if items were found
            if ( items.length == 2 ) {
                retVal.setArgs( parseArgs( items[ 1 ] ) );
            }
        }

        return retVal;
	}
}   // Command
