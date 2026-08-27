#! /bin/bash
export USER="$(id -u -n)"
export LOGNAME=${USER}
export HOME=/sphenix/u/${LOGNAME}
source /opt/sphenix/core/bin/sphenix_setup.sh -n new

export MYINSTALL=$HOME/install
export LD_LIBRARY_PATH=$MYINSTALL/lib:$LD_LIBRARY_PATH
# export ROOT_INCLUDE_PATH=$MYINSTALL/include:$ROOT_INCLUDE_PATH

source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL

# This is has to be after sourcing the setup_local.sh, otherwise ROOT_INCLUDE_PATH will be overwritten by the setup scripts
# export WORKSPACE=/sphenix/user/hjheng/sPHENIXRepo
# export ROOT_INCLUDE_PATH=$WORKSPACE/macros/common:$ROOT_INCLUDE_PATH

# $2 is the output file name. Make sure the output directory exists
# get the output directory from the output file path
outputdir=$(dirname "$2")
if [ ! -d "$outputdir" ]; then
  echo "Output directory $outputdir does not exist, creating it now."
  mkdir -p "$outputdir"
fi

./lfratio_pythia $1 $2

echo all done
