define(`TITLE', `ifdef(`MYTITLE', `MYTITLE', `Open Source Adventures')')dnl
define(`CAPTION', `ifdef(`MYCAPTION', `MYCAPTION', `Technology Projects and Articles')')dnl
dnl
dnl define(`forloop',
dnl 		 `pushdef(`$1', `$2')_forloop(`$1', `$2', `$3', `$4')popdef(`$1')')
dnl 		 define(`_forloop',
dnl 				  `ifelse($1, `$3', ,
dnl 				  `$4`'define(`$1', incr($1))_forloop(`$1', `$2', `$3', `$4')')')
dnl #define(`BACKPATH', `forloop(`p',`0',DEPTH, `../')')dnl
define(`pinclude', `include(BACKPATH`'$1)')dnl
