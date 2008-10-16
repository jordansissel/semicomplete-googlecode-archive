%{ 
#include <stdio.h>
#include "grok_config.tab.h"
#include "grok_config.h"

int yylineno;
void yyerror (YYLTYPE *loc, struct config *conf, char const *s) {
  fprintf (stderr, "some error: %s\n", s);
  fprintf (stderr, "Line: %d\n", yylineno);
}

%}

%union{
  char *str;
  int num;
}

%token <str> QUOTEDSTRING
%token PROGRAM "program"
%token PROG_FILE "file"
%token PROG_EXEC "exec"
%token PROG_MATCH "match"
%token FOLLOW "follow"
%token <num> BOOLEAN
%token '{' '}' ';' ':' '\n'

%pure-parser
%parse-param {struct config *conf}
%locations

%start config

%%

config: config statement 
      | statement 
      | error { printf("Error: %d\n", yylloc.first_line); }

statement: PROGRAM '{' { conf_new_program(conf); }
           program_block 
         '}'
       
program_block: program_block program_statement 
             | program_statement
program_statement: program_file 

program_file: "file" QUOTEDSTRING '{' 
              { /* create new file input */ }
              file_block 
              '}' 

file_block: /*empty*/
          | "follow" ':' BOOLEAN
