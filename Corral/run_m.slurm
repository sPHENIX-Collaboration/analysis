#!/bin/bash
#SBATCH -N 1
#SBATCH -t 2-23:59:59

#	called by slurm by run_m.bash

SET=$1
NEV=$2
OUT=${3:-$1}		# output set: the dataset, or <dataset>_<variant> (run_m.bash CORRAL_VARIANT=)
CRS=$4			# run string of the variant (empty = the code defaults)
echo "hostname  = "$HOSTNAME
echo "dataset   = "$SET
echo "nev       = "$NEV
echo "output    = "$OUT"   run string '"$CRS"'"

# CORRAL_DIR is exported by run_m.bash (sbatch runs a spooled copy of this script)
export PROJDIR=${CORRAL_DIR:?"CORRAL_DIR not set -- submit with run_m.bash"}/
echo "TMPDIR        = "$TMPDIR
echo "PROJDIR       = "$PROJDIR
export JOBSCR=$TMPDIR

pwd 
export JOBNUMA=`echo ${SLURM_JOBID} | awk -F. '{print $NR"\n"}'`
# echo "JOBNUMA = "$JOBNUMA
k=${#JOBNUMA} 
echo "Digits in JOBNUMA = "$k
if [ $k -lt 3 ]; then
	export JOBNUM=000000$JOBNUMA
elif [ $k -lt 4 ]; then
	export JOBNUM=00000$JOBNUMA
elif [ $k -lt 5 ]; then
	export JOBNUM=0000$JOBNUMA
elif [ $k -lt 6 ]; then
	export JOBNUM=000$JOBNUMA
elif [ $k -lt 7 ]; then
	export JOBNUM=00$JOBNUMA
elif [ $k -lt 8 ]; then
	export JOBNUM=0$JOBNUMA
elif [ $k -lt 9 ]; then
	export JOBNUM=$JOBNUMA
fi
# echo "JOBID     = "$SLURM_JOBID
echo "JOBNUM    = "$JOBNUM
# export JOBID=`echo $JOBNUM`

cd $JOBSCR
pwd
mkdir corral_${JOBNUM}
cd corral_${JOBNUM}
pwd
cp -v $PROJDIR/log/$OUT/corral.snapshot ./corral		# the binary run_m.bash snapshotted
mkdir root
mkdir pdf
ls -lasF

export rsd=`xxd -l 3 -ps -c 10 /dev/urandom` 
export rsdn=`printf "%i" 0x$rsd` 
echo "rsdn      = "$rsdn

#--------------------------------------
echo Starting..........................
# No run string: the code defaults are the validated best effort; a variant adds its own (-s), output name stays corral
time ./corral -n $NEV -l $PROJDIR/lists/$SET/list_all.txt -o corral ${CRS:+-s $CRS}
cp -v ./root/corral.root $PROJDIR/root/$OUT/
cp -v ./pdf/corral.pdf   $PROJDIR/pdf/$OUT/
#--------------------------------------

cd $JOBSCR
rm -rf corral_${JOBNUM}

exit





