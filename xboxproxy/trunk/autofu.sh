set -x
set -e
touch NEWS README AUTHORS ChangeLog depcomp
aclocal15 || aclocal19 || aclocal
autoheader259 || autoheader
autoconf259 || autoconf
automake15 -a Makefile || automake19 -a Makefile || automake -a Makefile

