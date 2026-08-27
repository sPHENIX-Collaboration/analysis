// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef DISPLAYEVENTS_H
#define DISPLAYEVENTS_H

#include <fun4all/SubsysReco.h>

#include <trackbase/ActsGeometry.h>
#include <calotrigger/TriggerAnalyzer.h>
#include <calotrigger/TriggerRunInfo.h>
#include <ffarawobjects/Gl1Packet.h>
#include <globalvertex/SvtxVertex.h>
#include <globalvertex/SvtxVertexMap.h>
#include <trackbase/TrkrCluster.h>           // for TrkrCluster
#include <trackbase/TrkrClusterContainer.h>
#include <trackbase_historic/SvtxTrack.h>
#include <trackbase_historic/SvtxTrackMap.h>

//calo stuff
#include <calobase/RawClusterDefs.h>
#include <calobase/RawCluster.h>
#include <calobase/RawClusterContainer.h>
#include <calobase/RawClusterUtility.h>
#include <calobase/RawTowerContainer.h>
#include <calobase/RawTowerGeom.h>
#include <calobase/RawTowerGeomContainer.h>
#include <calobase/TowerInfo.h>
#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfoDefs.h>

#include <nlohmann/json.hpp>

#include <string>
#include <fstream>

using json = nlohmann::json;

class PHCompositeNode;
class SvtxTrackMap;
class SvtxTrack;
class SvtxVertexMap;
class SvtxVertex;
class TriggerAnalyzer;

//calo stuff
class RawCluster;
class RawClusterContainer;
class RawTower;
class RawTowerContainer;
class RawTowerGeomContainer;

class displayEvents : public SubsysReco
{
 public:

  displayEvents(const std::string &name = "displayEvents");

  ~displayEvents() override;

  int Init(PHCompositeNode *topNode) override;

  int process_event(PHCompositeNode *topNode) override;

  int End(PHCompositeNode *topNode) override;

  void setEventDisplayPath(std::string path) { m_evt_display_path = path; }

  void setMaxEvtDisplays(int max = 10) { m_max_displays = max; }

  void setDecayTag(std::string tag) { m_decay_tag = tag; }

  void useTheseBCOs(std::vector<uint64_t> bcoList) { m_bco_list = bcoList; }

  void plotsApproved(bool approved = true) { m_approved = approved; }

  void addCaloInfo(bool add = true) { loadCalos = add; }

  void dontShowTrackClusters(bool avoid = true) { m_dont_show_track_clus = avoid; }

 private:
  int counter = 0;
  int m_runNumber = 0;
  std::string m_evt_display_path = "./";
  std::string m_run_date = "2024-04-14";
  std::string m_decay_tag = "200 GeV p+p";
  std::vector<uint64_t> m_bco_list;
  int m_max_displays = 100;
  bool m_approved = false;
  bool loadCalos = false;
  bool m_dont_show_track_clus = false;

  int load_nodes(PHCompositeNode *topNode);
  std::string getParticleName(int ID);
  SvtxTrack *getTrack(unsigned int track_id, SvtxTrackMap *trackmap);
  void getJSONdata(std::vector<SvtxTrack*> tracks, json &data);
  std::string getDate();
  bool isInRange(float min, float value, float max);

  ActsGeometry *geometry {nullptr};
  SvtxTrack *m_track {nullptr};
  SvtxTrackMap *m_trackMap {nullptr};
  SvtxTrackMap *m_kfp_trackMap {nullptr};
  SvtxVertexMap *m_vertexMap {nullptr};
  SvtxVertex *m_vertex {nullptr};
  Gl1Packet *gl1packet {nullptr};
  TriggerAnalyzer *triggeranalyzer {nullptr};
  TriggerRunInfo *triggerruninfo {nullptr};
  TrkrClusterContainer *dst_clustermap {nullptr};

  std::vector<SvtxTrack*> all_tracks;

  std::ofstream json_output;

  std::string eventName = "EVENT";
  std::string metadataName = "META";
  std::string allTrackName = "ALL";
  std::string trackName = "TRACKS";
  std::string hitsName = "HITS";
  std::vector<std::string> hitsNameVector = {allTrackName};

  int yellow = 16252672;
  int orange = 16743424;
  int green = 1834752;
  int turquoise = 4251856;
  int blue = 12031;
  int red = 16711680;
  int pink = 16070058;
  int white = 16446450;

  long emcalColor = 16766464;
  long ihcalColor = 4290445312;
  long ohcalColor = 24773;

  int allColour = white;

  std::map<std::string, int> pidToColourMap{
  {allTrackName, allColour}};

  //calo stuff
  void getJSONcalo(json &jsonData, std::string calo, TowerInfoContainer *towerCont, RawTowerGeomContainer *geom);
  json caloMetadata(std::string type);
  TowerInfoContainer *towersEM{nullptr};
  RawTowerGeomContainer *geomEMCal{nullptr};
  TowerInfoContainer *towersIH{nullptr};
  RawTowerGeomContainer *geoIHCal{nullptr};
  TowerInfoContainer *towersOH{nullptr};
  RawTowerGeomContainer *geoOHCal{nullptr};

  bool hasEMCal = true;
  bool hasIHCal = true;
  bool hasOHCal = true;

  std::string emcalName = "EMCal";
  std::string ihcalName = "IHCal";
  std::string ohcalName = "OHCal";

  std::map<std::string, float> caloThreshold{
    {emcalName, 0.08},
    {ihcalName, 0.01},
    {ohcalName, 0.04}};
    //{emcalName, 0.12},
    //{ihcalName, 0.01},
    //{ohcalName, 0.04}};
};

#endif // DISPLAYEVENTS_H
