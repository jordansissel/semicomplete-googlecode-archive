/*
 * .java
 *
 * Version:
 *    $Id$
 *
 * Revisions:
 *    $Log$
 *    Revision 1.1  2004/01/20 03:57:34  psionic
 *    *** empty log message ***
 *
 *
 */

public class Player {
   private String name;
   private String whowanted;

   public Player(String n) {
      name = n;
   }

   public void wants(String who) {
      whowanted = who;
   }
}

