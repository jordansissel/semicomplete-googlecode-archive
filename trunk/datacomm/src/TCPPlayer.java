/*
 * TCPPlayer.java
 *
 * Version:
 *    $Id$
 *
 * Revisions:
 *    $Log$
 *    Revision 1.1  2004/01/20 03:57:35  psionic
 *    *** empty log message ***
 *
 *
 */

public class TCPPlayer extends Player {
   private Socket sock;
   public TCPPlayer(String n, sock s) {
      super(n);
      sock = s;
   }
}
