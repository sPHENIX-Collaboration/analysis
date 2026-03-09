#!/usr/bin/tcsh 

set nevts=$1
set indst=$2
set outroot=$3
set startevt=$4

echo "Nevts = $nevts"
echo "In-DST = $indst"
echo "Out-Root = $outroot"
echo "Start-event = $startevt"

source /opt/sphenix/core/bin/sphenix_setup.csh -n new

# Additional commands for my local environment

set workdir="/sphenix/user/hachiya/INTT/INTT/general_codes/hachiya/SiliconSeeding/SiliconSeedAna/"
set myinstall="${workdir}/install"
source /opt/sphenix/core/bin/setup_local.csh $myinstall 


echo 'Fun4All_DST_SiliconSeedAna.C('${nevts}',"'${indst}'","'${outroot}'",'${nevts}')'
root -b -q 'Fun4All_DST_SiliconSeedAna.C('${nevts}',"'${indst}'","'${outroot}'",'${nevts}')'

echo "all done"

date
