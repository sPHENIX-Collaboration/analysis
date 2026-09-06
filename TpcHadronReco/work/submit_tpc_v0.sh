#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
MODE="local"
INPUT_DIR=""
PATTERN="*.root"
OUTPUT_DIR=""
OUTPUT_BASE="TpcV0Candidates"
FILES_PER_JOB=1
NEVENTS=0
SKIP=0
QUEUE=""
MEMORY="2GB"
JOB_FLAVOUR=""
CONFIG_OUT=""
DRY_RUN=0

usage() {
  cat <<'USAGE'
Run one local job or submit many HTCondor jobs.

Required:
  --input-dir DIR
  --pattern GLOB
  --output-dir DIR
  --output-base NAME

Execution:
  --mode local|condor        Default: local
  --files-per-job N          Default: 1
  --nevents N                0 means all events
  --skip N                   Default: 0 (local mode only in normal use)
  --memory VALUE             Condor request_memory; default 4GB
  --queue REQUIREMENT        Optional Condor requirements expression
  --job-flavour NAME         Optional +JobFlavour value
  --dry-run                  Prepare files but do not run/submit

Main analysis options:
  --fit-mode kalman|helix|final
  --min-points N
  --pre-track-pt-min X
  --pre-track-dca-xy-min X
  --pre-pair-dca-max X
  --pair-pca-z-max X
  --pair-pca-dz-max X
  --pair-decay-r-min X
  --pair-alpha-max X
  --pair-dca-max X
  --pair-dira-min X
  --kshort-mass-min X
  --kshort-mass-max X
  --write-kshort-details BOOL
  --write-track-tree BOOL
  --write-cluster-residual-tree BOOL
  --write-kalman-diagnostics BOOL
  --write-same-sign BOOL

Fit uncertainty options [cm]:
  --r1-sigma-rphi X  --r1-sigma-r X
  --r2-sigma-rphi X  --r2-sigma-r X
  --r3-sigma-rphi X  --r3-sigma-r X
  --sigma-z X
  --transition-sigma-rphi X
  --transition-sigma-r X
  --transition-sigma-z X

Other:
  --beam-x X --beam-y X --beam-z X
  --cluster-node NAME --track-node NAME --vertex-node NAME
  --setup-script FILE        Script sourced inside each job
  --macro FILE               Fun4All macro path

Example local:
  ./submit_tpc_v0.sh --mode local \
    --input-dir /path/to/dst --pattern 'HITS_pp_79513_*.root' \
    --output-dir ./out --output-base v0_test --nevents 100

Example Condor:
  ./submit_tpc_v0.sh --mode condor \
    --input-dir /path/to/dst --pattern 'HITS_pp_79513_*.root' \
    --output-dir /path/to/out --output-base v0_79513 --files-per-job 2
USAGE
}

set_env() {
  local name=$1 value=$2
  printf -v "$name" '%s' "$value"
  export "$name"
}

# Defaults mirrored by the Fun4All macro.
set_env V0_MACRO "$SCRIPT_DIR/Fun4All_TpcV0CandidateTree.C"
set_env V0_SETUP_SCRIPT ""
set_env V0_FIT_MODE kalman
set_env V0_MIN_POINTS 5
set_env V0_PRE_TRACK_PT_MIN 0.20
set_env V0_PRE_TRACK_DCA_XY_MIN 0.03
set_env V0_PRE_PAIR_DCA_MAX 5.0
set_env V0_PAIR_PCA_Z_MAX -1.0
set_env V0_PAIR_PCA_DZ_MAX -1.0
set_env V0_PAIR_DECAY_R_MIN -1.0
set_env V0_PAIR_ALPHA_ABS_MAX -1.0
set_env V0_PAIR_DCA_MAX -1.0
set_env V0_PAIR_DIRA_MIN -2.0
set_env V0_KSHORT_DETAIL_MASS_MIN 0.45
set_env V0_KSHORT_DETAIL_MASS_MAX 0.55
set_env V0_WRITE_KSHORT_DETAILS 1
set_env V0_WRITE_TRACK_TREE 0
set_env V0_WRITE_CLUSTER_RESIDUAL_TREE 0
set_env V0_WRITE_KALMAN_DIAGNOSTICS 1
set_env V0_WRITE_SAME_SIGN 0
set_env V0_R1_SIGMA_RPHI 0.040
set_env V0_R1_SIGMA_R 0.200
set_env V0_R2_SIGMA_RPHI 0.025
set_env V0_R2_SIGMA_R 0.050
set_env V0_R3_SIGMA_RPHI 0.025
set_env V0_R3_SIGMA_R 0.200
set_env V0_SIGMA_Z 0.100
set_env V0_TRANSITION_SIGMA_RPHI 0.200
set_env V0_TRANSITION_SIGMA_R 0.500
set_env V0_TRANSITION_SIGMA_Z 0.200
set_env V0_BEAM_X 0.0
set_env V0_BEAM_Y 0.0
set_env V0_BEAM_Z 0.0
set_env V0_CLUSTER_NODE TPC_POLYCLUSTERS
set_env V0_TRACK_NODE TPC_POLYTRACKS
set_env V0_VERTEX_NODE TPC_POLYTRACKVERTICES

while [[ $# -gt 0 ]]; do
  case "$1" in
    --mode) MODE=$2; shift 2 ;;
    --input-dir) INPUT_DIR=$2; shift 2 ;;
    --pattern) PATTERN=$2; shift 2 ;;
    --output-dir) OUTPUT_DIR=$2; shift 2 ;;
    --output-base) OUTPUT_BASE=$2; shift 2 ;;
    --files-per-job) FILES_PER_JOB=$2; shift 2 ;;
    --nevents) NEVENTS=$2; shift 2 ;;
    --skip) SKIP=$2; shift 2 ;;
    --memory) MEMORY=$2; shift 2 ;;
    --queue) QUEUE=$2; shift 2 ;;
    --job-flavour) JOB_FLAVOUR=$2; shift 2 ;;
    --config-out) CONFIG_OUT=$2; shift 2 ;;
    --dry-run) DRY_RUN=1; shift ;;
    --fit-mode) set_env V0_FIT_MODE "$2"; shift 2 ;;
    --min-points) set_env V0_MIN_POINTS "$2"; shift 2 ;;
    --pre-track-pt-min) set_env V0_PRE_TRACK_PT_MIN "$2"; shift 2 ;;
    --pre-track-dca-xy-min) set_env V0_PRE_TRACK_DCA_XY_MIN "$2"; shift 2 ;;
    --pre-pair-dca-max) set_env V0_PRE_PAIR_DCA_MAX "$2"; shift 2 ;;
    --pair-pca-z-max) set_env V0_PAIR_PCA_Z_MAX "$2"; shift 2 ;;
    --pair-pca-dz-max) set_env V0_PAIR_PCA_DZ_MAX "$2"; shift 2 ;;
    --pair-decay-r-min) set_env V0_PAIR_DECAY_R_MIN "$2"; shift 2 ;;
    --pair-alpha-max) set_env V0_PAIR_ALPHA_ABS_MAX "$2"; shift 2 ;;
    --pair-dca-max) set_env V0_PAIR_DCA_MAX "$2"; shift 2 ;;
    --pair-dira-min) set_env V0_PAIR_DIRA_MIN "$2"; shift 2 ;;
    --kshort-mass-min) set_env V0_KSHORT_DETAIL_MASS_MIN "$2"; shift 2 ;;
    --kshort-mass-max) set_env V0_KSHORT_DETAIL_MASS_MAX "$2"; shift 2 ;;
    --write-kshort-details) set_env V0_WRITE_KSHORT_DETAILS "$2"; shift 2 ;;
    --write-track-tree) set_env V0_WRITE_TRACK_TREE "$2"; shift 2 ;;
    --write-cluster-residual-tree) set_env V0_WRITE_CLUSTER_RESIDUAL_TREE "$2"; shift 2 ;;
    --write-kalman-diagnostics) set_env V0_WRITE_KALMAN_DIAGNOSTICS "$2"; shift 2 ;;
    --write-same-sign) set_env V0_WRITE_SAME_SIGN "$2"; shift 2 ;;
    --r1-sigma-rphi) set_env V0_R1_SIGMA_RPHI "$2"; shift 2 ;;
    --r1-sigma-r) set_env V0_R1_SIGMA_R "$2"; shift 2 ;;
    --r2-sigma-rphi) set_env V0_R2_SIGMA_RPHI "$2"; shift 2 ;;
    --r2-sigma-r) set_env V0_R2_SIGMA_R "$2"; shift 2 ;;
    --r3-sigma-rphi) set_env V0_R3_SIGMA_RPHI "$2"; shift 2 ;;
    --r3-sigma-r) set_env V0_R3_SIGMA_R "$2"; shift 2 ;;
    --sigma-z) set_env V0_SIGMA_Z "$2"; shift 2 ;;
    --transition-sigma-rphi) set_env V0_TRANSITION_SIGMA_RPHI "$2"; shift 2 ;;
    --transition-sigma-r) set_env V0_TRANSITION_SIGMA_R "$2"; shift 2 ;;
    --transition-sigma-z) set_env V0_TRANSITION_SIGMA_Z "$2"; shift 2 ;;
    --beam-x) set_env V0_BEAM_X "$2"; shift 2 ;;
    --beam-y) set_env V0_BEAM_Y "$2"; shift 2 ;;
    --beam-z) set_env V0_BEAM_Z "$2"; shift 2 ;;
    --cluster-node) set_env V0_CLUSTER_NODE "$2"; shift 2 ;;
    --track-node) set_env V0_TRACK_NODE "$2"; shift 2 ;;
    --vertex-node) set_env V0_VERTEX_NODE "$2"; shift 2 ;;
    --setup-script) set_env V0_SETUP_SCRIPT "$(readlink -f "$2")"; shift 2 ;;
    --macro) set_env V0_MACRO "$(readlink -f "$2")"; shift 2 ;;
    -h|--help) usage; exit 0 ;;
    *) echo "Unknown option: $1" >&2; usage; exit 2 ;;
  esac
done

[[ "$MODE" == "local" || "$MODE" == "condor" ]] || { echo "--mode must be local or condor" >&2; exit 2; }
[[ -n "$INPUT_DIR" && -d "$INPUT_DIR" ]] || { echo "Valid --input-dir is required" >&2; exit 2; }
[[ -n "$OUTPUT_DIR" ]] || { echo "--output-dir is required" >&2; exit 2; }
[[ "$FILES_PER_JOB" =~ ^[1-9][0-9]*$ ]] || { echo "--files-per-job must be a positive integer" >&2; exit 2; }

INPUT_DIR=$(readlink -f "$INPUT_DIR")
mkdir -p "$OUTPUT_DIR"
OUTPUT_DIR=$(readlink -f "$OUTPUT_DIR")
WORK_DIR="$OUTPUT_DIR/condor_${OUTPUT_BASE}"
LIST_DIR="$WORK_DIR/lists"
LOG_DIR="$WORK_DIR/log"
mkdir -p "$LIST_DIR" "$LOG_DIR"

mapfile -d '' FILES < <(find "$INPUT_DIR" -maxdepth 1 -type f -name "$PATTERN" -print0 | sort -z)
[[ ${#FILES[@]} -gt 0 ]] || { echo "No files match $INPUT_DIR/$PATTERN" >&2; exit 1; }

echo "Found ${#FILES[@]} input files"

CONFIG_FILE=${CONFIG_OUT:-$WORK_DIR/v0_config.sh}
{
  echo '#!/usr/bin/env bash'
  for name in $(compgen -v V0_ | sort); do
    printf 'export %s=%q\n' "$name" "${!name}"
  done
} > "$CONFIG_FILE"
chmod +x "$CONFIG_FILE"

JOB_COUNT=0
for ((start=0; start<${#FILES[@]}; start+=FILES_PER_JOB)); do
  list="$LIST_DIR/job_${JOB_COUNT}.list"
  : > "$list"
  for ((j=0; j<FILES_PER_JOB && start+j<${#FILES[@]}; ++j)); do
    printf '%s\n' "${FILES[start+j]}" >> "$list"
  done
  ((JOB_COUNT+=1))
done

echo "Prepared $JOB_COUNT job list(s) in $LIST_DIR"

if [[ "$MODE" == "local" ]]; then
  OUTPUT_FILE="$OUTPUT_DIR/${OUTPUT_BASE}.root"
  echo "Running one local job with $LIST_DIR/job_0.list"
  if [[ $DRY_RUN -eq 0 ]]; then
    "$SCRIPT_DIR/run_tpc_v0_job.sh" "$CONFIG_FILE" "$LIST_DIR/job_0.list" "$OUTPUT_FILE" "$NEVENTS" "$SKIP"
  fi
  exit 0
fi

SUBMIT_FILE="$WORK_DIR/submit_${OUTPUT_BASE}.sub"
cat > "$SUBMIT_FILE" <<EOF_SUB
universe = vanilla
executable = $SCRIPT_DIR/run_tpc_v0_job.sh
arguments = $CONFIG_FILE $LIST_DIR/job_\$(Process).list $OUTPUT_DIR/${OUTPUT_BASE}_\$(Process).root $NEVENTS 0
#output = $LOG_DIR/job_\$(Process).out
#error = $LOG_DIR/job_\$(Process).err
log = /tmp/poly_reco_v0_condor.log
request_memory = $MEMORY
should_transfer_files = NO
EOF_SUB

if [[ -n "$QUEUE" ]]; then
  printf 'requirements = %s\n' "$QUEUE" >> "$SUBMIT_FILE"
fi
if [[ -n "$JOB_FLAVOUR" ]]; then
  printf '+JobFlavour = "%s"\n' "$JOB_FLAVOUR" >> "$SUBMIT_FILE"
fi
printf 'queue %d\n' "$JOB_COUNT" >> "$SUBMIT_FILE"

echo "Submit file: $SUBMIT_FILE"
if [[ $DRY_RUN -eq 0 ]]; then
  condor_submit "$SUBMIT_FILE"
fi
