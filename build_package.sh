#!/usr/bin/env bash

rm -rf bin
mkdir -p bin
cd bin

echo "Running autogen"
# $(readlink -f ../src/Calibration)/autogen.sh --prefix=$MYINSTALL
$(readlink -f ../src)/autogen.sh --prefix=$MYINSTALL

echo "Running make"
make -j 8

echo "Running make install"
make -j 8 install
