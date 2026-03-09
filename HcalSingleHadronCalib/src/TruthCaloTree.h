// Truth Calorimeter Calibration TreeMaker

#ifndef __TRUTHCALOTREE_H__
#define __TRUTHCALOTREE_H__

#include <fun4all/SubsysReco.h>
#include <string>
#include <vector>
#include <map>
#include "TTree.h"
#include "TFile.h"
#include "TH2.h"
#include <set>

#include <g4main/PHG4TruthInfoContainer.h>
#include <phool/PHCompositeNode.h>
#include <g4main/PHG4Hit.h>
#include <g4main/PHG4HitContainer.h>

#include <calobase/RawTowerGeomContainer_Cylinderv1.h>
#include <calobase/RawTowerGeomContainer.h>
#include <calobase/RawTowerContainer.h>

class PHCompositeNode;
class RawTowerContainer;
class PHG4TruthInfoContainer;
class RawTowerGeomContainer;
class PHG4HitContainer;

class TruthCaloTree: public SubsysReco {
 public:
  
  TruthCaloTree(const std::string &name = "TruthCaloTree.root");
  
  virtual ~TruthCaloTree() {}
  
  int Init(PHCompositeNode *);
  
  int reset_tree_vars();

  int GetNodes(PHCompositeNode *);
  
  int process_event(PHCompositeNode *);
  
  int End(PHCompositeNode *);
  
 private:
  std::string _foutname; // outfile name
  std::string _hcal_sim_name;
  std::string _hcalIN_sim_name;
  std::string _EMcal_sim_name;

  int _startingSector, _numSectors; 
  RawTowerContainer* _towersSimOH;
  RawTowerGeomContainer* _geomOH;
  RawTowerGeomContainer* _geomIH;
  RawTowerGeomContainer* _geomEM;
  PHG4HitContainer* blackhole;
  RawTowerContainer* _towersSimIH;
  RawTowerContainer* _towersSimEM;
  PHG4TruthInfoContainer *truthinfo;

  TTree* _tree;
  TFile* _file;
  
  int _ievent, _b_event;

  static const bool _debug = false;
  static const int nTowers = 10000;
  static const int nTruth = 100000;
  static const int nTruths = 100000;
  static const bool recursive_shower = false;
  
  // outer Hcal
  
  int _b_tower_sim_n;
  float _b_tower_sim_E[nTowers];
  float _b_tower_sim_eta[nTowers];
  float _b_tower_sim_phi[nTowers];
  int   _b_tower_sim_ieta[nTowers];
  int   _b_tower_sim_iphi[nTowers];
  
  // inner HCal
  
  int   _b_hcalIN_sim_n = 0;
  float _b_hcalIN_sim_E[nTowers];
  float _b_hcalIN_sim_eta[nTowers];
  float _b_hcalIN_sim_phi[nTowers];
  int   _b_hcalIN_sim_ieta[nTowers];
  int   _b_hcalIN_sim_iphi[nTowers];

  // EMCal
  
  int   _b_EMcal_sim_n = 0;
  float _b_EMcal_sim_E[nTowers];
  float _b_EMcal_sim_eta[nTowers];
  float _b_EMcal_sim_phi[nTowers];
  int   _b_EMcal_sim_ieta[nTowers];
  int   _b_EMcal_sim_iphi[nTowers];

  // Primary truth particles 
  
  int _b_n_truth = 0;
  float _b_truthenergy[nTruth];
  float _b_trutheta[nTruth];
  float _b_truthphi[nTruth];
  float _b_truthpt[nTruth];
  float _b_truthp[nTruth];
  int _b_truthpid[nTruth];
  int _b_truth_trackid[nTruth];

  // Secondary truth particles

  int n_child = 0;
  int child_pid[nTruth];
  int child_parent_id[nTruth];
  int child_vertex_id[nTruth];
  float child_px[nTruth];
  float child_py[nTruth];
  float child_pz[nTruth];
  float child_energy[nTruth];

  // Truth particle vertices 

  int n_vertex = 0;
  int vertex_id[nTruth];
  float vertex_x[nTruth];
  float vertex_y[nTruth];
  float vertex_z[nTruth];

  // BH particles

  int _nBH = 0;
  float _BH_e[nTruth];
  float _BH_px[nTruth];
  float _BH_py[nTruth];
  float _BH_pz[nTruth];
  int _BH_track_id[nTruth];

};

#endif
