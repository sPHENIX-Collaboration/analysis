#include <TreeMaker.h>
#include <TTree.h>



int TreeMaker::InitializeTree(void)
{

  tree = new TTree("ttree","sequoia");

  tree->Branch("bkg_n",&bbkg_n, "bkg_n/I");
  tree->Branch("bkg_layer",&bbkg_layer, "bkg_layer[bkg_n]/I");
  tree->Branch("bkg_eta",&bbkg_eta, "bkg_eta[bkg_n]/I");
  tree->Branch("bkg_e",&bbkg_e, "bkg_e[bkg_n]/F");

  tree->Branch("bkg2_n",     &bbkg2_n,     "bkg2_n/I");
  tree->Branch("bkg2_layer", &bbkg2_layer, "bkg2_layer[bkg2_n]/I");
  tree->Branch("bkg2_eta",   &bbkg2_eta,   "bkg2_eta[bkg2_n]/I");
  tree->Branch("bkg2_e",     &bbkg2_e,     "bkg2_e[bkg2_n]/F");

  tree->Branch("particle_n", &b_particle_n,"particle_n/I");
  tree->Branch("particle_e", b_particle_e,"particle_e[particle_n]/F");
  tree->Branch("particle_pt", b_particle_pt,"particle_pt[particle_n]/F");
  tree->Branch("particle_eta", b_particle_eta,"particle_eta[particle_n]/F");
  tree->Branch("particle_phi", b_particle_phi,"particle_phi[particle_n]/F");
  tree->Branch("particle_pid", b_particle_pid,"particle_pid[particle_n]/I");

  tree->Branch("jet2seedraw_n", &b_jet2seedraw_n,"jet2seedraw_n/I");
  tree->Branch("jet2seedraw_e", b_jet2seedraw_e,"jet2seedraw_e[jet2seedraw_n]/F");
  tree->Branch("jet2seedraw_pt", b_jet2seedraw_pt,"jet2seedraw_pt[jet2seedraw_n]/F");
  tree->Branch("jet2seedraw_eta",b_jet2seedraw_eta,"jet2seedraw_eta[jet2seedraw_n]/F");
  tree->Branch("jet2seedraw_phi",b_jet2seedraw_phi,"jet2seedraw_phi[jet2seedraw_n]/F");

  tree->Branch("jet2seedsub_n", &b_jet2seedsub_n,"jet2seedsub_n/I");
  tree->Branch("jet2seedsub_e", b_jet2seedsub_e,"jet2seedsub_e[jet2seedsub_n]/F");
  tree->Branch("jet2seedsub_pt", b_jet2seedsub_pt,"jet2seedsub_pt[jet2seedsub_n]/F");
  tree->Branch("jet2seedsub_eta",b_jet2seedsub_eta,"jet2seedsub_eta[jet2seedsub_n]/F");
  tree->Branch("jet2seedsub_phi",b_jet2seedsub_phi,"jet2seedsub_phi[jet2seedsub_n]/F");

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

  tree->Branch("jet2sub_n", &b_jet2sub_n,"jet2sub_n/I");
  tree->Branch("jet2sub_e", b_jet2sub_e,"jet2sub_e[jet2sub_n]/F");
  tree->Branch("jet2sub_pt", b_jet2sub_pt,"jet2sub_pt[jet2sub_n]/F");
  tree->Branch("jet2sub_eta",b_jet2sub_eta,"jet2sub_eta[jet2sub_n]/F");
  tree->Branch("jet2sub_phi",b_jet2sub_phi,"jet2sub_phi[jet2sub_n]/F");

  tree->Branch("jet3sub_n", &b_jet3sub_n,"jet3sub_n/I");
  tree->Branch("jet3sub_e", b_jet3sub_e,"jet3sub_e[jet3sub_n]/F");
  tree->Branch("jet3sub_pt", b_jet3sub_pt,"jet3sub_pt[jet3sub_n]/F");
  tree->Branch("jet3sub_eta",b_jet3sub_eta,"jet3sub_eta[jet3sub_n]/F");
  tree->Branch("jet3sub_phi",b_jet3sub_phi,"jet3sub_phi[jet3sub_n]/F");

  tree->Branch("jet4sub_n", &b_jet4sub_n,"jet4sub_n/I");
  tree->Branch("jet4sub_e", b_jet4sub_e,"jet4sub_e[jet4sub_n]/F");
  tree->Branch("jet4sub_pt", b_jet4sub_pt,"jet4sub_pt[jet4sub_n]/F");
  tree->Branch("jet4sub_eta",b_jet4sub_eta,"jet4sub_eta[jet4sub_n]/F");
  tree->Branch("jet4sub_phi",b_jet4sub_phi,"jet4sub_phi[jet4sub_n]/F");

  tree->Branch("jet5sub_n", &b_jet5sub_n,"jet5sub_n/I");
  tree->Branch("jet5sub_e", b_jet5sub_e,"jet5sub_e[jet5sub_n]/F");
  tree->Branch("jet5sub_pt", b_jet5sub_pt,"jet5sub_pt[jet5sub_n]/F");
  tree->Branch("jet5sub_eta",b_jet5sub_eta,"jet5sub_eta[jet5sub_n]/F");
  tree->Branch("jet5sub_phi",b_jet5sub_phi,"jet5sub_phi[jet5sub_n]/F");

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

  b_jet2seedraw_n = 0;
  b_jet2seedsub_n = 0;

  b_jet2sub_n = 0;
  b_jet3sub_n = 0;
  b_jet4sub_n = 0;
  b_jet5sub_n = 0;

  bbkg_n = 0;

  return 0;

}

