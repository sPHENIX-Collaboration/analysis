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
  void SetLayerTPCBegins(int v) {fLayerTPCBegins=v;}
  void SetReconstructableTPCHits(int v) {fReconstructable_TPCHits=v;}
  void SetFairClustersContribution(int v) {fFair_ClustersContribution=v;}
  void SetGoodTrackChi2NDF(float v) {fGTrack_Chi2NDF=v;}
  void SetGoodTrackTPCClusters(int v) {fGTrack_TPCClusters=v;}

private:

  std::map<int,int> fEmbedded;
  int fLayerTPCBegins;
  int fReconstructable_TPCHits;
  int fFair_ClustersContribution;
  float fGTrack_Chi2NDF;
  int fGTrack_TPCClusters;

  TH1F *fHNEvents;

  TH1F *fHTN[5];
  TH1F *fHTPt[5];
  TH1F *fHTEta[5];
  TH1F *fHTPhi[5];
  TH1F *fHTNHits[5];
  TH2F *fHTChi2[5];
  TH2F *fHTDca2D[5];
  TH2F *fHTNClustersContribution[5];
  TH2F *fHTPtResolution[5];
  TH2F *fHTPtResolution2[5];
  TH2F *fHTPhiResolution[5];
  TH2F *fHTEtaResolution[5];

  TH1F *fHRN[4];
  TH1F *fHRPt[4];
  TH1F *fHREta[4];
  TH1F *fHRPhi[4];
  TH2F *fHRChi2[4];
  TH2F *fHRTPCClus[4];
  TH2F *fHRDca2D[4];
  TH2F *fHRNClustersContribution[4];
  TH2F *fHRPtResolution[4];
  TH2F *fHRPtResolution2[4];
  TH2F *fHRPhiResolution[4];
  TH2F *fHREtaResolution[4];

  TH1F *fHNVertexes;
};

#endif // __SVTXSIMPERFORMANCECHECKRECO_H__
