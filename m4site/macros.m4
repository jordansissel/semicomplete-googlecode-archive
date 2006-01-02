define(`TITLE', `ifdef(`MYTITLE', `MYTITLE', `Open Source Adventures')')dnl
define(`CAPTION', `ifdef(`MYCAPTION', `MYCAPTION', `Technology Projects and Articles')')dnl
dnl define(`forloop',
dnl 		 `pushdef(`$1', `$2')_forloop(`$1', `$2', `$3', `$4')popdef(`$1')')
dnl 		 define(`_forloop',
dnl 				  `ifelse($1, `$3', ,
dnl 				  `$4`'define(`$1', incr($1))_forloop(`$1', `$2', `$3', `$4')')')
dnl #define(`RELPATH', `forloop(`p',`0',DEPTH, `../')')dnl
define(`pinclude', `include(RELPATH`'$1)')dnl
define(`LINK', `<a href="/~psionic/new/site/$1">$2</a>')dnl
dnl define(`PROJECTLIST', `testing')dnl
define(`PROJECTLINK',`
<li>LINK($1/$2,$2)</li>
')dnl
pinclude(dirlinks.m4)
