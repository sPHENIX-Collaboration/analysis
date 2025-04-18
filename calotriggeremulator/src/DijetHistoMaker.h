
#ifndef DIJETHISTOMAKER_H
#define DIJETHISTOMAKER_H

#include <fun4all/SubsysReco.h>
#include <calotrigger/TriggerAnalyzer.h>
#include <string>
#include <array>
#include "TMath.h"

class TF1;

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
  float iso = 0;
  float emcal = 0;
  float ihcal = 0;
  float ohcal = 0;
  float edep = 0;
  float edep_em = 0;
  float edep_hd = 0;
};

class DijetHistoMaker : public SubsysReco
{
 public:

  DijetHistoMaker(const std::string &name = "DijetHistoMaker", const std::string &outfilename = "histos.root");

  virtual ~DijetHistoMaker();

  int Init(PHCompositeNode *topNode) override;

  int InitRun(PHCompositeNode *topNode) override;

  int process_event(PHCompositeNode *topNode) override;
  
  void process_truth_jets(JetContainer *jets);

  void GetNodes (PHCompositeNode *topNode);

  int ResetEvent(PHCompositeNode *topNode) override;

  int EndRun(const int runnumber) override;

  int End(PHCompositeNode *topNode) override;

  int Reset(PHCompositeNode * /*topNode*/) override;

  void SetIsSim(bool use) {isSim = use;}
  void SetPtCut(float pt) {pt_cut = pt;}
  double get_Dr(struct jetty jet1, struct jetty jet2);
 private:


  TriggerAnalyzer *triggeranalyzer{nullptr};;  

  int _verbosity;

  float pt_cut = 4;
  float dphi_cut[3] = {TMath::Pi()/2., 3.*TMath::Pi()/4., 7.*TMath::Pi()/8.};
  float eta_cut[3] = {0.7, 0.9, 1.1};
  float pt_cut1[3] = {10., 20., 30.};
  float pt_cut2[3] = {5., 10., 15.};
  std::string _foutname;
  std::string _nodename;
  std::string m_calo_nodename;
  int _i_event;
  bool isSim{0};
  Fun4AllHistoManager *hm{nullptr};
  std::array<std::string, 8> m_jet_triggernames;;

  TF1 *fsmear{nullptr};
  double isocut = 1.0;
};

#endif 
