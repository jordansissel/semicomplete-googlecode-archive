/*
 * ShipupdateCommand.java
 *
 * Version:
 *   $Id$
 *
 * Revisions:
 *   $Log$
 *   Revision 1.1  2004/02/15 22:37:30  psionic
 *   - New commands Boardupdate and Shipupdate
 *   - Added random selection for which player goes first.
 *   - Players can start games now
 *
 *
 *
 */

import java.util.List;

/**
 * A class for sending an shipupdate command.
 *
 * @author Jordan Sissel
 */
public class ShipupdateCommand extends Command {

	/**
	 * Initializes the shipupdate command.
	 */
	public ShipupdateCommand() {
		super("shipupdate");
	}

	/**
	 * Initialize the shipupdate command with args.
	 * @param args The args to initialize.
	 */
	public ShipupdateCommand(List args) {
		super("shipupdate", args);
	}

	/**
	 * Initializes the command with a string of args to parse.
	 * @param args The args to parse.
	 * @throws InvalidCommandArgumentsException If the args didn't parse right.
	 */
	public ShipupdateCommand(String args) throws InvalidCommandArgumentsException {
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
			System.err.println("ShipupdateCommand validateArguments()");

			// Number of arguments
			if ((args.size() != 20) || ( ((String)args.get(0)).equals("") )) {
				System.err.println("- Failed num args ("+args+")");
				return false;
			}

			return true;
		}
} // ShipupdateCommand
