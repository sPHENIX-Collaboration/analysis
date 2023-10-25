#! /bin/bash
f4a=Fun4All_Probe.C
nevnt=10
min_lead_truth_pt=10

calo_list=dst_calo_cluster_0.list
jettruth_list=dst_truth_jet_0.list
bbc_list=dst_bbc_g4hit_0.list
global_list=dst_global_0.list
truthg4_list=dst_truth_g4hit_0.list


# ../condor.sh test.root $pTlb $nevnt $calo_list $jettruth_list $g4truth_list $bbc_list $global_list
if [ ! -e "$f4a" ]; then
    cp "../$f4a" "./"
fi

echo ../$f4a $nevnt 0.4 test_r04.root $min_lead_truth_pt $calo_list $jettruth_list $bbc_list $global_list $truthg4_list 0
../run.sh ../$f4a $nevnt 0.4 test_r04.root $min_lead_truth_pt $calo_list $jettruth_list $bbc_list $global_list $truthg4_list 0

echo all done test.sh
