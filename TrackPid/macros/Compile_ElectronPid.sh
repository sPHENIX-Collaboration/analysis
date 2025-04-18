#!/bin/sh
cd /sphenix/u/weihuma/analysis/TrackPid/build
/sphenix/u/weihuma/analysis/TrackPid/src/autogen.sh --prefix=$MYINSTALL
make
make install
echo "finished ElectronPid make install"
cd /sphenix/u/weihuma/analysis/TrackPid/macros
