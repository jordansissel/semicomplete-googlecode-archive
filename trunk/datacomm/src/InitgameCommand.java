/*
 * InitgameCommand.java
 *
 * Version:
 *   $Id$
 *
 * Revisions:
 *   $Log$
 *   Revision 1.1  2004/02/14 02:30:36  psionic
 *   - initial commit
 *
 *
 */

import java.util.List;

/**
 * A class for responding hello.
 *
 * @author Tristan O'Tierney
 * @author Jordan Sissel
 * @author Nick Johnson
 */
public class InitgameCommand extends Command {

    /**
     * Initializes the hello command.
     */
	public InitgameCommand() {
		super("hello");
	}

    /**
     * Initialize the hello command with args.
     * @param args The args to initialize.
     */
	public InitgameCommand(List args) {
		super("hello", args);
	}

    /**
     * Initializes the command with a string of args to parse.
     * @param args The args to parse.
     * @throws InvalidCommandArgumentsException If the args didn't parse right.
     */
	public InitgameCommand(String args) throws InvalidCommandArgumentsException {
		super("hello");
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
		System.err.println("InitgameCommand validateArguments()");

		// Number of arguments
		if ((args.size() != 1) || ( ((String)args.get(0)).equals("") )) {
			System.err.println("- Failed num args ("+args+")");
			return false;
		}

		return true;
	}
} // InitgameCommand
