/*
 * FireCommand.java
 *
 * Version:
 *   $Id$
 *
 * Revisions:
 *   $Log$
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

public class FireCommand extends Command {

	public FireCommand() {
		super("fire");
	}

	public FireCommand(List args) {
		super("fire", args);
	}

	public FireCommand(String args) throws InvalidCommandArgumentsException {
		super("fire");
		this.args = this.parseArgs(args);
	}

	public boolean validateArguments() 
	        throws InvalidCommandArgumentsException {
		System.err.println("FireCommand validateArguments()");

		// Number of arguments
		if (this.args.size() != 2) {
			System.err.println("- Failed num args");
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
