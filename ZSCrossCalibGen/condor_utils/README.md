# Fun4All CaloFittingQA Condor Utilities

Modular Condor job submission automator for [Fun4All_CaloFittingQA.C](file:///sphenix/u/anarde/sPHENIX/analysis-ZSCrossCalibGen/ZSCrossCalibGen/macros/Fun4All_CaloFittingQA.C) in `ZSCrossCalibGen`.

## Features
- **1 Job per Segment List**: Reads an input list (such as `files/dst_triggered.list`) where each line is a segment list file containing 19 SEB files (`seb00` to `seb18`).
- **File Retrieval via `getinputfiles.pl`**: Automatically downloads all files in the segment list using `getinputfiles.pl --verbose --filelist <list>` into Condor scratch.
- **Run & Segment Encoding**: Output ROOT QA histograms are explicitly named `HIST_CaloFittingQA_run<RUN>_seg<SEG>.root`.
- **Fault-Tolerant & GPFS Resilient**:
  - Validates all 19 downloaded files exist and are non-empty before starting ROOT.
  - Implements a 5-iteration retry loop when copying outputs back to avoid GPFS latency issues.
  - Logs any failure directly to `failures/failure-log.txt`.
- **Intelligent Resource & Retries**:
  - Dynamically builds stepped Condor memory retry policies (e.g., `request_memory = 2GB`, `retry_request_memory = 2.5GB, 3GB, ...` up to 6GB).
  - Ranks submit nodes (`sphnxuser01`–`08`) to minimize contention.

---

## Usage

You can run the tool either from `ZSCrossCalibGen/` or from the repository root:

```bash
# Dry run: prepare submission directory, stage dependencies, and generate submit files
python3 run_condor.py -i files/dst_triggered.list -o scratch/calo_fitting_qa -n 0

# Direct submit: stage files and immediately submit the cluster to Condor
python3 run_condor.py -i files/dst_triggered.list -o scratch/calo_fitting_qa -n 0 --submit
```

### Common CLI Options
- `-i, --input-list`: Input list of segment list files (default: `files/dst_triggered.list`).
- `-o, --output-dir`: Project scratch directory for logs and outputs (default: `scratch/calo_fitting_qa`).
- `-o2, --job-output-dir`: Alternate directory for job output files; creates a symlink in `output_dir/output` (default: `None`).
- `-n, --events`: Number of events to process per job (default: `0` = all).
- `-i2, --dbtag`: CDB tag (default: `newcdbtag`).
- `-s, --memory`: Initial RAM request in GB (default: `2.0`).
- `--retry-memory-step`: Memory step in GB on OOM eviction (default: `0.5`).
- `--retry-memory-max`: Memory ceiling in GB (default: `6.0`).
- `-l, --condor-log-dir`: Path for Condor log files (default: `/tmp/anarde/dump`).
- `--submit`: Submit jobs directly to Condor instead of generating files only.
