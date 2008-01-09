
#include "grokregex.h"
#include "grokmatch.h"

GrokMatch::GrokMatch(const GrokRegex *gre, OnigRegion *region) {
  this->gre = gre;
  this->region = region;
}

GrokMatch::~GrokMatch() {
  onig_region_free(this->region, 1);
}

