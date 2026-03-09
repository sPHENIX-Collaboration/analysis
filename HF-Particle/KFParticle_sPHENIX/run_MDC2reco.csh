#!/bin/tcsh

setenv HOME /sphenix/u/${LOGNAME}
source /opt/sphenix/core/bin/sphenix_setup.csh -n new

#setenv SPHENIX $HOME/sPHENIX
#setenv MYINSTALL $SPHENIX/install
#setenv LD_LIBRARY_PATH $MYINSTALL/lib:$LD_LIBRARY_PATH
#setenv ROOT_INCLUDE_PATH $MYINSTALL/include:$ROOT_INCLUDE_PATH
#
#source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL

@ nEvents=0
set inputFiles=""

foreach fileList ($argv)
  set inputFiles=${inputFiles}\"${fileList}\",
end

set trimmedList=`echo $inputFiles | rev | cut -c 2- | rev`
echo running: run_HFreco.sh $*
root.exe -q -b Fun4All_MDC2reco.C\("{${trimmedList}}",$nEvents\)
echo Script done
