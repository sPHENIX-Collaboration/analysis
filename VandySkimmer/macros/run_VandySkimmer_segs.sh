#!/bin/bash                                                                                                                                                                                               

export USER="$(id -u -n)"
export LOGNAME=${USER}
export HOME=/sphenix/u/${LOGNAME}/macros/detectors/sPHENIX/
export MYINSTALL=/sphenix/user/bkimelman/sPHENIX/install/

source /opt/sphenix/core/bin/sphenix_setup.sh -n
source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL

runnumber=$1
jobOf25=$2
outDir=$3

if [[ "$outDir" == "" ]]; then
    outDir=/sphenix/tg/tg01/jets/bkimelman/VandyDSTs/
fi

if [ ! -d "$outDir" ]
then
    mkdir -p $outDir
fi

runShort=$(($1/100))

caloDSTlist=filelists/run${runnumber}_caloList_jobOf25_${jobOf25}.list
rm $caloDSTlist

jetDSTlist=filelists/run${runnumber}_jetList_jobOf25_${jobOf25}.list
rm $jetDSTlist

for (( s=$((25*jobOf25)); s<=$((25*jobOf25+24)); s+=1 ))
do
    caloFile=/sphenix/lustre01/sphnxpro/production2/run2pp/physics/jetskimmer/ana521_2025p007_v001/run_000${runShort}00_000$((runShort+1))00/DST_JETCALO_run2pp_ana521_2025p007_v001-000${runnumber}-$(printf "%05d" $s).root
    if [[ -f "$caloFile" ]];
    then
        echo $caloFile >> $caloDSTlist
    fi

    jetFile=/sphenix/lustre01/sphnxpro/production2/run2pp/physics/jetskimmer/ana521_2025p007_v001/run_000${runShort}00_000$((runShort+1))00/DST_Jet_run2pp_ana521_2025p007_v001-000${runnumber}-$(printf "%05d" $s).root
    if [[ -f "$jetFile" ]];
    then
        echo $jetFile >> $jetDSTlist
    fi
done


root -b -q Fun4All_VandySkimmer_segs.C\($jobOf25,\"$caloDSTlist\",\"$jetDSTlist\",\"$outDir\"\)
