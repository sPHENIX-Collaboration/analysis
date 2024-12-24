
#ifndef SMEARTRUTHJETS_H
#define SMEARTRUTHJETS_H

#include <fun4all/SubsysReco.h>
#include <calotrigger/TriggerAnalyzer.h>
#include <DijetEventDisplay.h>
#include <string>
#include <array>
#include "TMath.h"


class PHCompositeNode;
class JetContainer;
class TFile;
class TH1D;
class TF1;
class SmearTruthJets : public SubsysReco
{
 public:

  SmearTruthJets(const std::string &name = "SmearTruthJets");

  virtual ~SmearTruthJets();

  int Init(PHCompositeNode *topNode) override;

  int InitRun(PHCompositeNode *topNode) override;

  int process_event(PHCompositeNode *topNode) override;

  int End(PHCompositeNode *topNode) override;

  int Reset(PHCompositeNode * /*topNode*/) override;

 private:

  int _verbosity;
  TH1D *h_jes{nullptr};
  TH1D *h_jer{nullptr};
  TF1 *smearfunction{nullptr};
};

#endif 
