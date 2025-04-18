//____________________________________________________________________________..
//
// This is a template for a Fun4All SubsysReco module with all methods from the
// $OFFLINE_MAIN/include/fun4all/SubsysReco.h baseclass
// You do not have to implement all of them, you can just remove unused methods
// here and in CaloHotTower.h.
//
// CaloHotTower(const std::string &name = "CaloHotTower")
// everything is keyed to CaloHotTower, duplicate names do work but it makes
// e.g. finding culprits in logs difficult or getting a pointer to the module
// from the command line
//
// CaloHotTower::~CaloHotTower()
// this is called when the Fun4AllServer is deleted at the end of running. Be
// mindful what you delete - you do loose ownership of object you put on the node tree
//
// int CaloHotTower::Init(PHCompositeNode *topNode)
// This method is called when the module is registered with the Fun4AllServer. You
// can create historgrams here or put objects on the node tree but be aware that
// modules which haven't been registered yet did not put antyhing on the node tree
//
// int CaloHotTower::InitRun(PHCompositeNode *topNode)
// This method is called when the first event is read (or generated). At
// this point the run number is known (which is mainly interesting for raw data
// processing). Also all objects are on the node tree in case your module's action
// depends on what else is around. Last chance to put nodes under the DST Node
// We mix events during readback if branches are added after the first event
//
// int CaloHotTower::process_event(PHCompositeNode *topNode)
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
// int CaloHotTower::ResetEvent(PHCompositeNode *topNode)
// If you have internal data structures (arrays, stl containers) which needs clearing
// after each event, this is the place to do that. The nodes under the DST node are cleared
// by the framework
//
// int CaloHotTower::EndRun(const int runnumber)
// This method is called at the end of a run when an event from a new run is
// encountered. Useful when analyzing multiple runs (raw data). Also called at
// the end of processing (before the End() method)
//
// int CaloHotTower::End(PHCompositeNode *topNode)
// This is called at the end of processing. It needs to be called by the macro
// by Fun4AllServer::End(), so do not forget this in your macro
//
// int CaloHotTower::Reset(PHCompositeNode *topNode)
// not really used - it is called before the dtor is called
//
// void CaloHotTower::Print(const std::string &what) const
// Called from the command line - useful to print information when you need it
//
//____________________________________________________________________________..

#include "CaloHotTower.h"

#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/Fun4AllServer.h>

#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>
#include <phool/phool.h>

// Tower stuff
#include <calobase/TowerInfo.h>
#include <calobase/TowerInfoDefs.h>
#include <calobase/TowerInfoContainer.h>

// CDB
#include <cdbobjects/CDBTTree.h>
#include <ffamodules/CDBInterface.h>

// ROOT stuff
#include <TFile.h>
#include <TH1.h>

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
CaloHotTower::CaloHotTower(const string &name):
 SubsysReco(name),
 iEvent(0),
 energy_min(9999),
 energy_max(0),
 bins_energy(3280),
 energy_low(0),
 energy_high(16400), // 2^14 is the max value
 bins_events(m_triggers.size()),
 m_emcTowerNode("TOWERS_CEMC"),
 m_outputFile("test.root"),
 m_calibName_hotMap("CEMC_BadTowerMap"),
 triggeranalyzer(nullptr)
{
  cout << "CaloHotTower::CaloHotTower(const string &name) Calling ctor" << endl;
}

//____________________________________________________________________________..
CaloHotTower::~CaloHotTower() {
  cout << "CaloHotTower::~CaloHotTower() Calling dtor" << endl;
}

Int_t CaloHotTower::readHotTowerIndexFile() {

  cout << "Reading Hot and Ref tower indices" << endl;

  std::ifstream file(m_hotTowerIndexFile);

  // Check if the file was successfully opened
  if (!file.is_open()) {
      cerr << "Failed to open file: " << m_hotTowerIndexFile << endl;
      return 1;
  }

  string line;

  // skip header
  std::getline(file, line);

  // loop over each run
  while (std::getline(file, line)) {
      std::istringstream lineStream(line);

      UInt_t towerHotIndex;
      UInt_t towerRefIndex;
      Char_t comma;

      if (lineStream >> towerHotIndex >> comma >> towerRefIndex) {
          hotTowerIndex.push_back(std::make_pair(towerHotIndex, towerRefIndex));
          cout << "Hot: " << towerHotIndex << ", Ref: " << towerRefIndex << endl;
      }
      else {
          cerr << "Failed to parse line: " << line << endl;
          return 1;
      }
  }

  // Close the file
  file.close();

  return 0;
}

//____________________________________________________________________________..
Int_t CaloHotTower::Init(PHCompositeNode *topNode) {
  cout << "CaloHotTower::Init(PHCompositeNode *topNode) Initializing" << endl;

  // read the hot tower and reference tower indices
  Int_t ret = readHotTowerIndexFile();

  if(ret) return Fun4AllReturnCodes::ABORTRUN;

  // so that the histos actually get written out
  Fun4AllServer *se = Fun4AllServer::instance();
  se->Print("NODETREE");

  hEvents = new TH1F("hEvents","Events; Status; Counts", bins_events, 0, bins_events);
  for(UInt_t i = 1; i <= bins_events; ++i) {
    hEvents->GetXaxis()->SetBinLabel(i, m_triggers[i-1].c_str());
  }

  triggeranalyzer = new TriggerAnalyzer();

  UInt_t i = 0;
  // initialize histograms
  for(auto idx : hotTowerIndex) {

    UInt_t key    = TowerInfoDefs::encode_emcal(idx.first);
    UInt_t etabin = TowerInfoDefs::getCaloTowerEtaBin(key);
    UInt_t phibin = TowerInfoDefs::getCaloTowerPhiBin(key);

    string name  = "HotTower_"+to_string(phibin)+"_"+to_string(etabin);
    string title = "Hot Tower: iphi: " + to_string(phibin) + ", ieta: " + to_string(etabin) + "; ADC; Counts";
    auto h = new TH1F(name.c_str(), title.c_str(), bins_energy, energy_low, energy_high);
    hHotTowerEnergy.push_back(h);

    name  = "HotTowerComplement_"+to_string(phibin)+"_"+to_string(etabin);
    title = "Hot Tower Complement: iphi: " + to_string(phibin) + ", ieta: " + to_string(etabin) + "; ADC; Counts";
    h = new TH1F(name.c_str(), title.c_str(), bins_energy, energy_low, energy_high);
    hHotTowerComplementEnergy.push_back(h);

    key    = TowerInfoDefs::encode_emcal(idx.second);
    etabin = TowerInfoDefs::getCaloTowerEtaBin(key);
    phibin = TowerInfoDefs::getCaloTowerPhiBin(key);

    name  = "RefTower_"+to_string(phibin)+"_"+to_string(etabin)+"_"+to_string(i);
    title = "Reference Tower: iphi: " + to_string(phibin) + ", ieta: " + to_string(etabin) + "; ADC; Counts";
    h = new TH1F(name.c_str(), title.c_str(), bins_energy, energy_low, energy_high);
    hRefTowerEnergy.push_back(h);

    ++i;
  }

  string calibdir = CDBInterface::instance()->getUrl(m_calibName_hotMap);

  if (calibdir.empty()) {
      cout << PHWHERE << "CaloHotTower::Init Could not find " << m_calibName_hotMap << endl;
      return Fun4AllReturnCodes::ABORTRUN;
  }

  m_cdbttree_hotMap = new CDBTTree(calibdir);

  // print used DB files
  CDBInterface::instance()->Print();

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
Int_t CaloHotTower::process_event(PHCompositeNode *topNode) {

  if(iEvent%20 == 0) cout << "Progress: " << iEvent << endl;
  ++iEvent;

  // Get TowerInfoContainer
  TowerInfoContainer* towers = findNode::getClass<TowerInfoContainer>(topNode, m_emcTowerNode.c_str());
  if (!towers) {
    cout << PHWHERE << "CaloHotTower::process_event Could not find node " << m_emcTowerNode << endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  triggeranalyzer->decodeTriggers(topNode);

  Bool_t isMB = false;
  for(UInt_t i = 0; i < m_triggers.size(); ++i) {
    if(triggeranalyzer->didTriggerFire(m_triggers[i])) {
      isMB = true;
      hEvents->Fill(i);
    }
  }

  // skip event if MBD trigger isn't fired
  if(!isMB) return Fun4AllReturnCodes::ABORTEVENT;

  for (UInt_t i = 0; i < hotTowerIndex.size(); ++i) {
    UInt_t towerIndex = hotTowerIndex[i].first;
    UInt_t key        = towers->encode_key(towerIndex);
    Int_t hotMap_val  = m_cdbttree_hotMap->GetIntValue(key, "status");
    TowerInfo* tower  = towers->get_tower_at_channel(towerIndex);
    Float_t energy    = tower->get_energy();
    Bool_t isBadChi2  = tower->get_isBadChi2();

    // status 2: hot tower
    if(hotMap_val == 2 && !isBadChi2) {
      hHotTowerEnergy[i]->Fill(energy);
      energy_min = min(energy_min, energy);
      energy_max = max(energy_max, energy);
    }

    if(hotMap_val != 2 && !isBadChi2) {
      hHotTowerComplementEnergy[i]->Fill(energy);
      energy_min = min(energy_min, energy);
      energy_max = max(energy_max, energy);
    }

    towerIndex = hotTowerIndex[i].second;
    key        = towers->encode_key(towerIndex);
    hotMap_val = m_cdbttree_hotMap->GetIntValue(key, "status");
    tower      = towers->get_tower_at_channel(towerIndex);
    energy     = tower->get_energy();
    isBadChi2  = tower->get_isBadChi2();

    // status 0: good tower
    if(!hotMap_val && !isBadChi2) {
      hRefTowerEnergy[i]->Fill(energy);
      energy_min = min(energy_min, energy);
      energy_max = max(energy_max, energy);
    }
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
Int_t CaloHotTower::End(PHCompositeNode *topNode) {
  cout << "CaloHotTower::End(PHCompositeNode *topNode) This is the End..." << endl;
  cout << "Min Energy: " << energy_min << ", Max Energy: " << energy_max << endl;
  cout << "Trigger Summary" << endl;
  for(UInt_t i = 0; i < m_triggers.size(); ++i) {
    cout << m_triggers[i] << ": " << hEvents->GetBinContent(i+1) << " Events" << endl;
  }

  TFile output(m_outputFile.c_str(),"recreate");

  output.cd();
  hEvents->Write();

  output.mkdir("Hot");
  output.mkdir("HotComplement");
  output.mkdir("Ref");

  for(UInt_t i = 0; i < hotTowerIndex.size(); ++i) {
    output.cd("Hot");
    hHotTowerEnergy[i]->Write();

    output.cd("HotComplement");
    hHotTowerComplementEnergy[i]->Write();

    output.cd("Ref");
    hRefTowerEnergy[i]->Write();
  }

  output.Close();

  return Fun4AllReturnCodes::EVENT_OK;
}
