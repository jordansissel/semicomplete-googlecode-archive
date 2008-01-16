
#ifndef __GrokMatch_hpp
#define __GrokMatch_hpp

template <typename regex_type>
class GrokMatch {

  public:
    //pedef GrokRegex<regex_type>::capture_map_t match_map_type;
    typedef map<string, typename regex_type::string_type> match_map_type;

    GrokMatch(const typename regex_type::string_type &data, 
          const match_results<typename regex_type::iterator_type> &match,
          const match_map_type &backref_map) {

      map <string, unsigned int>::const_iterator backref_iter;

      this->match_string = match.str(0);
      this->matches = backref_map;

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

  private:
    match_map_type matches;
    typename regex_type::string_type match_string;

};

#endif /* __GrokMatch_hpp */
