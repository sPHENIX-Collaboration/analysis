#!/bin/sh
# Build Collect and install into $MYINSTALL.
# Run with:  sh do_build.sh
set -e

: "${OFFLINE_MAIN:?OFFLINE_MAIN is not set}"
: "${MYINSTALL:?MYINSTALL is not set}"

cd "$(dirname "$0")"

if [ ! -f configure ]; then
  aclocal -I "${OFFLINE_MAIN}/share" && libtoolize --force && automake -a --add-missing && autoconf
fi

mkdir -p build
cd build
if [ ! -f Makefile ]; then
  ../configure --prefix="$MYINSTALL"
fi
make
make install
