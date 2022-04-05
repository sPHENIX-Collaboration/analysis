// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef TRACKCLUSTEREVALUATOR_H
#define TRACKCLUSTEREVALUATOR_H

#include <fun4all/SubsysReco.h>
#include <trackbase/TrkrDefs.h>
#include <Acts/Definitions/Algebra.hpp>

#include <string>
#include <vector>
#include <map>
#include <set>

class PHCompositeNode;
class TrkrClusterContainer;
class SvtxTrackMap;
class PHG4TruthInfoContainer;
class SvtxEvalStack;
class SvtxTrack;
class TTree;
class TFile;

class TrackClusterEvaluator : public SubsysReco
{
 public:
  TrackClusterEvaluator(const std::string &name = "TrackClusterEvaluator");

  virtual ~TrackClusterEvaluator();

  int Init(PHCompositeNode *topNode) override;
  int InitRun(PHCompositeNode *topNode) override;
  int process_event(PHCompositeNode *topNode) override;
  int ResetEvent(PHCompositeNode *topNode) override;
  int EndRun(const int runnumber) override;
  int End(PHCompositeNode *topNode) override;
  int Reset(PHCompositeNode * /*topNode*/) override;

  void setProcess(const int proc) { m_proc = proc; }
  void setnEvent(const int nevent) { m_nevent = nevent; }
  void trackMapName(std::string name) { m_trackMapName = name; }
  void outfileName(std::string name) { m_outfilename = name; }
  void scanForPrimaries(bool scan) { m_scanForPrimaries = scan; }
  void scanForEmbedded(bool scan) { m_scanForEmbedded = scan; }
  void trackMatch(bool match) { m_trackMatch = match; }
  void Print(const std::string &what = "ALL") const override;

 private:
  int getNodes(PHCompositeNode *topNode);
  void processTruthTracks(PHCompositeNode *topNode);
  void processRecoTracks(PHCompositeNode *topNode);
  void setupTrees();
  void clearVectors();
  void resetTreeValues();

  PHG4TruthInfoContainer *m_truthContainer = nullptr;
  TrkrClusterContainer *m_clusterContainer = nullptr;
  SvtxTrackMap *m_trackMap = nullptr;
  SvtxEvalStack *m_svtxevalstack = nullptr;
  std::string m_trackMapName = "SvtxTrackMap";

  bool m_scanForEmbedded = true;
  bool m_scanForPrimaries = true;
  bool m_trackMatch = true;

  int m_proc = 0;
  int m_nevent = 1;
  TFile *m_outfile = nullptr;
  std::string m_outfilename = "TrackClusterEvaluator.root";
  TTree *m_recotree = nullptr;
  TTree *m_truthtree = nullptr;
  TTree *m_duplicatetree = nullptr;

  int event = -9999;
  int gflavor = -9999;
  int gntracks = -9999;
  int gtrackID = -9999;
  int gnmaps = -9999;
  int gnintt = -9999;
  int gntpc = -9999;
  int gnmms = -9999;
  float gpx = -9999;
  float gpy = -9999;
  float gpz = -9999;
  float gpt = -9999;
  float geta = -9999;
  float gphi = -9999;
  float gvx = -9999;
  float gvy = -9999;
  float gvz = -9999;
  float gvt = -9999;
  int gembed = -9999;
  int gprimary = -9999;
  int isDuplicate = -9999;
  std::vector<TrkrDefs::cluskey> gclusterkeys;
  std::vector<float> tgclusterx, tgclustery, tgclusterz, tclusterx, tclustery, tclusterz;
  std::vector<float> gclusterx, gclustery, gclusterz, gclusterrphierr, gclusterzerr;
  int trackID = -9999;
  float px = -9999;
  float py = -9999;
  float pz = -9999;
  float pt = -9999;
  float eta = -9999;
  float phi = -9999;
  int charge = -9999;
  float quality = -9999;
  int nmaps = -9999;
  int nintt = -9999;
  int ntpc = -9999;
  int nmms = -9999;
  float dca3dxy = -9999;
  float dca3dz = -9999;
  float pcax = -9999;
  float pcay = -9999;
  float pcaz = -9999;
  int matchedTrackID = -9999;
  
  int dtrackID = -9999;
  float dpx = -9999;
  float dpy = -9999;
  float dpz = -9999;
  float dpt = -9999;
  float deta = -9999;
  float dphi = -9999;
  int dcharge = -9999;
  float dquality = -9999;
  int dnmaps = -9999;
  int dnintt = -9999;
  int dntpc = -9999;
  int dnmms = -9999;
  float ddca3dxy = -9999;
  float ddca3dz = -9999;
  float dpcax = -9999;
  float dpcay = -9999;
  float dpcaz = -9999;
  std::vector<TrkrDefs::cluskey> dclusterkeys;
  std::vector<float> dclusterx, dclustery, dclusterz;
  std::vector<float> dclusterrphierr, dclusterzerr;

  std::vector<TrkrDefs::cluskey> clusterkeys;
  std::vector<float> clusterx, clustery, clusterz, clusterrphierr, clusterzerr;
  std::vector<unsigned int> matchedRecoTracksID;

 
};

#endif  // TRACKCLUSTEREVALUATOR_H
