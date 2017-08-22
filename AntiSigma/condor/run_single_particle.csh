#!/bin/csh
setenv HOME /phenix/u/$LOGNAME
source /etc/csh.login
foreach i (/etc/profile.d/*.csh)
  source $i
end
source /opt/sphenix/core/bin/sphenix_setup.csh -n
setenv LD_LIBRARY_PATH /phenix/u/jpinkenburg/sPHENIX/install/lib:$LD_LIBRARY_PATH
echo 'here comes your environment'
printenv
echo arg1 \(listfile\): $1
echo arg2 \(outfile\): $2
echo running root.exe -q -b MakeShowerSize.C\(\"$1\",\"$2\",0\)
root.exe -q -b MakeShowerSize.C\(\"$1\",\"$2\",0\)
echo "script done"
