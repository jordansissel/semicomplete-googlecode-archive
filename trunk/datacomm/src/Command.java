/*
 * Command.java
 *
 * Version:
 *	$Id$
 *
 * Revisions:
 *	$Log$
 *	Revision 1.14  2004/01/20 08:48:08  tristan
 *	added commenting.
 *
 *	Revision 1.13  2004/01/20 04:23:33  tristan
 *	updated comments for everything.
 *	
 *	Revision 1.12  2004/01/20 00:12:01  psionic
 *	- Fixed a little bug
 *	
 *	Revision 1.11  2004/01/19 23:48:52  psionic
 *	- Bulk commit: Command infrastructure improved. New commands can be added
 *	  dynamically now.
 *	
 *	Revision 1.10  2004/01/19 16:58:32  tristan
 *	changed two methods to static
 *	
 *	Revision 1.9  2004/01/19 16:17:42  tristan
 *	added char in tobytearray
 *	
 *	Revision 1.8  2004/01/19 06:13:02  tristan
 *	write toByteArray
 *	
 *	Revision 1.7  2004/01/19 05:36:49  psionic
 *	- initial
 *	
 *	Revision 1.6  2004/01/19 04:53:51  psionic
 *	- Added framework for generic syntax checking
 *	
 *	Revision 1.5  2004/01/19 02:25:24  psionic
 *	- Added validateArgument method. This will use the meta syntax specified by
 *	  each command and validate that it is indeed the proper data type.
 *	
 *	Revision 1.4  2004/01/17 18:22:08  tristan
 *	fixed parse bug
 *
 *	Revision 1.3  2004/01/14 06:02:14  tristan
 *	removed unneeded var
 *
 *	Revision 1.2  2004/01/14 05:53:33  tristan
 *	finished command parsing
 *
 *	Revision 1.1  2004/01/14 04:36:04  tristan
 *	initial
 *
 */

import java.util.List;
import java.util.Iterator;
import java.util.ArrayList;
import java.util.regex.Pattern;
import java.util.regex.Matcher;
import java.io.ByteArrayOutputStream;
import java.io.DataOutputStream;

/**
 * Command sent to a client from the server.
 *
 * @author Tristan O'Tierney
 * @author Jordan Sissel
 * @author Nick Johnson
 */
public class Command implements Message {

	/* Regular expression to parse a command */
	public static final Pattern argsPattern = Pattern.compile(
		"\\s*(.*?)\\s*,{1}\\s*(.*)\\s*"
	);

	/* Syntax definition for how the arguments should be defined. */
	public String[] getSyntax() { String[] a = { }; return a; }

	protected String command;
	protected List args;

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
		this.command = command;
		this.args = args;
	}

    /**
     * Initializes a command and it's arguments.
     * @param command The command var.
     * @param args The command's arguments.
     * @throws InvalidCommandArgumentsException Thrown if args aren't parsed.
     */
	public Command( String command, String args )
	       throws InvalidCommandArgumentsException {
		this.command = command;
		this.args = parseArgs(args);
	}

	/**
	 * Parses and initializes the command variable.
	 * @param command The command.
	 */
	public Command( String command ) {
		this( command, new ArrayList() );
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
	public void setArgs( List args ) throws InvalidCommandArgumentsException {
		if ( ! this.validateArguments(args) ) {
			throw new InvalidCommandArgumentsException();
		}

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
	public static List parseArgs( String unparsedArgs )
	       throws InvalidCommandArgumentsException {
		List retVal = new ArrayList();
		int cur = 0;

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
			cur++;
		} while ( true );

		// add trailing arg
		if ( unparsedArgs != null ) {
			retVal.add( unparsedArgs );
		}

		/*if ( ! this.validateArguments(retVal) ) {
			throw new InvalidCommandArgumentsException();
		}*/

		return retVal;
	}

	/**
	 * Parses a command and it's arguments.
	 * @param unparsedCommand The command to parse.
	 */
	public static Command parseCommand( String unparsedCommand )
	       throws InvalidCommandArgumentsException {
		String[] items = unparsedCommand.split( "\\s", 2 );
		items[0] = items[0].substring(0,1).toUpperCase() + 
		           items[0].substring(1).toLowerCase();

		Command retVal = null;

		try {
			retVal = (Command) Class.forName(items[0] + "Command").newInstance();
		} catch (Exception e) {
			System.err.println(" -> " + e);
		}
		if (retVal == null) {
			return null;
		}

		// if command was found
		if ( items.length > 0 ) {
			retVal.setCommand( items[ 0 ] );

			// if items were found
			if ( items.length == 2 ) {
				System.err.println("Parseargs ("+items[0]+") - " + items[1]);
				retVal.setArgs( parseArgs( items[ 1 ] ) );
			}
		}

		return retVal;
	}

	/**
	 * Validate argument data type
	 *
	 */
	public boolean validateArguments(List args) 
	        throws InvalidCommandArgumentsException {
		return true;
	}

    /**
     * Returns the command as a byte array.
     * @return The command in byte array form.
     */
    public byte[] toByteArray() {
        ByteArrayOutputStream out = new ByteArrayOutputStream();
        DataOutputStream data = new DataOutputStream( out );

        try {
            Iterator i = args.iterator();
            data.writeUTF( command );

            while ( i.hasNext() ) {
                Object item = i.next();

                if ( item instanceof Integer ) {
                    data.writeInt( ( ( Integer ) item ).intValue() );
                } else if ( item instanceof Byte ) {
                    data.writeByte( ( ( Byte ) item ).byteValue() );
                } else if ( item instanceof Double ) {
                    data.writeDouble( ( ( Double ) item ).doubleValue() );
                } else if ( item instanceof Float ) {
                    data.writeFloat( ( ( Float ) item ).floatValue() );
                } else if ( item instanceof Boolean ) {
                    data.writeBoolean( ( ( Boolean ) item ).booleanValue() );
                } else if ( item instanceof Short ) {
                    data.writeShort( ( ( Short ) item ).shortValue() );
                } else if ( item instanceof Long ) {
                    data.writeLong( ( ( Long ) item ).longValue() );
                } else if ( item instanceof Character ) {
                    data.writeChar( ( ( Character ) item ).charValue() );
                } else {
                    data.writeUTF( item.toString() );
                }

            }
        } catch ( Exception e ) {
        }

        return out.toByteArray();
    }
}	// Command.java
