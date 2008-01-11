
#include <iostream>
#include <string>
using namespace std;

#include "grokregex.h"

int main(int argc, char **argv) {
  if (argc == 1) {
    cout << "Usage: $0 pattern" << endl;
    return 1;
  }

  string pattern(argv[1]);
  GrokRegex foo(pattern);

  foo.search("foo");
  foo.search("foo bar baz");
  foo.search("test %FOO% bar");

  return 0;
}
