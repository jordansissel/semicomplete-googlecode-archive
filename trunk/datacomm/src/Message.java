/*
 * Message.java
 * 
 * Version:
 *   $Id$
 *
 * Revisions:
 *   $Log$
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

/**
 * Interface for binary network messages.
 * 
 * @author tristan
 */
public interface Message {
    public byte[] toByteArray();
    public String toString();
	 protected boolean validateArguments();
}   // Message

