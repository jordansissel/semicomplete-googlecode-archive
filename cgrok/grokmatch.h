
#ifndef GROKMATCH_H
#define GROKMATCH_H

#include <map>
using namespace std;

#include <oniguruma.h>

class GrokRegex; /* defined in grokregex.h */

class GrokMatch {
  public:
    GrokMatch(const GrokRegex *gre, OnigRegion * const region);
    ~GrokMatch();

  private:
    OnigRegion *region;
    const GrokRegex *gre;
    //map< string, string, less<string> > named_groups;
    //map< int, string, less<int> > numbered_groups;
};

#endif /* ifdef GROKMATCH_H */
