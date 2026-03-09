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
in_path = "./individual_files/run6n100pt2040pim_forPtCheck"
in_pref = "sPhenixG4_run6n100pt2040pim_embedScanOnHighPtCheck_embedOnly"
in_suff = "_g4svtx_eval.root"

# output parameters
out_pref = "sPhenixG4_forPtCheck_embedOnly"
out_suff = "_g4svtxeval.run6n100pt2040pim.d8m5y2023"
out_root = ".root"
out_list = ".list"

# chunks to copy
chunks = [
  "000",
  "001",
  "002",
  "003",
  "004",
]

# chunk labels
labels = [
  "0000s",
  "0100s",
  "0200s",
  "0300s",
  "0400s",
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
