#ifndef __TRACKDEADLAYERTAGGING_H__
#define __TRACKDEADLAYERTAGGING_H__

#include <fun4all/SubsysReco.h>
#include <string>
#include <trackbase_historic/TrackSeedContainer.h>
#include <trackbase_historic/SvtxTrackMap.h>
#include <trackbase/ActsGeometry.h>
#include <globalvertex/SvtxVertexMap.h>
#include <trackbase/TrackVertexCrossingAssoc.h>
#include <trackbase/TrkrClusterContainer.h>
#include <g4detectors/PHG4CylinderGeomContainer.h>
#include <trackreco/ActsPropagator.h>

//Forward declerations
class PHCompositeNode;
class TFile;
class TTree;
class TNtuple;

struct MissingSensor
{
  int event;
  int trackID;
  int layer;
  int phi_id;
  int z_id;
  int local_x;
  int local_y;
  bool operator==(const MissingSensor& state)
  {
    return (event==state.event)&&
           (trackID==state.trackID)&&
           (layer==state.layer)&&
           (phi_id==state.phi_id)&&
           (z_id==state.z_id)&&
           (local_x==state.local_x)&&
           (local_y==state.local_y);
  }
};

struct MissingSpacePoint
{
  int layer;
  Surface surface;
  Acts::Vector2 local_intersection;
  Acts::Vector3 global_intersection;
};

// dNdEta pp analysis module
class TrackDeadLayerTagging: public SubsysReco
{
 public: 
  //Default constructor
  TrackDeadLayerTagging(const std::string &name="TrackDeadLayerTagging");

  //Initialization, called for initialization
  int Init(PHCompositeNode *);

  //Process Event, called for each event
  int process_event(PHCompositeNode *);

  //End, write and close files
  int End(PHCompositeNode *);

  //Change output filename
  void set_filename(const char* file)
  { if(file) _outfile = file; }

  void setUseActsPropagator(bool val) { useActsPropagator = val; }
  void setIgnoreSilicon(bool val) { ignoreSilicon = val; }
  void setIgnoreTpc(bool val) { ignoreTpc = val; }

 private:
  //output filename
  std::string _outfile;
   
  //Event counter
  int _event;

  //Branches
  int _trackID;
  int _layer;
  int _phiID;
  int _zID;
  int _localindX;
  int _localindY;
  int _isDead;

  // if false, use helix propagation instead of ACTS propagator
  bool useActsPropagator = true;
  // flags to ignore silicon or TPC missing sensors, if doing seed-specific studies
  bool ignoreSilicon = false;
  bool ignoreTpc = false;

  TrkrClusterContainer* _clustermap = nullptr;
  SvtxTrackMap* _trackmap = nullptr;
  SvtxVertexMap* _vertexmap = nullptr;
  ActsGeometry* _tGeometry = nullptr;
  PHG4CylinderGeomContainer* _inttGeom = nullptr;

  std::array<std::vector<MissingSensor>,57> _dead_sensors; // per layer
  
  TTree* _missing_sensors = nullptr;

  //Get all the nodes
  void GetNodes(PHCompositeNode *);

  void get_dead_sensor_maps();
  void get_surfaces();
  bool is_dead(MissingSensor& sensor);

  std::map<Acts::GeometryIdentifier,TrkrDefs::hitsetkey> _surface_map;
  std::vector<float> getFitParams(TrackSeed* seed);
  std::vector<int> get_missing_layers(SvtxTrack* track);
  std::vector<MissingSpacePoint> get_missing_space_points(SvtxTrack* track, std::vector<int> missing_layers);
  std::vector<MissingSensor> find_sensors(std::vector<MissingSpacePoint> &missing_space_points);

  void find_dead_layers(PHCompositeNode *);
};

#endif //* __TRACKDEADLAYERTAGGING_H__ *//
