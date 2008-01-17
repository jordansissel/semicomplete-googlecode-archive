#ifndef __GrokRegex_hpp
#define __GrokRegex_hpp

#include <iostream>
#include <string>
#include <boost/xpressive/xpressive.hpp>

#include "grokpatternset.hpp"
#include "grokmatch.hpp"
#include "grokpredicate.hpp"

using namespace std;
using namespace boost::xpressive;

template <typename regex_type>
class GrokRegex {
  public:
    typedef map <string, typename regex_type::string_type> capture_map_t;
    GrokRegex(const string grok_pattern) 
    : pattern(grok_pattern) {
      this->generated_regex = NULL;
      this->generated_string = NULL;
      this->re_compiler = NULL;
      this->GenerateRegex();
    }

    GrokRegex() { 
      this->generated_regex = NULL;
      this->generated_string = NULL;
      this->re_compiler = NULL;
    }
    ~GrokRegex() { /* Nothing to do */ }

    void init(const string grok_pattern) {
      this->pattern = grok_pattern;
      this->GenerateRegex();
    }

    void init(const char *char_grok_pattern) {
      string grok_pattern = char_grok_pattern;
      this->init(grok_pattern);
    }

    void AddPatternSet(const GrokPatternSet<regex_type> &pattern_set) {
      this->pattern_set.Merge(pattern_set);
      this->GenerateRegex();
    }

    bool Search(const typename regex_type::string_type data, 
                GrokMatch<regex_type> &gm) {
      match_results<typename regex_type::iterator_type> match;
      int ret;

      /* Late binding with Boost.Xpressive. 
       * Inject capture_map for placeholder_map. */
      match.let(this->placeholder_map = this->capture_map);
      ret = regex_search(data.begin(), data.end(), match, *(this->generated_regex));
      if (!ret)
        return false;

      gm.init(data, match, this->capture_map);

      return true;
    }

  private:
    GrokPatternSet<regex_type> pattern_set;
    string pattern;
    regex_compiler<typename regex_type::iterator_type> *re_compiler;
    regex_type *generated_regex;
    string *generated_string;
    capture_map_t capture_map;
    placeholder< capture_map_t > placeholder_map;

    void GenerateRegex() {
      int backref = 0;

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

      /* XXX: Enforce a max recursion depth */
      this->generated_regex = this->RecursiveGenerateRegex(this->pattern, backref);
      //cout << "Regex str: " << *(this->generated_string) << endl;
    }

    /* XXX: Split this into smaller functions */
    regex_type* RecursiveGenerateRegex(string pattern, int &backref) {
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
          re_string += substr;
        }

        last_pos = match.position() + match.length();

        if (pattern_set.patterns.count(pattern_name) > 0) {
          string sub_pattern = pattern_set.patterns[pattern_name].regex_str;
          stringstream re_name(stringstream::out);

          backref++;

          //cout << "Appending pattern [" << backref << "] '" << pattern_name << "'" << endl;
          //cout << "--> " << sub_pattern << endl;
          //cout << "Setting backref of '" << pattern_name << "' to " << backref << endl;

          /* Recurse deep until we run out of patterns to expand */
          regex_type *ptmp_re = this->RecursiveGenerateRegex(sub_pattern, backref);
          regex_type backref_re;
          mark_tag backref_tag(backref);

          re_string += "(";

          /* Append predicate regex if we have one */
          if (pattern_predicate.size() > 0) {
            /* Need to track this memory and delete it when we destroy this object. */
            GrokPredicate<regex_type> *pred = 
              new GrokPredicate<regex_type>(pattern_predicate);
            backref_re = (*ptmp_re) [ check(*pred) ] [ (this->placeholder_map)[pattern_alias] = as<string>(_) ];
          } else {
            backref_re = (*ptmp_re) [ (this->placeholder_map)[pattern_alias] = as<string>(_) ];
          }

          /* Insert the result into the match */
          //backref_re = backref_re [ self.captures[pattern_alias] = as<string>(_) ];

          /* Generate the named regex name for (?$foo)  as 'pattern' + 'backref' */
          re_name << pattern_name;
          re_name << backref;

          (*this->re_compiler)[re_name.str()] = backref_re;
          re_string += "(?$"+ re_name.str() + ")";
          re_string += ")";
          delete ptmp_re;
        } else {
          //cout << "Appending nonpattern '" << pattern_name << "'" << endl;
          string str = "%" + pattern_name + "%";
          re_string += str;
        }
      }

      if (last_pos < pattern.size()) {
        /* XXX: Make this a function */
        string substr = pattern.substr(last_pos, pattern.size() - last_pos);
        //cout << "Appending regex '" << substr << "'" << endl;
        re_string += substr;
      }

      //cout << "String: " << re_string << endl;
      re = new regex_type(this->re_compiler->compile(re_string));
      return re;
    }
};

#endif /* ifndef __GrokRegex_hpp */
