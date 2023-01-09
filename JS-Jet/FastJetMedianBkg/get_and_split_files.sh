#!/bin/sh

mkdir -p out/jet_bg out/calo_rho/10GeV out/calo_rho/30GeV out/no_embed/10GeV out/no_embed/30GeV
mkdir -p lists/jet_bg lists/calo_rho/10GeV lists/calo_rho/30GeV lists/no_embed/10GeV lists/no_embed/30GeV
mkdir -p condor-out/jet_bg condor-out/calo_rho/10GeV condor-out/calo_rho/30GeV condor-out/no_embed/10GeV condor-out/no_embed/30GeV


#mkdir -p condor-out/jet_bg condor-out/calo_rho condor-out/no_embed condor-out/trk condor-out/no_embed
#mkdir -p condor-out/no_embed/10GeV condor-out/no_embed/30GeV condor-out/calo_rho/10GeV condor-out/calo_rho/30GeV
#mkdir -p out/calo_rho/10GeV out/calo_rho/30GeV out/calo_rho/10GeV out/calo_rho/30GeV
#mkdir -p lists lists/calo_rho/30GeV lists/jet_bg lists/trk lists/no_embed
#mkdir -p out out/trk out/jet_bg out/calo_rho out/no_embed plots

DIR=$(cd "$(dirname "$0")"; pwd)



cd lists/calo_rho/30GeV

echo "entering directory `pwd`"

CreateFileList.pl -run 40 -type 11 -embed DST_BBC_G4HIT DST_CALO_CLUSTER DST_CALO_G4HIT DST_TRUTH_JET DST_VERTEX

split -l50 --numeric-suffixes=1 --suffix-length=3 --additional-suffix=".list" dst_truth_jet.list "dst_truth_jet_"
split -l50 --numeric-suffixes=1 --suffix-length=3 --additional-suffix=".list" dst_calo_g4hit.list "dst_calo_g4hit_"
split -l50 --numeric-suffixes=1 --suffix-length=3 --additional-suffix=".list" dst_calo_cluster.list "dst_calo_cluster_"
split -l50 --numeric-suffixes=1 --suffix-length=3 --additional-suffix=".list" dst_vertex.list "dst_vertex_"
split -l50 --numeric-suffixes=1 --suffix-length=3 --additional-suffix=".list" dst_bbc_g4hit.list "dst_bbc_g4hit_"

if [ -f index.txt ]
then
    rm index.txt
fi

for i in {1..300..1}
do
  printf "%0*d\n" 3 $i >> index.txt
done

cd $DIR



cd lists/calo_rho/10GeV

echo "entering directory `pwd`"

CreateFileList.pl -run 40 -type 12 -embed DST_BBC_G4HIT DST_CALO_CLUSTER DST_CALO_G4HIT DST_TRUTH_JET DST_VERTEX

split -l5 --numeric-suffixes=1 --suffix-length=3 --additional-suffix=".list" dst_truth_jet.list "dst_truth_jet_"
split -l5 --numeric-suffixes=1 --suffix-length=3 --additional-suffix=".list" dst_calo_g4hit.list "dst_calo_g4hit_"
split -l5 --numeric-suffixes=1 --suffix-length=3 --additional-suffix=".list" dst_calo_cluster.list "dst_calo_cluster_"
split -l5 --numeric-suffixes=1 --suffix-length=3 --additional-suffix=".list" dst_vertex.list "dst_vertex_"
split -l5 --numeric-suffixes=1 --suffix-length=3 --additional-suffix=".list" dst_bbc_g4hit.list "dst_bbc_g4hit_"

if [ -f index.txt ]
then
    rm index.txt
fi

for i in {1..300..1}
do
  printf "%0*d\n" 3 $i >> index.txt
done

cd $DIR


cd lists/no_embed/30GeV

echo "entering directory `pwd`"

CreateFileList.pl -run 40 -type 11 DST_BBC_G4HIT DST_CALO_CLUSTER DST_CALO_G4HIT DST_TRUTH_JET DST_VERTEX

split -l50 --numeric-suffixes=1 --suffix-length=3 --additional-suffix=".list" dst_truth_jet.list "dst_truth_jet_"
split -l50 --numeric-suffixes=1 --suffix-length=3 --additional-suffix=".list" dst_calo_g4hit.list "dst_calo_g4hit_"
split -l50 --numeric-suffixes=1 --suffix-length=3 --additional-suffix=".list" dst_calo_cluster.list "dst_calo_cluster_"
split -l50 --numeric-suffixes=1 --suffix-length=3 --additional-suffix=".list" dst_vertex.list "dst_vertex_"
split -l50 --numeric-suffixes=1 --suffix-length=3 --additional-suffix=".list" dst_bbc_g4hit.list "dst_bbc_g4hit_"

if [ -f index.txt ]
then
    rm index.txt
fi

for i in {1..300..1}
do
  printf "%0*d\n" 3 $i >> index.txt
done

cd $DIR



cd lists/no_embed/10GeV

echo "entering directory `pwd`"

CreateFileList.pl -run 40 -type 12 DST_BBC_G4HIT DST_CALO_CLUSTER DST_CALO_G4HIT DST_TRUTH_JET DST_VERTEX

split -l10 --numeric-suffixes=1 --suffix-length=3 --additional-suffix=".list" dst_truth_jet.list "dst_truth_jet_"
split -l10 --numeric-suffixes=1 --suffix-length=3 --additional-suffix=".list" dst_calo_g4hit.list "dst_calo_g4hit_"
split -l10 --numeric-suffixes=1 --suffix-length=3 --additional-suffix=".list" dst_calo_cluster.list "dst_calo_cluster_"
split -l10 --numeric-suffixes=1 --suffix-length=3 --additional-suffix=".list" dst_vertex.list "dst_vertex_"
split -l10 --numeric-suffixes=1 --suffix-length=3 --additional-suffix=".list" dst_bbc_g4hit.list "dst_bbc_g4hit_"

if [ -f index.txt ]
then
    rm index.txt
fi

for i in {1..150..1}
do
  printf "%0*d\n" 3 $i >> index.txt
done

cd $DIR



cd lists/jet_bg

echo "entering directory `pwd`"

CreateFileList.pl -run 40 -type 4 DST_BBC_G4HIT DST_CALO_CLUSTER DST_CALO_G4HIT DST_VERTEX

split -l50 --numeric-suffixes=1 --suffix-length=3 --additional-suffix=".list" dst_calo_g4hit.list "dst_calo_g4hit_"
split -l50 --numeric-suffixes=1 --suffix-length=3 --additional-suffix=".list" dst_calo_cluster.list "dst_calo_cluster_"
split -l50 --numeric-suffixes=1 --suffix-length=3 --additional-suffix=".list" dst_vertex.list "dst_vertex_"
split -l50 --numeric-suffixes=1 --suffix-length=3 --additional-suffix=".list" dst_bbc_g4hit.list "dst_bbc_g4hit_"

if [ -f index.txt ]
then
    rm index.txt
fi

for i in {1..200..1}
do
  printf "%0*d\n" 3 $i >> index.txt
done




cd $DIR

cd lists/trk

echo "entering directory `pwd`"

CreateFileList.pl -type 11 -embed DST_BBC_G4HIT DST_CALO_CLUSTER DST_CALO_G4HIT DST_TRUTH_JET DST_VERTEX DST_TRACKS

split -l22 --numeric-suffixes=1 --suffix-length=2 --additional-suffix=".list" dst_truth_jet.list "dst_truth_jet_"
split -l22 --numeric-suffixes=1 --suffix-length=2 --additional-suffix=".list" dst_calo_g4hit.list "dst_calo_g4hit_"
split -l22 --numeric-suffixes=1 --suffix-length=2 --additional-suffix=".list" dst_calo_cluster.list "dst_calo_cluster_"
split -l22 --numeric-suffixes=1 --suffix-length=2 --additional-suffix=".list" dst_vertex.list "dst_vertex_"
split -l22 --numeric-suffixes=1 --suffix-length=2 --additional-suffix=".list" dst_bbc_g4hit.list "dst_bbc_g4hit_"
split -l22 --numeric-suffixes=1 --suffix-length=2 --additional-suffix=".list" dst_tracks.list "dst_tracks_"

if [ -f index.txt ]
then
    rm index.txt
fi

for i in {1..98..1}
do
  printf "%0*d\n" 2 $i >> index.txt
done



#embedd: part must come from same vtx as AuAu
#look at true vtx, put pythia event there
#not reading pre-processed

#run 4: different GEANT version


#for i in {1..800..1}
#do
#  printf "%0*d\n" 3 $i >> index.txt
#done


# rm dst_vertex_*.list dst_truth_jet_*.list dst_calo_g4hit_*.list dst_calo_cluster_*.list dst_bbc_g4hit_*.list
