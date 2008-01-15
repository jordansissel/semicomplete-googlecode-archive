#include <iostream>
#include <string>
#include <boost/xpressive/xpressive.hpp>

#include "grokpatternset.hpp"
#include "grokmatch.hpp"
#include "grokpredicate.hpp"

using namespace std;
using namespace boost::xpressive;

/* Helper function for concatonating regexes */
template <typename regex_type>
void _AppendRegex(regex_type **dst_re, regex_type &src_re);

template <typename regex_type>
void _ApplyPredicate(regex_type &re, string predicate);

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
    regex_compiler<typename regex_type::iterator_type> *re_compiler;
    regex_type *generated_regex;
    string *generated_string;
    map <string, unsigned int> backref_map;

    void GenerateRegex();
    regex_type* RecursiveGenerateRegex(string pattern, int &backref);
    void _ApplyPredicate(regex_type &re, string predicate);
};


template <typename regex_type>
GrokRegex<regex_type>::GrokRegex(const string grok_pattern) 
  : pattern(grok_pattern) {
  this->generated_regex = NULL;
  this->generated_string = NULL;
  this->re_compiler = NULL;
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
  this->generated_regex = new regex_type;

  if (this->generated_string != NULL)
    delete this->generated_string;
  this->generated_string = new string;

  /* make a new compiler */
  if (this->re_compiler != NULL)
    delete this->re_compiler;
  this->re_compiler = new regex_compiler<typename regex_type::iterator_type>;

  int backref = 0;
  this->backref_map.clear();

  /* XXX: Enforce a max recursion depth */
  this->generated_regex = this->RecursiveGenerateRegex(this->pattern, backref);

  //this->generated_regex = new regex_type(
    //regex_type::compile(*(this->generated_string))
  //);
  cout << "Regex str: " << *(this->generated_string) << endl;
  //cout << "Regex id: " << this->generated_regex->regex_id() << endl;
}

template <typename regex_type>
regex_type* GrokRegex<regex_type>::RecursiveGenerateRegex(string pattern, int &backref) {
  sregex not_percent = !+~(as_xpr('%'));
  unsigned int last_pos = 0;
  string re_string;
  regex_type *re;

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
    string pattern_predicate = match[mark_predicate].str();
    //cout << "P: " << pattern_name << " / " << pattern_alias << endl;

    if (match.position() > last_pos) {
      string substr = pattern.substr(last_pos, match.position() - last_pos);
      //cout << "Appending regex '" << substr << "'" << endl;
      //*(this->generated_string) += substr;
      re_string += substr;
      //regex_type tmp_re = regex_type::compile(substr);
      //_AppendRegex(&re, tmp_re);
    }

    last_pos = match.position() + match.length();

    if (pattern_set.patterns.count(pattern_name) > 0) {
      string sub_pattern = pattern_set.patterns[pattern_name].regex_str;
      stringstream re_name(stringstream::out);

      backref++;

      //cout << "Appending pattern [" << backref << "] '" << pattern_name << "'" << endl;
      //cout << "--> " << sub_pattern << endl;
      //cout << "Setting backref of '" << pattern_name << "' to " << backref << endl;

      //*(this->generated_string) += "(";

      this->backref_map[pattern_alias] = backref;
      //mark_tag backref_tag(backref);

      re_name << pattern_name;
      re_name << backref;

      /* Recurse deep until we run out of patterns to expand */
      regex_type *ptmp_re = this->RecursiveGenerateRegex(sub_pattern, backref);
      regex_type backref_re;

      re_string += "(";

      /* Append predicate regex if we have one */
      if (pattern_predicate.size() > 0) {
        /* Need to track this memory and delete it when we destroy this object. */
        GrokPredicate<regex_type> *pred = 
          new GrokPredicate<regex_type>(pattern_predicate);
        //backref_re = (backref_tag = *ptmp_re) [ check(*pred) ];
        backref_re = (*ptmp_re) [ check(*pred) ];
      } else {
        backref_re = *ptmp_re;
      }

      (*this->re_compiler)[re_name.str()] = backref_re;
      re_string += "(?$"+ re_name.str() + ")";
      re_string += ")";
      delete ptmp_re;
      //*(this->generated_string) += ")"
    } else {
      //cout << "Appending nonpattern '" << pattern_name << "'" << endl;
      string str = "%" + pattern_name + "%";
      //regex_type tmp_re = as_xpr(str);
      re_string += str;
      //*(this->generated_string) += str;
      //_AppendRegex(&re, tmp_re);
    }
  }

  if (last_pos < pattern.size()) {
    /* XXX: Make this a function */
    string substr = pattern.substr(last_pos, pattern.size() - last_pos);
    //cout << "Appending regex '" << substr << "'" << endl;
    //regex_type tmp_re = regex_type::compile(substr);
    //_AppendRegex(&re, tmp_re);
    //*(this->generated_string) += substr;
    re_string += substr;
  }

  cout << "String: " << re_string << endl;
  re = new regex_type(this->re_compiler->compile(re_string));
  return re;
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

  //cout << "Match: " << match.str(0) << endl;
  gm = new GrokMatch<regex_type>(data, match, this->backref_map);
  return gm;
}

template <typename regex_type>
void GrokRegex<regex_type>::AddPatternSet(const GrokPatternSet<regex_type> &pattern_set) {
  this->pattern_set.Merge(pattern_set);
  this->GenerateRegex();
}

//template <typename regex_type>
//void GrokRegex<regex_type>::_ApplyPredicate(regex_type &re, string predicate) {
  //GrokPredicate<regex_type> *pred = new GrokPredicate<regex_type>(predicate);
  //re >>= check(pred);
//}

template <typename regex_type>
void _AppendRegex(regex_type **dst_re, regex_type &src_re) {
  if (*dst_re == NULL)
    *dst_re = new regex_type;

  if ((*dst_re)->regex_id() == 0)
    **dst_re = src_re;
  else
    **dst_re >>= src_re;
}

