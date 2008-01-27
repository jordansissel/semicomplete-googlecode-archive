
#ifndef __GROKCONFIG_HPP
#define __GROKCONFIG_HPP

#include <strings.h> /* for index() */
#include <string>
#include <iostream>

#include <boost/xpressive/xpressive.hpp>
using namespace boost::xpressive;
using namespace std;

#define RE(x) (sregex::compile(x))

typedef void (*grok_config_method)(string arg);

class GrokConfig {
  public:
    GrokConfig() {
      this->re_comment = '#' >> *~_n;
      this->re_whitespace = +_s;
      this->re_skip = +(this->re_comment | this->re_whitespace);
      this->re_block_begin = as_xpr('{');
      this->re_block_end = as_xpr('}') >> !as_xpr(';');
      this->re_string = RE("(?<!\\\\)(?:\"(?:(?:\\\\\")*[^\"]*\"))");
      this->re_number = RE("(?:[+-]?(?:(?:[0-9]+(?:\\.[0-9]*)?)|(?:\\.[0-9]+)))");

      /* XXX: block types:
       * patterns, filters
       * file, exec, catlist, filecmd */
#define R_EQ (+_s >> "=" >> +_s)
#define R_SEMI (!as_xpr(';'))

      this->re_file = "file" >> +_s >> (s1=re_string) >> +_s >> re_block_begin ;
      this->re_filelist = "filelist" >> +_s >> (s1=re_string) >> +_s >> re_block_begin;

      this->re_matchtype = "type" >> R_EQ >>(s1=re_string) >> re_block_begin;

      this->re_match = "match" >> R_EQ >> (s1=re_string) >> R_SEMI;
      this->re_threshold = "threshold" >> R_EQ >> (s1=re_string) >> R_SEMI;
      this->re_interval = "interval" >> R_EQ >> (s1=re_string) >> R_SEMI;
      this->re_reaction = "reaction" >> R_EQ >> (s1=re_string) >> R_SEMI;
      this->re_key = "key" >> R_EQ >> (s1=re_string) >> R_SEMI;
      this->re_match_syslog = "match_syslog" >> R_EQ >> (s1=re_string) >> R_SEMI;
      this->re_syslog_prog = "syslog_prog" >> R_EQ >> (s1=re_string) >> R_SEMI;
      this->re_syslog_host = "syslog_host" >> R_EQ >> (s1=re_string) >> R_SEMI;
      this->re_shell = "shell" >> R_EQ >> (s1=re_string) >> R_SEMI;
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
        int pos = -1;

        string consumed = input.substr(0, len);
        while ((pos = consumed.find("\n", pos + 1)) != string::npos)
          this->line_number++;

        input = input.substr(len, input.size() - len);
        return true;
      }
      return false;
    }

    void block_config(string &input) {
      smatch m;
      bool done = false;
      while (!done) {
        if (this->consume(input, m, this->re_file)) {
          block_file(input);
        } else {
          cerr << "(config) Unrecognized input: '"
               << input.substr(0, input.find("\n")) << "'" << endl;
          cerr << "Line: " << this->line_number << endl;
        }

        if (input.size() == 0)
          done = true;
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
          done = true;
        } else {
          cerr << "(file block) Unrecognized input: '"
               << input.substr(0, input.find("\n")) << "'" << endl;
          cerr << "Line: " << this->line_number << endl;
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
          cout << "reaction: " << m.str(1) << endl;
        } else if (this->consume(input, m, this->re_key)) {
        } else if (this->consume(input, m, this->re_match_syslog)) {
        } else if (this->consume(input, m, this->re_syslog_prog)) {
        } else if (this->consume(input, m, this->re_syslog_host)) {
        } else if (this->consume(input, m, this->re_shell)) {
        } else if (this->consume(input, m, this->re_block_end)) {
          done = true;
        } else {
          cerr << "(matchtype block) Unrecognized input: '"
               << input.substr(0, input.find("\n")) << "'" << endl;
          cerr << "Line: " << this->line_number << endl;
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
};

#endif /* ifndef __GROKCONFIG_HPP */
