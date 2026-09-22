# Collect

`Collect` is a Fun4All `SubsysReco` module for sPHENIX Run-3 pp analyses. In a
single pass over each event it writes out:

- a **track tree**, with per-track kinematics, quality flags, dE/dx
  (multiple estimators), and optional calorimeter-tower-energy-around-track
  grids, and
- a **V0/decay tree**, built from `KFParticle` K-short / Lambda / anti-Lambda
  candidates, with full daughter-track linkage back to the track tree.

The tree written by Collect is intended to support both spectra and correlations
analyses. Both whole-acceptance CFs, e.g. R2(dy,dphi) and Femtoscopic CFs, e.g. C(Q),
can be build for arbitrary pair species and charges based on these trees. This
is possible because the trees contain ulong tracking hit masks and the i cluster
keys for every track. These variables support sensitive identification and 
removal of split tracks, which do great damage to C(Q) and R2 near (dy,dphi)~(0,0).

Track written to the tree must be associated with a primary vertex in a single
crossing, either directly by the vertexer or by hand (loop through tracks and
call those with fabs(dca_xy) and _z both <=1.5cm. Tracks that are the daughters 
of found V0s are always saved.

V0s are reconstructed by KFP already quite cleanly, as KFP V0 cuts are applied
in the F4A macro itself. The mass windows intentionally allow sufficient space to
perform side-band corrections to the CFs to remove the backgrounds seen in the V0
Minv plots. 

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
