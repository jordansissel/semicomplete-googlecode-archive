set -x
set -e
aclocal15 || aclocal
autoheader259  || autoheader
autoconf259 || autoconf
automake15 -a Makefile || automake15 -a Makefile
