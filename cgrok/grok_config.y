%{ 
#include <stdio.h>
#include "grok_config.h"

void yyerror (struct config *conf, char const *s) {
  fprintf (stderr, "%s\n", s);
}
%}

%union{
  char *str;
}

%token <str> QUOTEDSTRING
%token PROGRAM

%pure-parser
%parse-param {struct config *conf}
%lex-param {struct config *conf}
%start config

%%

config: qstring | programblock;

qstring: QUOTEDSTRING { printf("Read string: '%s'\n", $1); }
  ;

programblock: PROGRAM { conf->nprograms++; }
  ;
