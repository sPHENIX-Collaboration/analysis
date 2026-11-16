#!/bin/bash

PROCESS=$1

mkdir -p /tmp/log_mjpeters/

LAMBDA_FLAVOR=3122
LAMBDABAR_FLAVOR=-3122
KS_FLAVOR=310

LAMBDA_DAUGHTERS=\{-211,2212\}
LAMBDABAR_DAUGHTERS=\{211,-2212\}
KS_DAUGHTERS=\{211,-211\}

#LAMBDA_FILEBASE=/sphenix/tg/tg01/hf/mjpeters/lambdaKshortMB/lambdaKshort_20260422_DetroitMB_CR_2_mode_pTref_1p4/evaluator/outputeval
#KS_FILEBASE=/sphenix/tg/tg01/hf/mjpeters/lambdaKshortMB/lambdaKshort_20260422_DetroitMB_CR_2_mode_pTref_1p4/evaluator/outputeval

LAMBDA_FILEBASE=/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/closureTestSample/evaluator/outputeval
KS_FILEBASE=/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/closureTestSample/evaluator/outputeval

OUTPUT_FILEBASE=/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/geometricAcceptanceCorrection

source /opt/sphenix/core/bin/sphenix_setup.sh -n new

export USER="$(id -u -n)"
export LOGNAME=${USER}
export HOME=/sphenix/u/${LOGNAME}

export SPHENIX=${HOME}/sPHENIX
export MYINSTALL=$SPHENIX/install
export LD_LIBRARY_PATH=$MYINSTALL/lib:$LD_LIBRARY_PATH
export ROOT_INCLUDE_PATH=$MYINSTALL/include:$ROOT_INCLUDE_PATH

SEGMENT=`printf "%06d" $PROCESS`

rm ${OUTPUT_FILEBASE}/inclusive_parity/hists_${SEGMENT}.root
rm ${OUTPUT_FILEBASE}/lambda_only/hists_${SEGMENT}.root
rm ${OUTPUT_FILEBASE}/lambdabar_only/hists_${SEGMENT}.root

# (Lambda + Lambdabar) / 2Ks
root -l -b evaluator_geomAccept.C\($LAMBDA_FLAVOR,$LAMBDA_DAUGHTERS,$KS_FLAVOR,$KS_DAUGHTERS,\"\#Lambda\",\"K^{0}_{S}\",\"$LAMBDA_FILEBASE\",\"$KS_FILEBASE\",1.,2.,\"${OUTPUT_FILEBASE}/inclusive_parity/hists\",${PROCESS}\)
# Lambda/Ks
#root -l -b evaluator_geomAccept.C\($LAMBDA_FLAVOR,$LAMBDA_DAUGHTERS,$KS_FLAVOR,$KS_DAUGHTERS,\"\#Lambda\",\"K^{0}_{S}\",\"$LAMBDA_FILEBASE\",\"$KS_FILEBASE\",1.,1.,\"${OUTPUT_FILEBASE}/lambda_only/hists\",${PROCESS},false\)
#Lambdabar/Ks
#root -l -b evaluator_geomAccept.C\($LAMBDABAR_FLAVOR,$LAMBDABAR_DAUGHTERS,$KS_FLAVOR,$KS_DAUGHTERS,\"\#Lambda\",\"K^{0}_{S}\",\"$LAMBDA_FILEBASE\",\"$KS_FILEBASE\",1.,1.,\"${OUTPUT_FILEBASE}/lambdabar_only/hists\",${PROCESS},false\)
