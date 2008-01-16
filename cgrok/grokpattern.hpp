
#ifndef GROKPATTERN_H
#define GROKPATTERN_H

#include <string>
using namespace std;

/* XXX: This class was historically more complex. Perhaps we should delete it
 * now that it's just a string container */

template <typename regex_type>
class GrokPattern {
  public:
    string regex_str;

    GrokPattern() {
    }

    GrokPattern(string regex_string) {
      this->Update(regex_string);
    }

    void Update(string regex_string) {
      this->regex_str = regex_string;
    }
};

#endif /* ifndef GROKPATTERN_H */

