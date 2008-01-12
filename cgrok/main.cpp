
#include <iostream>
#include <string>
#include "grokregex.h"
#include "grokpatterns.h"
using namespace std;
using namespace boost::xpressive;


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

  pset.AddPattern("WORD", "\b\\w+\b");
  pset.AddPattern("NUMBER", "(?:[+-]?(?:(?:[0-9]+(?:\\.[0-9]*)?)|(?:\\.[0-9]+)))");
}
