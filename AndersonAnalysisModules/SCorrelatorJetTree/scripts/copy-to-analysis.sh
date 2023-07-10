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
copy_from="/sphenix/user/danderson/eec/SCorrelatorJetTree"
copy_to="/sphenix/user/danderson/analysis/AndersonAnalysisModules/SCorrelatorJetTree"

# what files to copy
files_to_copy[0]="README.md"
files_to_copy[1]="Fun4All_RunCorrelatorJetTree.C"
files_to_copy[2]="RunCorrelatorJetTree.sh"
files_to_copy[3]="macros/MergeFiles.C"
files_to_copy[4]="macros/Fun4All_RunCorrelatorJetTreeOnCondor.C"
files_to_copy[5]="scripts/MergeFiles.rb"
files_to_copy[6]="scripts/MergeCondorFiles.rb"
files_to_copy[7]="scripts/SwitchToCorrectBuild.sh"
files_to_copy[8]="scripts/wipe-source.sh"
files_to_copy[9]="scripts/copy-to-analysis.sh"
files_to_copy[10]="scripts/makeCondorJobs.py"
files_to_copy[11]="scripts/RunCorrelatorJetTreeOnCondor.sh"
files_to_copy[12]="src/SCorrelatorJetTree.cc"
files_to_copy[13]="src/SCorrelatorJetTree.h"
files_to_copy[14]="src/SCorrelatorJetTree.io.h"
files_to_copy[15]="src/SCorrelatorJetTree.jet.h"
files_to_copy[16]="src/SCorrelatorJetTree.sys.h"
files_to_copy[17]="src/SCorrelatorJetTreeLinkDef.h"
files_to_copy[18]="src/SCorrelatorJetTree.cst.h"
files_to_copy[19]="src/SCorrelatorJetTree.evt.h"
files_to_copy[20]="src/autogen.sh"
files_to_copy[21]="src/configure.ac"
files_to_copy[22]="src/Makefile.am"

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
