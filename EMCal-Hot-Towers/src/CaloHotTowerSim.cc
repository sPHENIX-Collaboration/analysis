//____________________________________________________________________________..
//
// This is a template for a Fun4All SubsysReco module with all methods from the
// $OFFLINE_MAIN/include/fun4all/SubsysReco.h baseclass
// You do not have to implement all of them, you can just remove unused methods
// here and in CaloHotTowerSim.h.
//
// CaloHotTowerSim(const std::string &name = "CaloHotTowerSim")
// everything is keyed to CaloHotTowerSim, duplicate names do work but it makes
// e.g. finding culprits in logs difficult or getting a pointer to the module
// from the command line
//
// CaloHotTowerSim::~CaloHotTowerSim()
// this is called when the Fun4AllServer is deleted at the end of running. Be
// mindful what you delete - you do loose ownership of object you put on the node tree
//
// int CaloHotTowerSim::Init(PHCompositeNode *topNode)
// This method is called when the module is registered with the Fun4AllServer. You
// can create historgrams here or put objects on the node tree but be aware that
// modules which haven't been registered yet did not put antyhing on the node tree
//
// int CaloHotTowerSim::InitRun(PHCompositeNode *topNode)
// This method is called when the first event is read (or generated). At
// this point the run number is known (which is mainly interesting for raw data
// processing). Also all objects are on the node tree in case your module's action
// depends on what else is around. Last chance to put nodes under the DST Node
// We mix events during readback if branches are added after the first event
//
// int CaloHotTowerSim::process_event(PHCompositeNode *topNode)
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
// int CaloHotTowerSim::ResetEvent(PHCompositeNode *topNode)
// If you have internal data structures (arrays, stl containers) which needs clearing
// after each event, this is the place to do that. The nodes under the DST node are cleared
// by the framework
//
// int CaloHotTowerSim::EndRun(const int runnumber)
// This method is called at the end of a run when an event from a new run is
// encountered. Useful when analyzing multiple runs (raw data). Also called at
// the end of processing (before the End() method)
//
// int CaloHotTowerSim::End(PHCompositeNode *topNode)
// This is called at the end of processing. It needs to be called by the macro
// by Fun4AllServer::End(), so do not forget this in your macro
//
// int CaloHotTowerSim::Reset(PHCompositeNode *topNode)
// not really used - it is called before the dtor is called
//
// void CaloHotTowerSim::Print(const std::string &what) const
// Called from the command line - useful to print information when you need it
//
//____________________________________________________________________________..

#include "CaloHotTowerSim.h"

#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/Fun4AllServer.h>

#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>
#include <phool/phool.h>

// Tower stuff
#include <calobase/TowerInfo.h>
#include <calobase/TowerInfoDefs.h>
#include <calobase/TowerInfoContainer.h>

// ROOT stuff
#include <TFile.h>

// c++
#include <iostream>
#include <fstream>

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::vector;
using std::pair;
using std::min;
using std::max;
using std::to_string;

//____________________________________________________________________________..
CaloHotTowerSim::CaloHotTowerSim(const string &name):
 SubsysReco(name),
 m_iEvent(0),
 m_bins_events(1),
 m_bins_phi(256),
 m_bins_eta(96),
 m_min_towerE(0.1), /*GeV*/
 m_energy_min(9999), /*GeV*/
 m_energy_max(0), /*GeV*/
 m_avgBadTowers(0),
 m_emcTowerNode("TOWERINFO_CALIB_CEMC"),
 m_outputFile("test.root")
{
  cout << "CaloHotTowerSim::CaloHotTowerSim(const string &name) Calling ctor" << endl;
}

//____________________________________________________________________________..
CaloHotTowerSim::~CaloHotTowerSim() {
  cout << "CaloHotTowerSim::~CaloHotTowerSim() Calling dtor" << endl;
}

//____________________________________________________________________________..
Int_t CaloHotTowerSim::Init(PHCompositeNode *topNode) {
  cout << "CaloHotTowerSim::Init(PHCompositeNode *topNode) Initializing" << endl;

  // so that the histos actually get written out
  Fun4AllServer *se = Fun4AllServer::instance();
  se->Print("NODETREE");

  hEvents = new TH1F("hEvents","Events; Status; Counts", m_bins_events, 0, m_bins_events);

  h2TowerEnergy = new TH2F("h2TowerEnergy","Tower Energy; Tower Index #phi; Tower Index #eta", m_bins_phi, -0.5, m_bins_phi-0.5, m_bins_eta, -0.5, m_bins_eta-0.5);

  h2TowerControlEnergy = new TH2F("h2TowerControlEnergy","Tower Energy; Tower Index #phi; Tower Index #eta", m_bins_phi, -0.5, m_bins_phi-0.5, m_bins_eta, -0.5, m_bins_eta-0.5);

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
Int_t CaloHotTowerSim::process_event(PHCompositeNode *topNode) {

  if(m_iEvent%20 == 0) cout << "Progress: " << m_iEvent << endl;
  ++m_iEvent;

  // Get TowerInfoContainer
  TowerInfoContainer* towers = findNode::getClass<TowerInfoContainer>(topNode, m_emcTowerNode.c_str());
  if (!towers) {
    cout << PHWHERE << "CaloHotTowerSim::process_event Could not find node " << m_emcTowerNode << endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  hEvents->Fill(0);

  for (UInt_t i = 0; i < towers->size(); ++i)
  {
    UInt_t key = TowerInfoDefs::encode_emcal(i);
    UInt_t iphi = TowerInfoDefs::getCaloTowerPhiBin(key);
    UInt_t ieta = TowerInfoDefs::getCaloTowerEtaBin(key);

    TowerInfo *tower = towers->get_tower_at_channel(i);

    Float_t energy = tower->get_energy();

    if (tower->get_isHot()) ++m_avgBadTowers;

    if (energy >= m_min_towerE)
    {
      h2TowerControlEnergy->Fill(iphi, ieta, energy);

      if (!tower->get_isHot())
      {
        h2TowerEnergy->Fill(iphi, ieta, energy);
      }

      m_energy_min = min(m_energy_min, energy);
      m_energy_max = max(m_energy_max, energy);
    }
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
Int_t CaloHotTowerSim::End(PHCompositeNode *topNode) {
  cout << "CaloHotTowerSim::End(PHCompositeNode *topNode) This is the End..." << endl;
  cout << "Min Energy: " << m_energy_min << ", Max Energy: " << m_energy_max << endl;
  cout << "Avg Bad Towers: " << m_avgBadTowers/hEvents->GetBinContent(1) << endl;

  TFile output(m_outputFile.c_str(),"recreate");

  output.cd();
  hEvents->Write();
  h2TowerEnergy->Write();
  h2TowerControlEnergy->Write();

  output.Close();

  return Fun4AllReturnCodes::EVENT_OK;
}
