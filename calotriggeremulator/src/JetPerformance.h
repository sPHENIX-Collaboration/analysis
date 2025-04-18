
#ifndef JETPERFORMANCE_H
#define JETPERFORMANCE_H

#include <fun4all/SubsysReco.h>
#include <calotrigger/TriggerAnalyzer.h>
#include <string>
#include <array>



class Fun4AllHistoManager;
class PHCompositeNode;
class TowerInfoContainer;
class JetContainer;
class PHG4TruthInfoContainer;

class JetPerformance : public SubsysReco
{
 public:

  JetPerformance(const std::string &name = "JetPerformance", const std::string &outfilename = "trees_jetperformance.root");

  virtual ~JetPerformance();

  int Init(PHCompositeNode *topNode) override;

  int InitRun(PHCompositeNode *topNode) override;

  int process_event(PHCompositeNode *topNode) override;
  
  void GetNodes (PHCompositeNode *topNode);

  int EndRun(const int runnumber) override;

  int End(PHCompositeNode *topNode) override;

  int Reset(PHCompositeNode * /*topNode*/) override;

  void SetPtCut(float pt) {pt_cut = pt;}

 private:
  TriggerAnalyzer *triggeranalyzer;  

  int _verbosity;

  float pt_cut = 4;
  std::string _foutname;
  std::string _nodename;
  std::string m_calo_nodename;
  int _i_event;
  Fun4AllHistoManager *hm{nullptr};
  std::array<std::string, 8> m_jet_triggernames;;

};

#endif 
