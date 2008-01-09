#include <string>
#include <map>
#include <iostream>
#include <exception>
using namespace std;

#include <oniguruma.h>

typedef map< string, string, less<string> > re_capture;

class GrokRegex {
  public:
    GrokRegex(string pattern);
    ~GrokRegex();

    int match(string data);

  private:
    OnigRegex regex;
    OnigRegion *region;
    OnigErrorInfo errinfo;
};

class OnigurumaException: public exception {
  public:
    OnigurumaException(int errorcode) {
      char s[ONIG_MAX_ERROR_MESSAGE_LEN];
      onig_error_code_to_str((UChar *)s, errorcode);
      /* Do we need to strdup here? */
      errormsg = (const char *) strdup(s);
    }

    /* Why is this necessary? */
    virtual ~OnigurumaException() throw() {
    }

    virtual const char* what() const throw() {
      return errormsg.c_str();
    }

  private:
    string errormsg;
};

GrokRegex::GrokRegex(string pattern) {
  int ret;
  ret = onig_new(&regex, 
                 (const OnigUChar *) pattern.c_str(), 
                 (const OnigUChar *) pattern.c_str() + pattern.size(),
                 ONIG_OPTION_DEFAULT,
                 ONIG_ENCODING_UTF8,
                 ONIG_SYNTAX_DEFAULT,
                 &errinfo);
  if (ret != ONIG_NORMAL)
    throw OnigurumaException(ret);
}

GrokRegex::~GrokRegex() { }

int main(int argc, char **argv) {
  GrokRegex gre(".[*");

  return 0;
}
