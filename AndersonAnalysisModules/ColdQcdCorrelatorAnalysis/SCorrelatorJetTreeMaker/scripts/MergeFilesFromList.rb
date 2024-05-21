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
in_list  = "goodFiles.forCheckingTuples.run0.list"
out_file = "correlatorJetTree.pp200py8jet10run8_openCutsWithTrkTupleQA_goodFiles_chunk0.d11m10y2023.root"

# count how many files there are
num_files = 0
File.open(in_list) { |list|
  num_files = list.read.count("\n")
}

# merge files
exec("root -b -q \'MergeFiles.C(#{num_files}, \"#{in_list}\", \"#{out_file}\")\'")

# end -------------------------------------------------------------------------
