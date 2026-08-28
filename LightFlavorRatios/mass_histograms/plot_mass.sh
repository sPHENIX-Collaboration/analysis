#!/bin/bash

source /opt/sphenix/core/bin/sphenix_setup.sh -n new

export USER="$(id -u -n)"
export LOGNAME=${USER}
export HOME=/sphenix/u/${LOGNAME}

export SPHENIX=${HOME}/sPHENIX
export MYINSTALL=$SPHENIX/install
export LD_LIBRARY_PATH=$MYINSTALL/lib:$LD_LIBRARY_PATH
export ROOT_INCLUDE_PATH=$MYINSTALL/include:$ROOT_INCLUDE_PATH

mkdir -p /tmp/log_mjpeters/

SEGMENT=$1

INFILE_LAMBDA=/sphenix/tg/tg01/hf/mjpeters/lambdaKshortMB/lambdaKshort_20260422_DetroitMB_CR_2_mode_pTref_1p4/ppi_reco/outputKFParticle_ppi_reco_${SEGMENT}.root
INFILE_KSHORT=/sphenix/tg/tg01/hf/mjpeters/lambdaKshortMB/lambdaKshort_20260422_DetroitMB_CR_2_mode_pTref_1p4/pipi_reco/outputKFParticle_pipi_reco_${SEGMENT}.root

OUTFILE_LAMBDA=/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/mass_histogram_output/MC/lambda_${SEGMENT}.root
OUTFILE_KSHORT=/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/mass_histogram_output/MC/Kshort_${SEGMENT}.root

OUTFILE_LAMBDA_POS=/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/mass_histogram_output/MC_pos/lambda_${SEGMENT}.root
OUTFILE_KSHORT_POS=/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/mass_histogram_output/MC_pos/Kshort_${SEGMENT}.root

OUTFILE_LAMBDA_NEG=/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/mass_histogram_output/MC_neg/lambda_${SEGMENT}.root
OUTFILE_KSHORT_NEG=/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/mass_histogram_output/MC_neg/Kshort_${SEGMENT}.root

rm $OUTFILE_LAMBDA
rm $OUTFILE_KSHORT
rm $OUTFILE_LAMBDA_POS
rm $OUTFILE_KSHORT_POS
rm $OUTFILE_LAMBDA_NEG
rm $OUTFILE_KSHORT_NEG

root -l -q -b plot_mass.C\(\"${INFILE_LAMBDA}\",\"Lambda0\",\"${OUTFILE_LAMBDA}\"\)
root -l -q -b plot_mass.C\(\"${INFILE_KSHORT}\",\"K_S0\",\"${OUTFILE_KSHORT}\"\)

root -l -q -b plot_mass.C\(\"${INFILE_LAMBDA}\",\"Lambda0\",\"${OUTFILE_LAMBDA_POS}\",BinInfo::mass_bins_MC_pos\)
root -l -q -b plot_mass.C\(\"${INFILE_KSHORT}\",\"K_S0\",\"${OUTFILE_KSHORT_POS}\",BinInfo::mass_bins_MC_pos\)

root -l -q -b plot_mass.C\(\"${INFILE_LAMBDA}\",\"Lambda0\",\"${OUTFILE_LAMBDA_NEG}\",BinInfo::mass_bins_MC_neg\)
root -l -q -b plot_mass.C\(\"${INFILE_KSHORT}\",\"K_S0\",\"${OUTFILE_KSHORT_NEG}\",BinInfo::mass_bins_MC_neg\)
