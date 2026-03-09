#!/usr/bin/env ruby
# -----------------------------------------------------------------------------
# \file   RunENCCalculation.rb
# \author Derek Anderson
# \date   01.18.2024
#
# Short script to run RunENCCalculation.cxx
# -----------------------------------------------------------------------------

if ARGV[0] == "condor"
  exec("condor_submit RunENCCalculationOnCondor.job")
else
  exec("root -b -q RunENCCalculation.cxx")
end

# end -------------------------------------------------------------------------
