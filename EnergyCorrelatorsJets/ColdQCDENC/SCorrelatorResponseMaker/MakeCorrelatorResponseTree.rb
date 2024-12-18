#!/usr/bin/env ruby
# -----------------------------------------------------------------------------
# @file   MakeCorrelatorResponseTree.rb
# @author Derek Anderson
# @date   05.16.2024
#
# Short script to run the 'MakeCorrelatorResponseTree.cxx' macro.
# -----------------------------------------------------------------------------

if ARGV[0] == "condor"
  exec("condor_submit MakeCorrelatorResponseTree.job")
else
  exec("root -b -q MakeCorrelatorResponseTree.cxx")
end

# end -------------------------------------------------------------------------

