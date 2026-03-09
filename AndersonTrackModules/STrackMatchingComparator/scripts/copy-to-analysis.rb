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
copy_from = "/sphenix/user/danderson/tracking/TrackMatchingComparison"
copy_to   = "/sphenix/user/danderson/sphenix/analysis/AndersonAnalysisModules/TrackStudies/STrackMatchingComparator"

# what files to copy
to_copy = [
  "README.md",
  "RunMatcherComparator.C",
  "MatcherComparatorOptions.h",
  "RunMatcherComparatorOnCondor.job",
  "RunMatcherComparatorOnCondor.sh",
  "RunMatcherComparator.rb",
  "src/STrackMatcherComparator.cc",
  "src/STrackMatcherComparator.h",
  "src/STrackMatcherComparatorConfig.h",
  "src/STrackMatcherComparatorHistDef.h",
  "src/STrackMatcherComparatorHistContent.h",
  "src/sphx-build",
  "src/autogen.sh",
  "src/Makefile.am",
  "src/configure.ac",
  "macros/MakeNewMatcherPlots.cxx",
  "macros/MakeNewMatcherTuplePlots.cxx",
  "macros/MakeOldEvaluatorPlots.cxx",
  "macros/CompareOldVsNewPlots.cxx",
  "macros/Fun4All_RunNewTruthMatcher.C",
  "macros/G4Setup_sPHENIX.C",
  "macros/QuickTreePlotter.C",
  "macros/QuickTuplePlotter.C",
  "macros/MergeFiles.C",
  "scripts/copy-to-analysis.rb",
  "scripts/RunNewTruthMatcherOnCondor.sh",
  "scripts/RunNewTruthMatcherOnCondor.job",
  "scripts/HAddFilesFromPattern.rb",
  "scripts/MergeCondorFiles.rb",
  "scripts/MergeFiles.rb"
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
