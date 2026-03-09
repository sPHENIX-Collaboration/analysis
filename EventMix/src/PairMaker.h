
#ifndef __PAIRMAKER_H__
#define __PAIRMAKER_H__

#include <fun4all/SubsysReco.h>

#include "sPHElectronv1.h"

#include <vector>

class TFile;
class TH1D;
class TRandom;
class SvtxTrack;
class sPHElectronPairContainerv1;


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
  int MakeMixedPairs(std::vector<sPHElectronv1> elepos, sPHElectronPairContainerv1* eePairs, unsigned int centbin);

  bool isElectron(SvtxTrack*);

  std::string outnodename;
  static const int NZ = 2;
  static const int NCENT = 2;
  std::vector<sPHElectronv1> _buffer[NZ][NCENT];
  unsigned int _min_buffer_depth;
  unsigned int _max_buffer_depth;
  double _ZMAX;
  double _ZMIN;
  std::vector<double> _multbins;

  TRandom* _rng;

  int EventNumber;

};

#endif

