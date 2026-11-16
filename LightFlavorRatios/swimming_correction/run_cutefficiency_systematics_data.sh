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

PROCESS=$1

#LAMBDA_INFILE_BASE=/sphenix/tg/tg01/hf/mjpeters/lambdaKshortMB/lambdaKshort_20260422_DetroitMB_CR_2_mode_pTref_1p4/ppi_reco/outputKFParticle_ppi_reco
#KSHORT_INFILE_BASE=/sphenix/tg/tg01/hf/mjpeters/lambdaKshortMB/lambdaKshort_20260422_DetroitMB_CR_2_mode_pTref_1p4/pipi_reco/outputKFParticle_pipi_reco

NOMINAL_CUTEFFICIENCY_INFILE=/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/cutEfficiencyCorrection/data_pvsv_uncertainty.root

LAMBDA_INFILE_BASE=/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/closureTestSample/ppi_reco/outputKFParticle_ppi_reco
KSHORT_INFILE_BASE=/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/closureTestSample/pipi_reco/outputKFParticle_pipi_reco

OUTFILE_MIN_BASE=/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/cutEfficiencyCorrection/systematics_data_min/cut_efficiency
OUTFILE_MAX_BASE=/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/cutEfficiencyCorrection/systematics_data_max/cut_efficiency

LAMBDA_PDGID=3122
LAMBDA_DAUGHTERS={-211,2212}
LAMBDABAR_DAUGHTERS={211,-2212}

KSHORT_PDGID=310
KSHORT_DAUGHTERS={211,-211}

for i in {0..9}; do
	SEGMENT=`printf "%06d" $(( 10*PROCESS + i ))`

        rm ${OUTFILE_MIN_BASE}_${SEGMENT}.root
        rm ${OUTFILE_MAX_BASE}_${SEGMENT}.root
        rm ${OUTFILE_MIN_BASE}_pos_${SEGMENT}.root
        rm ${OUTFILE_MAX_BASE}_pos_${SEGMENT}.root
        rm ${OUTFILE_MIN_BASE}_neg_${SEGMENT}.root
        rm ${OUTFILE_MAX_BASE}_neg_${SEGMENT}.root

	# (lambda + lambdabar)/2Kshort
	root -l -q -b CutEfficiency_systematics.C\(\"${NOMINAL_CUTEFFICIENCY_INFILE}\",\"Lambda0\",${LAMBDA_PDGID},${LAMBDA_DAUGHTERS},\"${LAMBDA_INFILE_BASE}_${SEGMENT}.root\",true,StandardCuts::MC_Lambda0_cuts,BinInfo::Lambda0_MC_mass_bins,\"K_S0\",${KSHORT_PDGID},${KSHORT_DAUGHTERS},\"${KSHORT_INFILE_BASE}_${SEGMENT}.root\",false,StandardCuts::MC_K_S0_cuts,BinInfo::K_S0_MC_mass_bins,\"${OUTFILE_MIN_BASE}_${SEGMENT}.root\",\"${OUTFILE_MAX_BASE}_${SEGMENT}.root\"\)
	# lambda/Kshort
        root -l -q -b CutEfficiency_systematics.C\(\"${NOMINAL_CUTEFFICIENCY_INFILE}\",\"Lambda0\",${LAMBDA_PDGID},${LAMBDA_DAUGHTERS},\"${LAMBDA_INFILE_BASE}_${SEGMENT}.root\",false,StandardCuts::MC_Lambda0_pos_cuts,BinInfo::Lambda0_MC_mass_bins,\"K_S0\",${KSHORT_PDGID},${KSHORT_DAUGHTERS},\"${KSHORT_INFILE_BASE}_${SEGMENT}.root\",false,StandardCuts::MC_K_S0_cuts,BinInfo::K_S0_MC_mass_bins,\"${OUTFILE_MIN_BASE}_pos_${SEGMENT}.root\",\"${OUTFILE_MAX_BASE}_pos_${SEGMENT}.root\"\)
	# lambdabar/kshort
        root -l -q -b CutEfficiency_systematics.C\(\"${NOMINAL_CUTEFFICIENCY_INFILE}\",\"Lambda0\",-${LAMBDA_PDGID},${LAMBDABAR_DAUGHTERS},\"${LAMBDA_INFILE_BASE}_${SEGMENT}.root\",false,StandardCuts::MC_Lambda0_neg_cuts,BinInfo::Lambda0_MC_mass_bins,\"K_S0\",${KSHORT_PDGID},${KSHORT_DAUGHTERS},\"${KSHORT_INFILE_BASE}_${SEGMENT}.root\",false,StandardCuts::MC_K_S0_cuts,BinInfo::K_S0_MC_mass_bins,\"${OUTFILE_MIN_BASE}_neg_${SEGMENT}.root\",\"${OUTFILE_MAX_BASE}_neg_${SEGMENT}.root\"\)
done
