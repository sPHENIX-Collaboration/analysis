#!/usr/bin/env ruby
# -----------------------------------------------------------------------------
# @file   MakeCorrelatorResponseStandalone.rb
# @author Derek Anderson
# @date   05.16.2024
#
# Short script to run the 'MakeCorrelatorResponseStandalone.cxx' macro.
# -----------------------------------------------------------------------------

if ARGV[0] == "condor"
  exec("condor_submit MakeCorrelatorResponseStandalone.job")
else
  exec("root -b -q MakeCorrelatorResponseStandalone.cxx")
end

# end -------------------------------------------------------------------------

