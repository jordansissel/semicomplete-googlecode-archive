%module grok

%{
#include "boost/xpressive/xpressive.hpp"
#include "../grokpatternset.hpp"
#include "../grokregex.hpp"
#include "../grokmatch.hpp"
#include "../grokconfig.hpp"
%}

namespace boost { namespace xpressive { }; };
%include "boost/xpressive/xpressive.hpp"
%include "../grokpatternset.hpp"
%include "../grokregex.hpp"
%include "../grokmatch.hpp"
%include "../grokconfig.hpp"
