/*
 * TCPPlayer.java
 *
 * Version:
 *    $Id$
 *
 * Revisions:
 *    $Log$
 *    Revision 1.2  2004/01/20 04:23:33  tristan
 *    updated comments for everything.
 *
 *    Revision 1.1  2004/01/20 03:57:35  psionic
 *    *** empty log message ***
 *
 *
 */

import java.net.Socket;

/**
 * Object for managing a player's connection.
 *
 * @author Tristan O'Tierney
 * @author Jordan Sissel
 * @author Nick Johnson
 */
public class TCPPlayer extends Player {
   private Socket sock;

   /**
    * Initializes a player's values.
    * @param n The player's name.
    * @param s The player's Socket.
    */
   public TCPPlayer(String n, Socket s) {
      super(n);
      sock = s;
   }
}
