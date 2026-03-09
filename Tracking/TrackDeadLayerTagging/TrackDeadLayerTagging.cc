#include "TrackDeadLayerTagging.h"

#include <phool/phool.h>
#include <phool/getClass.h>
#include <fun4all/PHTFileServer.h>
#include <fun4all/Fun4AllServer.h>

#include <cdbobjects/CDBTTree.h>
#include <ffamodules/CDBInterface.h>

#include <trackreco/ActsPropagator.h>

#include <trackbase/TrkrCluster.h>
#include <trackbase/MvtxDefs.h>
#include <trackbase/InttDefs.h>
#include <trackbase/TpcDefs.h>
#include <trackbase/TrackFitUtils.h>

#include <g4detectors/PHG4TpcGeom.h>

#include <mvtx/SegmentationAlpide.h>
#include <intt/CylinderGeomIntt.h>
#include <intt/InttMapping.h>
#include <micromegas/MicromegasDefs.h>
#include <micromegas/CylinderGeomMicromegas.h>

#include <TNtuple.h>
#include <TH1.h>

#include <iostream>

//____________________________________
TrackDeadLayerTagging::TrackDeadLayerTagging(const std::string &name):
    SubsysReco(name)
{ 
  //initialize
  _event = 0;
  _outfile = "dead_layers.root";
}

//___________________________________
int TrackDeadLayerTagging::InitRun(PHCompositeNode *topNode)
{
  std::cout << PHWHERE << " Opening file " << _outfile << std::endl;
  PHTFileServer::get().open( _outfile, "RECREATE");

  _missing_sensors = new TTree("missing_sensors","Sensors missing from track");
  _missing_sensors->Branch("event",&_event);
  _missing_sensors->Branch("trackID",&_trackID);
  _missing_sensors->Branch("layer",&_layer);
  _missing_sensors->Branch("phiID",&_phiID);
  _missing_sensors->Branch("zID",&_zID);
  _missing_sensors->Branch("localindX",&_localindX);
  _missing_sensors->Branch("localindY",&_localindY);
  _missing_sensors->Branch("isDead",&_isDead);
  _missing_sensors->Branch("global_x",&_global_x);
  _missing_sensors->Branch("global_y",&_global_y);
  _missing_sensors->Branch("global_z",&_global_z);

  std::cout << "loading nodes" << std::endl;
  tpcglobal.set_verbosity(5);
  tpcglobal.loadNodes(topNode);
  std::cout << "nodes loaded" << std::endl;
  //_outtree = new TNtuple("deadlayers_test","deadlayers_test","x:y:z:pt:pz:eta:phi:charge:nhits:nmaps:nintt:quality:crossing:vtx_x:vtx_y:vtx_z:vtx_ntracks:vtxid:ndead_mvtx:ndead_intt:ndead_tpc:ndead_tpot");

  return 0;
}

//__________________________________
//Call user instructions for every event
int TrackDeadLayerTagging::process_event(PHCompositeNode *topNode)
{
  _event++;
  if(_event%1000==0) std::cout << PHWHERE << "Events processed: " << _event << std::endl;
 
  _surface_maps.clear(); 
  GetNodes(topNode);
  for(auto& v : _dead_sensors) v.clear();
  get_surfaces();
  get_dead_sensor_maps();

  if(Verbosity()>0)
  {
    std::cout << "--------------------------------" << std::endl;
    std::cout << "event " << _event << std::endl;
  }

  find_dead_layers(topNode);

  return 0;
}

void TrackDeadLayerTagging::get_dead_sensor_maps()
{
  std::string mvtx_hotpixel = CDBInterface::instance()->getUrl("MVTX_HotPixelMap");
  std::string mvtx_deadpixel = CDBInterface::instance()->getUrl("MVTX_DeadPixelMap");
  std::string intt_hotstrip = CDBInterface::instance()->getUrl("INTT_HotMap");
  std::string tpc_hotchannel = CDBInterface::instance()->getUrl("TPC_HOTCHANNELMAP");
  std::string tpc_deadchannel = CDBInterface::instance()->getUrl("TPC_DEADCHANNELMAP");

  std::unique_ptr<CDBTTree> mvtx_hottree = std::make_unique<CDBTTree>(mvtx_hotpixel);
  std::unique_ptr<CDBTTree> mvtx_deadtree = std::make_unique<CDBTTree>(mvtx_deadpixel);
  std::unique_ptr<CDBTTree> intt_hottree = std::make_unique<CDBTTree>(intt_hotstrip);
  std::unique_ptr<CDBTTree> tpc_hottree;
  std::unique_ptr<CDBTTree> tpc_deadtree;
  if(!tpc_hotchannel.empty())
  {
    tpc_hottree = std::make_unique<CDBTTree>(tpc_hotchannel);
  }
  if(!tpc_deadchannel.empty())
  {
    tpc_deadtree = std::make_unique<CDBTTree>(tpc_deadchannel);
  }

  std::cout << "Verbosity is " << Verbosity() << std::endl;

  mvtx_hottree->LoadCalibrations();
  mvtx_deadtree->LoadCalibrations();
  intt_hottree->LoadCalibrations();
  if(!tpc_hotchannel.empty()) tpc_hottree->LoadCalibrations();
  if(!tpc_deadchannel.empty()) tpc_deadtree->LoadCalibrations();

  int n_mvtxhot = mvtx_hottree->GetSingleIntValue("TotalHotPixels");
  int n_mvtxdead = mvtx_deadtree->GetSingleIntValue("TotalDeadPixels");
  int n_intthot = intt_hottree->GetSingleIntValue("size");
  int n_tpchot = (!tpc_hotchannel.empty())? tpc_hottree->GetSingleIntValue("TotalHotChannels") : 0;
  int n_tpcdead = (!tpc_deadchannel.empty())? tpc_deadtree->GetSingleIntValue("TotalDeadChannels") : 0;

  if(Verbosity()>0)
  {
    std::cout << "MVTX has " << n_mvtxhot << " hot pixels" << std::endl;
    std::cout << "MVTX has " << n_mvtxdead << " dead pixels" << std::endl;
    std::cout << "INTT has " << n_intthot << " hot pixels" << std::endl;
    std::cout << "TPC has " << n_tpchot << " hot pixels" << std::endl;
    std::cout << "TPC has " << n_tpcdead << " dead pixels" << std::endl;
  }

  if(Verbosity()>2) std::cout << "MVTX hot sensors:" << std::endl;
  for(int i=0; i<n_mvtxhot; i++)
  {
    int layer = mvtx_hottree->GetIntValue(i,"layer");
    MissingSensor hotsensor_mvtx = {
      .layer = layer,
      .phi_id = mvtx_hottree->GetIntValue(i,"stave"),
      .z_id = mvtx_hottree->GetIntValue(i,"chip"),
      .local_x = mvtx_hottree->GetIntValue(i,"row"),
      .local_y = mvtx_hottree->GetIntValue(i,"col")
    };
    if(Verbosity()>2)
    {
      std::cout << "layer " << hotsensor_mvtx.layer << " stave " << hotsensor_mvtx.phi_id << " chip " << hotsensor_mvtx.z_id << " row " << hotsensor_mvtx.local_x << " col " << hotsensor_mvtx.local_y << std::endl;
    }
    if(layer>=0&&layer<=2)
    {
      _dead_sensors[layer].push_back(hotsensor_mvtx);
    }
  }

  if(Verbosity()>2) std::cout << "MVTX dead sensors:" << std::endl;
  for(int i=0; i<n_mvtxdead; i++)
  {
    int layer = mvtx_deadtree->GetIntValue(i,"layer");
    MissingSensor deadsensor_mvtx = {
      .layer = layer,
      .phi_id = mvtx_deadtree->GetIntValue(i,"stave"),
      .z_id = mvtx_deadtree->GetIntValue(i,"chip"),
      .local_x = mvtx_deadtree->GetIntValue(i,"row"),
      .local_y = mvtx_deadtree->GetIntValue(i,"col")
    };
    if(layer>=0&&layer<=2)
    {
      _dead_sensors[layer].push_back(deadsensor_mvtx);
    }
    if(Verbosity()>2)
    {
      std::cout << "layer " << deadsensor_mvtx.layer << " stave " << deadsensor_mvtx.phi_id << " chip " << deadsensor_mvtx.z_id << " row " << deadsensor_mvtx.local_x << " col " << deadsensor_mvtx.local_y << std::endl;
    }
  }

  if(Verbosity()>2) std::cout << "INTT hot sensors:" << std::endl;
  for(int i=0; i<n_intthot; i++)
  {
    InttNameSpace::RawData_s raw_sensor;
    raw_sensor.felix_server = intt_hottree->GetIntValue(i,"felix_server");
    raw_sensor.felix_channel = intt_hottree->GetIntValue(i,"felix_channel");
    raw_sensor.chip = intt_hottree->GetIntValue(i,"chip");
    raw_sensor.channel = intt_hottree->GetIntValue(i,"channel");
    InttNameSpace::Offline_s offline_sensor = InttNameSpace::ToOffline(raw_sensor);
    MissingSensor hotsensor_intt = {
      .layer = offline_sensor.layer,
      .phi_id = offline_sensor.ladder_phi,
      .z_id = offline_sensor.ladder_z,
      .local_x = offline_sensor.strip_y,
      .local_y = offline_sensor.strip_x
    };
    if(offline_sensor.layer>=3&&offline_sensor.layer<=6)
    {
      _dead_sensors[offline_sensor.layer].push_back(hotsensor_intt);
    }
    if(Verbosity()>2)
    {
      std::cout << "layer " << offline_sensor.layer << " ladder phi " << offline_sensor.ladder_phi << " ladder z " << offline_sensor.ladder_z << " strip y " << offline_sensor.strip_y << " strip x " << offline_sensor.strip_x << std::endl;
    }
  }

  if(Verbosity()>2) std::cout << "TPC hot sensors:" << std::endl;
  for(int i=0; i<n_tpchot; i++)
  {
    int layer = tpc_hottree->GetIntValue(i,"layer");
    MissingSensor hotsensor_tpc = {
      .layer = layer,
      .phi_id = tpc_hottree->GetIntValue(i,"sector"),
      .z_id = tpc_hottree->GetIntValue(i,"side"),
      .local_x = tpc_hottree->GetIntValue(i,"pad")
    };
    if(layer>=7&&layer<=54)
    {
      _dead_sensors[layer].push_back(hotsensor_tpc);
    }
    if(Verbosity()>2)
    {
      std::cout << "layer " << layer << " sector " << hotsensor_tpc.phi_id << " side " << hotsensor_tpc.z_id << " phibin " << hotsensor_tpc.local_x << std::endl;
    }
  }

  if(Verbosity()>2) std::cout << "TPC dead sensors:" << std::endl;
  for(int i=0; i<n_tpcdead; i++)
  {
    int layer = tpc_deadtree->GetIntValue(i,"layer");
    MissingSensor deadsensor_tpc = {
      .layer = layer,
      .phi_id = tpc_deadtree->GetIntValue(i,"sector"),
      .z_id = tpc_deadtree->GetIntValue(i,"side"),
      .local_x = tpc_deadtree->GetIntValue(i,"pad")
    };
    if(layer>=7&&layer<=54)
    {
      _dead_sensors[layer].push_back(deadsensor_tpc);
    }
    if(Verbosity()>2)
    {
      std::cout << "layer " << layer << " sector " << deadsensor_tpc.phi_id << " side " << deadsensor_tpc.z_id << " phibin " << deadsensor_tpc.local_x << std::endl;
    }
  }
  // TODO: figure out how TPOT works for this
}

void TrackDeadLayerTagging::get_surfaces()
{
  for(int layer=0; layer<57; layer++)
  {
    _surface_maps.emplace_back();
    if(layer<3) // mvtx
    {
      const int n_staves = 12 + 4*layer;
      const int n_chips = 9;
      for(int stave_id=0;stave_id<n_staves;stave_id++)
      {
        for(int chip_id=0;chip_id<n_chips;chip_id++)
        {
          // we only need spatial information from surface, and ActsSurfaceMaps::getSiliconSurface sets time info to 0 anyway
          TrkrDefs::hitsetkey hskey = MvtxDefs::genHitSetKey(layer,stave_id,chip_id,0);
          //std::cout << "layer " << layer << ", stave " << stave_id << ", chip " << chip_id << ", hskey " << std::to_string(hskey) << std::endl;
          
          const auto& surface = _tGeometry->maps().getSiliconSurface(hskey);
          if(!surface)
          {
            std::cout << PHWHERE << " cannot find MVTX surface with layer " << layer << " stave " << stave_id << " chip " << chip_id << std::endl;
          }
          else
          {
            //std::cout << "geoId: " << surface->geometryId() << std::endl;
            _surface_maps[layer].insert(std::make_pair(surface->geometryId(),hskey));
          }
        }
      }
    }
    else if(layer<7) // intt
    {
      int n_ladders;
      if(layer<5) n_ladders = 12;
      else n_ladders = 16;
      const int n_zsegments = 4;
      for(int ladder_phi_id=0;ladder_phi_id<n_ladders;ladder_phi_id++)
      {
        for(int ladder_z_id=0;ladder_z_id<n_zsegments;ladder_z_id++)
        {
          // we only need spatial information from surface, and ActsSurfaceMaps::getSiliconSurface sets time info to 0 anyway
          TrkrDefs::hitsetkey hskey = InttDefs::genHitSetKey(layer,ladder_z_id,ladder_phi_id,0);
          //std::cout << "hskey " << std::to_string(hskey) << std::endl;
          //std::cout << "gen hitsetkey should have layer " << layer << ", z id " << ladder_z_id << ", phi id " << ladder_phi_id << std::endl;
          //std::cout << "actually has layer " << int(TrkrDefs::getLayer(hskey)) << ", z id " << int(InttDefs::getLadderZId(hskey)) << ", phi id " << int(InttDefs::getLadderPhiId(hskey)) << std::endl;

          const auto& surface = _tGeometry->maps().getSiliconSurface(hskey);
          if(!surface)
          {
            std::cout << PHWHERE << " cannot find INTT surface with layer " << layer << " ladder_z " << ladder_z_id << " ladder_phi " << ladder_phi_id << std::endl;
          }
          else
          {
            //std::cout << "geoId: " << surface->geometryId() << std::endl;
            _surface_maps[layer].insert(std::make_pair(surface->geometryId(),hskey));
          }
        }
      }
    }
    else if(layer<55) // tpc
    {
      // TPC organizes itself into "fake" subsurfaces, 120 per layer, unrelated with sector positions(?)
      // This means that this surface map doesn't actually do anything useful...
      // Keeping it anyway for now, in case there turns out to be a way to grab sector and side from subsurface key
      const int n_subsurf = 288;
      // We need a dummy hitsetkey that tells the surface map which layer we're on
      TrkrDefs::hitsetkey hskey = TpcDefs::genHitSetKey(layer,0,0);
      for(int subsurf_id = 0; subsurf_id < n_subsurf; subsurf_id++)
      {
        // this only uses the layer from the hitsetkey, so this is fine
        const auto& surface = _tGeometry->maps().getTpcSurface(hskey,subsurf_id);
        if(!surface)
        {
          std::cout << PHWHERE << " cannot find TPC surface with layer " << layer << " subsurface " << subsurf_id << std::endl;
        }
        else
        {
          //std::cout << "geoId: " << surface->geometryId() << std::endl;
          // we store the subsurface ID instead of the hitsetkey here because most of the information of the hitsetkey is unknown (and the layer is redundant with the index)
          _surface_maps[layer].insert(std::make_pair(surface->geometryId(),subsurf_id));
        }
      }
    }
    else if(layer<57) // tpot
    {
      const int n_tiles = 8;
      for(int tile_id = 0; tile_id < n_tiles; tile_id++)
      {
        TrkrDefs::hitsetkey hskey_phi = MicromegasDefs::genHitSetKey(layer,MicromegasDefs::SegmentationType::SEGMENTATION_PHI,tile_id);
        TrkrDefs::hitsetkey hskey_z = MicromegasDefs::genHitSetKey(layer,MicromegasDefs::SegmentationType::SEGMENTATION_Z,tile_id);

        const auto& surface_phi = _tGeometry->maps().getMMSurface(hskey_phi);
        const auto& surface_z = _tGeometry->maps().getMMSurface(hskey_z);

        if(!surface_phi)
        {
          std::cout << PHWHERE << " cannot find TPOT surface with layer " << layer << " phi segmentation, tile " << tile_id << std::endl;
        }
        else
        {
          //std::cout << "geoId: " << surface_phi->geometryId() << std::endl;
          _surface_maps[layer].insert(std::make_pair(surface_phi->geometryId(),hskey_phi));
        }
        if(!surface_z)
        {
          std::cout << PHWHERE << " cannot find TPOT surface with layer " << layer << " z segmentation, tile " << tile_id << std::endl;
        }
        else
        {
          //std::cout << "geoId: " << surface_z->geometryId() << std::endl;
          _surface_maps[layer].insert(std::make_pair(surface_z->geometryId(),hskey_z));
        }
      }
    }
  }
}

std::vector<float> TrackDeadLayerTagging::getFitParams(TrackSeed* seed)
{
  std::vector<float> fitpars;
  fitpars.push_back(1./fabs(seed->get_qOverR()));
  fitpars.push_back(seed->get_X0());
  fitpars.push_back(seed->get_Y0());
  fitpars.push_back(seed->get_slope());
  fitpars.push_back(seed->get_Z0());
  return fitpars;
}

std::vector<MissingSpacePoint> TrackDeadLayerTagging::get_missing_space_points(SvtxTrack* track, std::vector<int> missing_layers)
{
  std::vector<MissingSpacePoint> missing_space_points;
  if(useActsPropagator)
  {
    ActsPropagator propagator(_tGeometry);
    propagator.verbosity(0);
    ActsPropagator::BoundTrackParamResult result_trackparams = propagator.makeTrackParams(track,_vertexmap);
    if(result_trackparams.ok())
    {
      Acts::BoundTrackParameters trackparams = result_trackparams.value();
      for(int layer : missing_layers)
      {
        ActsPropagator::BTPPairResult result_propagated_params = propagator.propagateTrack(trackparams,layer);
        if(result_propagated_params.ok())
        {
          // [path length (mm), BoundTrackParams]
          Acts::BoundTrackParameters updated_params = result_propagated_params.value().second;
          //Surface surf = updated_params.referenceSurface().getSharedPtr();

          Acts::Vector3 global_intersection = updated_params.position(_tGeometry->geometry().getGeoContext()) / Acts::UnitConstants::cm;
          Acts::Vector3 tangent = updated_params.direction();

          int nCompatibleSurfaces = 0;
          for(auto& [geoId, hskey] : _surface_maps[layer])
          {
            Surface surface = _tGeometry->geometry().tGeometry->findSurface(geoId)->getSharedPtr();
            auto local_result = surface->globalToLocal(_tGeometry->geometry().getGeoContext(),global_intersection * Acts::UnitConstants::cm,tangent);
            if(local_result.ok())
            {
              Acts::Vector2 local_intersection = local_result.value() / Acts::UnitConstants::cm;
              // trackparams = updated_params;
              if(surface->insideBounds(local_intersection * Acts::UnitConstants::cm))
              {
                MissingSpacePoint missingpoint = { layer, surface, local_intersection, global_intersection };
                missing_space_points.push_back(missingpoint);
                nCompatibleSurfaces++;
              }
            }
          }
          if(nCompatibleSurfaces>1 && Verbosity()>0) std::cout << PHWHERE << "NOTE: More than one compatible surface found!" << std::endl;
        }
        else
        {
          if(Verbosity()>0) std::cout << PHWHERE << "ActsPropagator returned error " << result_propagated_params.error().message() << " on propagating to layer " << layer << std::endl;
        }
      }
    }
    else
    {
      if(Verbosity()>0) std::cout << PHWHERE << "ActsPropagator returned error " << result_trackparams.error().message() << " on formation of track params" << std::endl;
    }
  }
  else
  {
    TrackSeed* siliconseed = track->get_silicon_seed();
    TrackSeed* tpcseed = track->get_tpc_seed();
    
    // get fit parameters for track; use TPC seed for this unless silicon-only track
    std::vector<float> fitpars = tpcseed? getFitParams(tpcseed) : getFitParams(siliconseed);

    // get nearest cluster phi for each layer (necessary to disambiguate multiple compatible surfaces)
    std::vector<std::pair<float,float>> filledlayer_pairs;
    if(siliconseed)
    {
      for(auto si_ckey = siliconseed->begin_cluster_keys(); si_ckey != siliconseed->end_cluster_keys(); si_ckey++)
      {
        TrkrCluster* c = _clustermap->findCluster(*si_ckey);
        Acts::Vector3 globalpos = _tGeometry->getGlobalPosition(*si_ckey,c);
        float phi = atan2(globalpos.y(),globalpos.x());
        filledlayer_pairs.push_back(std::make_pair(TrkrDefs::getLayer(*si_ckey),phi));
      }
    }
    if(tpcseed)
    {
      for(auto tpc_ckey = tpcseed->begin_cluster_keys(); tpc_ckey != tpcseed->end_cluster_keys(); tpc_ckey++)
      {
        TrkrCluster* c = _clustermap->findCluster(*tpc_ckey);
        Acts::Vector3 globalpos = tpcglobal.getGlobalPositionDistortionCorrected(*tpc_ckey,c,track->get_crossing());
        float phi = atan2(globalpos.y(),globalpos.x());
        filledlayer_pairs.push_back(std::make_pair(TrkrDefs::getLayer(*tpc_ckey),phi));
      }
    }

    // for each missing layer, find helix intersection with compatible surface in that layer
    for(int missing_layer : missing_layers)
    {
      if(Verbosity()>1)
      {
        std::cout << "layer " << missing_layer << ":" << std::endl;
      }
      std::vector<MissingSpacePoint> compatible_surfaces;
      
      for(auto& [geoId, hskey] : _surface_maps[missing_layer])
      {
        Surface surface = _tGeometry->geometry().tGeometry->findSurface(geoId)->getSharedPtr();
        Acts::Vector3 surface_center = surface->center(_tGeometry->geometry().getGeoContext()) / Acts::UnitConstants::cm;
        Acts::Vector3 intersection = TrackFitUtils::get_helix_surface_intersection(surface,fitpars,surface_center,_tGeometry);
        Acts::Vector3 tangent = TrackFitUtils::get_helix_tangent(fitpars,intersection).second;
        if(Verbosity()>2)
        {
          std::cout << "surface center: (" << surface_center.x() << ", " << surface_center.y() << ", " << surface_center.z() << ")" << std::endl;
          std::cout << "intersection with surface: (" << intersection.x() << ", " << intersection.y() << ", " << intersection.z() << ")" << std::endl;
          std::cout << "tangent vector: (" << tangent.x() << ", " << tangent.y() << ", " << tangent.z() << ")" << std::endl;
        }
        auto intersection_local_result = surface->globalToLocal(_tGeometry->geometry().getGeoContext(),intersection * Acts::UnitConstants::cm,tangent * Acts::UnitConstants::cm);
        if(!intersection_local_result.ok())
        {
          if(Verbosity()>2)
          {
            std::cout << "local coordinate conversion failed..." << std::endl;
          }
          continue;
        }
        Acts::Vector2 intersection_local = intersection_local_result.value() / Acts::UnitConstants::cm;
        if(Verbosity()>2)
        {
          std::cout << "local intersection: (" << intersection_local.x() << ", " << intersection_local.y() << ")" << std::endl;
        }
        if(surface->insideBounds(intersection_local * Acts::UnitConstants::cm))
        {
          if(Verbosity()>1)
          {
            std::cout << "missing track state within bounds" << std::endl;
          }
          MissingSpacePoint missingpoint = { missing_layer, surface, intersection_local, intersection };
          compatible_surfaces.push_back(missingpoint);
        }
      }

      // disambiguate multiple compatible surfaces on helix
      if(compatible_surfaces.size()>1)
      {
        // find cluster phi of closest on-track layer to space point
        float closest_phi = 1e9;
        // cover edge cases
        if(missing_layer < filledlayer_pairs[0].first) closest_phi = filledlayer_pairs[0].second;
        else if(missing_layer > filledlayer_pairs.back().first) closest_phi = filledlayer_pairs.back().second;
        for(size_t i=0; i<filledlayer_pairs.size(); i++)
        {
          if(filledlayer_pairs[i].first < missing_layer) continue;
          closest_phi = filledlayer_pairs[i].second;
          int abovelayer_difference = (filledlayer_pairs[i].first - missing_layer);
          int belowlayer_difference = filledlayer_pairs[i-1].first - missing_layer;
          if(abovelayer_difference < belowlayer_difference) closest_phi = filledlayer_pairs[i].second;
          else closest_phi = filledlayer_pairs[i-1].second;
          break;
        }
        // determine which space point has most similar phi to closest on-track cluster phi
        int mostcompatible_index = -1;
        float best_dphi = 1e9;
        for(size_t i=0; i < compatible_surfaces.size(); i++)
        {
          Acts::Vector3 globalpos = compatible_surfaces[i].global_intersection;
          float phi = atan2(globalpos.y(),globalpos.x());
          if(fabs(phi-closest_phi)<best_dphi)
          {
            mostcompatible_index = i;
            best_dphi = fabs(phi-closest_phi);
          }
        }
        missing_space_points.push_back(compatible_surfaces[mostcompatible_index]);
      }
      else if(compatible_surfaces.size()==1)
      {
        missing_space_points.push_back(compatible_surfaces[0]);
      }
    }
  }
  return missing_space_points;
}

std::vector<int> TrackDeadLayerTagging::get_missing_layers(SvtxTrack* track)
{
  std::vector<TrkrDefs::cluskey> ckeys;
  if(track->get_silicon_seed()) std::copy(track->get_silicon_seed()->begin_cluster_keys(),track->get_silicon_seed()->end_cluster_keys(),std::back_inserter(ckeys));
  if(track->get_tpc_seed()) std::copy(track->get_tpc_seed()->begin_cluster_keys(),track->get_tpc_seed()->end_cluster_keys(),std::back_inserter(ckeys));

  int start_layer = 0;
  int end_layer = 56;
  if(ignoreSilicon) start_layer = 7;
  if(ignoreTpc) end_layer = 6;

  std::vector<int> missing_layers;
  for(int l=start_layer; l<=end_layer; l++)
  {
    if(std::find_if(ckeys.begin(),ckeys.end(),[&](TrkrDefs::cluskey ckey){ return (TrkrDefs::getLayer(ckey) == l); }) == ckeys.end())
    {
      missing_layers.push_back(l);
    }
  }

  if(missing_layers.size()>0)
  {
    if(Verbosity()>2)
    {
      std::cout << "missing layers found: ";
      for(auto& l : missing_layers) std::cout << l << ", ";
      std::cout << std::endl;
    }
  }
  return missing_layers;
}

std::vector<MissingSensor> TrackDeadLayerTagging::find_sensors(std::vector<MissingSpacePoint> &spacepoints)
{
  std::vector<MissingSensor> missing_sensors;
  for(auto& spacepoint : spacepoints)
  {
    if(Verbosity()>1) std::cout << "sensor layer: " << spacepoint.layer << std::endl;
    if(spacepoint.layer<3) // MVTX
    {
      int row;
      int col;
      SegmentationAlpide::localToDetector(spacepoint.local_intersection.x(),spacepoint.local_intersection.y(),row,col);
      TrkrDefs::hitsetkey hskey = _surface_maps[spacepoint.layer][spacepoint.surface->geometryId()];
      int phi_id = MvtxDefs::getStaveId(hskey);
      int z_id = MvtxDefs::getChipId(hskey); 
      if(Verbosity()>1)
      {
        std::cout << "stave: " << phi_id << ", chip: " << z_id << std::endl;
        std::cout << "pixel: " << row << ", " << col << std::endl;
      }
      if(row >= 0 && row <= 511 && col>=0 && col<=1023)
      {
        MissingSensor missing_sensor = { _event, _trackID, spacepoint.layer, phi_id, z_id, row, col };
        missing_sensors.push_back(missing_sensor);
      }
    }
    else if(spacepoint.layer<7) // INTT
    {
      int strip_y;
      int strip_z;
      CylinderGeomIntt* intt_cylinder = dynamic_cast<CylinderGeomIntt*>(_inttGeom->GetLayerGeom(spacepoint.layer));
      TrkrDefs::hitsetkey hskey = _surface_maps[spacepoint.layer][spacepoint.surface->geometryId()];
      int phi_id = InttDefs::getLadderPhiId(hskey);
      int z_id = InttDefs::getLadderZId(hskey);
      int layer = TrkrDefs::getLayer(hskey);
      intt_cylinder->find_strip_index_values(z_id,spacepoint.local_intersection.x(),spacepoint.local_intersection.y(),strip_y,strip_z);
      if(Verbosity()>1)
      {
        std::cout << "layer: " << layer << std::endl;
        std::cout << "ladder: " << phi_id << ", " << z_id << std::endl;
        std::cout << "strip: " << strip_y << ", " << strip_z << std::endl;
      }
      if(strip_y >= 0 && strip_y <= 255 && strip_z>=0 && ((z_id == 0 && strip_z<=7) || (z_id == 1 && strip_z<=4)))
      {
        MissingSensor missing_sensor = { _event, _trackID, spacepoint.layer, phi_id, z_id, strip_y, strip_z };
        missing_sensors.push_back(missing_sensor);
      }
    }
    else if(spacepoint.layer<55) // TPC
    {
      int side = (spacepoint.global_intersection.z()>0)? 1 : 0;
      double phi = atan2(spacepoint.global_intersection.y(),spacepoint.global_intersection.x());
      PHG4TpcGeom* layergeom = _tpcGeom->GetLayerCellGeom(spacepoint.layer);
      int phibin = layergeom->get_phibin(phi,side);
      int zbin = layergeom->get_zbin(spacepoint.global_intersection.z());

      // turns out we don't need the _surface_maps for TPC because the sector and side information isn't carried there anyway
      const int sector_step = layergeom->get_phibins() / 12;
      int sector_id = phibin / sector_step;

      if(Verbosity()>1)
      {
        std::cout << "layer: " << spacepoint.layer << std::endl;
        std::cout << "sector: " << sector_id << ", side " << side << std::endl;
        std::cout << "phibin: " << phibin << ", zbin: " << zbin << std::endl;
      }
      MissingSensor missing_sensor = { _event, _trackID, spacepoint.layer, sector_id, side, phibin, zbin };
      missing_sensors.push_back(missing_sensor);
    }
    else if(spacepoint.layer<57) // TPOT
    {
      int strip_z = 0; // no strip z index for TPOT, that info is in tile ID
      CylinderGeomMicromegas* tpotlayergeom = dynamic_cast<CylinderGeomMicromegas*>(_tpotGeom->GetLayerGeom(spacepoint.layer));
      
      TrkrDefs::hitsetkey hskey = _surface_maps[spacepoint.layer][spacepoint.surface->geometryId()];
      int tile_id = MicromegasDefs::getTileId(hskey);
      int seg_id = (MicromegasDefs::getSegmentationType(hskey) == MicromegasDefs::SegmentationType::SEGMENTATION_Z)? 0 : 1;
      TVector2 local_tvector(spacepoint.local_intersection.x(),spacepoint.local_intersection.y());

      int strip_id = tpotlayergeom->find_strip_from_local_coords(tile_id,_tGeometry,local_tvector);

      if(Verbosity()>1)
      {
        std::cout << "layer: " << spacepoint.layer << std::endl;
        std::cout << "tile ID: " << tile_id << ", segmentation " << seg_id << std::endl;
        std::cout << "strip_id " << strip_id << std::endl;
      }
      MissingSensor missing_sensor = { _event, _trackID, spacepoint.layer, tile_id, seg_id, strip_id, strip_z };
      missing_sensors.push_back(missing_sensor);
    }
  }
  return missing_sensors;
}

bool TrackDeadLayerTagging::is_dead(MissingSensor& sensor)
{
  std::vector<MissingSensor> dead_thislayer = _dead_sensors.at(sensor.layer);
  return (std::find_if(dead_thislayer.begin(),dead_thislayer.end(),
            [&sensor](const MissingSensor& candidate) { return sensor.isSameSensor(candidate); })
          != dead_thislayer.end());
}

//_____________________________________
void TrackDeadLayerTagging::find_dead_layers(PHCompositeNode *topNode)
{

  for(const auto &[key, track] : *_trackmap)
  {
    if(!track) continue;

    _trackID = track->get_id();
    if(Verbosity()>0) std::cout << "track ID " << _trackID << std::endl;
    if(std::isnan(track->get_x()) ||
       std::isnan(track->get_y()) ||
       std::isnan(track->get_z()) ||
       std::isnan(track->get_px()) ||
       std::isnan(track->get_py()) ||
       std::isnan(track->get_pz()))
    {
      std::cout << "malformed track:" << std::endl;
      track->identify();
      std::cout << "skipping..." << std::endl;
      continue;
    }

    std::vector<int> missing_layers = get_missing_layers(track);
    std::vector<MissingSpacePoint> missing_space_points = get_missing_space_points(track,missing_layers);
    std::vector<MissingSensor> missing_sensors = find_sensors(missing_space_points);
 
    if(Verbosity()>0 && missing_sensors.size()>0)
    {
      std::cout << "missing sensors: " << std::endl;
      for(auto& sensor : missing_sensors)
      {
        if(!(ignoreSilicon && sensor.layer<7) && !(ignoreTpc && sensor.layer>=7))
        {
          std::cout << "layer: " << sensor.layer << " phi_id: " << sensor.phi_id << " z_id: " << sensor.z_id << " local_x: " << sensor.local_x << " local_y: " << sensor.local_y << std::endl;
        }
      }
    }

    for(size_t i=0; i<missing_sensors.size(); i++)
    {
      MissingSensor& sensor = missing_sensors[i];
      MissingSpacePoint& point = missing_space_points[i];
      if(!(ignoreSilicon && sensor.layer<7) && !(ignoreTpc && sensor.layer>=7))
      {
        _layer = sensor.layer;
        _phiID = sensor.phi_id;
        _zID = sensor.z_id;
        _localindX = sensor.local_x;
        _localindY = sensor.local_y;
        _isDead = is_dead(sensor)? 1 : 0;
        _global_x = point.global_intersection.x();
        _global_y = point.global_intersection.y();
        _global_z = point.global_intersection.z();
        _missing_sensors->Fill();
      }
    }
  }
}

//___________________________________
void TrackDeadLayerTagging::GetNodes(PHCompositeNode *topNode)
{

  _clustermap = findNode::getClass<TrkrClusterContainer>(topNode,"TRKR_CLUSTER");
  if(!_clustermap && _event<2)
  {
    std::cout << PHWHERE << " cannot find TrkrClusterContainer" << std::endl;
  }

  _trackmap = findNode::getClass<SvtxTrackMap>(topNode,"SvtxTrackMap");
  if(!_trackmap && _event<2)
  {
    std::cout << PHWHERE << " cannot find SvtxTrackMap" << std::endl;
  }

  _vertexmap = findNode::getClass<SvtxVertexMap>(topNode,"SvtxVertexMap");
  if(!_vertexmap && _event<2)
  {
    std::cout << PHWHERE << " cannot find SvtxVertexMap" << std::endl;
  }

  _tGeometry = findNode::getClass<ActsGeometry>(topNode, "ActsGeometry");
  if(!_tGeometry && _event<2)
  {
    std::cout << PHWHERE << " cannot find ActsGeometry" << std::endl;
  }

  _inttGeom = findNode::getClass<PHG4CylinderGeomContainer>(topNode, "CYLINDERGEOM_INTT");
  if(!_inttGeom && _event<2)
  {
    std::cout << PHWHERE << " cannot find CYLINDERGEOM_INTT" << std::endl;
  }

  _tpcGeom = findNode::getClass<PHG4TpcGeomContainer>(topNode,"TPCGEOMCONTAINER");
  if(!_tpcGeom && _event<2)
  {
    std::cout << PHWHERE << " cannot find CYLINDERCELLGEOM_SVTX" << std::endl;
  }

  _tpotGeom = findNode::getClass<PHG4CylinderGeomContainer>(topNode, "CYLINDERGEOM_MICROMEGAS_FULL");
  if(!_tpotGeom && _event<2)
  {
    std::cout << PHWHERE << " cannot find CYLINDERGEOM_MICROMEGAS_FULL" << std::endl;
  }

}

//______________________________________
int TrackDeadLayerTagging::End(PHCompositeNode *topNode)
{
 PHTFileServer::get().cd( _outfile );
 _missing_sensors->Write();
 return 0;
}

