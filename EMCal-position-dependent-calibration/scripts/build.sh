#!/usr/bin/env bash

cd current

echo "building g4eval"
./build_package.sh

echo "building Fun4All"
make clean

make
