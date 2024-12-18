#ifndef HCALEMULATORTREEMAKER_H
#define HCALEMULATORTREEMAKER_H

#include <fun4all/SubsysReco.h>

#include <string>
#include <vector>

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
class HCALEmulatorTreeMaker : public SubsysReco
{
 public:

  HCALEmulatorTreeMaker(const std::string &name = "HCALEmulatorTreeMaker", const std::string &outfilename = "trees_mbd_trig.root", const std::string &nodename = "LL1OUT_HCAL_RAW");

  virtual ~HCALEmulatorTreeMaker();

  int Init(PHCompositeNode *topNode) override;

  int InitRun(PHCompositeNode *topNode) override;

  int process_event(PHCompositeNode *topNode) override;
  
  void GetNodes (PHCompositeNode *topNode);

  int ResetEvent(PHCompositeNode *topNode) override;

  int EndRun(const int runnumber) override;

  int End(PHCompositeNode *topNode) override;

  int Reset(PHCompositeNode * /*topNode*/) override;

  void SetVerbosity(int verbo) ;
  
  void UseCaloTowerBuilder(bool use) {useCaloTowerBuilder = use;}
 private:
  void reset_tree_vars();

  int _verbosity;

  TFile *_f;
  TTree *_tree_hcal;
  std::string _foutname;
  std::string _nodename;
  int _i_event;
  bool useCaloTowerBuilder;
  //  LL1Outv2 *_ll1_mbd_raw;

  LL1Out *_ll1_hcal;

  TriggerPrimitive *_trigger_primitive;
  TriggerPrimitiveContainer *_trigger_primitives;

  TowerInfo *_tower;
  TowerInfoContainer* _towers;
  std::vector<unsigned int> *_trigger_bits;

  std::vector<unsigned int> b_trigger_bits_hcal;
  std::vector<unsigned int> b_trigger_sum_hcalin[24][16];
  std::vector<unsigned int> b_trigger_sum_hcalout[24][16];

  std::vector<float> b_hcalin_energy;
  std::vector<float> b_hcalin_etabin;
  std::vector<float> b_hcalin_phibin;

  std::vector<float> b_hcalout_energy;
  std::vector<float> b_hcalout_etabin;
  std::vector<float> b_hcalout_phibin;

  /* unsigned int b_clock_ll1; */
  /* unsigned int b_event_ll1; */
  /* std::vector<unsigned int> b_trigger_bits_ll1; */
  /* std::vector<unsigned int> b_trigger_words_ll1[8]; */
  /* std::vector<unsigned int> b_trigger_charge_ll1[4][8]; */
  /* std::vector<unsigned int> b_trigger_nhit_ll1[4]; */
  /* std::vector<unsigned int> b_trigger_time_ll1[4][4]; */
  
  
};

#endif 
