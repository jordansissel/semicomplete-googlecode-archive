/*
 * element: Id attribute* '{' content* '}';
 * attribute: Id '=' String;
 * content: element | String;
 */

import java.io.*;
//import org.w3c.*;

import org.xml.sax.Attributes;
import org.xml.sax.helpers.*;

public class main {
	public static Dommy d;
	public static StreamTokenizer st;

	public static void main(String[] args) throws Exception {
		d = new Dommy();
		st = new StreamTokenizer(new BufferedReader(new InputStreamReader(System.in)));

		st.nextToken();

		if (st.ttype != st.TT_EOF) 
			element(st.sval);

		d.printTree();
		System.out.println();
		System.out.println();
		System.out.println();
	}

	public static void element(String name) throws Exception {
		Attributes a = new AttributesImpl();
		st.nextToken();

		if (st.TT_EOF == st.ttype) return;
		System.out.println("Element: " + name);

		if ('{' != st.ttype) {
			buildAttributesList(a);
		}

		/* Create the element */
		d.startElement("", "", name, a);

		st.nextToken();
		do {
			/* Handle children (text or elements) */
			elementOrContent(name);
			st.nextToken();
		} while ('}' != st.ttype);

		if ('}' == st.ttype) {
			st.nextToken();
			if (';' == st.ttype) {
				d.endElement("", "", name);
			} else {
				throw new OMGParseException(";");
			}
		} else {
			throw new OMGParseException("}");
		}
	}

	/* We just read a '{' 
	 * determine if we are content or an element
	 */
	public static void elementOrContent(String parent) throws Exception {
		System.out.println("elementOrContent: " + parent);
		//st.nextToken();
		if (st.TT_EOF == st.ttype) throw new OMGParseException("Unexpected EOF");

		///System.out.println(" --- st: " + (char)st.ttype);
		if ('"' == st.ttype) {
			System.out.println("Content: " + st.sval);
			/* This is the start of a string */
			d.characters(st.sval.toCharArray(), 0, st.sval.length());
		} else if ('}' == st.ttype) {
			/* Empty element??? */
		} else {
			element(st.sval);
		}

	}

	public static void buildAttributesList(Attributes a) throws Exception {

	}

	private static class UnexpectedEOFException extends Exception {
	}

	private static class OMGParseException extends Exception {
		public OMGParseException(String c) {
			super("Parse error!");
			String e;
			e = "Parse error while expecting '" + c + "' but got ";

			if (st.ttype > 0) {
				e += "'" + (char)st.ttype + "'";
			} else {
				e += "\"" + st.sval + "\"";
			}

			e += " instead.";
			System.err.println("ERROR:");
			System.err.println(e);
			System.err.println();
		}
	}


}
