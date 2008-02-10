#ifndef __GROKMATCH_HPP
#define __GROKMATCH_HPP

#include <boost/xpressive/xpressive.hpp>
using namespace boost::xpressive;

static void StringSlashEscape(string &value, const string &chars) {
  sregex re_chars = sregex::compile("[" + chars + "]");
  string format = "\\$&";
  value = regex_replace(value, re_chars, format);
}


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
      this->matches[match_key] = this->match_string;
      this->matches[line_key] = data;

      this->pattern_expand_re = 
        as_xpr('%')
          >> (GrokMatch::mark_pattern_name =
              !(as_xpr('=')) >> +(alnum | as_xpr('_'))
              >> !(as_xpr(':') >> +(alnum))
             )
          >> (GrokMatch::mark_filters =
              *('|' >> +alnum)
             )
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
        string filter_str = match[this->mark_filters].str();

        if (match.position() > last_pos)
          dst += src.substr(last_pos, match.position() - last_pos);

        last_pos = match.position() + match.length();

        map_iter = this->matches.find(pattern_name);
        if (map_iter != this->matches.end()) {
          string value = (*map_iter).second;
          this->Filter(value, filter_str);
          dst += value;
        } else {
          dst += "%" + pattern_name + "%";
        }
      }

      if (last_pos < src.size())
        dst += src.substr(last_pos, src.size() - last_pos);
    }

    void ToJSON(string &dst) {
      typename match_map_type::const_iterator map_iter;
      dst = "{";

      for (map_iter = this->matches.begin();
           map_iter != this->matches.end();
           map_iter++) {
        string key = (*map_iter).first:
        string val = (*map_iter).second;
        StringSlashEscape(key, "\"");
        StringSlashEscape(val, "\"");
        dst += "\"" + key + "\": ";
        dst += "\"" + val;

        if (map_iter + 1 != this->matches.end())
          dst += ", ";
      }
      # trim 
      dst += "}";

    }
      
    void Filter(string &value, const string &filter_str) {
      string::size_type pos = 0;
      string::size_type last_pos = 0;
      string filter_func;
      vector<string> filters;
      cout << "Filter str: " << filter_str << endl;
      while ((pos = filter_str.find("|", last_pos)) != string::npos) {
        filter_func = filter_str.substr(last_pos, (pos - last_pos));
        last_pos = pos + 1;
        if (filter_func.size() > 0)
          filters.push_back(filter_func);
      }
      /* Capture the last one, too */
      filter_func = filter_str.substr(last_pos, (filter_str.size() - last_pos));
      if (filter_func.size() > 0)
        filters.push_back(filter_func);

      vector<string>::iterator filter_iter;
      for (filter_iter = filters.begin(); filter_iter != filters.end(); filter_iter++) {
        cout << "Filter: " << *filter_iter << endl;
        if (*filter_iter == "shellescape")
          this->Filter_ShellEscape(value);
      }
    }

    void Filter_ShellEscape(string &value) {
      StringSlashEscape(value, "(){}\\[\\]\"'!$^~;<>?\\\\");
    }

  private:
    match_map_type matches;
    typename regex_type::string_type match_string;
    int length;
    int position;
};

#endif /* ifndef __GROKMATCH_HPP */
