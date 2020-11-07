#!/bin/sh
srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

(cd $srcdir; aclocal -I ${OFFLINE_MAIN}/share;\
libtoolize --copy --force; automake -a --copy --add-missing; autoconf)

$srcdir/configure "$@"


