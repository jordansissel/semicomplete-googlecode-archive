
#ifndef __WATCHFILEENTRY_HPP
#define __WATCHFILEENTRY_HPP

#include "fileobserver.hpp"

struct WatchMatchType {
  vector < GrokRegex<sregex> > match_strings;
  float threshold;
  float interval;
  string key;
  string reaction;
  bool match_syslog;
  string syslog_prog;
  string syslog_host;
  string shell; /* Not supported yet */

  void clear() {
    this->threshold = 0.0;
    this->interval = 0.0;
    this->key = "";
    this->reaction = "";
    this->match_syslog = false;
    this->syslog_prog = "";
    this->syslog_host = "";
    this->shell = "";
    this->match_strings.clear();
  }
};

struct WatchFileEntry {
  FileObserver fo;
  string name;
  vector < WatchMatchType > match_types;

  void clear() {
    this->name = "";
    this->match_types.clear();
  }
};

#endif /* ifndef __WATCHFILEENTRY_HPP */
