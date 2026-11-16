# JES MC calibration (sPHENIX, Run 2024 p+p jets)

ROOT macros that derive the jet energy scale (JES) calibration of anti-kT jets (R = 0.2-0.8) from
PYTHIA simulation, and check it with a closure test. The output, `JES_Calibration_Final.root`, is the
calibration that `JetCalib` (coresoftware `offline/packages/jetbase`) applies in production.

## Method

1. **First pass:** the mean jet response, reco pT / truth pT, from Gaussian fits in bins of truth pT
   and the jet EMCal energy fraction (EMfrac). Only events with |z| <= 30 cm are used. The map is
   smoothed, interpolated to a fine grid and inverted. The result is the calibrated pT as a function of
   (reco pT, EMfrac).
2. **Residual (z, eta) correction:** after the first pass, a residual correction factor in bins of the
   signed z-vertex and the jet position in the z-dependent eta acceptance,
   f = (eta - eta_min(z)) / (eta_max(z) - eta_min(z)). Events without a vertex get a separate function of
   f, computed at z = 0.
3. **Full-chain closure:** both steps are applied to an independent pass over the simulation, and the
   response is checked versus z and f.

The two samples have different roles:
- Run 28 (natural z-vertex distribution) is used for the first pass and the closure.
- Run 38 (flat z-vertex distribution within +-200 cm) is used for the residual correction.

## Requirements

- **The sPHENIX environment and HTCondor:** for example the interactive nodes `sphnxuser01-08`. The condor
  scripts source `/opt/sphenix/core/bin/sphenix_setup.sh -n new`.
- **Simulation trees** from the sPHENIX jet treemaker, one file per job:
  `<treepath>/Run<N>_MBDz/single/output_<sample>_<index>.root`
  - Samples: `MB, Jet5GeV, Jet12GeV, Jet20GeV, Jet30GeV, Jet40GeV, Jet50GeV, Jet60GeV, Jet80GeV` for Run 28,
    and `Jet12GeV ... Jet80GeV` for Run 38.
  - Branches read: `z_vertex` (MBD z, -999 if none), `truth_z_vertex`, and for each radius X:
    `unsubjet0X_{pt,eta,phi,e,pt_calib,emcal_frac,ihcal_frac,ohcal_frac}` and `truthjet0X_{pt,eta,phi}`.
  - Use the campaign whose jets were reconstructed at the MBD vertex.
  - Gaps in `<index>` (failed jobs) are fine; missing files are skipped.
The next two inputs are **not in this repository: provide your own** and put them in this folder.
- **z-vertex reweighting inputs**: `input_zvertexreweight_run28.root` and `input_zvertexreweight_run38.root`.
  Each must hold a histogram `h_zvertex_reweight` vs z [cm]: the data MBD z distribution divided by the
  truth z distribution of that simulation run, both normalized. The histmaker weights each event with its
  value at the truth z.
- **`AtlasStyle.C` and `AtlasUtils.C`**, the ATLAS plot style macros (`SetAtlasStyle()`). They are included
  by the histcombiner, the histfitter and the zeta interpolation.

**Settings to adapt before running:**
- **The tree path: change it to your own trees.** As delivered, it points to the author's trees
  (`/sphenix/tg/tg01/jets/hanpuj/Run<N>_MBDz/single/`). It is set in two places, which must agree:
  - `Jet_resp_histmaker.C`: the `TString fname = ...` line;
  - `run_condor_sim.sh`: the `treepath=` line.
- **The radii and binning:** in `Jet_resp_Def_firstpass.h` and `Jet_resp_Def_closure.h`.
- **The truth-pT window of each sample:** in `Jet_resp_histmaker.C`.

## Files

| File | Role |
|---|---|
| `Jet_resp_Def_firstpass.h`, `Jet_resp_Def_closure.h` | Configuration: radii, z / eta / pT / EMfrac binning |
| `set_config.sh` | Installs one of them as `Jet_resp_Def.h`, which all macros include |
| `Jet_resp_histmaker.C` | Fills the response histograms from the trees: selection, matching, weights |
| `run_condor_sim.sh`, `CondorRunSim.sh`, `RunCondorSim.sub` | Runs the histmaker on condor, 20 tree files per job |
| `hadd.sh` | Merges the condor outputs per sample |
| `Jet_resp_histcombiner.C` | Combines the samples into `output_combined.root` |
| `Jet_resp_histfitter.C` | Gaussian fits of the response per bin -> `output_gaus/` |
| `Jet_resp_slicefitter.C` | Smooths the first-pass response map in pT and EMfrac |
| `Jet_resp_interpolation.C` | Fine interpolation and inversion -> `output_final_r0X_z0.root` |
| `run_condor_fit.sh`, `CondorRunFit.sh`, `RunCondorFit.sub` | Histfitter over all (R, z, eta) bins on condor, one job per (R, z) |
| `do_analysis.sh` | The same fits run locally, without condor |
| `Jet_resp_zeta_interpolation.C` | Residual (z, f) correction and no-vertex function -> `JES_Calibration_r0X.root` |
| `make_final_calibration.C` | Collects all radii -> `JES_Calibration_Final.root` |
| `draw_factorization.C`, `draw_fullclosure.C`, `draw_jertest.C` | Closure figures |

## Running

Run everything from this folder. Create the figure folder once, since the fitters don't create it:
```bash
mkdir -p figure_gaus
chmod +x *.sh
```
`run_condor_sim.sh` and `run_condor_fit.sh` submit to condor themselves. Wait until all jobs are done
(`condor_q`) before the next command.

### 1. First pass (Run 28)
```bash
./set_config.sh firstpass
./run_condor_sim.sh 28 0                 # closure_mode 0 = first pass; wait for condor
./hadd.sh 28
root -l -q -b 'Jet_resp_histcombiner.C(false,28)'
for R in 2 3 4 5 6 7 8; do
  root -l -q -b "Jet_resp_histfitter.C($R,0,0)"
  root -l -q -b "Jet_resp_slicefitter.C($R,0)"
  root -l -q -b "Jet_resp_interpolation.C($R,0)"
  cp output_final_r0${R}_z0.root calib_r0${R}.root
done
```
`calib_r0X.root` holds `h2_jes_r0X_z0`, the calibrated pT vs (reco pT, EMfrac).

### 2. Residual (z, f) correction (Run 38)
This step needs all `calib_r0X.root` from step 1. The binning is 11 signed z bins from -200 to +200 cm,
plus a no-vertex bin.
```bash
./set_config.sh closure
./run_condor_sim.sh 38 1                 # closure_mode 1 = first pass applied; wait for condor
./hadd.sh 38
root -l -q -b 'Jet_resp_histcombiner.C(true,38)'
cp output_combined.root output_combined_closure1.root
./run_condor_fit.sh                      # 7 radii x 12 z bins = 84 jobs; wait for condor
for R in 2 3 4 5 6 7 8; do
  root -l -q -b "Jet_resp_zeta_interpolation.C($R)"
done
root -l -q -b 'make_final_calibration.C()'
for R in 2 3 4 5 6 7 8; do
  root -l -q -b "draw_factorization.C($R)"   # before step 3 overwrites output_gaus/
done
```

### 3. Full-chain closure (Run 28)
This step needs `calib_r0X.root` and `JES_Calibration_r0X.root`, and the closure config from step 2.
```bash
./run_condor_sim.sh 28 2                 # closure_mode 2 = first pass x residual correction; wait for condor
./hadd.sh 28
root -l -q -b 'Jet_resp_histcombiner.C(true,28)'
cp output_combined.root output_combined_closure2.root
./run_condor_fit.sh                      # wait for condor
for R in 2 3 4 5 6 7 8; do
  root -l -q -b "draw_fullclosure.C($R)"
  root -l -q -b "draw_jertest.C($R)"
done
```
Figures: `figure_gaus/` (fits and smoothing) and `figure_dependence/` (z / f dependence, closure, JER).

## Output and how to apply it

`JES_Calibration_Final.root` holds three objects for each radius X:
- `h2d_jes_calib_r0X`: the calibrated pT vs (reco pT, EMfrac);
- `h2_zeta_corr_r0X`: the residual factor vs (signed z, f);
- `f_zeta_noz_corr_r0X`: the residual factor vs f for events without a vertex.

```
pT1 = h2d_jes_calib_r0X->Interpolate(pT, EMfrac)
f   = (eta - eta_min(z)) / (eta_max(z) - eta_min(z))   # get_eta_acceptance() in Jet_resp_histmaker.C, signed z
pT2 = pT1 * h2_zeta_corr_r0X at (z, f)                  # z clamped to the histogram range
# events without a vertex: pT2 = pT1 * f_zeta_noz_corr_r0X(f), with f computed at z = 0
```
