#!/usr/bin/env ruby
# -----------------------------------------------------------------------------
# 'MergeFiles.rb'
# Derek Anderson
# 04.22.2023
#
# For merging files using 'hadd_files.C'
# -----------------------------------------------------------------------------

# modules to use
require 'fileutils'

# input parameters
in_path = "./condor/intermediate_merge/pp200py8jet10run6_trksAndChargPars_2023may7"
in_pref = "correlatorJetTree.pp200py8jet10run6_trksAndChrgPars_"
in_suff = ".d7m5y2023.root"

# output parameters
out_list = "correlatorJetTree.pp200py8jet10run6_trksAndChrgPars.d7m5y2023.list"
out_file = "correlatorJetTree.pp200py8jet10run6_trksAndChrgPars.d7m5y2023.root"

# create input matching pattern
in_pattern = in_path + "/" + in_pref + "*" + in_suff
in_pattern.gsub!("//", "/")
in_pattern.gsub!("..", ".")

# create list of files to merge
File.open(out_list, "w") { |out|
  Dir[in_pattern].each do |file|
    out.puts file
  end
}

# grab number of files to merge
num_files = Dir[in_pattern].size

# merge files
exec("root -b -q \'MergeFiles.C(#{num_files}, \"#{out_list}\", \"#{out_file}\")\'")

# end -------------------------------------------------------------------------
