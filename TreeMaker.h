#ifndef __TRUTHJETTRIGGER_H__
#define __TRUTHJETTRIGGER_H__

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

  TFile *_f;

  TTree *_tree;

  std::string _foutname;

  int _b_tower_n;
  int _b_tower_layer[10000];
  float _b_tower_E[10000];
  float _b_tower_eta[10000];
  float _b_tower_phi[10000];

  float _b_tower_total_0;
  float _b_tower_total_1;
  float _b_tower_total_2;

  int _b_bkg_n;
  int _b_bkg_layer[100];
  int _b_bkg_eta[100];
  float _b_bkg_E[100];

  int _b_bkg2_n;
  int _b_bkg2_layer[100];
  int _b_bkg2_eta[100];
  float _b_bkg2_E[100];

  int _b_cluster_n;
  float _b_cluster_pt[500];
  float _b_cluster_eta[500];
  float _b_cluster_phi[500];
  int _b_cluster_layer[500];

  int _b_jet5_n;
  float _b_jet5_pt[200];
  float _b_jet5_eta[200];
  float _b_jet5_phi[200];

  int _b_jet4_n;
  float _b_jet4_pt[200];
  float _b_jet4_eta[200];
  float _b_jet4_phi[200];

  int _b_jet3_n;
  float _b_jet3_pt[200];
  float _b_jet3_eta[200];
  float _b_jet3_phi[200];

  int _b_jet2_n;
  float _b_jet2_pt[200];
  float _b_jet2_eta[200];
  float _b_jet2_phi[200];

  int _b_jet4sub_n;
  float _b_jet4sub_pt[200];
  float _b_jet4sub_eta[200];
  float _b_jet4sub_phi[200];

  int _b_jet5sub_n;
  float _b_jet5sub_pt[200];
  float _b_jet5sub_eta[200];
  float _b_jet5sub_phi[200];

  int _b_jet3sub_n;
  float _b_jet3sub_pt[200];
  float _b_jet3sub_eta[200];
  float _b_jet3sub_phi[200];

  int _b_jet2sub_n;
  float _b_jet2sub_pt[200];
  float _b_jet2sub_eta[200];
  float _b_jet2sub_phi[200];

  int _b_jet2seed_n;
  float _b_jet2seed_pt[200];
  float _b_jet2seed_eta[200];
  float _b_jet2seed_phi[200];

  int _b_truthjet5_n;
  float _b_truthjet5_pt[200];
  float _b_truthjet5_eta[200];
  float _b_truthjet5_phi[200];

  int _b_truthjet4_n;
  float _b_truthjet4_pt[200];
  float _b_truthjet4_eta[200];
  float _b_truthjet4_phi[200];

  int _b_truthjet3_n;
  float _b_truthjet3_pt[200];
  float _b_truthjet3_eta[200];
  float _b_truthjet3_phi[200];

  int _b_truthjet2_n;
  float _b_truthjet2_pt[200];
  float _b_truthjet2_eta[200];
  float _b_truthjet2_phi[200];

  float _b_trigEM2_E;
  float _b_trigEM2_eta;
  float _b_trigEM2_phi;

  float _b_trigEM4_E;
  float _b_trigEM4_eta;
  float _b_trigEM4_phi;

  float _b_trigFull2_E;
  float _b_trigFull4_E;
  float _b_trigFull6_E;
  float _b_trigFull8_E;
  float _b_trigFull10_E;

  float _b_trigFull2_eta;
  float _b_trigFull4_eta;
  float _b_trigFull6_eta;
  float _b_trigFull8_eta;
  float _b_trigFull10_eta;

  float _b_trigFull2_phi;
  float _b_trigFull4_phi;
  float _b_trigFull6_phi;
  float _b_trigFull8_phi;
  float _b_trigFull10_phi;

  int _b_particle_n;
  float _b_particle_pt[1000];
  float _b_particle_eta[1000];
  float _b_particle_phi[1000];
  int _b_particle_pid[1000];

};

#endif // __TRUTHJETTRIGGER_H__
