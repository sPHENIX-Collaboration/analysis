//____________________________________________________________________________..
//
// This is a template for a Fun4All SubsysReco module with all methods from the
// $OFFLINE_MAIN/include/fun4all/SubsysReco.h baseclass
// You do not have to implement all of them, you can just remove unused methods
// here and in CaloCheck.h.
//
// CaloCheck(const std::string &name = "CaloCheck")
// everything is keyed to CaloCheck, duplicate names do work but it makes
// e.g. finding culprits in logs difficult or getting a pointer to the module
// from the command line
//
// CaloCheck::~CaloCheck()
// this is called when the Fun4AllServer is deleted at the end of running. Be
// mindful what you delete - you do loose ownership of object you put on the node tree
//
// int CaloCheck::Init(PHCompositeNode *topNode)
// This method is called when the module is registered with the Fun4AllServer. You
// can create historgrams here or put objects on the node tree but be aware that
// modules which haven't been registered yet did not put antyhing on the node tree
//
// int CaloCheck::InitRun(PHCompositeNode *topNode)
// This method is called when the first event is read (or generated). At
// this point the run number is known (which is mainly interesting for raw data
// processing). Also all objects are on the node tree in case your module's action
// depends on what else is around. Last chance to put nodes under the DST Node
// We mix events during readback if branches are added after the first event
//
// int CaloCheck::process_event(PHCompositeNode *topNode)
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
// int CaloCheck::ResetEvent(PHCompositeNode *topNode)
// If you have internal data structures (arrays, stl containers) which needs clearing
// after each event, this is the place to do that. The nodes under the DST node are cleared
// by the framework
//
// int CaloCheck::EndRun(const int runnumber)
// This method is called at the end of a run when an event from a new run is
// encountered. Useful when analyzing multiple runs (raw data). Also called at
// the end of processing (before the End() method)
//
// int CaloCheck::End(PHCompositeNode *topNode)
// This is called at the end of processing. It needs to be called by the macro
// by Fun4AllServer::End(), so do not forget this in your macro
//
// int CaloCheck::Reset(PHCompositeNode *topNode)
// not really used - it is called before the dtor is called
//
// void CaloCheck::Print(const std::string &what) const
// Called from the command line - useful to print information when you need it
//
//____________________________________________________________________________..

#include "CaloCheck.h"

#include <fun4all/Fun4AllReturnCodes.h>

// -- Event
#include <ffaobjects/EventHeader.h>

// -- Vtx
#include <globalvertex/GlobalVertex.h>
#include <globalvertex/GlobalVertexMap.h>

// -- Calo
#include <calobase/TowerInfoDefs.h>
#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfo.h>

#include <phool/PHCompositeNode.h>
#include <phool/PHNodeIterator.h>
#include <phool/getClass.h>

// c++
#include <format>

//____________________________________________________________________________..
CaloCheck::CaloCheck(const std::string& name)
  : SubsysReco(name)
{
  std::cout << "CaloCheck::CaloCheck(const std::string &name) Calling ctor" << std::endl;
}

//____________________________________________________________________________..
int CaloCheck::Init([[maybe_unused]] PHCompositeNode* topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int CaloCheck::process_event([[maybe_unused]] PHCompositeNode* topNode)
{
  // Event
  auto* eventInfo = findNode::getClass<EventHeader>(topNode, "EventHeader");
  if (!eventInfo)
  {
    return Fun4AllReturnCodes::ABORTRUN;
  }

  int globalEvent = eventInfo->get_EvtSequence();

  std::cout << std::format("Event: {}\n", globalEvent);

  // Z Vertex
  auto* vertexmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");

  if (!vertexmap)
  {
    return Fun4AllReturnCodes::ABORTRUN;
  }

  double zvtx = -9999;

  if (!vertexmap->empty())
  {
    GlobalVertex *vtx = vertexmap->begin()->second;
    zvtx = vtx->get_z();
  }

  std::cout << std::format("Z Vertex: {:.2f} cm\n", zvtx);

  auto* towersCEMC  = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_CEMC");
  auto* towersIHCal = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALIN");
  auto* towersOHCal = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALOUT");

  if (!towersCEMC)
  {
    return Fun4AllReturnCodes::ABORTRUN;
  }

  std::cout << std::format("{:#<20}\n", "");
  std::cout << std::format("EMCal: Total Towers {}\n", towersCEMC->size());
  for(unsigned int towerIndex = 0; towerIndex < towersCEMC->size(); ++towerIndex) {
    unsigned int key = TowerInfoDefs::encode_emcal(towerIndex);
    unsigned int iphi = TowerInfoDefs::getCaloTowerPhiBin(key);
    unsigned int ieta = TowerInfoDefs::getCaloTowerEtaBin(key);

    auto* tower = towersCEMC->get_tower_at_channel(towerIndex);
    double energy =  tower->get_energy();

    std::cout << std::format("Tower: {}: ({},{}), Energy: {:.6f} GeV\n", towerIndex, iphi, ieta, energy);
  }

  std::cout << std::format("{:#<20}\n", "");
  std::cout << std::format("IHCal: Total Towers {}\n", towersIHCal->size());
  for(unsigned int towerIndex = 0; towerIndex < towersIHCal->size(); ++towerIndex) {
    unsigned int key = TowerInfoDefs::encode_emcal(towerIndex);
    unsigned int iphi = TowerInfoDefs::getCaloTowerPhiBin(key);
    unsigned int ieta = TowerInfoDefs::getCaloTowerEtaBin(key);

    auto* tower = towersIHCal->get_tower_at_channel(towerIndex);
    double energy =  tower->get_energy();
    std::cout << std::format("Tower: {}: ({},{}), Energy: {:.6f} GeV\n", towerIndex, iphi, ieta, energy);
  }

  std::cout << std::format("{:#<20}\n", "");
  std::cout << std::format("OHCal: Total Towers {}\n", towersOHCal->size());
  for(unsigned int towerIndex = 0; towerIndex < towersOHCal->size(); ++towerIndex) {
    unsigned int key = TowerInfoDefs::encode_emcal(towerIndex);
    unsigned int iphi = TowerInfoDefs::getCaloTowerPhiBin(key);
    unsigned int ieta = TowerInfoDefs::getCaloTowerEtaBin(key);

    auto* tower = towersOHCal->get_tower_at_channel(towerIndex);
    double energy =  tower->get_energy();
    std::cout << std::format("Tower: {}: ({},{}), Energy: {:.6f} GeV\n", towerIndex, iphi, ieta, energy);
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int CaloCheck::ResetEvent([[maybe_unused]] PHCompositeNode* topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int CaloCheck::End([[maybe_unused]] PHCompositeNode* topNode)
{
  std::cout << "CaloCheck::End(PHCompositeNode *topNode) This is the End..." << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}
