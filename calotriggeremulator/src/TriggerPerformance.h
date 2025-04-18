#ifndef TRIGGERPERFORMANCE_H
#define TRIGGERPERFORMANCE_H

#include <fun4all/SubsysReco.h>
#include <calotrigger/TriggerAnalyzer.h>
#include <string>
#include <array>
#include "TMath.h"

class Fun4AllHistoManager;
class PHCompositeNode;
class TowerInfoContainer;
class JetContainer;

class TriggerPerformance : public SubsysReco
{
 public:

  TriggerPerformance(const std::string &name = "TriggerPerformance", const std::string &outfilename = "trees_trigger.root");

  virtual ~TriggerPerformance();

  int Init(PHCompositeNode *topNode) override;

  int InitRun(PHCompositeNode *topNode) override;

  int process_event(PHCompositeNode *topNode) override;

  void GetNodes (PHCompositeNode *topNode);

  int ResetEvent(PHCompositeNode *topNode) override;

  int EndRun(const int runnumber) override;

  int End(PHCompositeNode *topNode) override;

  int Reset(PHCompositeNode * /*topNode*/) override;

  void UseEmulator(bool use) { m_useEmulator = use; }
 private:
  bool m_useEmulator{false};
  TriggerAnalyzer *triggeranalyzer{nullptr};;  
  int _verbosity;
  std::string _foutname;
  std::string _nodename;
  std::string m_calo_nodename;
  int _i_event;
  Fun4AllHistoManager *hm{nullptr};
  std::array<std::string, 4> m_jet_triggernames;
  std::array<int, 4> m_jet_prescales;
  std::array<std::string, 4> m_photon_triggernames;
  std::array<int, 4> m_photon_prescales;
  std::array<std::string, 4> m_photon_emu_triggernames;
  std::array<std::string, 4> m_jet_emu_triggernames;
  int m_mbd_prescale;


};

#endif 
