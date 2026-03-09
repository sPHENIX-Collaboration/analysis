#!/usr/bin/env ruby
# -----------------------------------------------------------------------------
# 'RunTracksInJetsQAMaker.rb'
# Derek Anderson
# 04.13.2024
#
# Short script to run the 'Fun4All_MakeTracksInJetsQA.C' macro
# with default options.
# -----------------------------------------------------------------------------

if ARGV[0] == "local"
  exec("root -b -q Fun4All_MakeTrksInJetQA.C")
else
  exec("condor_submit RunTrksInJetQAOnCondor.job")
end

# end -------------------------------------------------------------------------
