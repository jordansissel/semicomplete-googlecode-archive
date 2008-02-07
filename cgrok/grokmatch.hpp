#ifndef __GROKMATCH_HPP
#define __GROKMATCH_HPP

#include <boost/xpressive/xpressive.hpp>
using namespace boost::xpressive;

template <typename regex_type>
class GrokMatch {

  public:
    typedef map<string, typename regex_type::string_type> match_map_type;
    sregex pattern_expand_re;
    mark_tag mark_pattern_name;
    mark_tag mark_filters;

    GrokMatch() : mark_pattern_name(1), mark_filters(2) { 
      /* nothing to do */
    }

    void init(const typename regex_type::string_type &data, 
              const match_results<typename regex_type::iterator_type> &match,
              const match_map_type &backref_map) {

      this->match_string = match.str(0);
      this->matches = backref_map;
      this->length = match.length();
      this->position = match.position();

      /* Set some default values */
      string match_key = "=MATCH";
      string line_key = "=LINE";
      this->matches[match_key] = match.str(0);
      this->matches[line_key] = this->match_string;

      this->pattern_expand_re = 
        as_xpr('%')
          >> (GrokMatch::mark_pattern_name =
              !(as_xpr('=')) >> +(alnum | as_xpr('_'))
              >> !(as_xpr(':') >> +(alnum | digit))
             )
          //>> (GrokMatch::mark_filters =
        >> as_xpr('%');
    }

    ~GrokMatch() {
      /* Nothing to do */
    };

    const match_map_type& GetMatches() const {
      return this->matches;
    };

    typename regex_type::string_type GetMatchString() const {
      return this->match_string;
    }

    int GetLength() {
      return this->length;
    }

    int GetPosition() {
      return this->position;
    }

    void ExpandString(const string &src, string &dst) {
      regex_iterator<typename regex_type::iterator_type> cur(
           src.begin(), src.end(), pattern_expand_re);
      regex_iterator<typename regex_type::iterator_type> end;
      unsigned int last_pos = 0;
      typename match_map_type::const_iterator map_iter;
      
      dst = "";

      for (; cur != end; cur++) {
        match_results<typename regex_type::iterator_type> match = *cur;
        string pattern_name = match[this->mark_pattern_name].str();

        if (match.position() > last_pos)
          dst += src.substr(last_pos, match.position() - last_pos);

        last_pos = match.position() + match.length();

        map_iter = this->matches.find(pattern_name);
        if (map_iter != this->matches.end()) {
          dst += (*map_iter).second;
        } else {
          dst += "%" + pattern_name + "%";
        }
      }

      if (last_pos < src.size())
        dst += src.substr(last_pos, src.size() - last_pos);
    }

  private:
    match_map_type matches;
    typename regex_type::string_type match_string;
    int length;
    int position;
};

#endif /* ifndef __GROKMATCH_HPP */
