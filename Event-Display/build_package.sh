#!/usr/bin/env bash

mkdir -p build
cd build

echo "Running autogen"
$(readlink -f ../src)/autogen.sh --prefix=$MYINSTALL

echo "Running make install"
make -j 8 install
