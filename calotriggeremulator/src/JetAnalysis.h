
#ifndef JETANALYSIS_H
#define JETANALYSIS_H

#include <fun4all/SubsysReco.h>
#include <calotrigger/TriggerAnalyzer.h>
#include <string>
#include <vector>
#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfoContainerv1.h>
#include <calobase/TowerInfoContainerv2.h>
#include <calobase/TowerInfoContainerv3.h>
#include <calobase/TowerInfo.h>
#include <calobase/TowerInfov1.h>
#include <calobase/TowerInfov2.h>
#include <calobase/TowerInfov3.h>
// jet includes
#include <jetbase/Jet.h>
#include <jetbase/JetContainer.h>
#include <jetbase/JetContainerv1.h>
#include <jetbase/Jetv2.h>

#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TFile.h"


class PHCompositeNode;
class TowerInfoContainer;
class JetContainer;
class PHG4TruthInfoContainer;
class JetAnalysis : public SubsysReco
{
 public:

  JetAnalysis(const std::string &name = "JetAnalysis", const std::string &outfilename = "hist_jetanalysis.root");

  virtual ~JetAnalysis();

  int Init(PHCompositeNode *topNode) override;

  int InitRun(PHCompositeNode *topNode) override;

  int process_event(PHCompositeNode *topNode) override;
  
  void GetNodes (PHCompositeNode *topNode);

  int ResetEvent(PHCompositeNode *topNode) override;

  int EndRun(const int runnumber) override;

  int End(PHCompositeNode *topNode) override;

  int Reset(PHCompositeNode * /*topNode*/) override;

  void SetVerbosity(int verbo) ;
  void SetIsSim(bool use) {isSim = use;}

  void SetPtCut(float pt) {pt_cut = pt;}
  Double_t getDPHI(Double_t phi1, Double_t phi2);
 private:
  TriggerAnalyzer *triggeranalyzer;  
  void reset_tree_vars();

  int _verbosity;

  float pt_cut = 4;

  TFile *_f;
  TTree *_tree;
  std::string _trigger;
  std::string _foutname;
  std::string _nodename;
  std::string m_ll1_nodename;
  std::string m_ll1_raw_nodename;
  std::string m_calo_nodename;
  int _i_event;
  bool useCaloTowerBuilder;
  bool useLL1;
  bool isSim{0};
  LL1Out *_ll1out_pair_trigger;
  LL1Out *_ll1out_photon_trigger;
  LL1Out *_ll1out_jet_trigger;
  LL1Out *_ll1out_raw_trigger;
  Gl1Packet *_gl1_packet;
  TriggerPrimitive *_trigger_primitive;
  TriggerPrimitiveContainer *_trigger_primitives_raw_emcal;
  TriggerPrimitiveContainer *_trigger_primitives_raw_emcal_ll1;
  TriggerPrimitiveContainer *_trigger_primitives_raw_trigger;
  TriggerPrimitiveContainer *_trigger_primitives_emcal;
  TriggerPrimitiveContainer *_trigger_primitives_emcal_ll1;
  TriggerPrimitiveContainer *_trigger_primitives_hcalin;
  TriggerPrimitiveContainer *_trigger_primitives_hcalout;
  TriggerPrimitiveContainer *_trigger_primitives_hcal_ll1;
  TriggerPrimitiveContainer *_trigger_primitives_trigger;

  TowerInfo *_tower;
  TowerInfoContainer* _towers;

  /* const int emcal_nsums = 6144; // number of sums in all of emcal 4*12*4*32; */
  /* const int hcal_nsums = 384; // number of sums in all of emcal 12*32; */
  /* const int emcal_ll1_nsums = 384; // number of 8x8 sums in all of emcal 4*12*4*32 /16; */
  /* const int jet_nsums = 288; //32*9; */

  unsigned int b_trigger_sum_smpl_emcal[6144];
  unsigned int b_trigger_sumkey_emcal[6144];
  unsigned int b_trigger_sum_emcal[6144];  
  unsigned int b_trigger_sum_smpl_hcalin[384];
  unsigned int b_trigger_sumkey_hcalin[384];
  unsigned int b_trigger_sum_hcalin[384];
  unsigned int b_trigger_sum_smpl_hcalout[384];
  unsigned int b_trigger_sumkey_hcalout[384];
  unsigned int b_trigger_sum_hcalout[384];

  std::vector<unsigned int> b_triggered_sums_jet;
  std::vector<unsigned int> b_triggered_sums_photon;
  std::vector<unsigned int> b_triggered_sums_pair;
  uint64_t b_gl1_rawvec;
  uint64_t b_gl1_livevec;
  uint64_t b_gl1_scaledvec;
  uint64_t b_gl1_clock;
  uint64_t b_gl1_raw[64]; 
  uint64_t b_gl1_live[64]; 
  uint64_t b_gl1_scaled[64]; 
  unsigned int b_trigger_sum_smpl_emcal_ll1[384];
  unsigned int b_trigger_sumkey_emcal_ll1[384];
  unsigned int b_trigger_sum_emcal_ll1[384];
  unsigned int b_trigger_sum_smpl_hcal_ll1[384];
  unsigned int b_trigger_sumkey_hcal_ll1[384];
  unsigned int b_trigger_sum_hcal_ll1[384];

  unsigned int b_trigger_sum_smpl_jet[288];
  unsigned int b_trigger_sumkey_jet[288];
  unsigned int b_trigger_sum_jet[288];

  unsigned int b_trigger_sum_smpl_jet_input[384];
  unsigned int b_trigger_sumkey_jet_input[384];
  unsigned int b_trigger_sum_jet_input[384];

  unsigned int b_trigger_raw_sum_smpl_emcal[6144];
  unsigned int b_trigger_raw_sumkey_emcal[6144];
  unsigned int b_trigger_raw_sum_emcal[6144];

  unsigned int b_trigger_raw_sum_smpl_emcal_ll1[384];
  unsigned int b_trigger_raw_sumkey_emcal_ll1[384];
  unsigned int b_trigger_raw_sum_emcal_ll1[384];

  unsigned int b_trigger_raw_sum_smpl_jet[288];
  unsigned int b_trigger_raw_sumkey_jet[288];
  unsigned int b_trigger_raw_sum_jet[288];

  unsigned int b_trigger_raw_sum_smpl_jet_input[384];
  unsigned int b_trigger_raw_sumkey_jet_input[384];
  unsigned int b_trigger_raw_sum_jet_input[384];
  unsigned int b_trigger_bits_photon;
  unsigned int b_trigger_bits_jet;
  unsigned int b_trigger_bits_pair;
  unsigned int b_trigger_raw_bits;

  float b_vertex_x;
  float b_vertex_y;
  float b_vertex_z;

  int b_cluster_n;
  std::vector<float> b_cluster_prob;
  std::vector<float> b_cluster_chi2;
  std::vector<float> b_cluster_ecore;
  std::vector<float> b_cluster_pt;
  std::vector<float> b_cluster_phi;
  std::vector<float> b_cluster_eta;
  std::vector<float> b_cluster_z;
  std::vector<float> b_cluster_y;
  std::vector<float> b_cluster_x;
  std::vector<float> b_cluster_iso;

  std::vector<short> b_emcal_good;
  std::vector<float> b_emcal_energy;
  std::vector<float> b_emcal_time;
  std::vector<float> b_emcal_etabin;
  std::vector<float> b_emcal_phibin;

  std::vector<short> b_emcalre_good;
  std::vector<float> b_emcalre_energy;
  std::vector<float> b_emcalre_time;
  std::vector<float> b_emcalre_etabin;
  std::vector<float> b_emcalre_phibin;

  std::vector<short> b_hcalin_good;
  std::vector<float> b_hcalin_energy;
  std::vector<float> b_hcalin_time;
  std::vector<float> b_hcalin_etabin;
  std::vector<float> b_hcalin_phibin;

  std::vector<short> b_hcalout_good;
  std::vector<float> b_hcalout_energy;
  std::vector<float> b_hcalout_time;
  std::vector<float> b_hcalout_etabin;
  std::vector<float> b_hcalout_phibin;

  std::vector<short> b_emcalresub_good;
  std::vector<float> b_emcalresub_energy;
  std::vector<float> b_emcalresub_time;
  std::vector<float> b_emcalresub_etabin;
  std::vector<float> b_emcalresub_phibin;

  std::vector<short> b_hcalinsub_good;
  std::vector<float> b_hcalinsub_energy;
  std::vector<float> b_hcalinsub_time;
  std::vector<float> b_hcalinsub_etabin;
  std::vector<float> b_hcalinsub_phibin;

  std::vector<short> b_hcaloutsub_good;
  std::vector<float> b_hcaloutsub_energy;
  std::vector<float> b_hcaloutsub_time;
  std::vector<float> b_hcaloutsub_etabin;
  std::vector<float> b_hcaloutsub_phibin;

  float b_mbd_charge[128];
  float b_mbd_time[128];
  int b_mbd_ipmt[128];
  int b_mbd_side[128];
  int b_truth_particle_n = 0;
  std::vector<int> b_truth_particle_pid = {};
  std::vector<float> b_truth_particle_pt = {};
  std::vector<float>  b_truth_particle_eta = {};
  std::vector<float>  b_truth_particle_phi = {};

  int b_njet_2 = 0;
  std::vector<float> b_jet_pt_2 = {};
  std::vector<float> b_jet_et_2 = {};
  std::vector<float> b_jet_mass_2 = {};
  std::vector<float> b_jet_eta_2 = {};
  std::vector<float> b_jet_phi_2 = {};
  std::vector<float> b_jet_emcal_frac_2 = {};
  std::vector<float> b_jet_emcal_tower_frac_2 = {};
  std::vector<float> b_jet_hcalin_frac_2 = {};
  std::vector<float> b_jet_hcalout_frac_2 = {};

  int b_njet_4 = 0;
  std::vector<float> b_jet_pt_4 = {};
  std::vector<float> b_jet_et_4 = {};
  std::vector<float> b_jet_mass_4 = {};
  std::vector<float> b_jet_eta_4 = {};
  std::vector<float> b_jet_phi_4 = {};
  std::vector<float> b_jet_emcal_4 = {};
  std::vector<float> b_jet_et_em_4 = {};
  std::vector<float> b_jet_emcal_tower_frac_4 = {};
  std::vector<float> b_jet_hcalin_4 = {};
  std::vector<float> b_jet_hcalout_4 = {};

  int b_njet_4nosub = 0;
  std::vector<float> b_jet_pt_4nosub = {};
  std::vector<float> b_jet_et_4nosub = {};
  std::vector<float> b_jet_mass_4nosub = {};
  std::vector<float> b_jet_eta_4nosub = {};
  std::vector<float> b_jet_phi_4nosub = {};
  std::vector<float> b_jet_emcal_frac_4nosub = {};
  std::vector<float> b_jet_emcal_tower_frac_4nosub = {};
  std::vector<float> b_jet_hcalin_frac_4nosub = {};
  std::vector<float> b_jet_hcalout_frac_4nosub = {};

  int b_njet_6 = 0;
  std::vector<float> b_jet_pt_6 = {};
  std::vector<float> b_jet_et_6 = {};
  std::vector<float> b_jet_mass_6 = {};
  std::vector<float> b_jet_eta_6 = {};
  std::vector<float> b_jet_phi_6 = {};
  std::vector<float> b_jet_emcal_frac_6 = {};
  std::vector<float> b_jet_emcal_tower_frac_6 = {};
  std::vector<float> b_jet_hcalin_frac_6 = {};
  std::vector<float> b_jet_hcalout_frac_6 = {};

  int trash;
  int r1;
  float emcal_energy;
  float energy;
  float spread;


};

#endif 
