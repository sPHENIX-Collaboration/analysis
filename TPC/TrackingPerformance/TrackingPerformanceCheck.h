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

  TH1F *fHNTruths;
  TH1F *fHNEmbedded;
  TH1F *fHNReconstructables;
  TH1F *fPt[3];
  TH1F *fEta[3];
  TH1F *fPhi[3];
  TH1F *fHEmbeddedNHits[3];

  TH1F *fHNTracks;
  TH1F *fHNTracksMatched;
  TH1F *fHNTracksEmbedded;
  TH1F *fHNTracksReconstructable;
  TH1F *fHRPt[4];
  TH1F *fHREta[4];
  TH1F *fHRPhi[4];
  TH2F *fHRDca2D[4];
  TH1F *fHNClustersContribution[3];
  TH2F *fHPtResolution[3];
  TH2F *fHPhiResolution[3];
  TH2F *fHEtaResolution[3];

  TH1F *fHNVertexes;
};

#endif // __SVTXSIMPERFORMANCECHECKRECO_H__
