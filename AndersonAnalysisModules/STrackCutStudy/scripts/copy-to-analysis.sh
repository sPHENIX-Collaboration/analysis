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
files_to_copy[0]="README.md"
files_to_copy[1]="DoTrackCutStudy.C"
files_to_copy[2]="DoFastTrackCutStudy.C"
files_to_copy[3]="DoFastTrackCutStudy.sh"
files_to_copy[4]="macros/QuickTuplePlotter.C"
files_to_copy[5]="macros/QuickDeltaPtExtractor.C"
files_to_copy[6]="macros/Fun4All_G4_sPHENIX_ForTrackCutStudy.C"
files_to_copy[7]="macros/Fun4All_G4_sPHENIX_ForTrackCutStudy_EmbedScanOff.C"
files_to_copy[8]="macros/G4Setup_sPHENIX.C"
files_to_copy[9]="macros/MergeFiles.C"
files_to_copy[10]="scripts/MergeFiles.rb"
files_to_copy[11]="scripts/MergeCondorFiles.rb"
files_to_copy[12]="scripts/MergeFilesFromList.rb"
files_to_copy[13]="scripts/RunCreateFileList.sh"
files_to_copy[14]="scripts/RunSingleTrackCutStudyG4.sh"
files_to_copy[15]="scripts/RunSingleTrackCutStudyWithPileupG4.sh"
files_to_copy[16]="scripts/RunTrackCutStudyG4.job"
files_to_copy[17]="scripts/RunTrackCutStudyG4.sh"
files_to_copy[18]="scripts/RunTrackCutStudyWithPileupG4.job"
files_to_copy[19]="scripts/wipe-source.sh"
files_to_copy[20]="scripts/copy-to-analysis.sh"
files_to_copy[21]="src/STrackCutStudy.cc"
files_to_copy[22]="src/STrackCutStudy.io.h"
files_to_copy[23]="src/STrackCutStudy.ana.h"
files_to_copy[24]="src/STrackCutStudy.hist.h"
files_to_copy[25]="src/STrackCutStudy.plot.h"
files_to_copy[26]="src/STrackCutStudy.h"
files_to_copy[27]="src/autogen.sh"
files_to_copy[28]="src/configure.ac"
files_to_copy[29]="src/Makefile.am"

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
