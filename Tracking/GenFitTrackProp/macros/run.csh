#!/bin/tcsh -f

unsetenv OFFLINE_MAIN 
unsetenv ONLINE_MAIN
unsetenv ROOTSYS
unsetenv LD_LIBRARY_PATH
source /opt/sphenix/core/bin/sphenix_setup.csh -n new
echo $OFFLINE_MAIN

setenv LD_LIBRARY_PATH $GITHUB_ROOT/HaiwangYu/install/lib/:$LD_LIBRARY_PATH 

source /opt/phenix/bin/odbcini_setup.csh 

#time root -l -q RunGenFitTrackProp.C\(\"$1\",$2,$3\)

time root -l -q Fun4All_single_particle.C\($1,\"\",\"SvtxClusters.root\",\"\",15,0,$2\)
time root -l -q RunGenFitTrackProp.C\(\"SvtxClusters.root\",$2,0\)
