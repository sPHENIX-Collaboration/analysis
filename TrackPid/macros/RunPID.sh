#!/bin/sh
cd /sphenix/u/weihuma/tutorials/AnaTutorial/src/build/
/sphenix/u/weihuma/tutorials/AnaTutorial/src/autogen.sh --prefix=$MYINSTALL
make
make install
echo "finished anatutorial make install"
cd /sphenix/u/weihuma/analysis/TrackPid/build
/sphenix/u/weihuma/analysis/TrackPid/autogen.sh --prefix=$MYINSTALL
make
make install
echo "finished ElectronPid make install"
cd /sphenix/u/weihuma/analysis/TrackPid/macros
