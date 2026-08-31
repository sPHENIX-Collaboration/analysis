#ifndef PARTICLEFLOWRECO_H
#define PARTICLEFLOWRECO_H

//===========================================================
/// \file ParticleFlowReco.h
/// \brief Particle flow event reconstruction
/// \author Dennis V. Perepelitsa
//===========================================================

#include <fun4all/SubsysReco.h>

#include <CLHEP/Vector/ThreeVector.h>

#include <gsl/gsl_rng.h>

#include <string>
#include <vector>

class PHCompositeNode;
class SvtxTrack;
class SvtxTrackMap;
class RawCluster;
class RawClusterContainer;
class GlobalVertexMap;
class SvtxVertexMap;
class ParticleFlowElementContainer;
class RawTowerGeomContainer;

class ParticleFlowReco : public SubsysReco
{
 public:
  ParticleFlowReco(const std::string &name = "ParticleFlowReco");

  ~ParticleFlowReco() override = default;

  int InitRun(PHCompositeNode *topNode) override;

  int process_event(PHCompositeNode *topNode) override;

  void set_energy_match_Nsigma(float Nsigma)
  {
    _energy_match_Nsigma = Nsigma;
  }
  void set_track_map_name(std::string &name) { _track_map_name = name; }

  void set_only_crossing_zero(bool b) { _only_crossing_zero = b; }

  void dont_use_global_vertex(bool b) { _dont_use_global_vertex = b; }

  void set_track_min_pt(float ptmin) { _min_track_pt = ptmin; }

  void set_track_eta_acc(float eta_acc) { _acc_track_eta = eta_acc; }

  void use_overlap_match(bool b) { _use_overlap_match = b; }

  void set_trk_emc_overlap_phi(float f) { _trk_emc_overlap_phi = f; }
  void set_trk_emc_overlap_eta(float f) { _trk_emc_overlap_eta = f; }
  void set_trk_had_overlap_phi(float f) { _trk_had_overlap_phi = f; }
  void set_trk_had_overlap_eta(float f) { _trk_had_overlap_eta = f; }
  void set_emc_had_overlap_phi(float f) { _emc_had_overlap_phi = f; }
  void set_emc_had_overlap_eta(float f) { _emc_had_overlap_eta = f; }

  void set_isEMCalDetailedGeo(bool b) { _isEMCalDetailedGeo = b; }

 private:
  void ResetRepresentation();
  void PrepareTracks();
  void PrepareEMCalClusters();
  void PrepareHCalClusters();
  void LinkTrackToCaloClusters();
  void LinkEMCalToHCalClusters();
  void LinkTrackToHCalViaEMCal();
  void ProcessMatchedHCalClusters();
  void ProcessMatchedEMCalClusters();
  void ProcessClusterOnlyMatch();
  void ProcessUnmatchedEMCalClusters();
  void ProcessUnmatchedHCalClusters();
  void ProcessUnmatchedTracks();
  CLHEP::Hep3Vector GetVertexForCalorimeter();

  std::pair<int, int> GetCaloLimits(CLHEP::Hep3Vector &cluster, int trackID);
  CLHEP::Hep3Vector Minimize_PhiEta(CLHEP::Hep3Vector &cluster, int trackID, CLHEP::Hep3Vector &vertex, bool ok);

  bool isAcceptableTrack(SvtxTrack *track);

  int CreateNode(PHCompositeNode *topNode);

  float calculate_dR(float, float, float, float);
  std::pair<float, float> get_expected_signature(int);

  bool _only_crossing_zero = true;
  bool _dont_use_global_vertex = false;
  float _energy_match_Nsigma;
  int _global_pflow_index = 0;
  bool _use_overlap_match = true;
  float _trk_emc_overlap_phi = 2.5;
  float _trk_emc_overlap_eta = 2.5;
  float _trk_had_overlap_phi = 1.5;
  float _trk_had_overlap_eta = 1.5;
  float _emc_had_overlap_phi = 1.5;
  float _emc_had_overlap_eta = 1.5;

  RawTowerGeomContainer *_geomEM = nullptr;
  RawTowerGeomContainer *_geomIH = nullptr;
  RawTowerGeomContainer *_geomOH = nullptr;

  SvtxTrackMap *_trackmap = nullptr;
  RawClusterContainer *_clustersEM = nullptr;
  RawClusterContainer *_clustersHAD = nullptr;

  ParticleFlowElementContainer *_pflowContainer = nullptr;

  GlobalVertexMap *_global_vertexmap = nullptr;
  SvtxVertexMap *_svtx_vertexmap = nullptr;

  std::vector<float> _pflow_TRK_p;
  std::vector<int> _pflow_TRK_vertex;
  std::vector<float> _pflow_TRK_eta;
  std::vector<float> _pflow_TRK_phi;
  std::vector<std::vector<CLHEP::Hep3Vector>> _pflow_TRK_projections;
  /*
  std::vector<float> _pflow_TRK_EMproj_x;
  std::vector<float> _pflow_TRK_EMproj_y;
  std::vector<float> _pflow_TRK_EMproj_z;
  std::vector<float> _pflow_TRK_HADproj_x;
  std::vector<float> _pflow_TRK_HADproj_y;
  std::vector<float> _pflow_TRK_HADproj_z;
  */
  std::vector<SvtxTrack *> _pflow_TRK_trk;
  std::vector<std::vector<int> > _pflow_TRK_match_EM;
  std::vector<std::vector<int> > _pflow_TRK_match_HAD;

  // convention is ( EM index, dR value )
  std::vector<std::vector<std::pair<int, float> > > _pflow_TRK_addtl_match_EM;

  std::vector<float> _pflow_EM_E;
  std::vector<float> _pflow_EM_x;
  std::vector<float> _pflow_EM_y;
  std::vector<float> _pflow_EM_z;
  std::vector<RawCluster *> _pflow_EM_cluster;
  std::vector<std::vector<float> > _pflow_EM_tower_x;
  std::vector<std::vector<float> > _pflow_EM_tower_y;
  std::vector<std::vector<float> > _pflow_EM_tower_z;
  std::vector<std::vector<int> > _pflow_EM_match_HAD;
  std::vector<std::vector<int> > _pflow_EM_match_TRK;

  std::vector<float> _pflow_HAD_E;
  std::vector<float> _pflow_HAD_x;
  std::vector<float> _pflow_HAD_y;
  std::vector<float> _pflow_HAD_z;
  std::vector<RawCluster *> _pflow_HAD_cluster;
  std::vector<std::vector<float> > _pflow_HAD_tower_x;
  std::vector<std::vector<float> > _pflow_HAD_tower_y;
  std::vector<std::vector<float> > _pflow_HAD_tower_z;
  std::vector<std::vector<int> > _pflow_HAD_match_EM;
  std::vector<std::vector<int> > _pflow_HAD_match_TRK;

  std::string _track_map_name = "SvtxTrackMap";
  float _min_track_pt = 0.2;
  float _acc_track_eta = 1.1;
  bool _isEMCalDetailedGeo = false;

};

#endif  // PARTICLEFLOWRECO_H
