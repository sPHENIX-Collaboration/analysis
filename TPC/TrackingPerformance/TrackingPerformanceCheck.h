#ifndef __TRACKINGPERFORMANCECHECK_H__
#define __TRACKINGPERFORMANCECHECK_H__

#include <map>

#include <fun4all/SubsysReco.h>
#include <string>
#include <TH1F.h>
#include <TH2F.h>
#include <TH1D.h>
#include <TH2D.h>

class PHCompositeNode;

class TrackingPerformanceCheck: public SubsysReco {

public: 

  TrackingPerformanceCheck(const std::string &name="TrackingPerformanceCheck");

  int Init(PHCompositeNode *topNode);
  int process_event(PHCompositeNode *topNode);
  int End(PHCompositeNode *topNode);

private:

  // output histograms ---------------------------------------------------------
  std::map<int,int> fEmbedded;

  TH1F *fHNEvents;

  TH1F *fHTN[4];
  TH1F *fHTPt[4];
  TH1F *fHTEta[4];
  TH1F *fHTPhi[4];
  TH1F *fHTNHits[4];

  TH1F *fHRN[4];
  TH1F *fHRPt[4];
  TH1F *fHREta[4];
  TH1F *fHRPhi[4];
  TH2F *fHRDca2D[4];
  TH2F *fHRNClustersContribution[4];
  TH2F *fHRPtResolution[4];
  TH2F *fHRPhiResolution[4];
  TH2F *fHREtaResolution[4];

  TH1F *fHNVertexes;
};

#endif // __SVTXSIMPERFORMANCECHECKRECO_H__
