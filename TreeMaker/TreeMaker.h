#ifndef __TREEMAKER_H__
#define __TREEMAKER_H__

// --- need to check all these includes...
#include <fun4all/SubsysReco.h>

#include "TTree.h"
#include "TFile.h"

class PHCompositeNode;

class TreeMaker: public SubsysReco
{

 public:

  TreeMaker(const std::string &name="TreeMaker.root");
  ~TreeMaker();

  int Init(PHCompositeNode*);
  int InitRun(PHCompositeNode*);
  int process_event(PHCompositeNode*);
  int End(PHCompositeNode*);
  int CreateNode(PHCompositeNode*);

  int InitializeTree();
  int InitializeCounters();

  int GetTruthJets(PHCompositeNode*);
  int GetRecoJets(PHCompositeNode*);
  int GetClusters(PHCompositeNode*);
  int CopyAndMakeJets(PHCompositeNode*);
  int CopyAndMakeClusters(PHCompositeNode*);
  int UseFastJet(PHCompositeNode*);

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

  int b_modjet2_n;
  float b_modjet2_e[200];
  float b_modjet2_pt[200];
  float b_modjet2_eta[200];
  float b_modjet2_phi[200];

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

};

#endif // __TREEMAKER_H__

