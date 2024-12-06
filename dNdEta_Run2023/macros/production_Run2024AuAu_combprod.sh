#! /bin/bash
export USER="$(id -u -n)"
export LOGNAME=${USER}
export HOME=/sphenix/u/${LOGNAME}

if [ "$#" -ne 8 ]; then
    echo "Usage: $0 <run number> <productionTag> <calodstlist> <inttdstlist> <number of events> <output directory> <output file name> <software version>"
    exit 1
fi

# print all arguments
echo "run number: $1"
echo "productionTag: $2"
echo "calodstlist: $3"
echo "inttdstlist: $4"
echo "number of events: $5"
echo "output directory: $6"
echo "output file name: $7"
echo "software version: $8"

source /opt/sphenix/core/bin/sphenix_setup.sh -n $8

export MYINSTALL=$HOME/install
export LD_LIBRARY_PATH=$MYINSTALL/lib:$LD_LIBRARY_PATH
export ROOT_INCLUDE_PATH=$MYINSTALL/include:$ROOT_INCLUDE_PATH

source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL

# check if the output directory exists. If not, create it.
if [ ! -d "$6" ]; then
    mkdir -p $6
fi

# output file name is $6/$7
OUTPUTFILE="$6/$7"

# 1st argument: run number
# 2nd argument: productionTag
# 3rd argument: calodstlist
# 4th argument: inttdstlist
# 5th argument: number of events
# 6th argument: output directory
# 7th argument: output file name
root.exe -q -b Fun4All_dNdeta2024AuAu_production.C\($1,\"$2\",\"$3\",\"$4\",$5,\"$OUTPUTFILE\",0,0\)

echo all done
