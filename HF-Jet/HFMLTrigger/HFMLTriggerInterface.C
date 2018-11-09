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
#include <TH2D.h>
#include <TLorentzVector.h>
#include <TString.h>
#include <TTree.h>

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
  , _svtxevalstack(nullptr)
  , m_hitMap(nullptr)
  , m_cellGeoms(nullptr)
{
  _foutname = filename;
}

int HFMLTriggerInterface::Init(PHCompositeNode* topNode)
{
  _ievent = 0;

  _f = new TFile((_foutname + string(".root")).c_str(), "RECREATE");

  m_jsonOut.open((_foutname + string(".json")).c_str(), fstream::out);

  //  _h2 = new TH2D("h2", "", 100, 0, 100.0, 40, -2, +2);
  //  _h2_b = new TH2D("h2_b", "", 100, 0, 100.0, 40, -2, +2);
  //  _h2_c = new TH2D("h2_c", "", 100, 0, 100.0, 40, -2, +2);
  //  _h2all = new TH2D("h2all", "", 100, 0, 100.0, 40, -2, +2);

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
  m_cellGeoms =
      findNode::getClass<PHG4CylinderCellGeomContainer>(topNode, "CYLINDERCELLGEOM_SVTX");
  if (!m_cellGeoms)
  {
    std::cout << PHWHERE << "ERROR: Can't find node CYLINDERCELLGEOM_SVTX" << std::endl;
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
  //  SvtxClusterEval* clustereval = _svtxevalstack->get_cluster_eval();
  //  SvtxHitEval* hiteval = _svtxevalstack->get_hit_eval();
  //  SvtxTruthEval* trutheval = _svtxevalstack->get_truth_eval();

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
    theEvent->print();

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

  ptree truthTriggerFlagTree;
  metaTree.put("Description", "These are categorical true/false MonteCalo truth tags for the event. These are only known in training sample. This would be trigger output in real data processing.");
  metaTree.put("Signal1", true);
  metaTree.put("Signal2", false);

  ptree rawHitTree;
  rawHitTree.put("Description",
                 "Raw data in the event in an unordered set of hit ID. To help in visualization stage, the coordinate of the hit is also appended. These would be input in real data.");

  assert(m_hitMap);
  for (SvtxHitMap::Iter iter = m_hitMap->begin();
       iter != m_hitMap->end();
       ++iter)
  {
  }

  ptree truthHitTree;
  rawHitTree.put("Description", "From the MonteCalo truth information, pairs of track ID and subset of RawHit that belong to the track. These are not presented in real data. The track ID is arbitary.");

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
  }

  if (m_jsonOut.is_open())
    m_jsonOut.close();

  cout << "HFMLTriggerInterface::End - output to " << _foutname << ".*" << endl;

  return Fun4AllReturnCodes::EVENT_OK;
}
