#!/bin/bash

PROCESS=$1

LAMBDA_FLAVOR=3122
KS_FLAVOR=310

LAMBDA_DAUGHTERS=\{-211,2212\}
KS_DAUGHTERS=\{211,-211\}

LAMBDA_MASS=1.115
KS_MASS=0.497

LAMBDA_FILEBASE=/sphenix/tg/tg01/hf/mjpeters/lambdaKshortMB/lambdaKshort_20260422_DetroitMB_CR_2_mode_pTref_1p4/evaluator/outputeval
KS_FILEBASE=/sphenix/tg/tg01/hf/mjpeters/lambdaKshortMB/lambdaKshort_20260422_DetroitMB_CR_2_mode_pTref_1p4/evaluator/outputeval

for i in {0..0}
do
	root -l -q -b calculate_truth_ratio.C\($LAMBDA_FLAVOR,$LAMBDA_MASS,$LAMBDA_DAUGHTERS,$KS_FLAVOR,$KS_MASS,$KS_DAUGHTERS,\"\#Lambda\",\"K^{0}_{S}\",\"$LAMBDA_FILEBASE\",\"$KS_FILEBASE\",1.,\"output/truth_ratio_pos\",$((10*PROCESS+$i)),false\)
done
