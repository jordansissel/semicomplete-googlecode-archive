
#ifndef GROKPATTERN_H
#define GROKPATTERN_H

#include <string>
using namespace std;

template <typename regex_type>
class GrokPattern {
  public:
    regex_type regex;
    string regex_str;

    GrokPattern() {
    }

    GrokPattern(string regex_string) {
      this->Update(regex_string);
      /* nothing to do */
    }

    void Update(string regex_string) {
      this->regex_str = regex_string;
      this->regex = regex_type::compile(regex_string);
    }
};

#endif /* ifndef GROKPATTERN_H */

