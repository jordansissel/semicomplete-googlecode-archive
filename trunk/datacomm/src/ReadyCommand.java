/*
 * ReadyCommand.java
  *
 * Version:
 *   $Id$
 *
 * Revisions:
 *   $Log$
 *   Revision 1.1  2004/01/20 00:05:55  psionic
 *   - initial commit
 *
 *
 */

import java.util.List;

public class ReadyCommand extends Command {

	public ReadyCommand() {
		super("ready");
	}

	public ReadyCommand(List args) {
		super("ready", args);
	}

	public ReadyCommand(String args) throws InvalidCommandArgumentsException {
		super("ready");
		this.args = this.parseArgs(args);
	}

	public boolean validateArguments(List args) 
	       throws InvalidCommandArgumentsException {
		System.err.println("ReadyCommand validateArguments()");

		// Number of arguments
		if (args.size() != 1) {
			System.err.println("- Failed num args ("+args+")");
			return false;
		}

		return true;
	}

} // ReadyCommand
