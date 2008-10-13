%{ 
#define YYPARSE_PARAM scanner
#define YYLEX_PARAM   scanner
#include <stdio.h>
void yyerror (char const *s) {
  fprintf (stderr, "%s\n", s);
}
%}

%union{
  int num;
  char *str;
}

%pure-parser
%token <str> QUOTEDSTRING
%token <int> DIGIT
%start config

%%

config: /* */
        | DIGIT { printf("testing\n"); }
        | QUOTEDSTRING { printf("Read string: '%s'\n", $1); }
      ;
