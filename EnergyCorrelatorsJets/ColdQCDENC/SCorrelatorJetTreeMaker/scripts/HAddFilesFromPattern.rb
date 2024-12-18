#!/usr/bin/env ruby
# -----------------------------------------------------------------------------
# 'HAddFilesFromPattern.rb'
# Derek Anderson
# 10.11.2023
#
# Runs hadd over set of files matching a certain pattern...
# -----------------------------------------------------------------------------

# modules to use
require 'fileutils'

# input parameters
in_path = "./condor/intermediate_merge/pp200py8jet10run8_openCutsWithTrkTupleQA_2023oct12"
in_pref = "correlatorJetTree.pp200py8jet10run8_openCutsWithTrkTupleQA_goodFiles_"
in_suff = ".d12m10y2023.root"

# output parameters
out_file = "correlatorJetTree.pp200py8jet10run8_openCutsWithTrkTupleQA.d12m10y2023.root"

# create input matching pattern
in_pattern = in_path + "/" + in_pref + "*" + in_suff
in_pattern.gsub!("//", "/")
in_pattern.gsub!("..", ".")

# create input argument
arg_input = ""
num_input = Dir[in_pattern].size
Dir[in_pattern].each_with_index do |file, iFile|
  arg_input += file
  arg_input += " " if iFile + 1 != num_input
end

# run hadd
exec("hadd #{out_file} #{arg_input}")

# end -------------------------------------------------------------------------
