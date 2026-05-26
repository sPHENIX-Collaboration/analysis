#!/usr/bin/env bash

nevts="$1"
indst_seed="$2"
indst_calo="$3"
outroot="$4"
startevt="$5"

echo "Nevts = ${nevts}"
echo "In-DST-seed = ${indst_seed}"
echo "In-DST-calo = ${indst_calo}"
echo "Out-Root = ${outroot}"
echo "Start-event = ${startevt}"

source /opt/sphenix/core/bin/sphenix_setup.sh -n ana.538

# Additional commands for my local environment

#myinstall="/sphenix/user/mikemoto/SiCalo/siliconseedana/install"

myinstall="/sphenix/user/hachiya/myrepo/analysis/SiCalo/install"
source /opt/sphenix/core/bin/setup_local.sh "${myinstall}"


echo "Fun4All_DST_SiliconSeedAna_run3pp.C(${nevts},\"${indst_seed}\",\"${indst_calo}\",\"${outroot}\",${startevt})"
root -b -q "Fun4All_DataDST_SiliconSeedAna_run3pp.C(${nevts},\"${indst_seed}\",\"${indst_calo}\",\"${outroot}\",${startevt})"

ls -lh "${outroot}" || echo "[WARN] output not found"

echo "all done"
date
