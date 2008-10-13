%{
#include "grok_config.tab.h"
//qstring ["][^\n\"]*["]
%}
%option noyywrap debug bison-bridge 

qstring \"((\\.)|[^\\\"])*\"
digit [0-9]

%%

{qstring} { yylval->str = yytext; return QUOTEDSTRING; }
{digit} { printf("Digit: %c\n", *yytext); return DIGIT; }

[ \t] { /* ignore whitespace */ }
[\n] { yylineno++; }

%%
