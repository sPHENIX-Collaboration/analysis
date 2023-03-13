#!/bin/bash
# -----------------------------------------------------------------------------
# 'copy-to-analysis.sh'
# Derek Anderson
# 01.06.2023
#
# Script to automate copying files
# over to the sPHENIX analysis
# repository.
# -----------------------------------------------------------------------------

# declare filelist
declare -a files_to_copy

# top directory to copy from/to
copy_from="/sphenix/user/danderson/tracking/TrackCutStudy"
copy_to="/sphenix/user/danderson/analysis/AndersonAnalysisModules/STrackCutStudy"

# what files to copy
files_to_copy[0]="DoTrackCutStudy.C"
files_to_copy[1]="macros/Fun4All_G4_sPHENIX_ForTrackCutStudy.C"
files_to_copy[2]="macros/Fun4All_G4_sPHENIX_ForTrackCutStudy_EmbedScanOff.C"
files_to_copy[3]="macros/G4Setup_sPHENIX.C"
files_to_copy[4]="macros/MergeFiles.C"
files_to_copy[5]="scripts/MergeFiles.sh"
files_to_copy[6]="scripts/RunCreateFileList.sh"
files_to_copy[7]="scripts/RunSingleTrackCutStudyG4.sh"
files_to_copy[8]="scripts/RunSingleTrackCutStudyWithPileupG4.sh"
files_to_copy[9]="scripts/RunTrackCutStudyG4.job"
files_to_copy[10]="scripts/RunTrackCutStudyG4.sh"
files_to_copy[11]="scripts/RunTrackCutStudyWithPileupG4.job"
files_to_copy[12]="scripts/wipe-source.sh"
files_to_copy[13]="scripts/copy-to-analysis.sh"
files_to_copy[14]="src/STrackCutStudy.cc"
files_to_copy[15]="src/STrackCutStudy.h"
files_to_copy[16]="src/autogen.sh"
files_to_copy[17]="src/configure.ac"
files_to_copy[18]="src/Makefile.am"

# do copying
# TODO: automate detection/creation of sub-directories
(( nFile=0 ))
for file in ${files_to_copy[@]}; do
  source_file="$copy_from/$file"
  target_file="$copy_to/$file"
  rsync -azP $source_file $target_file
  (( nFile++ ))
done

# delete array
unset files_to_copy

# end -------------------------------------------------------------------------
