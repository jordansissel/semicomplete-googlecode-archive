/*
 * StartGameCommand.java
  *
 * Version:
 *   $Id$
 *
 * Revisions:
 *   $Log$
 *   Revision 1.1  2004/01/20 01:56:22  psionic
 *   - initial commit
 *
 *
 */

import java.util.List;

public class StartGameCommand extends Command {

	public StartGameCommand() {
		super("startgame");
	}

	public StartGameCommand(List args) {
		super("startgame", args);
	}

	public StartGameCommand(String args) throws InvalidCommandArgumentsException {
		super("startgame");
		this.args = this.parseArgs(args);
	}

	public boolean validateArguments(List args) 
	       throws InvalidCommandArgumentsException {
		System.err.println("StartGameCommand validateArguments()");

		// Number of arguments
		if (args.size() != 2) {
			System.err.println("- Failed num args ("+args+")");
			return false;
		}

		// the args are both strings.

		return true;
	}

} // StartGameCommand
