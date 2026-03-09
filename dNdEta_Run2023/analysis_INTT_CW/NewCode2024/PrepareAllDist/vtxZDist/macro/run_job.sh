#! /bin/bash
# export USER="$(id -u -n)"
# export LOGNAME=${USER}
# export HOME=/sphenix/u/${LOGNAME}

source /opt/sphenix/core/bin/sphenix_setup.sh -n ana.441

# export MYINSTALL1=/sphenix/user/ChengWei/build_sPH_coresoftware/EvtIDReporter/install
# export MYINSTALL=/sphenix/user/ChengWei/INTT/INTT/general_codes/CWShih/INTTChipOccupancy/build/install

# export LD_LIBRARY_PATH=$MYINSTALL1/lib:$LD_LIBRARY_PATH
# export ROOT_INCLUDE_PATH=$MYINSTALL1/include:$ROOT_INCLUDE_PATH

# export LD_LIBRARY_PATH=$MYINSTALL/lib:$LD_LIBRARY_PATH
# export ROOT_INCLUDE_PATH=$MYINSTALL/include:$ROOT_INCLUDE_PATH

# source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL1
# source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL

# echo $ROOT_INCLUDE_PATH | sed -e "s/:/\n/g"

process=$1
runnumber=$2
nEvents=$3
output_directory=$4
input_directory=$5
input_filename=$6
formatted_process_id=$(printf "%05d" ${process})

echo process: ${process}
echo runnumber: ${runnumber}
echo nEvents: ${nEvents}
echo output_directory: ${output_directory}
echo input_directory: ${input_directory}
echo input_filename: ${input_filename}

root.exe -q -b Run_vtxZDist.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\"\)


echo all done process the Run_vtxZDist.C, process_id ${process}
