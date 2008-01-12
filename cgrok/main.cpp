
#include <iostream>
#include <string>
#include <boost/xpressive/xpressive.hpp>

using namespace std;
using namespace boost::xpressive;

#include "grokpatternset.hpp"

#if 0
int main_1(int argc, char **argv) {
  if (argc == 1) {
    cout << "Usage: $0 pattern" << endl;
    return 1;
  }

  string pattern(argv[1]);
  GrokRegex foo(pattern);

  for (int x = 2; x < argc; x++) {
    foo.search(string(argv[x]));
  }
  return 0;
}
#endif

int main(int argc, char **argv) {
  GrokPatternSet<sregex> pset;
  pset.AddPattern("WORD", "\\w+");
  pset.AddPattern("NUMBER", "(?:[+-]?(?:(?:[0-9]+(?:\\.[0-9]*)?)|(?:\\.[0-9]+)))");

  string str = "   Hello 3.4414  ";
  smatch m;
  int ret;

  ret = regex_search(str, m, pset.patterns["NUMBER"].regex);
  cout << "Match: " << ret << endl;
  cout << "M: " << m[0] << endl;

  return 0;
}
