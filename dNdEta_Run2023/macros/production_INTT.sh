#! /bin/bash
export USER="$(id -u -n)"
export LOGNAME=${USER}
export HOME=/sphenix/u/${LOGNAME}
source /opt/sphenix/core/bin/sphenix_setup.sh -n $9

export MYINSTALL=$HOME/install
export LD_LIBRARY_PATH=$MYINSTALL/lib:$LD_LIBRARY_PATH
export ROOT_INCLUDE_PATH=$MYINSTALL/include:$ROOT_INCLUDE_PATH

source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL

# $7 is the output file name
# break up the output file name into the directory and the file name
OUTDIR=$(dirname $7)
if [ ! -d $OUTDIR ]; then
    echo "$OUTDIR does not exist, creating it"
    mkdir -p $OUTDIR
fi

# print the environment - needed for debugging
# printenv
# this is how you run your Fun4All_G4_sPHENIX.C macro in batch:
root.exe -q -b Fun4All_G4_sPHENIX.C\($1,$2,$3,\"$4\",\"$5\",$6,\"$7\",$8\)

echo all done
