
#ifndef __FILTEREVENTSUPSILON_H__
#define __FILTEREVENTSUPSILON_H__

#include <fun4all/SubsysReco.h>

class SvtxTrack;
class SvtxTrackMap;
class SvtxVertexMap;
class GlobalVertexMap;
class RawCluster;
class RawClusterContainer;

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
  RawCluster* MatchClusterCEMC(SvtxTrack* track, RawClusterContainer* cemc_clusters, double &dphi, double &deta);

  PHCompositeNode*     _topNode;
  SvtxTrackMap*        _trackmap;
  SvtxVertexMap*       _vtxmap;
  GlobalVertexMap*     _global_vtxmap;
  RawClusterContainer* _cemc_clusters;

  SvtxTrackMap*        _trackmap_ee;

  std::string outnodename;
  int EventNumber;

};

#endif

