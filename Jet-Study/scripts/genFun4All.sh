#!/usr/bin/env bash
export USER="$(id -u -n)"
export LOGNAME=${USER}
export HOME=/sphenix/u/${LOGNAME}
export MYINSTALL="$HOME/Documents/sPHENIX/install"

source /opt/sphenix/core/bin/sphenix_setup.sh -n new
source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL

exe=${1}
inputJET=${2}
output=${3}
submitDir=${4}

# extract runnumber from file name
file=$(basename "$inputJET")
IFS='-' read -r p1 p2 p3 <<< "$file"

if [[ ! -z "$_CONDOR_SCRATCH_DIR" && -d $_CONDOR_SCRATCH_DIR ]]
 then
   cd $_CONDOR_SCRATCH_DIR
   # transfer the input file(s)
   if [[ "$inputJET" =~ \.root$ ]]; then
     getinputfiles.pl $inputJET
     run=$(echo "$p2" | sed 's/^0*//') # Remove leading zeros using sed
     echo "File Transferred: $(readlink -f $inputJET)"
   else
     getinputfiles.pl --filelist $inputJET
     run=$(echo "$p3" | sed 's/^0*//' | sed 's/\.[^.]*$//') # Remove leading zeros and extension
   fi
else
  echo "condor scratch NOT set"
  exit -1
fi

# print the environment - needed for debugging
printenv

mkdir -p $run

$exe $inputJET $run/$output

echo "All Done and Transferring Files Back"
cp -rv $run $submitDir

echo "Finished"
