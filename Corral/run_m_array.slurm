#!/bin/bash
# One array task = one job list: lists/<set>/list_NN.txt, NN = SLURM_ARRAY_TASK_ID (2 digits).
# Submitted by run_m_array.bash (which snapshots the binary and makes the output folders).
# Outputs: root/<set>/chunks/corral_m_NN.root, pdf/<set>/chunks/corral_m_NN.pdf; the log is
# log/<set>/chunks/corral_m_NN.log.
#   $1 = set (dataset = the list folder, e.g. ana532)
SET=$1
# CORRAL_DIR is exported by run_m_array.bash (sbatch runs a spooled copy of this script, so it cannot
# find the project directory from its own location)
export PROJDIR=${CORRAL_DIR:?"CORRAL_DIR not set -- submit with run_m_array.bash"}
NN=$(printf "%02d" ${SLURM_ARRAY_TASK_ID})
LIST=$PROJDIR/lists/$SET/list_$NN.txt
BIN=$PROJDIR/log/$SET/chunks/corral_m.snapshot
NAME=corral_m_$NN
echo "hostname  = "$HOSTNAME
echo "set       = "$SET
echo "task      = "$NN"   (array job "$SLURM_ARRAY_JOB_ID")"
echo "list      = "$LIST"   ("$(wc -l < $LIST)" files)"
echo "binary    = "$BIN
WORK=$TMPDIR/corral_${SLURM_ARRAY_JOB_ID}_$NN
mkdir -p $WORK/root $WORK/pdf
cd $WORK || exit 1
pwd
cp -v $BIN ./corral_m
echo Starting..........................
time ./corral_m -n 0 -l $LIST -o $NAME
cp -v ./root/$NAME.root $PROJDIR/root/$SET/chunks/
cp -v ./pdf/$NAME.pdf   $PROJDIR/pdf/$SET/chunks/
cd $TMPDIR
rm -rf $WORK
exit
