/*
 * UDPPlayer.java
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

public class UDPPlayer extends Player {
   private Socket sock;
   public UDPPlayer(String n, sock s) {
      super(n);
      sock = s;
   }
}
