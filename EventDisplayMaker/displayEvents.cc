#include "displayEvents.h"

#include <fun4all/Fun4AllReturnCodes.h>

#include <trackbase/InttDefs.h>              // for getLadderPhiId, getLad...
#include <trackbase/MvtxDefs.h>              // for getChipId, getStaveId
#include <trackbase/TpcDefs.h>               // for getSectorId, getSide
#include <trackbase/TrkrDefs.h>              // for getLayer, getTrkrId

#include <phool/getClass.h>
#include <phool/PHCompositeNode.h>
#include <phool/recoConsts.h>

#include <TDatabasePDG.h>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

//____________________________________________________________________________..
displayEvents::displayEvents(const std::string &name)
 : SubsysReco(name)
{
}

//____________________________________________________________________________..
displayEvents::~displayEvents()
{
}

//____________________________________________________________________________..
int displayEvents::Init(PHCompositeNode *topNode)
{
  load_nodes(topNode);

  triggeranalyzer = new TriggerAnalyzer();

  recoConsts *rc = recoConsts::instance();
  m_runNumber = rc->get_IntFlag("RUNNUMBER");

  m_run_date = getDate();

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int displayEvents::process_event(PHCompositeNode *topNode)
{
  if (counter >= m_max_displays) return Fun4AllReturnCodes::EVENT_OK; //Made the max number of displays

  load_nodes(topNode);

  for (auto &vtx_iter : *m_vertexMap)
  {
    if (counter >= m_max_displays) break; //Made the max number of displays

    all_tracks.clear();
  
    m_vertex = vtx_iter.second;

    //Get all tracks associated to this vertex
    for (SvtxVertex::TrackIter all_track_iter =  m_vertex->begin_tracks(); all_track_iter != m_vertex->end_tracks(); ++all_track_iter)
    {
      m_track = m_trackMap->get(*all_track_iter);
      all_tracks.push_back(m_track);
    }

    if (loadCalos && (hasEMCal || hasIHCal || hasOHCal) && m_track->get_crossing() != 0) continue;

    triggeranalyzer->decodeTriggers(topNode);

    //Now fill the json file
    json data;

    uint64_t m_bco = gl1packet->lValue(0, "BCO") + all_tracks[0]->get_crossing();

    //Check if we have a list of BCOs we want a plot for, and this is one of them
    if (m_bco_list.size() != 0 && std::find(std::begin(m_bco_list), std::end(m_bco_list), m_bco) == std::end(m_bco_list)) continue;

    std::string date_run_stamp = m_run_date + ", Run " + std::to_string(m_runNumber);
    std::string bco_stamp = "BCO: " + std::to_string(m_bco);

    data[eventName]["evtid"] = 1;
    data[eventName]["runid"] = m_runNumber;
    data[eventName]["type"] = "Collision";
    data[eventName]["s_nn"] = 0;
    data[eventName]["B"] = 1.38;
    data[eventName]["pv"] = {m_vertex->get_x(), m_vertex->get_y(), m_vertex->get_z()};

    if (m_approved)
    {
      data[eventName]["runstats"] = {"sPHENIX Tracking", date_run_stamp, m_decay_tag};
    }
    else
    {
      data[eventName]["runstats"] = {"sPHENIX Internal", date_run_stamp, bco_stamp, m_decay_tag};
    }

    for (auto& hitsMetaSetup : hitsNameVector)
    {      
      data[metadataName][hitsName][hitsMetaSetup]["type"] = "3D";
      data[metadataName][hitsName][hitsMetaSetup]["options"]["size"] = 1;
      data[metadataName][hitsName][hitsMetaSetup]["options"]["color"] = pidToColourMap[hitsMetaSetup];
    }

    data[metadataName][trackName][allTrackName]["width"] = 10;

    data[trackName]["B"] = 0.000014;

    getJSONdata(all_tracks, data);

    if (loadCalos)
    {
      data[metadataName][hitsName][emcalName] = caloMetadata(emcalName);
      data[metadataName][hitsName][ihcalName] = caloMetadata(ihcalName);
      data[metadataName][hitsName][ohcalName] = caloMetadata(ohcalName);

      if (hasEMCal) getJSONcalo(data, emcalName, towersEM, geomEMCal);
      if (hasIHCal) getJSONcalo(data, ihcalName, towersIH, geoIHCal);
      if (hasOHCal) getJSONcalo(data, ohcalName, towersOH, geoOHCal);
    }

    std::string m_output_file = m_evt_display_path + "/EvtDisplay_" + std::to_string(m_runNumber) + "_" + std::to_string(m_bco) + ".json";

    json_output.open(m_output_file);
    json_output << data.dump(2);
    json_output.close();

    ++counter;
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int displayEvents::End(PHCompositeNode *topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

int displayEvents::load_nodes(PHCompositeNode *topNode)
{
  m_vertexMap = findNode::getClass<SvtxVertexMap>(topNode, "SvtxVertexMap");
  if (!m_vertexMap)
  {
    std::cout << __PRETTY_FUNCTION__ << " Fatal Error : missing SvtxVertexMap" << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  m_trackMap = findNode::getClass<SvtxTrackMap>(topNode, "SvtxTrackMap");
  if (!m_trackMap)
  {
    std::cout << __PRETTY_FUNCTION__ << " Fatal Error : missing SvtxTrackMap" << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  gl1packet = findNode::getClass<Gl1Packet>(topNode, "GL1RAWHIT");
  if (!gl1packet)
  {
    gl1packet = findNode::getClass<Gl1Packet>(topNode, "GL1Packet");
    if (!gl1packet)
    {
      std::cout << __PRETTY_FUNCTION__ << " Fatal Error : missing Gl1Packet" << std::endl;
      return Fun4AllReturnCodes::ABORTRUN;
    }
  }

  triggerruninfo = findNode::getClass<TriggerRunInfo>(topNode, "TriggerRunInfo");
  if (!triggerruninfo)
  {
    std::cout << __PRETTY_FUNCTION__ << " Fatal Error : missing TriggerRunInfo" << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  dst_clustermap = findNode::getClass<TrkrClusterContainer>(topNode, "TRKR_CLUSTER");
  if (!dst_clustermap)
  {
    std::cout << __PRETTY_FUNCTION__ << " Fatal Error : missing TRKR_CLUSTER" << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  geometry = findNode::getClass<ActsGeometry>(topNode, "ActsGeometry");
  if (!geometry)
  {
    std::cout << __PRETTY_FUNCTION__ << " Fatal Error : missing ActsGeometry" << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  if (loadCalos)
  {
    if (!towersEM) towersEM = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_CEMC");
    if (!towersEM) towersEM = findNode::getClass<TowerInfoContainer>(topNode, "TOWERS_CEMC");
    if (!geomEMCal) geomEMCal = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_CEMC");

    if (!towersIH) towersIH = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALIN");
    if (!towersIH) towersIH = findNode::getClass<TowerInfoContainer>(topNode, "TOWERS_HCALIN");
    if (!geoIHCal) geoIHCal = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALIN");

    if (!towersOH) towersOH = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALOUT");
    if (!towersOH) towersOH = findNode::getClass<TowerInfoContainer>(topNode, "TOWERS_HCALOUT");
    if (!geoOHCal) geoOHCal = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALOUT");

    if (!towersEM || !geomEMCal) hasEMCal = false;
    if (!towersIH || !geoIHCal) hasIHCal = false;
    if (!towersOH || !geoOHCal) hasOHCal = false;
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

std::string displayEvents::getParticleName(int ID)
{
  return TDatabasePDG::Instance()->GetParticle(ID)->GetName();
}

SvtxTrack *displayEvents::getTrack(unsigned int track_id, SvtxTrackMap *trackmap)
{
  SvtxTrack *matched_track = nullptr;

  for (auto &iter : *trackmap)
  {
    if (iter.first == track_id)
    {
      matched_track = iter.second;
    }
  }

  return matched_track;
}

void displayEvents::getJSONcalo(json &jsonData, std::string calo, TowerInfoContainer *towerCont, RawTowerGeomContainer *geom)
{
  json calos;

  for (size_t channel = 0; channel < towerCont->size(); channel++)
  {
    TowerInfo* tower = towerCont->get_tower_at_channel(channel);
    float raw_energy = tower->get_energy();
    float thresholdCut = caloThreshold[calo];
    if (raw_energy < thresholdCut)
    {
      continue;
    } 

    unsigned int towerkey = towerCont->encode_key(channel);
    int ieta = towerCont->getTowerEtaBin(towerkey);
    int iphi = towerCont->getTowerPhiBin(towerkey);
    calos["e"] = raw_energy;
    calos["phi"] = geom->get_phicenter(iphi);
    calos["eta"] = geom->get_etacenter(ieta);
    calos["event"] = 42;

    jsonData[hitsName][calo] += calos;
  }
}

void displayEvents::getJSONdata(std::vector<SvtxTrack*> tracks, json &jsonData)
{
  json tracksJson, clustersJson;

  int trackCounter = 0; //Needed for getting KFP objects

  for (auto track : tracks)
  {
    float length = 0;

    TrackSeed *silseed = track->get_silicon_seed();
    if (silseed)
    {
      for (auto cluster_iter = silseed->begin_cluster_keys(); cluster_iter != silseed->end_cluster_keys(); ++cluster_iter)
      {
        uint64_t clusterKey = *cluster_iter;
    
        TrkrCluster *cluster = dst_clustermap->findCluster(clusterKey);
        auto global = geometry->getGlobalPosition(clusterKey, cluster);

        clustersJson["x"] = (float) global.x();
        clustersJson["y"] = (float) global.y();
        clustersJson["z"] = (float) global.z();
        clustersJson["e"] = 0;

        if (m_dont_show_track_clus == false) jsonData[hitsName][allTrackName] += clustersJson;
      }
    }

    for (auto state_iter = track->begin_states();
         state_iter != track->end_states();
         ++state_iter)
    {
      SvtxTrackState* tstate = state_iter->second;
      if (tstate->get_pathlength() == 0) continue; //The first track state is an extrapolation so has no cluster

      uint64_t clusterKey = tstate->get_cluskey();

      TrkrCluster *cluster = dst_clustermap->findCluster(clusterKey);
      auto global = geometry->getGlobalPosition(clusterKey, cluster);

      uint8_t id = TrkrDefs::getTrkrId(clusterKey);

      if (id == TrkrDefs::mvtxId || id == TrkrDefs::inttId) continue; //Due to track state issue, use seeds to populate silicon clusters

      clustersJson["x"] = id == TrkrDefs::tpcId ? tstate->get_x() : (float) global.x();
      clustersJson["y"] = id == TrkrDefs::tpcId ? tstate->get_y() : (float) global.y();
      clustersJson["z"] = id == TrkrDefs::tpcId ? tstate->get_z() : (float) global.z();
      clustersJson["e"] = 0;

      if (m_dont_show_track_clus == false) jsonData[hitsName][allTrackName] += clustersJson;

      length = std::max(tstate->get_pathlength(), length);

    } //End of state loop

    std::vector<float> trackPosition;
    std::vector<float> trackMomentum;

    trackPosition = {track->get_x(), track->get_y(), track->get_z()};
    trackMomentum = {track->get_px(), track->get_py(), track->get_pz()};

    tracksJson["pxyz"] = trackMomentum;
    tracksJson["xyz"] = trackPosition;
    tracksJson["trk_color"] = allColour;
    tracksJson["nh"] = 60;
    tracksJson["l"] = length;
    tracksJson["q"] = track->get_charge();

    jsonData[trackName][allTrackName] += tracksJson;    

    ++trackCounter;

  } //End of track loop
}

//https://stackoverflow.com/questions/997946/how-to-get-current-time-and-date-in-c
std::string displayEvents::getDate()
{
    std::time_t t = std::time(0);   // get time now
    std::tm* now = std::localtime(&t);

    std::stringstream date;
    date << (now->tm_year + 1900) << '-'
         << (now->tm_mon + 1) << '-'
         <<  now->tm_mday;
    return date.str();
}

bool displayEvents::isInRange(float min, float value, float max)
{
  return min <= value && value <= max;
}

json displayEvents::caloMetadata(std::string type)
{
  json metaData, options, minmax;

  //Default for EMCal
  float rMin = 90;
  float rMax = 136.1;
  int color = emcalColor;
  float scaleMax = 2.71998;

  if (type != emcalName)
  {
    rMin = type == ihcalName ? 147.27 : 183.3;
    rMax = type == ihcalName ? 175.0 : 348.634;
    color = type == ihcalName ? ihcalColor : ohcalColor;
    scaleMax = type == ihcalName ? 0.569264 : 2.57763;   
  }

  metaData["type"] = "PROJECTIVE";

  options["rmin"] = rMin;
  options["rmax"] = rMax;
  options["deta"] = 0.025;
  options["dphi"] = 0.025;
  options["color"] = color;
  options["transparent"] = 0.6;
  minmax["min"] = 0.05;
  minmax["max"] = scaleMax;
  options["scaleminmax"] = minmax;

  metaData["options"] = options;

  return metaData;
}
