
#ifndef FINDDIJETS_H
#define FINDDIJETS_H

#include <fun4all/SubsysReco.h>
#include <calotrigger/TriggerAnalyzer.h>
#include <string>
#include <array>



class Fun4AllHistoManager;
class PHCompositeNode;
class TowerInfoContainer;
class JetContainer;
class PHG4TruthInfoContainer;
struct jetty
{
  float pt = 0;
  float et = 0;
  float eta = 0;
  float phi = 0;
  float emcal = 0;
  float ihcal = 0;
  float ohcal = 0;

};
class FindDijets : public SubsysReco
{
 public:

  FindDijets(const std::string &name = "FindDijets", const std::string &outfilename = "trees_finddijets.root");

  virtual ~FindDijets();

  int Init(PHCompositeNode *topNode) override;

  int InitRun(PHCompositeNode *topNode) override;

  int process_event(PHCompositeNode *topNode) override;
  
  void GetNodes (PHCompositeNode *topNode);

  int ResetEvent(PHCompositeNode *topNode) override;

  int EndRun(const int runnumber) override;

  int End(PHCompositeNode *topNode) override;

  int Reset(PHCompositeNode * /*topNode*/) override;

  void SetIsSim(bool use) {isSim = use;}
  void SetPtCut(float pt) {pt_cut = pt;}

 private:
  TriggerAnalyzer *triggeranalyzer;  

  int _verbosity;

  float pt_cut = 4;
  float pt_cut1[3] = {10., 20., 40.};
  float pt_cut2[3] = {4., 8., 15.};
  std::string _foutname;
  std::string _nodename;
  std::string m_calo_nodename;
  int _i_event;
  bool isSim{0};
  Fun4AllHistoManager *hm{nullptr};
  std::array<std::string, 8> m_jet_triggernames;;
};

#endif 
