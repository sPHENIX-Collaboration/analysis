#! /bin/bash

Nevents=$1
particle_pT=$2
energy_range_down=$3
energy_range_up=$4
particle_species=$5
output_directory=$6
output_filename=$7
ANA_build=$8

echo Nevents = $Nevents
echo particle_pT = $particle_pT
echo energy_range_down = $energy_range_down
echo energy_range_up = $energy_range_up
echo particle_species = $particle_species
echo output_directory = $output_directory
echo output_filename = $output_filename
echo ANA_build = $ANA_build

source /opt/sphenix/core/bin/sphenix_setup.sh -n ana.$ANA_build
# Additional commands for my local environment
export SPHENIX=/sphenix/u/jzhang1
export MYINSTALL=$SPHENIX/install
source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL

cd /sphenix/user/jzhang1/INTT-EMCAL/InttSeedingTrackDev/ParticleGen/macro

if [ ! -d "$output_directory" ]; then
    mkdir -p $output_directory
fi

sub_foldername=${particle_species}\_${particle_pT}GeV
final_output_directory=${output_directory}/${sub_foldername}
completed_foldername=${final_output_directory}/completed

if [ ! -d "${final_output_directory}" ]; then
    mkdir -p ${final_output_directory}
    mkdir -p ${output_directory}/JY_log
    mkdir -p ${completed_foldername}
fi

root.exe -q -b Fun4All_physiTuto.C\($Nevents,$particle_pT,$energy_range_up,$energy_range_down,\"$particle_species\",\"$final_output_directory\",\"$output_filename\"\)

output_filename_nosuffix=${output_filename%.root}

echo check the output_filename_nosuffix = ${output_filename_nosuffix} while the original is ${output_filename}

mv ${final_output_directory}/${output_filename} ${completed_foldername}

echo all done

date
