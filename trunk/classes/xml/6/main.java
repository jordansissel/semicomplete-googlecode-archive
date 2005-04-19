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

		/* EOF here is non-fatal, produce an empty document */
		if (st.ttype != st.TT_EOF) {
			if (st.TT_WORD == st.ttype) element(st.sval);
			else throw new OMGParseException("TT_WORD");
		}

		d.printTree();
		//System.out.println();
		//System.out.println();
		System.out.println();
	}

	public static void element(String name) throws Exception {
		AttributesImpl a = new AttributesImpl();

		/* We need the next token here to see if it's a '{' or 
		 * the beginning of an attributes list
		 */
		st.nextToken();
		if (st.TT_EOF == st.ttype) throw new UnexpectedEOFException();

		if ('{' != st.ttype) {
			buildAttributesList(a);
		}

		/* Create the element */
		d.startElement("", "", name, a);

		st.nextToken();

		while ('}' != st.ttype)  {
			if (st.TT_EOF == st.ttype) throw new UnexpectedEOFException();
			/* Handle children (text or elements) */
			elementOrContent(name);
			st.nextToken();
		}

		//System.out.println("THIS SHOULD END THE ELEMENT " + name);
		if ('}' == st.ttype) {
			st.nextToken();
			if (st.TT_EOF == st.ttype) throw new UnexpectedEOFException();
			if (';' == st.ttype) {
				//System.out.println("ENDING ELEMENT " + name);
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
		//System.out.println("elementOrContent: " + parent);
		if (st.TT_EOF == st.ttype) throw new UnexpectedEOFException();

		//System.out.println(" --- st: " + (char)st.ttype);
		if ('"' == st.ttype) {
			//System.out.println("Content: " + st.sval);
			/* This is the start of a string */
			d.characters(st.sval.toCharArray(), 0, st.sval.length());
		} else if ('}' == st.ttype) {
			/* Empty element??? */
		} else {
			element(st.sval);
		}

	}

	public static void buildAttributesList(AttributesImpl a) throws Exception {
		String name, value;

		/* Expect a word */
		while (st.TT_WORD == st.ttype) {
			if (st.TT_EOF == st.ttype) throw new UnexpectedEOFException();

			//System.out.println("Attr name: " + st.sval);
			name = st.sval;

			/* Expect an '=' */
			st.nextToken();
			if (st.TT_EOF == st.ttype) throw new UnexpectedEOFException();
			if ('=' != st.ttype) throw new OMGParseException("=");

			/* Expect a word or string */
			st.nextToken();
			if (st.TT_EOF == st.ttype) throw new UnexpectedEOFException();
			if (st.TT_WORD != st.ttype && st.ttype < 0) 
				throw new OMGParseException("TT_WORD or STRING");
			
			value = st.sval;

			/* Expect a ';' */
			st.nextToken();
			if (st.TT_EOF == st.ttype) throw new UnexpectedEOFException();
			if (';' != st.ttype) throw new OMGParseException(";");

			a.addAttribute("", "", name, "CDATA", value);

			st.nextToken();
		}

		if (st.TT_EOF == st.ttype) throw new UnexpectedEOFException();
	}

	private static class UnexpectedEOFException extends Exception {
		public UnexpectedEOFException() {
			super("Unexpected end of line on input");
		}
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
