# Corral

**CORRelation AnaLysis.** Corral is a general-purpose code that produces correlation functions from
sPHENIX data. Both whole-acceptance correlations, e.g. R2(dy,dphi), and femtoscopic correlations, e.g.
C(Q), are produced. Any pair of reconstructed particles can be studied: the pair types are a simple table
in `src/PairTypes.h`, easy to edit. The code expects trees written by
[Collect](https://github.com/sPHENIX-Collaboration/analysis/tree/master/Collect), which provides the track,
V0, and calorimeter energy information used here. The code includes sensitive removal of three different
kinds of split tracks in the Run-3 p+p (polyseeder) data, as well as a correction for track crossing.

Correlations are measured by comparing **sibling** pairs (both particles from the same event) with
**mixed** pairs (particles from different events with a similar vertex position zvtx, NMIX=10 events per
zvtx bin, 16 bins over -16 < zvtx < 16 cm), which see the same acceptance but carry no physical
correlation.

The default `src/PairTypes.h` has 16 pair types:

| index | pairs | index | pairs |
|---|---|---|---|
| 0 | pi+ pi- | 8 | K0S anti-Lambda |
| 1 | pi- pi+ (species order swapped: cross-check of 0) | 9 | Lambda anti-Lambda |
| 2 | pi+ pi+ | 10 | K0S pi+ |
| 3 | pi- pi- | 11 | K0S pi- |
| 4 | K0S K0S | 12 | Lambda pi+ |
| 5 | Lambda Lambda | 13 | Lambda pi- |
| 6 | anti-Lambda anti-Lambda | 14 | anti-Lambda pi+ |
| 7 | K0S Lambda | 15 | anti-Lambda pi- |

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
  onto dphi are shown too, and R2(dy) is also obtained independently from the (y1, y2) map, as a check.
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
  split-track candidates, drawn against the kept tracks.

A single `corral_m` job writes all of these for its own events. For a large production, the data are
cut into chunks, one job runs per chunk, and **Finalize** combines them (see "Running"): it adds up the
chunks zvtx bin by zvtx bin, forms the final crossing-corrected, zvtx-averaged correlation functions,
takes the uncertainties from the chunk-to-chunk scatter, and writes the physics pages (four per pair
type: overview, C(Q), crossing correction, and the size of the correction versus time and detector
occupancy), followed by diagnostic pages.

## What Corral does to correct and check the correlations

Correlation functions are ratios of pair densities, so even a small population of tracks that come in
correlated copies, or a small pair inefficiency, makes order-one features in the narrow region where
the pairs land. Most of the work in Corral is about finding and removing these, with the same rules in
the sibling and mixed pairs. The defaults (no run-string needed) are the validated best effort:

- **Like-sign split-track removal.** A single real particle is sometimes reconstructed as two tracks
  that share the same silicon (MVTX/INTT) seed. Before any pairs are formed, every like-sign pair of
  tracks in an event is tested: `SiSplitScore` (the fraction of shared silicon cluster keys, MVTX-led)
  together with a STAR-style **Splitting Level** SL (from the TPC pad-row hit patterns) catches
  "duplicate" splits, and a **RadialGap** test catches "complementary" splits where the TPC clusters
  are divided between an inner and an outer piece. The extra copy is removed from the event.
- **Opposite-charge split-track veto.** The same silicon seed can also be attached to two TPC tracks of
  opposite charge. These are removed by a track-level veto on the shared silicon clusters.
- **Cross-crossing duplicate cleaner.** sPHENIX reads out continuously; one trigger frame (TF) holds
  several bunch crossings, and each crossing with a vertex is one event. One silicon seed can be
  attached to TPC tracks in different crossings of the same TF, so the "same" particle appears in two
  events -- and then a mixed pair made of the two copies is correlated. The cleaner finds tracks in
  different events of one TF that share silicon clusters (cluster keys are unique within one TF) and
  keeps the copy whose own INTT time bucket matches its event's crossing; if neither matches, both are
  dropped (strict mode, the default); remaining ties go to the better fit.
- **No mixing with adjacent trigger frames.** A smaller residual of the same effect comes from events in
  neighbouring TFs, where cluster keys cannot be compared. Whole mixed event pairs from adjacent TFs of
  the same run are therefore skipped (about 9% of mixed event pairs). The mixed normalization is kept per
  zvtx bin, so the exclusion does not bias it.
- **Crossing correction.** When two real tracks cross in the TPC, one can lose hits to the other and
  fail the hit requirement: a pair inefficiency that a single-track efficiency cannot see. Pairs are
  pt-ordered when filled, so that every instance of this loss falls on the same ("dirty") side of dphi
  for a given magnetic-field sign. After the correlation functions are formed, each dirty near-side bin
  is replaced by its mirror bin from the clean side, and the away side is symmetrized in dphi. Only dphi
  is ever symmetrized; dy stays y1 - y2. Pairs with a neutral V0 leg are not pt-ordered and are left
  untouched.
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

This builds the single executable `corral_m` in the Corral directory (not in `build/`).

## Running

`corral_m` writes `root/<name>.root` and `pdf/<name>.pdf` below the directory it is run in, so run it in a
work directory that has `root/` and `pdf/` subdirectories. The default name is `corral_m`.

```sh
mkdir -p work/root work/pdf && cd work
/path/to/Corral/corral_m -n 200000 -l /path/to/list.txt   # 200k events from the files in list.txt
```

### Options

| option | meaning |
|---|---|
| `-h` | help: all options and run-string tokens, with the current defaults |
| `-n N` | process N events (tree entries; one entry = one crossing with a vertex); 0 = all |
| `-l file` | read the Collect files listed in `file`, one path per line |
| `-d dataset` | dataset for Finalize (default: `FINALIZE_SET` in `src/finalize_hists.h`) |
| `-o name` | output base name: `root/name.root`, `pdf/name.pdf` (not scanned for run-string tokens) |
| `-s string` | run string: appended to the output name (`corral_m_<string>`) and scanned for the tokens below |

Without `-l`, all files matching the environment variable `CORRAL_TREES` are read in one job
(for example `export CORRAL_TREES='/path/to/production/outputCollect_*.root'`, quoted). This is
useful for tests; if it is not set, a site default (the WSU copy of the production) is used, and the log
says which glob was used and how many files matched.

### Run-string tokens

The defaults need no run string. Tokens change them; they are matched as substrings, case-insensitively
unless marked [case]. `NN` is two digits, read as 0.NN.

| token | effect |
|---|---|
| **like-sign split tracks** | |
| `noLS` | turn off the whole like-sign split-track removal |
| `noRG` | turn off only the RadialGap path (the SL/SiSplitScore path stays on) |
| `removecutNN` [case] | SL threshold 0.NN (default 0.18); `removecut-NN` for a negative value |
| `skfNN` [case] | SiSplitScore gate 0.NN (default 0.10) |
| `radialgapNN` [case] | RadialGap threshold 0.NN (default 0.45) |
| **opposite-charge split tracks** | |
| `noulstest` | turn off the opposite-charge veto |
| `ulstestNN` | opposite-charge veto at SiSplitScore 0.NN (default 0.05) |
| **cross-crossing duplicates and mixing** | |
| `noXTF` | turn off the duplicate cleaner and the adjacent-TF mixing exclusion |
| `XTFclean` | cleaner on, non-strict (if neither copy matches the INTT timing, keep the better one) |
| `XTFcleanStrict` | cleaner on, strict (default) |
| `mixAdjTF` | allow mixing with adjacent TFs |
| `mixNoAdjTF` | no mixing with adjacent TFs (default; with `noXTF` also none within the same TF) |
| **event selections (diagnostics; they cost statistics)** | |
| `onlyfirsttf` | keep only the first event of each TF |
| `Xing0` | keep only crossing-0 (triggered) events |
| `XingPos` | keep only the first event with crossing > 0 of each TF |
| **other** | |
| `nocross` | turn off the crossing correction |
| `ntpcNN` [case] | track cut: at least NN TPC clusters (default 18) |
| `qcut` | reject pairs with Q < 0.150 GeV |
| `548` | fine (dy, dphi) binning, 5 x 48 (default 32 x 36) |

Watch for substring collisions when choosing a run string: `mixNoAdjTF` contains `NoAdjTF`,
`XTFcleanStrict` contains `XTFclean`, `noulstest` contains `ulstest`, and any word containing `nocross`
turns off the crossing correction.

`-s Finalize` is special: instead of processing events, it runs Finalize (below).

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
   The binary is snapshotted first, so every task runs the same code. Outputs go to
   `root/<dataset>/chunks/corral_m_NN.root`, and the same under `pdf/` and `log/`.
3. **Run the single full-statistics job**, which reads `list_all.txt` and is the reference Finalize
   compares against:
   ```sh
   Corral/run_m.bash <dataset>
   ```
   Output: `root/<dataset>/corral_m.root`, `pdf/<dataset>/corral_m.pdf`, `log/<dataset>/corral_m.log`.
4. **Finalize**, in a work directory with `root/` and `pdf/`:
   ```sh
   /path/to/Corral/corral_m -d <dataset> -s Finalize
   ```
   It reads `lists/<dataset>/`, `root/<dataset>/chunks/` and `root/<dataset>/corral_m.root`, and writes
   `root/corral_m_Finalize.root` and `pdf/corral_m_Finalize.pdf` in the work directory; copy them to
   `root/<dataset>/` and `pdf/<dataset>/`.

### Where things are looked for

The project directory (holding `lists/` and `root/` for Finalize) defaults to the source directory
`corral_m` was built from. Set `CORRAL_DIR` to use another one. The run scripts find their own
directory, or use `CORRAL_DIR` if it is set, and pass it on to the SLURM jobs.

## Data layout

Outputs are kept per dataset, so that productions never mix:

```
lists/<dataset>/                    job lists (made by lists/make_lists.bash)
root/<dataset>/corral_m.root        single full-statistics job (the Finalize reference)
root/<dataset>/corral_m_Finalize.root
root/<dataset>/chunks/              one file per job list
root/DATASET_<dataset>.md           what the dataset is: trees, events, runs, job IDs (kept outside
                                    root/<dataset>/, so it survives when a superseded dataset is deleted)
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
| `src/fluct_common.h`, `corral_class.h` | shared definitions; `NOCORRELATIONS` in `corral_class.h` skips all pair work (fast QA runs) |

## The name

- A corral is the pen where cattle or horses are rounded up and held before you work with them. That is
  what the code does: it rounds up the particles of each event into species (pions, K0S, Lambda, ...) and
  pairs them.
- The event-mixing buffer is literally a holding pen: NMIX events are kept corralled together until the
  mixed pairs are formed.
- **CORR**elation an**AL**ysis -> C-O-R-R-A-L.
