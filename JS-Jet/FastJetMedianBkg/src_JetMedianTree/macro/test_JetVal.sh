#! /bin/bash
jettruth_list=dst_truth_jet_0.list
calo_list=dst_calo_cluster_0.list
global_list=dst_global_0.list
bbc_list=dst_bbc_g4hit_0.list
out_name=test_jetval.root

../run_JetVal.sh $jettruth_list  $calo_list $global_list $bbc_list  $out_name

echo all done test.sh
