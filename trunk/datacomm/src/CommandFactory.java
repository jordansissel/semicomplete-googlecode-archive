/*
 * CommandFactory.java
 * 
 * Version:
 *   $Id$
 *
 * Revisions:
 *   $Log$
 *   Revision 1.1  2004/01/19 04:33:42  tristan
 *   added command factory parser
 *
 */

/**
 * Factory for creating commands by command name.
 *
 * @author Tristan O'Tierney
 */
public class CommandFactory {

	// vars
	private String packageName;

	/**
	 * Not able to create an instance
	 */
	public CommandFactory( String packageName ) {
		setPackageName( packageName );
	}

	/**
	 * Returns the package name for this command factory.
	 * @return The package name.
	 */
	public String getPackageName() {
		return packageName;
	}

	/**
	 * Changes the package name for this command factory.
	 * @param packageName The new package name.
	 */
	public void setPackageName( String packageName ) {
		this.packageName = packageName;
	}

	/**
	 * Creates a command based on it's name.
	 * @param commandName The command to create.
	 * @return The instantiated command (or null if no command found).
	 */
	public Command createCommand( String commandName ) {
        Command parse = Command.parseCommand( commandName );
		Command command = null;

        try {
            String name = parse.getCommand();

            if ( packageName != null ) {
                name = packageName + "." + name;
            }

            command = ( Command ) Class.forName( name ).newInstance();

            // assign values
            if ( command != null ) {
                command.setCommand( parse.getCommand() );
                command.setArgs( parse.getArgs() );
            }
        } catch ( Exception e ) {
            // command was not found.
        }

		return command;
	}

}   // CommandFactory