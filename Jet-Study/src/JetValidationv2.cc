#include <JetValidationv2.h>
// -- c++
#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>
// -- event
#include <ffaobjects/EventHeader.h>
#include <fun4all/Fun4AllServer.h>
// -- fun4all
#include <fun4all/Fun4AllReturnCodes.h>
// -- vertex
#include <globalvertex/GlobalVertex.h>
#include <globalvertex/GlobalVertexMap.h>
// -- jet
#include <jetbase/JetContainer.h>
// -- DST node
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>
// -- root
#include <TFile.h>
// -- Tower stuff
#include <calobase/TowerInfoDefs.h>
#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfo.h>
// -- Trigger
#include <calotrigger/TriggerRunInfoReco.h>

// Jet Utils
#include "JetUtils.h"

using std::cout;
using std::endl;
using std::string;
using std::to_string;
using std::stringstream;

//____________________________________________________________________________..
JetValidationv2::JetValidationv2()
  : SubsysReco("JetValidationv2")
  , m_recoJetName_r04("AntiKt_Tower_r04_Sub1")
  , m_emcTowerNodeBase("TOWERINFO_CALIB_CEMC_RETOWER")
  , m_emcTowerNode("TOWERINFO_CALIB_CEMC_RETOWER_SUB1")
  , m_ihcalTowerNode("TOWERINFO_CALIB_HCALIN_SUB1")
  , m_ohcalTowerNode("TOWERINFO_CALIB_HCALOUT_SUB1")
  , m_triggerBits(42)
  , m_zvtx_max(60)
  , m_bins_phi(64)
  , m_phi_low(-M_PI)
  , m_phi_high(M_PI)
  , m_bins_eta(24)
  , m_eta_low(-1.1)
  , m_eta_high(1.1)
  , m_bins_pt(400)
  , m_pt_low(0) /*GeV*/
  , m_pt_high(200) /*GeV*/
  , m_pt_background(70) /*GeV*/
  , m_bins_zvtx(400)
  , m_zvtx_low(-100)
  , m_zvtx_high(100)
  , m_event(0)
  , m_R(0.4)
{
  cout << "JetValidationv2::JetValidationv2(const std::string &name) Calling ctor" << endl;
}

//____________________________________________________________________________..
JetValidationv2::~JetValidationv2()
{
  cout << "JetValidationv2::~JetValidationv2() Calling dtor" << endl;
}

//____________________________________________________________________________..
Int_t JetValidationv2::Init(PHCompositeNode *topNode)
{
  cout << "JetValidationv2::Init(PHCompositeNode *topNode) Initializing" << endl;

  // so that the histos actually get written out
  Fun4AllServer *se = Fun4AllServer::instance();
  se->Print("NODETREE");

  hEvents = new TH1F("hEvents","Events; Status; Counts", m_triggerBits, 0, m_triggerBits);
  hEventsBkg = new TH1F("hEventsBkg","Events; Status; Counts", m_triggerBits, 0, m_triggerBits);

  stringstream title;
  title << "Jet p_{T} #geq " << m_pt_background << " GeV; Trigger; z [cm]";
  hTriggerZvtxBkg = new TH2F("hTriggerZvtxBkg",title.str().c_str(), m_triggerBits, 0, m_triggerBits
                                                                  , m_bins_zvtx, m_zvtx_low, m_zvtx_high);

  hzvtxAll = new TH1F("zvtxAll","Z Vertex; z [cm]; Counts", m_bins_zvtx, m_zvtx_low, m_zvtx_high);

  for(UInt_t i = 0; i < m_triggerBits; ++i) {
    hEvents->GetXaxis()->SetBinLabel(i+1, JetUtils::m_triggers[i].c_str());
    hEventsBkg->GetXaxis()->SetBinLabel(i+1, JetUtils::m_triggers[i].c_str());
    hTriggerZvtxBkg->GetXaxis()->SetBinLabel(i+1, JetUtils::m_triggers[i].c_str());

    // zvtx
    string name = "hzvtx_"+to_string(i);
    string title = "Z Vertex: " + JetUtils::m_triggers[i] + "; z [cm]; counts";
    auto h = new TH1F(name.c_str(),title.c_str(), m_bins_zvtx, m_zvtx_low, m_zvtx_high);
    hzvtx.push_back(h);

    // jet
    name = "hjetPhiEtaPt_"+to_string(i);
    title = "Jet: " + JetUtils::m_triggers[i] + "; #phi; #eta; p_{T} [GeV]";
    auto h3 = new TH3F(name.c_str(),title.c_str(), m_bins_phi, m_phi_low, m_phi_high,
                                                   m_bins_eta, m_eta_low, m_eta_high,
                                                   m_bins_pt, m_pt_low, m_pt_high);
    hjetPhiEtaPt.push_back(h3);
  }

  m_triggeranalyzer = new TriggerAnalyzer();

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
Int_t JetValidationv2::process_event(PHCompositeNode *topNode)
{
  EventHeader* eventInfo = findNode::getClass<EventHeader>(topNode,"EventHeader");
  if(!eventInfo)
  {
    cout << PHWHERE << "JetValidationv2::process_event - Fatal Error - EventHeader node is missing. " << endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  Int_t m_globalEvent = eventInfo->get_EvtSequence();
  Int_t m_run         = eventInfo->get_RunNumber();

  if (m_event % 20 == 0) cout << "Progress: " << m_event << ", Global: " << m_globalEvent << endl;
  ++m_event;

  TriggerRunInfo* triggerruninfo = findNode::getClass<TriggerRunInfo>(topNode, "TriggerRunInfo");
  if (!triggerruninfo) {
    cout << "JetValidationv2::process_event - Error can not find TriggerRunInfo node " << endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }
  if(m_event == 1) {
    triggerruninfo->identify();
  }

  // zvertex
  Float_t m_zvtx = -9999;
  GlobalVertexMap* vertexmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");

  if (!vertexmap) {
    cout << "JetValidationv2::process_event - Error can not find global vertex node " << endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  if(!vertexmap->empty()) {
    GlobalVertex* vtx = vertexmap->begin()->second;
    m_zvtx = vtx->get_z();
  }

  hzvtxAll->Fill(m_zvtx);

  // skip event if zvtx is too large
  if(fabs(m_zvtx) >= m_zvtx_max) {
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  m_triggeranalyzer->decodeTriggers(topNode);

  vector<Int_t> triggerIdx;
  for(UInt_t i = 0; i < m_triggerBits; ++i) {
    if(m_triggeranalyzer->didTriggerFire(i)) {
      triggerIdx.push_back(i);
    }
  }

  // default value: None
  if(triggerIdx.empty()) {
    triggerIdx.push_back(41);
  }

  for(auto idx : triggerIdx) {
    hEvents->Fill(idx);
  }

  // interface to reco jets
  JetContainer* jets_r04 = findNode::getClass<JetContainer>(topNode, m_recoJetName_r04);
  if (!jets_r04) {
    cout << "JetValidationv2::process_event - Error can not find DST Reco JetContainer node " << m_recoJetName_r04 << endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  Int_t jetPtLead    = 0;
  Int_t jetPtSubLead = 0;

  Float_t jetPhiLead = 0;
  Float_t jetPhiSubLead = 0;

  Float_t jetEtaLead = 0;
  Float_t jetEtaSubLead = 0;

  // trigger
  Bool_t hasBkg = false;
  for (auto jet : *jets_r04) {
    Float_t phi = jet->get_phi();
    Float_t eta = jet->get_eta();
    Float_t pt = jet->get_pt();

    // exclude jets near the edge of the detector
    if(JetUtils::check_bad_jet_eta(eta, m_zvtx, m_R)) continue;

    if(pt >= m_pt_background) {
      hasBkg = true;
    }

    if((Int_t)pt > jetPtSubLead) {
      if((Int_t)pt > jetPtLead) {
        jetPtSubLead = jetPtLead;
        jetPtLead = pt;

        jetPhiSubLead = jetPhiLead;
        jetPhiLead = phi;

        jetEtaSubLead = jetEtaLead;
        jetEtaLead = eta;
      }
      else {
        jetPtSubLead  = pt;
        jetPhiSubLead = phi;
        jetEtaSubLead = eta;
      }
    }
  }

  // if event doesn't have a high pt jet then skip to the next event
  if(!hasBkg) return Fun4AllReturnCodes::ABORTEVENT;

  // round nearest 0.1
  jetPhiLead    = (Int_t)(jetPhiLead*10)/10.;
  jetPhiSubLead = (Int_t)(jetPhiSubLead*10)/10.;

  jetEtaLead    = (Int_t)(jetEtaLead*10)/10.;
  jetEtaSubLead = (Int_t)(jetEtaSubLead*10)/10.;

  // Get TowerInfoContainer
  // Unsubtracted EMCal Towers
  TowerInfoContainer* towersCEMCBase = findNode::getClass<TowerInfoContainer>(topNode, m_emcTowerNodeBase.c_str());

  // Subtracted Towers
  TowerInfoContainer* towersCEMC  = findNode::getClass<TowerInfoContainer>(topNode, m_emcTowerNode.c_str());
  TowerInfoContainer* towersIHCal = findNode::getClass<TowerInfoContainer>(topNode, m_ihcalTowerNode.c_str());
  TowerInfoContainer* towersOHCal = findNode::getClass<TowerInfoContainer>(topNode, m_ohcalTowerNode.c_str());

  if (!towersCEMCBase || !towersCEMC || !towersIHCal || !towersOHCal) {
    cout << PHWHERE << "JetValidation::process_event Could not find one of "
         << m_emcTowerNodeBase      << ", "
         << m_emcTowerNode      << ", "
         << m_ihcalTowerNode    << ", "
         << m_ohcalTowerNode    << ", "
         << endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  stringstream name;
  stringstream nameSuffix;
  stringstream title;
  stringstream titleSuffix;

  titleSuffix << "Run: " << m_run << ", Event: " << m_globalEvent
              << ", Jet 1 p_{T}: " << jetPtLead << " GeV, (" << jetPhiLead << "," << jetEtaLead << ")"
              << ", Jet 2 p_{T}: " << jetPtSubLead << " GeV, (" << jetPhiSubLead << "," << jetEtaSubLead << ")"
              << "; #phi; #eta";

  string s_triggerIdx = "";

  for(auto idx : triggerIdx) {
    if(!s_triggerIdx.empty()) s_triggerIdx += "-";
    s_triggerIdx += to_string(idx);
  }

  nameSuffix << m_run << "_" << m_globalEvent << "_" << s_triggerIdx << "_" << jetPtLead << "_" << jetPtSubLead;

  name << "hCEMCBase_" << nameSuffix.str();
  title << "CEMC: " << titleSuffix.str();

  auto hCEMCBase_ = new TH2F(name.str().c_str(), title.str().c_str(), m_bins_phi, m_phi_low, m_phi_high, m_bins_eta, m_eta_low, m_eta_high);

  name.str("");
  title.str("");
  name << "hCEMC_" << nameSuffix.str();
  title << "CEMC: " << titleSuffix.str();

  auto hCEMC_ = new TH2F(name.str().c_str(), title.str().c_str(), m_bins_phi, m_phi_low, m_phi_high, m_bins_eta, m_eta_low, m_eta_high);

  name.str("");
  title.str("");
  name << "hIHCal_" << nameSuffix.str();
  title << "IHCal: " << titleSuffix.str();

  auto hIHCal_ = new TH2F(name.str().c_str(), title.str().c_str(), m_bins_phi, m_phi_low, m_phi_high, m_bins_eta, m_eta_low, m_eta_high);

  name.str("");
  title.str("");
  name << "hOHCal_" << nameSuffix.str();
  title << "OHCal: " << titleSuffix.str();

  auto hOHCal_ = new TH2F(name.str().c_str(), title.str().c_str(), m_bins_phi, m_phi_low, m_phi_high, m_bins_eta, m_eta_low, m_eta_high);

  Float_t totalCEMC = 0;
  Float_t totalHCal = 0;

  // loop over towers
  for(UInt_t towerIndex = 0; towerIndex < towersCEMC->size(); ++towerIndex) {
    UInt_t key  = TowerInfoDefs::encode_hcal(towerIndex);
    UInt_t iphi = TowerInfoDefs::getCaloTowerPhiBin(key);
    UInt_t ieta = TowerInfoDefs::getCaloTowerEtaBin(key);

    // map: [0, 31]  -> [32,63], [0,pi]
    // map: [32, 63] -> [0,31], [-pi,0]
    iphi = (iphi + m_bins_phi/2) % m_bins_phi;

    // EMCal
    TowerInfo* tower = towersCEMC->get_tower_at_channel(towerIndex);
    Float_t energy = (tower->get_isGood()) ? tower->get_energy() : 0;

    hCEMC_->SetBinContent(iphi+1, ieta+1, energy);

    totalCEMC += energy;

    tower = towersCEMCBase->get_tower_at_channel(towerIndex);
    energy = (tower->get_isGood()) ? tower->get_energy() : 0;

    hCEMCBase_->SetBinContent(iphi+1, ieta+1, energy);

    // HCal
    tower = towersIHCal->get_tower_at_channel(towerIndex);
    energy = (tower->get_isGood()) ? tower->get_energy() : 0;

    hIHCal_->SetBinContent(iphi+1, ieta+1, energy);

    totalHCal += energy;

    tower = towersOHCal->get_tower_at_channel(towerIndex);
    energy = (tower->get_isGood()) ? tower->get_energy() : 0;

    hOHCal_->SetBinContent(iphi+1, ieta+1, energy);

    totalHCal += energy;
  }

  // ensure that EMCal has the majority of the energy recorded
  if(totalCEMC <= totalHCal) return Fun4AllReturnCodes::ABORTEVENT;

  hCEMCBase.push_back(hCEMCBase_);
  hCEMC.push_back(hCEMC_);
  hIHCal.push_back(hIHCal_);
  hOHCal.push_back(hOHCal_);


  for(auto idx : triggerIdx) {
    hEventsBkg->Fill(idx);
    hzvtx[idx]->Fill(m_zvtx);
    hTriggerZvtxBkg->Fill(idx, m_zvtx);
  }

  for (auto jet : *jets_r04) {
    Float_t phi = jet->get_phi();
    Float_t eta = jet->get_eta();
    Float_t pt = jet->get_pt();

    // exclude jets near the edge of the detector
    if(JetUtils::check_bad_jet_eta(eta, m_zvtx, m_R)) continue;

    for(auto idx : triggerIdx) {
      hjetPhiEtaPt[idx]->Fill(phi, eta, pt);
    }
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
Int_t JetValidationv2::ResetEvent(PHCompositeNode *topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
Int_t JetValidationv2::End(PHCompositeNode *topNode)
{
  cout << "JetValidationv2::End(PHCompositeNode *topNode) This is the End..." << endl;
  cout << "Trigger Summary" << endl;
  for(UInt_t i = 0; i < m_triggerBits; ++i) {
    UInt_t evts    = hEvents->GetBinContent(i+1);
    UInt_t evtsBkg = hEventsBkg->GetBinContent(i+1);
    Float_t frac = (evts) ? evtsBkg*100./evts : 0;
    cout << JetUtils::m_triggers[i] << ": " << evts << " Events, "
                                            << evtsBkg << " Background Events, "
                                            << frac << " %"
                                            << endl;
  }

  TFile output(m_outputFile.c_str(),"recreate");

  output.cd();

  output.mkdir("event");
  output.mkdir("zvtx");
  output.mkdir("jets");

  output.mkdir("CEMCBase");
  output.mkdir("CEMC");
  output.mkdir("IHCal");
  output.mkdir("OHCal");

  output.cd("event");
  hEvents->Write();
  hEventsBkg->Write();
  hTriggerZvtxBkg->Write();

  output.cd("zvtx");
  hzvtxAll->Write();
  for(UInt_t i = 0; i < m_triggerBits; ++i) {
    if(hzvtx[i]->GetEntries()) {
      output.cd("zvtx");
      hzvtx[i]->Write();
    }

    if(hjetPhiEtaPt[i]->GetEntries()) {
      output.cd("jets");
      hjetPhiEtaPt[i]->Write();
    }
  }

  for(UInt_t i = 0; i < hCEMC.size(); ++i) {
    output.cd("CEMCBase");
    hCEMCBase[i]->Write();

    output.cd("CEMC");
    hCEMC[i]->Write();

    output.cd("IHCal");
    hIHCal[i]->Write();

    output.cd("OHCal");
    hOHCal[i]->Write();
  }

  output.Close();

  cout << "JetValidationv2::End(PHCompositeNode *topNode) This is the End..." << endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

