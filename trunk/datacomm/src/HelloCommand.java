/*
 * HelloCommand.java
  *
 * Version:
 *   $Id$
 *
 * Revisions:
 *   $Log$
 *   Revision 1.2  2004/01/20 03:57:34  psionic
 *   *** empty log message ***
 *
 *   Revision 1.1  2004/01/20 00:05:55  psionic
 *   - initial commit
 *
 *
 */

import java.util.List;

public class HelloCommand extends Command {

	public HelloCommand() {
		super("hello");
	}

	public HelloCommand(List args) {
		super("hello", args);
	}

	public HelloCommand(String args) throws InvalidCommandArgumentsException {
		super("hello");
		this.args = this.parseArgs(args);
	}

	public boolean validateArguments(List args) 
	       throws InvalidCommandArgumentsException {
		System.err.println("HelloCommand validateArguments()");

		// Number of arguments
		if ((args.size() != 1) || ( ((String)args.get(0)).equals("") )) {
			System.err.println("- Failed num args ("+args+")");
			return false;
		}

		return true;
	}

} // HelloCommand
