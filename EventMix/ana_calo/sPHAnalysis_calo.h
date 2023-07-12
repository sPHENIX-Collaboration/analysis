#ifndef __SPHANALYSIS_H__
#define __SPHANALYSIS_H__

#include <fun4all/SubsysReco.h>

class TFile;
class TNtuple;
class TH1F;
class TH1D;
class TRandom;
class RawCluster;
class RawClusterContainer;
class RawTowerContainer;
class RawTowerGeomContainer;

class sPHAnalysis_calo: public SubsysReco {


public:

  sPHAnalysis_calo(const std::string &name = "sPHAnalysis_calo", const std::string &filename = "test.root");
  virtual ~sPHAnalysis_calo() {}

  int Init(PHCompositeNode *topNode);
  int InitRun(PHCompositeNode *topNode);
  int process_event(PHCompositeNode *topNode);
  int End(PHCompositeNode *topNode);

  void set_whattodo(int what) { _whattodo = what; }

protected:

  int process_event_test(PHCompositeNode *topNode);
  int process_event_data(PHCompositeNode *topNode);

  TFile* OutputNtupleFile;
  std::string OutputFileName;

  TNtuple* ntp_notracking;
  TH1D* h_mult;
  TH1D* h_ecore;

  TH1F* h_pedestal[256*96];

  int EventNumber;
  int _whattodo;

  TRandom* _rng;

};

#endif


