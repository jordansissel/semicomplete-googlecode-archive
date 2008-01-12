#ifndef GROKPATTERNSET_H
#define GROKPATTERNSET_H

#include <iostream>
#include <map>
#include <string>
#include <boost/xpressive/xpressive.hpp>

using namespace std;
using namespace boost::xpressive;

#include "grokpattern.hpp"

template <typename regex_type>
class GrokPatternSet {
  public:
    GrokPatternSet();
    ~GrokPatternSet();

    void AddPattern(string name_str, string regex_str);
    void Merge(const GrokPatternSet<regex_type> &other_set);
    
  //private:
    map< string, GrokPattern<regex_type> > patterns;
}; 
template <typename regex_type>
GrokPatternSet<regex_type>::GrokPatternSet(void) {
  cout << "Constructor" << endl;
}

template <typename regex_type>
GrokPatternSet<regex_type>::~GrokPatternSet() {
  /* nothing to do */ 
  cout << "deconstructor" << endl;
}

template <typename regex_type>
void GrokPatternSet<regex_type>::AddPattern(string name_str, string regex_str) {
  cout << "Name: " << name_str << " == " << regex_str << endl;
  this->patterns[name_str].Update(regex_str);
} 

template <typename regex_type>
void GrokPatternSet<regex_type>::Merge(const GrokPatternSet<regex_type> &other_set)
{
  //map< string, GrokPattern<regex_type> >::iterator iter;
  //iter = other_set.patterns.start();

  //while (iter != this->patterns.end()) {
  //}
}

#endif /* ifndef GROKPATTERNSET_H */
