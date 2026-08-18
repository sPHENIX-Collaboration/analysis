# HERWIG (Nashville) vs STAR UE cross-check for sPHENIX

Truth-level Fun4All analysis that reproduces **Fig. 4 (top right)** of the
HERWIG7 RHIC tune paper (Qureshi et al., arXiv:2411.16897 / PRD 112, 116019):
mean charged-particle multiplicity density `<dNch/(deta dphi)>` in the
**Transverse region** vs leading-jet pT, compared to STAR,
**PRD 101, 052004 (2020)** (arXiv:1912.08187).

The selections are ported 1:1 from the Rivet reference implementation
`STAR_2019_I1771348` (github.com/star-bnl/star-pythia8-tune), which is what
both the Detroit (PYTHIA8) and Nashville/New Haven (HERWIG7) tune papers ran:

| item | selection |
|---|---|
| final-state particles | status == 1, pT > 0.2 GeV/c, \|eta\| < 1.0 |
| jet finding | anti-kT, R = 0.6, clustered from **all** such particles (charged + neutral) |
| leading jet | hardest jet with 5 < pT < 45 GeV/c and \|eta\| < 0.4 |
| Toward / Transverse / Away | \|dphi\| < pi/3  /  pi/3 < \|dphi\| < 2pi/3  /  \|dphi\| > 2pi/3 |
| density | Nch / (deta * dphi) = Nch / (2 x 2pi/3), per event, profiled vs leading-jet pT |
| jet-pT binning | {5, 7, 9, 11, 15, 20, 25, 35, 45} GeV/c (STAR HEPData) |

Also filled for free: Toward/Away densities, Transverse density for
pT > 0.5 GeV/c, and the `<pT_ch>` profiles (STAR Figs. 2-4 companions),
plus cutflow / leading-jet / dphi diagnostics.

## Build

```
cd src
mkdir -p build && cd build
../autogen.sh --prefix=$MYINSTALL
make -j4 install
```

(usual sPHENIX setup: `source /opt/sphenix/core/bin/sphenix_setup.sh -n` and
`sphenix_setup_myinstall` or add `$MYINSTALL` to `LD_LIBRARY_PATH` /
`ROOT_INCLUDE_PATH`.)

## Run

```
ls /path/to/herwig/DST*.root > herwig_dst.list
root -b -q 'macro/Fun4All_HerwigUE.C("herwig_dst.list","herwig_ue_check.root",0)'
root -l 'macro/PlotUETransverse.C("herwig_ue_check.root")'
```

`PlotUETransverse.C` has the STAR Transverse-region data points (values,
stat, asymmetric sys from HEPData d01-x01-y03) baked in and produces the
two-panel figure (distribution + MC/data ratio with the data uncertainty
band) plus a per-bin numerical table.

## What "sufficient" looks like

From the tune paper, the Nashville curve tracks the STAR Transverse density
within roughly the experimental systematics (~5-8%) across the full range,
with mild low-pT (5-10 GeV/c) and high-pT (>~30 GeV/c) deviations. If the
sPHENIX production reproduces MC/data ratios at that level, the UE modeling
in the production is behaving as published.

## Caveats to keep in mind

1. **pThat-binned / triggered productions.** The tune papers generated
   *minimum-bias inelastic* HERWIG. If the sPHENIX production has a hard
   pThat floor (e.g. a Jet10/Jet30-style sample), the leading-jet-pT bins
   near and below the floor are sculpted — only trust bins with leading-jet
   pT comfortably above pThat_min (rule of thumb: +5-10 GeV/c). If multiple
   weighted pThat bins are chained, the HepMC event weight is picked up
   automatically (`evt->weights().front()`), but check the normalization
   convention of the production.
2. **Particle-level decay settings.** The tune comparisons used HERWIG's
   generator-level stable particles. If the sPHENIX production changed the
   stable-particle lifetime threshold (e.g. decaying K0s/Lambda for Geant),
   secondary charged particles will inflate the UE density at the few-%
   level. Check which strange hadrons appear with status 1 in the DSTs.
3. **Neutrinos in jets.** The Rivet reference clusters a plain `FinalState`,
   i.e. neutrinos included; the module matches this by default
   (`set_include_neutrinos(false)` to flip). Effect is negligible.
4. **Crossing-angle boost.** If the production applied the sPHENIX beam
   crossing boost via PHHepMCGenHelper, eta/phi are shifted by O(mrad) —
   irrelevant here, but it's why this module reads the HepMC record rather
   than assuming a CM frame.
5. **Statistics.** STAR stat errors are per-mille; the comparison is
   systematics-limited (~5%). A few 10^5 accepted leading jets (see the
   `h_nevents` cutflow) gives MC stat errors well below that in all but the
   35-45 GeV/c bin.
