#! /bin/bash

# tagstr=dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0
# corrfiletag=HIJING_Baseline_${tagstr}
# outfilepath_baseline=HIJING_Baseline_${tagstr}
# outfilepath_closure=HIJING_closure_${tagstr}
# outfilepath_data=Data_Run20869_${tagstr}
# hijingbaselinedir=/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/minitree/TrackletMinitree_Sim_Ntuple_HIJING_new_20240424/${tagstr}
# hijingclosuredir=/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/minitree/TrackletMinitree_Sim_Ntuple_HIJING_new_20240424/${tagstr}
# datadir=/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/minitree/TrackletMinitree_Data_CombinedNtuple_Run20869_HotDead_BCO_ADC_Survey/${tagstr}

xcheckdir=cross-checks

centralities_alice=(0 5 10 20 30 40 50 60 70) # ALICE 2010 binnings
length_alice=${#centralities_alice[@]}

for ((i=0; i<$length_alice-1; i++))
do
    python systematics.py --desc Centrality${centralities_alice[i]}to${centralities_alice[i+1]}_Zvtxm30p0tom10p0_noasel 
    python closure.py --datahistdir ./systematics --simhistdir ./corrections/HIJING_closure_dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0 --filedesc Centrality${centralities_alice[i]}to${centralities_alice[i+1]}_Zvtxm30p0tom10p0_noasel --plotdir ${xcheckdir}
    # python closure.py --datahistdir ./corrections/${outfilepath_data} --simhistdir ./corrections/${outfilepath_closure} --filedesc Centrality${centralities_alice[i]}to${centralities_alice[i+1]}_Zvtxm30p0tom20p0_noasel --plotdir ${xcheckdir}
    # python closure.py --datahistdir ./corrections/${outfilepath_data} --simhistdir ./corrections/${outfilepath_closure} --filedesc Centrality${centralities_alice[i]}to${centralities_alice[i+1]}_Zvtxm20p0tom10p0_noasel --plotdir ${xcheckdir}
done

# centralities_phobos=(0 3 6 10 15 20 25 30 35 40 45 50 55 60 65 70) # PHOBOS 2011 binnings
# length_phobos=${#centralities_phobos[@]}

# for ((i=0; i<$length_phobos-1; i++))
# do
#     python closure.py --datahistdir ./corrections/${outfilepath_data} --simhistdir ./corrections/${outfilepath_closure} --filedesc Centrality${centralities_phobos[i]}to${centralities_phobos[i+1]}_Zvtxm30p0tom10p0_noasel --plotdir ${xcheckdir}
#     python closure.py --datahistdir ./corrections/${outfilepath_data} --simhistdir ./corrections/${outfilepath_closure} --filedesc Centrality${centralities_phobos[i]}to${centralities_phobos[i+1]}_Zvtxm30p0tom20p0_noasel --plotdir ${xcheckdir}
#     python closure.py --datahistdir ./corrections/${outfilepath_data} --simhistdir ./corrections/${outfilepath_closure} --filedesc Centrality${centralities_phobos[i]}to${centralities_phobos[i+1]}_Zvtxm20p0tom10p0_noasel --plotdir ${xcheckdir}
# done

python systematics.py --desc Centrality0to70_Zvtxm30p0tom10p0_noasel 
python closure.py --datahistdir ./systematics --simhistdir ./corrections/HIJING_closure_dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0 --filedesc Centrality0to70_Zvtxm30p0tom10p0_noasel --plotdir ${xcheckdir}
# python closure.py --datahistdir ./corrections/${outfilepath_data} --simhistdir ./corrections/${outfilepath_closure} --filedesc Centrality0to70_Zvtxm30p0tom20p0_noasel --plotdir ${xcheckdir}
# python closure.py --datahistdir ./corrections/${outfilepath_data} --simhistdir ./corrections/${outfilepath_closure} --filedesc Centrality0to70_Zvtxm20p0tom10p0_noasel --plotdir ${xcheckdir}