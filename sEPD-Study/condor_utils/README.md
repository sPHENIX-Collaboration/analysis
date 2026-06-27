# Condor Utils

`condor_utils` is a package designed to automate the Fun4All Condor job submission procedure for sEPD studies. 
It replaces the old monolithic `utils.py` script with a clean, modular structure.

You can run any of the commands using the wrapper script at the root of the project:
```bash
python3 utils.py <command> [options]
```
Alternatively, use `./utils.py <command> [options]`.

---

## 1. Data Preparation (`data`)
This command fetches the latest physics run information from the sPHENIX DAQ database and prepares merged DST file lists.

**Usage:**
```bash
python3 utils.py data --tag <dst_tag> --output-dir <output_dir>
```
**What it does:**
- Queries `sphnxdaqdbreplica` for physics runs longer than 5 minutes.
- Uses `CreateDstList.pl` to find the corresponding `DST_CALOFITTING` and `DST_ZDC_RAW` files.
- Merges the lists using `merge_lists.sh` so they can be processed by downstream Condor jobs.

---

## 2. Quality Assurance (QA)
These commands generate Condor submissions for Fun4All QA tasks.

**Commands:**
- `trigger_qa`: Runs Trigger QA (`macros/Fun4All_TriggerQA.C`).
- `calo_qa`: Runs Calorimeter QA (`bin/Fun4All_CaloQA`).
- `centrality_qa`: Runs Centrality QA (`bin/Fun4All_CentralityQA`).

**Example Usage:**
```bash
python3 utils.py calo_qa -i files/run3auau/<list_file>.list -o scratch/calo_qa/ -n 0
```
*(Use `-n 0` to process all events).*

---

## 3. Fun4All Analysis (`f4a`)
These commands generate Condor jobs for the core Fun4All analysis.

**Commands:**
- `f4a`: Standard sEPD Fun4All job.
- `f4a_zdc`: ZDC-specific Fun4All job.
- `f4a_mc`: Monte Carlo analysis job.
- `f4a_data_mc`: Data/MC analysis job that utilizes Q Vector Calibrations.

**Example Usage (f4a_data_mc):**
```bash
python3 utils.py f4a_data_mc -i files/run3auau/runs.list --calib files/calibrations.txt -o scratch/f4a_data_mc/
```
**What it does:**
- Validates the required macros and directories (`src/`, `macros/`, etc.).
- Splits the input file lists into chunks (e.g., 4 DSTs per job).
- Matches runs with specific calibration configurations from the `--calib` file.
- Copies all dependencies to the output directory and generates a `jobs.list` and `.sub` file for Condor.

---

## 4. Jet Analysis (`jetAna`)
Runs the `Jet-Ana` binary over the TTree files produced by the `f4a` step.

**Usage:**
```bash
python3 utils.py jetAna -i lists/f4a_trees.list -i2 lists/f4a_qa.list -o scratch/jetAna/ --jet-pt-min 10
```
**What it does:**
- Groups input TTrees by run ID to ensure no mixing across runs.
- Splits the inputs into batches (default: 100 trees per job).
- Matches inputs against the corresponding QA outputs.
- Submits Condor jobs to run the `Jet-Ana` binary.

---

## 5. Merging Output (`hadd`)
A two-stage Condor procedure for hierarchically merging (`hadd`) the many output ROOT files produced by `f4a` or `jetAna`.

**Usage:**
```bash
python3 utils.py hadd -i scratch/f4a/output/ -o scratch/f4a_merged/
```
**What it does:**
- **Stage 1 (Partial):** Finds all ROOT files in the input directories and splits them into chunks (e.g., 10 files each). Condor merges these chunks into intermediate "partial" files.
- The script waits and monitors the partial files until Stage 1 completes.
- **Stage 2 (Final):** Discovers all partial files and submits a second wave of Condor jobs to merge them into a single final ROOT file per run.

---

## Common Arguments
Most commands (except `data` and `hadd`) share the following common arguments, handled by `condor_utils/cli.py`:

- `-i`, `--input-list`: The path to the input list of files.
- `-o`, `--output-dir`: Where to place the submission scripts, logs, and outputs.
- `-n`, `--events`: Number of events to process per job (`0` for all).
- `-n2`, `--dst-per-job`: Number of DST files to process per job.
- `-s`, `--memory`: Memory limit to request in Condor (e.g., `1.0` GB).
- `-f2`, `--src-dir`: Source code directory to compile/include.
- `-l`, `--condor-log-dir`: Temporary storage directory for Condor log files.

Use `python3 utils.py <command> -h` for a complete list of arguments available for each specific command!
