#! /bin/bash
out_name=out.root
pTlb=30
nevnt=-1
l_calo_cluster=dst_calo_cluster_0.list
l_truth_cluster=dst_truth_jet_0.list

root.exe -q -l ../Fun4All_JetRhoMedian.C\(\"$out_name\",$pTlb,$nevnt,\"$l_calo_cluster\",\"$l_truth_cluster\",\"\",0\) 

echo all done
