
#ifndef GROKREGEX_H
#define GROKREGEX_H

#include <string>
#include <boost/xpressive/xpressive.hpp>
#include <boost/xpressive/regex_actions.hpp>
using namespace std;
using namespace boost::xpressive;

class GrokRegex {
  public:
    GrokRegex(const string pattern);
    ~GrokRegex();

    //GrokMatch *search(string pattern);
  private:
    sregex regex;
    const string pattern;

    void GenerateRegexFromPattern(const string pattern);
};

#endif /* ifndef GROKREGEX_H */
