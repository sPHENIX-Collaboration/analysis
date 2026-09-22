# Collect

`Collect` is a Fun4All `SubsysReco` module for sPHENIX Run-3 pp analyses. In a
single pass over each event it writes out:

- a **track tree**, with per-track kinematics, quality flags, dE/dx
  (multiple estimators), and optional calorimeter-tower-around-track energy
  grids, and
- a **V0/decay tree**, built from `KFParticle` K-short / Lambda / anti-Lambda
  candidates, with full daughter-track linkage back to the track tree.

It replaces several earlier single-purpose analysis modules (dE/dx
harvesting, decay-vertex reconstruction, etc.) with one module reading
directly off the `KFParticle` containers and track maps already built by the
standard sPHENIX tracking/KFParticle reconstruction chain.

## How to Build

`Collect` is a standard sPHENIX autotools package (`configure.ac` /
`Makefile.am`), meant to be built against an sPHENIX offline software
environment.

```sh
# Inside an sPHENIX environment (OFFLINE_MAIN and MYINSTALL set, e.g. via
# `source setup.sh <build>` from sPHENIX's cvmfs area or your own build):

git clone https://github.com/wjllope/Collect.git
cd Collect

sh autogen.sh
mkdir build && cd build
../configure --prefix="$MYINSTALL"
make
make install
```

This installs `libcollect.so` (and `Collect.h`) into `$MYINSTALL`. A
`do_build.sh` convenience script wraps the same steps:

```sh
sh do_build.sh
```

## How to Run

Load the library and register `Collect` as a `SubsysReco` in your Fun4All
macro, after the tracking/KFParticle reconstruction chain and after
calorimeter tower geometry has been loaded (`Collect::InitRun()` needs the
tower geometry already on the node tree):

```cpp
#include <collect/Collect.h>
R__LOAD_LIBRARY(libcollect.so)

...

Collect *myCollect = new Collect("myCollect");
myCollect->setVerbosity(0);
myCollect->setSegment(segment);
myCollect->setRequireXingZero(ReqXing0);
myCollect->setAddTowersToTrackTree(collectAddTowersToTrackTree);
myCollect->outfileName(collFile.c_str());
se->registerSubsystem(myCollect);
```

- `setSegment()` records the input segment number in the output tree, for
  bookkeeping across batch jobs.
- `setRequireXingZero()` restricts output to bunch-crossing-zero vertices.
- `setAddTowersToTrackTree()` enables the calorimeter-tower-around-track
  energy grid on the track tree; when `false` (the default), extra dE/dx
  branches are booked instead and track-quality cuts are looser.
