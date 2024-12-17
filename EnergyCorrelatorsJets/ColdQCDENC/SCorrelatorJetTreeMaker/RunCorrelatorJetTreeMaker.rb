#!/usr/bin/env ruby
# -----------------------------------------------------------------------------
# @file   RunCorrelatorJetTreeMaker.rb'
# @author Derek Anderson
# @date   04.11.2024
#
# Short script to run the 'Fun4All_RunCorrelatorJetTreeMaker.C' macro.
# -----------------------------------------------------------------------------

if ARGV[0] == "HIJetReco"
  exec("root -b -q Fun4All_RunJetTreeMakerOnHIJetReco.C")
else
  exec("root -b -q Fun4All_RunCorrelatorJetTreeMaker.C")
end

# end -------------------------------------------------------------------------
