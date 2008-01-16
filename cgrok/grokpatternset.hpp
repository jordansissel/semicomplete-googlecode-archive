#ifndef GROKPATTERNSET_H
#define GROKPATTERNSET_H

#include <boost/xpressive/xpressive.hpp>
#include <iostream>
#include <map>
#include <string>

using namespace std;
using namespace boost::xpressive;

#include "grokpattern.hpp"

/* XXX: Expose more map<> methods directly, or just extend map<> */

template <typename regex_type>
class GrokPatternSet {
  public:
    map< string, GrokPattern<regex_type> > patterns;

    GrokPatternSet(void) {
      /* Nothing to do */
    }

     ~GrokPatternSet() {
       /* nothing to do */ 
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
