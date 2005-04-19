
import org.xml.sax.helpers.DefaultHandler;
import org.w3c.dom.*;
import javax.xml.parsers.DocumentBuilderFactory;

public class Dommy extends DefaultHandler {

	private Document doc;

	public Dommy() {
		DocumentBuilderFactory dbf = DocumentBuilderFactory.newInstance();
		DocumentBuilder db = dbf.newDocumentBuilder()

		doc = db.newDocument();
	}

}
