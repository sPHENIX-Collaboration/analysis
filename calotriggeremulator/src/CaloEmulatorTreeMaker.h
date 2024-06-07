#ifndef CALOEMULATORTREEMAKER_H
#define CALOEMULATORTREEMAKER_H

#include <fun4all/SubsysReco.h>

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

#include "TTree.h"
#include "TFile.h"


class PHCompositeNode;
class TriggerPrimitive;
class TriggerPrimitiveContainer;
class LL1Out;
class TowerInfoContainer;
class CaloEmulatorTreeMaker : public SubsysReco
{
 public:

  CaloEmulatorTreeMaker(const std::string &name = "CaloEmulatorTreeMaker", const std::string &outfilename = "trees_caloemulator.root");

  virtual ~CaloEmulatorTreeMaker();

  int Init(PHCompositeNode *topNode) override;

  int InitRun(PHCompositeNode *topNode) override;

  int process_event(PHCompositeNode *topNode) override;
  
  void GetNodes (PHCompositeNode *topNode);

  int ResetEvent(PHCompositeNode *topNode) override;

  int EndRun(const int runnumber) override;

  int End(PHCompositeNode *topNode) override;

  int Reset(PHCompositeNode * /*topNode*/) override;

  void SetVerbosity(int verbo) ;
  void SetTrigger(const std::string trigger) {_trigger = trigger;}
  void UseCaloTowerBuilder(bool use) {useCaloTowerBuilder = use;}
  void UseLL1(bool use) {useLL1 = use;}
 private:
  void reset_tree_vars();

  int _verbosity;

  TFile *_f;
  TTree *_tree;
  std::string _trigger;
  std::string _foutname;
  std::string _nodename;
  std::string m_ll1_nodename;
  std::string m_ll1_raw_nodename;
  int _i_event;
  bool useCaloTowerBuilder;
  bool useLL1;

  LL1Out *_ll1out_trigger;
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

  std::vector<unsigned int> b_triggered_sums;
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
  unsigned int b_trigger_bits;
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
  std::vector<float> b_cluster_iso;

  std::vector<short> b_emcal_good;
  std::vector<float> b_emcal_energy;
  std::vector<float> b_emcal_time;
  std::vector<float> b_emcal_etabin;
  std::vector<float> b_emcal_phibin;

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

  float b_mbd_charge[128];
  float b_mbd_time[128];
  int b_mbd_ipmt[128];
  int b_mbd_side[128];
};

#endif 
