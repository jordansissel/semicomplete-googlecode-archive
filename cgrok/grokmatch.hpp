
#ifndef __GrokMatch_hpp
#define __GrokMatch_hpp

template <typename regex_type>
class GrokMatch {

  public:
    typedef map < string, typename regex_type::string_type > match_map_type;

    GrokMatch(const typename regex_type::string_type &data, 
              const match_results<typename regex_type::iterator_type> &match,
              const map <string, unsigned int> &backref_map);
    ~GrokMatch();

    const match_map_type& GetMatches() {
      return this->matches;
    };

  private:
    match_map_type matches;
    typename regex_type::string_type match_string;
    //regex_type::string_type input;

};

template <typename regex_type>
GrokMatch<regex_type>::GrokMatch(const typename regex_type::string_type &data, 
          const match_results<typename regex_type::iterator_type> &match,
          const map <string, unsigned int> &backref_map) {

  map <string, unsigned int>::const_iterator backref_iter;

  this->match_string = match.str(0);
  for (backref_iter = backref_map.begin();
       backref_iter != backref_map.end();
       backref_iter++) {
    string name = (*backref_iter).first;
    unsigned int capture_num = (*backref_iter).second;

    this->matches[name] = match.str(capture_num);
    cout << "Match: (" << capture_num << ") " << name << " => " << this->matches[name] << endl;
  }
}

template <typename regex_type>
GrokMatch<regex_type>::~GrokMatch() {
  /* Nothing to do? */
}

#endif /* __GrokMatch_hpp */
