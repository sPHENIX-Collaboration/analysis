#include "HFMLTriggerOccupancy.h"

#include <fun4all/Fun4AllReturnCodes.h>
#include <pdbcalbase/PdbParameterMap.h>
#include <phool/PHCompositeNode.h>
#include <phool/PHTimeServer.h>
#include <phool/PHTimer.h>
#include <phool/getClass.h>

#include <phool/PHCompositeNode.h>

#include <g4main/PHG4Hit.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4VtxPoint.h>
#include <g4main/PHG4TruthInfoContainer.h>

#include <g4detectors/PHG4Cell.h>
#include <g4detectors/PHG4CellContainer.h>
#include <g4detectors/PHG4CylinderCellGeom.h>
#include <g4detectors/PHG4CylinderCellGeomContainer.h>
#include <g4detectors/PHG4CylinderGeom.h>
#include <g4detectors/PHG4CylinderGeomContainer.h>

#include <g4eval/SvtxEvalStack.h>

#include <trackbase/TrkrHitSetContainerv1.h>
#include <trackbase/TrkrHitSetv1.h>
#include <trackbase/TrkrHitv1.h>

#include <mvtx/CylinderGeom_Mvtx.h>
#include <mvtx/MvtxDefs.h>

#include <HepMC/GenEvent.h>
#include <HepMC/GenVertex.h>
#include <phhepmc/PHHepMCGenEvent.h>
#include <phhepmc/PHHepMCGenEventMap.h>

#include <TDatabasePDG.h>
#include <TFile.h>
#include <TH1D.h>
#include <TH1F.h>
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

HFMLTriggerOccupancy::HFMLTriggerOccupancy(std::string filename)
  : SubsysReco("HFMLTriggerOccupancy")
  , _ievent(0)
  , _f(nullptr)
  , _eta_min(-1)
  , _eta_max(1)
  , _embedding_id(1)
  , _nlayers_maps(3)
  , _svtxevalstack(nullptr)
  , m_hitsetcont(nullptr)
  , m_Geoms(nullptr)
  , m_truthInfo(nullptr)
  , m_Flags(nullptr)
  , m_hNorm(nullptr)
  , m_hNChEta(nullptr)
  , m_hVertexZ(nullptr)
  , m_hitStaveLayer(nullptr)
  , m_hitModuleHalfStave(nullptr)
  , m_hitChipModule(nullptr)
  , m_hitLayerMap(nullptr)
  , m_hitPixelPhiMap(nullptr)
  , m_hitPixelPhiMapHL(nullptr)
  , m_hitPixelZMap(nullptr)
  , m_Multiplicity(nullptr)
  , m_LayerMultiplicity(nullptr)
  , m_LayerChipMultiplicity(nullptr)
{
  _foutname = filename;
}

int HFMLTriggerOccupancy::Init(PHCompositeNode* topNode)
{
  _ievent = 0;

  _f = new TFile((_foutname + string(".root")).c_str(), "RECREATE");

  //  m_jsonOut.open((_foutname + string(".json")).c_str(), fstream::out);
  //
  //  m_jsonOut << "{" << endl;
  //  m_jsonOut << "\"Events\" : [" << endl;

  //  _h2 = new TH2D("h2", "", 100, 0, 100.0, 40, -2, +2);
  //  _h2_b = new TH2D("h2_b", "", 100, 0, 100.0, 40, -2, +2);
  //  _h2_c = new TH2D("h2_c", "", 100, 0, 100.0, 40, -2, +2);
  //  _h2all = new TH2D("h2all", "", 100, 0, 100.0, 40, -2, +2);

  m_hNorm = new TH1F("hNormalization",  //
                     "Normalization;Items;Summed quantity", 10, .5, 10.5);
  int i = 1;
  m_hNorm->GetXaxis()->SetBinLabel(i++, "IntegratedLumi");
  m_hNorm->GetXaxis()->SetBinLabel(i++, "Event");
  m_hNorm->GetXaxis()->LabelsOption("v");

  m_hNChEta = new TH1F("hNChEta",  //
                       "Charged particle #eta distribution;#eta;Count",
                       1000, -5, 5);
  m_hVertexZ = new TH1F("hVertexZ",  //
                        "Vertex z distribution;z [cm];Count",
                        1000, -200, 200);

  m_hitLayerMap = new TH3F("hitLayerMap", "hitLayerMap", 600, -10, 10, 600, -10, 10, 10, -.5, 9.5);
  m_hitLayerMap->SetTitle("hitLayerMap;x [mm];y [mm];Half Layers");

//  m_hitPixelPhiMap = new TH3F("hitPixelPhiMap", "hitPixelPhiMap", 16000, -.5, 16000 - .5, 600, -M_PI, M_PI, 10, -.5, 9.5);
//  m_hitPixelPhiMap->SetTitle("hitPixelPhiMap;PixelPhiIndex in layer;phi [rad];Half Layers Index");
//  m_hitPixelPhiMapHL = new TH3F("hitPixelPhiMapHL", "hitPixelPhiMapHL", 16000, -.5, 16000 - .5, 600, -M_PI, M_PI, 10, -.5, 9.5);
//  m_hitPixelPhiMapHL->SetTitle("hitPixelPhiMap;PixelPhiIndex in half layer;phi [rad];Half Layers Index");
//  m_hitPixelZMap = new TH3F("hitPixelZMap", "hitPixelZMap", 16000, -.5, 16000 - .5, 600, 15, 15, 10, -.5, 9.5);
//  m_hitPixelZMap->SetTitle("hitPixelZMap;hitPixelZMap;z [cm];Half Layers");

  m_hitStaveLayer = new TH2F("hitStaveLayer", "hitStaveLayer", 100, -.5, 100 - .5, 10, -.5, 9.5);
  m_hitStaveLayer->SetTitle("hitStaveLayer;Stave index;Half Layers");
  m_hitModuleHalfStave = new TH2F("hitModuleHalfStave", "hitModuleHalfStave", 100, -.5, 100 - .5, 10, -.5, 9.5);
  m_hitModuleHalfStave->SetTitle("hitModuleHalfStave;Module index;Half Stave");
  m_hitChipModule = new TH2F("hitChipModule", "hitChipModule", 100, -.5, 100 - .5, 10, -.5, 9.5);
  m_hitChipModule->SetTitle("hitChipModule;Chip;Module");

  m_Multiplicity = new TH1F("Multiplicity", "Multiplicity", 10000, -.5, 10000 - .5);
  m_Multiplicity->SetTitle("Multiplicity;Chip Multiplicity;Count");

  m_LayerMultiplicity = new TH2F("LayerMultiplicity", "LayerMultiplicity", 3, -.5, 2.5, 1000, -.5, 1000 - .5);
  m_LayerMultiplicity->SetTitle("LayerMultiplicity;Layer;Chip Multiplicity");

  m_LayerChipMultiplicity = new TH3F("LayerChipMultiplicity", "LayerChipMultiplicity", 3, -.5, 2.5, kNCHip, -.5, kNCHip - .5, 1000, -.5, 1000 - .5);
  m_LayerChipMultiplicity->SetTitle("LayerChipMultiplicity;Layer;Chip");

  return Fun4AllReturnCodes::EVENT_OK;
}

int HFMLTriggerOccupancy::InitRun(PHCompositeNode* topNode)
{
  m_hitsetcont = findNode::getClass<TrkrHitSetContainerv1>(topNode, "TRKR_HITSET");
  if (!m_hitsetcont)
  {
    std::cout << PHWHERE << "ERROR: Can't find node TrkrHitSetContainer" << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }
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
    cout << "HFMLTriggerOccupancy::InitRun - WARNING - missing HFMLTrigger_HepMCTriggerFlags" << endl;
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

int HFMLTriggerOccupancy::process_event(PHCompositeNode* topNode)
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
  //  SvtxClusterEval* clustereval = _svtxevalstack->get_cluster_eval();
  // SvtxHitEval* hiteval = _svtxevalstack->get_hit_eval();
  //    SvtxTruthEval* trutheval = _svtxevalstack->get_truth_eval();

  //  PHHepMCGenEventMap* geneventmap = findNode::getClass<PHHepMCGenEventMap>(topNode, "PHHepMCGenEventMap");
  //  if (!geneventmap)
  //  {
  //    std::cout << PHWHERE << " - Fatal error - missing node PHHepMCGenEventMap" << std::endl;
  //    return Fun4AllReturnCodes::ABORTRUN;
  //  }
  //
  //  PHHepMCGenEvent* genevt = geneventmap->get(_embedding_id);
  //  if (!genevt)
  //  {
  //    std::cout << PHWHERE << " - Fatal error - node PHHepMCGenEventMap missing subevent with embedding ID " << _embedding_id;
  //    std::cout << ". Print PHHepMCGenEventMap:";
  //    geneventmap->identify();
  //    return Fun4AllReturnCodes::ABORTRUN;
  //  }
  //
  //  HepMC::GenEvent* theEvent = genevt->getEvent();
  //  assert(theEvent);
  //  if (Verbosity())
  //  {
  //    cout << "HFMLTriggerOccupancy::process_event - process HepMC::GenEvent with signal_process_id = "
  //         << theEvent->signal_process_id();
  //    if (theEvent->signal_process_vertex())
  //    {
  //      cout << " and signal_process_vertex : ";
  //      theEvent->signal_process_vertex()->print();
  //    }
  //    cout << "  - Event record:" << endl;
  //    theEvent->print();
  //  }

  assert(m_hNorm);
  m_hNorm->Fill("Event", 1.);

  assert(m_truthInfo);
  assert(m_hNChEta);
  assert(m_hVertexZ);

  const PHG4VtxPoint* vtx =
  m_truthInfo->GetPrimaryVtx(m_truthInfo->GetPrimaryVertexIndex());
  if (vtx)
  {
    m_hVertexZ -> Fill(vtx->get_z());
  }

  const auto primary_range =
      m_truthInfo->GetPrimaryParticleRange();
  for (auto particle_iter =
           primary_range.first;
       particle_iter != primary_range.second;
       ++particle_iter)
  {
    const PHG4Particle *p = particle_iter->second;
    assert(p);
    TParticlePDG *pdg_p = TDatabasePDG::Instance()->GetParticle(
        p->get_pid());
    assert(pdg_p);
    if (fabs(pdg_p->Charge()) > 0)
    {
      TVector3 pvec(p->get_px(), p->get_py(), p->get_pz());
      if (pvec.Perp2() > 0)
      {
        assert(m_hNChEta);
        m_hNChEta->Fill(pvec.PseudoRapidity());
      }
    }
  }  //          if (_load_all_particle) else


  // chip counting
  vector<vector<vector<int> > >
      multiplicity_vec(_nlayers_maps);

  //
  for (unsigned int layer = 0; layer < _nlayers_maps; ++layer)
  {
    CylinderGeom_Mvtx* geom = dynamic_cast<CylinderGeom_Mvtx*>(m_Geoms->GetLayerGeom(layer));
    assert(geom);

    const int n_stave = geom->get_N_staves();

    assert(multiplicity_vec[layer].empty());
    multiplicity_vec[layer].resize(n_stave, vector<int>(kNCHip, 0));
  }

  //set<unsigned int> mapsHits;  //internal consistency check for later stages of truth tracks
  assert(m_hitsetcont);

  TrkrHitSetContainer::ConstRange hitset_range = m_hitsetcont->getHitSets(TrkrDefs::TrkrId::mvtxId);
  for (TrkrHitSetContainer::ConstIterator hitset_iter = hitset_range.first;
       hitset_iter != hitset_range.second;
       ++hitset_iter)
  {
    TrkrDefs::hitsetkey hitSetKey = hitset_iter->first;
    TrkrHitSet::ConstRange hit_range = hitset_iter->second->getHits();
    for (TrkrHitSet::ConstIterator hit_iter = hit_range.first;
         hit_iter != hit_range.second;
         ++hit_iter)
    {
      TrkrDefs::hitkey hitKey = hit_iter->first;
      TrkrHit* hit = hit_iter->second;
      assert(hit);
      unsigned int layer = TrkrDefs::getLayer(hitset_iter->first);
      if (layer < _nlayers_maps)
      {
       // unsigned int hitID = hit->get_id();
        //mapsHits.insert(hitID);

        //PHG4Cell* cell = hiteval->get_cell(hit);
        //assert(cell);

        CylinderGeom_Mvtx* geom = dynamic_cast<CylinderGeom_Mvtx*>(m_Geoms->GetLayerGeom(layer));
        assert(geom);
        unsigned int pixel_x = MvtxDefs::getRow(hitKey);
        unsigned int pixel_z = MvtxDefs::getCol(hitKey);
        assert((int)pixel_x < geom->get_NX());
        assert((int)pixel_z < geom->get_NZ());
        unsigned int chip = MvtxDefs::getChipId(hitSetKey);
        unsigned int stave = MvtxDefs::getStaveId(hitSetKey);
        TVector3 local_coords = geom->get_local_coords_from_pixel(pixel_x, pixel_z);
        TVector3 world_coords = geom->get_world_from_local_coords(stave,
                                                                  0,
                                                                  0,
                                                                  chip,
                                                                  local_coords);

        //unsigned int pixel_x(cell->get_pixel_index() % geom->get_NX());
        //unsigned int pixel_z(cell->get_pixel_index() / geom->get_NX());
        //unsigned int halflayer = (int) pixel_x >= geom->get_NX() / 2 ? 0 : 1;

        //unsigned int halfLayerIndex(layer * 2 + halflayer);
//      unsigned int pixelPhiIndex(
//          cell->get_stave_index() * geom->get_NX() + pixel_x);
//      unsigned int pixelPhiIndexHL(
//          cell->get_stave_index() * geom->get_NX() / 2 + pixel_x % (geom->get_NX() / 2));
//      unsigned int pixelZIndex(cell->get_chip_index() * geom->get_NZ() + pixel_z);

      //      //      ptree hitTree;
      //      rapidjson::Value hitTree(rapidjson::kObjectType);
      //
      //      //      ptree hitIDTree;
      //      rapidjson::Value hitIDTree(rapidjson::kObjectType);
      //      hitIDTree.AddMember("HitSequenceInEvent", hitID, alloc);
      //
      //      hitIDTree.AddMember("PixelHalfLayerIndex", halfLayerIndex, alloc);
      //      hitIDTree.AddMember("PixelPhiIndexInLayer", pixelPhiIndex, alloc);
      //      hitIDTree.AddMember("PixelPhiIndexInHalfLayer", pixelPhiIndexHL, alloc);
      //      hitIDTree.AddMember("PixelZIndex", pixelZIndex, alloc);
      //
      //      hitIDTree.AddMember("Layer", layer, alloc);
      //      hitIDTree.AddMember("HalfLayer", halflayer, alloc);
      //      hitIDTree.AddMember("Stave", cell->get_stave_index(), alloc);
      //      //      hitIDTree.put("HalfStave", cell->get_half_stave_index());
      //      //      hitIDTree.put("Module", cell->get_module_index());
      //      hitIDTree.AddMember("Chip", cell->get_chip_index(), alloc);
      //      hitIDTree.AddMember("Pixel", cell->get_pixel_index(), alloc);
      //      hitTree.AddMember("ID", hitIDTree, alloc);
      //
      //      hitTree.AddMember("Coordinate",
      //                        loadCoordinate(world_coords.x(),
      //                                       world_coords.y(),
      //                                       world_coords.z()),
      //                        alloc);

        if (Verbosity() >= 2)
          cout << "HFMLTriggerOccupancy::process_event - MVTX hit "
               << "layer " << layer << "\t"
               << "Stave " << stave << "\t"
               << "Chip " <<  chip  << "\t"
               << "Pixel x " << pixel_x << "\t"
               << "Pixel z " << pixel_z << "\t"
               << "Coordinate"
               << "(" << world_coords.x() << "," << world_coords.y() << "," << world_coords.z() << ")" << endl;

      //      rawHitsTree.add_child("MVTXHit", hitTree);
      //      rawHitsTree.PushBack(hitTree, alloc);

        m_hitStaveLayer->Fill(stave, layer);
        m_hitModuleHalfStave->Fill(stave, stave);
        m_hitChipModule->Fill(chip, stave);

        m_hitLayerMap->Fill(world_coords.x(), world_coords.y(), layer);
//      m_hitPixelPhiMap->Fill(pixelPhiIndex, atan2(world_coords.y(), world_coords.x()), halfLayerIndex);
//      m_hitPixelPhiMapHL->Fill(pixelPhiIndexHL, atan2(world_coords.y(), world_coords.x()), halfLayerIndex);
//      m_hitPixelZMap->Fill(pixelZIndex, world_coords.z(), halfLayerIndex);

        assert(layer < multiplicity_vec.size());
        auto& multiplicity_layer = multiplicity_vec[layer];
        assert(static_cast<size_t>(stave) < multiplicity_layer.size());
        auto& multiplicity_stave = multiplicity_layer[stave];
        assert(static_cast<size_t>(chip) < multiplicity_stave.size());
        multiplicity_stave[chip]++;

      }  //    if (layer < _nlayers_maps)

    }  //  hit_iter loop

  }  // hitset_iter loop
     //  rawHitTree.AddMember("MVTXHits", rawHitsTree, alloc);

  //  m_Multiplicity = new TH1F("Multiplicity", "Multiplicity", 10000, -.5, 10000 - .5);
  //  m_Multiplicity->SetTitle("Multiplicity;Chip Multiplicity;Count");
  //
  //  m_LayerMultiplicity = new TH2F("LayerMultiplicity", "LayerMultiplicity", 3, -.5, 2.5, 1000, -.5, 1000 - .5);
  //  m_LayerMultiplicity->SetTitle("LayerMultiplicity;Layer;Chip Multiplicity");
  //
  //  m_LayerChipMultiplicity = new TH3F("LayerChipMultiplicity", "LayerChipMultiplicity", 3, -.5, 2.5, 9, -.5, 8.5, 1000, -.5, 1000 - .5);
  //  m_LayerChipMultiplicity->SetTitle("LayerChipMultiplicity;Layer;Chip");

  int layer = 0;
  for (auto& multiplicity_layer : multiplicity_vec)
  {
    int stave = 0;
    for (auto& multiplicity_stave : multiplicity_layer)
    {
      int chip = 0;
      for (auto& multiplicity_chip : multiplicity_stave)
      {
        m_Multiplicity->Fill(multiplicity_chip);
        m_LayerMultiplicity->Fill(layer, multiplicity_chip);
        m_LayerChipMultiplicity->Fill(layer, chip, multiplicity_chip);

        chip++;
      }

      if (Verbosity() >= 2)
        cout << "HFMLTriggerOccupancy::process_event - fill layer " << layer << " stave. " << stave << "Accumulated chips = "
             << m_Multiplicity->GetSum()
             << endl;

      stave++;
    }
    layer++;
  }

  ++_ievent;

  return Fun4AllReturnCodes::EVENT_OK;
}

int HFMLTriggerOccupancy::End(PHCompositeNode* topNode)
{
  if (_f)
  {
    _f->cd();
    _f->Write();
    //_f->Close();

    //    m_hitLayerMap->Write();
  }

  //  if (m_jsonOut.is_open())
  //  {
  //    m_jsonOut << "]" << endl;
  //    m_jsonOut << "}" << endl;
  //
  //    m_jsonOut.close();
  //  }

  cout << "HFMLTriggerOccupancy::End - output to " << _foutname << ".*" << endl;

  return Fun4AllReturnCodes::EVENT_OK;
}
