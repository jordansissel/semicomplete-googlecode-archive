define(`TITLE', `ifdef(`MYTITLE', `MYTITLE', `Open Source Adventures')')dnl
define(`CAPTION', `ifdef(`MYCAPTION', `MYCAPTION', `Technology Projects and Articles')')dnl
dnl
define(`forloop',
		 `pushdef(`$1', `$2')_forloop(`$1', `$2', `$3', `$4')popdef(`$1')')
		 define(`_forloop',
				  `ifelse($1, `$3', ,
				  `$4`'define(`$1', incr($1))_forloop(`$1', `$2', `$3', `$4')')')



