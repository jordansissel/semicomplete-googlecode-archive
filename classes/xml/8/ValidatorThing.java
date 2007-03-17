
//import org.apache.xerces.xs.*;
//import org.apache.xerces.dom3.*;
import org.xml.sax.helpers.DefaultHandler;
import org.apache.xerces.parsers.SAXParser;
import org.xml.sax.SAXException;
import org.xml.sax.SAXParseException;
import java.io.IOException;

/* Most of the ideas in this come from the samples
 * Other help came from the xerces java docs and
 * googling for 'xerces java schema validation'
 *
 * Validation is pretty straight forward
 * -jordan
 */

public class ValidatorThing {
	public static void main(String[] args) throws Exception {
		SAXParser parser = new SAXParser();
		String schema = null, xmldoc = null;
		
		if (args.length != 2)
			usage();

		schema = args[0];
		xmldoc = args[1];

		//try {
			parser.setFeature("http://xml.org/sax/features/validation", Boolean.TRUE);
			parser.setFeature("http://apache.org/xml/features/validation/schema", Boolean.TRUE);
			parser.setFeature("http://apache.org/xml/features/validation/schema-full-checking", Boolean.TRUE);
			parser.setProperty("http://apache.org/xml/properties/schema/external-noNamespaceSchemaLocation", schema);

			ErrorHandler eh = new ErrorHandler();
			parser.setErrorHandler(eh);
			parser.parse(xmldoc);

			if (eh.hasfailed) {
				System.out.println("Document is not valid: " + eh.exception);
			} else {
				System.out.println("Document is valid!");
			}
/*
		} catch (IOException e) {
			System.err.println(e);
		} catch (SAXException e) {
			System.err.println(e);
		}
*/
	}

	public static void usage() {
		System.err.println("Usage: java ValidatorThing [schemaurl] [xmldocurl]");
		System.exit(1);
	}

	private static class ErrorHandler extends DefaultHandler {
		public boolean hasfailed = false;
		public SAXParseException exception = null;

		public void error(SAXParseException e) throws SAXException {
			hasfailed = true;
			exception = e;
		}

		public void fatalError(SAXParseException e) throws SAXException {
			hasfailed = true;
			exception = e;
		}
	}
}
