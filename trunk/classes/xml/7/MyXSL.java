import java.io.IOException;
import java.io.InputStreamReader;
import java.io.StreamTokenizer;
import java.io.FileReader;
import java.util.HashMap;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.stream.StreamResult;
import javax.xml.transform.stream.StreamSource;
import javax.xml.transform.Source;
import org.xml.sax.SAXException;
import org.xml.sax.SAXParseException;
import org.xml.sax.helpers.AttributesImpl;


public class MyXSL extends MyXML {

	public MyXSL() throws Exception { super(null, null); }
	public MyXSL(StreamTokenizer st, Dommy d) throws Exception {
		super(st,d);
	}

	public static void main(String[] args) throws Exception {
		Dommy d = new Dommy();
		StreamTokenizer st;

		if (args.length < 2) {
			System.err.println("Usage: java MyXSL myxsl.mml [key=value ...] myxmldoc.xml");
			System.exit(1);
		}

		st = new StreamTokenizer(new FileReader(args[0]));

		st.resetSyntax();
		st.whitespaceChars(0, ' ');
		st.quoteChar('"'); st.quoteChar('\''); // strings in '' or ""
		st.wordChars('A', 'Z'); st.wordChars('a', 'z'); st.wordChars('0', '9'); // alphanum words
		st.wordChars('-', '-'); st.wordChars(':', ':'); st.wordChars('_', '_'); // and : - _
		st.commentChar('#');

		new MyXSL(st, d).parse();

		Transformer t = TransformerFactory.newInstance().newTransformer(new DOMSource(d.getDocument()));
		int i;
		for (i = 1; i < args.length; i++) {
			if (args[i].matches("^.+?=.+$")) {
				String[] sp = args[i].split("=",2);
				if (sp.length < 2) {
					System.err.println("Invalid argument '" + args[i] + "'");
				} else {
					t.setParameter(sp[0], sp[1]);
				}
			}
		}

		Source xml;
		if (args[i - 1].equals("-")) {
			xml = new StreamSource(System.in);
		} else {
			xml = new StreamSource(args[i - 1]);
		}

		t.setOutputProperty("method", "xml");
		t.setOutputProperty("indent", "yes");
		t.transform(xml, new StreamResult(System.out));
	}

	protected void content (Map map) throws IOException, ParseException, SAXException {
		switch (st.ttype) {
			case StreamTokenizer.TT_WORD:
				element_or_declaration(map);
				return;

			case '\'': case '"':
				handler.characters(st.sval.toCharArray(), 0, st.sval.length());
				st.nextToken(); // consume String
				return;

			default:
				throw new ParseException("expecting nested string or element");
		}
	}

	protected void element_or_declaration(Map map) throws IOException, ParseException, SAXException {
		String id = st.sval;
		st.nextToken(); 

		switch(st.ttype) {
			case '<': 
				st.nextToken();
			case '=':
				st.nextToken(); 
				System.out.println("declaration: " + st.sval);
				declaration(id, map);
				break;
			case '{':
				st.nextToken();
			default:
				st.pushBack();
				System.out.println("element: " + st.sval);
				System.out.println("element: " + (char) st.ttype);
				element(map);
		}
	}

	protected void declaration(String id, Map map) throws IOException, ParseException , SAXException{

		switch (st.ttype) {
			case '\'': case '"':
				/* Value is st.sval */
				break;
			default:
				content(map);
		}

		/* st.sval is the "value" of the variable */
		/* id is the name of the variable */

	}
}

