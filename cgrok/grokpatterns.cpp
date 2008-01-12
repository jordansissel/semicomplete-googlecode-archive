
#include "grokpatterns.h"

template <typename regex_type>
GrokPatternSet<regex_type>::GrokPatternSet() 
  : patterns() { 
  /* nothing to do */ 
}

template <typename regex_type>
GrokPatternSet<regex_type>::~GrokPatternSet() { 
  /* nothing to do */ 
}

template <typename regex_type>
void GrokPatternSet<regex_type>::AddPattern(string name_str, string regex_str) {
  this->patterns[name_str] = GrokPattern<regex_type>(regex_str);
}

int main() {
  GrokPatternSet<sregex> pset;

  pset.AddPattern("WORD", "\b\\w+\b");
  pset.AddPattern("NUMBER", "(?:[+-]?(?:(?:[0-9]+(?:\\.[0-9]*)?)|(?:\\.[0-9]+)))");
}
