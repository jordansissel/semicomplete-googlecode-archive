/*
 * Message.java
 * 
 * Version:
 *   $Id$
 *
 * Revisions:
 *   $Log$
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
}   // Message

