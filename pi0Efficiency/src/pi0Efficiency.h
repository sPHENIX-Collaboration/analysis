// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef PI0EFFICIENCY_H
#define PI0EFFICIENCY_H

#include <fun4all/SubsysReco.h>

#include <string>

class PHCompositeNode;
//TopNode
class PHCompositeNode;
//histos and stuff
class TH1F;
class TH3F;
class TH2F;
class TH2I;
class TString;
//emcal stuff
class RawClusterContainer;
class RawCluster;
class RawTowerGeomContainer;
class RawTowerContainer; 
class RawTower;
//tracking for iso cone
class SvtxTrackMap;
//vertex info
class GlobalVertexMap;
class GlobalVertex;
//Fun4all
class Fun4AllHistoManager;
class TFile;
class PHG4TruthInfoContainer;
class PHG4Particle;
class PHG4VtxPoint;

const int nEtaBins = 5;

class pi0Efficiency : public SubsysReco
{
 public:

  pi0Efficiency(const std::string &name, const std::string &outName);

  ~pi0Efficiency() override;

 
  int Init(PHCompositeNode *topNode) override;

  int InitRun(PHCompositeNode *topNode) override;

  int process_event(PHCompositeNode *topNode) override;

  /// Clean up internals after each event.
  int ResetEvent(PHCompositeNode *topNode) override;

  /// Called at the end of each run.
  int EndRun(const int runnumber) override;

  /// Called at the end of all processing.
  int End(PHCompositeNode *topNode) override;

  /// Reset
  int Reset(PHCompositeNode * /*topNode*/) override;

  void Print(const std::string &what = "ALL") const override;

 private:
  //functors
  int getEtaBin(float eta);
  float getEta(PHG4Particle *particle);
  //this thing
  std::string OutFile;
  //histos
  TH3F *ePi0InvMassEcut[nEtaBins];
  TH1F *clusterE;
  TH1F *photonE;
  TH1F *prim_pi0_E[nEtaBins];
  TH2F *pi0EScale;
  TH2F *unmatchedLocale;
  TH1F *unmatchedE;
  TH2F *truthPi0EDeltaR;
  TH2F *truthPi0EAsym;
  TH1F *hMassRat;
  
  Fun4AllHistoManager *hm = nullptr;
  TFile *out;
};

#endif // PI0EFFICIENCY_H
