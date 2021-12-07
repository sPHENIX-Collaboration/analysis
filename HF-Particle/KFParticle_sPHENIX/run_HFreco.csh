#!/bin/tcsh

setenv HOME /sphenix/u/${LOGNAME}
source /opt/sphenix/core/bin/sphenix_setup.csh -n new

'''
setenv SPHENIX $HOME/sPHENIX
setenv MYINSTALL $SPHENIX/install
setenv LD_LIBRARY_PATH $MYINSTALL/lib:$LD_LIBRARY_PATH
setenv ROOT_INCLUDE_PATH $MYINSTALL/include:$ROOT_INCLUDE_PATH

source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL
'''
nEvents=0
echo "running: run_HFreco.sh $*"
echo "arg1 \(track inputFile\) : $1"
echo "arg2 \(truth inputFile\) : $2"
echo "arg3 \(calo inputFile\) : $3"
echo "Pileup is not implemented yet"
root.exe -q -b Fun4All_HFreco.C\(\"$1\",\"$2\",\"$3\",\"\",$nEvents\)
echo "Script done"
