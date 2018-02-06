#include <TreeMaker.h>
#include <TTree.h>



int TreeMaker::InitializeTree(void)
{

  tree = new TTree("ttree","sequoia");

  tree->Branch("jet2_n", &b_jet2_n,"jet2_n/I");
  tree->Branch("jet2_e", b_jet2_e,"jet2_e[jet2_n]/F");
  tree->Branch("jet2_pt", b_jet2_pt,"jet2_pt[jet2_n]/F");
  tree->Branch("jet2_eta",b_jet2_eta,"jet2_eta[jet2_n]/F");
  tree->Branch("jet2_phi",b_jet2_phi,"jet2_phi[jet2_n]/F");

  tree->Branch("jet3_n", &b_jet3_n,"jet3_n/I");
  tree->Branch("jet3_e", b_jet3_e,"jet3_e[jet3_n]/F");
  tree->Branch("jet3_pt", b_jet3_pt,"jet3_pt[jet3_n]/F");
  tree->Branch("jet3_eta",b_jet3_eta,"jet3_eta[jet3_n]/F");
  tree->Branch("jet3_phi",b_jet3_phi,"jet3_phi[jet3_n]/F");

  tree->Branch("jet4_n", &b_jet4_n,"jet4_n/I");
  tree->Branch("jet4_e", b_jet4_e,"jet4_e[jet4_n]/F");
  tree->Branch("jet4_pt", b_jet4_pt,"jet4_pt[jet4_n]/F");
  tree->Branch("jet4_eta",b_jet4_eta,"jet4_eta[jet4_n]/F");
  tree->Branch("jet4_phi",b_jet4_phi,"jet4_phi[jet4_n]/F");

  tree->Branch("jet5_n", &b_jet5_n,"jet5_n/I");
  tree->Branch("jet5_e", b_jet5_e,"jet5_e[jet5_n]/F");
  tree->Branch("jet5_pt", b_jet5_pt,"jet5_pt[jet5_n]/F");
  tree->Branch("jet5_eta",b_jet5_eta,"jet5_eta[jet5_n]/F");
  tree->Branch("jet5_phi",b_jet5_phi,"jet5_phi[jet5_n]/F");

  tree->Branch("modjet2_n", &b_modjet2_n,"modjet2_n/I");
  tree->Branch("modjet2_e", b_modjet2_e,"modjet2_e[modjet2_n]/F");
  tree->Branch("modjet2_pt", b_modjet2_pt,"modjet2_pt[modjet2_n]/F");
  tree->Branch("modjet2_eta",b_modjet2_eta,"modjet2_eta[modjet2_n]/F");
  tree->Branch("modjet2_phi",b_modjet2_phi,"modjet2_phi[modjet2_n]/F");

  tree->Branch("truthjet2_n", &b_truthjet2_n,"truthjet2_n/I");
  tree->Branch("truthjet2_e", b_truthjet2_e,"truthjet2_e[truthjet2_n]/F");
  tree->Branch("truthjet2_pt", b_truthjet2_pt,"truthjet2_pt[truthjet2_n]/F");
  tree->Branch("truthjet2_eta",b_truthjet2_eta,"truthjet2_eta[truthjet2_n]/F");
  tree->Branch("truthjet2_phi",b_truthjet2_phi,"truthjet2_phi[truthjet2_n]/F");

  tree->Branch("truthjet3_n", &b_truthjet3_n,"truthjet3_n/I");
  tree->Branch("truthjet3_e", b_truthjet3_e,"truthjet3_e[truthjet3_n]/F");
  tree->Branch("truthjet3_pt", b_truthjet3_pt,"truthjet3_pt[truthjet3_n]/F");
  tree->Branch("truthjet3_eta",b_truthjet3_eta,"truthjet3_eta[truthjet3_n]/F");
  tree->Branch("truthjet3_phi",b_truthjet3_phi,"truthjet3_phi[truthjet3_n]/F");

  tree->Branch("truthjet4_n", &b_truthjet4_n,"truthjet4_n/I");
  tree->Branch("truthjet4_e", b_truthjet4_e,"truthjet4_e[truthjet4_n]/F");
  tree->Branch("truthjet4_pt", b_truthjet4_pt,"truthjet4_pt[truthjet4_n]/F");
  tree->Branch("truthjet4_eta",b_truthjet4_eta,"truthjet4_eta[truthjet4_n]/F");
  tree->Branch("truthjet4_phi",b_truthjet4_phi,"truthjet4_phi[truthjet4_n]/F");

  tree->Branch("truthjet5_n", &b_truthjet5_n,"truthjet5_n/I");
  tree->Branch("truthjet5_e", b_truthjet5_e,"truthjet5_e[truthjet5_n]/F");
  tree->Branch("truthjet5_pt", b_truthjet5_pt,"truthjet5_pt[truthjet5_n]/F");
  tree->Branch("truthjet5_eta",b_truthjet5_eta,"truthjet5_eta[truthjet5_n]/F");
  tree->Branch("truthjet5_phi",b_truthjet5_phi,"truthjet5_phi[truthjet5_n]/F");

  return 0;

}



int TreeMaker::InitializeCounters()
{

  b_tower_n = 0;

  b_tower_total_0 = 0;
  b_tower_total_1 = 0;
  b_tower_total_2 = 0;

  b_truthjet2_n = 0;
  b_truthjet3_n = 0;
  b_truthjet4_n = 0;
  b_truthjet5_n = 0;

  b_jet2_n = 0;
  b_jet3_n = 0;
  b_jet4_n = 0;
  b_jet5_n = 0;

  b_modjet2_n = 0;

  return 0;

}

