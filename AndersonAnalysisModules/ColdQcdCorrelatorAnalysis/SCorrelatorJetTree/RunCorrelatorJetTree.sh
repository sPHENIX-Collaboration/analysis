#!/bin/bash
# -----------------------------------------------------------------------------
# 'RunCorrelatorJetTree.sh'
# Derek Anderson
# 01.03.2022
#
# Short to script to run the macro
# 'Fun4All_ForCorrelatorJetTree.C'
# over a (small) set of files.
# -----------------------------------------------------------------------------

# declare i/o lists
declare -a inHitsFiles
declare -a inCaloFiles
declare -a inSeedFiles
declare -a inTrksFiles
declare -a inTrueFiles
declare -a outputFiles

# input g4 hits files
inHitsFiles[0]="\"/sphenix/lustre01/sphnxpro/mdc2/pythia8_pp_mb/g4hits/G4Hits_pythia8_pp_mb-0000000050-00000.root\""
inHitsFiles[1]="\"/sphenix/lustre01/sphnxpro/mdc2/pythia8_pp_mb/g4hits/G4Hits_pythia8_pp_mb-0000000050-00001.root\""
inHitsFiles[2]="\"/sphenix/lustre01/sphnxpro/mdc2/pythia8_pp_mb/g4hits/G4Hits_pythia8_pp_mb-0000000050-00002.root\""
inHitsFiles[3]="\"/sphenix/lustre01/sphnxpro/mdc2/pythia8_pp_mb/g4hits/G4Hits_pythia8_pp_mb-0000000050-00003.root\""
inHitsFiles[4]="\"/sphenix/lustre01/sphnxpro/mdc2/pythia8_pp_mb/g4hits/G4Hits_pythia8_pp_mb-0000000050-00004.root\""
inHitsFiles[5]="\"/sphenix/lustre01/sphnxpro/mdc2/pythia8_pp_mb/g4hits/G4Hits_pythia8_pp_mb-0000000050-00005.root\""
inHitsFiles[6]="\"/sphenix/lustre01/sphnxpro/mdc2/pythia8_pp_mb/g4hits/G4Hits_pythia8_pp_mb-0000000050-00006.root\""
inHitsFiles[7]="\"/sphenix/lustre01/sphnxpro/mdc2/pythia8_pp_mb/g4hits/G4Hits_pythia8_pp_mb-0000000050-00007.root\""
inHitsFiles[8]="\"/sphenix/lustre01/sphnxpro/mdc2/pythia8_pp_mb/g4hits/G4Hits_pythia8_pp_mb-0000000050-00008.root\""
inHitsFiles[9]="\"/sphenix/lustre01/sphnxpro/mdc2/pythia8_pp_mb/g4hits/G4Hits_pythia8_pp_mb-0000000050-00009.root\""

# input calorimeter files
inCaloFiles[0]="\"/sphenix/lustre01/sphnxpro/mdc2/pythia8_pp_mb/calocluster/DST_CALO_CLUSTER_pythia8_pp_mb_3MHz-0000000050-00000.root\""
inCaloFiles[1]="\"/sphenix/lustre01/sphnxpro/mdc2/pythia8_pp_mb/calocluster/DST_CALO_CLUSTER_pythia8_pp_mb_3MHz-0000000050-00001.root\""
inCaloFiles[2]="\"/sphenix/lustre01/sphnxpro/mdc2/pythia8_pp_mb/calocluster/DST_CALO_CLUSTER_pythia8_pp_mb_3MHz-0000000050-00002.root\""
inCaloFiles[3]="\"/sphenix/lustre01/sphnxpro/mdc2/pythia8_pp_mb/calocluster/DST_CALO_CLUSTER_pythia8_pp_mb_3MHz-0000000050-00003.root\""
inCaloFiles[4]="\"/sphenix/lustre01/sphnxpro/mdc2/pythia8_pp_mb/calocluster/DST_CALO_CLUSTER_pythia8_pp_mb_3MHz-0000000050-00004.root\""
inCaloFiles[5]="\"/sphenix/lustre01/sphnxpro/mdc2/pythia8_pp_mb/calocluster/DST_CALO_CLUSTER_pythia8_pp_mb_3MHz-0000000050-00005.root\""
inCaloFiles[6]="\"/sphenix/lustre01/sphnxpro/mdc2/pythia8_pp_mb/calocluster/DST_CALO_CLUSTER_pythia8_pp_mb_3MHz-0000000050-00006.root\""
inCaloFiles[7]="\"/sphenix/lustre01/sphnxpro/mdc2/pythia8_pp_mb/calocluster/DST_CALO_CLUSTER_pythia8_pp_mb_3MHz-0000000050-00007.root\""
inCaloFiles[8]="\"/sphenix/lustre01/sphnxpro/mdc2/pythia8_pp_mb/calocluster/DST_CALO_CLUSTER_pythia8_pp_mb_3MHz-0000000050-00008.root\""
inCaloFiles[9]="\"/sphenix/lustre01/sphnxpro/mdc2/pythia8_pp_mb/calocluster/DST_CALO_CLUSTER_pythia8_pp_mb_3MHz-0000000050-00009.root\""

# input track seed files
inSeedFiles[0]="\"/sphenix/lustre01/sphnxpro/mdc2/pythia8_pp_mb/trackseeds/DST_TRACKSEEDS_pythia8_pp_mb_3MHz-0000000050-00000.root\""
inSeedFiles[1]="\"/sphenix/lustre01/sphnxpro/mdc2/pythia8_pp_mb/trackseeds/DST_TRACKSEEDS_pythia8_pp_mb_3MHz-0000000050-00001.root\""
inSeedFiles[2]="\"/sphenix/lustre01/sphnxpro/mdc2/pythia8_pp_mb/trackseeds/DST_TRACKSEEDS_pythia8_pp_mb_3MHz-0000000050-00002.root\""
inSeedFiles[3]="\"/sphenix/lustre01/sphnxpro/mdc2/pythia8_pp_mb/trackseeds/DST_TRACKSEEDS_pythia8_pp_mb_3MHz-0000000050-00003.root\""
inSeedFiles[4]="\"/sphenix/lustre01/sphnxpro/mdc2/pythia8_pp_mb/trackseeds/DST_TRACKSEEDS_pythia8_pp_mb_3MHz-0000000050-00004.root\""
inSeedFiles[5]="\"/sphenix/lustre01/sphnxpro/mdc2/pythia8_pp_mb/trackseeds/DST_TRACKSEEDS_pythia8_pp_mb_3MHz-0000000050-00005.root\""
inSeedFiles[6]="\"/sphenix/lustre01/sphnxpro/mdc2/pythia8_pp_mb/trackseeds/DST_TRACKSEEDS_pythia8_pp_mb_3MHz-0000000050-00006.root\""
inSeedFiles[7]="\"/sphenix/lustre01/sphnxpro/mdc2/pythia8_pp_mb/trackseeds/DST_TRACKSEEDS_pythia8_pp_mb_3MHz-0000000050-00007.root\""
inSeedFiles[8]="\"/sphenix/lustre01/sphnxpro/mdc2/pythia8_pp_mb/trackseeds/DST_TRACKSEEDS_pythia8_pp_mb_3MHz-0000000050-00008.root\""
inSeedFiles[9]="\"/sphenix/lustre01/sphnxpro/mdc2/pythia8_pp_mb/trackseeds/DST_TRACKSEEDS_pythia8_pp_mb_3MHz-0000000050-00009.root\""

# input track files
inTrksFiles[0]="\"/sphenix/lustre01/sphnxpro/mdc2/pythia8_pp_mb/tracks/DST_TRACKS_pythia8_pp_mb_3MHz-0000000050-00000.root\""
inTrksFiles[1]="\"/sphenix/lustre01/sphnxpro/mdc2/pythia8_pp_mb/tracks/DST_TRACKS_pythia8_pp_mb_3MHz-0000000050-00001.root\""
inTrksFiles[2]="\"/sphenix/lustre01/sphnxpro/mdc2/pythia8_pp_mb/tracks/DST_TRACKS_pythia8_pp_mb_3MHz-0000000050-00002.root\""
inTrksFiles[3]="\"/sphenix/lustre01/sphnxpro/mdc2/pythia8_pp_mb/tracks/DST_TRACKS_pythia8_pp_mb_3MHz-0000000050-00003.root\""
inTrksFiles[4]="\"/sphenix/lustre01/sphnxpro/mdc2/pythia8_pp_mb/tracks/DST_TRACKS_pythia8_pp_mb_3MHz-0000000050-00004.root\""
inTrksFiles[5]="\"/sphenix/lustre01/sphnxpro/mdc2/pythia8_pp_mb/tracks/DST_TRACKS_pythia8_pp_mb_3MHz-0000000050-00005.root\""
inTrksFiles[6]="\"/sphenix/lustre01/sphnxpro/mdc2/pythia8_pp_mb/tracks/DST_TRACKS_pythia8_pp_mb_3MHz-0000000050-00006.root\""
inTrksFiles[7]="\"/sphenix/lustre01/sphnxpro/mdc2/pythia8_pp_mb/tracks/DST_TRACKS_pythia8_pp_mb_3MHz-0000000050-00007.root\""
inTrksFiles[8]="\"/sphenix/lustre01/sphnxpro/mdc2/pythia8_pp_mb/tracks/DST_TRACKS_pythia8_pp_mb_3MHz-0000000050-00008.root\""
inTrksFiles[9]="\"/sphenix/lustre01/sphnxpro/mdc2/pythia8_pp_mb/tracks/DST_TRACKS_pythia8_pp_mb_3MHz-0000000050-00009.root\""

# input truth files
inTrueFiles[0]="\"/sphenix/lustre01/sphnxpro/mdc2/pythia8_pp_mb/trkrhit/DST_TRUTH_pythia8_pp_mb_3MHz-0000000050-00000.root\""
inTrueFiles[1]="\"/sphenix/lustre01/sphnxpro/mdc2/pythia8_pp_mb/trkrhit/DST_TRUTH_pythia8_pp_mb_3MHz-0000000050-00001.root\""
inTrueFiles[2]="\"/sphenix/lustre01/sphnxpro/mdc2/pythia8_pp_mb/trkrhit/DST_TRUTH_pythia8_pp_mb_3MHz-0000000050-00002.root\""
inTrueFiles[3]="\"/sphenix/lustre01/sphnxpro/mdc2/pythia8_pp_mb/trkrhit/DST_TRUTH_pythia8_pp_mb_3MHz-0000000050-00003.root\""
inTrueFiles[4]="\"/sphenix/lustre01/sphnxpro/mdc2/pythia8_pp_mb/trkrhit/DST_TRUTH_pythia8_pp_mb_3MHz-0000000050-00004.root\""
inTrueFiles[5]="\"/sphenix/lustre01/sphnxpro/mdc2/pythia8_pp_mb/trkrhit/DST_TRUTH_pythia8_pp_mb_3MHz-0000000050-00005.root\""
inTrueFiles[6]="\"/sphenix/lustre01/sphnxpro/mdc2/pythia8_pp_mb/trkrhit/DST_TRUTH_pythia8_pp_mb_3MHz-0000000050-00006.root\""
inTrueFiles[7]="\"/sphenix/lustre01/sphnxpro/mdc2/pythia8_pp_mb/trkrhit/DST_TRUTH_pythia8_pp_mb_3MHz-0000000050-00007.root\""
inTrueFiles[8]="\"/sphenix/lustre01/sphnxpro/mdc2/pythia8_pp_mb/trkrhit/DST_TRUTH_pythia8_pp_mb_3MHz-0000000050-00008.root\""
inTrueFiles[9]="\"/sphenix/lustre01/sphnxpro/mdc2/pythia8_pp_mb/trkrhit/DST_TRUTH_pythia8_pp_mb_3MHz-0000000050-00009.root\""

# output files
outputFiles[0]="\"output/individual/correlatorJetTree.Pythia8_ppMB50_00000.root\""
outputFiles[1]="\"output/individual/correlatorJetTree.Pythia8_ppMB50_00001.root\""
outputFiles[2]="\"output/individual/correlatorJetTree.Pythia8_ppMB50_00002.root\""
outputFiles[3]="\"output/individual/correlatorJetTree.Pythia8_ppMB50_00003.root\""
outputFiles[4]="\"output/individual/correlatorJetTree.Pythia8_ppMB50_00004.root\""
outputFiles[5]="\"output/individual/correlatorJetTree.Pythia8_ppMB50_00005.root\""
outputFiles[6]="\"output/individual/correlatorJetTree.Pythia8_ppMB50_00006.root\""
outputFiles[7]="\"output/individual/correlatorJetTree.Pythia8_ppMB50_00007.root\""
outputFiles[8]="\"output/individual/correlatorJetTree.Pythia8_ppMB50_00008.root\""
outputFiles[9]="\"output/individual/correlatorJetTree.Pythia8_ppMB50_00009.root\""

# other parameters
nEvt=100
verb=0

# loop over files
(( nFile=0 ))
for input in ${inHitsFiles[@]}; do
  root -b -q "Fun4All_RunCorrelatorJetTree.C($input, ${inCaloFiles[$nFile]}, ${inSeedFiles[$nFile]}, ${inTrksFiles[$nFile]}, ${inTrueFiles[$nFile]}, ${outputFiles[$nFile]}, $nEvt, $verb)"
  (( nFile++ ))
done

# delete arrays
unset inHitsFiles
unset inCaloFiles
unset inSeedFiles
unset inTrksFiles
unset inTrueFiles
unset outputFiles

# end -------------------------------------------------------------------------
