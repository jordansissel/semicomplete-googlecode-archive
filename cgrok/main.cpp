
#include <iostream>
#include <fstream>
#include <string>
#include <boost/xpressive/xpressive.hpp>

using namespace std;
using namespace boost::xpressive;

#include "grokpatternset.hpp"
#include "grokregex.hpp"
#include "grokmatch.hpp"

int main(int argc, char **argv) {
  GrokPatternSet<sregex> default_set;
  ifstream pattern_file("./patterns");
  string pattern_desc;
  const string delim(" \t");

  while (getline(pattern_file, pattern_desc)) {
    string::size_type pos;
    string name;
    string regex_str;
    pos = pattern_desc.find_first_of(delim, 0);
    name = pattern_desc.substr(0, pos);
    pos = pattern_desc.find_first_not_of(delim, pos);
    regex_str = pattern_desc.substr(pos, pattern_desc.size() - pos);
    //default_set.AddPattern(name, regex_str);
    cout << name << " => " << regex_str << endl;
  }



  if (argc != 2) {
    cout << "Usage: $0 [regexp]" << endl;
    exit(1);
  }

  GrokMatch<sregex> *gm;
  GrokRegex<sregex> gre(argv[1]);
  gre.AddPatternSet(default_set);

  GrokMatch<sregex>::match_map_type m;
  string str = "1.344 bar";
  gm = gre.Search(str);
  m = gm->GetMatches();

  return 0;
}
