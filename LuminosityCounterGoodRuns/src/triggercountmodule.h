#ifndef TRIGGERCOUNTMODULE_H
#define TRIGGERCOUNTMODULE_H
#include <fun4all/SubsysReco.h>
#include <string>
#include <globalvertex/GlobalVertex.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
class PHCompositeNode;
class CentralityInfo;
class triggercountmodule : public SubsysReco
{
 public:

  triggercountmodule(const std::string &filename, int rn, int segn, int maxseg, int debug = 0, const std::string &name = "triggercountmodule");

  virtual ~triggercountmodule();

  int Init(PHCompositeNode *topNode) override;

  int InitRun(PHCompositeNode *topNode) override;

  int process_event(PHCompositeNode *topNode) override;

  int ResetEvent(PHCompositeNode *topNode) override;

  int EndRun(const int runnumber) override;

  int End(PHCompositeNode *topNode) override;

  int Reset(PHCompositeNode * /*topNode*/) override;

  void Print(const std::string &what = "ALL") const override;


 private:
  std::string _name;
  std::string _filename;
  TFile* _outfile;
  TTree* _tree;
  int _debug;
  TH1D* _mbzhist;
  TH1D* _bunchHist;
  long long unsigned int _startBCO = 0;
  long long unsigned int _endBCO = 0;
  int _nBunch = 0;
  int _rn;
  int _lastseg;
  int _nseg;
  long long unsigned int _evtn = 0;
  long long unsigned int _startLive[64] = {0};
  long long unsigned int _startScal[64] = {0};
  long long unsigned int _endLive[64] = {0};
  long long unsigned int _endScal[64] = {0};
  long long unsigned int _startRaw[64] = {0};
  long long unsigned int _endRaw[64] = {0};
  double _avgPS[64] = {0};
  long long unsigned int _trigCounts[6][64] = {0};
  double _eMBDlive[5] = {0};
  int _badFlag = 0;
  GlobalVertex::VTXTYPE _vtxtype = GlobalVertex::MBD;
};

#endif
