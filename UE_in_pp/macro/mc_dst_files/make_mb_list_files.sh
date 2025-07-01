#!/bin/bash

lines=10
rm "../mc_list_files/run22_mb_dst_truth_jet_*"
rm "../mc_list_files/run22_mb_dst_global_*"
rm "../mc_list_files/run22_mb_dst_calo_cluster_*"
rm "../mc_list_files/run22_mb_dst_truthinfo_*"
rm "../mc_list_files/run22_mb_dst_tracks_*"
args="-l $lines --numeric-suffixes=0 --suffix-length=4 --additional-suffix=.list"
split $args run22_mb_dst_truth_jet.list "../mc_list_files/run22_mb_dst_truth_jet_"
split $args run22_mb_dst_global.list "../mc_list_files/run22_mb_dst_global_"
split $args run22_mb_dst_calo_cluster.list "../mc_list_files/run22_mb_dst_calo_cluster_"
split $args run22_mb_dst_truthinfo.list "../mc_list_files/run22_mb_dst_truthinfo_"
split $args run22_mb_dst_tracks.list "../mc_list_files/run22_mb_dst_tracks_"
