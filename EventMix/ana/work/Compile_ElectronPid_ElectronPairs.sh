#!/bin/sh
cd /sphenix/u/weihuma/analysis/TrackPid/build
/sphenix/u/weihuma/analysis/TrackPid/src/autogen.sh --prefix=$MYINSTALL
make
make install
echo "finished ElectronPid make install"
cd /sphenix/u/weihuma/analysis/EventMix/src/build
/sphenix/u/weihuma/analysis/EventMix/src/autogen.sh --prefix=$MYINSTALL
make
make install
echo "finished EventMix make install"
cd /sphenix/u/weihuma/analysis/EventMix/ana/build
/sphenix/u/weihuma/analysis/EventMix/ana/autogen.sh --prefix=$MYINSTALL
make
make install
echo "finished EventMix ana make install"
cd /sphenix/u/weihuma/analysis/EventMix/ana/work
