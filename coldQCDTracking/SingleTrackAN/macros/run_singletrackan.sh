#!/bin/bash

export USER="$(id -u -n)"
export LOGNAME=${USER}
export HOME=/sphenix/u/${LOGNAME}
export MYINSTALL=/sphenix/user/dloomis/install/

source /opt/sphenix/core/bin/sphenix_setup.sh -n
source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL

#printenv

echo "Done with setup"

runnumber=$1
segment=$2
dir=$3


runShort=$(($1/100))


if [ ! -d "/sphenix/tg/tg01/coldqcd/dloomis/singletrackan/$dir" ]
then
    mkdir -p /sphenix/tg/tg01/coldqcd/dloomis/singletrackan/$dir
fi


file=/sphenix/lustre01/sphnxpro/production/run3pp/physics/ana556_2025p013_v002/DST_TRKR_SEED/run_00079500_00079600/DST_TRKR_SEED_run3pp_ana556_2025p013_v002-000${runnumber}-${segment}.root

root -b -q "Fun4All_SingleTrackAN.C(-1,\"${file}\",\"/sphenix/tg/tg01/coldqcd/dloomis/singletrackan/${dir}/\")"

