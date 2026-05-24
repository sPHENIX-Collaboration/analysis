#!/usr/bin/env bash

nevts="$1"
indst_clus="$2"
indst_seed="$3"
indst_calo="$4"
outroot="$5"
startevt="$6"

echo "Nevts = ${nevts}"
echo "In-DST-clus = ${indst_clus}"
echo "In-DST-seed = ${indst_seed}"
echo "In-DST-calo = ${indst_calo}"
echo "Out-Root = ${outroot}"
echo "Start-event = ${startevt}"

# source /opt/sphenix/core/bin/sphenix_setup.sh -n ana.516
source /opt/sphenix/core/bin/sphenix_setup.sh -n ana.527
#source /opt/sphenix/core/bin/sphenix_setup.sh -n ana.540
#source /opt/sphenix/core/bin/sphenix_setup.sh 
# source /opt/sphenix/core/bin/sphenix_setup.sh -n new

# Additional commands for my local environment

#myinstall="/sphenix/user/mikemoto/SiCalo/siliconseedana/install"

myinstall="/sphenix/user/hachiya/myrepo/analysis/SiCalo/install"
source /opt/sphenix/core/bin/setup_local.sh "${myinstall}"

#export NOPAYLOADCLIENT_CONF=/sphenix/user/phnxbld/sPHENIX_newcdb.json

echo "Fun4All_DST_SiliconSeedAna.C(${nevts},\"${indst_clus}\",\"${indst_seed}\",\"${indst_calo}\",\"${outroot}\",${startevt})"
root -b -q "Fun4All_DataDST_SiliconSeedAna.C(${nevts},\"${indst_clus}\",\"${indst_seed}\",\"${indst_calo}\",\"${outroot}\",${startevt})"

ls -lh "${outroot}" || echo "[WARN] output not found"

echo "all done"
date
