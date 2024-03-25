#!/bin/bash

declare -a numbers=("28530" "28534" "28535" "28536" "28537" "28549")

# Directory to look in
#directory="/sphenix/lustre01/sphnxpro/commissioning/aligned_cosmic"
directory="/sphenix/tg/tg01/commissioning/CaloCalibWG/hanpuj"
export macropath="/sphenix/user/hanpuj/HCalCosmicCalib/macro"
export TargetDir="/sphenix/user/hanpuj/HCalCosmicCalib/condor_file"

# Counter
i=0

# Loop through all .prdf files
for file in $directory/cosmics-*.prdf
do
    # Extract the run number from the filename
    num=$(basename $file | cut -d'-' -f 2 | cut -d'-' -f 1)
    num=$((10#$num)) # Remove leading zeros

    # Check if the number is in the list
    if [[ " ${numbers[@]} " =~ " ${num} " ]]; then
        echo "$file contains the number $num."
	li=$i
        [ $i -lt 1000 ] && li="0$li"
        [ $i -lt 100 ] && li="0$li"
        [ $i -lt 10 ] && li="0$li"

        # Create a directory for each run number, then create OutDir within that
        mkdir -p ${TargetDir}/Run$num/OutDir$li
        export WorkDir="${TargetDir}/Run$num/OutDir$li"
        echo "WorkDir:" ${WorkDir}

        pushd ${WorkDir}

        cp -v "$macropath"/CondorRun.sh CondorRunTC$li.sh
        cp "$macropath"/Fun4All_HCalCalib.C
        echo $file >> prdf_file.txt

        chmod +x CondorRunTC$li.sh
        cat >>ff.sub<< EOF
+JobFlavour                   = "workday"
transfer_input_files          = ${WorkDir}/CondorRunTC$li.sh,${WorkDir}/Fun4All_HCalCalib.C
Executable                    = CondorRunTC$li.sh
Universe                      = vanilla
Notification                  = Never
GetEnv                        = True
Priority                      = +40
Output                        = test.out
Error                         = test.err
Log                           = test.log
Notify_user                   = hj2552@columbia.edu

Queue
EOF

        condor_submit ff.sub
        popd
        ((i++))
    fi  
done
