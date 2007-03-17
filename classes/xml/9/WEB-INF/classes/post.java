
import javax.servlet.*;
import javax.servlet.http.*;
import java.io.*;
import java.util.Enumeration;


import javax.xml.transform.Source;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.stream.StreamSource;
import javax.xml.transform.stream.StreamResult;
import javax.xml.transform.TransformerConfigurationException;
import javax.xml.transform.TransformerException;
import java.io.*;


public class post  extends HttpServlet
{
	private final String DB_FILE = "db.xmlish";
	String html ="";
	String value ="";
	String file = "";

	public void search(HttpServletRequest req, HttpServletResponse resp) { } 
	public void createNew(HttpServletRequest req, HttpServletResponse resp) { 
		try {
			FileWriter db = new FileWriter(DB_FILE, true);
			Enumeration enoom = req.getParameterNames();
			db.write("<entry>\n");
			while (enoom.hasMoreElements()) {
				String key = (String) enoom.nextElement();
				String value = req.getParameterValues(key)[0];  
				db.write("<" + key + ">" + value + "</" + key + ">\n");
			}
			db.write("</entry>\n");
		} catch (Exception e) {
			try {
				resp.sendError(resp.SC_INTERNAL_SERVER_ERROR, e.getMessage());
			} catch (Exception em) {
				// Who cares, at this point...
				System.err.println("Something's wrong: " + em);
			}
		}
	}

	private void outputFile(String file2, PrintWriter ow) {
		String output = "";
		String tmp = "";
		try {
			File xmlFile = new File(file2);
			File xsltFile = new File(file2 + ".xsl");

			if (!xmlFile.exists()) {
				throw new FileNotFoundException(file2);
			}

			// JAXP reads data using the Source interface
			Source xmlSource = new StreamSource(xmlFile);
			Source xsltSource = new StreamSource(xsltFile);

			// the factory pattern supports different XSLT processors
			TransformerFactory transFact =
				TransformerFactory.newInstance();
			Transformer trans = transFact.newTransformer(xsltSource);

			trans.transform(xmlSource, new StreamResult(ow));
		} catch (FileNotFoundException e) {
			e.printStackTrace();
			ow.println(" Requested form (" + e + ") does not exist please contact administrator </html>");
		} catch (TransformerConfigurationException e) {
			e.printStackTrace();
			ow.println(" error: " + e);
		} catch (TransformerException e) {
			e.printStackTrace();
			ow.println("error: " + e);
		}


	}

	protected void doGet(HttpServletRequest req,
								HttpServletResponse res)
		throws ServletException, IOException
		{
			res.setContentType("text/html");
			PrintWriter out = res.getWriter();
			out.println("<html>");
			out.println("<body>");
			Enumeration enoom = req.getParameterNames();
			String tmp = (String) enoom.nextElement();
			if( tmp.equals("form") ){
				file = req.getParameterValues("form")[0];  
			} else {
				if( tmp.equals("submit") ){
					value = req.getParameterValues("form")[0];  
				} else {
					out.println("error in requests first param please contact administrator";
				}
			}
			if( file.equals("")){
				if( value.equals("new")){
					createNew(req, res);
				}else {
					if( value.equals("search")){
						search( req, res );
					}else {
						out.println("error in requests first param value please contact administraotr");
					}
				} 
			} else {
				if (file.contains(".."))
					res.sendError(res.SC_FORBIDDEN, "File: " + file + " is not allowed");
				outputFile("forms/" + file, out);
			}
			out.println("</body>");
			out.println("</html>");
			out.close();
		}

	private String createNew() {
		// TODO Auto-generated method stub
		return null;
	}

	public String getServletInfo()
	{
		return "HelloClientServlet 1.0 by Stefan Zeiger";
	}
}
