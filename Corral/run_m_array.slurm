#!/bin/bash
# One array task = one job list: lists/<set>/list_NN.txt, NN = SLURM_ARRAY_TASK_ID (2 digits).
# Submitted by run_m_array.bash (which snapshots the binary and makes the output folders).
# Outputs: root/<set>/chunks/corral_NN.root, pdf/<set>/chunks/corral_NN.pdf; the log is
# log/<set>/chunks/corral_NN.log.
#   $1 = set (dataset = the list folder, e.g. ana532); $2 = output set (= $1, or <set>_<variant>); $3 = run string (optional)
SET=$1
OUT=${2:-$1}
CRS=$3
# CORRAL_DIR is exported by run_m_array.bash (sbatch runs a spooled copy of this script, so it cannot
# find the project directory from its own location)
export PROJDIR=${CORRAL_DIR:?"CORRAL_DIR not set -- submit with run_m_array.bash"}
NN=$(printf "%02d" ${SLURM_ARRAY_TASK_ID})
LIST=$PROJDIR/lists/$SET/list_$NN.txt
BIN=$PROJDIR/log/$OUT/chunks/corral.snapshot
NAME=corral_$NN
echo "hostname  = "$HOSTNAME
echo "set       = "$SET"   -> "$OUT"   run string '"$CRS"'"
echo "task      = "$NN"   (array job "$SLURM_ARRAY_JOB_ID")"
echo "list      = "$LIST"   ("$(wc -l < $LIST)" files)"
echo "binary    = "$BIN
WORK=$TMPDIR/corral_${SLURM_ARRAY_JOB_ID}_$NN
mkdir -p $WORK/root $WORK/pdf
cd $WORK || exit 1
pwd
cp -v $BIN ./corral
echo Starting..........................
time ./corral -n 0 -l $LIST -o $NAME ${CRS:+-s $CRS}
cp -v ./root/$NAME.root $PROJDIR/root/$OUT/chunks/
cp -v ./pdf/$NAME.pdf   $PROJDIR/pdf/$OUT/chunks/
cd $TMPDIR
rm -rf $WORK
exit
