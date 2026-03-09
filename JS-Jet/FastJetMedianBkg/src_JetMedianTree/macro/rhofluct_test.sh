#! /bin/bash
out_name=out.root
nevnt=10
min_lead_truth_pt=10

calo_list=dst_calo_cluster.list
bbc_list=dst_bbc_g4hit.list
global_list=dst_global.list
g4hit_list=dst_bbc_g4hit.list
truth_jet_list=dst_truth_jet.list
g4hit_list=dst_truth_g4hit.list
calo_list=dst_calo_cluster.list


echo ../rhofluct_condor.sh $nevnt 0.4 test_r04.root $calo_list $bbc_list $global_list 0
../rhofluct_condor.sh $nevnt 0.4 test_r04.root $calo_list $bbc_list  $global_list 0

echo all done test.sh
