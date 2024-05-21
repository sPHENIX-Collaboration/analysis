#!/usr/bin/env ruby
# -----------------------------------------------------------------------------
# 'HAddFilesFromList.rb'
# Derek Anderson
# 10.11.2023
#
# For merging files from a specific list via hadd.
# -----------------------------------------------------------------------------

# modules to use
require 'fileutils'

# i/o parameters
in_list  = "goodFiles.forCheckingTuples_pp200py8jet10run8_openCutsWithTrkTupleQA.run8.list"
out_file = "correlatorJetTree.pp200py8jet10run8_openCutsWithTrkTupleQA_goodFiles_chunk8.d12m10y2023.root"

# count how many files there are
num_files = 0
File.open(in_list) { |list|
  num_files = list.read.count("\n")
}

# flatten list into hadd argument
arg_input = ""
File.readlines(in_list).each_with_index do |line, iLine|
  line.gsub!("\n", "")
  arg_input += line
  arg_input += " " if iLine + 1 != num_files
end

# run hadd
exec("hadd #{out_file} #{arg_input}")

# end -------------------------------------------------------------------------
