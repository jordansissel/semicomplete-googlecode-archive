%{
#include "grok_config.tab.h"
#include "grok_config.h"
%}
%option noyywrap debug bison-bridge 

qstring \"((\\.)|[^\\\"])*\"
program "program"

%%

{qstring} { yylval->str = yytext; return QUOTEDSTRING; }
{program} { return PROGRAM; }

[ \t] { /* ignore whitespace */ }
[\n] { yylineno++; }

%%
