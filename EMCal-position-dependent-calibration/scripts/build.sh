#!/usr/bin/env bash

cd current

echo "building g4eval"
./build_package.sh
# ./build_package_fast.sh

echo "building Fun4All"
make clean

make -j 8
