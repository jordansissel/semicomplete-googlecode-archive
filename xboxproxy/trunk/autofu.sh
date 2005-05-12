set -x
set -e
aclocal15 || aclocal19 || aclocal
autoheader259 || autoheader
autoconf259 || autoconf
automake15 -a || automake -a || automake19 -a

