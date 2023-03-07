#!/bin/bash
# 'RunTrackCutStudyG4.sh'
# Derek Anderson
# 11.11.2022
#
# Short to script to run the macro
# 'Fun4All_G4_sPHENIX_ForTrackCutStudy.C'
# over a (small) set of files.

# declare i/o lists
declare -a inFiles
declare -a embFiles
declare -a outFiles

# input files
inFiles[0]="\"/sphenix/lustre01/sphnxpro/mdc2/shijing_hepmc/fm_0_20/g4hits/G4Hits_sHijing_0_20fm-0000000040-00806.root\""
inFiles[1]="\"/sphenix/lustre01/sphnxpro/mdc2/shijing_hepmc/fm_0_20/g4hits/G4Hits_sHijing_0_20fm-0000000040-00817.root\""
inFiles[2]="\"/sphenix/lustre01/sphnxpro/mdc2/shijing_hepmc/fm_0_20/g4hits/G4Hits_sHijing_0_20fm-0000000040-00818.root\""
inFiles[3]="\"/sphenix/lustre01/sphnxpro/mdc2/shijing_hepmc/fm_0_20/g4hits/G4Hits_sHijing_0_20fm-0000000040-00835.root\""
inFiles[4]="\"/sphenix/lustre01/sphnxpro/mdc2/shijing_hepmc/fm_0_20/g4hits/G4Hits_sHijing_0_20fm-0000000040-00848.root\""
inFiles[5]="\"/sphenix/lustre01/sphnxpro/mdc2/shijing_hepmc/fm_0_20/g4hits/G4Hits_sHijing_0_20fm-0000000040-00869.root\""
inFiles[6]="\"/sphenix/lustre01/sphnxpro/mdc2/shijing_hepmc/fm_0_20/g4hits/G4Hits_sHijing_0_20fm-0000000040-00882.root\""
inFiles[7]="\"/sphenix/lustre01/sphnxpro/mdc2/shijing_hepmc/fm_0_20/g4hits/G4Hits_sHijing_0_20fm-0000000040-00888.root\""
inFiles[8]="\"/sphenix/lustre01/sphnxpro/mdc2/shijing_hepmc/fm_0_20/g4hits/G4Hits_sHijing_0_20fm-0000000040-00893.root\""
inFiles[9]="\"/sphenix/lustre01/sphnxpro/mdc2/shijing_hepmc/fm_0_20/g4hits/G4Hits_sHijing_0_20fm-0000000040-00895.root\""
inFiles[10]="\"/sphenix/lustre01/sphnxpro/mdc2/shijing_hepmc/fm_0_20/g4hits/G4Hits_sHijing_0_20fm-0000000040-00906.root\""
inFiles[11]="\"/sphenix/lustre01/sphnxpro/mdc2/shijing_hepmc/fm_0_20/g4hits/G4Hits_sHijing_0_20fm-0000000040-00907.root\""
inFiles[12]="\"/sphenix/lustre01/sphnxpro/mdc2/shijing_hepmc/fm_0_20/g4hits/G4Hits_sHijing_0_20fm-0000000040-00937.root\""
inFiles[13]="\"/sphenix/lustre01/sphnxpro/mdc2/shijing_hepmc/fm_0_20/g4hits/G4Hits_sHijing_0_20fm-0000000040-00949.root\""
inFiles[14]="\"/sphenix/lustre01/sphnxpro/mdc2/shijing_hepmc/fm_0_20/g4hits/G4Hits_sHijing_0_20fm-0000000040-00951.root\""
inFiles[15]="\"/sphenix/lustre01/sphnxpro/mdc2/shijing_hepmc/fm_0_20/g4hits/G4Hits_sHijing_0_20fm-0000000040-00952.root\""
inFiles[16]="\"/sphenix/lustre01/sphnxpro/mdc2/shijing_hepmc/fm_0_20/g4hits/G4Hits_sHijing_0_20fm-0000000040-00973.root\""

# input embed files
embFiles[0]="\"/sphenix/lustre01/sphnxpro/mdc2/shijing_hepmc/fm_0_20/g4hits/G4Hits_sHijing_0_20fm-0000000040-00806.root\""
embFiles[1]="\"/sphenix/lustre01/sphnxpro/mdc2/shijing_hepmc/fm_0_20/g4hits/G4Hits_sHijing_0_20fm-0000000040-00817.root\""
embFiles[2]="\"/sphenix/lustre01/sphnxpro/mdc2/shijing_hepmc/fm_0_20/g4hits/G4Hits_sHijing_0_20fm-0000000040-00818.root\""
embFiles[3]="\"/sphenix/lustre01/sphnxpro/mdc2/shijing_hepmc/fm_0_20/g4hits/G4Hits_sHijing_0_20fm-0000000040-00835.root\""
embFiles[4]="\"/sphenix/lustre01/sphnxpro/mdc2/shijing_hepmc/fm_0_20/g4hits/G4Hits_sHijing_0_20fm-0000000040-00848.root\""
embFiles[5]="\"/sphenix/lustre01/sphnxpro/mdc2/shijing_hepmc/fm_0_20/g4hits/G4Hits_sHijing_0_20fm-0000000040-00869.root\""
embFiles[6]="\"/sphenix/lustre01/sphnxpro/mdc2/shijing_hepmc/fm_0_20/g4hits/G4Hits_sHijing_0_20fm-0000000040-00882.root\""
embFiles[7]="\"/sphenix/lustre01/sphnxpro/mdc2/shijing_hepmc/fm_0_20/g4hits/G4Hits_sHijing_0_20fm-0000000040-00888.root\""
embFiles[8]="\"/sphenix/lustre01/sphnxpro/mdc2/shijing_hepmc/fm_0_20/g4hits/G4Hits_sHijing_0_20fm-0000000040-00893.root\""
embFiles[9]="\"/sphenix/lustre01/sphnxpro/mdc2/shijing_hepmc/fm_0_20/g4hits/G4Hits_sHijing_0_20fm-0000000040-00895.root\""
embFiles[10]="\"/sphenix/lustre01/sphnxpro/mdc2/shijing_hepmc/fm_0_20/g4hits/G4Hits_sHijing_0_20fm-0000000040-00906.root\""
embFiles[11]="\"/sphenix/lustre01/sphnxpro/mdc2/shijing_hepmc/fm_0_20/g4hits/G4Hits_sHijing_0_20fm-0000000040-00907.root\""
embFiles[12]="\"/sphenix/lustre01/sphnxpro/mdc2/shijing_hepmc/fm_0_20/g4hits/G4Hits_sHijing_0_20fm-0000000040-00937.root\""
embFiles[13]="\"/sphenix/lustre01/sphnxpro/mdc2/shijing_hepmc/fm_0_20/g4hits/G4Hits_sHijing_0_20fm-0000000040-00949.root\""
embFiles[14]="\"/sphenix/lustre01/sphnxpro/mdc2/shijing_hepmc/fm_0_20/g4hits/G4Hits_sHijing_0_20fm-0000000040-00951.root\""
embFiles[15]="\"/sphenix/lustre01/sphnxpro/mdc2/shijing_hepmc/fm_0_20/g4hits/G4Hits_sHijing_0_20fm-0000000040-00952.root\""
embFiles[16]="\"/sphenix/lustre01/sphnxpro/mdc2/shijing_hepmc/fm_0_20/g4hits/G4Hits_sHijing_0_20fm-0000000040-00973.root\""

# output files
outFiles[0]="\"sPhenixG4_hijing50khz_embedOnly00806.root\""
outFiles[1]="\"sPhenixG4_hijing50khz_embedOnly00817.root\""
outFiles[2]="\"sPhenixG4_hijing50khz_embedOnly00818.root\""
outFiles[3]="\"sPhenixG4_hijing50khz_embedOnly00835.root\""
outFiles[4]="\"sPhenixG4_hijing50khz_embedOnly00848.root\""
outFiles[5]="\"sPhenixG4_hijing50khz_embedOnly00869.root\""
outFiles[6]="\"sPhenixG4_hijing50khz_embedOnly00882.root\""
outFiles[7]="\"sPhenixG4_hijing50khz_embedOnly00888.root\""
outFiles[8]="\"sPhenixG4_hijing50khz_embedOnly00893.root\""
outFiles[9]="\"sPhenixG4_hijing50khz_embedOnly00895.root\""
outFiles[10]="\"sPhenixG4_hijing50khz_embedOnly00906.root\""
outFiles[11]="\"sPhenixG4_hijing50khz_embedOnly00907.root\""
outFiles[12]="\"sPhenixG4_hijing50khz_embedOnly00937.root\""
outFiles[13]="\"sPhenixG4_hijing50khz_embedOnly00949.root\""
outFiles[14]="\"sPhenixG4_hijing50khz_embedOnly00951.root\""
outFiles[15]="\"sPhenixG4_hijing50khz_embedOnly00952.root\""
outFiles[16]="\"sPhenixG4_hijing50khz_embedOnly00973.root\""

# other parameters
nEvt=50
nSkip=0

# loop over files
(( nFile=0 ))
for input in ${inFiles[@]}; do
  root -b -q "Fun4All_G4_sPHENIX_ForTrackCutStudy.C($nEvt, $input, ${outFiles[$nFile]}, ${embFiles[$nFile]}, $nSkip)"
  (( nFile++ ))
done

# delete arrays
unset inFiles
unset embFiles
unset outFiles

# end -------------------------------------------------------------------------
