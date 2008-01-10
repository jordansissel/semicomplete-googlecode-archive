#include <string>
#include <iostream>
#include <map>
#include <boost/xpressive/xpressive.hpp>
#include <boost/xpressive/regex_actions.hpp>
using namespace std;
using namespace boost::xpressive;

typedef map<string, string> ssmap_t;

struct is_private {
  bool operator()(ssub_match const &sub) const {
    sregex re = sregex::compile("^(?:192\\.168|10)\\.");
    smatch m;
    return !bool(regex_search(sub.str(), m, re));
  }
};

int main() {
  ssmap_t result;
  smatch m;
  string foo = "hello 1.2.3.4 4.5.6.7 192.168.0.5 bar";
  sregex re_orig = sregex::compile("(?:[0-9]+\\.){3}(?:[0-9]+)");
  sregex re = (s1=re_orig)[ check(is_private()) ];

  if (regex_search(foo, m, re)) {
    cout << m[0] << endl;
  }
  return 0;
}
