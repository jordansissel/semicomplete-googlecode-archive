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
import org.json.JSONArray;
import org.json.JSONException;

public class GrokJSONImport {
  public static void main(String[] args) throws IOException, JSONException, ParseException {
    buildIndex();
  }

  private static void buildIndex() throws IOException, JSONException, ParseException {
    String line = null;
    Boolean done = false;
    BufferedReader stdin = new BufferedReader(new InputStreamReader(System.in));

    //Directory index = new RAMDirectory();
    Directory dir = FSDirectory.getDirectory("/tmp/lucene.db");
    IndexWriter w = new IndexWriter(dir, new StandardAnalyzer(), true);

    while (!done) {
      try {
        line = stdin.readLine();
      } catch (IOException e) { /* ignore */ }

      if (line == null) {
        done = true;
        break;
      }

      Document doc = new Document();
      //Field fullentry = new Field("FULLTEXT", line,
          //Field.Store.COMPRESS, Field.Index.NO);
      //doc.add(fullentry);

      JSONObject obj = new JSONObject(line);
      /* Each grok object starts with { "grok": [ ... ] } */
      JSONArray fieldlist = obj.getJSONArray("grok"); 
      for (int i = 0; i < fieldlist.length(); i++) {
        obj = fieldlist.getJSONObject(i);
        addJsonFieldsToDoc(doc, obj);
      }
      w.addDocument(doc);
    }

    w.close();

  }

  private static void addJsonFieldsToDoc(Document doc, JSONObject obj) throws JSONException{
    Iterator<String> i = obj.keys();
    while (i.hasNext()) {
      String key = i.next();
      //System.out.println(key + " => " + value);
      /* grok json output is: 
       * "field": { "start": N, "end": N, "value": string }
       */
      String value = obj.getJSONObject(key).getString("value");

      /* If the key has a subname, like NUMBER:httpversion, set the
       * field name to 'httpversion' */
      if (key.indexOf(':') != -1) {
        key = key.split(":")[1];
      }
      Field field = new Field(key, value,
          Field.Store.COMPRESS, Field.Index.ANALYZED);
      doc.add(field);
    }
  }
}
