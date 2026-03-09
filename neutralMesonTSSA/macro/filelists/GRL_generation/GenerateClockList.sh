#!/bin/bash

dst_file="GoldenGL1RunList.txt"
clock_directory="list_allFEM_clock/"
mkdir -p $clock_directory

# Count total number of run numbers to give progress updates
total_runs=$(wc -l < "$dst_file")
current_run=0

while IFS= read -r runnumber; do
    current_run=$((current_run + 1))
    echo "Processing run number $runnumber ($current_run of $total_runs)..."

    if [[ ! -f "${clock_directory}/FEM_clock_${runnumber}.txt" ]]; then
        output_file="${clock_directory}/FEM_clock_${runnumber}.txt"
        

        for i in {0..9}; do
            echo "Processing SEB $i for run $runnumber..."
            echo $i >> "$output_file"
            ddump -i -e 10 /sphenix/lustre01/sphnxpro/physics/emcal/physics/physics_seb0$i-000$runnumber-0000.prdf | grep 'FEM Clock:' | sed 's/[^0-9 ]//g' >> "$output_file"
        done
        
        # Process SEB machines 10 through 15
        for i in {10..15}; do
            echo "Processing SEB $i for run $runnumber..."
            echo $i >> "$output_file"
            ddump -i -e 10 /sphenix/lustre01/sphnxpro/physics/emcal/physics/physics_seb$i-000$runnumber-0000.prdf | grep 'FEM Clock:' | sed 's/[^0-9 ]//g' >> "$output_file"
        done
        
        # Process SEB machines 16 and 17 (HCal)
        for i in {16..17}; do
            echo "Processing HCal SEB $i for run $runnumber..."
            echo $i >> "$output_file"
            ddump -i -e 10 /sphenix/lustre01/sphnxpro/physics/HCal/physics/physics_seb$i-000$runnumber-0000.prdf | grep 'FEM Clock:' | sed 's/[^0-9 ]//g' >> "$output_file"
        done
        
        echo "Finished processing run number $runnumber. FEM clock file saved to $output_file."
    
    else
    echo "FEM clock file for run $runnumber already exists. Skipping..."
    
    
    fi
done < "$dst_file"
 
echo "All runnumbers processed. Results saved in list_allFEM_clock directory"
