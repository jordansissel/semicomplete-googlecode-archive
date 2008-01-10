
#include <iostream>
#include <string>
#include "grokregex.h"
#include <exception>
#include <boost/xpressive/xpressive.hpp>
#include <boost/xpressive/regex_actions.hpp>
using namespace std;
using namespace boost::xpressive;


GrokRegex::GrokRegex(const string grok_pattern) 
                    : pattern(grok_pattern) {
  this->GenerateRegexFromPattern(pattern);
}

GrokRegex::~GrokRegex() {
  // clean up stuff, but what?
}

void GrokRegex::GenerateRegexFromPattern(const string pattern) {
  sregex notpercent = +~(as_xpr('%'));
  mark_tag pattern_expr_capture(1);
  unsigned int last_pos = 0;

  sregex pattern_expr_re(
    as_xpr('%')
      >> (pattern_expr_capture=notpercent)
    >> '%'
  );

  this->regex = sregex::compile("");
  string test("");

  smatch m;
  cout << "Testing: " << pattern << endl;

  sregex_iterator cur(pattern.begin(), pattern.end(), pattern_expr_re);
  sregex_iterator end;

  for (; cur != end; cur++) {
    smatch const &what = *cur;
    if (what.position() > last_pos) {
      this->regex >>= pattern.substr(last_pos, what.position() - last_pos);
      test += pattern.substr(last_pos, what.position() - last_pos);
    }
    last_pos = what.position() + what.length();

    this->regex >>= as_xpr("<>") >> what[pattern_expr_capture] >> "<>";
    test += "!!!" + what[pattern_expr_capture].str() + "!!!";
    //cout << what.position() << "." << what.length() << " = " << what[pattern_expr_capture] << endl;
  }

  cout << test << endl;
}
