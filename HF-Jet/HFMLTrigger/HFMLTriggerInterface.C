#include "HFMLTriggerInterface.h"

#include <fun4all/Fun4AllReturnCodes.h>
#include <phool/PHCompositeNode.h>
#include <phool/PHTimeServer.h>
#include <phool/PHTimer.h>
#include <phool/getClass.h>

#include <phool/PHCompositeNode.h>

#include <g4main/PHG4Hit.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4TruthInfoContainer.h>

#include <g4detectors/PHG4Cell.h>
#include <g4detectors/PHG4CellContainer.h>
#include <g4detectors/PHG4CylinderCellGeom.h>
#include <g4detectors/PHG4CylinderCellGeomContainer.h>
#include <g4detectors/PHG4CylinderGeom.h>
#include <g4detectors/PHG4CylinderGeomContainer.h>
#include <g4detectors/PHG4CylinderGeomSiLadders.h>
#include <g4detectors/PHG4CylinderGeom_MAPS.h>

#include <g4hough/SvtxCluster.h>
#include <g4hough/SvtxClusterMap.h>
#include <g4hough/SvtxHit.h>
#include <g4hough/SvtxHitMap.h>
#include <g4hough/SvtxTrack.h>
#include <g4hough/SvtxTrackMap.h>
#include <g4hough/SvtxVertex.h>
#include <g4hough/SvtxVertexMap.h>

#include <g4eval/SvtxEvalStack.h>

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
  , _svtxevalstack(nullptr)
  , m_hitMap(nullptr)
  , m_Geoms(nullptr)
  , m_truthInfo(nullptr)
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
  m_hitMap = findNode::getClass<SvtxHitMap>(topNode, "SvtxHitMap");
  if (!m_hitMap)
  {
    std::cout << PHWHERE << "ERROR: Can't find node SvtxHitMap" << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }
  m_Geoms =
      findNode::getClass<PHG4CylinderGeomContainer>(topNode, "CYLINDERGEOM_MAPS");
  if (!m_Geoms)
  {
    std::cout << PHWHERE << "ERROR: Can't find node CYLINDERCELLGEOM_SVTX" << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  m_truthInfo = findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");
  if (!m_truthInfo)
  {
    std::cout << PHWHERE << "ERROR: Can't find node G4TruthInfo" << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

int HFMLTriggerInterface::process_event(PHCompositeNode* topNode)
{
  if (!_svtxevalstack)
  {
    _svtxevalstack = new SvtxEvalStack(topNode);
    _svtxevalstack->set_strict(0);
    _svtxevalstack->set_verbosity(Verbosity() + 1);
  }
  else
  {
    _svtxevalstack->next_event(topNode);
  }

  //  SvtxVertexEval* vertexeval = _svtxevalstack->get_vertex_eval();
  //  SvtxTrackEval* trackeval = _svtxevalstack->get_track_eval();
  SvtxClusterEval* clustereval = _svtxevalstack->get_cluster_eval();
  SvtxHitEval* hiteval = _svtxevalstack->get_hit_eval();
  //    SvtxTruthEval* trutheval = _svtxevalstack->get_truth_eval();

  PHHepMCGenEventMap* geneventmap = findNode::getClass<PHHepMCGenEventMap>(topNode, "PHHepMCGenEventMap");
  if (!geneventmap)
  {
    std::cout << PHWHERE << " - Fatal error - missing node PHHepMCGenEventMap" << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  PHHepMCGenEvent* genevt = geneventmap->get(_embedding_id);
  if (!genevt)
  {
    std::cout << PHWHERE << " - Fatal error - node PHHepMCGenEventMap missing subevent with embedding ID " << _embedding_id;
    std::cout << ". Print PHHepMCGenEventMap:";
    geneventmap->identify();
    return Fun4AllReturnCodes::ABORTRUN;
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
  using boost::property_tree::ptree;
  ptree pt;

  auto loadCoordinate = [](double x, double y, double z) {
    ptree vertexTree;

    ptree vertexX;
    vertexX.put("", x);
    vertexTree.push_back(make_pair("", vertexX));

    ptree vertexY;
    vertexY.put("", y);
    vertexTree.push_back(make_pair("", vertexY));

    ptree vertexZ;
    vertexZ.put("", z);
    vertexTree.push_back(make_pair("", vertexZ));

    return vertexTree;
  };

  // Create a root
  ptree pTree;

  // meta data
  ptree metaTree;

  metaTree.put("Description", "These are meta data for this event. Not intended to use in ML algorithm");
  metaTree.put("EventID", _ievent);
  metaTree.put("Unit", "cm");
  metaTree.add_child("CollisionVertex",
                     loadCoordinate(genevt->get_collision_vertex().x(),
                                    genevt->get_collision_vertex().y(),
                                    genevt->get_collision_vertex().z()));

  metaTree.put("Layer_Count", _nlayers_maps);
  metaTree.put("PixelHalfLayerIndex_Count", _nlayers_maps * 2);

  //  hitIDTree.put("PixelPhiIndex", pixelPhiIndex);
  //  hitIDTree.put("PixelZIndex", pixelZIndex);
  //
  //  hitIDTree.put("Layer", layer);
  //  hitIDTree.put("HalfLayer", halflayer);
  //  hitIDTree.put("Stave", cell->get_stave_index());
  //  hitIDTree.put("HalfStave", cell->get_half_stave_index());
  //  hitIDTree.put("Module", cell->get_module_index());
  //  hitIDTree.put("Chip", cell->get_chip_index());
  //  hitIDTree.put("Pixel", cell->get_pixel_index());

  for (unsigned int layer = 0; layer < _nlayers_maps; ++layer)
  {
    PHG4CylinderGeom_MAPS* geom = dynamic_cast<PHG4CylinderGeom_MAPS*>(m_Geoms->GetLayerGeom(layer));
    assert(geom);

    ptree layerDescTree;

    static const unsigned int nChip(9);

    layerDescTree.put("PixelPhiIndexInLayer_Count", geom->get_N_staves() * geom->get_NX());
    layerDescTree.put("PixelPhiIndexInHalfLayer_Count", geom->get_N_staves() * geom->get_NX() / 2);
    layerDescTree.put("PixelZIndex_Count", nChip * geom->get_NZ());
    layerDescTree.put("HalfLayer_Count", 2);
    layerDescTree.put("Stave_Count", geom->get_N_staves());
    layerDescTree.put("Chip_Count", nChip);
    layerDescTree.put("Pixel_Count", geom->get_NX() * geom->get_NZ());

    metaTree.add_child(str(boost::format{"Layer%1%"} % layer), layerDescTree);
  }

  ptree truthTriggerFlagTree;
  truthTriggerFlagTree.put("Description", "These are categorical true/false MonteCalo truth tags for the event. These are only known in training sample. This would be trigger output in real data processing.");
  truthTriggerFlagTree.put("ExampleSignal1", true);
  truthTriggerFlagTree.put("ExampleSignal2", false);

  // Raw hits
  ptree rawHitTree;
  rawHitTree.put("Description",
                 "Raw data in the event in an unordered set of hit ID. To help in visualization stage, the coordinate of the hit is also appended. These would be input in real data.");

  //  rawHitTree.put("LayerRage", "0-2");

  ptree rawHitsTree;

  set<unsigned int> mapsHits;
  assert(m_hitMap);
  for (SvtxHitMap::Iter iter = m_hitMap->begin();
       iter != m_hitMap->end();
       ++iter)
  {
    SvtxHit* hit = iter->second;
    assert(hit);
    unsigned int layer = hit->get_layer();
    if (layer < _nlayers_maps)
    {
      unsigned int hitID = hit->get_id();
      mapsHits.insert(hitID);

      PHG4Cell* cell = hiteval->get_cell(hit);
      assert(cell);
      PHG4CylinderGeom_MAPS* geom = dynamic_cast<PHG4CylinderGeom_MAPS*>(m_Geoms->GetLayerGeom(layer));
      assert(geom);

      TVector3 local_coords = geom->get_local_coords_from_pixel(cell->get_pixel_index());
      TVector3 world_coords = geom->get_world_from_local_coords(cell->get_stave_index(), cell->get_half_stave_index(), cell->get_module_index(), cell->get_chip_index(), local_coords);

      unsigned int pixel_x(cell->get_pixel_index() % geom->get_NX());
      unsigned int pixel_z(cell->get_pixel_index() / geom->get_NX());
      unsigned int halflayer = (int) pixel_x >= geom->get_NX() / 2 ? 0 : 1;

      assert((int) pixel_x < geom->get_NX());
      assert((int) pixel_z < geom->get_NZ());

      unsigned int halfLayerIndex(layer * 2 + halflayer);
      unsigned int pixelPhiIndex(
          cell->get_stave_index() * geom->get_NX() + pixel_x);
      unsigned int pixelPhiIndexHL(
          cell->get_stave_index() * geom->get_NX() / 2 + pixel_x % (geom->get_NX() / 2));
      unsigned int pixelZIndex(cell->get_chip_index() * geom->get_NZ() + pixel_z);

      ptree hitTree;

      ptree hitIDTree;
      hitIDTree.put("HitSequenceInEvent", hitID);

      hitIDTree.put("PixelHalfLayerIndex", halfLayerIndex);
      hitIDTree.put("PixelPhiIndexInLayer", pixelPhiIndex);
      hitIDTree.put("PixelPhiIndexInHalfLayer", pixelPhiIndexHL);
      hitIDTree.put("PixelZIndex", pixelZIndex);

      hitIDTree.put("Layer", layer);
      hitIDTree.put("HalfLayer", halflayer);
      hitIDTree.put("Stave", cell->get_stave_index());
      //      hitIDTree.put("HalfStave", cell->get_half_stave_index());
      //      hitIDTree.put("Module", cell->get_module_index());
      hitIDTree.put("Chip", cell->get_chip_index());
      hitIDTree.put("Pixel", cell->get_pixel_index());
      hitTree.add_child("ID", hitIDTree);

      hitTree.add_child("Coordinate",
                        loadCoordinate(world_coords.x(),
                                       world_coords.y(),
                                       world_coords.z()));

//      rawHitsTree.add_child("MVTXHit", hitTree);
      rawHitsTree.push_back(make_pair("", hitTree));

      m_hitStaveLayer->Fill(cell->get_stave_index(), halfLayerIndex);
      m_hitModuleHalfStave->Fill(cell->get_module_index(), cell->get_half_stave_index());
      m_hitChipModule->Fill(cell->get_chip_index(), cell->get_module_index());

      m_hitLayerMap->Fill(world_coords.x(), world_coords.y(), halfLayerIndex);
      m_hitPixelPhiMap->Fill(pixelPhiIndex, atan2(world_coords.y(), world_coords.x()), halfLayerIndex);
      m_hitPixelPhiMapHL->Fill(pixelPhiIndexHL, atan2(world_coords.y(), world_coords.x()), halfLayerIndex);
      m_hitPixelZMap->Fill(pixelZIndex, world_coords.z(), halfLayerIndex);
    }  //    if (layer < _nlayers_maps)

  }  //   for (SvtxHitMap::Iter iter = m_hitMap->begin();
  rawHitTree.add_child("MVTXHits", rawHitsTree);

  // Truth hits
  ptree truthHitTree;
  truthHitTree.put("Description", "From the MonteCalo truth information, pairs of track ID and subset of RawHit that belong to the track. These are not presented in real data. The track ID is arbitary.");

  assert(m_truthInfo);

  ptree truthTracksTree;

  PHG4TruthInfoContainer::ConstRange range = m_truthInfo->GetPrimaryParticleRange();
  for (PHG4TruthInfoContainer::ConstIterator iter = range.first;
       iter != range.second;
       ++iter)
  {
    PHG4Particle* g4particle = iter->second;
    assert(g4particle);

    std::set<SvtxCluster*> g4clusters = clustereval->all_clusters_from(g4particle);

    ptree trackHitTree;
    unsigned int nMAPS(0);

    for (const SvtxCluster* cluster : g4clusters)
    {
      assert(cluster);
      unsigned int layer = cluster->get_layer();
      if (layer < _nlayers_maps)
      {
        ++nMAPS;

        for (SvtxCluster::ConstHitIter hiter = cluster->begin_hits();
             hiter != cluster->end_hits();
             ++hiter)
        {
          //        SvtxHit* hit = _hitmap->get(*hiter);
          unsigned int hitID = *hiter;
          assert(mapsHits.find(hitID) != mapsHits.end());

          ptree hitIDTree;
          hitIDTree.put("", hitID);
          trackHitTree.push_back(make_pair("", hitIDTree));
        }
      }

    }  //    for (const SvtxCluster* cluster : g4clusters)

    if (nMAPS > 1)
    {
      ptree trackTree;
      trackTree.put("TrackSequenceInEvent", g4particle->get_track_id());
      trackTree.add_child("HitSequenceInEvent", trackHitTree);

      trackTree.put("ParticleTypeID", g4particle->get_pid());
      trackTree.add_child("TrackMomentum",
                          loadCoordinate(g4particle->get_px(),
                                         g4particle->get_py(),
                                         g4particle->get_pz()));
      //      trackTree.put("TrackDCA3DXY", track->get_dca3d_xy());
      //      trackTree.put("TrackDCA3DZ", track->get_dca3d_z());

      //      trackTree.add_child("TruthHit", trackHitTree);

//      truthTracksTree.add_child("TruthTrack", trackTree);
      truthTracksTree.push_back(make_pair("", trackTree));
    }  //      if (nMAPS > 1)

  }  //  for (PHG4TruthInfoContainer::ConstIterator iter = range.first;
  truthHitTree.add_child("TruthTracks", truthTracksTree);

  //output
  pTree.add_child("MetaData", metaTree);
  pTree.add_child("TruthTriggerFlag", truthTriggerFlagTree);
  pTree.add_child("RawHit", rawHitTree);
  pTree.add_child("TruthHit", truthHitTree);

  assert(m_jsonOut.is_open());
  write_json(m_jsonOut, pTree);
  //  write_xml(m_jsonOut, jsonTree);

  ++_ievent;

  return Fun4AllReturnCodes::EVENT_OK;
}

int HFMLTriggerInterface::End(PHCompositeNode* topNode)
{
  if (_f)
  {
    _f->cd();
    _f->Write();
    //_f->Close();

    //    m_hitLayerMap->Write();
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
