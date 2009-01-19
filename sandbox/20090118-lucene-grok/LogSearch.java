import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.IOException;
import java.util.Iterator;
import java.util.List;
import java.util.ArrayList;

import org.apache.lucene.analysis.standard.StandardAnalyzer;
import org.apache.lucene.document.Document;
import org.apache.lucene.document.Field;
import org.apache.lucene.index.IndexWriter;
import org.apache.lucene.queryParser.ParseException;
import org.apache.lucene.queryParser.QueryParser;
import org.apache.lucene.search.Hits;
import org.apache.lucene.search.Hit;
import org.apache.lucene.search.TopDocs;
import org.apache.lucene.search.ScoreDoc;
import org.apache.lucene.search.IndexSearcher;
import org.apache.lucene.search.Query;
import org.apache.lucene.search.Sort;
import org.apache.lucene.search.SortField;
import org.apache.lucene.store.Directory;
import org.apache.lucene.store.RAMDirectory;
import org.apache.lucene.store.FSDirectory;

import org.json.JSONObject;
import org.json.JSONException;

public class LogSearch {
  public static void main(String[] args) throws IOException, JSONException, ParseException {
    if (args.length < 1) {
      System.err.println("Usage: $0 <query> [field1 field2 ... fieldN]");
      System.exit(1);
    } 

    List fields = new ArrayList<String>();
    for (int i = 1; i < args.length; i++) {
      fields.add(args[i]);
    } 
    search(args[0], fields);
  }

  private static void search(String query, List fields) throws IOException, JSONException, ParseException {
    Directory dir = FSDirectory.getDirectory("/tmp/lucene.db");
    Query q = new QueryParser("@LINE", new StandardAnalyzer()).parse(query);
    IndexSearcher s = new IndexSearcher(dir);
    SortField sf = new SortField((String)fields.get(0));
    TopDocs results = s.search(q, null, 100, new Sort(sf));
    Float maxscore = results.getMaxScore();

    System.out.println("Found " + results.totalHits + " hits.");
    for (int i = 0; i < results.scoreDocs.length; i++) {
      ScoreDoc sd = results.scoreDocs[i];

      /* maxscore == best match, let's only show 'best match' results */
      if (sd.score < maxscore)
        break;

      Document doc = s.doc(sd.doc);
      //System.out.println(doc);
      Iterator<String> fielditer = fields.iterator();
      while (fielditer.hasNext()) {
        String field = fielditer.next();
        //System.out.println(field + "(" + sd.score + "): " + doc.get(field));
        System.out.println(field + ": " + doc.get(field));
      }
      System.out.println();
    }
  }
}
