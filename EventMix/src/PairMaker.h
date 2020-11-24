
#ifndef __PAIRMAKER_H__
#define __PAIRMAKER_H__

#include <fun4all/SubsysReco.h>

class TFile;
class TH1D;

class PairMaker: public SubsysReco {

public:

  PairMaker(const std::string &name = "PairMaker", const std::string &filename = "test.root");
  virtual ~PairMaker() {}

  int Init(PHCompositeNode *topNode);
  int InitRun(PHCompositeNode *topNode);
  int process_event(PHCompositeNode *topNode);
  int End(PHCompositeNode *topNode);

protected:

  int process_event_test(PHCompositeNode *topNode);

  TFile* OutputNtupleFile;
  std::string OutputFileName;

  TH1D* htest;

  int EventNumber;

};

#endif

