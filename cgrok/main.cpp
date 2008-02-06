
#include <iostream>
#include <fstream>
#include <string>
#include <boost/xpressive/xpressive.hpp>

#include "grokpatternset.hpp"
#include "grokregex.hpp"
#include "grokmatch.hpp"
#include "grokconfig.hpp"

using namespace std;
using namespace boost::xpressive;

#define CONFIG_BUFSIZE 4096

typedef map < string, WatchFileEntry > watch_map_type;

void grok_line(const FileObserver::data_pair_type &input_pair, 
               watch_map_type &watchmap) {
  const DataInput &di = input_pair.first;
  watch_map_type::iterator map_entry = watchmap.find(di.data);
  WatchFileEntry &wfe = (*map_entry).second;
  const string &line = input_pair.second;

  cout << "(" << di.data << ") " << line << endl;

  vector<WatchMatchType>::iterator wmt_iter;

  for (wmt_iter = wfe.match_types.begin();
       wmt_iter != wfe.match_types.end(); 
       wmt_iter++) {
    WatchMatchType::grok_regex_vector_type &gre_vector = \
      (*wmt_iter).match_strings;
    WatchMatchType::grok_regex_vector_type::iterator gre_iter;

    if (gre_vector.size() == 0) {
      cout << "Match by default." << endl;
      continue;
    }

    for (gre_iter = gre_vector.begin();
         gre_iter != gre_vector.end();
         gre_iter++) {
      /* XXX: gre.Search() modifies self, so we can't be const... blah */
      GrokRegex<sregex> &gre = (*gre_iter);
      GrokMatch<sregex> gm;
      if (gre.Search(line, gm)) {
        GrokMatch<sregex>::match_map_type::const_iterator m_iter;
        m_iter = gm.GetMatches().find("=MATCH");
        cout << "Match: " << (*m_iter).second << endl;
      }
    }
  }

}

int main(int argc, char **argv) {
  GrokConfig config;
  ifstream in(argv[1]);
  string config_data = "";
  char buffer[CONFIG_BUFSIZE];
  int bytes = 0;

  while (!(in.eof() || in.fail())) {
    in.read(buffer, CONFIG_BUFSIZE);
    bytes = in.gcount();
    buffer[bytes] = '\0';
    config_data += buffer;
  }

  //cout << config_data << endl;

  config.parse(config_data);

  GrokConfig::watch_file_vector_type::iterator watch_iter;
  GrokConfig::watch_file_vector_type watches;

  watches = config.GetFileEntries();
  map < string, WatchFileEntry > watchmap;
  FileObserver fo;
  for (watch_iter = watches.begin(); watch_iter != watches.end(); watch_iter++) {
    const vector<DataInput> &di_vector = (*watch_iter).fo.GetDataInputs();
    vector<DataInput>::const_iterator di_iter;
    cout << "FileObserver name: " << (*watch_iter).name << endl;
    for (di_iter = di_vector.begin(); di_iter != di_vector.end(); di_iter++) {
      cout << "File name: " << (*di_iter).data << endl;
      watchmap[(*di_iter).data] = *watch_iter;
    }
    fo.Merge((*watch_iter).fo);
  }

  FileObserver::data_input_vector_type input_vector;
  FileObserver::data_pair_type input_pair;
  FileObserver::data_input_vector_type::iterator input_iter;

  fo.OpenAll();
  while (!fo.DoneReading()) {
    fo.ReadLines(30, input_vector);
    for (input_iter = input_vector.begin(); 
         input_iter != input_vector.end();
         input_iter++) {
      input_pair = *input_iter;
      grok_line(input_pair, watchmap);
      //cout << "(" << input_pair.first.data << ") " << input_pair.second << endl;
      //cout << "watchmap: " << watchmap[input_pair.first.data].name << endl;
    }
    input_vector.clear();
  }
  return 0;
}
