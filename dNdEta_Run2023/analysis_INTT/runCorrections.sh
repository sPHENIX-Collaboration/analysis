#! /bin/bash

corrfiletag=HIJING_Baseline_EtaDepAdc
outfilepath_baseline=HIJING_Baseline_EtaDepAdc
outfilepath_closure=HIJING_closure_EtaDepAdc
outfilepath_data=Data_Run20869_EtaDepAdc
xcheckdir=cross-checks-EtaDepAdc

centralities_alice=(0 5 10 20 30 40 50 60 70) # ALICE 2010 binnings
length_alice=${#centralities_alice[@]}

# for ((i=0; i<$length_alice-1; i++))
# do
#     echo "Processing centrality bin: ${centralities_alice[i]} - ${centralities_alice[i+1]}"
#     # ./Corrections [infile] [CentLow] [CentHigh] [pvzmin] [pvzmax] [applyc] [applyg] [applym] [estag] [aselstring] [correctionfiletag] [outfilepath] [debug]
#     echo "Processing HIJING Baseline"
#     ./Corrections ./minitree/TrackletMinitree_Sim_Ntuple_HIJING_new_20240424/dRcut0p5/minitree_set1.root ${centralities_alice[i]} ${centralities_alice[i+1]} -30 -10 0 0 0 null null ${corrfiletag} ${outfilepath_baseline} 1
#     ./Corrections ./minitree/TrackletMinitree_Sim_Ntuple_HIJING_new_20240424/dRcut0p5/minitree_set1.root ${centralities_alice[i]} ${centralities_alice[i+1]} -30 -20 0 0 0 null null ${corrfiletag} ${outfilepath_baseline} 1
#     ./Corrections ./minitree/TrackletMinitree_Sim_Ntuple_HIJING_new_20240424/dRcut0p5/minitree_set1.root ${centralities_alice[i]} ${centralities_alice[i+1]} -20 -10 0 0 0 null null ${corrfiletag} ${outfilepath_baseline} 1
#     echo "Processing HIJING Closure"
#     ./Corrections ./minitree/TrackletMinitree_Sim_Ntuple_HIJING_new_20240424/dRcut0p5/minitree_set2.root ${centralities_alice[i]} ${centralities_alice[i+1]} -30 -10 1 0 0 null null ${corrfiletag} ${outfilepath_closure} 1
#     ./Corrections ./minitree/TrackletMinitree_Sim_Ntuple_HIJING_new_20240424/dRcut0p5/minitree_set2.root ${centralities_alice[i]} ${centralities_alice[i+1]} -30 -20 1 0 0 null null ${corrfiletag} ${outfilepath_closure} 1
#     ./Corrections ./minitree/TrackletMinitree_Sim_Ntuple_HIJING_new_20240424/dRcut0p5/minitree_set2.root ${centralities_alice[i]} ${centralities_alice[i+1]} -20 -10 1 0 0 null null ${corrfiletag} ${outfilepath_closure} 1
#     echo "Processing Data Run2023"
#     ./Corrections ./minitree/TrackletMinitree_Data_CombinedNtuple_Run20869_HotDead_BCO_ADC_Survey/dRcut0p5/minitree_merged.root ${centralities_alice[i]} ${centralities_alice[i+1]} -30 -10 1 1 1 null null ${corrfiletag} ${outfilepath_data} 1
#     ./Corrections ./minitree/TrackletMinitree_Data_CombinedNtuple_Run20869_HotDead_BCO_ADC_Survey/dRcut0p5/minitree_merged.root ${centralities_alice[i]} ${centralities_alice[i+1]} -30 -20 1 1 1 null null ${corrfiletag} ${outfilepath_data} 1
#     ./Corrections ./minitree/TrackletMinitree_Data_CombinedNtuple_Run20869_HotDead_BCO_ADC_Survey/dRcut0p5/minitree_merged.root ${centralities_alice[i]} ${centralities_alice[i+1]} -20 -10 1 1 1 null null ${corrfiletag} ${outfilepath_data} 1
# done

centralities_phobos=(0 3 6 10 15 20 25 30 35 40 45 50 55 60 65 70) # PHOBOS 2011 binnings
length_phobos=${#centralities_phobos[@]}

# for ((i=0; i<$length_phobos-1; i++))
# do
#     echo "Processing centrality bin: ${centralities_phobos[i]} - ${centralities_phobos[i+1]}"
#     # ./Corrections [infile] [CentLow] [CentHigh] [pvzmin] [pvzmax] [applyc] [applyg] [applym] [estag] [aselstring] [correctionfiletag] [outfilepath] [debug]
#     echo "Processing HIJING Baseline"
#     ./Corrections ./minitree/TrackletMinitree_Sim_Ntuple_HIJING_new_20240424/dRcut0p5/minitree_set1.root ${centralities_phobos[i]} ${centralities_phobos[i+1]} -30 -10 0 0 0 null null ${corrfiletag} ${outfilepath_baseline} 1
#     ./Corrections ./minitree/TrackletMinitree_Sim_Ntuple_HIJING_new_20240424/dRcut0p5/minitree_set1.root ${centralities_phobos[i]} ${centralities_phobos[i+1]} -30 -20 0 0 0 null null ${corrfiletag} ${outfilepath_baseline} 1
#     ./Corrections ./minitree/TrackletMinitree_Sim_Ntuple_HIJING_new_20240424/dRcut0p5/minitree_set1.root ${centralities_phobos[i]} ${centralities_phobos[i+1]} -30 -10 0 0 0 null null ${corrfiletag} ${outfilepath_baseline} 1
#     echo "Processing HIJING Closure"
#     ./Corrections ./minitree/TrackletMinitree_Sim_Ntuple_HIJING_new_20240424/dRcut0p5/minitree_set2.root ${centralities_phobos[i]} ${centralities_phobos[i+1]} -30 -10 1 0 0 null null ${corrfiletag} ${outfilepath_closure} 1
#     ./Corrections ./minitree/TrackletMinitree_Sim_Ntuple_HIJING_new_20240424/dRcut0p5/minitree_set2.root ${centralities_phobos[i]} ${centralities_phobos[i+1]} -30 -20 1 0 0 null null ${corrfiletag} ${outfilepath_closure} 1
#     ./Corrections ./minitree/TrackletMinitree_Sim_Ntuple_HIJING_new_20240424/dRcut0p5/minitree_set2.root ${centralities_phobos[i]} ${centralities_phobos[i+1]} -20 -10 1 0 0 null null ${corrfiletag} ${outfilepath_closure} 1
#     echo "Processing Data Run2023"
#     ./Corrections ./minitree/TrackletMinitree_Data_CombinedNtuple_Run20869_HotDead_BCO_ADC_Survey/dRcut0p5/minitree_merged.root ${centralities_phobos[i]} ${centralities_phobos[i+1]} -30 -10 1 1 1 null null ${corrfiletag} ${outfilepath_data} 1
#     ./Corrections ./minitree/TrackletMinitree_Data_CombinedNtuple_Run20869_HotDead_BCO_ADC_Survey/dRcut0p5/minitree_merged.root ${centralities_phobos[i]} ${centralities_phobos[i+1]} -30 -20 1 1 1 null null ${corrfiletag} ${outfilepath_data} 1
#     ./Corrections ./minitree/TrackletMinitree_Data_CombinedNtuple_Run20869_HotDead_BCO_ADC_Survey/dRcut0p5/minitree_merged.root ${centralities_phobos[i]} ${centralities_phobos[i+1]} -20 -10 1 1 1 null null ${corrfiletag} ${outfilepath_data} 1
# done

# echo "Processing centrality bin: 0 - 70"
# echo "Processing HIJING Baseline"
# ./Corrections ./minitree/TrackletMinitree_Sim_Ntuple_HIJING_new_20240424/dRcut0p5/minitree_set1.root 0 70 -30 -10 0 0 0 null null ${corrfiletag} ${outfilepath_baseline} 1
# ./Corrections ./minitree/TrackletMinitree_Sim_Ntuple_HIJING_new_20240424/dRcut0p5/minitree_set1.root 0 70 -30 -20 0 0 0 null null ${corrfiletag} ${outfilepath_baseline} 1
# ./Corrections ./minitree/TrackletMinitree_Sim_Ntuple_HIJING_new_20240424/dRcut0p5/minitree_set1.root 0 70 -20 -10 0 0 0 null null ${corrfiletag} ${outfilepath_baseline} 1
# echo "Processing HIJING Closure"
# ./Corrections ./minitree/TrackletMinitree_Sim_Ntuple_HIJING_new_20240424/dRcut0p5/minitree_set2.root 0 70 -30 -10 1 0 0 null null ${corrfiletag} ${outfilepath_closure} 1
# ./Corrections ./minitree/TrackletMinitree_Sim_Ntuple_HIJING_new_20240424/dRcut0p5/minitree_set2.root 0 70 -30 -20 1 0 0 null null ${corrfiletag} ${outfilepath_closure} 1
# ./Corrections ./minitree/TrackletMinitree_Sim_Ntuple_HIJING_new_20240424/dRcut0p5/minitree_set2.root 0 70 -20 -10 1 0 0 null null ${corrfiletag} ${outfilepath_closure} 1
# echo "Processing Data Run2023"
# ./Corrections ./minitree/TrackletMinitree_Data_CombinedNtuple_Run20869_HotDead_BCO_ADC_Survey/dRcut0p5/minitree_merged.root 0 70 -30 -10 1 1 1 null null ${corrfiletag} ${outfilepath_data} 1
# ./Corrections ./minitree/TrackletMinitree_Data_CombinedNtuple_Run20869_HotDead_BCO_ADC_Survey/dRcut0p5/minitree_merged.root 0 70 -30 -20 1 1 1 null null ${corrfiletag} ${outfilepath_data} 1
# ./Corrections ./minitree/TrackletMinitree_Data_CombinedNtuple_Run20869_HotDead_BCO_ADC_Survey/dRcut0p5/minitree_merged.root 0 70 -20 -10 1 1 1 null null ${corrfiletag} ${outfilepath_data} 1

cd ./plot/
for ((i=0; i<$length_alice-1; i++))
do
    python closure.py --datahistdir ./corrections/${outfilepath_data} --simhistdir ./corrections/${outfilepath_closure} --filedesc Centrality${centralities_alice[i]}to${centralities_alice[i+1]}_Zvtxm30p0tom10p0_noasel --plotdir ${xcheckdir}
    python closure.py --datahistdir ./corrections/${outfilepath_data} --simhistdir ./corrections/${outfilepath_closure} --filedesc Centrality${centralities_alice[i]}to${centralities_alice[i+1]}_Zvtxm30p0tom20p0_noasel --plotdir ${xcheckdir}
    python closure.py --datahistdir ./corrections/${outfilepath_data} --simhistdir ./corrections/${outfilepath_closure} --filedesc Centrality${centralities_alice[i]}to${centralities_alice[i+1]}_Zvtxm20p0tom10p0_noasel --plotdir ${xcheckdir}
done

for ((i=0; i<$length_phobos-1; i++))
do
    python closure.py --datahistdir ./corrections/${outfilepath_data} --simhistdir ./corrections/${outfilepath_closure} --filedesc Centrality${centralities_phobos[i]}to${centralities_phobos[i+1]}_Zvtxm30p0tom10p0_noasel --plotdir ${xcheckdir}
    python closure.py --datahistdir ./corrections/${outfilepath_data} --simhistdir ./corrections/${outfilepath_closure} --filedesc Centrality${centralities_phobos[i]}to${centralities_phobos[i+1]}_Zvtxm30p0tom20p0_noasel --plotdir ${xcheckdir}
    python closure.py --datahistdir ./corrections/${outfilepath_data} --simhistdir ./corrections/${outfilepath_closure} --filedesc Centrality${centralities_phobos[i]}to${centralities_phobos[i+1]}_Zvtxm20p0tom10p0_noasel --plotdir ${xcheckdir}
done

python closure.py --datahistdir ./corrections/${outfilepath_data} --simhistdir ./corrections/${outfilepath_closure} --filedesc Centrality0to70_Zvtxm30p0tom10p0_noasel --plotdir ${xcheckdir}
python closure.py --datahistdir ./corrections/${outfilepath_data} --simhistdir ./corrections/${outfilepath_closure} --filedesc Centrality0to70_Zvtxm30p0tom20p0_noasel --plotdir ${xcheckdir}
python closure.py --datahistdir ./corrections/${outfilepath_data} --simhistdir ./corrections/${outfilepath_closure} --filedesc Centrality0to70_Zvtxm20p0tom10p0_noasel --plotdir ${xcheckdir}