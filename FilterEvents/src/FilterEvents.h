
#ifndef __FILTEREVENTS_H__
#define __FILTEREVENTS_H__

#include <fun4all/SubsysReco.h>

#include <TVector3.h>

class SvtxTrack;
class SvtxTrackMap;
class SvtxVertexMap_v1;
class GlobalVertexMap;
class RawCluster;
class RawClusterContainer;
class TrackSeedContainer;
class TrkrClusterContainerv4;

class FilterEvents: public SubsysReco {

public:

  FilterEvents(const std::string &name = "FilterEvents");
  virtual ~FilterEvents() {}

  int Init(PHCompositeNode *topNode);
  int InitRun(PHCompositeNode *topNode);
  int process_event(PHCompositeNode *topNode);
  int End(PHCompositeNode *topNode);

  void get_dca(SvtxTrack* track, SvtxVertexMap_v1* vertexmap,
                                  double& DCA, double& DCAxy);
  void get_dca_SvtxEval(SvtxTrack* track, SvtxVertexMap_v1* vertexmap,
			    double& dca3dxy, double& dca3dz,
			    double& dca3dxysigma, double& dca3dzsigma);
  void setCuts(double ptCut, double dcaCut, double chi2ndofCut, bool CEMCuse){
    pt_cut = ptCut;
    dca_cut = dcaCut;
    chi2ndof_cut = chi2ndofCut;
    CEMC_use = CEMCuse;
  };

protected:

  int GetNodes(PHCompositeNode *topNode);
  TVector3 GetProjectionCEMC(SvtxTrack* track);
  RawCluster* MatchClusterCEMC(SvtxTrack* track, RawClusterContainer* cemc_clusters, double &dphi, double &deta, double Zvtx);

  PHCompositeNode*     _topNode;
  SvtxTrackMap*        _trackmap;
  SvtxVertexMap_v1*       _vtxmap;
  //GlobalVertexMap*     _global_vtxmap;
  RawClusterContainer* _cemc_clusters;
  TrackSeedContainer* _trackseedcontainer_svtx;
  TrackSeedContainer* _trackseedcontainer_silicon;
  TrackSeedContainer* _trackseedcontainer_tpc;
  TrkrClusterContainerv4* _trkrclusters;

  SvtxTrackMap*        _trackmap_ee;
  RawClusterContainer* _cemc_clusters_ee;

  std::string outnodename_trackmap;
  std::string outnodename_cemc_clusters;

  int EventNumber;
  int goodEventNumber;

  double pt_cut;
  double dca_cut;
  double chi2ndof_cut;

  bool CEMC_use;
};

#endif

