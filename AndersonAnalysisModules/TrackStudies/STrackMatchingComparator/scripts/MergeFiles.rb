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
in_path = "./new_matcher/num10evt500pt020pipm"
in_pref = "sPhenixG4_testingNewTruthMatcher_"
in_suff = ".num10evt500pt020pipm.d12m9y2023_g4trackmatching.root"
#in_suff = ".et020n5pipm.d11m8y2023_g4trackmatching.root"

# output parameters
out_list = "sPhenixG4_testingNewMatcher_newMatcher.pt020num10evt500pipm.d15m9y2023.list"
out_file = "sPhenixG4_testingNewMatcher_newMatcher.pt020num10evt500pipm.d15m9y2023.root"

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
