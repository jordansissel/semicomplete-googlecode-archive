/*
 * BoardupdateCommand.java
 *
 * Version:
 *   $Id$
 *
 * Revisions:
 *   $Log$
 *   Revision 1.1  2004/02/15 22:37:26  psionic
 *   - New commands Boardupdate and Shipupdate
 *   - Added random selection for which player goes first.
 *   - Players can start games now
 *
 *
 */

import java.util.List;

/**
 * A class for sending an boardupdate command.
 *
 * @author Jordan Sissel
 */
public class BoardupdateCommand extends Command {

	/**
	 * Initializes the BoardupdateCommand command.
	 */
	public BoardupdateCommand() {
		super("BoardupdateCommand");
	}

	/**
	 * Initialize the BoardupdateCommand command with args.
	 * @param args The args to BoardupdateCommand.
	 */
	public BoardupdateCommand(List args) {
		super("BoardupdateCommand", args);
	}

	/**
	 * Initializes the command with a string of args to parse.
	 * @param args The args to parse.
	 * @throws InvalidCommandArgumentsException If the args didn't parse right.
	 */
	public BoardupdateCommand(String args) 
		    throws InvalidCommandArgumentsException {
		super("shipupdate");
		this.args = parseArgs(args);
	}

	/**
	 * Validates the arguments.
	 * @param args The arguments to check.
	 * @return If the args are validated.
	 * @throws InvalidCommandArgumentsException If the args are parsed wrong.
	 */
	public boolean validateArguments(List args)
		throws InvalidCommandArgumentsException {
			System.err.println("BoardupdateCommand validateArguments()");

			// Number of arguments
			if ((args.size() != 100) || ( ((String)args.get(0)).equals("") )) {
				System.err.println("- Failed num args ("+args+")");
				return false;
			}

			return true;
		}
} // BoardupdateCommand
