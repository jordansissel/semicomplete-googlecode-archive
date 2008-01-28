
#ifndef __GROKCONFIG_HPP
#define __GROKCONFIG_HPP

#include <string>
#include <iostream>

#include <boost/xpressive/xpressive.hpp>
using namespace boost::xpressive;
using namespace std;

/* Shorthand to compile a dynamic regex */
#define RE(x) (sregex::compile(x))

/* Match "\s*=\s*" */
#define R_EQ (*_s >> "=" >> *_s)

/* optionally match ';' */
#define R_SEMI (!as_xpr(';'))

typedef void (*grok_config_method)(string arg);

class GrokConfig {
  public:
    GrokConfig() {
      this->re_comment = ('#' >> *~_n);
      this->re_whitespace = +_s;

      /* Tokens */
      this->re_block_begin = *_s >> as_xpr('{');
      this->re_block_end = *_s >> as_xpr('}') >> !as_xpr(';');
      this->re_string = RE("(?<!\\\\)(?:\"(?:(?:\\\\\")*[^\"]*\"))");
      this->re_number = RE("(?:[+-]?(?:(?:[0-9]+(?:\\.[0-9]*)?)|(?:\\.[0-9]+)))");

      /* Prefixing everything with 'bos >>' is a hack because adding it later
       * causes captures not to be obeyed. Strange. */
      this->re_skip = bos >> +(this->re_comment | this->re_whitespace);
      this->re_file = bos >> "file" >> +_s >> (s1=re_string) >> re_block_begin;
      this->re_filelist = bos >> "filelist" >> +_s >> (s1=re_string) >> re_block_begin;

      this->re_matchtype = bos >> "type" >> +_s >> (s1=re_string) >> re_block_begin;

      this->re_match = bos >> "match" >> R_EQ >> (s1=re_string) >> R_SEMI;
      this->re_threshold = bos >> "threshold" >> R_EQ >> (s1=re_string) >> R_SEMI;
      this->re_interval = bos >> "interval" >> R_EQ >> (s1=re_string) >> R_SEMI;
      this->re_reaction = bos >> "reaction" >> R_EQ >> (s1=re_string) >> R_SEMI;
      this->re_key = bos >> "key" >> R_EQ >> (s1=re_string) >> R_SEMI;
      this->re_match_syslog = bos >> "match_syslog" >> R_EQ >> (s1=re_string) >> R_SEMI;
      this->re_syslog_prog = bos >> "syslog_prog" >> R_EQ >> (s1=re_string) >> R_SEMI;
      this->re_syslog_host = bos >> "syslog_host" >> R_EQ >> (s1=re_string) >> R_SEMI;
      this->re_shell = bos >> "shell" >> R_EQ >> (s1=re_string) >> R_SEMI;
    }

    void parse(const string config) {
      string input = config;
      this->line_number = 1;
      block_config(input);
    }

    bool consume(string &input, smatch &m, sregex &re) {
      int ret;
      if (re.regex_id() != this->re_skip.regex_id()) {
        this->consume(input, m, this->re_skip);
      }

      ret = regex_search(input, m, re);
      if (ret) {
        int len = m.position(0) + m.length(0);
        int pos = string::npos;

        string consumed = input.substr(0, len);
        while ((pos = consumed.find("\n", pos + 1)) != string::npos)
          this->line_number++;

        input = input.substr(len, input.size() - len);
        return true;
      }
      return false;
    }

    void parse_error(const string &where, string &input) const {
      cerr << "(" << where << ") Unrecognized input on line " 
           << this->line_number << ": '"
           << input.substr(0, input.find("\n")) << "'" << endl;
    }

    void block_config(string &input) {
      smatch m;
      bool done = false;
      while (!done) {
        if (this->consume(input, m, this->re_file)) {
          block_file(input);
        } else if (input.size() == 0) {
          /* We've reached eof */
          done = true;
        } else {
          this->parse_error("main config", input);
        }
      }
    }

    void block_file(string &input) {
      smatch m;
      bool done = false;
      while (!done) {
        if (this->consume(input, m, this->re_matchtype)) {
          /* Track the type we're adding */
          block_matchtype(input);
        } else if (this->consume(input, m, this->re_block_end)) {
          //cout << "(file block) block close found" << endl;
          done = true;
        } else {
          this->parse_error("file block", input);
        }
      }
    }

    void block_matchtype(string &input) {
      smatch m;
      bool done = false;
      while (!done) {
        if (this->consume(input, m, this->re_match)) {
          cout << "Match: " << m.str(1) << endl;
        } else if (this->consume(input, m, this->re_threshold)) {
        } else if (this->consume(input, m, this->re_interval)) {
        } else if (this->consume(input, m, this->re_reaction)) {
          string data = m.str(1);
          /* remove quotes */
          data = data.substr(1, data.size() - 2);
          cout << "reaction: " << data << endl;
        } else if (this->consume(input, m, this->re_key)) {
        } else if (this->consume(input, m, this->re_match_syslog)) {
        } else if (this->consume(input, m, this->re_syslog_prog)) {
        } else if (this->consume(input, m, this->re_syslog_host)) {
        } else if (this->consume(input, m, this->re_shell)) {
        } else if (this->consume(input, m, this->re_block_end)) {
          //cout << "(matchtype block) block close found" << endl;
          done = true;
        } else {
          this->parse_error("matchtype block", input);
        }
      }
    }

  private:
    int line_number;
    sregex re_block_begin;
    sregex re_block_end;
    sregex re_comment;
    sregex re_string;
    sregex re_number;
    sregex re_whitespace;
    sregex re_skip;

    sregex re_config;
    sregex re_file;
    sregex re_filelist;

    sregex re_matchtype;
      sregex re_match;
      sregex re_threshold;
      sregex re_interval;
      sregex re_reaction;
      sregex re_key;
      sregex re_match_syslog;
      sregex re_syslog_prog;
      sregex re_syslog_host;
      sregex re_shell;

    GrokPatternSet patterns;

};

#endif /* ifndef __GROKCONFIG_HPP */
