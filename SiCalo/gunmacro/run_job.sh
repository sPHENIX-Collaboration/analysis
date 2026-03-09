#! /bin/bash

Process_ID=$1
ANA_build=$2
echo ID = $Process_ID
echo ANA_build = $ANA_build

#source /opt/sphenix/core/bin/sphenix_setup.sh -n ana.$ANA_build
source /opt/sphenix/core/bin/sphenix_setup.sh 
# Additional commands for my local environment
export SPHENIX=/sphenix/user/jaein213/tracking/buildtest/INTT/general_codes/Jaein/SiliconSeeding/SiliconSeedAna
export MYINSTALL=$SPHENIX/install
source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL

cd /sphenix/user/jaein213/tracking/buildtest/INTT/general_codes/Jaein/SiliconSeeding/gunmacro

# if [ ! -d "$output_directory" ]; then
#     mkdir -p $output_directory
# fi

# sub_foldername=${particle_species}\_${particle_pT}GeV
# final_output_directory=${output_directory}/${sub_foldername}
# completed_foldername=${final_output_directory}/completed

# if [ ! -d "${final_output_directory}" ]; then
#     mkdir -p ${final_output_directory}
#     mkdir -p ${output_directory}/JY_log
#     mkdir -p ${completed_foldername}
# fi

root.exe -q -b Fun4All_singleParticle_Silicon.C\(\"$Process_ID\"\)

echo all done

date
