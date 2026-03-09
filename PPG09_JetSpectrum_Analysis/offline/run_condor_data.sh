#!/bin/bash
mkdir -p condor_data
mkdir -p output_data

rm condor_data/*
rm output_data/*

segperjob=20
radius=(0.4)

export treepath="/sphenix/tg/tg01/jets/hanpuj/CaloDataAna_ppg09"
> ListCondorRunData.csv

total_condorjobs=0

echo "Starting to form arguement list..."
for ((r=0; r<${#radius[@]}; r++)); do
    export radius_value=${radius[$r]}

    for run in $(cat listrunnumber.txt); do
        echo "Run: ${run}"
        seg_number=$(find "$treepath/Run${run}" -mindepth 1 -maxdepth 1 -type d -name 'OutDir*' | wc -l)
        total_jobs=$(( (seg_number + segperjob - 1) / segperjob ))
        for ((i=0; i<total_jobs; i++)); do
            if [[ $(((i+1)*segperjob)) -gt $seg_number ]]; then
                nseg=$((seg_number - i*segperjob))
            else
                nseg=$segperjob
            fi
            iseg=$((i*segperjob))

            echo "$run,$nseg,$iseg,$radius_value" >> ListCondorRunData.csv

            total_condorjobs=$((total_condorjobs+1))
        done
    done
done
echo "Arguement list formed!"
echo "Total jobs: ${total_condorjobs}"

condor_submit RunCondorData.sub

echo "All jobs submitted!"
