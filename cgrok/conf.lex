%{
#include "grok_config.tab.h"
#include "grok_config.h"

%}
%option noyywrap debug bison-bridge 

qstring \"((\\.)|[^\\\"])*\"
true true|yes|on|1
false false|no|off|0

%%

{qstring} { yylval->str = yytext; return QUOTEDSTRING; }
program { return PROGRAM; }
file { return PROG_FILE; }
exec { return PROG_EXEC; }
follow { return FOLLOW; }
\{ { return '{'; }
\} { return '}'; }
: { return ':'; }

[ \t] { /* ignore whitespace */ }
{true} { yylval->num = 1; return BOOLEAN; }
{false} { yylval->num = 0; return BOOLEAN; }
%%
