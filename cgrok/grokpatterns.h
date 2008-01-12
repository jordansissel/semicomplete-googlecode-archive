#ifndef GROKPATTERNS_H
#define GROKPATTERNS_H

#include <map>
#include <string>
#include <boost/xpressive/xpressive.hpp>

using namespace std;
using namespace boost::xpressive;

template <typename regex_type>
class GrokPatternSet {
  public:
    GrokPatternSet();
    ~GrokPatternSet();

    void AddPattern(string name_str, string regex_str);
    
  private:
    /* XXX: Maybe we should worry about using cregex/sregex/wsregex etc? */
    /* Perhaps use templating here... */
    map < string, regex_type, less<string> > patterns;
};

template <typename regex_type>
class GrokPattern {
  public:
    const regex_type regex;
    const string regex_str;

    GrokPattern(string regex_string) 
      : regex(regex_string), regex_str(regex_string) {
      /* nothing to do */
    }
};

#endif /* ifndef GROKPATTERNS_H */
