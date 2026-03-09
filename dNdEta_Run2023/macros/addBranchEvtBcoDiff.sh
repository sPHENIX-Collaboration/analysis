#! /bin/bash
export USER="$(id -u -n)"
export LOGNAME=${USER}
export HOME=/sphenix/u/${LOGNAME}

if [ "$#" -ne 4 ]; then
    echo "Usage: $0 <file directory> <input file name> <output file name> <software version>"
    exit 1
fi

# print all arguments
echo "file directory: $1"
echo "input file name: $2"
echo "output file name: $3"
echo "software version: $4"

source /opt/sphenix/core/bin/sphenix_setup.sh -n $4

export MYINSTALL=$HOME/install
export LD_LIBRARY_PATH=$MYINSTALL/lib:$LD_LIBRARY_PATH
export ROOT_INCLUDE_PATH=$MYINSTALL/include:$ROOT_INCLUDE_PATH

source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL

INPUTFILE="$1/$2"
OUTPUTFILE="$1/$3"

root.exe -q -b EvtInttBCODiff.C\(\"$INPUTFILE\",\"$OUTPUTFILE\",61,0,1,0\)

echo all done
