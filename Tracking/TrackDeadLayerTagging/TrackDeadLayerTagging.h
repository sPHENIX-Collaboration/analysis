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
#include <g4detectors/PHG4TpcGeomContainer.h>
#include <trackreco/ActsPropagator.h>
#include <tpc/TpcGlobalPositionWrapper.h>

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
  // check if two instances of missing sensor are on the same physical sensor
  // (i.e. ignore event and trackID, and ignore zbin/tbin for TPC)
  bool isSameSensor(const MissingSensor& othersensor)
  {
    if(layer!=othersensor.layer) return false;
    if(layer<7) // silicon sensor -- needs all spatial coordinates to agree
    {
      return (layer==othersensor.layer)&&
             (phi_id==othersensor.phi_id)&&
             (z_id==othersensor.z_id)&&
             (local_x==othersensor.local_x)&&
             (local_y==othersensor.local_y);
    }
    else if(layer<55) // TPC sensor -- phibin, side and layer are the only meaningful quantities
    {
      return (layer==othersensor.layer)&&
             (z_id==othersensor.z_id)&&
             (local_x==othersensor.local_x);
    }
    else if(layer<57) // TPOT sensor -- tile ID, strip number and layer are the only meaningful quanitities
    {
      return (layer==othersensor.layer)&&
             (phi_id==othersensor.phi_id)&&
             (local_x==othersensor.local_x);
    }
    else return false; // fall through for control flow reasons
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
  int InitRun(PHCompositeNode *);

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
  double _global_x;
  double _global_y;
  double _global_z;

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
  PHG4CylinderGeomContainer* _tpotGeom = nullptr;
  PHG4TpcGeomContainer* _tpcGeom = nullptr;

  std::array<std::vector<MissingSensor>,57> _dead_sensors; // per layer

  TpcGlobalPositionWrapper tpcglobal;
  
  TTree* _missing_sensors = nullptr;

  //Get all the nodes
  void GetNodes(PHCompositeNode *);

  void get_dead_sensor_maps();
  void get_surfaces();
  bool is_dead(MissingSensor& sensor);

  std::vector<std::map<Acts::GeometryIdentifier,TrkrDefs::hitsetkey>> _surface_maps;
  std::vector<float> getFitParams(TrackSeed* seed);
  std::vector<int> get_missing_layers(SvtxTrack* track);
  std::vector<MissingSpacePoint> get_missing_space_points(SvtxTrack* track, std::vector<int> missing_layers);
  std::vector<MissingSensor> find_sensors(std::vector<MissingSpacePoint> &missing_space_points);

  void find_dead_layers(PHCompositeNode *);
};

#endif //* __TRACKDEADLAYERTAGGING_H__ *//
