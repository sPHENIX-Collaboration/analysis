//____________________________________________________________________________..
//
// This is a template for a Fun4All SubsysReco module with all methods from the
// $OFFLINE_MAIN/include/fun4all/SubsysReco.h baseclass
// You do not have to implement all of them, you can just remove unused methods
// here and in CaloTower.h.
//
// CaloTower(const std::string &name = "CaloTower")
// everything is keyed to CaloTower, duplicate names do work but it makes
// e.g. finding culprits in logs difficult or getting a pointer to the module
// from the command line
//
// CaloTower::~CaloTower()
// this is called when the Fun4AllServer is deleted at the end of running. Be
// mindful what you delete - you do loose ownership of object you put on the node tree
//
// int CaloTower::Init(PHCompositeNode *topNode)
// This method is called when the module is registered with the Fun4AllServer. You
// can create historgrams here or put objects on the node tree but be aware that
// modules which haven't been registered yet did not put antyhing on the node tree
//
// int CaloTower::InitRun(PHCompositeNode *topNode)
// This method is called when the first event is read (or generated). At
// this point the run number is known (which is mainly interesting for raw data
// processing). Also all objects are on the node tree in case your module's action
// depends on what else is around. Last chance to put nodes under the DST Node
// We mix events during readback if branches are added after the first event
//
// int CaloTower::process_event(PHCompositeNode *topNode)
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
// int CaloTower::ResetEvent(PHCompositeNode *topNode)
// If you have internal data structures (arrays, stl containers) which needs clearing
// after each event, this is the place to do that. The nodes under the DST node are cleared
// by the framework
//
// int CaloTower::EndRun(const int runnumber)
// This method is called at the end of a run when an event from a new run is
// encountered. Useful when analyzing multiple runs (raw data). Also called at
// the end of processing (before the End() method)
//
// int CaloTower::End(PHCompositeNode *topNode)
// This is called at the end of processing. It needs to be called by the macro
// by Fun4AllServer::End(), so do not forget this in your macro
//
// int CaloTower::Reset(PHCompositeNode *topNode)
// not really used - it is called before the dtor is called
//
// void CaloTower::Print(const std::string &what) const
// Called from the command line - useful to print information when you need it
//
//____________________________________________________________________________..

#include "CaloTower.h"

#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/Fun4AllServer.h>

// Tower stuff
#include <calobase/TowerInfo.h>
#include <calobase/TowerInfoDefs.h>
#include <calobase/TowerInfoContainer.h>

#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

// ROOT stuff
#include <TFile.h>
#include <TH2F.h>

// c++
#include <iostream>
#include <fstream>
#include <vector>

using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::pair;
using std::min;
using std::max;
using std::to_string;

//____________________________________________________________________________..
CaloTower::CaloTower(const string &name):
 SubsysReco(name)
 , m_emcTowerNode("TOWERS_CEMC")
 , m_Event(0)
 , m_nphi(256)
 , m_neta(96)
 , m_min_energy(9999)
 , m_max_energy(0)
{
  cout << "CaloTower::CaloTower(const string &name) Calling ctor" << endl;
}

//____________________________________________________________________________..
CaloTower::~CaloTower()
{
  cout << "CaloTower::~CaloTower() Calling dtor" << endl;
}

//____________________________________________________________________________..
int CaloTower::Init(PHCompositeNode *topNode)
{
  cout << "CaloTower::Init(PHCompositeNode *topNode) Initializing" << endl;
  Fun4AllServer *se = Fun4AllServer::instance();
  se->Print("NODETREE");

  m_hists["h2CEMC"] = new TH2F("h2CEMC","EMCal [ADC]; Tower Index #phi; Tower Index #eta", m_nphi, -0.5, m_nphi-0.5, m_neta, -0.5, m_neta-0.5);
  m_hists["hEvent"] = new TH1F("hEvent","Events; Status; Counts", 1, 0, 1);

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int CaloTower::process_event(PHCompositeNode *topNode)
{
  if(m_Event % 5 == 0) {
    cout << "Event: " << m_Event << endl;
  }
  ++m_Event;

  // Get TowerInfoContainer
  TowerInfoContainer* towers = findNode::getClass<TowerInfoContainer>(topNode, m_emcTowerNode.c_str());
  if (!towers) {
    cout << PHWHERE << "CaloHotTower::process_event Could not find node " << m_emcTowerNode << endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  // loop over CEMC towers
  for(UInt_t towerIndex = 0; towerIndex < towers->size(); ++towerIndex) {
    UInt_t key  = TowerInfoDefs::encode_emcal(towerIndex);
    UInt_t iphi = TowerInfoDefs::getCaloTowerPhiBin(key);
    UInt_t ieta = TowerInfoDefs::getCaloTowerEtaBin(key);

    TowerInfo* tower = towers->get_tower_at_channel(towerIndex);
    Float_t energy = (tower->get_isGood()) ? tower->get_energy() : 0;
    // Float_t energy = tower->get_energy();
    m_min_energy = min(m_min_energy, energy);
    m_max_energy = max(m_max_energy, energy);

    ((TH2*)m_hists["h2CEMC"])->Fill(iphi, ieta, energy);
  }

  m_hists["hEvent"]->Fill(0);

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int CaloTower::End(PHCompositeNode *topNode)
{
  cout << "CaloTower::End(PHCompositeNode *topNode) This is the End..." << endl;
  cout << "Energy: Min: " << m_min_energy << ", Max: " << m_max_energy << endl;
  cout << "Processed Events: " << m_Event << endl;

  // scale the hists to get the average tower energy
  TFile output(m_outputFile.c_str(),"recreate");

  output.cd();

  for (const auto &pair : m_hists) {
    pair.second->Write();
  }

  output.Close();
  return Fun4AllReturnCodes::EVENT_OK;
}
