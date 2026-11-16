#!/bin/bash

BASEDIR=/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/geometricAcceptanceCorrection/corrections

root -l -q -b get_geoAcceptance_syserr.C\(\"${BASEDIR}/geo_acceptance_inclusive.root\",\"${BASEDIR}/geo_acceptance_inclusive_minthreshold.root\",\"${BASEDIR}/geo_acceptance_inclusive_maxthreshold.root\",\"${BASEDIR}/geo_acceptance_inclusive_syserr.root\",\"Lambda0\",\"K_S0\",\"\(\#Lambda+\#bar\{\#Lambda\}\)\",\"K_{S}^{0}\"\)

root -l -q -b get_geoAcceptance_syserr.C\(\"${BASEDIR}/geo_acceptance_lambdaonly.root\",\"${BASEDIR}/geo_acceptance_lambdaonly_minthreshold.root\",\"${BASEDIR}/geo_acceptance_lambdaonly_maxthreshold.root\",\"${BASEDIR}/geo_acceptance_lambdaonly_syserr.root\",\"Lambda0\",\"K_S0\",\"\#Lambda\",\"K_{S}^{0}\"\)

root -l -q -b get_geoAcceptance_syserr.C\(\"${BASEDIR}/geo_acceptance_lambdabaronly.root\",\"${BASEDIR}/geo_acceptance_lambdabaronly_minthreshold.root\",\"${BASEDIR}/geo_acceptance_lambdabaronly_maxthreshold.root\",\"${BASEDIR}/geo_acceptance_lambdabaronly_syserr.root\",\"Lambda0\",\"K_S0\",\"\#bar\{\#Lambda\}\",\"K_{S}^{0}\"\)
