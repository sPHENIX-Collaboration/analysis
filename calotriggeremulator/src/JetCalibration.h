#ifndef JETCALIBRATION_H
#define JETCALIBRATION_H

#include <fun4all/SubsysReco.h>
#include <string>
#include <array>
#include "TMath.h"
#include "FindDijets.h"

class Fun4AllHistoManager;
class PHCompositeNode;
class JetContainer;

class PHG4HitContainer;
class JetCalibration : public SubsysReco
{
 public:

  JetCalibration(const std::string &name = "JetCalibration", const std::string &outfilename = "trees_trigger.root");

  virtual ~JetCalibration();

  int Init(PHCompositeNode *topNode) override;

  int InitRun(PHCompositeNode *topNode) override;

  int process_event(PHCompositeNode *topNode) override;

  bool isEM(int pid);
  double getDr(struct jetty jet1, struct jetty jet2);
  void GetNodes (PHCompositeNode *topNode);

  int ResetEvent(PHCompositeNode *topNode) override;

  int EndRun(const int runnumber) override;

  int End(PHCompositeNode *topNode) override;

  int Reset(PHCompositeNode * /*topNode*/) override;

 private:

  int _verbosity;
  std::string _foutname;
  int _i_event;
  Fun4AllHistoManager *hm{nullptr};

  std::map<int, PHG4HitContainer*> m_HitContainerMap;

};

#endif 
