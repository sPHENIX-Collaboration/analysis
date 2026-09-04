# TpcHadronReco

Standalone sPHENIX Fun4All analysis module extracted from `TrackingDiagnostics`.
It keeps the existing TPC PolyTrack fit and two-track candidate reconstruction,
while adding Lambda-bachelor reconstruction for strange resonances and cascades.

## Contents

- `TpcTrackFit.h` — common fit/state data structures.
- `TpcTrackHelixFitter.{h,cc}` — analytic helix fit and helix geometry/PCA tools.
- `TpcTrackKalmanFitter.{h,cc}` — custom Kalman/RKN track fit and propagation.
- `TpcV0CandidateTree.{h,cc}` — Fun4All module and candidate output.

The public class name is intentionally kept as `TpcV0CandidateTree` for macro
compatibility. The module now reconstructs more than V0s, so this can be renamed
later after its final repository/location is chosen.

## Fit modes

```cpp
reco->set_track_fit_method("kalman");  // aliases: "kf"
reco->set_track_fit_method("helix");   // alias: "circle"
reco->set_track_fit_method("line");
```

## Added channels

- `Sigma(1385)^+/- -> Lambda pi^+/-` and charge conjugates.
- `Xi^- -> Lambda pi^-`, `anti-Xi^+ -> anti-Lambda pi^+`.
- `Omega^- -> Lambda K^-`, `anti-Omega^+ -> anti-Lambda K^+`.

`Xi`/`Omega` use a real cascade topology: the reconstructed neutral Lambda is a
straight line from its V0 decay vertex, and the charged bachelor is propagated
with the selected Kalman/helix track model. The cascade vertex is the PCA of
those two trajectories.

A separate loose Lambda seed selection is used for the hyperon stage. It does
**not** require the Lambda to point to the primary vertex, because a Lambda from
Xi/Omega feed-down originates at the cascade vertex.

## Output trees

- `pairTree` — existing two-track candidates.
- `likeSignPairTree` — existing optional same-sign background.
- `sigma1385Tree` — present when Sigma reconstruction is enabled.
- `cascadeTree` — present when Xi and/or Omega reconstruction is enabled.
- `trackTree` and `clusterResidualTree` — existing optional QA.

In `cascadeTree`, `candidate_mask` is bit 0 for Xi and bit 1 for Omega. Both
`mass_xi` and `mass_omega` are stored for every accepted Lambda+bachelor
combination.

## Build

Typical local sPHENIX build:

```bash
./autogen.sh --prefix=$MYINSTALL
make -j4
make install
```

Then load `libTpcHadronReco.so` and include
`<tpchadronreco/TpcV0CandidateTree.h>` from the Fun4All macro.

## Macro switches

The supplied `Fun4All_TpcV0CandidateTree.C` keeps the existing reconstruction
settings and adds the new channels as opt-in switches, so an existing K0S/Lambda
production does not become more expensive unless requested.

```bash
# Track fit
export V0_FIT_MODE=kalman   # or: helix

# New channels
export V0_ENABLE_SIGMA1385=1
export V0_ENABLE_XI=1
export V0_ENABLE_OMEGA=1

root -l -b -q 'Fun4All_TpcV0CandidateTree.C("input.list","hadron.root",0,0)'
```

The Lambda seed used by the composite-hyperon stage has its own
`V0_HYPERON_LAMBDA_*` settings. Cascade topology and numerical PCA search
settings use `V0_XI_*`, `V0_OMEGA_*`, and `V0_CASCADE_*` variables in the macro.

## Migration from TrackingDiagnostics

Replace

```cpp
R__LOAD_LIBRARY(libTrackingDiagnostics.so)
#include <trackingdiagnostics/TpcV0CandidateTree.h>
```

with

```cpp
R__LOAD_LIBRARY(libTpcHadronReco.so)
#include <tpchadronreco/TpcV0CandidateTree.h>
```

No change is required to the `TpcV0CandidateTree` constructor or to the existing
K0S/Lambda/phi/Jpsi/D0 setters.
