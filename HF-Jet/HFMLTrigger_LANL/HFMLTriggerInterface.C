#include "HFMLTriggerInterface.h"

#include <fun4all/Fun4AllReturnCodes.h>
#include <pdbcalbase/PdbParameterMap.h>
#include <phool/PHCompositeNode.h>
#include <phool/PHTimeServer.h>
#include <phool/PHTimer.h>
#include <phool/getClass.h>

#include <phool/PHCompositeNode.h>

#include <g4main/PHG4Hit.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4HitContainer.h>
#include <g4main/PHG4TruthInfoContainer.h>

//#include <trackbase/TrkrHitTruthAssoc.h>
#include <trackbase/TrkrHitSetContainer.h>
//#include <trackbase/TrkrClusterHitAssoc.h>
#include <trackbase/TrkrHitSet.h>
#include <trackbase/TrkrHit.h>
//#include <trackbase/TrkrCluster.h>

#include <mvtx/MvtxDefs.h>
#include <mvtx/CylinderGeom_Mvtx.h>

#include <g4detectors/PHG4Cell.h>
#include <g4detectors/PHG4CellContainer.h>
#include <g4detectors/PHG4CylinderCellGeom.h>
#include <g4detectors/PHG4CylinderCellGeomContainer.h>
#include <g4detectors/PHG4CylinderGeom.h>
#include <g4detectors/PHG4CylinderGeomContainer.h>

//#include <g4eval/SvtxEvalStack.h>

#include <HepMC/GenEvent.h>
#include <HepMC/GenVertex.h>
#include <phhepmc/PHHepMCGenEvent.h>
#include <phhepmc/PHHepMCGenEventMap.h>

#include <TDatabasePDG.h>
#include <TFile.h>
#include <TH2F.h>
#include <TH3F.h>
#include <TLorentzVector.h>
#include <TString.h>
#include <TTree.h>

#include <rapidjson/document.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/prettywriter.h>

#include <boost/format.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>


#include <cassert>
#include <cmath>
#include <cstddef>
#include <iostream>

using namespace std;

HFMLTriggerInterface::HFMLTriggerInterface(std::string filename)
  : SubsysReco("HFMLTriggerInterface")
  , _ievent(0)
  , _f(nullptr)
  , _eta_min(-1)
  , _eta_max(1)
  , _embedding_id(1)
  , _nlayers_maps(3)
  , m_hitsets(nullptr)
  , m_GenEventMap(nullptr)
  , m_truthInfo(nullptr)
  , m_g4hits_mvtx(nullptr)
 // , _svtxevalstack(nullptr)
 // , m_hit_truth_map(nullptr)
  , m_Flags(nullptr)
  , m_Geoms(nullptr)
  , m_hitStaveLayer(nullptr)
  , m_hitModuleHalfStave(nullptr)
  , m_hitChipModule(nullptr)
  , m_hitLayerMap(nullptr)
  , m_hitPixelPhiMap(nullptr)
  , m_hitPixelPhiMapHL(nullptr)
  , m_hitPixelZMap(nullptr)
{
  _foutname = filename;
}

int HFMLTriggerInterface::Init(PHCompositeNode* topNode)
{
  _ievent = 0;

  _f = new TFile((_foutname + string(".root")).c_str(), "RECREATE");

  m_jsonOut.open((_foutname + string(".json")).c_str(), fstream::out);

  m_jsonOut << "{" << endl;
  m_jsonOut << "\"Events\" : [" << endl;

  //  _h2 = new TH2D("h2", "", 100, 0, 100.0, 40, -2, +2);
  //  _h2_b = new TH2D("h2_b", "", 100, 0, 100.0, 40, -2, +2);
  //  _h2_c = new TH2D("h2_c", "", 100, 0, 100.0, 40, -2, +2);
  //  _h2all = new TH2D("h2all", "", 100, 0, 100.0, 40, -2, +2);

  m_hitLayerMap = new TH3F("hitLayerMap", "hitLayerMap", 600, -10, 10, 600, -10, 10, 10, -.5, 9.5);
  m_hitLayerMap->SetTitle("hitLayerMap;x [mm];y [mm];Half Layers");

  m_hitPixelPhiMap = new TH3F("hitPixelPhiMap", "hitPixelPhiMap", 16000, -.5, 16000 - .5, 600, -M_PI, M_PI, 10, -.5, 9.5);
  m_hitPixelPhiMap->SetTitle("hitPixelPhiMap;PixelPhiIndex in layer;phi [rad];Half Layers Index");
  m_hitPixelPhiMapHL = new TH3F("hitPixelPhiMapHL", "hitPixelPhiMapHL", 16000, -.5, 16000 - .5, 600, -M_PI, M_PI, 10, -.5, 9.5);
  m_hitPixelPhiMapHL->SetTitle("hitPixelPhiMap;PixelPhiIndex in half layer;phi [rad];Half Layers Index");
  m_hitPixelZMap = new TH3F("hitPixelZMap", "hitPixelZMap", 16000, -.5, 16000 - .5, 600, 15, 15, 10, -.5, 9.5);
  m_hitPixelZMap->SetTitle("hitPixelZMap;hitPixelZMap;z [cm];Half Layers");

  m_hitStaveLayer = new TH2F("hitStaveLayer", "hitStaveLayer", 100, -.5, 100 - .5, 10, -.5, 9.5);
  m_hitStaveLayer->SetTitle("hitStaveLayer;Stave index;Half Layers");
  m_hitModuleHalfStave = new TH2F("hitModuleHalfStave", "hitModuleHalfStave", 100, -.5, 100 - .5, 10, -.5, 9.5);
  m_hitModuleHalfStave->SetTitle("hitModuleHalfStave;Module index;Half Stave");
  m_hitChipModule = new TH2F("hitChipModule", "hitChipModule", 100, -.5, 100 - .5, 10, -.5, 9.5);
  m_hitChipModule->SetTitle("hitChipModule;Chip;Module");

  return Fun4AllReturnCodes::EVENT_OK;
}

int HFMLTriggerInterface::InitRun(PHCompositeNode* topNode)
{
  m_Geoms =
      findNode::getClass<PHG4CylinderGeomContainer>(topNode, "CYLINDERGEOM_MVTX");
  if (!m_Geoms)
  {
    std::cout << PHWHERE << "ERROR: Can't find node CYLINDERCELLGEOM_MVTX" << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  m_truthInfo = findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");
  if (!m_truthInfo)
  {
    std::cout << PHWHERE << "ERROR: Can't find node G4TruthInfo" << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  m_Flags = findNode::getClass<PdbParameterMap>(topNode, "HFMLTrigger_HepMCTriggerFlags");
  if (!m_Flags)
  {
    cout << "HFMLTriggerInterface::InitRun - WARNING - missing HFMLTrigger_HepMCTriggerFlags" << endl;
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

int HFMLTriggerInterface::process_event(PHCompositeNode* topNode)
{
  // load nodes
  auto res = load_nodes(topNode);
  if (res != Fun4AllReturnCodes::EVENT_OK)
    return res;


 // if (!_svtxevalstack)
 // {
 //   _svtxevalstack = new SvtxEvalStack(topNode);
 //   _svtxevalstack->set_strict(0);
 //   _svtxevalstack->set_verbosity(Verbosity() + 1);
 // }
 // else
 // {
 //   _svtxevalstack->next_event(topNode);
 // }

  //  SvtxVertexEval* vertexeval = _svtxevalstack->get_vertex_eval();
  //  SvtxTrackEval* trackeval = _svtxevalstack->get_track_eval();
 // SvtxClusterEval* clustereval = _svtxevalstack->get_cluster_eval();
  //SvtxHitEval* hiteval = _svtxevalstack->get_hit_eval();
  //    SvtxTruthEval* trutheval = _svtxevalstack->get_truth_eval();

  assert(m_GenEventMap);

  PHHepMCGenEvent* genevt = m_GenEventMap->get(_embedding_id);
  if (!genevt)
  {
    std::cout << PHWHERE << " - Fatal error - node PHHepMCGenEventMap missing subevent with embedding ID " << _embedding_id;
    std::cout << ". Print PHHepMCGenEventMap:";
    m_GenEventMap->identify();
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  HepMC::GenEvent* theEvent = genevt->getEvent();
  assert(theEvent);
  if (Verbosity())
  {
    cout << "HFMLTriggerInterface::process_event - process HepMC::GenEvent with signal_process_id = "
         << theEvent->signal_process_id();
    if (theEvent->signal_process_vertex())
    {
      cout << " and signal_process_vertex : ";
      theEvent->signal_process_vertex()->print();
    }
    cout << "  - Event record:" << endl;
    theEvent->print();
  }

  // property tree preparation
  //  using boost::property_tree::ptree;
  //  ptree pt;

  rapidjson::Document d;
  d.SetObject();
  rapidjson::Document::AllocatorType& alloc = d.GetAllocator();

  auto loadCoordinate = [&](double x, double y, double z) {
    //    ptree vertexTree;
    rapidjson::Value vertexTree(rapidjson::kArrayType);

    //    ptree vertexX;
    //    vertexX.put("", x);
    //    vertexTree.push_back(make_pair("", vertexX));

    //    ptree vertexY;
    //    vertexY.put("", y);
    //    vertexTree.push_back(make_pair("", vertexY));

    //    ptree vertexZ;
    //    vertexZ.put("", z);
    //    vertexTree.push_back(make_pair("", vertexZ));

    vertexTree.PushBack(x, alloc).PushBack(y, alloc).PushBack(z, alloc);

    return vertexTree;
  };

  // Create a root
  //  ptree pTree;

  // meta data
  //  ptree metaTree;
  rapidjson::Value metaTree(rapidjson::kObjectType);

  metaTree.AddMember("Description", "These are meta data for this event. Not intended to use in ML algorithm", alloc);
  metaTree.AddMember("EventID", _ievent, alloc);
  metaTree.AddMember("Unit", "cm", alloc);
  metaTree.AddMember("CollisionVertex",
                     loadCoordinate(genevt->get_collision_vertex().x(),
                                    genevt->get_collision_vertex().y(),
                                    genevt->get_collision_vertex().z()),
                     alloc);

  metaTree.AddMember("Layer_Count", _nlayers_maps, alloc);
  metaTree.AddMember("PixelHalfLayerIndex_Count", _nlayers_maps * 2, alloc);

  for (unsigned int layer = 0; layer < _nlayers_maps; ++layer)
  {
    CylinderGeom_Mvtx* geom = dynamic_cast<CylinderGeom_Mvtx*>(m_Geoms->GetLayerGeom(layer));
    assert(geom);

    //    ptree layerDescTree;
    rapidjson::Value layerDescTree(rapidjson::kObjectType);

    static const unsigned int nChip(9);

    layerDescTree.AddMember("PixelPhiIndexInLayer_Count", geom->get_N_staves() * geom->get_NX(), alloc);
    layerDescTree.AddMember("PixelPhiIndexInHalfLayer_Count", geom->get_N_staves() * geom->get_NX() / 2, alloc);
    layerDescTree.AddMember("PixelZIndex_Count", nChip * geom->get_NZ(), alloc);
    layerDescTree.AddMember("HalfLayer_Count", 2, alloc);
    layerDescTree.AddMember("Stave_Count", geom->get_N_staves(), alloc);
    layerDescTree.AddMember("Chip_Count", nChip, alloc);
    layerDescTree.AddMember("Pixel_Count", geom->get_NX() * geom->get_NZ(), alloc);

    //    metaTree.AddMember(
    //        str(boost::format{"Layer%1%"} % layer).c_str(),
    //        layerDescTree, alloc);
    rapidjson::Value keyName(str(boost::format{"Layer%1%"} % layer).c_str(), alloc);
    metaTree.AddMember(keyName,
                       layerDescTree, alloc);
  }

  //  ptree truthTriggerFlagTree;
  rapidjson::Value truthTriggerFlagTree(rapidjson::kObjectType);

  truthTriggerFlagTree.AddMember("Description",
                                 "These are categorical true/false MonteCalo truth tags for the event. These are only known in training sample. This would be trigger output in real data processing.",
                                 alloc);
  //    truthTriggerFlagTree.AddMember("ExampleSignal1", true, alloc);
  //    truthTriggerFlagTree.AddMember("ExampleSignal2", false, alloc);
  rapidjson::Value flagsTree(rapidjson::kObjectType);
  if (m_Flags)
  {
    auto range = m_Flags->get_iparam_iters();

    for (auto flagIter = range.first; flagIter != range.second; ++flagIter)
    {
//      rapidjson::Value aFlag(rapidjson::kObjectType);

      const string& name = flagIter->first;
      rapidjson::Value keyName(name.c_str(), alloc);
      const bool flag = flagIter->second > 0 ? true : false;

      flagsTree.AddMember(keyName, flag, alloc);
//      flagsTree.PushBack(aFlag, alloc);
    }
  }
  truthTriggerFlagTree.AddMember("Flags", flagsTree, alloc);

  // Raw hits
  //  ptree rawHitTree;
  rapidjson::Value rawHitTree(rapidjson::kObjectType);
  rawHitTree.AddMember("Description",
                       "Raw data in the event in an unordered set of hit ID. To help in visualization stage, the coordinate of the hit is also appended. These would be input in real data.",
                       alloc);

  //  rawHitTree.put("LayerRage", "0-2");

  //  ptree rawHitsTree;
  rapidjson::Value rawHitsTree(rapidjson::kArrayType);

  assert(m_hitsets);
  unsigned int hitID(0);
  auto hitset_range = m_hitsets->getHitSets(TrkrDefs::TrkrId::mvtxId);
  for (auto hitset_iter =  hitset_range.first; hitset_iter != hitset_range.second; ++hitset_iter)
  {
    auto hitsetkey = hitset_iter->first;
    auto hit_range = hitset_iter->second->getHits();
    for (auto hit_iter = hit_range.first;
         hit_iter != hit_range.second;
         ++hit_iter)
    {
      auto hitkey = hit_iter->first;
      auto hit = hit_iter->second;
      assert(hit);
      unsigned int layer = TrkrDefs::getLayer(hitset_iter->first);
      if (layer < _nlayers_maps)
      {
        CylinderGeom_Mvtx* geom = dynamic_cast<CylinderGeom_Mvtx*>(m_Geoms->GetLayerGeom(layer));
        assert(geom);

        unsigned int pixel_x = MvtxDefs::getRow(hitkey);
        unsigned int pixel_z = MvtxDefs::getCol(hitkey);
        assert((int)pixel_x < geom->get_NX());
        assert((int)pixel_z < geom->get_NZ());
        unsigned int chip = MvtxDefs::getChipId(hitsetkey);
        unsigned int stave = MvtxDefs::getStaveId(hitsetkey);
        TVector3 local_coords = geom->get_local_coords_from_pixel(pixel_x, pixel_z);
        TVector3 world_coords = geom->get_world_from_local_coords(stave,
                                                                  0,
                                                                  0,
                                                                  chip,
                                                                  local_coords);

        //unsigned int halflayer = (int) pixel_x >= geom->get_NX() / 2 ? 0 : 1;

        //unsigned int halfLayerIndex(layer * 2 + halflayer);
        //unsigned int pixelPhiIndex(
        //  cell->get_stave_index() * geom->get_NX() + pixel_x);
        //unsigned int pixelPhiIndexHL(
        //  cell->get_stave_index() * geom->get_NX() / 2 + pixel_x % (geom->get_NX() / 2));
        // unsigned int pixelZIndex(cell->get_chip_index() * geom->get_NZ() + pixel_z);

        //      ptree hitTree;
        rapidjson::Value hitTree(rapidjson::kObjectType);

        //      ptree hitIDTree;
        rapidjson::Value hitIDTree(rapidjson::kObjectType);
        hitIDTree.AddMember("HitSequenceInEvent", hitID, alloc);

        //hitIDTree.AddMember("PixelHalfLayerIndex", halfLayerIndex, alloc);
        //hitIDTree.AddMember("PixelPhiIndexInLayer", pixelPhiIndex, alloc);
        //hitIDTree.AddMember("PixelPhiIndexInHalfLayer", pixelPhiIndexHL, alloc);
        //hitIDTree.AddMember("PixelZIndex", pixelZIndex, alloc);

        hitIDTree.AddMember("Layer", layer, alloc);
        //hitIDTree.AddMember("HalfLayer", halflayer, alloc);
        hitIDTree.AddMember("Stave", stave, alloc);
        //      hitIDTree.put("HalfStave", cell->get_half_stave_index());
        //      hitIDTree.put("Module", cell->get_module_index());
        hitIDTree.AddMember("Chip", chip, alloc);
        hitIDTree.AddMember("Pixel_x", pixel_x, alloc);
        hitIDTree.AddMember("Pixel_z", pixel_z, alloc);
        hitTree.AddMember("ID", hitIDTree, alloc);

        hitTree.AddMember("Coordinate",
                          loadCoordinate(world_coords.x(),
                                         world_coords.y(),
                                         world_coords.z()),
                          alloc);

        //      rawHitsTree.add_child("MVTXHit", hitTree);
        rawHitsTree.PushBack(hitTree, alloc);

        m_hitStaveLayer->Fill(stave, layer);
        m_hitModuleHalfStave->Fill(stave, layer);
        m_hitChipModule->Fill(chip, stave);

        m_hitLayerMap->Fill(world_coords.x(), world_coords.y(), layer);
        //m_hitPixelPhiMap->Fill(pixelPhiIndex, atan2(world_coords.y(), world_coords.x()), layer);
        //m_hitPixelPhiMapHL->Fill(pixelPhiIndexHL, atan2(world_coords.y(), world_coords.x()), layer);
        //m_hitPixelZMap->Fill(pixelZIndex, world_coords.z(), halfLayerIndex);
        ++hitID;
      }  //    if (layer < _nlayers_maps)
    }  //   for (TrkrHitSetContainer::ConstIterator hitset_iter = hitset_range.first;
  }  //   for (TrkrHitSet::ConstIterator hit_iter = hit_range.first;
  rawHitTree.AddMember("MVTXHits", rawHitsTree, alloc);

  // Truth hits
  //  ptree truthHitTree;
  rapidjson::Value truthHitTree(rapidjson::kObjectType);
  truthHitTree.AddMember("Description", "From the MonteCalo truth information, pairs of track ID and subset of RawHit that belong to the track. These are not presented in real data. The track ID is arbitary.",
                         alloc);

    assert(m_g4hits_mvtx);
    std::multimap<const int, const unsigned long long> m_track_g4hits_map;
    for (auto g4hit_iter = m_g4hits_mvtx->getHits().first;
        g4hit_iter != m_g4hits_mvtx->getHits().second;
        ++g4hit_iter)
    {
      PHG4Hit *g4hit = g4hit_iter->second;
      m_track_g4hits_map.insert(make_pair(g4hit->get_trkid(), g4hit_iter->first));
    }

  //  ptree truthTracksTree;
  rapidjson::Value truthTracksTree(rapidjson::kArrayType);

  // get set of primary particle
  assert(m_truthInfo);
  auto pp_range = m_truthInfo->GetPrimaryParticleRange();
  for (auto pp_iter = pp_range.first;
       pp_iter != pp_range.second;
       ++pp_iter)
  {
    PHG4Particle *g4particle = pp_iter->second;
    assert(g4particle);

    if (! m_track_g4hits_map.count(g4particle->get_track_id()))
    {
      if (Verbosity() >= VERBOSITY_MORE)
        std::cout << "WARNING: G4 particle " << g4particle->get_track_id() \
                  << " does not hit any MVTX layer." << std::endl;
      continue;
    }

    //    ptree trackHitTree;
    rapidjson::Value trackHitTree(rapidjson::kArrayType);
    //unsigned int nMAPS(0);
    auto g4hits_iter = m_track_g4hits_map.equal_range(g4particle->get_track_id());
    for (auto& g4hit_iter = g4hits_iter.first;
        g4hit_iter != g4hits_iter.second; ++g4hit_iter)
    {
      //          ptree hitIDTree;
      //          hitIDTree.put("", g4hit_key);
      trackHitTree.PushBack(static_cast<uint64_t>(g4hit_iter->second), alloc);
    }  //   for (auto& clus_key : clus_keys)

    //      ptree trackTree;
    rapidjson::Value trackTree(rapidjson::kObjectType);
    trackTree.AddMember("TrackSequenceInEvent", g4particle->get_track_id(), alloc);
    trackTree.AddMember("HitSequenceInEvent", trackHitTree, alloc);

    trackTree.AddMember("ParticleTypeID", g4particle->get_pid(), alloc);
    trackTree.AddMember("TrackMomentum",
                        loadCoordinate(g4particle->get_px(),
                                       g4particle->get_py(),
                                       g4particle->get_pz()),
                        alloc);
      //      trackTree.put("TrackDCA3DXY", track->get_dca3d_xy());
      //      trackTree.put("TrackDCA3DZ", track->get_dca3d_z());

      //      trackTree.add_child("TruthHit", trackHitTree);

      //      truthTracksTree.add_child("TruthTrack", trackTree);
    truthTracksTree.PushBack(trackTree, alloc);
  }  //  for (PHG4TruthInfoContainer::ConstIterator iter = range.first;

  truthHitTree.AddMember("TruthTracks", truthTracksTree, alloc);

  //output
  d.AddMember("MetaData", metaTree, alloc);
  d.AddMember("TruthTriggerFlag", truthTriggerFlagTree, alloc);
  d.AddMember("RawHit", rawHitTree, alloc);
  d.AddMember("TruthHit", truthHitTree, alloc);

  assert(m_jsonOut.is_open());

  if (_ievent > 0)
  {
    m_jsonOut << "," << endl;
  }

  //  write_json(m_jsonOut, pTree);
  //  write_xml(m_jsonOut, jsonTree);

  //  d.AddMember("Test", 1, d.GetAllocator());

  rapidjson::OStreamWrapper osw(m_jsonOut);
  rapidjson::PrettyWriter<rapidjson::OStreamWrapper> writer(osw);

  d.Accept(writer);

  ++_ievent;

  return Fun4AllReturnCodes::EVENT_OK;
}

int HFMLTriggerInterface::End(PHCompositeNode* topNode)
{
  if (_f)
  {
    _f->cd();
    _f->Write();
  }

  if (m_jsonOut.is_open())
  {
    m_jsonOut << "]" << endl;
    m_jsonOut << "}" << endl;

    m_jsonOut.close();
  }

  cout << "HFMLTriggerInterface::End - output to " << _foutname << ".*" << endl;

  return Fun4AllReturnCodes::EVENT_OK;
}

int HFMLTriggerInterface::load_nodes(PHCompositeNode* topNode)
{

  m_hitsets = findNode::getClass<TrkrHitSetContainer>(topNode, "TRKR_HITSET");
  if (!m_hitsets)
  {
    std::cout << PHWHERE << "ERROR: Can't find node TRKR_HITSET" << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  //m_hit_truth_map = findNode::getClass<TrkrHitTruthAssoc>(topNode, "TRKR_HITTRUTHASSOC");
  //if (!m_hit_truth_map)
  //{
  //  std::cout << PHWHERE << " unable to find DST node TRKR_HITTRUTHASSOC" << std::endl;
  //  return Fun4AllReturnCodes::ABORTEVENT;
  //}

  //m_cluster_hit_map = findNode::getClass<TrkrClusterHitAssoc>(topNode, "TRKR_CLUSTERHITASSOC");
  //if (!m_cluster_hit_map)
  //{
  //  std::cout << PHWHERE << " unable to find DST node TRKR_CLUSTERHITASSOC" << std::endl;
  //  return Fun4AllReturnCodes::ABORTEVENT;
  //}

  m_g4hits_mvtx = findNode::getClass<PHG4HitContainer>(topNode, "G4HIT_MVTX");
  if (!m_g4hits_mvtx)
  {
    std::cout << PHWHERE << " unable to find DST node G4HIT_MVTX" << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  m_GenEventMap = findNode::getClass<PHHepMCGenEventMap>(topNode, "PHHepMCGenEventMap");
  if (!m_GenEventMap)
  {
    std::cout << PHWHERE << " - Fatal error - missing node PHHepMCGenEventMap" << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  return Fun4AllReturnCodes::EVENT_OK;
}
