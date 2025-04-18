#! /bin/bash

export USER="$(id -u -n)"
export LOGNAME=${USER}
export HOME=/sphenix/u/${LOGNAME}

source /opt/sphenix/core/bin/sphenix_setup.sh -n new

export MYINSTALL=$HOME/install
export LD_LIBRARY_PATH=$MYINSTALL/lib:$LD_LIBRARY_PATH
export ROOT_INCLUDE_PATH=$MYINSTALL/include:$ROOT_INCLUDE_PATH

source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL

corrfiletag=$1
outfilepath_baseline=$2
outfilepath_closure=$3
outfilepath_data=$4
hijingbaselinedir=$5
hijingclosuredir=$6
datadir=$7

# print out the input arguments
echo "corrfiletag: ${corrfiletag}"
echo "outfilepath_baseline: ${outfilepath_baseline}"
echo "outfilepath_closure: ${outfilepath_closure}"
echo "outfilepath_data: ${outfilepath_data}"
echo "hijingbaselinedir: ${hijingbaselinedir}"
echo "hijingclosuredir: ${hijingclosuredir}"
echo "datadir: ${datadir}"

# hadd the files in directories
hadd -f -j 20 -k ${hijingbaselinedir}/minitree_set1.root ${hijingbaselinedir}/minitree_{00000..02499}.root
hadd -f -j 20 -k ${hijingclosuredir}/minitree_set2.root ${hijingclosuredir}/minitree_{02500..04999}.root
hadd -f -j 20 -k ${datadir}/minitree_segment1.root ${datadir}/minitree_00{000..299}.root
hadd -f -j 20 -k ${datadir}/minitree_segment2.root ${datadir}/minitree_00{300..599}.root
hadd -f -j 20 -k ${datadir}/minitree_segment3.root ${datadir}/minitree_00{600..899}.root
hadd -f -j 20 -k ${datadir}/minitree_segment4.root ${datadir}/minitree_0{0900..1199}.root
hadd -f -j 20 -k ${datadir}/minitree_segment5.root ${datadir}/minitree_0{1200..1499}.root
hadd -f -j 20 -k ${datadir}/minitree_segment6.root ${datadir}/minitree_0{1500..1757}.root

# hadd -f -k ${datadir}/minitree_segment1.root ${datadir}/minitree_00{000..299}.root
# hadd -f -k ${datadir}/minitree_segment2.root ${datadir}/minitree_00{300..599}.root
# hadd -f -k ${datadir}/minitree_segment3.root ${datadir}/minitree_00{600..899}.root
# hadd -f -k ${datadir}/minitree_segment4.root ${datadir}/minitree_0{0900..1199}.root
# hadd -f -k ${datadir}/minitree_segment5.root ${datadir}/minitree_0{1200..1499}.root
# hadd -f -k ${datadir}/minitree_segment6.root ${datadir}/minitree_0{1500..1757}.root

centralities_phobos=(0 3 6 10 15 20 25 30 35 40 45 50 55 60 65 70) # PHOBOS 2011 binnings
length_phobos=${#centralities_phobos[@]}
for ((i=0; i<$length_phobos-1; i++))
do
    echo "Processing centrality bin: ${centralities_phobos[i]} - ${centralities_phobos[i+1]}"
    # ./Corrections [infile] [CentLow] [CentHigh] [pvzmin] [pvzmax] [applyc] [applyg] [applym] [estag] [aselstring] [correctionfiletag] [outfilepath] [debug]
    echo "Processing HIJING Baseline"
    ./Corrections ${hijingbaselinedir}/minitree_set1.root ${centralities_phobos[i]} ${centralities_phobos[i+1]} -10 10 0 0 0 null null ${corrfiletag} ${outfilepath_baseline} 1
    echo "Processing HIJING Closure"
    ./Corrections ${hijingclosuredir}/minitree_set2.root ${centralities_phobos[i]} ${centralities_phobos[i+1]} -10 10 1 0 0 null null ${corrfiletag} ${outfilepath_closure} 1
    echo "Processing Data Run2023"
    ./Corrections ${datadir}/minitree_segment1.root ${centralities_phobos[i]} ${centralities_phobos[i+1]} -10 10 1 1 1 null null ${corrfiletag} ${outfilepath_data}_segment1 1
    ./Corrections ${datadir}/minitree_segment2.root ${centralities_phobos[i]} ${centralities_phobos[i+1]} -10 10 1 1 1 null null ${corrfiletag} ${outfilepath_data}_segment2 1
    ./Corrections ${datadir}/minitree_segment3.root ${centralities_phobos[i]} ${centralities_phobos[i+1]} -10 10 1 1 1 null null ${corrfiletag} ${outfilepath_data}_segment3 1
    ./Corrections ${datadir}/minitree_segment4.root ${centralities_phobos[i]} ${centralities_phobos[i+1]} -10 10 1 1 1 null null ${corrfiletag} ${outfilepath_data}_segment4 1
    ./Corrections ${datadir}/minitree_segment5.root ${centralities_phobos[i]} ${centralities_phobos[i+1]} -10 10 1 1 1 null null ${corrfiletag} ${outfilepath_data}_segment5 1
    ./Corrections ${datadir}/minitree_segment6.root ${centralities_phobos[i]} ${centralities_phobos[i+1]} -10 10 1 1 1 null null ${corrfiletag} ${outfilepath_data}_segment6 1
done

echo "Processing centrality bin: 0 - 70"
echo "Processing HIJING Baseline"
./Corrections ${hijingbaselinedir}/minitree_set1.root 0 70 -10 10 0 0 0 null null ${corrfiletag} ${outfilepath_baseline} 1
echo "Processing HIJING Closure"
./Corrections ${hijingclosuredir}/minitree_set2.root 0 70 -10 10 1 0 0 null null ${corrfiletag} ${outfilepath_closure} 1
echo "Processing Data Run2024 (run 54280)"
./Corrections ${datadir}/minitree_segment1.root 0 70 -10 10 1 1 1 null null ${corrfiletag} ${outfilepath_data}_segment1 1
./Corrections ${datadir}/minitree_segment2.root 0 70 -10 10 1 1 1 null null ${corrfiletag} ${outfilepath_data}_segment2 1
./Corrections ${datadir}/minitree_segment3.root 0 70 -10 10 1 1 1 null null ${corrfiletag} ${outfilepath_data}_segment3 1
./Corrections ${datadir}/minitree_segment4.root 0 70 -10 10 1 1 1 null null ${corrfiletag} ${outfilepath_data}_segment4 1
./Corrections ${datadir}/minitree_segment5.root 0 70 -10 10 1 1 1 null null ${corrfiletag} ${outfilepath_data}_segment5 1
./Corrections ${datadir}/minitree_segment6.root 0 70 -10 10 1 1 1 null null ${corrfiletag} ${outfilepath_data}_segment6 1