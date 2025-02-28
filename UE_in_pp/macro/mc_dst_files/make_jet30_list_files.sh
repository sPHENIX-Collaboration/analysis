#!/bin/bash

lines=3
rm run22_jet30_dst_truth_jet_* run22_jet30_dst_global_* run22_jet30_dst_calo_cluster_* run22_jet30_dst_truthinfo_*
args="-l $lines --numeric-suffixes=0 --suffix-length=4 --additional-suffix=.list"
split $args run22_jet30_dst_truth_jet.list "run22_jet30_dst_truth_jet_"
split $args run22_jet30_dst_global.list "run22_jet30_dst_global_"
split $args run22_jet30_dst_calo_cluster.list "run22_jet30_dst_calo_cluster_"
split $args run22_jet30_dst_truthinfo.list "run22_jet30_dst_truthinfo_"
