#!/usr/bin/env bash

rm -rf build
mkdir -p build/CaloReco build/g4eval
cd build/g4eval

echo "Running autogen: g4eval"
$(readlink -f ../../src/g4eval)/autogen.sh --prefix=$MYINSTALL

echo "Running make install"
make -j 8 install

cd ../CaloReco

echo "Running autogen: CaloReco"
$(readlink -f ../../src/CaloReco)/autogen.sh --prefix=$MYINSTALL

echo "Running make install"
make -j 8 install
