/*
 * StartgameCommand.java
  *
 * Version:
 *   $Id$
 *
 * Revisions:
 *   $Log$
 *   Revision 1.2  2004/01/26 21:44:40  psionic
 *   *** empty log message ***
 *
 *   Revision 1.1  2004/01/26 21:21:28  psionic
 *   - Updated commands framework, added game-starting ability
 *
 *   Revision 1.2  2004/01/20 04:23:33  tristan
 *   updated comments for everything.
 *
 *   Revision 1.1  2004/01/20 01:56:22  psionic
 *   - initial commit
 *
 *
 */

import java.util.List;

/**
 * Command for synchronizing clients.
 *
 * @author Tristan O'Tierney
 * @author Jordan Sissel
 * @author Nick Johnson
 */
public class StartgameCommand extends Command {

    /**
     * Creates a new startgame object.
     */
	public StartgameCommand() {
		super("startgame");
	}

    /**
     * Creates a new startgame with args.
     * @param args The args to set.
     */
	public StartgameCommand(List args) {
		super("startgame", args);
	}

    /**
     * Creates a new start game and parses the args sent to it.
     * @param args The args to parse.
     * @throws InvalidCommandArgumentsException If there was an error parsing.
     */
	public StartgameCommand(String args) throws InvalidCommandArgumentsException {
		super("startgame");
		this.args = parseArgs(args);
	}

    /**
     * Validates the arguments sent to this command.
     * @param args The args to validate.
     * @return True if the args are correct.
     * @throws InvalidCommandArgumentsException If the args failed to parse.
     */
	public boolean validateArguments(List args)
	       throws InvalidCommandArgumentsException {
		System.err.println("StartgameCommand validateArguments()");

		// Number of arguments
		if (args.size() != 2) {
			System.err.println("- Failed num args ("+args+")");
			return false;
		}

		// the args are both strings.

		return true;
	}
} // StartgameCommand
