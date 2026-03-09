#!/bin/bash
mkdir -p condor_sim
mkdir -p output_sim

rm condor_sim/*
rm output_sim/*

runindex=28
segperjob=20
radius=(0.4)

> ListCondorRunSim.csv

for ((r=0; r<${#radius[@]}; r++)); do
    export radius_value=${radius[$r]}

    runtype="MB"
    export treepath="/sphenix/tg/tg01/jets/hanpuj/JES_MC_run${runindex}/$runtype"
    totalseg=$(find "$treepath/" -mindepth 1 -maxdepth 1 -type d -name 'OutDir*' | wc -l)
    total_condorjobs=$(( (totalseg + segperjob - 1) / segperjob ))
    for ((i=0; i<total_condorjobs; i++)); do
        if [[ $(((i+1)*segperjob)) -gt $totalseg ]]; then
            nseg=$((totalseg - i*segperjob))
        else
            nseg=$segperjob
        fi
        iseg=$((i*segperjob))
    
        echo "$runtype,$nseg,$iseg,$radius_value" >> ListCondorRunSim.csv
    done
    
    runtype="Jet5GeV"
    export treepath="/sphenix/tg/tg01/jets/hanpuj/JES_MC_run${runindex}/$runtype"
    totalseg=$(find "$treepath/" -mindepth 1 -maxdepth 1 -type d -name 'OutDir*' | wc -l)
    total_condorjobs=$(( (totalseg + segperjob - 1) / segperjob ))
    for ((i=0; i<total_condorjobs; i++)); do
        if [[ $(((i+1)*segperjob)) -gt $totalseg ]]; then
            nseg=$((totalseg - i*segperjob))
        else
            nseg=$segperjob
        fi
        iseg=$((i*segperjob))
    
        echo "$runtype,$nseg,$iseg,$radius_value" >> ListCondorRunSim.csv
    done

    runtype="Jet10GeV"
    export treepath="/sphenix/tg/tg01/jets/hanpuj/JES_MC_run${runindex}/$runtype"
    totalseg=$(find "$treepath/" -mindepth 1 -maxdepth 1 -type d -name 'OutDir*' | wc -l)
    total_condorjobs=$(( (totalseg + segperjob - 1) / segperjob ))
    for ((i=0; i<total_condorjobs; i++)); do
        if [[ $(((i+1)*segperjob)) -gt $totalseg ]]; then
            nseg=$((totalseg - i*segperjob))
        else
            nseg=$segperjob
        fi
        iseg=$((i*segperjob))
    
        echo "$runtype,$nseg,$iseg,$radius_value" >> ListCondorRunSim.csv
    done
    
    runtype="Jet15GeV"
    export treepath="/sphenix/tg/tg01/jets/hanpuj/JES_MC_run${runindex}/$runtype"
    totalseg=$(find "$treepath/" -mindepth 1 -maxdepth 1 -type d -name 'OutDir*' | wc -l)
    total_condorjobs=$(( (totalseg + segperjob - 1) / segperjob ))
    for ((i=0; i<total_condorjobs; i++)); do
        if [[ $(((i+1)*segperjob)) -gt $totalseg ]]; then
            nseg=$((totalseg - i*segperjob))
        else
            nseg=$segperjob
        fi
        iseg=$((i*segperjob))
    
        echo "$runtype,$nseg,$iseg,$radius_value" >> ListCondorRunSim.csv
    done
    
    runtype="Jet20GeV"
    export treepath="/sphenix/tg/tg01/jets/hanpuj/JES_MC_run${runindex}/$runtype"
    totalseg=$(find "$treepath/" -mindepth 1 -maxdepth 1 -type d -name 'OutDir*' | wc -l)
    total_condorjobs=$(( (totalseg + segperjob - 1) / segperjob ))
    for ((i=0; i<total_condorjobs; i++)); do
        if [[ $(((i+1)*segperjob)) -gt $totalseg ]]; then
            nseg=$((totalseg - i*segperjob))
        else
            nseg=$segperjob
        fi
        iseg=$((i*segperjob))
    
        echo "$runtype,$nseg,$iseg,$radius_value" >> ListCondorRunSim.csv
    done
    
    runtype="Jet30GeV"
    export treepath="/sphenix/tg/tg01/jets/hanpuj/JES_MC_run${runindex}/$runtype"
    totalseg=$(find "$treepath/" -mindepth 1 -maxdepth 1 -type d -name 'OutDir*' | wc -l)
    total_condorjobs=$(( (totalseg + segperjob - 1) / segperjob ))
    for ((i=0; i<total_condorjobs; i++)); do
        if [[ $(((i+1)*segperjob)) -gt $totalseg ]]; then
            nseg=$((totalseg - i*segperjob))
        else
            nseg=$segperjob
        fi
        iseg=$((i*segperjob))
    
        echo "$runtype,$nseg,$iseg,$radius_value" >> ListCondorRunSim.csv
    done
    
    runtype="Jet50GeV"
    export treepath="/sphenix/tg/tg01/jets/hanpuj/JES_MC_run${runindex}/$runtype"
    totalseg=$(find "$treepath/" -mindepth 1 -maxdepth 1 -type d -name 'OutDir*' | wc -l)
    total_condorjobs=$(( (totalseg + segperjob - 1) / segperjob ))
    for ((i=0; i<total_condorjobs; i++)); do
        if [[ $(((i+1)*segperjob)) -gt $totalseg ]]; then
            nseg=$((totalseg - i*segperjob))
        else
            nseg=$segperjob
        fi
        iseg=$((i*segperjob))
    
        echo "$runtype,$nseg,$iseg,$radius_value" >> ListCondorRunSim.csv
    done

    runtype="Jet70GeV"
    export treepath="/sphenix/tg/tg01/jets/hanpuj/JES_MC_run${runindex}/$runtype"
    totalseg=$(find "$treepath/" -mindepth 1 -maxdepth 1 -type d -name 'OutDir*' | wc -l)
    total_condorjobs=$(( (totalseg + segperjob - 1) / segperjob ))
    for ((i=0; i<total_condorjobs; i++)); do
        if [[ $(((i+1)*segperjob)) -gt $totalseg ]]; then
            nseg=$((totalseg - i*segperjob))
        else
            nseg=$segperjob
        fi
        iseg=$((i*segperjob))
    
        echo "$runtype,$nseg,$iseg,$radius_value" >> ListCondorRunSim.csv
    done
done

echo "Arguement list formed!"

condor_submit RunCondorSim.sub

echo "All jobs submitted!"
