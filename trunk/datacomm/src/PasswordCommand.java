/*
 * PasswordCommand.java
 *
 * Version:
 *   $Id$
 *
 * Revisions:
 *   $Log$
 *   Revision 1.1  2004/02/19 06:42:47  psionic
 *   *** empty log message ***
 *
 *
 */

import java.util.List;

/**
 * A class for responding hello.
 *
 * @author Jordan Sissel
 */
public class PasswordCommand extends Command {

	/**
	 * Initializes the hello command.
	 */
	public PasswordCommand() {
		super("password");
	}

	/**
	 * Initialize the hello command with args.
	 * @param args The args to initialize.
	 */
	public PasswordCommand(List args) {
		super("password", args);
	}

	/**
	 * Initializes the command with a string of args to parse.
	 * @param args The args to parse.
	 * @throws InvalidCommandArgumentsException If the args didn't parse right.
	 */
	public PasswordCommand(String args) throws InvalidCommandArgumentsException {
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

			// Number of arguments
			if ((args.size() != 1) || ( ((String)args.get(0)).equals("") )) {
				return false;
			}

			return true;
		}
} // PasswordCommand
