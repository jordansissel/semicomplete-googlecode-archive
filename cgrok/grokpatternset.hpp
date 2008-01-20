#ifndef GROKPATTERNSET_H
#define GROKPATTERNSET_H

#include <boost/xpressive/xpressive.hpp>
#include <iostream>
#include <map>
#include <fstream>
#include <string>

using namespace std;
using namespace boost::xpressive;

#include "grokpattern.hpp"

/* XXX: Expose more map<> methods directly, or just extend map<> */

template <typename regex_type>
class GrokPatternSet {
  public:
    typedef map< string, GrokPattern<regex_type> > pattern_set_type;
    pattern_set_type patterns;

    GrokPatternSet(void) {
      /* Nothing to do */
    }

    ~GrokPatternSet() {
      /* nothing to do */ 
    }

    void LoadFromFile(string filename) {
      this->LoadFromFile(filename.c_str());
    }

    void LoadFromFile(const char *filename) {
      ifstream pattern_file(filename);
      string pattern_desc;
      sregex pattern_re = sregex::compile("^([^#]\\S+)\\s+(.*)$");
      smatch match;

      while (getline(pattern_file, pattern_desc)) {
        string name, regex_str;
        int valid = regex_search(pattern_desc, match, pattern_re);
        if (!valid)
          continue;
        name = match.str(1);
        regex_str = match.str(2);
        this->AddPattern(name, regex_str);
      }
    }

    void AddPattern(string name_str, string regex_str) {
      this->patterns[name_str].Update(regex_str);
    } 

    void Merge(const GrokPatternSet<regex_type> &other_set) {
      typename map< string, GrokPattern<regex_type> >::const_iterator iter;

      iter = other_set.patterns.begin();
      for (; iter != other_set.patterns.end(); iter++) {
        this->patterns[(*iter).first] = (*iter).second;
      }
    }
}; 
#endif /* ifndef GROKPATTERNSET_H */
