#!/bin/bash
# One Finalize job, submitted by run_finalize.bash (which snapshots the binary and makes the output folders).
#   $1 = dataset (-d), $2 = Finalize run string (Finalize or Finalize_<tokens>)
# Runs in a scratch dir on the worker node, then copies root/pdf to root|pdf/<dataset>/; the log is written
# directly to log/<dataset>/corral_<runstring>.log.
SET=$1
FRS=${2:-Finalize}
# CORRAL_DIR is exported by run_finalize.bash (sbatch runs a spooled copy of this script)
export PROJDIR=${CORRAL_DIR:?"CORRAL_DIR not set -- submit with run_finalize.bash"}
NAME=corral_$FRS
echo "hostname  = "$HOSTNAME
echo "dataset   = "$SET"   run string "$FRS"   (job "$SLURM_JOB_ID")"
echo "binary    = "$PROJDIR/log/$SET/$NAME.snapshot
[ -n "$FINALIZE_NEXPMIN" ] && echo "FINALIZE_NEXPMIN = "$FINALIZE_NEXPMIN
[ -n "$FINALIZE_MFRAC" ]   && echo "FINALIZE_MFRAC   = "$FINALIZE_MFRAC
WORK=${TMPDIR:-/tmp}/fin_${SLURM_JOB_ID}
mkdir -p $WORK/root $WORK/pdf
cd $WORK || exit 1
pwd
cp $PROJDIR/log/$SET/$NAME.snapshot ./corral
echo Starting..........................
time ./corral -d $SET -s $FRS
RC=$?
cp -v ./root/$NAME.root $PROJDIR/root/$SET/
cp -v ./pdf/$NAME.pdf   $PROJDIR/pdf/$SET/
cd /
rm -rf $WORK
exit $RC
