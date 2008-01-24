#ifndef __GROKPREDICATE_HPP
#define __GROKPREDICATE_HPP

#include <iostream>
#include <string>
#include <sstream>
#include <boost/xpressive/xpressive.hpp>
#include <boost/xpressive/regex_actions.hpp>

using namespace std;
using namespace boost::xpressive;

#define P_EQUAL  (1<<1)
#define P_NOT  (1<<2)
#define P_LESS (1<<3)
#define P_GREATER (1<<4)
#define P_REGEX (1<<5)

#define T_INT (1<<0)
#define T_STR (1<<1)

template <typename regex_type>
class GrokPredicate {
  public:
    typedef sub_match<typename regex_type::iterator_type> sub_match_t;

    GrokPredicate(string predicate) {
      local<unsigned int> flags(0);
      match_results<typename regex_type::iterator_type> match;

      /* XXX: Should this just be a dispatch table? */
      regex_type op_re = 
        ( /* Test for < > <= >= */
          (as_xpr('>')  [ flags |= P_GREATER ]
           | as_xpr('<') [ flags |= P_LESS ]
          )
          >> !(as_xpr('=') [ flags |= P_EQUAL ]
          )
        ) | ( /* Test for '==' */
          as_xpr("==") [ flags |= P_EQUAL ]
        ) | ( /* Test for != */
          as_xpr("!=") [ flags |= P_NOT | P_EQUAL ]
        ) | ( /* Test for =~ and !~ */
          ((as_xpr('=') [ flags |= P_EQUAL ]
            | as_xpr('!') [ flags |= P_NOT ] 
           ) >> as_xpr('~')) [ flags |= P_REGEX ]
        ); /* end 'op' regex */

      regex_search(predicate, match, op_re);
      this->flags = flags.get();

      if (this->flags == 0) {
        /* Throw an error that this was an invalid predicate */
        cerr << "Invalid predicate: '" << predicate << "'" << endl;
      }

      string remainder_string = predicate.substr(match.length(), 
                                                 predicate.size() - match.length());
      stringstream remainder(remainder_string, stringstream::in);

      this->type = T_STR;
      this->value_string = remainder_string;

      /* If not a regex, consider using integer comparisons */
      if (this->flags & P_REGEX) {
        this->value_regex = regex_type::compile(this->value_string);
      } else {
        /* Try to see if this is an integer predicate 
         * (ie, try to convert it to an int) */
        int tmp = 0;
        remainder >> tmp;
        if (!remainder.fail()) {
          this->type = T_INT;
          this->value_int = tmp;
        }
      }

      //cout << "Predicate type: " << (this->type == T_INT ? "int" : "string") << endl;
      //cout << "Flags: " << this->flags << endl;
      //cout << "String: " << this->value_string << endl;
      //cout << "Int: " << this->value_int << endl;
    }

    bool operator()(const sub_match_t &match) const {
      if (this->flags & P_REGEX) 
        return this->call_regex(match);
      else if (this->type == T_STR)
        return this->call_string(match);
      return this->call_int(match);
    }

    bool call_regex(const sub_match_t &match) const {
      return regex_search(match.str(), this->value_regex);
    }

    bool call_string(const sub_match_t &match) const {
      int cmp = this->compare_string(match);
      return this->result(cmp);
    }

    bool call_int(const sub_match_t &match) const {
      int val = 0;
      int ret;
      stringstream ss(match.str(), stringstream::in);
      ss >> val;

      ret = this->result(val - this->value_int);
      //cout << val << " vs " << this->value_int << " == " << ret << endl;
      /* Throw an exception if ss.fail() ? */
      return ret;
    }

    int compare_string(const sub_match_t &match) const {
      return match.compare(this->value_string);
    }

    int compare_int(const sub_match_t &match) const {
      return this->value_int - as<int>(_);
    }

    bool result(int compare) const {
      int flags = this->flags;
      switch (flags) {
        case P_LESS:
          return compare < 0; break;
        case P_LESS | P_EQUAL:
          return compare <= 0; break;
        case P_GREATER:
          return compare > 0; break;
        case P_GREATER | P_EQUAL:
          return compare >= 0; break;
        case P_EQUAL:
          return compare == 0; break;
        case P_EQUAL | P_NOT:
          return compare != 0; break;
        default:
          /* Should not get here */
          cout << "SHOULD NOT GET HERE. flags = " << this->flags << endl;
      }
      cout << "SHOULD(2) NOT GET HERE. flags = " << this->flags << endl;
    }

  private:
    unsigned int flags;
    unsigned int type;
    int value_int;
    string value_string;
    regex_type value_regex;
};
#endif /* ifndef __GROKPREDICATE_HPP */
