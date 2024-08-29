#include <EventValidation.h>
// -- c++
#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>
// -- event / trigger
#include <ffaobjects/EventHeader.h>
#include <ffarawobjects/Gl1Packet.h>
// -- fun4all
#include <fun4all/Fun4AllBase.h>
#include <fun4all/Fun4AllReturnCodes.h>
// -- vertex
#include <globalvertex/GlobalVertex.h>
#include <globalvertex/GlobalVertexMap.h>
// -- DST node
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>
// -- root
#include <TFile.h>
#include <TTree.h>
// -- Tower stuff
#include <calobase/TowerInfo.h>
#include <calobase/TowerInfoDefs.h>
#include <calobase/TowerInfoContainer.h>

using std::cout;
using std::endl;
using std::string;
using std::to_string;
using std::vector;
using std::stringstream;

//____________________________________________________________________________..
EventValidation::EventValidation()
  : SubsysReco("EventValidation")
  , m_outputTreeFile(nullptr)
  , m_outputQAFile(nullptr)
  , m_outputTreeFileName("test.root")
  , m_outputQAFileName("qa.root")
  , m_emcTowerNodeBase("TOWERINFO_CALIB_CEMC")
  , m_emcTowerNode("TOWERINFO_CALIB_CEMC_RETOWER")
  , m_ihcalTowerNode("TOWERINFO_CALIB_HCALIN")
  , m_ohcalTowerNode("TOWERINFO_CALIB_HCALOUT")
  , m_saveHistMax(5)
  , m_use_zvtx(true)
  , m_zvtx_max(30) /*cm*/
  , m_zvtx_max2(20) /*cm*/
  , m_zvtx_max3(10) /*cm*/
  , m_bins_events(5)
  , m_bins_events_jets(3)
  , m_bins_phi(64)
  , m_bins_eta(24)
  , m_bins_zvtx(200)
  , m_zvtx_low(-50)
  , m_zvtx_high(50)
  , m_bkg_tower_energy(0.6) /*GeV*/
  , m_bkg_tower_neighbor_energy(0.06) /*GeV*/
  , m_bkg_towers(6)
  , m_T(nullptr)
  , m_run(0)
  , m_globalEvent(0)
  , m_event(0)
  , m_eventZVtx(0)
  , m_eventZVtx30(0)
{
  cout << "EventValidation::EventValidation(const std::string &name) Calling ctor" << endl;
}

//____________________________________________________________________________..
EventValidation::~EventValidation()
{
  cout << "EventValidation::~EventValidation() Calling dtor" << endl;
}

//____________________________________________________________________________..
Int_t EventValidation::Init(PHCompositeNode *topNode)
{
  cout << "EventValidation::Init(PHCompositeNode *topNode) Initializing" << endl;
  m_outputTreeFile = new TFile(m_outputTreeFileName.c_str(),"RECREATE");
  cout << "EventValidation::Init - Output to " << m_outputTreeFileName << endl;

  // configure Tree
  m_T = new TTree("T", "T");
  m_T->Branch("event", &m_globalEvent, "event/I");
  m_T->Branch("run", &m_run, "run/I");
  m_T->Branch("zvtx", &m_zvtx);
  m_T->Branch("hasBkg", & m_hasBkg);
  m_T->Branch("hasBkgCEMC", &m_hasBkgCEMC);
  m_T->Branch("triggerVector", &m_triggerVector);

  m_T->Branch("towersCEMCBase_isGood", &m_towersCEMCBase_isGood);
  m_T->Branch("towersCEMCBase_energy", &m_towersCEMCBase_energy);
  m_T->Branch("towersCEMCBase_time",   &m_towersCEMCBase_time);

  m_T->Branch("towersCEMC_isGood",  &m_towersCEMC_isGood);
  m_T->Branch("towersIHCal_isGood", &m_towersIHCal_isGood);
  m_T->Branch("towersOHCal_isGood", &m_towersOHCal_isGood);

  m_T->Branch("towersCEMC_energy",  &m_towersCEMC_energy);
  m_T->Branch("towersIHCal_energy", &m_towersIHCal_energy);
  m_T->Branch("towersOHCal_energy", &m_towersOHCal_energy);

  m_outputQAFile = new TFile(m_outputQAFileName.c_str(),"RECREATE");
  cout << "EventValidation::Init - Output to " << m_outputQAFileName << endl;

  hEvents       = new TH1F("hEvents","Events; Status; Counts", m_bins_events, 0, m_bins_events);
  hEvents_Jet6  = new TH1F("hEvents_Jet6","Events: Jet 6 GeV, |Z| < 30 cm; Status; Counts", m_bins_events_jets, 0, m_bins_events_jets);
  hEvents_Jet8  = new TH1F("hEvents_Jet8","Events: Jet 8 GeV, |Z| < 30 cm; Status; Counts", m_bins_events_jets, 0, m_bins_events_jets);
  hEvents_Jet10 = new TH1F("hEvents_Jet10","Events: Jet 10 GeV, |Z| < 30 cm; Status; Counts", m_bins_events_jets, 0, m_bins_events_jets);
  hEvents_Jet12 = new TH1F("hEvents_Jet12","Events: Jet 12 GeV, |Z| < 30 cm; Status; Counts", m_bins_events_jets, 0, m_bins_events_jets);
  hZVtx = new TH1F("hZVtx","Z Vertex; z [cm]; Counts", m_bins_zvtx, m_zvtx_low, m_zvtx_high);

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
Int_t EventValidation::process_event(PHCompositeNode *topNode)
{
  EventHeader* eventInfo = findNode::getClass<EventHeader>(topNode,"EventHeader");
  if(!eventInfo)
  {
    cout << PHWHERE << "EventValidation::process_event - Fatal Error - EventHeader node is missing. " << endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  m_globalEvent = eventInfo->get_EvtSequence();
  m_run         = eventInfo->get_RunNumber();

  if (m_event % 20 == 0) cout << "Progress: " << m_event << ", Global: " << m_globalEvent << endl;
  ++m_event;
  hEvents->Fill(0);

  // zvertex
  GlobalVertexMap* vertexmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");
  if (!vertexmap) {
    cout << PHWHERE << "EventValidation::process_event - Fatal Error - GlobalVertexMap node is missing. " << endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }
  if (m_use_zvtx && vertexmap->empty()) {
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  // grab the gl1 data
  Gl1Packet* gl1PacketInfo = findNode::getClass<Gl1Packet>(topNode, "GL1Packet");
  if (!gl1PacketInfo) {
    cout << PHWHERE << "EventValidation::process_event - Fatal Error - GL1Packet node is missing. " << endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  uint64_t triggervec = gl1PacketInfo->getTriggerVector();
  for (Int_t i = 0; i < 64; ++i) {
    Bool_t trig_decision = ((triggervec & 0x1U) == 0x1U);
    m_triggerVector.push_back(trig_decision);
    triggervec = (triggervec >> 1U) & 0xffffffffU;
  }

  if(!vertexmap->empty()) {
    GlobalVertex *vtx = vertexmap->begin()->second;
    m_zvtx = vtx->get_z();

    hZVtx->Fill(m_zvtx);
    ++m_eventZVtx;
    hEvents->Fill(1);
  }
  else m_zvtx = -9999;

  if(m_use_zvtx && abs(m_zvtx) >= m_zvtx_max) return Fun4AllReturnCodes::ABORTEVENT;

  if(abs(m_zvtx) < m_zvtx_max)  {
    hEvents->Fill(2);
    ++m_eventZVtx30;
  }
  if(abs(m_zvtx) < m_zvtx_max2) hEvents->Fill(3);
  if(abs(m_zvtx) < m_zvtx_max3) hEvents->Fill(4);

  // Get TowerInfoContainer
  // Base EMCal Towers
  TowerInfoContainer* towersCEMCBase = findNode::getClass<TowerInfoContainer>(topNode, m_emcTowerNodeBase.c_str());

  // unsubtracted
  TowerInfoContainer* towersCEMC  = findNode::getClass<TowerInfoContainer>(topNode, m_emcTowerNode.c_str());
  TowerInfoContainer* towersIHCal = findNode::getClass<TowerInfoContainer>(topNode, m_ihcalTowerNode.c_str());
  TowerInfoContainer* towersOHCal = findNode::getClass<TowerInfoContainer>(topNode, m_ohcalTowerNode.c_str());

  if (!towersCEMCBase || !towersCEMC || !towersIHCal || !towersOHCal) {
    cout << PHWHERE << "EventValidation::process_event Could not find one of "
         << m_emcTowerNodeBase  << ", "
         << m_emcTowerNode      << ", "
         << m_ihcalTowerNode    << ", "
         << m_ohcalTowerNode
         << endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  vector<Float_t> towerEnergy(towersCEMC->size());
  vector<Float_t> towerEnergyCEMC(towersCEMC->size());
  stringstream name;
  stringstream title;

  name.str("");
  title.str("");

  name << "h2TowerEnergy_" << m_run << "_" << m_globalEvent;
  title << "Tower Energy, Run: " << m_run << ", Event: " << m_globalEvent << ", Z: " << (Int_t)(m_zvtx*10)/10. << " cm; Tower Index #phi; Tower Index #eta";

  auto h2 = new TH2F(name.str().c_str(), title.str().c_str(), m_bins_phi, -0.5, m_bins_phi-0.5, m_bins_eta, -0.5, m_bins_eta-0.5);

  name.str("");
  title.str("");

  name << "h2TowerEnergyCEMC_" << m_run << "_" << m_globalEvent;
  title << "Tower Energy EMCal, Run: " << m_run << ", Event: " << m_globalEvent << ", Z: " << (Int_t)(m_zvtx*10)/10. << " cm; Tower Index #phi; Tower Index #eta";

  auto h2CEMC = new TH2F(name.str().c_str(), title.str().c_str(), m_bins_phi, -0.5, m_bins_phi-0.5, m_bins_eta, -0.5, m_bins_eta-0.5);

  // loop over towers
  for(UInt_t towerIndex = 0; towerIndex < towersCEMC->size(); ++towerIndex) {
    UInt_t key       = TowerInfoDefs::encode_hcal(towerIndex);
    UInt_t iphi      = TowerInfoDefs::getCaloTowerPhiBin(key);
    UInt_t ieta      = TowerInfoDefs::getCaloTowerEtaBin(key);
    TowerInfo* tower = towersCEMC->get_tower_at_channel(towerIndex);

    m_towersCEMC_energy.push_back(tower->get_energy());
    m_towersCEMC_isGood.push_back(tower->get_isGood());

    Float_t energy = (tower->get_isGood()) ? tower->get_energy() : 0;

    towerEnergyCEMC[towerIndex] = energy;

    h2CEMC->SetBinContent(iphi+1,ieta+1,energy);

    tower = towersIHCal->get_tower_at_channel(towerIndex);
    m_towersIHCal_energy.push_back(tower->get_energy());
    m_towersIHCal_isGood.push_back(tower->get_isGood());

    energy += (tower->get_isGood()) ? tower->get_energy() : 0;

    tower = towersOHCal->get_tower_at_channel(towerIndex);
    m_towersOHCal_energy.push_back(tower->get_energy());
    m_towersOHCal_isGood.push_back(tower->get_isGood());

    energy += (tower->get_isGood()) ? tower->get_energy() : 0;

    towerEnergy[towerIndex] = energy;

    h2->SetBinContent(iphi+1,ieta+1,energy);
  }

  h2TowerEnergy.push_back(h2);
  h2TowerEnergyCEMC.push_back(h2CEMC);

  m_hasBkgCEMC = isBackgroundEvent(towerEnergyCEMC);
  m_hasBkg     = (m_hasBkgCEMC) ? m_hasBkgCEMC : isBackgroundEvent(towerEnergy);

  m_hasBkgCEMC_vec.push_back(m_hasBkgCEMC);
  m_hasBkg_vec.push_back(m_hasBkg);

  if(!m_use_zvtx || (m_use_zvtx && abs(m_zvtx) < m_zvtx_max)) {
    if(m_triggerVector[(Int_t)Trigger::JET_6]) {
      hEvents_Jet6->Fill(0);
      if(m_hasBkg) hEvents_Jet6->Fill(1);
      if(m_hasBkgCEMC) hEvents_Jet6->Fill(2);
    }

    if(m_triggerVector[(Int_t)Trigger::JET_8]) {
      hEvents_Jet8->Fill(0);
      if(m_hasBkg) hEvents_Jet8->Fill(1);
      if(m_hasBkgCEMC) hEvents_Jet8->Fill(2);
    }

    if(m_triggerVector[(Int_t)Trigger::JET_10]) {
      hEvents_Jet10->Fill(0);
      if(m_hasBkg) hEvents_Jet10->Fill(1);
      if(m_hasBkgCEMC) hEvents_Jet10->Fill(2);
    }

    if(m_triggerVector[(Int_t)Trigger::JET_12]) {
      hEvents_Jet12->Fill(0);
      if(m_hasBkg) hEvents_Jet12->Fill(1);
      if(m_hasBkgCEMC) hEvents_Jet12->Fill(2);
    }
  }

  // looping over base emcal towers is time intensive so only do this when event has background
  if(m_hasBkgCEMC || m_hasBkg) {
    // loop over base towers
    for(UInt_t towerIndex = 0; towerIndex < towersCEMCBase->size(); ++towerIndex) {
      TowerInfo* tower = towersCEMCBase->get_tower_at_channel(towerIndex);
      m_towersCEMCBase_isGood.push_back(tower->get_isGood());
      m_towersCEMCBase_time.push_back(tower->get_time_float());
      m_towersCEMCBase_energy.push_back(tower->get_energy());
    }

    // fill the tree
    m_T->Fill();
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
Int_t EventValidation::ResetEvent(PHCompositeNode *topNode)
{
  m_triggerVector.clear();

  m_towersCEMCBase_isGood.clear();
  m_towersCEMCBase_energy.clear();
  m_towersCEMCBase_time.clear();

  m_towersCEMC_isGood.clear();
  m_towersIHCal_isGood.clear();
  m_towersOHCal_isGood.clear();

  m_towersCEMC_energy.clear();
  m_towersIHCal_energy.clear();
  m_towersOHCal_energy.clear();

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
Int_t EventValidation::End(PHCompositeNode *topNode)
{
  cout << "Events With Z Vtx: " << m_eventZVtx << ", " << m_eventZVtx * 100./m_event << " %" << endl;
  cout << "Events With |Z| < 30 cm: " << m_eventZVtx30 << ", " << m_eventZVtx30 * 100./m_event << " %" << endl;

  cout << "EventValidation::End - Output to " << m_outputTreeFileName << endl;
  m_outputTreeFile->cd();

  m_T->Write();

  m_outputTreeFile->Close();
  delete m_outputTreeFile;

  cout << "EventValidation::End - Output to " << m_outputQAFileName << endl;

  m_outputQAFile->cd();

  hEvents->Write();
  hZVtx->Write();

  hEvents_Jet6->Write();
  hEvents_Jet8->Write();
  hEvents_Jet10->Write();
  hEvents_Jet12->Write();

  stringstream dirName("");
  stringstream dirNameCEMC("");

  dirName << "h2TowerEnergy/" << m_run << "/Total";
  m_outputQAFile->mkdir(dirName.str().c_str());

  dirNameCEMC << "h2TowerEnergy/" << m_run << "/CEMC";
  m_outputQAFile->mkdir(dirNameCEMC.str().c_str());

  UInt_t ctr[2] = {0};
  for(UInt_t i = 0; i < h2TowerEnergy.size(); ++i) {
    if(m_hasBkgCEMC_vec[i] && ++ctr[0] <= m_saveHistMax) {
      m_outputQAFile->cd(dirNameCEMC.str().c_str());
      h2TowerEnergyCEMC[i]->Write();
    }

    if(m_hasBkg_vec[i] && ++ctr[1] <= m_saveHistMax) {
      m_outputQAFile->cd(dirName.str().c_str());
      h2TowerEnergy[i]->Write();
    }
  }
  cout << "CEMC Background Events Tagged: " << ctr[0] << endl;
  cout << "Background Events Tagged: "      << ctr[1] << endl;

  m_outputQAFile->Close();
  delete m_outputQAFile;

  cout << "EventValidation::End(PHCompositeNode *topNode) This is the End..." << endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
Bool_t EventValidation::isBackgroundEvent(std::vector<Float_t> &towerEnergy) {
  UInt_t ctr = 0;

  for(UInt_t i = 0; i < m_bins_phi; ++i) {
    for(UInt_t j = 0; j < m_bins_eta; ++j) {
      UInt_t key        = TowerInfoDefs::encode_hcal(j,i);
      UInt_t towerIndex = TowerInfoDefs::decode_hcal(key);
      Float_t energy    = towerEnergy[towerIndex];

      if(energy < m_bkg_tower_energy) {
        ctr = 0;
        continue;
      }

      // left side
      key        = (i == 0) ? TowerInfoDefs::encode_hcal(j,m_bins_phi-1) : TowerInfoDefs::encode_hcal(j,i-1);
      towerIndex = TowerInfoDefs::decode_hcal(key);
      energy     = towerEnergy[towerIndex];

      if(energy >= m_bkg_tower_neighbor_energy) {
        ctr = 0;
        continue;
      }

      // right side
      key        = (i == m_bins_phi-1) ? TowerInfoDefs::encode_hcal(j,0) : TowerInfoDefs::encode_hcal(j,i+1);
      towerIndex = TowerInfoDefs::decode_hcal(key);
      energy     = towerEnergy[towerIndex];

      if(energy < m_bkg_tower_neighbor_energy){
        ++ctr;
        if(ctr >= m_bkg_towers) {
          cout << "Found Background: (" << i << "," << j-m_bkg_towers+1 << ")" << " to (" << i << "," << j << ")" << endl;
          return true;
        }
      }
      else ctr = 0;
    }
    ctr = 0;
  }

  return false;
}
