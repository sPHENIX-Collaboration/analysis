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
#include <sstream>

using std::cout;
using std::endl;
using std::string;
using std::stringstream;
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
 , m_ntowIBSide(8)
 , m_nsamples(12)
 , m_bins_adc(180)
 , m_adc_low(0)
 , m_adc_high(1.8e4)
 , m_bins_ADC(1640)
 , m_ADC_low(0)
 , m_ADC_high(16400)
 , m_min_adc(9999)
 , m_max_adc(0)
 , m_doAllWaveforms(false)
{
  cout << "CaloTower::CaloTower(const string &name) Calling ctor" << endl;
}

//____________________________________________________________________________..
CaloTower::~CaloTower()
{
  cout << "CaloTower::~CaloTower() Calling dtor" << endl;

  for (const auto& [name, hist] : m_hists) {
    delete hist;
  }

  cout << "CaloTower::~CaloTower() Finished dtor" << endl;
}

//____________________________________________________________________________..
int CaloTower::Init(PHCompositeNode *topNode)
{
  cout << "CaloTower::Init(PHCompositeNode *topNode) Initializing" << endl;
  Fun4AllServer *se = Fun4AllServer::instance();
  se->Print("NODETREE");

  m_hists["h2CEMC"] = new TH2F("h2CEMC","EMCal [ADC]; Tower Index #phi; Tower Index #eta", m_nphi, -0.5, m_nphi-0.5, m_neta, -0.5, m_neta-0.5);
  m_hists["h2CEMC"]->Sumw2();
  m_hists["hCEMC"] = new TH1F("hCEMC","EMCal [ADC]; ADC; Counts", m_bins_ADC, m_ADC_low, m_ADC_high);
  m_hists["hCEMC"]->Sumw2();

  m_hists["hEvent"] = new TH1F("hEvent","Events; Status; Counts", 1, 0, 1);

  stringstream name;
  stringstream title;
  if(!m_doAllWaveforms) {
    for (const auto& [phi, eta] : m_nphi_neta_low) {
      for(Int_t iphi = phi; iphi < phi+m_ntowIBSide; ++iphi) {
        for(Int_t ieta = eta; ieta < eta+m_ntowIBSide; ++ieta) {
            name.str("");
            title.str("");
            name << "h2adc_" << iphi << "_" << ieta;
            title << "adc: (" << iphi << "," << ieta << "); Sample; adc";

            m_hists[name.str()] = new TH2F(name.str().c_str(), title.str().c_str(), m_nsamples, 0, m_nsamples, m_bins_adc, m_adc_low, m_adc_high);
        }
      }
    }
  }
  else {
    for (Int_t iphi = 0; iphi < m_nphi; ++iphi) {
      for (Int_t ieta = 0; ieta < m_neta; ++ieta) {
        name.str("");
        title.str("");
        name << "h2adc_" << iphi << "_" << ieta;
        title << "adc: (" << iphi << "," << ieta << "); Sample; adc";

        m_hists[name.str()] = new TH2F(name.str().c_str(), title.str().c_str(), m_nsamples, 0, m_nsamples, m_bins_adc, m_adc_low, m_adc_high);
      }
    }
  }

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
  stringstream name;
  for(UInt_t towerIndex = 0; towerIndex < towers->size(); ++towerIndex) {
    UInt_t key  = TowerInfoDefs::encode_emcal(towerIndex);
    Int_t iphi = TowerInfoDefs::getCaloTowerPhiBin(key);
    Int_t ieta = TowerInfoDefs::getCaloTowerEtaBin(key);

    TowerInfo* tower = towers->get_tower_at_channel(towerIndex);
    Double_t energy = (tower->get_isGood()) ? tower->get_energy() : 0;
    // Double_t energy = tower->get_energy();
    m_min_energy = min(m_min_energy, energy);
    m_max_energy = max(m_max_energy, energy);

    ((TH2*)m_hists["h2CEMC"])->Fill(iphi, ieta, energy);

    name.str("");
    name << "h2adc_" << iphi << "_" << ieta;
    if(m_doAllWaveforms || m_hists.contains(name.str())) {
      for(Int_t sample = 0; sample < m_nsamples; ++sample) {
        Int_t adc = tower->get_waveform_value(sample);
        m_min_adc = min(m_min_adc, adc);
        m_max_adc = max(m_max_adc, adc);

        ((TH2*)m_hists[name.str()])->Fill(sample, adc);
      }
    }
  }

  m_hists["hEvent"]->Fill(0);

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int CaloTower::End(PHCompositeNode *topNode)
{
  cout << "CaloTower::End(PHCompositeNode *topNode) This is the End..." << endl;
  cout << "Energy: Min: " << m_min_energy << ", Max: " << m_max_energy << endl;
  cout << "adc: Min: " << m_min_adc << ", Max: " << m_max_adc << endl;

  Int_t nEvents = m_hists["hEvent"]->GetBinContent(1);
  cout << "Events Total: " << m_Event << ", Processed: " << nEvents << endl;
  // scale the hists to get the average tower energy
  m_hists["h2CEMC"]->Scale(1./nEvents);

  for(Int_t iphi = 1; iphi <= m_nphi; ++iphi) {
    for(Int_t ieta = 1; ieta <= m_neta; ++ieta) {
      Double_t energy = ((TH2*)m_hists["h2CEMC"])->GetBinContent(iphi, ieta);
      m_hists["hCEMC"]->Fill(energy);
    }
  }
  
  TFile output(m_outputFile.c_str(),"recreate");

  output.cd();
  output.mkdir("waveform");

  for (const auto& [name, hist] : m_hists) {
    if(!name.starts_with("h2adc_")) hist->Write();
  }

  output.cd("waveform");
  for (const auto& [name, hist] : m_hists) {
    if(name.starts_with("h2adc_")) hist->Write();
  }

  output.Close();
  return Fun4AllReturnCodes::EVENT_OK;
}
