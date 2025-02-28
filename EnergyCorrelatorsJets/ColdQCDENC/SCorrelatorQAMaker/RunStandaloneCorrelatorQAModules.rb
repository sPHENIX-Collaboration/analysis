#!/usr/bin/env ruby
# -----------------------------------------------------------------------------
# 'RunStandaloneCorrelatorQAModules.rb'
# Derek Anderson
# 02.19.2024
#
# Short script to run 'RunStandaloneCorrelatorQAModules.C'.
# -----------------------------------------------------------------------------

if ARGV[0] == "condor"
  exec("condor_submit RunStandaloneCorrelatorQAModulesOnCondor.job")
else
  exec("root -b -q RunStandaloneCorrelatorQAModules.cxx")
end

# end -------------------------------------------------------------------------

