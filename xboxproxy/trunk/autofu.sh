set -x
set -e
touch NEWS README AUTHORS ChangeLog

aclocal19 || aclocal15 || aclocal
autoheader259 || autoheader
autoconf259 || autoconf
automake19 -a || automake15 -a || automake -a

