
#ifndef __FILTEREVENTSUPSILON_H__
#define __FILTEREVENTSUPSILON_H__

#include <fun4all/SubsysReco.h>

#include <TVector3.h>

class SvtxTrack;
class SvtxTrackMap;
class SvtxVertexMap;
class GlobalVertexMap;
class RawCluster;
class RawClusterContainer;
class TrackSeedContainer;
class TrkrClusterContainer;

class FilterEventsUpsilon: public SubsysReco {

public:

  FilterEventsUpsilon(const std::string &name = "FilterEventsUpsilon");
  virtual ~FilterEventsUpsilon() {}

  int Init(PHCompositeNode *topNode);
  int InitRun(PHCompositeNode *topNode);
  int process_event(PHCompositeNode *topNode);
  int End(PHCompositeNode *topNode);

protected:

  int GetNodes(PHCompositeNode *topNode);
  TVector3 GetProjectionCEMC(SvtxTrack* track);
  RawCluster* MatchClusterCEMC(SvtxTrack* track, RawClusterContainer* cemc_clusters, double &dphi, double &deta, double Zvtx);

  PHCompositeNode*     _topNode;
  SvtxTrackMap*        _trackmap;
  SvtxVertexMap*       _vtxmap;
  //GlobalVertexMap*     _global_vtxmap;
  RawClusterContainer* _cemc_clusters;
  TrackSeedContainer* _trackseedcontainer_svtx;
  TrackSeedContainer* _trackseedcontainer_silicon;
  TrackSeedContainer* _trackseedcontainer_tpc;
  TrkrClusterContainer* _trkrclusters;

  SvtxTrackMap*        _trackmap_ee;
  RawClusterContainer* _cemc_clusters_ee;

  std::string outnodename_trackmap;
  std::string outnodename_cemc_clusters;

  int EventNumber;
  int goodEventNumber;

};

#endif

