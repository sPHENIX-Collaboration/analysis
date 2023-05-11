#!/bin/bash

INPUTLIST=filelist_lq_20x250_3pion.txt

FILENAME=LeptoAna_r05_p250_e20.csv

FILENAME_TMP=LeptoAna_r05_p250_e20_temp.txt

# write trees to .csv file
root -b -q tree2csv.C\(\"$INPUTLIST\",\"$FILENAME_TMP\"\)

#cat $FILENAME | awk '{print $2 "," $4 "," $6 "," $8 "," $10 "," $12 "," $14}' | grep -v ',,' > $FILENAME_CLEAN

# clean file- replace '*' with ',', remove line separator, remove whitespaces
cat $FILENAME_TMP | \
    tr -d "[:blank:]" | \
    tr "*" "," | \
    sed 's,.\(.*\).$,\1,g' | \
    grep -v ',,' > $FILENAME

# cleanup temporary file
rm $FILENAME_TMP
