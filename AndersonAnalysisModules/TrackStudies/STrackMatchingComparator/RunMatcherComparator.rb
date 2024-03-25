#!/usr/bin/env ruby
# -----------------------------------------------------------------------------
# 'RunMatcherComparator.rb'
# Derek Anderson
# 01.30.2024
#
# short script to run RunMatcherComparator.C
# -----------------------------------------------------------------------------

if ARGV[0] == "condor"
  exec("condor_submit RunMatcherComparatorOnCondor.job")
else
  exec("root -b -q RunMatcherComparator.C")
end

# end -------------------------------------------------------------------------
