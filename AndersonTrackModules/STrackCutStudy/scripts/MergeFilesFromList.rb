#!/usr/bin/env ruby
# -----------------------------------------------------------------------------
# 'MergeFilesFromList.rb'
# Derek Anderson
# 05.08.2023
#
# For merging files from a specific list (e.g. you've
# prepared a list of good files and want to merge only
# those).
# -----------------------------------------------------------------------------

# modules to use
require 'fileutils'

# i/o parameters
in_list  = "checkingTrackTuples.goodFiles_embedOnly0200s.run6n100pt2040pim.d8m5y2023.list"
out_file = "sPhenixG4_forPtCheck_embedOnly0200s_goodFiles_g4svtxeval.run6n100pt2040pim.d8m5y2023.root"

# count how many files there are
num_files = 0
File.open(in_list) { |list|
  num_files = list.read.count("\n")
}

# merge files
exec("root -b -q \'MergeFiles.C(#{num_files}, \"#{in_list}\", \"#{out_file}\")\'")

# end -------------------------------------------------------------------------
