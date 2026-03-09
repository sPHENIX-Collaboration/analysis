#!/usr/bin/env ruby
# -----------------------------------------------------------------------------
# 'SplitFileLists.rb'
# Derek Anderson
# 09.11.2023
#
# Use to split input file lists into chunks
# of equal(ish) size.
# -----------------------------------------------------------------------------

# modules to use
require 'fileutils'

# i/o parameters
in_lists = [
  "goodFiles.forCheckingTuples_pp200py8jet10run8_openCutsWithTrkTupleQA.list"
]
in_dir  = "./"
out_dir = "./"

# no. of files per output lists
num_file_per_list = 100

# loop over input files
out_lists = Array.new(in_lists.size) { Array.new }
in_lists.each_with_index do |list, num_list|

  # loop over lines in file
  num_chunk = 0
  num_lines = 0
  File.readlines(in_dir + list).each do |line|

    # if starting new chunk, create output list
    if num_lines == 0
      out_lists[num_list][num_chunk] = list.clone
      out_lists[num_list][num_chunk].gsub!(".list", ".run#{num_chunk}.list")
      FileUtils.touch out_dir + out_lists[num_list][num_chunk]
    end

    # write out line
    File.open(out_dir + out_lists[num_list][num_chunk], 'a+') do |file|
      file.puts(line)
    end
    num_lines += 1

    # if exceeded no. of files per job, start new chunk
    if num_lines == num_file_per_list
      num_lines  = 0
      num_chunk += 1
    end
  end  # end input line loop
end  # end input file loop

# end -------------------------------------------------------------------------
