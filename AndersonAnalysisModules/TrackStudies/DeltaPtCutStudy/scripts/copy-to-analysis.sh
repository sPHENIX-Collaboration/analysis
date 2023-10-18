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
copy_from="/sphenix/user/danderson/tracking/DeltaPtCutStudy"
copy_to="/sphenix/user/danderson/analysis/AndersonAnalysisModules/TrackStudies/DeltaPtCutStudy"

# what files to copy
files_to_copy[0]="README.md"
files_to_copy[1]="DoDeltaPtCutStudy.C"
files_to_copy[2]="scripts/wipe-source.sh"
files_to_copy[3]="scripts/copy-to-analysis.sh"
files_to_copy[4]="src/SDeltaPtCutStudy.cc"
files_to_copy[5]="src/SDeltaPtCutStudy.h"
files_to_copy[6]="src/SDeltaPtCutStudy.io.h"
files_to_copy[7]="src/SDeltaPtCutStudy.sys.h"
files_to_copy[8]="src/SDeltaPtCutStudy.ana.h"
files_to_copy[9]="src/SDeltaPtCutStudy.plot.h"
files_to_copy[10]="src/SDeltaPtCutStudyLinkDef.h"
files_to_copy[11]="src/autogen.sh"
files_to_copy[12]="src/configure.ac"
files_to_copy[13]="src/Makefile.am"
files_to_copy[14]="src/sphx-build"

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
