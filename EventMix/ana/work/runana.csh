#!/bin/csh

set baseDir=`pwd`

setenv HOME /phenix/u/$LOGNAME
source /etc/csh.login
foreach i (/etc/profile.d/*.csh)
source $i
end
#source $HOME/.login
source /opt/sphenix/core/bin/sphenix_setup.csh

setenv LD_LIBRARY_PATH /gpfs/mnt/gpfs02/sphenix/user/lebedev/mdc/test/analysis/EventMix/install/lib:${LD_LIBRARY_PATH}

printenv LD_LIBRARY_PATH
#echo "nargs"
#echo $#
echo "input parameters: " $1 $2

@ runno = $1 + $2 * 10
echo "run number: " $runno
#set strout="DST_sHijing_0_488fm-0000000001-0"
#set strext=".root"
#set outfilename="$strout$runno$strext"

echo "running..."
cd ${baseDir}
pwd

root -b -q  'runanamb.C('$runno')'


