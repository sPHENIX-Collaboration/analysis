#ifndef RANDOMCONEANA_H
#define RANDOMCONEANA_H

#include <fun4all/SubsysReco.h>
#include <calotrigger/TriggerAnalyzer.h>
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

#include "TTree.h"
#include "TFile.h"
#include "TVector3.h"

class TVector3;
class TH1D;
class TProfile;
class PHCompositeNode;
class TowerInfoContainer;
class Fun4AllHistoManager;
class TRandom;
class TriggerAnalyzer;
class RandomConeAna : public SubsysReco
{
 public:

  RandomConeAna(const std::string &name = "RandomConeAna", const std::string &outfilename = "histos_random.root");

  virtual ~RandomConeAna();

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
  void SetIsSim(bool use) {isSim = use;}
 private:
  TriggerAnalyzer *triggeranalyzer{nullptr};  
  void reset_tree_vars();

  int _verbosity;

  std::string m_mb_triggerlist[2];
  Fun4AllHistoManager *hm{nullptr};

  std::string _trigger;
  std::string _foutname;
  std::string _nodename;
  std::string m_calo_nodename;
  int _i_event;
  bool isSim{0};
  
  TowerInfo *_tower;
  TowerInfoContainer* _towers;

  TH1D *h_random_et{nullptr};
  TH1D *h_random_emcalre_et{nullptr};
  TH1D *h_random_emcal_et{nullptr};
  TH1D *h_random_hcalout_et{nullptr};
  TH1D *h_random_hcalin_et{nullptr};

  TProfile *hp_random_emcalre_fraction{nullptr};
  TProfile *hp_random_emcal_fraction{nullptr};
  TProfile *hp_random_hcalin_fraction{nullptr};
  TProfile *hp_random_hcalout_fraction{nullptr};

  TRandom *rdm{nullptr};

};

#endif 
