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
copy_from = "/sphenix/user/danderson/eec/SCorrelatorJetTreeMaker"
copy_to   = "/sphenix/user/danderson/sphenix/analysis/AndersonAnalysisModules/ColdQcdCorrelatorAnalysis/SCorrelatorJetTreeMaker"

# what files to copy
to_copy = [
  "README.md",
  "Fun4All_RunCorrelatorJetTreeMaker.C",
  "JetTreeMakerOptions.h",
  "TopoClusterOptions.h",
  "RunCorrelatorJetTreeMaker.rb",
  "macros/MergeFiles.C",
  "macros/Fun4All_RunCorrelatorJetTreeMakerOnCondor.C",
  "scripts/MergeFiles.rb",
  "scripts/MergeCondorFiles.rb",
  "scripts/SwitchToCorrectBuild.sh",
  "scripts/wipe-source.sh",
  "scripts/copy-to-analysis.rb",
  "scripts/makeCondorJobs.py",
  "scripts/RunCorrelatorJetTreeMakerOnCondor.sh",
  "src/SCorrelatorJetTreeMaker.cc",
  "src/SCorrelatorJetTreeMaker.h",
  "src/SCorrelatorJetTreeMaker.ana.h",
  "src/SCorrelatorJetTreeMaker.sys.h",
  "src/SCorrelatorJetTreeMakerConfig.h",
  "src/SCorrelatorJetTreeMakerOutput.h",
  "src/SCorrelatorJetTreeMakerLinkDef.h",
  "src/autogen.sh",
  "src/configure.ac",
  "src/Makefile.am",
  "src/sphx-build",
  "scripts/HAddFilesFromList.rb",
  "scripts/HAddFilesFromPattern.rb",
  "scripts/MergeFilesFromList.rb",
  "scripts/SplitFileLists.rb"
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
