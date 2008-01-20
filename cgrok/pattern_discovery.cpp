
#include <iostream>
#include <fstream>
#include <string>
#include <list>

#include "grokpatternset.hpp"
#include "grokregex.hpp"
#include "grokmatch.hpp"

template <typename regex_type>
class ExpandedPattern {
  public:
    string name;
    string pattern;
    GrokRegex<regex_type> gre;

    ExpandedPattern(string name, GrokPatternSet<regex_type> pattern_set) {
      string gre_string = "%" + name + "%";
      gre.SetTrackMatches(false);
      gre.AddPatternSet(pattern_set);
      this->gre.SetRegex(gre_string);
      this->name = name;
      this->pattern = gre.GetExpandedPattern();
    }

    ~ExpandedPattern() {
      /* nothing to do */
    }

    const string &GetExpandedPattern() {
      return this->pattern;
    }

    const string &GetName() const {
      return this->name;
    }
    
    int GetComplexity() {
      sregex space_re, period_re, punctuation_re;
      int complexity(0);
      space_re = _s >> "\\s"; // literal space or meta space
      period_re = as_xpr(".");
      punctuation_re = sregex::compile("['\":_.=+-]");

      sregex_iterator end;
      sregex_iterator space_iter(this->pattern.begin(), this->pattern.end(),
                                 space_re);
      sregex_iterator period_iter(this->pattern.begin(), this->pattern.end(),
                                  period_re);
      sregex_iterator punctuation_iter(this->pattern.begin(), this->pattern.end(),
                                       punctuation_re);

      for(; space_iter != end; space_iter++)
        complexity += 15;
      for(; period_iter != end; period_iter++)
        complexity += 10;
      for(; punctuation_iter != end; punctuation_iter++)
        complexity += 6;

      complexity += this->pattern.size() * 10;

      return complexity;
    }

    bool operator<(ExpandedPattern<regex_type> &other) {
      return (this->GetComplexity() - other.GetComplexity()) < 0;
    }
};

void GetPatternNames(GrokPatternSet<sregex> &patterns, 
                     list< ExpandedPattern<sregex> > &expanded_patterns) {
  GrokPatternSet<sregex>::pattern_set_type::const_iterator iter;
  for (iter = patterns.patterns.begin(); iter != patterns.patterns.end(); iter++) {
    ExpandedPattern<sregex> ep((*iter).first, patterns);
    expanded_patterns.push_back(ep);
  }
}

void Analyze(string &line, GrokPatternSet<sregex> &patterns, 
             list< ExpandedPattern<sregex> > expanded_patterns) {
  bool done = false;
  
  while (!done) {
    list< ExpandedPattern<sregex> >::iterator iter;
    for (iter = expanded_patterns.begin();
         iter != expanded_patterns.end();
         iter++) {
      ExpandedPattern<sregex> ep = *iter;
      GrokMatch<sregex> gm;
      bool success;
      string pattern = "%" + ep.GetName() + "%";
      ep.gre.Replace(line, pattern);
      //success = ep.gre.Search(line, gm);
      //if (success) {
        //cout << "Matches " << pattern << endl;
      //}
    }
    done = true;
  }

  cout << "New line: " << line << endl;
}

int main(int argc, char **argv) {
  string line;
  GrokPatternSet<sregex> patterns;
  list< ExpandedPattern<sregex> > expanded_patterns;
  patterns.LoadFromFile("patterns");
  GetPatternNames(patterns, expanded_patterns);
  expanded_patterns.sort();

  /* We want the most complex pattern first */
  expanded_patterns.reverse();

  list< ExpandedPattern<sregex> >::iterator iter;
  for (iter = expanded_patterns.begin(); iter != expanded_patterns.end(); iter++) {
    cout << (*iter).GetName() << " = " << (*iter).GetExpandedPattern() << endl;
  }
  while (getline(cin, line)) {
    Analyze(line, patterns, expanded_patterns);
  }
  return 0;
}
