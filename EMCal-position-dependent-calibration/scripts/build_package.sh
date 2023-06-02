#!/usr/bin/env bash

rm -rf src/g4eval
cd /gpfs/mnt/gpfs02/sphenix/user/anarde/sPHENIX/coresoftware
git pull
cd -

cp -r  /gpfs/mnt/gpfs02/sphenix/user/anarde/sPHENIX/coresoftware/simulation/g4simulation/g4eval/ src
cp src/CaloEvaluator.cc src/CaloEvaluator.h src/g4eval

rm -rf build
mkdir -p build
cd build

echo "Running autogen"
$(readlink -f ../src/g4eval)/autogen.sh --prefix=$MYINSTALL

echo "Running make"
make -j 8

echo "Running make install"
make -j 8 install
