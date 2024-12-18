#!/usr/bin/env ruby
# -----------------------------------------------------------------------------
# 'copy-to-analysis.rb'
# Derek Anderson
# 12.21.2023
#
# Script to automate copying files over to
# a fork of the sPHENIX analysis repo.
# -----------------------------------------------------------------------------

# modules to use
require 'fileutils'

# top directory to copy from/to
copy_from = "/sphenix/user/danderson/tracking/TrackCutStudy"
copy_to   = "/sphenix/user/danderson/sphenix/analysis/AndersonAnalysisModules/TrackStudies/STrackCutStudy"

# what files to copy
to_copy = [
  "README.md",
  "DoTrackCutStudy.C",
  "DoFastTrackCutStudy.C",
  "DoFastTrackCutStudy.sh",
  "DoTrackCutStudyOnCondor.sh",
  "DoTrackCutStudyOnCondor.job",
  "SubmitTrackCutStudy.sh",
  "macros/QuickTuplePlotter.C",
  "macros/Fun4All_G4_sPHENIX_ForTrackCutStudy_EmbedScanOff.C",
  "macros/Fun4All_G4_sPHENIX_ForTrackCutStudy_EmbedScanOn.C",
  "macros/Fun4All_G4_sPHENIX_ForTrackCutStudy_WithPileup_EmbedScanOn.C",
  "macros/Fun4All_G4_sPHENIX_ForTrackCutStudy_WithPileup_EmbedScanOff.C",
  "macros/G4Setup_sPHENIX.C",
  "macros/MergeFiles.C",
  "scripts/MergeFiles.rb",
  "scripts/MergeCondorFiles.rb",
  "scripts/MergeFilesFromList.rb",
  "scripts/RunCreateFileList.sh",
  "scripts/RunSingleTrackCutStudyG4.sh",
  "scripts/RunSingleTrackCutStudyWithPileupG4.sh",
  "scripts/RunTrackCutStudyG4.job",
  "scripts/RunTrackCutStudyG4.sh",
  "scripts/RunTrackCutStudyWithPileupG4.job",
  "scripts/wipe-source.sh",
  "scripts/copy-to-analysis.rb",
  "src/STrackCutStudy.cc",
  "src/STrackCutStudy.io.h",
  "src/STrackCutStudy.ana.h",
  "src/STrackCutStudy.hist.h",
  "src/STrackCutStudy.plot.h",
  "src/STrackCutStudy.h",
  "src/autogen.sh",
  "src/configure.ac",
  "src/Makefile.am"
]

# do copying
to_copy.each do |file|

  # make directory in target if needed
  if file.include? "/"

    # grab relative path to file
    relative_path = file.clone
    relative_path.gsub!(copy_from, "")

    # clean up and isolate path
    relative_path.gsub!("//",  "/")
    relative_path.gsub!("/./", "/")
    relative_path.slice!(relative_path.rindex("/")..-1)

    # make directory
    to_make = copy_to + "/" + relative_path
    FileUtils.mkdir_p(to_make, :verbose => true) unless File.exists?(to_make)
  end

  # make source and target paths
  source = copy_from + "/" + file
  target = copy_to + "/" + file

  # remove any unwanted patterns
  source.gsub!("//",  "/")
  target.gsub!("//",  "/")
  source.gsub!("/./", "/")
  target.gsub!("/./", "/")

  # copy file
  FileUtils.cp_r(source, target, :verbose => true)
end

# end -------------------------------------------------------------------------
