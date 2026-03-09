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
NAME="forTrackCutStudy_newMbHijingFiles"

# CreateFileList.pl args
RUN=6
TYPE=6
PILE=1
FILE="G4Hits"

# construct list prefix
summ="run"$RUN"typ"$TYPE"pil"$PILE
pref=$NAME"_"$summ

# run script
CreateFileList.pl -run $RUN -type $TYPE -pileup $PILE $FILE

for list in *.list; do
  new=$pref"_"$list
  mv $list $new
done

# end -------------------------------------------------------------------------
