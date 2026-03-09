
#ifndef DIJETTREEMAKER_H
#define DIJETTREEMAKER_H

#include <fun4all/SubsysReco.h>
#include <calotrigger/TriggerAnalyzer.h>
#include <string>
#include <vector>
#include <ffarawobjects/Gl1Packet.h>
#include <ffarawobjects/Gl1Packetv1.h>
#include <ffarawobjects/Gl1Packetv2.h>
#include <calotrigger/LL1Out.h>
#include <calotrigger/LL1Outv1.h>
#include <calotrigger/TriggerPrimitive.h>
#include <calotrigger/TriggerPrimitivev1.h>
#include <calotrigger/TriggerPrimitiveContainer.h>
#include <calotrigger/TriggerPrimitiveContainerv1.h>
#include <calotrigger/TriggerDefs.h>
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

#include "TTree.h"
#include "TFile.h"
#include "TVector3.h"

class TVector3;
class PHCompositeNode;
class TowerInfoContainer;
class JetContainer;
class PHG4TruthInfoContainer;
class DijetTreeMaker : public SubsysReco
{
 public:

  DijetTreeMaker(const std::string &name = "DijetTreeMaker", const std::string &outfilename = "trees_caloemulator.root");

  virtual ~DijetTreeMaker();

  int Init(PHCompositeNode *topNode) override;

  int InitRun(PHCompositeNode *topNode) override;

  int process_event(PHCompositeNode *topNode) override;
  
  void GetNodes (PHCompositeNode *topNode);

  int ResetEvent(PHCompositeNode *topNode) override;

  int EndRun(const int runnumber) override;

  int End(PHCompositeNode *topNode) override;

  int Reset(PHCompositeNode * /*topNode*/) override;
  float get_eccentricity(std::vector<TVector3> hcaltowers, float oh_sum);
  void SetVerbosity(int verbo) ;
  void SetTrigger(const std::string trigger) {_trigger = trigger;}
  void SetIsSim(bool use) {isSim = use;}
  void SetPtCut(float pt) {pt_cut = pt;}
  Double_t getDPHI(Double_t phi1, Double_t phi2);
  void saveCalo(bool save){save_calo = save;}
 private:
  TriggerAnalyzer *triggeranalyzer{nullptr};  
  void reset_tree_vars();

  int _verbosity;

  float dijetcut = 10.;
  float pt_cut = 4;
  float pt_cut_truth = 4;
  bool save_calo = true;
  TFile *_f;
  TTree *_tree;
  std::string _trigger;
  std::string _foutname;
  std::string _nodename;
  std::string m_calo_nodename;
  int _i_event;
  bool isSim{0};
  
  TowerInfo *_tower;
  TowerInfoContainer* _towers;

  float b_vertex_z;

  std::vector<short> b_emcal_good;
  std::vector<float> b_emcal_energy;
  std::vector<float> b_emcal_time;
  std::vector<float> b_emcal_etabin;
  std::vector<float> b_emcal_phibin;
  std::vector<float> b_emcal_eta;
  std::vector<float> b_emcal_phi;

  std::vector<short> b_hcalin_good;
  std::vector<float> b_hcalin_energy;
  std::vector<float> b_hcalin_time;
  std::vector<float> b_hcalin_etabin;
  std::vector<float> b_hcalin_phibin;
  std::vector<float> b_hcalin_eta;
  std::vector<float> b_hcalin_phi;

  std::vector<short> b_hcalout_good;
  std::vector<float> b_hcalout_energy;
  std::vector<float> b_hcalout_time;
  std::vector<float> b_hcalout_etabin;
  std::vector<float> b_hcalout_phibin;
  std::vector<float> b_hcalout_eta;
  std::vector<float> b_hcalout_phi;

  int b_truth_particle_n = 0;
  std::vector<int> b_truth_particle_pid = {};
  std::vector<float> b_truth_particle_pt = {};
  std::vector<float>  b_truth_particle_eta = {};
  std::vector<float>  b_truth_particle_phi = {};


  int b_njet_2 = 0;
  std::vector<float> b_jet_pt_2 = {};
  std::vector<float> b_jet_et_2 = {};
  std::vector<float> b_jet_eta_2 = {};
  std::vector<float> b_jet_phi_2 = {};
  std::vector<float> b_jet_emcal_2 = {};
  std::vector<float> b_jet_hcalin_2 = {};
  std::vector<float> b_jet_hcalout_2 = {};
  std::vector<float> b_jet_eccen_2 = {};

  int b_njet_4 = 0;
  std::vector<float> b_jet_pt_4 = {};
  std::vector<float> b_jet_et_4 = {};
  std::vector<float> b_jet_eta_4 = {};
  std::vector<float> b_jet_phi_4 = {};
  std::vector<float> b_jet_emcal_4 = {};
  std::vector<float> b_jet_hcalin_4 = {};
  std::vector<float> b_jet_hcalout_4 = {};
  std::vector<float> b_jet_eccen_4 = {};

  int b_njet_6 = 0;
  std::vector<float> b_jet_pt_6 = {};
  std::vector<float> b_jet_et_6 = {};
  std::vector<float> b_jet_eta_6 = {};
  std::vector<float> b_jet_phi_6 = {};
  std::vector<float> b_jet_emcal_6 = {};
  std::vector<float> b_jet_hcalin_6 = {};
  std::vector<float> b_jet_hcalout_6 = {};
  std::vector<float> b_jet_eccen_6 = {};

  int b_ntruth_jet_2 = 0;
  std::vector<float> b_truth_jet_pt_2 = {};
  std::vector<float> b_truth_jet_eta_2 = {};
  std::vector<float> b_truth_jet_phi_2 = {};

  int b_ntruth_jet_4 = 0;
  std::vector<float> b_truth_jet_pt_4 = {};
  std::vector<float> b_truth_jet_eta_4 = {};
  std::vector<float> b_truth_jet_phi_4 = {};

  int b_ntruth_jet_6 = 0;
  std::vector<float> b_truth_jet_pt_6 = {};
  std::vector<float> b_truth_jet_eta_6 = {};
  std::vector<float> b_truth_jet_phi_6 = {};

  int b_ncluster = 0;
  std::vector<float> b_cluster_e = {};
  std::vector<float> b_cluster_ecore = {};
  std::vector<float> b_cluster_eta = {};
  std::vector<float> b_cluster_phi = {};
  std::vector<float> b_cluster_pt = {};
  std::vector<float> b_cluster_chi2 = {};
  std::vector<float> b_cluster_prob = {};

};

#endif 
