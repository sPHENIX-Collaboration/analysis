# Migration notes

## What moved

The standalone package owns the complete fitting/reconstruction chain that was
used by `TpcV0CandidateTree`:

- `TpcTrackFit.h`
- `TpcTrackHelixFitter.{h,cc}`
- `TpcTrackKalmanFitter.{h,cc}`
- `TpcV0CandidateTree.{h,cc}`

The library is now `libTpcHadronReco.so`; installed headers live under
`tpchadronreco/`.

## Compatibility choices

- The public class remains `TpcV0CandidateTree` for now.
- Existing pair-tree branches and existing species selection setters are kept.
- Existing fit aliases are kept: `kalman`/`kf`, `helix`/`circle`, and
  `none`/`line`.
- Sigma(1385), Xi, and Omega are disabled by default.

## New reconstruction

### Sigma(1385)

A loose reconstructed Lambda/anti-Lambda is combined with a bachelor pion whose
momentum is evaluated at its primary-vertex PCA. Accepted candidates are written
to `sigma1385Tree`.

### Xi and Omega

A Lambda/anti-Lambda is first reconstructed from its two charged daughters. The
neutral Lambda trajectory is then represented by a line beginning at the Lambda
decay vertex. The charged bachelor trajectory uses the selected fitted-track
model (Kalman first, helix when selected, straight-line fallback). Their PCA
defines the cascade vertex. Xi and Omega mass hypotheses are evaluated using the
pion and kaon bachelor masses respectively and written to `cascadeTree`.

The hyperon Lambda seed intentionally has no primary-vertex DIRA requirement;
that requirement would reject the topology we are trying to reconstruct for Xi
and Omega.

## Small cleanup/fixes made during extraction

- The magnetic-field object created by the module is owned with `std::unique_ptr`
  instead of being left unmanaged.
- The supplied macro now applies `V0_KALMAN_ITERATIONS` rather than only printing
  it.
- The supplied macro respects `V0_WRITE_LIKESIGN_TREE` rather than forcing the
  like-sign tree on.
- Same-crossing node/requirement settings are exposed through environment
  variables.
- Output metadata records the fit mode and enabled composite channels.
