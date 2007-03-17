/*
 * Plist-to-Container fun!
 *
 * $Id$
 */

import java.io.File;
import javax.xml.parsers.SAXParser;
import javax.xml.parsers.SAXParserFactory;
import org.xml.sax.Attributes;
import org.xml.sax.SAXParseException;
import org.xml.sax.helpers.DefaultHandler;
import java.util.*;

public class Plist {
	public static void main(String args[]) {
		SAXParserFactory pfac = SAXParserFactory.newInstance();
		SAXParser parser = null;

		File xml = new File(args[0]);

		if (!xml.exists()) {
			System.err.println("No such file " + args[0]);
			System.exit(1);
		}

		XMLHandler x = new XMLHandler();
		try {
			parser = pfac.newSAXParser();
			parser.parse(xml,x);
		} catch (Exception e) {
			System.err.println("Error parsing XML");
			System.err.println(e);
		}

		System.out.println(x.data);
	}

	/* Placeholder classes so we can easily figure out what's up */
	private static class Key { 
		public String key;
	}

	private static class XMLHandler extends DefaultHandler {
		private LinkedList stack;
		private int indent = 0;
		private Object data;

		public XMLHandler() {
			super();
			stack = new LinkedList();
		}

		public void startElement(String uri, String localname, String qname, Attributes attr) {
			for (int i = 0; i < indent; i++) System.out.print("   ");
			
			if (qname.equals("dict")) {
				stack.add(new ArrayList());
				System.out.println("<" + qname + "> (" + stack.getLast().getClass().getName() + ")");
			} else if (qname.equals("array")) {
				stack.add(new ArrayList());
				System.out.println("<" + qname + "> (" + stack.getLast().getClass().getName() + ")");
			} else if (qname.equals("key")) {
				stack.add(new String("DEFAULT_KEYNAME"));
				System.out.println("<" + qname + "> (" + stack.getLast().getClass().getName() + ")");
			} else if (qname.matches("string|data|date")) {
				stack.add(new String("DEFAULT"));
				System.out.println("<" + qname + "> (" + stack.getLast().getClass().getName() + ")");
			} else if (qname.equals("integer")) {
				stack.add(new Integer(-1));
				System.out.println("<" + qname + "> (" + stack.getLast().getClass().getName() + ")");
			} else if (qname.equals("real")) {
				stack.add(new Double(-1));
				System.out.println("<" + qname + "> (" + stack.getLast().getClass().getName() + ")");
			} else if (qname.matches("true")) {
				stack.add(new Boolean(true));
				System.out.println("<" + qname + "> (" + stack.getLast().getClass().getName() + ")");
			} else if (qname.matches("false")) {
				stack.add(new Boolean(false));
				System.out.println("<" + qname + "> (" + stack.getLast().getClass().getName() + ")");
			}
			indent++;
		}
		
		public void endElement(String uri, String localname, String qname) {
			/* Pop the top of the stack off */
			/* However, don't pop for 'key' 's - that gets handled specially */

			indent--;

			for (int i = 0; i < indent; i++) System.out.print("   ");
			System.out.print("</" + qname + ">");

			try {
				if (qname.matches("string|data|date|integer|real|true|false|key|array")) {
					Object o = stack.removeLast();
					System.out.print(" (" + o + ")");
					if (!stack.isEmpty()) {
						((ArrayList)stack.getLast()).add(o);
					} else {
						this.data = o;
					}
				} else if (qname.matches("dict")) {
					/* Turn this into a HashMap */
					Object o = stack.removeLast();
					ArrayList a = (ArrayList)o;
					HashMap h = new HashMap();
					Iterator i = a.iterator();
					boolean mod = false;
					Object key = null, value = null;
					while (i.hasNext()) {
						if (mod) { /* If mod is true, even numbered entry */
							value = i.next();
							h.put(key,value);
						} else {
							key = i.next();
						}
						mod = !mod;
					}
					if (!stack.isEmpty()) {
						((ArrayList)stack.getLast()).add(h);
					} else {
						this.data = h;
					}

				}
			} catch (Exception e) {
				System.err.println("Error: " + e);
				e.printStackTrace();
			}
			System.out.println();
		}

		public void characters(char[] ch, int start, int length) {
			for (int i = 0; i < indent; i++) System.out.print("   ");
			//System.out.print(String.copyValueOf(ch, start, length));
			
			/* We've got some character data, put it in the toplevel element */
			Object o = stack.getLast();
			if (o.getClass().getName().equals("java.lang.String")) {
				/* I haven't used java in a while, and references suck. */
				/* n will be our new string that's the actual PCDATA contents */
				/* Since doing 'o = n' doesn't work, becuase java is bad... */
				stack.removeLast();
				//System.out.print(String.copyValueOf(ch, start, length));
				stack.add(String.copyValueOf(ch, start, length));
				//System.out.print(String.copyValueOf(ch, start, length));
			}
			System.out.print(" (" + stack.getLast() + ")");

			System.out.println();
		}

		public void error(SAXParseException e) {
			System.err.println("Error: " + e);
			e.printStackTrace();
		}

		public void fatalError(SAXParseException e) {
			System.err.println("Fatal Error: " + e);
			e.printStackTrace();
		}
	}
}
