
#ifndef __PAIRMAKER_H__
#define __PAIRMAKER_H__

#include <fun4all/SubsysReco.h>

#include <vector>

class TFile;
class TH1D;

class SvtxTrack;

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

  bool isElectron(SvtxTrack*);

//  TFile* OutputNtupleFile;
//  std::string OutputFileName;
//  TH1D* htest;

  std::string outnodename;


//  const int NZ = 5;
//  const int NCENT = 1;
//  vector<sPHElectronv1> buffer1[NZ][NCENT];
//  vector<sPHElectronv1> buffer2[NZ][NCENT];

  int EventNumber;

};

#endif

