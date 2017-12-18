#!/bin/bash

FILENAME=LeptoAna_r05_p250_e20.csv
FILENAME_CLEAN=LeptoAna_r05_p250_e20_clean.csv

root -b -q tree2csv.C

cat $FILENAME | awk '{print $2 "," $4 "," $6 "," $8 "," $10 "," $12 "," $14 "," $16}' | grep -v ',,' > $FILENAME_CLEAN