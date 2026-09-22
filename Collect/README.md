# Collect

`Collect` is a Fun4All `SubsysReco` module for sPHENIX Run-3 pp analyses. In a
single pass over each event it writes out:

- a **track tree**, with per-track kinematics, quality flags, dE/dx
  (multiple estimators), and optional calorimeter-tower-energy-around-track
  grids, and
- a **V0/decay tree**, built from `KFParticle` K-short / Lambda / anti-Lambda
  candidates, with full daughter-track linkage back to the track tree.

The tree written by Collect is intended to support both spectra and correlations
analyses. Both whole-acceptance correlation functions, e.g. R2(dy,dphi), and
femtoscopic correlation functions, e.g. C(Q), can be built for arbitrary pair
species and charges from these trees. This is possible because the trees carry
per-track tracking-hit bitmasks and cluster keys, which support sensitive
identification and removal of split tracks — a single real particle mistakenly
reconstructed as two separate tracks, which does real damage to C(Q) and R2
near (dy,dphi)~(0,0).

Tracks written to the tree must be associated with a primary vertex in a single
crossing, either directly by the vertex finder or by hand: for every track,
Collect computes its 3D DCA to each candidate vertex and requires |dca_xy| and
|dca_z| both <= 1.5 cm. Tracks that are daughters of a found V0 are always
saved, regardless of this cut.

V0s are reconstructed by KFParticle already quite cleanly, since KFP's own V0
cuts are applied in the F4A macro itself. The mass windows intentionally leave
enough room on either side of the peak to perform sideband subtraction and
remove the backgrounds visible in the V0 invariant-mass plots.

A few extra things Collect does to keep the trees clean:

- **Duplicate-V0 cleanup**: if two reconstructed V0s in the same crossing
  share a daughter track, only the better-fit one (lower chi2/ndf) is kept.
- **One-vertex-per-crossing**: when several reconstructed vertices land in
  the same beam crossing (real pileup, or a split-vertex artifact), Collect
  keeps only the "hottest" one — the vertex with the most tracks genuinely
  consistent with it by DCA — so tracks and V0s are never double-counted
  across near-duplicate vertices.
- **Split-track bookkeeping**: persisted per-track hit masks and cluster
  keys let downstream analysis recompute a STAR-style Splitting Level for
  any pair, sibling or mixed-event, to identify and remove split-track
  pairs. An inline version of this cut exists in Collect itself but is
  currently disabled pending validation on real data.

## How to Build

`Collect` is a standard sPHENIX autotools package (`configure.ac` /
`Makefile.am`), meant to be built against an sPHENIX offline software
environment.

```sh
# Inside an sPHENIX environment (OFFLINE_MAIN and MYINSTALL set, e.g. via
# `source setup.sh <build>` from sPHENIX's cvmfs area or your own build):

git clone https://github.com/sPHENIX-Collaboration/analysis.git
cd analysis/Collect

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
