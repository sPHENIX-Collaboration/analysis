#!/bin/bash

BASEDIR=/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/closureTestSample/geometricAcceptance
OUTDIR=/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/geometricAcceptanceCorrection

hadd -d . -f -k -j ${BASEDIR}/merged_inclusive_parity.root ${BASEDIR}/Lambda0_geo_acceptance_0*.root &
hadd -d . -f -k -j ${BASEDIR}/merged_lambda_only.root ${BASEDIR}/Lambda0_geo_acceptance_pos_*.root &
hadd -d . -f -k -j ${BASEDIR}/merged_lambdabar_only.root ${BASEDIR}/Lambda0_geo_acceptance_neg_*.root &
hadd -d . -f -k -j ${BASEDIR}/merged_kshort.root ${BASEDIR}/K_S0_geo_acceptance_*.root &

hadd -d . -f -k -j ${BASEDIR}/systematics_minthreshold/merged_inclusive_parity.root ${BASEDIR}/systematics_minthreshold/Lambda0_geo_acceptance_minthreshold_0*.root &
hadd -d . -f -k -j ${BASEDIR}/systematics_minthreshold/merged_lambda_only.root ${BASEDIR}/systematics_minthreshold/Lambda0_geo_acceptance_pos_minthreshold_*.root &
hadd -d . -f -k -j ${BASEDIR}/systematics_minthreshold/merged_lambdabar_only.root ${BASEDIR}/systematics_minthreshold/Lambda0_geo_acceptance_neg_minthreshold_*.root &
hadd -d . -f -k -j ${BASEDIR}/systematics_minthreshold/merged_kshort.root ${BASEDIR}/systematics_minthreshold/K_S0_geo_acceptance_*.root &

hadd -d . -f -k -j ${BASEDIR}/systematics_maxthreshold/merged_inclusive_parity.root ${BASEDIR}/systematics_maxthreshold/Lambda0_geo_acceptance_maxthreshold_0*.root &
hadd -d . -f -k -j ${BASEDIR}/systematics_maxthreshold/merged_lambda_only.root ${BASEDIR}/systematics_maxthreshold/Lambda0_geo_acceptance_pos_maxthreshold_*.root &
hadd -d . -f -k -j ${BASEDIR}/systematics_maxthreshold/merged_lambdabar_only.root ${BASEDIR}/systematics_maxthreshold/Lambda0_geo_acceptance_neg_maxthreshold_*.root &
hadd -d . -f -k -j ${BASEDIR}/systematics_maxthreshold/merged_kshort.root ${BASEDIR}/systematics_maxthreshold/K_S0_geo_acceptance_*.root &
