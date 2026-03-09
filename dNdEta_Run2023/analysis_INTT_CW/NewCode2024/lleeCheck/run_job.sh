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

# "$INT(process_shift) $(output_directory) $(input_directory) $(input_filename) $(macro_name)"

process=$1
output_directory=$2
input_directory=$3
input_filename=$4
macro_name=$5


echo process: ${process}
echo output_directory: ${output_directory}
echo input_directory: ${input_directory}
echo input_filename: ${input_filename}
echo macro_name: ${macro_name}

root.exe -q -b ${macro_name}.C\(${process},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\"\)


echo all done process the ${macro_name}.C, process_id ${process}
