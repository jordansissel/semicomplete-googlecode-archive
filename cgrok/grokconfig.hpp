
#ifndef __GROKCONFIG_HPP
#define __GROKCONFIG_HPP

#include <string>
#include <iostream>
#include <sstream>

#include <boost/xpressive/xpressive.hpp>

#include "watchfileentry.hpp"

using namespace boost::xpressive;
using namespace std;

/* Shorthand to compile a dynamic regex */
#define RE(x) (sregex::compile(x))

/* Match "\s*=\s*" */
#define R_EQ (*_s >> "=" >> *_s)

/* match ';' or newline */
#define R_TERMINATOR (as_xpr(';') | _n)

typedef void (*grok_config_method)(string arg);

string StripQuotes(const string &str) {
  return str.substr(1, str.size() - 2);
}

class GrokConfig {
  public:
    typedef vector<WatchFileEntry> watch_file_vector_type;

    GrokConfig() {
      this->patterns.LoadFromFile("patterns");
      this->re_comment = ('#' >> *~_n);
      this->re_whitespace = +_s;

      /* Tokens */
      this->re_block_begin = *_s >> as_xpr('{');
      this->re_block_end = *_s >> as_xpr('}') >> !as_xpr(';');
      this->re_string = RE("(?<!\\\\)(?:\"(?:(?:\\\\\")*[^\"]*\"))");
      this->re_number = RE("(?:[+-]?(?:(?:[0-9]+(?:\\.[0-9]*)?)|(?:\\.[0-9]+)))");
      this->re_boolean = RE("(?:[Tt]rue|[Ff]alse)");

      /* Prefixing everything with 'bos >>' is a hack because adding it later
       * causes captures not to be obeyed. Strange. */
      this->re_skip = bos >> +(this->re_comment | this->re_whitespace);
      this->re_file = bos >> "file" >> +_s >> (s1=re_string) >> re_block_begin;
      this->re_exec = bos >> "exec" >> +_s >> (s1=re_string) >> re_block_begin;
      this->re_filelist = bos >> "filelist" >> +_s >> (s1=re_string) >> re_block_begin;

      this->re_matchtype = bos >> "type" >> +_s >> (s1=re_string) >> re_block_begin;

      this->re_match = bos >> "match" >> R_EQ >> (s1=re_string) >> R_TERMINATOR;
      this->re_reaction = bos >> "reaction" >> R_EQ >> (s1=re_string) >> R_TERMINATOR;
      this->re_threshold = bos >> "threshold" >> R_EQ >> (s1=re_string) >> R_TERMINATOR;
      this->re_follow = bos >> "follow" >> R_EQ >> (s1=re_boolean) >> R_TERMINATOR;
      this->re_interval = bos >> "interval" >> R_EQ >> (s1=re_string) >> R_TERMINATOR;
      this->re_key = bos >> "key" >> R_EQ >> (s1=re_string) >> R_TERMINATOR;
      this->re_match_syslog = bos >> "match_syslog" >> R_EQ >> (s1=re_string) >> R_TERMINATOR;
      this->re_syslog_prog = bos >> "syslog_prog" >> R_EQ >> (s1=re_string) >> R_TERMINATOR;
      this->re_syslog_host = bos >> "syslog_host" >> R_EQ >> (s1=re_string) >> R_TERMINATOR;
      this->re_shell = bos >> "shell" >> R_EQ >> (s1=re_string) >> R_TERMINATOR;

      //cout << "this->re_comment: " << this->re_comment.regex_id() << endl;
      //cout << "this->re_whitespace: " << this->re_whitespace.regex_id() << endl;
      //cout << "this->re_block_begin: " << this->re_block_begin.regex_id() << endl;
      //cout << "this->re_block_end: " << this->re_block_end.regex_id() << endl;
      //cout << "this->re_string: " << this->re_string.regex_id() << endl;
      //cout << "this->re_number: " << this->re_number.regex_id() << endl;
      //cout << "this->re_boolean: " << this->re_boolean.regex_id() << endl;
      //cout << "this->re_skip: " << this->re_skip.regex_id() << endl;
      //cout << "this->re_file: " << this->re_file.regex_id() << endl;
      //cout << "this->re_filelist: " << this->re_filelist.regex_id() << endl;
      //cout << "this->re_matchtype: " << this->re_matchtype.regex_id() << endl;
      //cout << "this->re_match: " << this->re_match.regex_id() << endl;
      //cout << "this->re_reaction: " << this->re_reaction.regex_id() << endl;
      //cout << "this->re_threshold: " << this->re_threshold.regex_id() << endl;
      //cout << "this->re_follow: " << this->re_follow.regex_id() << endl;
      //cout << "this->re_interval: " << this->re_interval.regex_id() << endl;
      //cout << "this->re_key: " << this->re_key.regex_id() << endl;
      //cout << "this->re_match_syslog: " << this->re_match_syslog.regex_id() << endl;
      //cout << "this->re_syslog_prog: " << this->re_syslog_prog.regex_id() << endl;
      //cout << "this->re_syslog_host: " << this->re_syslog_host.regex_id() << endl;
      //cout << "this->re_shell: " << this->re_shell.regex_id() << endl;
    }

    void parse(const string config) {
      string input = config;
      this->line_number = 1;
      block_config(input);
    }

    bool consume(string &input, smatch &m, sregex &re) {
      int ret;

      /* Skip whitespace and comments */
      if (re.regex_id() != this->re_skip.regex_id()) {
        this->consume(input, m, this->re_skip);
      }

      ret = regex_search(input, m, re);
      if (ret) {
        string::size_type len = m.position(0) + m.length(0);
        string::size_type pos = string::npos;

        string consumed = input.substr(0, len);
        while ((pos = consumed.find("\n", pos + 1)) != string::npos)
          this->line_number++;

        //cout << "Consuming: [" << re.regex_id() << "] '" << input.substr(0, len) << "'" << endl;
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
          WatchFileEntry f;
          cout << "Filename: " << m.str(1) << endl;
          f.name = StripQuotes(m.str(1));
          f.fo.AddFile(f.name);
          current_file_entry = f;
          block_file(input);
          inputs.push_back(current_file_entry);
        } else if (this->consume(input, m, this->re_exec)) {
          WatchFileEntry f;
          cout << "Exec: " << m.str(1) << endl;
          f.name = StripQuotes(m.str(1));
          f.fo.AddCommand(f.name);
          current_file_entry = f;
          block_file(input);
          inputs.push_back(current_file_entry);
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
          WatchMatchType wmt;
          wmt.clear();
          wmt.type_name = StripQuotes(m.str(1));
          current_match_type = wmt;
          block_matchtype(input);
          current_file_entry.match_types.push_back(current_match_type);
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
      stringstream strconv(stringstream::in | stringstream::out);
      cout << "Matchtype" << endl;
      while (!done) {
        if (this->consume(input, m, this->re_match)) {
          cout << "Match: " << m.str(1) << endl;
          GrokRegex<sregex> gre(StripQuotes(m.str(1)));
          gre.AddPatternSet(this->patterns);
          current_match_type.match_strings.push_back(gre);
        } else if (this->consume(input, m, this->re_threshold)) {
          strconv << m.str(1);
          strconv >> current_match_type.threshold;
        } else if (this->consume(input, m, this->re_reaction)) {
          current_match_type.reaction = StripQuotes(m.str(1));
          cout << "reaction: " << current_match_type.reaction << endl;
        } else if (this->consume(input, m, this->re_boolean)) {
          if (m.str(1) == "true" || m.str(1) == "True")
            current_match_type.follow = true;
          else
            current_match_type.follow = false;
        } else if (this->consume(input, m, this->re_interval)) {
          strconv << m.str(1);
          strconv >> current_match_type.interval;
        } else if (this->consume(input, m, this->re_key)) {
          current_match_type.key = StripQuotes(m.str(1));
        } else if (this->consume(input, m, this->re_match_syslog)) {
          //current_match_type.match_syslog = StripQuotes(m.str(1));
        } else if (this->consume(input, m, this->re_syslog_prog)) {
          current_match_type.syslog_prog = StripQuotes(m.str(1));
        } else if (this->consume(input, m, this->re_syslog_host)) {
          current_match_type.syslog_host = StripQuotes(m.str(1));
        } else if (this->consume(input, m, this->re_shell)) {
        } else if (this->consume(input, m, this->re_block_end)) {
          cout << "(matchtype block) block close found" << endl;
          done = true;
        } else {
          this->parse_error("matchtype block", input);
        }
      }
    }

    watch_file_vector_type &GetFileEntries() {
      return inputs;
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
    sregex re_exec;
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
      sregex re_follow;
      sregex re_boolean;

    GrokPatternSet<sregex> patterns;
    watch_file_vector_type inputs;
    WatchFileEntry current_file_entry;
    WatchMatchType current_match_type;
};

#endif /* ifndef __GROKCONFIG_HPP */
