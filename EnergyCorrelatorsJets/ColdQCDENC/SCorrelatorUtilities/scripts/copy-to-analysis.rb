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
copy_from = "/sphenix/user/danderson/eec/SCorrelatorUtilities"
copy_to   = "/sphenix/user/danderson/sphenix/analysis/EnergyCorrelatorsJets/ColdQCDENC/SCorrelatorUtilities"

# what files to copy
to_copy = [
  "README.md",
  "src/ClustInfo.cc",
  "src/ClustInfo.h",
  "src/ClustInterfaces.cc",
  "src/ClustInterfaces.h",
  "src/ClustTools.cc",
  "src/ClustTools.h",
  "src/Constants.h",
  "src/CstInfo.cc",
  "src/CstInfo.h",
  "src/CstInterfaces.cc",
  "src/CstInterfaces.h",
  "src/FlowInfo.cc",
  "src/FlowInfo.h",
  "src/FlowInterfaces.cc",
  "src/FlowInterfaces.h",
  "src/GEvtInfo.cc",
  "src/GEvtInfo.h",
  "src/GEvtTools.cc",
  "src/GEvtTools.h",
  "src/Interfaces.h",
  "src/JetInfo.cc",
  "src/JetInfo.h",
  "src/NodeInterfaces.cc",
  "src/NodeInterfaces.h",
  "src/ParInfo.cc",
  "src/ParInfo.h",
  "src/ParInterfaces.cc",
  "src/ParInterfaces.h",
  "src/ParTools.cc",
  "src/ParTools.h",
  "src/REvtInfo.cc",
  "src/REvtInfo.h",
  "src/REvtTools.cc",
  "src/REvtTools.h",
  "src/TreeInterfaces.cc",
  "src/TreeInterfaces.h",
  "src/TrkInfo.cc",
  "src/TrkInfo.h",
  "src/TrkInterfaces.cc",
  "src/TrkInterfaces.h",
  "src/TrkTools.cc",
  "src/TrkTools.h",
  "src/TwrInfo.cc",
  "src/TwrInfo.h",
  "src/TwrInterfaces.cc",
  "src/TwrInterfaces.h",
  "src/TwrTools.cc",
  "src/TwrTools.h",
  "src/Tools.h",
  "src/Types.h",
  "src/TypesLinkDef.h",
  "src/TupleInterfaces.cc",
  "src/TupleInterfaces.h",
  "src/VtxInterfaces.cc",
  "src/VtxInterfaces.h",
  "scripts/copy-to-analysis.rb",
  "scripts/wipe-source.sh"
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
