#ifndef __GROKMATCH_HPP
#define __GROKMATCH_HPP

template <typename regex_type>
class GrokMatch {

  public:
    typedef map<string, typename regex_type::string_type> match_map_type;

    GrokMatch() { 
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
      this->matches[match_key] = match.str(0);
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

  private:
    match_map_type matches;
    typename regex_type::string_type match_string;
    int length;
    int position;

};

#endif /* ifndef __GROKMATCH_HPP */
