#!/bin/bash

export USER="$(id -u -n)"
export LOGNAME=${USER}
export HOME=/sphenix/u/${LOGNAME}/macros/detectors/sPHENIX/
export MYINSTALL=/sphenix/user/bkimelman/sPHENIX/install/

source /opt/sphenix/core/bin/sphenix_setup.sh -n
source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL
#printenv

echo "Done with setup"

Sample=$1
segment=$2

outfile=/sphenix/tg/tg01/jets/bkimelman/Herwig_UE/Herwig_rootFiles_Aug5_2026/Herwig${Sample}_${segment}.root

sample=`echo "$Sample" | tr '[:upper:]' '[:lower:]'`

cd /sphenix/u/bkimelman/analysis/HERWIG_UE/

if [ -f "filelists/Herwig${Sample}_${segment}.list" ];
then
    rm filelists/Herwig${Sample}_${segment}.list
fi

#for i in {0..99};
for ((i=segment; i<segment+20; i++));
do
    #f=/sphenix/lustre01/sphnxpro/mdc2/herwig/g4hits/run0028/${sample}/G4Hits_Herwig_${Sample}-0000000028-$(printf "%06d" $((segment*50+i))).root
    f=/sphenix/lustre01/sphnxpro/mdc2/herwig/g4hits/run0028/${sample}/G4Hits_Herwig_${Sample}-0000000028-$(printf "%06d" $i).root
    if [ -f "$f" ];
    then
        echo $f >> filelists/Herwig${Sample}_${segment}.list
    fi
done

if [ ! -s "filelists/Herwig${Sample}_${segment}.list" ];
then
    echo "file filelists/Herwig${Sample}_${segment}.list did not exist or was empty. Skipping"
    if [ -f "filelists/Herwig${Sample}_${segment}.list" ];
    then
        rm filelists/Herwig${Sample}_${segment}.list
    fi
    exit 1
fi

root -b -q Fun4All_HerwigUE.C\(-1,\"filelists/Herwig${Sample}_${segment}.list\",\"${outfile}\"\)