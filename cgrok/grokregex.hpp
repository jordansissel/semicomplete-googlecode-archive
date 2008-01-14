#include <iostream>
#include <string>
#include <boost/xpressive/xpressive.hpp>

#include "grokpatternset.hpp"
#include "grokmatch.hpp"

using namespace std;
using namespace boost::xpressive;

template <typename regex_type>
class GrokRegex {
  public:
    GrokRegex(const string pattern);
    ~GrokRegex();

    GrokMatch<regex_type>* Search(const string data);
    void AddPatternSet(const GrokPatternSet<regex_type> &pattern_set);

  private:
    GrokPatternSet<regex_type> pattern_set;
    const string pattern;
    regex_type *generated_regex;
    string *generated_string;
    map <string, unsigned int> backref_map;

    void GenerateRegex();
    void RecursiveGenerateRegex(string pattern, int &backref);
    void AppendRegex(regex_type re);
};


template <typename regex_type>
GrokRegex<regex_type>::GrokRegex(const string grok_pattern) 
  : pattern(grok_pattern) {
  this->generated_regex = NULL;
  this->generated_string = NULL;
  this->GenerateRegex();
}

template <typename regex_type>
GrokRegex<regex_type>::~GrokRegex() {
  /* Nothing to do */
}

template <typename regex_type>
void GrokRegex<regex_type>::GenerateRegex() {

  if (this->generated_regex != NULL)
    delete this->generated_regex;
  this->generated_regex = new regex_type();

  if (this->generated_string != NULL)
    delete this->generated_string;
  this->generated_string = new string();

  int backref = 0;
  this->backref_map.clear();

  /* XXX: Enforce a max recursion depth */
  this->RecursiveGenerateRegex(this->pattern, backref);
  this->generated_regex = new regex_type(
    regex_type::compile(*(this->generated_string))
  );
  cout << "Regex str: " << *(this->generated_string) << endl;
}

template <typename regex_type>
void GrokRegex<regex_type>::RecursiveGenerateRegex(string pattern, int &backref) {
  sregex not_percent = !+~(as_xpr('%'));
  unsigned int last_pos = 0;

  mark_tag mark_name(1), mark_alias(2), mark_predicate(3);
  /* Match %foo(:bar)?% */
  sregex pattern_expr_re(
    as_xpr('%')
      /* Pattern name and alias (FOO and FOO:BAR) */
      >> (mark_alias = 
          (mark_name = +(alnum | digit))
          >> !(as_xpr(':') >> +(alnum | digit))
         )
      /* Predicate conditionals */
      >> !(mark_predicate = /* predicates are optional */
          ((boost::xpressive::set= '<', '>', '=') >> !as_xpr('=')
           | (!as_xpr('!') >> as_xpr('~')))
           >> not_percent
          )
    >> '%'
  );

  //cout << "pattern: " << pattern << endl;

  /* probaly should use regex_iterator<regex_type> here */
  sregex_iterator cur(pattern.begin(), pattern.end(), pattern_expr_re);
  sregex_iterator end;

  for (; cur != end; cur++) {
    smatch const &match = *cur;
    string pattern_name = match[mark_name].str();
    string pattern_alias = match[mark_alias].str();
    cout << "P: " << pattern_name << " / " << pattern_alias << endl;
    //string pattern_predicate = match[mark_predicate].str();
    if (match.position() > last_pos) {
      string substr = pattern.substr(last_pos, match.position() - last_pos);
      //cout << "Appending substr '" << substr << "'" << endl;
      *(this->generated_string) += substr;
    }
    last_pos = match.position() + match.length();

    if (pattern_set.patterns.count(pattern_name) > 0) {
      string sub_pattern = pattern_set.patterns[pattern_name].regex_str;
      backref++;

      //cout << "Appending pattern [" << backref << "] '" << pattern_name << "'" << endl;
      //cout << "--> " << sub_pattern << endl;
      //cout << "Setting backref of '" << pattern_name << "' to " << backref << endl;
      *(this->generated_string) += "(";
      this->backref_map[pattern_alias] = backref;

      this->RecursiveGenerateRegex(sub_pattern, backref);
      *(this->generated_string) += ")";
    } else {
      //cout << "Appending nonpattern '" << pattern_name << "'" << endl;
      *(this->generated_string) += "%" + pattern_name + "%";
    }
  }

  if (last_pos < pattern.size()) {
    string substr = pattern.substr(last_pos, pattern.size() - last_pos);
    *(this->generated_string) += substr;
  }
}

template <typename regex_type>
GrokMatch<regex_type>* GrokRegex<regex_type>::Search(const string data) {
  match_results<typename regex_type::iterator_type> match;
  GrokMatch<regex_type>* gm;
  int ret;
  /* Grumble. regex_search() doesn't understand 'const string' */
  ret = regex_search(data.begin(), data.end(), match, *(this->generated_regex));
  if (!ret)
    return NULL;

  gm = new GrokMatch<regex_type>(data, match, this->backref_map);

  return gm;
}

template <typename regex_type>
void GrokRegex<regex_type>::AddPatternSet(const GrokPatternSet<regex_type> &pattern_set) {
  this->pattern_set.Merge(pattern_set);
  this->GenerateRegex();
}
