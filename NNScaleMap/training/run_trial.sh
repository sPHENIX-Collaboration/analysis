#!/bin/bash
# One train.py job of the hpo/ or syst/ campaign (Condor executable):
#   run_trial.sh <card> <outdir> <kshort> <lam> [train.py flags ...]
# The flags come after the card, so they override it; a later --kshort/--lam replaces <kshort>/<lam>.
set -e

training_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
card=$1
outdir=$2
kshort=$3
lam=$4
shift 4

source "${training_dir}/../setup.sh"
cd "${training_dir}"
echo "host $(hostname), start $(date -Is), OMP_NUM_THREADS=${OMP_NUM_THREADS}"
python train.py --config "${card}" --kshort "${kshort}" --lam "${lam}" --outdir "${outdir}" "$@"
echo "end $(date -Is)"
