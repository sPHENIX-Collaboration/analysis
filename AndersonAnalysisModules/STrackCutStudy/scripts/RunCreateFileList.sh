#!/bin/bash
# -----------------------------------------------------------------------------
# 'RunCreateFileList.sh'
# Derek Anderson
# 11.10.2022
#
# Quick script to run CreateFileList.pl
# with specified arguments.
# -----------------------------------------------------------------------------

# output parameter
NAME="forTrackCutStudy_withPileup"

# CreateFileList.pl args
TYPE=6
PILE=1
FILE="G4Hits DST_TRUTH_G4HIT"
SEG=300

# construct list prefix
summ="typ"$TYPE"pil"$PILE"seg"$SEG
pref=$NAME"_"$summ

# run script
CreateFileList.pl -type $TYPE -pileup $PILE $FILE -s $SEG

for list in *.list; do
  new=$pref"_"$list
  mv $list $new
done

# end -------------------------------------------------------------------------
