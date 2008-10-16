%{ 
#include <stdio.h>
#include <string.h>

#include "conf.tab.h"
#include "grok_config.h"
#include "grok_input.h"
#include "grok_matchconf.h"

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
%token <num> INTEGER

%token PROGRAM "program"
%token PROG_FILE "file"
%token PROG_EXEC "exec"
%token PROG_MATCH "match"
%token PROG_LOADPATTERNS "load-patterns"

%token FILE_FOLLOW "follow"

%token EXEC_RESTARTONFAIL "restart-on-failure"
%token EXEC_MINRESTARTDELAY "minimum-restart-delay"
%token EXEC_RUNINTERVAL "run-interval"

%token MATCH_PATTERN "pattern"
%token MATCH_REACTION "reaction"

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
       
program_block: /*empty*/
             | program_block program_statement 
             | program_statement

program_statement: program_file 
                 | program_exec
                 | program_match
                 | program_load_patterns

program_load_patterns: "load-patterns" ':' QUOTEDSTRING 
                     { conf_new_patternfile(conf); CURPATTERNFILE = $3; }

program_file: "file" QUOTEDSTRING '{' 
              { conf_new_input(conf);
                CURINPUT.type = I_FILE;
                CURINPUT.source.file.filename = $2;
                printf("curinput: %x\n", &CURINPUT);
              }
              file_block 
              '}' 

program_exec: "exec" QUOTEDSTRING '{'
              { conf_new_input(conf);
                CURINPUT.type = I_PROCESS;
                CURINPUT.source.process.cmd = $2;
              }
              exec_block
              '}'

program_match: "match" '{' { conf_new_matchconf(conf); }
               match_block
               '}'
                

file_block: /* empty */
          | "follow" ':' INTEGER 

match_block: /* empty */
           | "pattern" ':' QUOTEDSTRING { grok_compile(&CURMATCH.grok, $3); }
           | "reaction" ':' QUOTEDSTRING { CURMATCH.reaction.cmd = $3; }

exec_block: /* empty */
          | "restart-on-failure" ':'  INTEGER 
             { CURINPUT.source.process.restart_on_death = $3 }
          | "minimum-restart-delay" ':' INTEGER
             { CURINPUT.source.process.min_restart_delay = $3 }
          | "run-interval" ':' INTEGER
             { CURINPUT.source.process.run_interval = $3 }
