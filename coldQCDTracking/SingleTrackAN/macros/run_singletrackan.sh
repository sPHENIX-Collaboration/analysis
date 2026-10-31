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


if [ ! -d "/sphenix/tg/tg01/coldqcd/dillfitz/singletrackan/$dir" ]
then
    mkdir -p /sphenix/tg/tg01/coldqcd/dillfitz/singletrackan/$dir
fi


file=/sphenix/lustre01/sphnxpro/production/run3pp/physics/ana573_2026p003_v001/DST_TRKR_TRACKS/run_00079500_00079600/DST_TRKR_TRACKS_run3pp_ana573_2026p003_v001-000${runnumber}-${segment}.root
lumifile=/sphenix/tg/tg01/coldqcd/dillfitz/singletrackan/lumi/Oct13/DST_STREAMING_LUMIINFO-000${runnumber}-${segment}.root

root -b -q "Fun4All_SingleTrackAN.C(-1,\"${file}\", \"${lumifile}\", \"/sphenix/tg/tg01/coldqcd/dillfitz/singletrackan/${dir}/\")"

