
import org.xml.sax.helpers.DefaultHandler;
import org.w3c.dom.*;
import java.io.*;
import javax.xml.parsers.*;
import javax.xml.transform.*;
import javax.xml.transform.dom.*;
import javax.xml.transform.stream.*;
import java.util.LinkedList;
import org.xml.sax.Attributes;

public class Dommy extends DefaultHandler {

	private Document doc;
	private LinkedList stack;

	public Dommy() throws Exception {
		DocumentBuilderFactory dbf = DocumentBuilderFactory.newInstance();
		DocumentBuilder db = dbf.newDocumentBuilder();
		stack = new LinkedList();

		doc = db.newDocument();
		stack.add(doc);
	}

	public void startElement(String uri, String localname, String qname, Attributes attributes) {
		Element e; 
		e = doc.createElement(qname);
		Node n = (Node) stack.getLast();
		//System.out.println("<" + qname + "> / parent: " + n);

		for (int i = 0; i < attributes.getLength(); i++) {
			e.setAttribute(attributes.getQName(i), attributes.getValue(i));
		}
		
		n.appendChild(e);
		stack.add(e);
	}

	public void characters(char[] ch, int start, int length) {
		Text t;
		Node n = (Node) stack.getLast();

		t = doc.createTextNode(new String(ch, start, length));
		n.appendChild(t);
	}

	public void endElement(String uri, String localname, String qname) {
		//System.out.println("</" + qname + ">");
		stack.removeLast();
	}

	public void printTree() throws Exception {
		Transformer tf = TransformerFactory.newInstance().newTransformer();
		tf.transform(new DOMSource((Node) doc), new StreamResult(new OutputStreamWriter(System.out)));
	}

}
