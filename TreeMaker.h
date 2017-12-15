#ifndef __TREEMAKER_H__
#define __TREEMAKER_H__

// --- need to check all these includes...
#include <fun4all/SubsysReco.h>
#include <vector>

#include "TTree.h"
#include "TFile.h"

class PHCompositeNode;

class TreeMaker: public SubsysReco
{

 public:

  TreeMaker(const std::string &name="TreeMaker.root");

  int Init(PHCompositeNode*);
  int process_event(PHCompositeNode*);
  int End(PHCompositeNode*);

 private:

  TFile *outfile;

  TTree *tree;

  std::string foutname;

  int b_tower_n;
  int b_tower_layer[10000];
  float b_tower_e[10000];
  float b_tower_eta[10000];
  float b_tower_phi[10000];

  float b_tower_total_0;
  float b_tower_total_1;
  float b_tower_total_2;

  int bbkg_n;
  int bbkg_layer[100];
  int bbkg_eta[100];
  float bbkg_e[100];

  int bbkg2_n;
  int bbkg2_layer[100];
  int bbkg2_eta[100];
  float bbkg2_e[100];

  int b_cluster_n;
  float b_cluster_e[500];
  float b_cluster_pt[500];
  float b_cluster_eta[500];
  float b_cluster_phi[500];
  int b_cluster_layer[500];

  int b_jet5_n;
  float b_jet5_e[200];
  float b_jet5_pt[200];
  float b_jet5_eta[200];
  float b_jet5_phi[200];

  int b_jet4_n;
  float b_jet4_e[200];
  float b_jet4_pt[200];
  float b_jet4_eta[200];
  float b_jet4_phi[200];

  int b_jet3_n;
  float b_jet3_e[200];
  float b_jet3_pt[200];
  float b_jet3_eta[200];
  float b_jet3_phi[200];

  int b_jet2_n;
  float b_jet2_e[200];
  float b_jet2_pt[200];
  float b_jet2_eta[200];
  float b_jet2_phi[200];

  int b_jet4sub_n;
  float b_jet4sub_e[200];
  float b_jet4sub_pt[200];
  float b_jet4sub_eta[200];
  float b_jet4sub_phi[200];

  int b_jet5sub_n;
  float b_jet5sub_e[200];
  float b_jet5sub_pt[200];
  float b_jet5sub_eta[200];
  float b_jet5sub_phi[200];

  int b_jet3sub_n;
  float b_jet3sub_e[200];
  float b_jet3sub_pt[200];
  float b_jet3sub_eta[200];
  float b_jet3sub_phi[200];

  int b_jet2sub_n;
  float b_jet2sub_e[200];
  float b_jet2sub_pt[200];
  float b_jet2sub_eta[200];
  float b_jet2sub_phi[200];

  int b_jet2seed_n;
  float b_jet2seed_e[200];
  float b_jet2seed_pt[200];
  float b_jet2seed_eta[200];
  float b_jet2seed_phi[200];

  int b_truthjet5_n;
  float b_truthjet5_e[200];
  float b_truthjet5_pt[200];
  float b_truthjet5_eta[200];
  float b_truthjet5_phi[200];

  int b_truthjet4_n;
  float b_truthjet4_e[200];
  float b_truthjet4_pt[200];
  float b_truthjet4_eta[200];
  float b_truthjet4_phi[200];

  int b_truthjet3_n;
  float b_truthjet3_e[200];
  float b_truthjet3_pt[200];
  float b_truthjet3_eta[200];
  float b_truthjet3_phi[200];

  int b_truthjet2_n;
  float b_truthjet2_e[200];
  float b_truthjet2_pt[200];
  float b_truthjet2_eta[200];
  float b_truthjet2_phi[200];

  float b_trigEM2_e;
  float b_trigEM2_eta;
  float b_trigEM2_phi;

  float b_trigEM4_e;
  float b_trigEM4_eta;
  float b_trigEM4_phi;

  float b_trigFull2_e;
  float b_trigFull4_e;
  float b_trigFull6_e;
  float b_trigFull8_e;
  float b_trigFull10_e;

  float b_trigFull2_eta;
  float b_trigFull4_eta;
  float b_trigFull6_eta;
  float b_trigFull8_eta;
  float b_trigFull10_eta;

  float b_trigFull2_phi;
  float b_trigFull4_phi;
  float b_trigFull6_phi;
  float b_trigFull8_phi;
  float b_trigFull10_phi;

  int b_particle_n;
  float b_particle_e[1000];
  float b_particle_pt[1000];
  float b_particle_eta[1000];
  float b_particle_phi[1000];
  int b_particle_pid[1000];

};

#endif // __TREEMAKER_H__
