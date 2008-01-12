
#include <iostream>
#include <string>
#include "grokregex.h"
#include <exception>
#include <boost/xpressive/xpressive.hpp>
#include <boost/xpressive/regex_actions.hpp>
using namespace std;
using namespace boost::xpressive;

#include "patterns.h"

GrokRegex::GrokRegex(const string grok_pattern) 
                    : pattern(grok_pattern),
                      regex_created(false) {
  this->GenerateRegexFromPattern(pattern);
}

GrokRegex::~GrokRegex() {
  // clean up stuff, but what?
}

void GrokRegex::AppendRegex(sregex re) {
  if (this->regex_created) {
    this->regex >>= re;
  } else {
    this->regex = re;
    this->regex_created = true;
  }
}

void GrokRegex::GenerateRegexFromPattern(const string pattern) {
  sregex notpercent = !+~(as_xpr('%'));
  unsigned int last_pos = 0;
  string regex_str("");
  mark_tag pattern_name(1), pattern_alias(2), pattern_predicate(3);

  //sregex_map_t patterns = DefaultPatterns();
  pattern_map_t patterns = DefaultPatterns();

  sregex pattern_expr_re(
    as_xpr('%')
      >> (pattern_name=notpercent)
      //>> (as_xpr(';') >> (pattern_alias=notpercent))
    >> '%'
  );


  smatch m;
  cout << "Testing: " << pattern << endl;

  sregex_iterator cur(pattern.begin(), pattern.end(), pattern_expr_re);
  sregex_iterator end;

  for (; cur != end; cur++) {
    smatch const &what = *cur;
    string patname = what[pattern_name].str();
    sregex target_re;
    if (what.position() > last_pos) {
      string substr = pattern.substr(last_pos, what.position() - last_pos);
      //sregex prefix_re = sregex::compile(substr);
      //cout << "Appending '" << substr << "'" << endl;
      //this->AppendRegex(prefix_re);
      regex_str += substr;
    }
    last_pos = what.position() + what.length();

    if (patterns.count(patname)) {
      cout << "Appending pattern '" << patname << "'" << endl;
      //target_re = sregex::compile(patterns[patname]);
      //test += "!" + what[pattern_name].str() + "!";
      regex_str += patterns[patname];
    } else {
      cout << "Appending nonpattern '" << patname << "'" << endl;
      //target_re = as_xpr('%') >> patname >> '%' ;
      regex_str += "%" + patname + "%";
    }
    //this->AppendRegex(target_re);
  }

  if (last_pos < pattern.size()) {
    string substr = pattern.substr(last_pos, pattern.size() - last_pos);
    //sregex prefix_re = sregex::compile(substr);
    cout << "Appending '" << substr << "'" << endl;
    //this->AppendRegex(prefix_re);
    regex_str += substr;
  }
  cout << "Test str: '" << regex_str << "'" << endl;

  //this->regex = -*_ >> (s1= sregex(this->regex)) >> -*_;
  //this->regex = (s1= sregex(this->regex));
  //th
  this->regex = (s1 = sregex::compile(regex_str));
  cout << "regexid: " << this->regex.regex_id() << endl;
}

void GrokRegex::search(const string text) {
  smatch m;
  int ret;
  ret = regex_search(text, m, this->regex);
  if (ret) {
    cout << "Match: " << ret << " / '" << m[0] << "'" << endl;
    //cout << "Match: " << ret << " / '" << m[1] << "'" << endl;
  }
}
