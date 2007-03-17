/*
 * FireCommand.java
 *
 * Version:
 *   $Id$
 *
 * Revisions:
 *   $Log$
 *   Revision 1.7  2004/01/20 04:23:33  tristan
 *   updated comments for everything.
 *
 *   Revision 1.6  2004/01/19 23:48:52  psionic
 *   - Bulk commit: Command infrastructure improved. New commands can be added
 *     dynamically now.
 *
 *   Revision 1.5  2004/01/19 19:23:13  psionic
 *   - validateArguments changed to public from protected
 *
 *   Revision 1.4  2004/01/19 05:56:00  psionic
 *   *** empty log message ***
 *
 *   Revision 1.3  2004/01/19 05:36:49  psionic
 *   - initial
 *
 *   Revision 1.2  2004/01/19 04:54:07  psionic
 *   - Added generic framework for syntax checking
 *
 *   Revision 1.1  2004/01/18 23:54:53  psionic
 *   - Stub written for FireCommand
 *
 *
 */

import java.util.List;

/**
 * Creates a command that fires at the other player.
 *
 * @author Tristan O'Tierney
 * @author Jordan Sissel
 * @author Nick Johnson
 */
public class FireCommand extends Command {
    /**
     * Initializes a fire command.
     */
	public FireCommand() {
		super("fire");
	}

    /**
     * Initializes a fire command with args.
     * @param args The args to initialize.
     */
	public FireCommand(List args) {
		super("fire", args);
	}

    /**
     * Creates a fire command with a string of args.
     * @param args The args to set.
     * @throws InvalidCommandArgumentsException If the args are bad.
     */
	public FireCommand(String args) throws InvalidCommandArgumentsException {
		super("fire");
		this.args = parseArgs(args);
	}

    /**
     * Validates the arguments for a fire command.
     * @param args The args to validate
     * @return If the args are correct.
     * @throws InvalidCommandArgumentsException If the args are bad.
     */
	public boolean validateArguments(List args)
	        throws InvalidCommandArgumentsException {
		System.err.println("FireCommand validateArguments()");

		// Number of arguments
		if (args.size() != 2) {
			System.err.println("- Failed num args ("+args+")");
			return false;
		}

		// First Argument
		String arg = (String) args.get(0);
		if ( (arg.length() > 1) || (!arg.matches("[a-jA-J]")) ) {
			System.err.println("- Failed arg 1");
			return false;
		}

		// Second Argument
		try {
			Integer i = new Integer((String)args.get(1));
		} catch (Exception e) {
			System.err.println("- Failed arg 1");
			return false;
		}

		return true;
	}
}
