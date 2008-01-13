
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
  //smatch m;
  //int ret;

  GrokRegex<sregex> gre("%NUMBER%");
  gre.AddPatternSet(default_set);
  //gre.Search(str);

  //ret = regex_search(str, m, pset.patterns["NUMBER"].regex);
  //cout << "Match: " << ret << endl;
  //cout << "M: " << m[0] << endl;

  return 0;
}
