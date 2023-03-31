#!/bin/bash
# -----------------------------------------------------------------------------
# 'MergeFiles.sh'
# Derek Anderson
# 09.25.2020
#
# For merging files using 'hadd_files.C'
# -----------------------------------------------------------------------------

# i/o parameters
path="."
pref="sPhenixG4_test"
suff="_g4svtx_eval.root"
list="sPhenixG4_forTrackCutStudy_test00000t19_g4svtxeval.d14m11y2022.list"
root="sPhenixG4_forTrackCutStudy_test00000t19_g4svtxeval.d14m11y2022.root"

# create list of files
nFiles=$(ls -1 $path/$pref*$suff | wc -l)
ls $path/$pref*$suff > $list

# merge files
root -b -q "MergeFiles.C($nFiles, \"$list\", \"$root\")"

# end -------------------------------------------------------------------------
