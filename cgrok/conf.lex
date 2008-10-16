%{
#include <string.h>
#include "conf.tab.h"
#include "grok_config.h"

%}
%option noyywrap bison-bridge  debug

true true|yes|on|1
false false|no|off|0
qstring \"((\\.)|[^\\\"])*\"
number [0-9]+
%%

\{ { return '{'; }
\} { return '}'; }
: { return ':'; }

{qstring} { yylval->str = strndup(yytext + 1, yyleng - 2); return QUOTEDSTRING; }

program { return PROGRAM; }
load-patterns { return PROG_LOADPATTERNS; }

file { return PROG_FILE; }
follow { return FILE_FOLLOW; }

exec { return PROG_EXEC; }
restart-on-failure { return EXEC_RESTARTONFAIL; }
minimum-restart-delay { return EXEC_MINRESTARTDELAY; }
run-interval { return EXEC_RUNINTERVAL; }

match { return PROG_MATCH; }
pattern { return MATCH_PATTERN; }
reaction { return MATCH_REACTION; }

{true} { yylval->num = 1; return INTEGER; }
{false} { yylval->num = 0; return INTEGER; }
{number} { yylval->num = atoi(yytext); return INTEGER; }

[ \t] { /* ignore whitespace */ }
%%
