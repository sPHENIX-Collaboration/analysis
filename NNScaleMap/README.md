# NNScaleMap

A neural-network calibration of the track momentum scale from K0s->π+π- and Lambda->p+pi decays. It reads KFParticle nTuples, learns a correction in curvature as a function of daughter (pT, eta, phi, charge) that puts the reconstructed V0 mass peaks at their PDG values, and exports it as a lookup table. An optional second stage calibrates the momentum resolution.

**Correction convention.** In curvature,

    (1/pT)_corr = (1/pT) · (1 + epsilon + q * delta)

- epsilon(pT, eta, phi) is charge-even: a field-scale-like correction.
- delta(pT, eta, phi) is charge-odd: a sagitta-like correction.
- The exported quantity is the pT multiplier kappa = 1/(1 + epsilong + q * delta), so
  pT_corr = kappa * pT.
- kappa is always positive, so the track charge is never changed.

## Layout

```
NNScaleMap/
├── setup.sh                  Python environment (creates .venv on first use)
├── requirements.txt
├── training/
│   ├── skim.py               pT-stratified skim of the KFParticle nTuples
│   ├── train.py              stage 1 (momentum scale) and stage 2 (resolution) training
│   ├── scalemap2root.py      trained model -> TH2D kappa maps for ROOT plotting
│   ├── resample.py           even/odd halves and bootstrap replicas of a skim (hpo/, syst/)
│   ├── run_trial.sh          one train.py Condor job of the hpo/ or syst/ campaign
│   ├── hpo/                  hyperparameter scan: trial list, Condor job, held-out score
│   ├── syst/                 map uncertainty: variation list, Condor job, combination
│   ├── configs/              TOML run cards (skim and training)
│   ├── nnscalemap/           the package: data loading, models, losses, figures, export
│   └── results/              all training outputs (git-ignored): calib_out_*/, hpo/, syst/, logs
└── plotting/                 ROOT macros (kappa maps, mass closure, injection residuals)
```

## Setup

```bash
cd /sphenix/user/hjheng/sPHENIXRepo/analysis/NNScaleMap
source setup.sh
cd training
```

- **First use:** `setup.sh` creates `.venv` with the system Python 3.12 and installs `requirements.txt` (CPU PyTorch, uproot, numpy, ...).
- **Afterwards:** it just activates `.venv`.
- **Use a separate shell for the ROOT macros.** `setup.sh` unsets `PYTHONPATH` and `LD_LIBRARY_PATH` in the current shell, because the sPHENIX cvmfs Python breaks the venv. Run the macros in `plotting/` from a fresh shell with the usual sPHENIX environment.

## Workflow

### 1. Skim the nTuples (optional)

Training on the raw nTuples is impractical for two reasons:
- they are large (hundreds of GB);
- the pT spectrum falls steeply, so a uniform subsample has almost no high-pT candidates.

`skim.py` keeps:
- every candidate whose own (V0) pT is above `pt_threshold`;
- a random `bulk_fraction` of the rest.

It writes only the branches the training reads.

```bash
python skim.py --config configs/skim_V0_polyseeding_ana573.toml
python skim.py --config configs/skim_V0_polyseeding_ana573.toml --file <one input file>   # e.g. one Condor job per file
```

Output layout:

```
<skim_dir>/<production>/<tag>/{Kshort,Lambda0}/skimmed_<input name>.root   (TTree, same name as the source tree)
                                                skimmed_<input name>.toml   (read-only: card values, source path + sha256, kept counts)
```

`<tag>` encodes the selection, e.g. `ptcand1p0_bulk0p02_seed42_stage1`, so skims with different settings never overwrite each other. Every input file gets its own reproducible random draw, so rerunning a single file reproduces its output exactly. Files that cannot be read are reported at the end, and the skim exits non-zero; all other files are still written.

Skim card settings:

| Key | Meaning |
|---|---|
| `[input] production` | names the output subfolder |
| `[input] kshort`, `lam` | input files; glob patterns are allowed |
| `[input] tree` | TTree name |
| `[selection] pt_threshold` | GeV; every candidate whose V0 pT is above it is kept (default 1.0) |
| `[selection] bulk_fraction` | fraction kept below the threshold (default 0.02) |
| `[selection] seed` | random seed; combined with each file name |
| `[branches.stage1]` | branches written for stage-1 training |
| `[branches.stage2]` | extra branches for stage 2 (`pTErr`, `Covariance`) |
| `[branches] keep_stage2` | `true` also writes the stage-2 branches (needed for `--reso_model pull`) |
| `[output] skim_dir` | top-level output directory |

The skim checks before reading any file that the branch lists contain everything the training reader requires.

The selection does not bias the correction:
- κ is a function of each track's own (pT, eta, phi, q), so selecting candidates by pT does not change it.
- The selection uses the V0 pT, which treats the two daughters the same way. The K0s ⟨α⟩ and Λ/Λ̄ loss terms therefore keep their symmetry.

### 2. Train

```bash
python train.py --config configs/fullsample_20261010.toml
python train.py --config configs/fullsample_20261010.toml --epochs 800 --outdir results/trial_017   # flags override the card
nohup python train.py --config <card> > <name>.log 2>&1 &                                   # long runs
```

**Run card format:**
- Keys are the `train.py` option names (`python train.py --help`). Sections only organise the file.
- Flags given on the command line after `--config` override the card.
- A boolean set to `true` in the card cannot be switched off from the command line.
- `configs/example.toml` lists every option with its default; copy it to start a new card.

**Inputs:**
- `kshort` and `lam` take one or more files or glob patterns. All matching files are read and concatenated.
- `max_candidates > 0` subsamples each species. The budget is shared across files in proportion to their entries, and the reader takes the first candidates of each file, not a random draw over it.

**Stages:**
- **Stage 1** learns ε and δ.
- **Stage 2** learns the resolution with stage 1 frozen. `no_reso = true` skips it. Stage 2 with `reso_model = "pull"` needs the Covariance branches, i.e. a skim made with `keep_stage2 = true`.
- `inject = true` (or `inject_only`) runs the stage-2 toy recovery test.

### 3. Outputs (`<outdir>/`)

Every training writes under `training/results/`: the `--outdir` default is `results/calib_out` and the run cards set `results/calib_out_<tag>`. The directory is git-ignored.

| File | Content |
|---|---|
| `model.pt` | stage-1 network; `--load_stage1` reuses it |
| `kappa_lookup.csv` | kappa, epsilon, delta on a (q, pT, eta, phi) grid; the header records the convention and feature normalisation |
| `stage1_mass_histograms.root` | K0s, Lambda, anti-Lambda mass spectra before and after the correction |
| `stage1_training_history.csv`, `fig*.png/pdf` | training history and diagnostics |
| `reso_model.pt`, `reso_lookup.csv` | stage 2 (when run) |
| `injection/` | toy recovery test (when run) |
| `run_metadata.toml` | read-only: the command, every input file with its sha256, and every resolved option |

`run_metadata.toml` is written at the end of the run. It hashes all inputs, which takes about 3 s per GB.

### 4. Plot

Run these from `training/`, in the venv shell:

```bash
python scalemap2root.py --model <outdir>/model.pt --out <outdir>/kappa_maps.root
```

Then, in a fresh sPHENIX shell, from `plotting/` (the macros include `plotUtils.h` from there):

```bash
cd /sphenix/user/hjheng/sPHENIXRepo/analysis/NNScaleMap/plotting
root -l -b -q 'makeScaleMapPlot.C("../training/<outdir>/kappa_maps.root")'
root -l -b -q 'massClosure.C("../training/<outdir>/stage1_mass_histograms.root")'
```

`makeScaleMapPlot.C` writes its figures to `<outdir>/kappa_maps_plots/`.

`scalemap2root.py --hidden` and `--layers` must match the values used in training.

## Tuning the model

Stage-1 options (`[stage1]` in the card):

| Option | Default | What it controls |
|---|---|---|
| `feature` | `log_pt` | momentum input: `log_pt` (ln pT), `inv_pt` (1/pT) or `pt` |
| `epochs` | 800 | training epochs (full-batch Adam) |
| `lr` | 2e-3 | Adam learning rate |
| `hidden` | 48 | hidden-layer width (SiLU) |
| `layers` | 3 | number of hidden layers |
| `seed` | 17 | torch seed for the network initialisation |
| `dtype` | `float32` | precision of the network (`float32` is about 2.5× faster than `float64`); masses, loss sums and binning stay float64. float32 and float64 maps differ by much less than the seed-to-seed spread |
| `refresh` | 50 | epochs between refits of the mass peaks that set the signal weights |
| `lam_prior` | 0.05 | L2 pull of epsilon, delta toward zero at epoch 0, annealed linearly to 0 |
| `lam_split` | 1.0 | weight of the Lambda/anti-Lambda peak-split term, binned in leg-1 (pion) kinematics |
| `lam_split2` | 0.0 | weight of the same term binned in leg-2 (proton) kinematics; off in `example.toml` (no gain in the 2026-10-30 A/B test on fill 79514–79516) |
| `lam_alpha` | 2.0 | weight of the K0s Armenteros ⟨α⟩ symmetry term |
| `k0s_window`, `lam_window` | [0.42, 0.58], [1.09, 1.145] | GeV, mass windows of the peak fits that set the signal weights (inside the 0.40–0.60 and 1.08–1.15 preselection) |
| `bkg_order` | 1 | polynomial order of the background in those peak fits |
| `loss_pt_binning` | `quantile` | daughter-pT loss-bin edges: `fixed` (`loss_pt_edges`), `quantile` (equal population) or `log` (equal width in ln pT over the `lookup_pt_values` range); `example.toml` sets `fixed` |
| `loss_pt_bins` | 6 | number of daughter-pT loss bins (`quantile`, `log`) |
| `loss_pt_edges` | [0, 0.3, 0.5, 0.75, 1, 1.5, 2, 3, 4, 6, 100] | GeV, daughter-pT loss-bin edges (`fixed`); daughters outside them fall in the first or last bin |
| `loss_n_eta`, `loss_n_phi` | 10, 8 | eta and phi cells per daughter-pT loss bin (each cell is split by charge) |
| `loss_coarse_above_pt` | -1 | GeV, loss bins whose lower edge is at or above this use the coarse grid; negative = none; `example.toml` sets 2.0 |
| `loss_n_eta_coarse`, `loss_n_phi_coarse` | 5, 4 | eta and phi cells per coarse loss bin |
| `chunk_size` | 500000 | candidates per gradient chunk (memory only, see below) |

**The loss.** Each daughter pT bin is split into η, φ and charge cells. The loss is the signal-weighted mean pull² of the V0 mass per cell, plus the Lambda/anti-Lambda split terms (in pion and in proton kinematics), the K0s alpha term and the annealed prior. Cells with a signal weight of 5 or less do not enter.

**Choosing the loss binning:**
- With `quantile`, the top pT bin starts where the bulk of the population ends. Everything above it is constrained only on average.
- `loss_pt_binning = "log"` with 12 bins gives separate loss bins up to 5 GeV. The 2026-10-11 full-sample cards use this.
- `loss_pt_binning = "fixed"` with the `example.toml` edges gives separate loss bins up to 100 GeV for high-pT (heavy-flavour) daughters. Above `loss_coarse_above_pt` the η × φ grid is coarser, so that the sparse high-pT bins keep populated cells.
- At the start of training the log prints, per species, leg and pT bin, the populated cells out of all (η, φ, charge) cells. For K0s each leg has one charge, so at most half of its cells are populated.

**Lookup grid (`[output]`).** `lookup_pt_values` (default [0.1, 0.2, 0.3, 0.4, 0.5, 0.75, 1, 1.25, 1.5, 2, 2.5, 3, 4, 6, 100] GeV), `lookup_n_eta` (13) and `lookup_n_phi` (25) set the points of `kappa_lookup.csv`. The `lookup_pt_values` range also sets the κ-map diagnostics and widens the feature clamp to cover it, so κ is constant above the last point.

**Memory.** Training is full batch: every epoch uses the gradient of the loss over the whole sample. That gradient is accumulated over chunks of `chunk_size` candidates, so memory scales with `chunk_size`, not with the sample size. Results do not depend on it; it was validated against unchunked training to machine precision. Lower it if a job runs out of memory.

**Choosing `feature`.** The three choices carry the same information but give the network a different smoothness prior, and the numerical clamp on the normalised feature (±6 robust σ) cuts a different pT tail for each:
- `inv_pt` keeps high-pT tracks inside the clamp range;
- `pt` can clamp the high-pT tail. The clamp sits at x_ref +- 6 * x_scale, and the training log prints x_ref and x_scale in its `[norm]` line.

Differences between features in poorly populated regions are a modelling systematic.

Stage-2 options are in `[stage2]` (`reso_model`, `reso_epochs`, `reso_lr`, `reso_hidden`, `reso_layers`, `c_per_resonance`, `reso_scalar_steps`). The toy-test parameters are in `[injection]`.

## Hyperparameter scan and map uncertainty

Both campaigns run on fill 79514–79516 with its nominal card. Results go to `training/results/{hpo,syst}/fill79514to79516/` (trial directories, logs, summary CSVs, `figures/`); the resampled inputs, which `resample.py` regenerates exactly, stay next to the skims under `/sphenix/tg/tg01/hf/hjheng/HF-analysis/NNScaleMap/{hpo,syst}/fill79514to79516/inputs/`. Each job is one `train.py` run through `run_trial.sh`; the flags of a job override the card. The header of each Condor job file lists the one-time preparation (log directory, resampled inputs) and the output directory.

**Hyperparameter scan (`training/hpo/`).** Scans `hidden`, `layers` and `lr` around the nominal (800 epochs, float32). A variant replaces the nominal only if it improves the held-out mass term by more than twice the nominal seed-to-seed spread; the systematic campaign is then run around the chosen configuration.
- Every trial trains on the even half of the skim. `resample.py --halves` splits by entry parity, because the skims carry no event number.
- `trials.txt` changes one setting per trial. The nominal runs with five seeds, so the seed-to-seed spread is the yardstick for "better".
- `python hpo/score.py <campaign dir>` scores each trial on the odd half and writes `hpo_summary.csv`. The score is the stage-1 mass pull² in the loss cells and the K0s (alpha)^2 term, plus the fitted peak positions and widths. The same numbers on the even half give the overfitting gap.

**Map uncertainty (`training/syst/`).** `variations.txt` lists one job per line:
- `stat`: nominal settings on 20 bootstrap replicas (`resample.py --bootstrap 20`).
- `seed`: network-initialisation seeds.
- Modelling choices, one at a time: `feature`, loss binning, `lam_split`, `lam_alpha`, `lam_prior`, signal-weight fit windows, background order.

`python syst/combine.py <campaign dir>` compares every `kappa_lookup.csv` with the nominal one at each grid point:
- `stat` and `seed` contribute the spread of κ over their variations.
- Every other source contributes its largest |\delta kappa|.
- sigma_syst is the quadrature sum of all sources except `stat`.
- The output `kappa_lookup_unc.csv` is the nominal lookup plus sigma_stat, sigma_syst and one column per source. The script also prints how often each source exceeds σ_stat.

---
### Disclaimer and acknowledgement
This project is an effort to investigate the use of artificial intelligence, including but not limited to generative AI, to assist and accelerate the development of physics-object calibrations.

The overall design philosophy, underlying physics concepts, calibration strategies, and validation procedures are developed by humans. The implementation of these ideas, including software development and code generation, is assisted by generative AI tools, primarily Codex through the MIT generative AI platform Parley API and Claude Code.

All AI-generated code and related outputs are carefully reviewed, validated, and refined by humans before use. The resulting implementations are verified through a comprehensive set of closure tests using both simulated events and real collision data processed with the standard sPHENIX reconstruction chain. These validation studies are used to verify that the AI-assisted implementations reproduce the expected physics performance and meet the requirements for accuracy, reliability, and reproducibility.