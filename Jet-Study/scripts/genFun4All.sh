#!/usr/bin/env bash
export USER="$(id -u -n)"
export LOGNAME=${USER}
export HOME=/sphenix/u/${LOGNAME}
export MYINSTALL="$HOME/Documents/sPHENIX/install"

source /opt/sphenix/core/bin/sphenix_setup.sh -n new
source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL

exe=${1}
inputJET=${2}
inputJETCALO=${3}
output=${4}
submitDir=${5}

# extract runnumber from file name
file=$(basename "$inputJET")
IFS='-' read -r p1 p2 p3 <<< "$file"

if [[ ! -z "$_CONDOR_SCRATCH_DIR" && -d $_CONDOR_SCRATCH_DIR ]]
 then
   cd $_CONDOR_SCRATCH_DIR
   # transfer the input file(s)
   if [[ "$inputJET" =~ \.root$ ]]; then
     getinputfiles.pl $inputJET
     getinputfiles.pl $inputJETCALO
     run=$(echo "$p2" | sed 's/^0*//') # Remove leading zeros using sed
     echo "File Transferred: $(readlink -f $inputJET)"
     echo "File Transferred: $(readlink -f $inputJETCALO)"
   else
     getinputfiles.pl --filelist $inputJET
     getinputfiles.pl --filelist $inputJETCALO
     run=$(echo "$p3" | sed 's/^0*//' | sed 's/\.[^.]*$//') # Remove leading zeros and extension
   fi
else
  echo "condor scratch NOT set"
  exit -1
fi

# print the environment - needed for debugging
printenv

mkdir -p $run

$exe $inputJET $inputJETCALO $run/$output

echo "All Done and Transferring Files Back"
cp -rv $run $submitDir

echo "Finished"
