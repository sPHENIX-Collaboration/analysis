#!/usr/bin/env sh

cp src/CaloEvaluator.cc src/CaloEvaluator.h src/g4eval

rm -rf bin
mkdir -p bin
cd bin

echo "Running autogen"
# $(readlink -f ../src/Calibration)/autogen.sh --prefix=$MYINSTALL
$(readlink -f ../src/g4eval)/autogen.sh --prefix=$MYINSTALL

echo "Running make"
make -j 8

echo "Running make install"
make -j 8 install
