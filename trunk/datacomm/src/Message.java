/*
 * Message.java
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
 *   Revision 1.5  2004/01/19 19:23:35  psionic
 *   - changed validateArguments to public from protected
 *
 *   Revision 1.4  2004/01/19 05:56:00  psionic
 *   *** empty log message ***
 *
 *   Revision 1.3  2004/01/19 05:36:49  psionic
 *   - initial
 *
 *   Revision 1.2  2004/01/19 05:12:55  tristan
 *   required overriding toString
 *
 *   Revision 1.1  2004/01/19 04:52:55  tristan
 *   wrote message interface
 *
 */

import java.util.List;

/**
 * Interface for binary network messages.
 *
 * @author Tristan O'Tierney
 * @author Jordan Sissel
 * @author Nick Johnson
 */
public interface Message {
    public byte[] toByteArray();
    public String toString();
	public boolean validateArguments(List args)
	           throws InvalidCommandArgumentsException;
}   // Message

