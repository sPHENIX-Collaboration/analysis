#!/bin/bash                                                                                                                                                                                               

export USER="$(id -u -n)"
export LOGNAME=${USER}
export HOME=/sphenix/u/${LOGNAME}/macros/detectors/sPHENIX/
export MYINSTALL=/sphenix/user/bkimelman/sPHENIX/install/

source /opt/sphenix/core/bin/sphenix_setup.sh -n
source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL

runnumber=$1
segment=$2
outDir=$3

if [[ "$outDir" == "" ]]; then
    outDir=/sphenix/tg/tg01/jets/bkimelman/wEEC/
fi

runShort=$(($1/100))

caloDST=/sphenix/lustre01/sphnxpro/production2/run2pp/physics/jetskimmer/ana509_2024p022_v001/run_000${runShort}00_000$((runShort+1))00/DST_JETCALO_run2pp_ana509_2024p022_v001-000${runnumber}-${segment}.root
jetDST=/sphenix/lustre01/sphnxpro/production2/run2pp/physics/jetskimmer/ana509_2024p022_v001/run_000${runShort}00_000$((runShort+1))00/DST_Jet_run2pp_ana509_2024p022_v001-000${runnumber}-${segment}.root

root -b -q Fun4All_wholeEventEECs.C\(\"$caloDST\",\"$jetDST\",\"$outDir\"\)
