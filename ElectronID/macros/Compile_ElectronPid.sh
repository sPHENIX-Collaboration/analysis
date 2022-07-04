#!/bin/sh
cd /sphenix/u/weihuma/analysis/ElectronID/src/build
/sphenix/u/weihuma/analysis/ElectronID/src/autogen.sh --prefix=$MYINSTALL
make
make install
echo "finished ElectronID make install"
cd /sphenix/u/weihuma/analysis/ElectronID/macros
