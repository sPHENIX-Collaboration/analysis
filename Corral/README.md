# Corral

**CORRelation AnaLysis.** Corral is a general-purpose code that produces correlation functions from
sPHENIX data. Both whole-acceptance correlations, e.g. R2(dy,dphi), and femtoscopic correlations, e.g.
C(Q), are produced. Any pair of reconstructed particles can be studied: the pair types are a simple table
in `src/PairTypes.h`, easy to edit. The code expects trees written by
[Collect](https://github.com/sPHENIX-Collaboration/analysis/tree/master/Collect), which provides the track,
V0, and calorimeter energy information used here. Charged tracks are identified as pi, K or p from their
dE/dx (the KFParticle gates in `dedxGates_KFP.root`). The code includes sensitive removal of the different
kinds of split and duplicate tracks in the Run-3 p+p (polyseeder) data -- like-sign and opposite-charge
split tracks, copies across bunch crossings and trigger frames, and loopers -- as well as a correction for
track crossing. `ana573/SplitTrackTypes_ana573.pdf` shows what each kind looks like, and
`ana573/SplitTrackLogic_ana573.pdf` every decision in processing order (3 pages each).

Correlations are measured by comparing **sibling** pairs (both particles from the same event) with
**mixed** pairs (particles from different events with a similar vertex position zvtx, NMIX=10 events per
zvtx bin, 16 bins over -16 < zvtx < 16 cm), which see the same acceptance but carry no physical
correlation.

The default `src/PairTypes.h` has 46 pair types (the lighter species first, a charged hadron before a V0):

| pairs | index: pair type |
|---|---|
| pi pi | 0 pi+pi-, 1 pi-pi+, 2 pi+pi+, 3 pi-pi- |
| K K | 4 K+K- (phi), 5 K+K+, 6 K-K- |
| p p | 7 p pbar, 8 p p, 9 pbar pbar |
| pi K | 10 pi+K-, 11 pi-K+, 12 pi+K+, 13 pi-K- |
| pi p | 14 pi+pbar, 15 pi-p (Lambda, Delta0), 16 pi+p (Delta++), 17 pi-pbar |
| K p | 18 K+pbar, 19 K-p, 20 K+p, 21 K-pbar |
| K Lambda | 22 K+Lambda, 23 K+Lbar, 24 K-Lambda, 25 K-Lbar |
| p Lambda | 26 p Lambda, 27 p Lbar, 28 pbar Lambda, 29 pbar Lbar |
| pi V0 | 30 pi+K0S (K*+), 31 pi-K0S (K*-), 32 pi+Lambda (Sigma*+), 33 pi-Lambda (Sigma*-, Xi-), 34 pi+Lbar, 35 pi-Lbar |
| K, p with K0S | 36 K+K0S, 37 K-K0S, 38 p K0S, 39 pbar K0S |
| V0 V0 | 40 K0S K0S, 41 Lambda Lambda, 42 Lbar Lbar, 43 K0S Lambda, 44 K0S Lbar, 45 Lambda Lbar |

The pair densities are filled in rapidity y (from the identified mass), inside abs(eta) < 1.1 and each species'
y window abs(y) < `Species_yu` (fluct_common.h): pi 1.0, K 0.7, p 0.6, K0s / Lambda / Lbar 0.7. Each pair type has
one y bin width (4th field of `PairTypes_Info`, PairTypes.h, units of 0.01) used for y1, y2 and dy; it must divide
both species' windows (each row lists its allowed values), and dy always has an odd number of bins with dy = 0 at a
bin centre. Each pair type gets as many bins as its data support (canary: empty mixed-event bins). Protons and antiprotons enter pairs only above pT = 0.4 GeV/c (beam-pipe
spallation protons). Tracks used by a V0 in its mass peak (+-3 sigma) stay out of every pair type.

## The physics plots

For every pair type, Corral makes:

- **Single-particle and pair multiplicities.** The number of particles per event and its moments. The
  sibling-to-mixed level of the correlation functions is set by these (a factorial-cumulant baseline),
  so they are shown next to the correlation functions.
- **R2 and C2 versus (y1, y2) and (dy, dphi).** R2 = rho2 / (rho1 rho1) - 1 is the normalized pair
  correlation, where rho2 is the sibling pair density per event and rho1 rho1 is the mixed pair density;
  C2 = rho2 - rho1 rho1 is the same excess in absolute units (pairs per event). The (dy, dphi) maps --
  rapidity difference versus azimuthal-angle difference -- show the near-side peak from jets and
  resonances around (0,0), and the away-side structure around dphi = 180 deg. Projections onto dy and
  onto dphi are shown too, and R2(dy) is also obtained independently from the (y1, y2) map (as sum rho2 /
  sum rho1 rho1 along each dy diagonal), as a check.
- **The crossing-corrected (dy, dphi) map** (charged-charged pairs), with the uncorrected map, the
  difference between the two, and the projections, so the size of the correction is visible (see
  "Crossing correction", below).
- **Femtoscopic C(Q).** The ratio of sibling to mixed pairs versus the invariant relative momentum Q, in
  fine (5 MeV) bins over 0-1.2 GeV and in bins of the pair transverse momentum kT. At small Q it shows
  the HBT enhancement (identical pions) and final-state interactions. Resonances that decay into the
  pair show up as peaks at a known Q; the plots carry signposts at those Q values (for example K0S and
  rho0 in pi+pi-, K*(892) in K0S pi, Sigma*(1385) and Xi in Lambda pi).
- **Quality-assurance pages.** Vertex, track (pt, eta, phi, TPC/MVTX/INTT hits, dE/dx, calorimeter
  energy) and V0 (mass, momentum, eta, phi, daughter dE/dx) distributions, and the tracks flagged as
  split-track candidates, drawn against the kept tracks. A **"Track cuts in force"** page lists every
  track cut with the vertex-phi mask drawn on the distributions it cuts, followed by the DCA of each
  species vs pT, (eta, phi) and phi; a **notch monitor** page follows the one known pair loss (below).

A single `corral` job writes all of these for its own events. For a large production, the data are
cut into chunks, one job runs per chunk, and **Finalize** combines them (see "Running"): it adds up the
chunks zvtx bin by zvtx bin, forms the final crossing-corrected, zvtx-averaged correlation functions,
takes the uncertainties from the chunk-to-chunk scatter, and writes the physics pages (four per pair
type: overview, C(Q), crossing correction, and the size of the correction versus time and detector
occupancy), followed by diagnostic pages.

## What Corral does to correct and check the correlations

Correlation functions are ratios of pair densities, so even a small population of tracks that come in
correlated copies, or a small pair inefficiency, makes order-one features in the narrow region where
the pairs land. Most of the work in Corral is about finding and removing these, with the same rules in
the sibling and mixed pairs. The defaults (no run-string needed) are the validated best effort; for
ana573 they were re-derived on 2026-09-27/29 (README_SplitTracks573.md, whose "Final state" section gives
what each step removes). Two charts in the repository draw them, and each item below points to its page:
`ana573/SplitTrackTypes_ana573.pdf`, pictures (page 1: like-sign split tracks and whole-event copies, panels
1-3; page 2: opposite-sign pairs, panels 4-6; page 3: the central-membrane hole, panels 7-9), and
`ana573/SplitTrackLogic_ana573.pdf`, the decisions in processing order (page 1, stages 0-2: events and
tracks; page 2, stage 3: pairs and the maps, and what is watched; page 3: the V0s and their daughters):

- **Vertex-phi mask (SiPhiMask), ana573.** Two vertex-phi windows, [70,83) and [94,115) deg, where the
  tracks are few and their DCA is large (a feature fixed at the silicon radius, for this data only), are
  left out: 4.4% of the tracks. `nosiphimask` turns it off; `siphimaskall` adds six small windows.
  Charts: SplitTrackLogic page 1, stage 1.

- **Central-membrane mask (CM mask), ana573.** Tracks that cross z = 0 inside the TPC (eta ~ asinh(-zvtx/R),
  R = 0.30-0.78 m) are often lost, and how often changes from event to event; same-event pairs in that hole
  are then enhanced over mixed ones, which made a peak at y1 = y2 ~ 0 in R2(y1,y2). Tracks with
  w = (eta + 0.018 z_c) sign(-z_c) in [-0.07,0.10) are left out, z_c = the centre of the event's 2 cm zvtx
  slice (a mask that follows each event's own zvtx does not cancel in S/M): 2.8% of the tracks.
  `nocmmask` turns it off; `cmmaskAABB` sets the window [-0.AA,0.BB) (README_SplitTracks573.md sec 38).
  Charts: SplitTrackTypes page 3, panels 7-9; SplitTrackLogic page 1, stage 1.

- **Like-sign split-track removal.** A single real particle is sometimes reconstructed as two tracks
  that share the same silicon (MVTX/INTT) seed. Before any pairs are formed, every like-sign pair of
  tracks in an event is tested. Candidates are pre-selected by a small ellipse in (deta, dphi) that is
  centred on where split fragments land: two fragments that share the silicon hits but were fitted with
  different curvatures are separated in dphi by the azimuth difference they build up by the outer
  silicon (R = 6 cm), which grows with abs(1/pT1 - 1/pT2); its dphi half-width follows the measured
  spread of the fragments (max(2 deg, 3 sigma)). Inside it, `SiSplitScore` (the fraction of
  shared silicon cluster keys, MVTX-led) together with a STAR-style **Splitting Level** SL (from the TPC
  pad-row hit patterns) catches "duplicate" splits, a **RadialGap** test catches "complementary" splits
  where the TPC clusters are divided between an inner and an outer piece, and a pair whose two tracks
  carry the same cluster on at least two MVTX layers is flagged whatever its SL (two particles from one
  vertex cannot share two MVTX clusters unless their momenta are nearly equal). The extra copy is
  removed from the event.
  Charts: SplitTrackTypes page 1, panels 1-2; SplitTrackLogic page 1, stage 2, same charge (paths 1-3).
- **Opposite-charge split-track veto.** The same silicon seed can also be attached to two TPC tracks of
  opposite charge. These are removed by a track-level veto on the shared silicon clusters.
  Charts: SplitTrackTypes page 2, panel 4; SplitTrackLogic page 1, stage 2, opposite charge.
- **V0 daughters.** A V0 inside its mass peak (+-3 sigma) claims its two daughters: they leave every track
  pair type and are never the loser of a split-track test. For Lambda and anti-Lambda only (not K0S), an
  accepted track that is a split copy of a daughter is removed: like-sign within dR < 0.05 (except a
  pion-tagged partner of a p/pbar), opposite-sign by shared silicon, where a proton-tagged partner of the
  (anti)proton daughter is its wrong-sign copy. `nodau` turns this off.
  Charts: SplitTrackLogic page 3.
- **Looper veto.** A track with pT below ~0.17 GeV/c curls back inside the TPC, and its returning half,
  read outward, is a second track of the opposite charge, exactly back to back with the same momentum.
  Opposite-charge pairs with both |p| < 0.20 GeV/c, dphi >= 170 deg and a relative momentum sum
  |p1 + p2| / (|p1| + |p2|) < 0.08 are one particle: the worse half is removed (0.01% of the tracks, 0.08%
  of those at pT 0.1-0.2 GeV/c). Without it the pi+pi- away side has a spike at dy ~ 0.
  Charts: SplitTrackTypes page 2, panel 5; SplitTrackLogic page 1, stage 2, opposite charge.
- **Cross-crossing duplicate cleaner.** sPHENIX reads out continuously; one trigger frame (TF) holds
  several bunch crossings, and each crossing with a vertex is one event. One silicon seed can be
  attached to TPC tracks in different crossings of the same TF, so the "same" particle appears in two
  events -- and then a mixed pair made of the two copies is correlated. The cleaner finds tracks in
  different events of one TF that share silicon clusters (cluster keys are unique within one TF) and
  keeps the copy whose own INTT time bucket matches its event's crossing; if neither matches, both are
  dropped (strict mode, the default); remaining ties go to the better fit.
  Charts: SplitTrackTypes page 2, panel 4; SplitTrackLogic page 1, stage 0 (b).
- **No mixing with adjacent trigger frames.** A smaller residual of the same effect comes from events in
  neighbouring TFs, where cluster keys cannot be compared. Whole mixed event pairs from adjacent TFs of
  the same run are therefore skipped (2-10% of the mixed event pairs, depending on the zvtx bin). The mixed
  normalization is kept per zvtx bin, so the exclusion does not bias it.
  Charts: SplitTrackTypes page 2, panel 4; SplitTrackLogic page 2, stage 3.
- **Collisions copied into overlapping trigger frames.** When two triggers are closer in time than a
  TF's readout window, both TFs reconstruct the collisions in the overlap: the same vertex appears in two
  events of different TFs, with the crossing shifted by exactly the difference of the two TFs' GL1 BCO.
  The cluster keys cannot be compared across TFs, so the cleaner above cannot see these. With the
  Collect `bco` branch, `bco + crossing` is the absolute crossing, and (run, `bco + crossing`) identifies
  the bunch crossing: the first event with a given value is kept, and later copies are skipped entirely
  (sibling and mixed). In ana573 about 13% of events are such copies, most in the next TF but some two
  TFs later (which the adjacent-TF mixing exclusion does not reach). Trees without `bco` (ana532 and
  older) are processed exactly as before; the log says the removal was not possible.
  Charts: SplitTrackTypes page 1, panel 3; SplitTrackLogic page 1, stage 0 (a).
- **Two-track resolution cut.** Two like-sign tracks that stay close through the TPC are lost to
  merging. Like-sign pairs with abs(dy) < 0.06 whose minimum azimuthal separation anywhere in the TPC
  (dphi* over R = 0.30-0.78 m) is below 2 deg are removed from sibling and mixed pairs alike, so the
  loss cancels in the ratio. It is visible mostly in C(Q) at Q < ~20 MeV.
  Charts: SplitTrackLogic page 2, stage 3.
- **Near-vertex two-track cut.** Sibling tracks at nearly the same phi near the vertex are lost; charged
  pairs with abs(dphi*(R = 3 cm)) < 2 deg at abs(dy) < 1 are removed from sibling and mixed pairs alike
  (0.9% of the pion pairs). The log prints the fraction of pairs each pair cut removes.
  Charts: SplitTrackLogic page 2, stage 3.
- **Watched, not removed: the +-15 deg notch.** Opposite-charge tracks bend toward each other; a pair
  whose two tracks meet in phi between the INTT and the TPC (R ~ 0.10-0.30 m) is lost in the silicon-TPC
  matching, which leaves a ~4% deficit of pi+pi- pairs at 10-20 deg in dphi (README_SplitTracks573.md secs
  32-34). It is upstream of Corral; the notch monitor page shows it in every production.
  Charts: SplitTrackTypes page 2, panel 6; SplitTrackLogic page 2, WATCH.
- **Crossing correction.** When two real tracks cross in the TPC, one can lose hits to the other and
  fail the hit requirement: a pair inefficiency that a single-track efficiency cannot see. Pairs are
  pt-ordered when filled, so that every instance of this loss falls on the same ("dirty") side of dphi
  for a given magnetic-field sign. After the correlation functions are formed, each dirty near-side bin
  is replaced by its mirror bin from the clean side, and the away side is symmetrized in dphi. Only dphi
  is ever symmetrized; dy stays y1 - y2. Pairs with a neutral V0 leg are not pt-ordered and are left
  untouched.
  Charts: SplitTrackLogic page 2, stage 3 (the maps).
- **V0 zero-value repair.** Some V0 getters return exactly 0 for eta, phi, mass or decay length when the
  value was not filled; Corral recomputes these from the V0 momentum and decay geometry instead of
  using the zero.
- **Empty-denominator protection and zvtx averaging.** A bin is empty only if its mixed density is zero
  (a zero sibling count is a valid measurement). Correlation functions are formed per zvtx bin and then
  averaged over the zvtx bins that are valid for that bin -- the mixed density is non-zero and enough
  pairs are expected there (a minimum per pair type, in `src/PairTypes.h`).
- **Uncertainties from subgroups.** Finalize runs every chunk through the same chain, with the
  full-statistics zvtx weights and masks, and takes the error from the chunk-to-chunk scatter. It also
  fits the per-chunk values versus time and versus occupancy, to catch any drift.
- **Built-in cross-checks.** pi- pi+ is the pi+ pi- system with the species order swapped (dy -> -dy),
  a check of the sign and crossing conventions; R2(dy) is computed both
  from the (dy, dphi) map and from the (y1, y2) map; Finalize compares every histogram with a
  single-job full-statistics reference and reports the pulls; and the logs count every removed track,
  vetoed pair and skipped mixed pair.

## Building

Needs ROOT 6 (tested with 6.34) with its CMake configuration available (e.g. after
`source thisroot.sh`), CMake >= 3.16, a C++17 compiler, and `ps2pdf` (Ghostscript) for the PDF output.

```sh
cmake -S Corral -B Corral/build
make -C Corral/build
```

This builds the executable `corral` in the Corral directory (not in `build/`). The pair types are set in
`src/PairTypes.h` (number and order change now and then; a pions-only study edits that table, it gets no
separate binary). Until 2026-09-27 the executable and its
output files were called `corral_m`; files made before then keep that name. Finalize knows which datasets
use it (`CorralFilePrefix` in `src/finalize_hists.h`: ana532 uses `corral_m`, later datasets use `corral`).

## Running

`corral` writes `root/<name>.root` and `pdf/<name>.pdf` below the directory it is run in, so run it in a
work directory that has `root/` and `pdf/` subdirectories. The default name is `corral`.

```sh
mkdir -p work/root work/pdf && cd work
/path/to/Corral/corral -n 200000 -l /path/to/list.txt   # 200k events from the files in list.txt
```

### Options

| option | meaning |
|---|---|
| `-h` | help: all options and run-string tokens, with the current defaults |
| `-n N` | process N events (tree entries; one entry = one crossing with a vertex); 0 = all |
| `-l file` | read the Collect files listed in `file`, one path per line |
| `-d dataset` | dataset for Finalize (default: `FINALIZE_SET` in `src/finalize_hists.h`) |
| `-o name` | output base name: `root/name.root`, `pdf/name.pdf` (not scanned for run-string tokens) |
| `-s string` | run string: appended to the output name (`corral_<string>`) and scanned for the tokens below |

Without `-l`, all files matching the environment variable `CORRAL_TREES` are read in one job
(for example `export CORRAL_TREES='/path/to/production/outputCollect_*.root'`, quoted). This is
useful for tests; if it is not set, a site default (the WSU copy of the production) is used, and the log
says which glob was used and how many files matched.

### Run-string tokens

The defaults need no run string. Tokens either turn a mechanism off (`no...`) or change an important cut
value; they are matched as substrings, case-insensitively. `NN` is two digits, read as 0.NN. A value outside
its sane range (given in the table) stops the job with a message; it is never clamped silently. Cuts that
are not listed are fixed constants (see README_SplitTracks573.md sec 18 for the like-sign split-track ones).

| token | effect |
|---|---|
| **like-sign split tracks** | |
| `noLS` | turn off the whole like-sign split-track removal |
| `noRG` | turn off only the RadialGap path (the SL/SiSplitScore path stays on) |
| `removecutNN` | SL threshold 0.NN (default 0.18; sane range 0.05-0.60) |
| `skfNN` | SiSplitScore gate 0.NN (default 0.10; 0.05-0.95) |
| `radialgapNN` | RadialGap threshold 0.NN (default 0.45; 0.20-0.90) |
| `noMVTX` | turn off the third LS path (pairs with >= 2 matched MVTX layers, whatever the SL) |
| `nolooper` | turn off the looper veto |
| `looperNN` | looper veto relative momentum sum below 0.NN (default 0.08; 0.02-0.10) |
| `dpsNN` | pregate half-width: the pregate is centred on the split peak, (deta/0.040)^2 + ((abs(dphi) - dphi0)/w)^2 < 1, dphi0 = the azimuth difference the two fragments build up by R = 6 cm, w = NN/10 deg (default `dps20`; 1.0-4.0 deg). The third LS path (>= 2 matched MVTX layers) has a fixed threshold; `noMVTX` turns it off. |
| `tsepmYYPP` | two-track-resolution LS pair cut, sibling and mixed alike: reject abs(dy) < 0.YY and min over R = 0.30-0.78 m of abs(dphi*) < P.P deg (default `tsepm0620`; dy 0.02-0.10, dphi* 1.0-4.0) |
| `notsep` | turn off the two-track pair cut (`noLS` does not: it is a pair cut, not a split-track path) |
| **near-vertex two-track loss (LS and ULS)** | |
| `isepPPP` | near-vertex two-track pair cut, sibling and mixed alike, LS and ULS: reject abs(dy) < 1.0 and abs(dphi*(R = 3 cm)) < P.P deg (default `isep020` = 2.0 deg; 0.3-4.0). Sibling tracks at nearly the same phi near the vertex are lost out to abs(dy) ~ 0.8 (README_SplitTracks573.md sec 28-29); the PDF page "Near-vertex two-track loss" draws the cut on the loss. |
| `noisep` | turn off the near-vertex two-track cut |
| **PID and y windows (README_PID.md)** | |
| `oldpid` | legacy PID: pi = dedx70s < 400 at any momentum, no p or K, split-track candidates = those pions (default: KFP dE/dx gates on dedxKFP, pi then p then K, all accepted tracks are split-track candidates) |
| `nosiphimask` | turn off the ana573 vertex-phi mask (default: the two windows [70,83) and [94,115) deg) |
| `siphimaskall` | use all eight vertex-phi mask windows (the two above and six small ones; a study option) |
| `nocmmask` | turn off the ana573 central-membrane mask (default: w in [-0.07,0.10), see above) |
| `cmmaskAABB` | the central-membrane mask window w in [-0.AA,0.BB) (each 0.00-0.20) |
| `nchLLHH` | multiplicity class: only events with LL <= N_ch <= HH (accepted tracks, `hntrk`) reach CalcRm, so siblings, mixing pools and hmult (r2) stay inside the class (default all events; LL 0-40, HH LL-99; e.g. `nch0607`). The loop's QA pages still see every event. |
| **opposite-charge split tracks** | |
| `noulstest` | turn off the opposite-charge veto |
| `ulstestNN` | opposite-charge veto at SiSplitScore 0.NN (default 0.05; 0.02-0.95) |
| **V0-daughter split partners** | |
| `nodau` | turn off the split-partner check of Lambda/Lbar daughters (see "V0 daughters" above) |
| **cross-crossing duplicates and mixing** | |
| `noXTF` | turn off the duplicate cleaner and the adjacent-TF mixing exclusion |
| `XTFclean` | cleaner non-strict (if neither copy matches the INTT timing, keep the better one; strict is the default) |
| `mixAdjTF` | allow mixing with adjacent TFs |
| `noTFdup` | keep collisions copied into overlapping TFs (default: skip the later copies; needs the Collect `bco` branch, no effect without it) |
| **event selections (diagnostics; they cost statistics)** | |
| `onlyfirsttf` | keep only the first event of each TF |
| `Xing0` | keep only crossing-0 (triggered) events |
| `XingPos` | keep only the first event with crossing > 0 of each TF |
| **other** | |
| `nocross` | turn off the crossing correction |
| `ntpcNN` | track cut: at least NN TPC clusters (default 18; 10-40) |
| `qcut` | reject pairs with Q < 0.150 GeV |
| `548` | fine (dy, dphi) binning, 5 x 48 (default 32 x 36) |
| **acceptance box (studies)** | |
| `vzNN` | event abs(zvtx) < NN cm (default 16; 6-16). For physics the Zvtx range is chosen in Finalize (`fzNN`), not here. |

Watch for substring collisions when choosing a run string: `mixNoAdjTF` contains `NoAdjTF`,
`XTFcleanStrict` contains `XTFclean`, `noulstest` contains `ulstest`, `nolooper` contains `looper`, `nocmmask`
contains `cmmask`, and any word containing `nocross` turns off the crossing correction.

`-s Finalize` is special: instead of processing events, it runs Finalize (below). `-s Finalize_<tokens>`
also runs Finalize; its token `fzNN` sets the Zvtx range for physics: Finalize averages only the 2 cm Zvtx
slices inside abs(zvtx) < NN cm (default **8**; 2-16). The chunks keep every slice (abs(zvtx) < 16), so the
choice can be changed without rerunning. R2 per Zvtx slice is flat only at abs(zvtx) < ~6-8 cm
(README_SplitTracks573.md sec 30); the output names record the tokens (`corral_Finalize_fz16.*`).
A bin-zvtx enters the Zvtx average only if N_exp = nevt rho2(M) >= N_min (PairTypes.h; env `FINALIZE_NEXPMIN`)
and rho2(M) >= 0.3 x the bin's largest rho2(M) over the slices (partial-acceptance edges such as the
central-membrane mask; env `FINALIZE_MFRAC`, 0 = off; README_SplitTracks573.md sec 38). The log counts both.

### A full production

Everything belonging to one dataset (for example `ana532`) uses the dataset name as its folder, see
"Data layout" below.

1. **Make job lists** from a Collect production directory, about N events per list:
   ```sh
   Corral/lists/make_lists.bash /path/to/collect_dir 1000000 <dataset>
   ```
   This writes `lists/<dataset>/` with `list_NN.txt`, `list_all.txt` (all lists together), `manifest.txt`,
   `lists_summary.txt` and `segment_entries.txt`. Each list holds contiguous segments of one run, so the
   chunks follow the data-taking time order. `<dataset>` defaults to the name of the Collect directory.
2. **Run one job per list** as a SLURM array (the partition and time limit in the scripts are set for the
   WSU grid; adjust them for your site):
   ```sh
   Corral/run_m_array.bash <dataset> [array-range]
   ```
   The binary is snapshotted first, so every task runs the same code. The run scripts keep jobs off the
   hax* and rpr* nodes by default (`EXCLUDE="..."` replaces the list, `EXCLUDE=` allows every node). Outputs go to
   `root/<dataset>/chunks/corral_NN.root`, and the same under `pdf/` and `log/`.
   A **variant** of the dataset (the same lists with a run string) is
   `CORRAL_VARIANT=<name> CORRAL_RS=<runstring> Corral/run_m_array.bash <dataset>` (and the same for
   `run_m.bash`): outputs go under `<dataset>_<name>/`, and `lists/<dataset>_<name>` is made as a link to the
   dataset's lists, so Finalize runs with `-d <dataset>_<name>`. For example, everything off:
   `CORRAL_VARIANT=raw CORRAL_RS=noLS_noulstest_noXTF_noTFdup_nolooper_notsep_noisep_nocross`.
3. **Run the single full-statistics job**, which reads `list_all.txt` and is the reference Finalize
   compares against:
   ```sh
   Corral/run_m.bash <dataset>
   ```
   Output: `root/<dataset>/corral.root`, `pdf/<dataset>/corral.pdf`, `log/<dataset>/corral.log`. It asks for
   16 GB and 7 days (`MEM=`, `TIME=` override). With 28 pair types a job took about 2 h 15 min and
   3 GB per 50M events (ana573, 642M events: ~29 h, MEM=32G); with 46 a 10M-event chunk takes ~1.5 h.
   This job also snapshots its binary.
4. **Finalize**, in a work directory with `root/` and `pdf/`:
   ```sh
   /path/to/Corral/corral -d <dataset> -s Finalize          # abs(zvtx) < 8 cm for the physics (fz08)
   /path/to/Corral/corral -d <dataset> -s Finalize_fz16     # the whole zvtx range
   ```
   It reads `lists/<dataset>/`, `root/<dataset>/chunks/` and `root/<dataset>/corral.root`, and writes
   `root/corral_Finalize.root` and `pdf/corral_Finalize.pdf` in the work directory; copy them to
   `root/<dataset>/` and `pdf/<dataset>/`.
   Without `corral.root` (the single job still running) it skips only the pages that compare with it. The
   crossing correction follows the chunks: a `nocross` production stays uncorrected in Finalize too.

### Where things are looked for

The project directory (holding `lists/` and `root/` for Finalize, and `dedxGates_KFP.root` for the PID)
defaults to the source directory `corral` was built from. Set `CORRAL_DIR` to use another one. The run scripts find their own
directory, or use `CORRAL_DIR` if it is set, and pass it on to the SLURM jobs.

## Data layout

Outputs are kept per dataset, so that productions never mix:

```
lists/<dataset>/                    job lists (made by lists/make_lists.bash)
root/<dataset>/corral.root        single full-statistics job (the Finalize reference)
root/<dataset>/corral_Finalize.root
root/<dataset>/chunks/              one file per job list
root/<dataset>_<variant>/           the same for a variant (run_m*.bash CORRAL_VARIANT=; lists/<dataset>_<variant> -> <dataset>)
DATASET_<dataset>.md                what the dataset is: trees, events, runs, job IDs (in the Corral
                                    directory itself, so it survives when a superseded dataset is deleted)
pdf/<dataset>/, log/<dataset>/      the same structure
root/Development/, pdf/Development/, log/Development/   study and test outputs
```

The input trees at WSU are in `/rs/rs_grp_rhi/sPHENIX/<dataset>/`. None of these output folders are in
the repository.

## Code layout

| file | contents |
|---|---|
| `src/corral_main.cxx` | command line, help text |
| `src/corral_utils.cxx` | input chain, run-string parsing, defaults |
| `src/corral_loop.cxx` | the event loop: track and V0 selection, split-track and duplicate removal, QA histograms, PDF pages |
| `src/CalcRm.cxx`, `.h` | pair filling (sibling and mixed), correlation functions per zvtx bin, C(Q) |
| `src/corral_finalize.cxx`, `finalize_hists.h` | Finalize and its configuration |
| `src/CrossingCorrect.h`, `ZvtxAverage.h` | crossing correction and zvtx averaging, shared by CalcRm and Finalize |
| `src/PairTypes.h` | the pair-type table and the per-type minimum expected pairs |
| `src/fluct_common.h`, `corral_class.h` | shared definitions (species, PID caps and pT minima, the SiPhiMask windows, the V0 mass peaks); `NOCORRELATIONS` in `corral_class.h` skips all pair work (fast QA runs) |
| `dedxGates_KFP.root` | the KFParticle dE/dx gates (lower/upper edge vs momentum for pi, K, p, d), read at run time |
| `ana573/SplitTrackTypes_ana573.pdf` | pictures of each split / duplicate case the ana573 default removes, the notch, and the central-membrane hole with its mask (3 pages) |
| `ana573/SplitTrackLogic_ana573.pdf` | flowchart of every removal decision in processing order, with the fractions removed (3 pages) |

## The name

- A corral is the pen where cattle or horses are rounded up and held before you work with them. That is
  what the code does: it rounds up the particles of each event into species (pions, K0S, Lambda, ...) and
  pairs them.
- The event-mixing buffer is literally a holding pen: NMIX events are kept corralled together until the
  mixed pairs are formed.
- **CORR**elation an**AL**ysis -> C-O-R-R-A-L.
