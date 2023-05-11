#!/bin/zsh

# EIC settings
#XVAL=7.94e-05
#Q2VAL=1.33e+00
#YVAL=0.84
#NVAL=1.07e+04
#MLEPTON=0.000511
#
#root -b -q calculate_g1.C\($XVAL,$Q2VAL,$YVAL,$NVAL,$MLEPTON\)



# SMC settings
XVAL=0.0144
Q2VAL=3.58
YVAL=0.705
NVAL=1e6
MLEPTON=0.105

root -b -q calculate_g1.C\($XVAL,$Q2VAL,$YVAL,$NVAL,$MLEPTON\)

