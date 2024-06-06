#!/usr/bin/env ruby
# -----------------------------------------------------------------------------
# 'MergeCondorFiles.rb'
# Derek Anderson
# 04.22.2023
#
# For merging files in chunks using 'hadd_files.C'.
# (Ideal for condor output!)
# -----------------------------------------------------------------------------

# modules to use
require 'fileutils'

# input parameters
in_path = "./condor/individual_files/pp200py8jet10run6_trksAndChargPars_2023may7/"
in_pref = "outputData_CorrelatorJetTree_"
in_suff = ".root"

# output parameters
out_pref = "correlatorJetTree.pp200py8jet10run6_trksAndChrgPars_"
out_suff = ".d7m5y2023"
out_root = ".root"
out_list = ".list"

# chunks to copy
chunks = [
  "000",
  "001",
  "002",
  "003",
  "004",
  "005",
  "006",
  "007",
  "008",
  "009",
  "010",
  "011",
  "012",
  "013",
  "014",
  "015",
  "016",
  "017",
  "018",
  "019"
]

# chunk labels
labels = [
  "0000to0099",
  "0100to0199",
  "0200to0299",
  "0300to0399",
  "0400to0499",
  "0500to0599",
  "0600to0699",
  "0700to0799",
  "0800to0899",
  "0900to0999",
  "1000to1099",
  "1100to1199",
  "1200to1299",
  "1300to1399",
  "1400to1499",
  "1500to1599",
  "1600to1699",
  "1700to1799",
  "1800to1899",
  "1900to1999"
]

# create output arrays
num_chunks = chunks.size
in_pattern = Array.new(num_chunks)
list_files = Array.new(num_chunks)
root_files = Array.new(num_chunks)

# loop over chunks
chunks.each_with_index do |chunk, iChunk|

  # create input matching pattern
  in_pattern[iChunk] = in_path + "/" + in_pref + chunk + "*" + in_suff
  in_pattern[iChunk].gsub!("//", "/")
  in_pattern[iChunk].gsub!("..", ".")

  # create output list file
  list_files[iChunk] = out_pref + labels[iChunk] + out_suff + out_list
  list_files[iChunk].gsub!("//", "/")
  list_files[iChunk].gsub!("..", ".")

  # create output root file
  root_files[iChunk] = out_pref + labels[iChunk] + out_suff + out_root
  root_files[iChunk].gsub!("//", "/")
  root_files[iChunk].gsub!("..", ".")

  # create list of files in a chunk
  File.open(list_files[iChunk], "w") { |list|
    Dir[in_pattern[iChunk]].each do |file|
      list.puts file
    end
  }

  # grab number of files to merge
  num_files = Dir[in_pattern[iChunk]].size

  # merge files
  system("root -b -q \'MergeFiles.C(#{num_files}, \"#{list_files[iChunk]}\", \"#{root_files[iChunk]}\")\'")

# end chunk loop
end

# end -------------------------------------------------------------------------
