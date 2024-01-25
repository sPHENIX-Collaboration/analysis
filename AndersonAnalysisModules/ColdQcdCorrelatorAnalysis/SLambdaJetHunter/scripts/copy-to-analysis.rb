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
copy_from = "/sphenix/user/danderson/eec/SLambdaJetHunter"
copy_to   = "/sphenix/user/danderson/sphenix/analysis/AndersonAnalysisModules/ColdQcdCorrelatorAnalysis/SLambdaJetHunter"

# what files to copy
to_copy = [
  "README.md",
  "Fun4All_RunLambdaJetHunter.C",
  "LambdaJetHunterOptions.h",
  "RunLamdbdaJetHunter.rb",
  "scripts/copy-to-analysis.rb",
  "scripts/wipe-source.sh",
  "src/SLambdaJetHunter.cc",
  "src/SLambdaJetHunter.h",
  "src/SLambdaJetHunter.ana.h",
  "src/SLambdaJetHunter.sys.h",
  "src/SLambdaJetHunterConfig.h",
  "src/SLambdaJetHunterLinkDef.h",
  "src/autogen.sh",
  "src/configure.ac",
  "src/Makefile.am",
  "src/sphx-build"
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
