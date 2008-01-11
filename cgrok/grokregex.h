
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

    void search(const string text);
  private:
    sregex regex;
    const string pattern;
    bool regex_created;

    void GenerateRegexFromPattern(const string pattern);
    void AppendRegex(sregex re);
};

#endif /* ifndef GROKREGEX_H */
