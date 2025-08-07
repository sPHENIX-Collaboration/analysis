#ifndef MIXEDJETANALYSIS_H
#define MIXEDJETANALYSIS_H

#include <fun4all/SubsysReco.h>
#include <string>
#include <vector>
#include <ffarawobjects/Gl1Packet.h>
#include <ffarawobjects/Gl1Packetv1.h>
#include <ffarawobjects/Gl1Packetv2.h>

#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfoContainerv1.h>
#include <calobase/TowerInfoContainerv2.h>
#include <calobase/TowerInfoContainerv3.h>
#include <calobase/TowerInfo.h>
#include <calobase/TowerInfov1.h>
#include <calobase/TowerInfov2.h>
#include <calobase/TowerInfov3.h>
#include <calobase/RawTowerGeom.h>// jet includes
#include <calobase/RawCluster.h>
#include <calobase/RawClusterContainer.h>
#include <calobase/RawClusterUtility.h>
#include <calobase/RawTowerGeomContainer.h>
#include <calobase/RawTower.h>
#include <calobase/RawTowerContainer.h>
#include <calotrigger/MinimumBiasInfov1.h>
#include <jetbase/Jet.h>
#include <jetbase/JetContainer.h>
#include <jetbase/JetContainerv1.h>
#include <jetbase/Jetv2.h>

#include "TTree.h"
#include "TFile.h"
#include "TVector3.h"

class Gl1Packet;
class CentralityInfo;
class EventplaneinfoMap;
class TowerBackground;
class MinimumBiasInfo;
class TVector3;
class PHCompositeNode;
class Jet;
class JetContainer;
class MixedJetAnalysis : public SubsysReco
{
 public:

  MixedJetAnalysis(const std::string &name = "MixedJetAnalysis", const std::string &outfilename = "trees_caloemulator.root");

  virtual ~MixedJetAnalysis();

  int Init(PHCompositeNode */*topNode*/) override;

  int process_event(PHCompositeNode *topNode) override;

  int process_jets(int cone_index, PHCompositeNode *topNode);

  int store_mixed_events(int cone_index);
  
  int get_mixed_events(int cone_index);

  int show_queue(int cone_index);
    
  int End(PHCompositeNode * /*topNode*/) override;

  void SetPtCut(float pt) {pt_cut = pt;}
  void SetMaxPtCut(float pt) {signal_pt_cut = pt;}
  void usePsi(bool use) { m_use_psi = use; }
  
  void addConeSize(int conesize, bool bkg = false) { m_cone_sizes[m_n_cone_sizes++] = std::make_pair(conesize, bkg); }

  void setRunnumber(int run){b_runnumber = run;}

  void setSegment(int seg){b_segment = seg;}

  struct mixedjet
  {
    float pt = 0;
    float eta = 0;
    float phi = 0;
  };
    
 private:

  int m_n_cone_sizes = 0;
  std::map<int, std::pair<int, bool>> m_cone_sizes{};
  
  void reset_tree_vars();
  
  float m_higheta = 1.1;
  float m_loweta = -1.1;
  bool m_use_psi{false};
  float pt_cut = 3;
  float signal_pt_cut = 20;

  float vertex_cut = 60;
  float vertex_bin_width = 5;
  TFile *_f;
  TTree *_tree;
  TTree *_tree_end;

  std::string _foutname;
  std::string _nodename;
  std::string m_calo_nodename;

  int _i_event;

  TowerBackground *m_towback = nullptr;
  CentralityInfo *m_central = nullptr;
  MinimumBiasInfo *m_minimumbiasinfo = nullptr;
  EventplaneinfoMap *m_evpmap = nullptr;
  
  int b_centrality = 0;
  float b_psi = 0;
  float b_v2 = 0;
  int b_flow_fail = 0;
  int b_ismb = 0;
  unsigned int m_total_bin = 0;

  int b_runnumber = 0;
  int b_segment = 0;
  ULong64_t first_scaled[64];
  ULong64_t first_live[64];
  ULong64_t first_raw[64];
  ULong64_t last_scaled[64];
  ULong64_t last_live[64];
  ULong64_t last_raw[64];
  ULong64_t b_scaled_scalers[64];
  ULong64_t b_live_scalers[64];
  ULong64_t b_raw_scalers[64];
  
  float b_vertex_z;

  float b_time_zero;

  int b_mbd_npmt;
  std::vector<float> b_mbd_charge;
  std::vector<float> b_mbd_time;
  std::vector<short> b_mbd_ipmt;
  std::vector<short> b_mbd_side;

  std::vector<int> b_njet{};
  std::vector<std::vector<float>*> b_jet_pt;
  std::vector<std::vector<float>*> b_jet_eta;
  std::vector<std::vector<float>*> b_jet_phi;

  std::vector<int> b_njet_mix{};
  std::vector<std::vector<float>*> b_jet_mix_pt;
  std::vector<std::vector<float>*> b_jet_mix_eta;
  std::vector<std::vector<float>*> b_jet_mix_phi;

  std::vector<std::map<unsigned int, std::vector<std::vector<struct mixedjet>>*>> m_mixed_jet_bank;

  ULong64_t b_gl1_scaled{0};
  ULong64_t b_gl1_live{0};
  ULong64_t b_gl1_raw{0};
  unsigned int m_nbin_vertex = 10;
  unsigned int m_nbin_psi = 8;
  float m_vertex_bin_edges[11] = {-60, -30, -20, -10, -5, 0, 5, 10, 20, 30 , 60}; 

  unsigned int m_max_length_buffer = 20;
};

#endif 
