#!/bin/bash

lines=3
#rm run21_herwig_jet10_dst_truth_jet_* run21_herwig_jet10_dst_mbd_epd_* run21_herwig_jet10_dst_calo_cluster_* run21_herwig_jet10_g4hits_*
args="-l $lines --numeric-suffixes=0 --suffix-length=4 --additional-suffix=.list"
split $args run21_herwig_jet10_dst_truth_jet.list "../mc_list_files/run21_herwig_jet10_dst_truth_jet_"
split $args run21_herwig_jet10_dst_mbd_epd.list "../mc_list_files/run21_herwig_jet10_dst_mbd_epd_"
split $args run21_herwig_jet10_dst_calo_cluster.list "../mc_list_files/run21_herwig_jet10_dst_calo_cluster_"
split $args run21_herwig_jet10_g4hits.list "../mc_list_files/run21_herwig_jet10_g4hits_"
