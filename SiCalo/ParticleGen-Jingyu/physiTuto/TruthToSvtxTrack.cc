//____________________________________________________________________________..
//
// This is a template for a Fun4All SubsysReco module with all methods from the
// $OFFLINE_MAIN/include/fun4all/SubsysReco.h baseclass
// You do not have to implement all of them, you can just remove unused methods
// here and in TruthToSvtxTrack.h.
//
// TruthToSvtxTrack(const std::string &name = "TruthToSvtxTrack")
// everything is keyed to TruthToSvtxTrack, duplicate names do work but it makes
// e.g. finding culprits in logs difficult or getting a pointer to the module
// from the command line
//
// TruthToSvtxTrack::~TruthToSvtxTrack()
// this is called when the Fun4AllServer is deleted at the end of running. Be
// mindful what you delete - you do loose ownership of object you put on the node tree
//
// int TruthToSvtxTrack::Init(PHCompositeNode *topNode)
// This method is called when the module is registered with the Fun4AllServer. You
// can create historgrams here or put objects on the node tree but be aware that
// modules which haven't been registered yet did not put antyhing on the node tree
//
// int TruthToSvtxTrack::InitRun(PHCompositeNode *topNode)
// This method is called when the first event is read (or generated). At
// this point the run number is known (which is mainly interesting for raw data
// processing). Also all objects are on the node tree in case your module's action
// depends on what else is around. Last chance to put nodes under the DST Node
// We mix events during readback if branches are added after the first event
//
// int TruthToSvtxTrack::process_event(PHCompositeNode *topNode)
// called for every event. Return codes trigger actions, you find them in
// $OFFLINE_MAIN/include/fun4all/Fun4AllReturnCodes.h
//   everything is good:
//     return Fun4AllReturnCodes::EVENT_OK
//   abort event reconstruction, clear everything and process next event:
//     return Fun4AllReturnCodes::ABORT_EVENT; 
//   proceed but do not save this event in output (needs output manager setting):
//     return Fun4AllReturnCodes::DISCARD_EVENT; 
//   abort processing:
//     return Fun4AllReturnCodes::ABORT_RUN
// all other integers will lead to an error and abort of processing
//
// int TruthToSvtxTrack::ResetEvent(PHCompositeNode *topNode)
// If you have internal data structures (arrays, stl containers) which needs clearing
// after each event, this is the place to do that. The nodes under the DST node are cleared
// by the framework
//
// int TruthToSvtxTrack::EndRun(const int runnumber)
// This method is called at the end of a run when an event from a new run is
// encountered. Useful when analyzing multiple runs (raw data). Also called at
// the end of processing (before the End() method)
//
// int TruthToSvtxTrack::End(PHCompositeNode *topNode)
// This is called at the end of processing. It needs to be called by the macro
// by Fun4AllServer::End(), so do not forget this in your macro
//
// int TruthToSvtxTrack::Reset(PHCompositeNode *topNode)
// not really used - it is called before the dtor is called
//
// void TruthToSvtxTrack::Print(const std::string &what) const
// Called from the command line - useful to print information when you need it
//
//____________________________________________________________________________..

#include "TruthToSvtxTrack.h"

#include <globalvertex/GlobalVertexMap.h>
#include <globalvertex/SvtxVertexMap.h>
#include <ffarawobjects/Gl1Packet.h>
#include <trackbase_historic/SvtxPHG4ParticleMap_v1.h>
#include <trackbase_historic/SvtxTrack.h>
#include <trackbase_historic/SvtxTrack_v1.h>
#include <trackbase_historic/SvtxTrack_v2.h>
#include <trackbase_historic/SvtxTrackMap.h>
#include <trackbase_historic/SvtxTrackMap_v1.h>
#include <trackbase_historic/SvtxTrackState_v1.h>
#include <trackbase_historic/TrackSeed.h>

#include <calobase/RawClusterContainer.h>
#include <calobase/RawTowerGeomContainer.h>
#include <calobase/RawCluster.h>
#include <calobase/RawClusterUtility.h>
#include <calobase/RawTowerDefs.h>
#include <calobase/RawTowerGeom.h>
#include <calobase/RawTowerGeomv5.h>
#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfo.h>
#include <calobase/TowerInfoDefs.h>

#include <ffaobjects/EventHeaderv1.h>

#include <fun4all/Fun4AllReturnCodes.h>

#include <globalvertex/GlobalVertex.h>
#include <globalvertex/GlobalVertexMap.h>
#include <globalvertex/MbdVertexMap.h>
#include <globalvertex/MbdVertex.h>
#include <globalvertex/SvtxVertexMap.h>
#include <globalvertex/SvtxVertex.h>

#include <phool/getClass.h>
#include <phool/PHCompositeNode.h>

#include <Acts/Geometry/GeometryIdentifier.hpp>
#include <Acts/MagneticField/ConstantBField.hpp>
#include <Acts/MagneticField/MagneticFieldProvider.hpp>
#include <Acts/Surfaces/CylinderSurface.hpp>
#include <Acts/Surfaces/PerigeeSurface.hpp>
#include <Acts/Geometry/TrackingGeometry.hpp>

#include <CLHEP/Vector/ThreeVector.h>
#include <math.h>
#include <vector>

#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TLorentzVector.h>

//____________________________________________________________________________..
TruthToSvtxTrack::TruthToSvtxTrack(
    const std::string & name_in):
    SubsysReco(name_in)
{
  std::cout << "TruthToSvtxTrack::TruthToSvtxTrack(const std::string &name) Calling ctor" << std::endl;
}

//____________________________________________________________________________..
TruthToSvtxTrack::~TruthToSvtxTrack()
{
  std::cout << "TruthToSvtxTrack::~TruthToSvtxTrack() Calling dtor" << std::endl;
}

//____________________________________________________________________________..
int TruthToSvtxTrack::Init(PHCompositeNode *topNode)
{
    std::cout << topNode << std::endl;
    std::cout << "TruthToSvtxTrack::Init(PHCompositeNode *topNode) Initializing" << std::endl;

    return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int TruthToSvtxTrack::InitRun(PHCompositeNode * /*topNode*/)
{
  std::cout << "TruthToSvtxTrack::InitRun(PHCompositeNode *topNode) Initializing for Run XXX" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int TruthToSvtxTrack::process_event(PHCompositeNode * topNode) {
    createTracksFromTruth(topNode);

    return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int TruthToSvtxTrack::ResetEvent(PHCompositeNode *topNode)
{
    std::cout << "TruthToSvtxTrack::ResetEvent(PHCompositeNode *topNode) Resetting internal structures, prepare for next event" << std::endl;

    return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int TruthToSvtxTrack::EndRun(const int runnumber)
{
    std::cout << "TruthToSvtxTrack::EndRun(const int runnumber) Ending Run for Run " << runnumber << std::endl;

    return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int TruthToSvtxTrack::End(PHCompositeNode *topNode)
{
  std::cout << "TruthToSvtxTrack::End(PHCompositeNode *topNode) This is the End..." << std::endl;

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int TruthToSvtxTrack::Reset(PHCompositeNode *topNode)
{
 std::cout << "TruthToSvtxTrack::Reset(PHCompositeNode *topNode) being Reset" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
void TruthToSvtxTrack::Print(const std::string &what) const
{
  std::cout << "TruthToSvtxTrack::Print(const std::string &what) const Printing info for " << what << std::endl;
}

int TruthToSvtxTrack::createTracksFromTruth(PHCompositeNode* topNode)
{
    m_truth_info = findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");   

    if (!m_truth_info)
    {
        std::cout << PHWHERE << "Error, can't find G4TruthInfo" << std::endl;
        exit(1);
    }

    // 获取或创建 SvtxTrackMap 节点
    SvtxTrackMap* trackmap = findNode::getClass<SvtxTrackMap>(topNode, "SvtxTrackMap"); // "TruthTrackMap"
    if (!trackmap)
    {
        trackmap = new SvtxTrackMap_v1();
        PHNodeIterator iter(topNode);
        PHCompositeNode* dstNode = dynamic_cast<PHCompositeNode*>(iter.findFirst("PHCompositeNode", "DST"));

        PHIODataNode<PHObject> *truthtracknode = new PHIODataNode<PHObject>(trackmap, "SvtxTrackMap", "PHObject");
        dstNode->addNode(truthtracknode);
    }

    const auto prange = m_truth_info->GetPrimaryParticleRange();
    for (auto iter = prange.first; iter != prange.second; ++iter)
    {
        PHG4Particle* ptcl = iter->second;
        if (!ptcl) continue;

        TLorentzVector p;
        p.SetPxPyPzE(ptcl->get_px(), ptcl->get_py(), ptcl->get_pz(), ptcl->get_e());

        SvtxTrack_v2* track = new SvtxTrack_v2();
        track->set_id(trackmap->size());
        track->set_px(ptcl->get_px());
        track->set_py(ptcl->get_py());
        track->set_pz(ptcl->get_pz());
        track->set_charge(static_cast<int>(TDatabasePDG::Instance()->GetParticle(ptcl->get_pid())->Charge()));
        track->set_chisq(1.0);
        track->set_ndf(1);
        track->set_vertex_id(0);  // 可改为对应 vtx id

        trackmap->insert(track);
    }

    return Fun4AllReturnCodes::EVENT_OK;
}

