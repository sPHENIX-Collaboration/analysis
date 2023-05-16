#!/usr/bin/env bash

rm -rf build
mkdir -p build
cd build

echo "Running autogen"
$(readlink -f ../src)/autogen.sh --prefix=$MYINSTALL

echo "Running make"
make -j 8

echo "Running make install"
make -j 8 install
