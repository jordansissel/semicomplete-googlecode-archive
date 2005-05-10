
import javax.servlet.*;
import javax.servlet.http.*;
import java.io.*;
import java.util.Enumeration;

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
			db.write("<foo>");
			db.write("<a/>");
			db.write("</foo>");
			db.close();
		} catch (Exception e) {
			try {
				resp.sendError(resp.SC_INTERNAL_SERVER_ERROR, e.getMessage());
			} catch (Exception em) {
				// Who cares, at this point...
				System.err.println("Something's wrong: " + em);
			}
		}
	}
	private String getFile(String file2) {
		String toReturn = "";
		String output = "";
		String tmp = "";
		try {
			BufferedReader in = new BufferedReader(new FileReader("forms/" + file2));
			output = in.readLine(); 
			while( !output.equals("")){
				output = in.readLine();
				if ( output.contains("text") ){
					tmp = output.substring(output.indexOf("name=")+6);
					toReturn += output.substring(0,output.indexOf("\""));
					toReturn += output;
					toReturn += "</br>";
				} else {
					toReturn += output;
				}
			}
		} catch (FileNotFoundException e) {
			e.printStackTrace();
			return " Requested form does not exist please contact administrator </html>";

		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			return " I/O error please contact administrator </html>";

		}
		return toReturn;
	}

	protected void doGet(HttpServletRequest req,
								HttpServletResponse res)
		throws ServletException, IOException
		{
			res.setContentType("text/html");
			PrintWriter out = res.getWriter();
			html = "<HTML>";
			Enumeration enoom = req.getParameterNames();
			String tmp = (String) enoom.nextElement();
			if( tmp.equals("form") ){
				file = req.getParameterValues("form")[0];  
			} else {
				if( tmp.equals("submit") ){
					value = req.getParameterValues("form")[0];  
				} else {
					html = "<html> error in requests first param please contact administrator </html>";
				}
			}
			if( file.equals("")){
				if( value.equals("new")){
					html ="";
					createNew(req, res);
				}else {
					if( value.equals("search")){
						html ="";
						search( req, res );
					}else {
						html = "<html> error in requests first param value please contact administrator </html>";
					}
				} 
			} else {
				html += getFile(file);
			}
			out.println(html);
			out.close();
		}

	private String getFile() {
		// TODO Auto-generated method stub
		return null;
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
