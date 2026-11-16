#!/bin/bash

export USER="$(id -u -n)"
export LOGNAME=${USER}
export HOME=/sphenix/u/${LOGNAME}
export MYINSTALL=/gpfs/mnt/gpfs02/phenix/spin/spin1/phnxsp01/dillfitz/sphenix_software/install

source /opt/sphenix/core/bin/sphenix_setup.sh -n
source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL

#printenv

echo "Done with setup"

runnumber=$1
segment=$2
dir=$3


if [ ! -d "/sphenix/tg/tg01/coldqcd/dillfitz/singletrackan/lumi/$dir" ]
then
    mkdir -p /sphenix/tg/tg01/coldqcd/dillfitz/singletrackan/lumi/$dir
fi


file=/sphenix/lustre01/sphnxpro/production/run3pp/physics/ana573_2026p003_v001/DST_TRKR_TRACKS/run_00079500_00079600/DST_TRKR_TRACKS_run3pp_ana573_2026p003_v001-000${runnumber}-${segment}.root

root -b -q  "Fun4All_StreamingLumi.C(-1, \"${file}\",\"/sphenix/tg/tg01/coldqcd/dillfitz/singletrackan/lumi/${dir}/\")"
        

