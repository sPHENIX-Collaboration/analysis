#!/bin/bash
# 'RunCorrelatorJetTree.sh'
# Derek Anderson
# 01.03.2022
#
# Short to script to run the macro
# 'Fun4All_ForCorrelatorJetTree.C'
# over a (small) set of files.

# declare i/o lists
declare -a inTrkrFiles
declare -a inCaloFiles
declare -a inTrueFiles
declare -a outputFiles

# input tracker files
inTrkrFiles[0]="\"/sphenix/lustre01/sphnxpro/mdc2/pythia8_pp_mb/g4hits/G4Hits_pythia8_pp_mb-0000000050-00000.root\""
inTrkrFiles[1]="\"/sphenix/lustre01/sphnxpro/mdc2/pythia8_pp_mb/g4hits/G4Hits_pythia8_pp_mb-0000000050-00001.root\""
inTrkrFiles[2]="\"/sphenix/lustre01/sphnxpro/mdc2/pythia8_pp_mb/g4hits/G4Hits_pythia8_pp_mb-0000000050-00002.root\""
inTrkrFiles[3]="\"/sphenix/lustre01/sphnxpro/mdc2/pythia8_pp_mb/g4hits/G4Hits_pythia8_pp_mb-0000000050-00003.root\""
inTrkrFiles[4]="\"/sphenix/lustre01/sphnxpro/mdc2/pythia8_pp_mb/g4hits/G4Hits_pythia8_pp_mb-0000000050-00004.root\""
inTrkrFiles[5]="\"/sphenix/lustre01/sphnxpro/mdc2/pythia8_pp_mb/g4hits/G4Hits_pythia8_pp_mb-0000000050-00005.root\""
inTrkrFiles[6]="\"/sphenix/lustre01/sphnxpro/mdc2/pythia8_pp_mb/g4hits/G4Hits_pythia8_pp_mb-0000000050-00006.root\""
inTrkrFiles[7]="\"/sphenix/lustre01/sphnxpro/mdc2/pythia8_pp_mb/g4hits/G4Hits_pythia8_pp_mb-0000000050-00007.root\""
inTrkrFiles[8]="\"/sphenix/lustre01/sphnxpro/mdc2/pythia8_pp_mb/g4hits/G4Hits_pythia8_pp_mb-0000000050-00008.root\""
inTrkrFiles[9]="\"/sphenix/lustre01/sphnxpro/mdc2/pythia8_pp_mb/g4hits/G4Hits_pythia8_pp_mb-0000000050-00009.root\""

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
for input in ${inTrkrFiles[@]}; do
  root -b -q "Fun4All_ForCorrelatorJetTree.C($input, ${inCaloFiles[$nFile]}, ${inTrueFiles[$nFile]}, ${outputFiles[$nFile]}, $nEvt, $verb)"
  (( nFile++ ))
done

# delete arrays
unset inTrkrFiles
unset inCaloFiles
unset inTrueFiles
unset outputFiles

# end -------------------------------------------------------------------------
