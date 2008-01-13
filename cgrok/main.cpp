
#include <iostream>
#include <string>
#include <boost/xpressive/xpressive.hpp>

using namespace std;
using namespace boost::xpressive;

#include "grokpatternset.hpp"
#include "grokregex.hpp"

int main(int argc, char **argv) {
  GrokPatternSet<sregex> default_set;
  GrokPatternSet<sregex> pset;
  default_set.AddPattern("WORD", "\\w+");
  default_set.AddPattern("NUMBER", "(?:[+-]?(?:(?:[0-9]+(?:\\.[0-9]*)?)|(?:\\.[0-9]+)))");

  pset.Merge(default_set);

  string str = "   Hello 3.4414  ";

  if (argc != 2) {
    cout << "Usage: $0 [regexp]" << endl;
    exit(1);
  }

  GrokRegex<sregex> gre(argv[1]);
  gre.AddPatternSet(default_set);
  //gre.Search(str);

  return 0;
}
