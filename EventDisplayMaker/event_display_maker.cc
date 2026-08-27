#include "event_display_maker.h"

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

float vertex[3];

//____________________________________________________________________________..
event_display_maker::event_display_maker(const std::string &name)
 : SubsysReco(name)
 , m_kfparticleContainerName("reconstructedParticles")
{
}

//____________________________________________________________________________..
event_display_maker::~event_display_maker()
{
}

//____________________________________________________________________________..
int event_display_maker::Init(PHCompositeNode *topNode)
{
  load_nodes(topNode);

  triggeranalyzer = new TriggerAnalyzer();

  recoConsts *rc = recoConsts::instance();
  m_runNumber = rc->get_IntFlag("RUNNUMBER");

  m_run_date = getDate();

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int event_display_maker::process_event(PHCompositeNode *topNode)
{
  if (counter >= m_max_displays) return Fun4AllReturnCodes::EVENT_OK; //Made the max number of displays

  load_nodes(topNode);
  std::string trackableParticles[] = {"e-", "mu-", "pi+", "K+", "proton"};

  for (auto &vtx_iter : *m_vertexMap)
  {
    if (counter >= m_max_displays) break; //Made the max number of displays

    kfp_daughters.clear();
    intermediates.clear();
    trigger_tracks.clear();
    all_tracks.clear();
  
    m_vertex = vtx_iter.second;

    for (KFParticle_Container::Iter kfp_iter = m_kfp_container->begin(); kfp_iter != m_kfp_container->end(); ++kfp_iter)
    {
      KFParticle *myParticle = kfp_iter->second;
      std::string thisParticle = getParticleName(abs(myParticle->GetPDG())); //Only care about trackable particles

      if (thisParticle == m_mother_name)
      {
        mother = myParticle;
        vertex[0] = mother->GetX();
        vertex[1] = mother->GetY();
        vertex[2] = mother->GetZ();

        if (!isInRange(m_min_mass, mother->GetMass(), m_max_mass))
        {
          return Fun4AllReturnCodes::EVENT_OK;
        }
      }

      if (std::find(std::begin(m_intermerdiate_names), std::end(m_intermerdiate_names), thisParticle) != std::end(m_intermerdiate_names))
      {
        intermediates.push_back(myParticle);
      }

      if (std::find(std::begin(trackableParticles), std::end(trackableParticles), thisParticle) != std::end(trackableParticles))
      {
        m_track = getTrack(myParticle->Id(), m_trackMap);

        //If we want calo information then we can only use bunch crossing 0 candidates
        //if (loadCalos && (hasEMCal || hasIHCal || hasOHCal) && m_track->get_crossing() != 0) continue;

        if (m_track->get_crossing() != m_vertex->get_beam_crossing()) continue; //Track isnt from same crossing (need continue to account for mother)

        kfp_daughters.push_back(myParticle);
        trigger_tracks.push_back(m_track);
      }
    }

    if (kfp_daughters.size() < m_number_of_daughters) continue; //This could be risky
    //Get all tracks associated to this vertex
    for (SvtxVertex::TrackIter all_track_iter =  m_vertex->begin_tracks(); all_track_iter != m_vertex->end_tracks(); ++all_track_iter)
    {
      m_track = m_trackMap->get(*all_track_iter);
      all_tracks.push_back(m_track);
    }

    triggeranalyzer->decodeTriggers(topNode);

    //Now fill the json file
    json data;

    uint64_t m_bco = gl1packet->lValue(0, "BCO") + trigger_tracks[0]->get_crossing();

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

    data[metadataName][trackName][triggerTrackName]["width"] = 0.3;

    data[trackName]["B"] = 0.000014;

    getJSONdata(trigger_tracks, kfp_daughters, data, triggerTrackName);
    getJSONdata(all_tracks, kfp_daughters, data, allTrackName);

    if (loadCalos)
    {
      data[metadataName][hitsName][emcalName] = caloMetadata(emcalName);
      data[metadataName][hitsName][ihcalName] = caloMetadata(ihcalName);
      data[metadataName][hitsName][ohcalName] = caloMetadata(ohcalName);

      if (hasEMCal) getJSONcalo(data, emcalName, towersEM, geomEMCal);
      if (hasIHCal) getJSONcalo(data, ihcalName, towersIH, geoIHCal);
      if (hasOHCal) getJSONcalo(data, ohcalName, towersOH, geoOHCal);
    }

    std::string m_output_file = m_evt_display_path + "/EvtDisplay_" + m_mother_name + "_" + std::to_string(m_runNumber) + "_" + std::to_string(m_bco) + ".json";

    json_output.open(m_output_file);
    json_output << data.dump(2);
    json_output.close();

    ++counter;
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int event_display_maker::End(PHCompositeNode *topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

int event_display_maker::load_nodes(PHCompositeNode *topNode)
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

  m_kfparticleContainerName_container = m_kfparticleContainerName + "_KFParticle_Container";

  m_kfp_container = findNode::getClass<KFParticle_Container>(topNode, m_kfparticleContainerName_container);
  if (!m_kfp_container)
  {
    std::cout << __PRETTY_FUNCTION__ << " Fatal Error : missing " << m_kfparticleContainerName_container << std::endl;
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

std::string event_display_maker::getParticleName(int ID)
{
  return TDatabasePDG::Instance()->GetParticle(ID)->GetName();
}

SvtxTrack *event_display_maker::getTrack(unsigned int track_id, SvtxTrackMap *trackmap)
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

void event_display_maker::getJSONcalo(json &jsonData, std::string calo, TowerInfoContainer *towerCont, RawTowerGeomContainer *geom)
{
  json calos;

  for (size_t channel = 0; channel < towerCont->size(); channel++)
  {
    TowerInfo* tower = towerCont->get_tower_at_channel(channel);
    float raw_energy = tower->get_energy();
    if (raw_energy < m_e_low_cut)
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

void event_display_maker::getJSONdata(std::vector<SvtxTrack*> tracks, std::vector<KFParticle*> particles, json &jsonData, std::string jsonEntryName)
{
  json tracksJson, clustersJson;

  int trackCounter = 0; //Needed for getting KFP objects

  for (auto track : tracks)
  {
    //Skip drawing of trigger tracks in all tracks
    if (jsonEntryName == allTrackName)
    {
      bool continueOn = false;
      for (unsigned int i = 0; i < particles.size(); ++i)
      {
        if (track->get_id() == (unsigned) particles[i]->Id())
        {
          continueOn = true;
          break;
        }
      }
      if (continueOn) continue;
    }

    float length = 0;

    std::string hitType = jsonEntryName == allTrackName ? allTrackName : pidToHitMap[particles[trackCounter]->GetPDG()]; //Figure out if this is background or a triggered track

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

        jsonData[hitsName][hitType] += clustersJson;
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

      jsonData[hitsName][hitType] += clustersJson;

      length = std::max(tstate->get_pathlength(), length);

    } //End of state loop

    std::vector<float> trackPosition;
    std::vector<float> trackMomentum;

    if (jsonEntryName == triggerTrackName)
    {
      trackPosition = {particles[trackCounter]->GetX(), particles[trackCounter]->GetY(), particles[trackCounter]->GetZ()};
      trackMomentum = {particles[trackCounter]->GetPx(), particles[trackCounter]->GetPy(), particles[trackCounter]->GetPz()};
    }
    else
    {
      trackPosition = {track->get_x(), track->get_y(), track->get_z()};
      trackMomentum = {track->get_px(), track->get_py(), track->get_pz()};
    }

    tracksJson["pxyz"] = trackMomentum;
    tracksJson["xyz"] = trackPosition;
    tracksJson["trk_color"] = jsonEntryName == triggerTrackName ? pidMap[particles[trackCounter]->GetPDG()] : allColour;
    tracksJson["nh"] = 60;
    tracksJson["l"] = length;
    tracksJson["q"] = jsonEntryName == triggerTrackName ? particles[trackCounter]->Q() : track->get_charge();

    jsonData[trackName][jsonEntryName] += tracksJson;    

    ++trackCounter;

  } //End of track loop

  //Now add mother
  if (jsonEntryName == triggerTrackName)
  {
    tracksJson["pxyz"] = {mother->GetPx(), mother->GetPy(), mother->GetPz()};
    tracksJson["xyz"] = {mother->GetX(), mother->GetY(), mother->GetZ()};
    tracksJson["trk_color"] = motherColour;
    tracksJson["nh"] = 60;
    tracksJson["l"] = mother->GetDecayLength();
    tracksJson["q"] = mother->Q();

    jsonData[trackName][jsonEntryName] += tracksJson;

    for (auto& intermediate : intermediates)
    {
      tracksJson["pxyz"] = {intermediate->GetPx(), intermediate->GetPy(), intermediate->GetPz()};
      tracksJson["xyz"] = {intermediate->GetX(), intermediate->GetY(), intermediate->GetZ()};
      tracksJson["trk_color"] = intermediateColour;
      tracksJson["nh"] = 60;
      tracksJson["l"] = intermediate->GetDecayLength();
      tracksJson["q"] = intermediate->Q();

      jsonData[trackName][jsonEntryName] += tracksJson;
    } 
     
  }
}

//https://stackoverflow.com/questions/997946/how-to-get-current-time-and-date-in-c
std::string event_display_maker::getDate()
{
    std::time_t t = std::time(0);   // get time now
    std::tm* now = std::localtime(&t);

    std::stringstream date;
    date << (now->tm_year + 1900) << '-'
         << (now->tm_mon + 1) << '-'
         <<  now->tm_mday;
    return date.str();
}

bool event_display_maker::isInRange(float min, float value, float max)
{
  return min <= value && value <= max;
}

json event_display_maker::caloMetadata(std::string type)
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
