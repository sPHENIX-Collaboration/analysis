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
// -- flags
#include <phool/recoConsts.h>
// -- root
#include <TFile.h>
// -- Tower stuff
#include <calobase/TowerInfoDefs.h>
#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfo.h>
// -- Trigger
#include <calotrigger/TriggerRunInfo.h>

// Jet Utils
#include "JetUtils.h"

using std::cout;
using std::endl;
using std::string;
using std::to_string;
using std::stringstream;
using std::min;
using std::max;

//____________________________________________________________________________..
JetValidationv2::JetValidationv2()
  : SubsysReco("JetValidationv2")
  , m_recoJetName_r04("AntiKt_unsubtracted_r04")
  , m_emcTowerBaseNode("TOWERINFO_CALIB_CEMC")
  , m_emcTowerNode("TOWERINFO_CALIB_CEMC_RETOWER")
  , m_ihcalTowerNode("TOWERINFO_CALIB_HCALIN")
  , m_ohcalTowerNode("TOWERINFO_CALIB_HCALOUT")
  , m_triggerBit(17) /*Jet 8 GeV + MBD NS >= 1*/
  , m_triggerBits(42)
  , m_zvtx_max(30) /*cm*/
  , m_bins_phi(64)
  , m_bins_phi_cemc(256)
  , m_phi_low(-M_PI)
  , m_phi_high(M_PI)
  , m_bins_eta(24)
  , m_bins_eta_cemc(96)
  , m_eta_low(-1.1)
  , m_eta_high(1.1)
  , m_bins_pt(400)
  , m_pt_low(0) /*GeV*/
  , m_pt_high(200) /*GeV*/
  , m_pt_background(60) /*GeV*/
  , m_bins_zvtx(400)
  , m_zvtx_low(-100)
  , m_zvtx_high(100)
  , m_bins_frac(140)
  , m_frac_low(-0.2)
  , m_frac_high(1.2)
  , m_bins_ET(190)
  , m_ET_low(10)
  , m_ET_high(200)
  , m_bins_constituents(300)
  , m_constituents_low(0)
  , m_constituents_high(300)
  , m_bins_nJets(100)
  , m_nJets_low(0)
  , m_nJets_high(100)
  , m_event(0)
  , m_R(0.4)
  , m_nJets_min(9999)
  , m_nJets_max(0)
  , m_constituents_min(9999)
  , m_constituents_max(0)
  , m_pt_min(9999)
  , m_pt_max(0)
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

  hEvents = new TH1F("hEvents","Events; Status; Counts", m_eventStatus.size(), 0, m_eventStatus.size());

  for(UInt_t i = 0; i < m_eventStatus.size(); ++i) {
    hEvents->GetXaxis()->SetBinLabel(i+1, m_eventStatus[i].c_str());
  }

  hzvtxAll = new TH1F("zvtxAll","Z Vertex; z [cm]; Counts", m_bins_zvtx, m_zvtx_low, m_zvtx_high);

  hjetPhiEtaPt = new TH3F("hjetPhiEtaPt", "Jet; #phi; #eta; p_{T} [GeV]", m_bins_phi, m_phi_low, m_phi_high
                                                                        , m_bins_eta, m_eta_low, m_eta_high
                                                                        , m_bins_pt, m_pt_low, m_pt_high);

  hjetConstituentsVsPt = new TH2F("hjetConstituentsVsPt", "Jet; p_{T} [GeV]; Constituents", m_bins_pt, m_pt_low, m_pt_high
                                                            , m_bins_constituents, m_constituents_low, m_constituents_high);

  hNJetsVsLeadPt = new TH2F("hNJetsVsLeadPt", "Event; Lead p_{T} [GeV]; # of Jets", m_bins_pt, m_pt_low, m_pt_high
                                                                                  , m_bins_nJets, m_nJets_low, m_nJets_high);

  h2ETVsFracCEMC = new TH2F("h2ETVsFracCEMC","Jet; Fraction of E_{T,Lead Jet} EMCal; E_{T,Lead Jet} [GeV]"
                            , m_bins_frac, m_frac_low, m_frac_high, m_bins_ET, m_ET_low, m_ET_high);

  h2ETVsFracCEMC_miss = new TH2F("h2ETVsFracCEMC_miss","Jet; Fraction of E_{T,Lead Jet} EMCal; E_{T,Lead Jet} [GeV]"
                                 , m_bins_frac, m_frac_low, m_frac_high, m_bins_ET, m_ET_low, m_ET_high);

  h2FracOHCalVsFracCEMC = new TH2F("h2FracOHCalVsFracCEMC","Jet; Fraction of E_{T,Lead Jet} EMCal; Fraction of E_{T,Lead Jet} OHCal"
                                   , m_bins_frac, m_frac_low, m_frac_high, m_bins_frac, m_frac_low, m_frac_high);

  h2FracOHCalVsFracCEMC_miss = new TH2F("h2FracOHCalVsFracCEMC_miss","Jet; Fraction of E_{T,Lead Jet} EMCal; Fraction of E_{T,Lead Jet} OHCal"
                                   , m_bins_frac, m_frac_low, m_frac_high, m_bins_frac, m_frac_low, m_frac_high);

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
  hEvents->Fill(m_status::ALL);

  // skip event if zvtx is too large
  if(fabs(m_zvtx) >= m_zvtx_max) {
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  hEvents->Fill(m_status::ZVTX30);

  m_triggeranalyzer->decodeTriggers(topNode);

  // skip event if it did not fire the desired trigger
  if(!m_triggeranalyzer->didTriggerFire(m_triggerBit)) {
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  vector<Int_t> triggerIdx;
  for(Int_t i = 0; i < m_triggerBits; ++i) {
    if(m_triggeranalyzer->didTriggerFire(i)) {
      triggerIdx.push_back(i);
    }
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

  Int_t nJets = 0;

  // trigger
  Bool_t hasBkg = false;
  for (auto jet : *jets_r04) {
    Float_t phi = jet->get_phi();
    Float_t eta = jet->get_eta();
    Float_t pt = jet->get_pt();
    Int_t constituents = jet->get_comp_vec().size();

    // exclude jets near the edge of the detector
    if(JetUtils::check_bad_jet_eta(eta, m_zvtx, m_R)) continue;
    ++nJets;

    hjetPhiEtaPt->Fill(phi, eta, pt);

    hjetConstituentsVsPt->Fill(pt, constituents);

    m_constituents_min = min(m_constituents_min, constituents);
    m_constituents_max = max(m_constituents_max, constituents);

    m_pt_min = min(m_pt_min, (Int_t)pt);
    m_pt_max = max(m_pt_max, (Int_t)pt);

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

  hNJetsVsLeadPt->Fill(jetPtLead, nJets);

  m_nJets_min = min(m_nJets_min, nJets);
  m_nJets_max = max(m_nJets_max, nJets);

  recoConsts* rc = recoConsts::instance();

  Bool_t failsLoEmJetCut = rc->get_IntFlag("failsLoEmJetCut");
  Bool_t failsHiEmJetCut = rc->get_IntFlag("failsHiEmJetCut");
  Bool_t failsIhJetCut   = rc->get_IntFlag("failsIhJetCut");
  Bool_t failsAnyJetCut  = rc->get_IntFlag("failsAnyJetCut");

  Bool_t isDijet   = rc->get_IntFlag("isDijet");
  Float_t frcem    = rc->get_FloatFlag("frcem");
  Float_t frcoh    = rc->get_FloatFlag("frcoh");
  Float_t maxJetET = rc->get_FloatFlag("maxJetET");
  Float_t dPhi     = rc->get_FloatFlag("dPhi");

  h2ETVsFracCEMC->Fill(frcem, maxJetET);

  // if event doesn't have a high pt jet then skip to the next event
  if(!hasBkg) return Fun4AllReturnCodes::ABORTEVENT;

  h2FracOHCalVsFracCEMC->Fill(frcem, frcoh);

  cout << "Background Jet: " << jetPtLead << " GeV, Event: " << m_globalEvent << endl;

  cout << "isDijet: " << isDijet
       << ", frcem: " << frcem
       << ", frcoh: " << frcoh
       << ", maxJetET: " << maxJetET
       << ", dPhi: " << dPhi << endl;

  hEvents->Fill(m_status::ZVTX30_BKG);

  if(failsLoEmJetCut) {
    hEvents->Fill(m_status::ZVTX30_BKG_failsLoEmJetCut);
  }
  if(failsHiEmJetCut) {
    hEvents->Fill(m_status::ZVTX30_BKG_failsHiEmJetCut);
  }
  if(failsIhJetCut) {
    hEvents->Fill(m_status::ZVTX30_BKG_failsIhJetCut);
  }
  if(!failsAnyJetCut) {
    hEvents->Fill(m_status::ZVTX30_BKG_failsNoneJetCut);
  }
  else {
    // okay to skip the event since the background jet is correctly flagged
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  h2ETVsFracCEMC_miss->Fill(frcem, maxJetET);
  h2FracOHCalVsFracCEMC_miss->Fill(frcem, frcoh);

  cout << "Background Jet Not Flagged!" << endl;

  // round nearest 0.1
  jetPhiLead    = (Int_t)(jetPhiLead*10)/10.;
  jetPhiSubLead = (Int_t)(jetPhiSubLead*10)/10.;

  jetEtaLead    = (Int_t)(jetEtaLead*10)/10.;
  jetEtaSubLead = (Int_t)(jetEtaSubLead*10)/10.;

  // round the bkg check variables
  frcem = (Int_t)(frcem*100)/100.;
  frcoh = (Int_t)(frcoh*100)/100.;
  dPhi  = (Int_t)(dPhi*1000)/1000.;

  // Get TowerInfoContainer
  TowerInfoContainer* towersCEMCBase = findNode::getClass<TowerInfoContainer>(topNode, m_emcTowerBaseNode.c_str());
  TowerInfoContainer* towersCEMC     = findNode::getClass<TowerInfoContainer>(topNode, m_emcTowerNode.c_str());
  TowerInfoContainer* towersIHCal    = findNode::getClass<TowerInfoContainer>(topNode, m_ihcalTowerNode.c_str());
  TowerInfoContainer* towersOHCal    = findNode::getClass<TowerInfoContainer>(topNode, m_ohcalTowerNode.c_str());

  if (!towersCEMCBase || !towersCEMC || !towersIHCal || !towersOHCal) {
    cout << PHWHERE << "JetValidation::process_event Could not find one of "
         << m_emcTowerBaseNode  << ", "
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

  nameSuffix << m_run << "_" << m_globalEvent << "_" << s_triggerIdx << "_" << (Int_t)m_zvtx << "_" << jetPtLead << "_" << jetPtSubLead
            << "_" << frcem << "_" << frcoh << "_" << (Int_t)maxJetET << "_" << dPhi << "_" << isDijet;

  name << "hCEMCBase_" << nameSuffix.str();
  title << "CEMC Base: " << titleSuffix.str();

  auto hCEMCBase_ = new TH2F(name.str().c_str(), title.str().c_str(), m_bins_phi_cemc, m_phi_low, m_phi_high, m_bins_eta_cemc, m_eta_low, m_eta_high);

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

  // loop over CEMC towers
  for(UInt_t towerIndex = 0; towerIndex < towersCEMCBase->size(); ++towerIndex) {
    UInt_t key  = TowerInfoDefs::encode_emcal(towerIndex);
    UInt_t iphi = TowerInfoDefs::getCaloTowerPhiBin(key);
    UInt_t ieta = TowerInfoDefs::getCaloTowerEtaBin(key);

    // map: [0, 127]  -> [128,255], [0,pi]
    // map: [128, 255] -> [0,127], [-pi,0]
    iphi = (iphi + m_bins_phi_cemc/2) % m_bins_phi_cemc;

    // EMCal
    TowerInfo* tower = towersCEMCBase->get_tower_at_channel(towerIndex);
    Float_t energy = (tower->get_isGood()) ? tower->get_energy() : 0;

    hCEMCBase_->SetBinContent(iphi+1, ieta+1, energy);
  }

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

    // HCal
    tower = towersIHCal->get_tower_at_channel(towerIndex);
    energy = (tower->get_isGood()) ? tower->get_energy() : 0;

    hIHCal_->SetBinContent(iphi+1, ieta+1, energy);

    tower = towersOHCal->get_tower_at_channel(towerIndex);
    energy = (tower->get_isGood()) ? tower->get_energy() : 0;

    hOHCal_->SetBinContent(iphi+1, ieta+1, energy);
  }

  hCEMCBase.push_back(hCEMCBase_);
  hCEMC.push_back(hCEMC_);
  hIHCal.push_back(hIHCal_);
  hOHCal.push_back(hOHCal_);

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
  cout << "Events Summary" << endl;
  for(UInt_t i = 0; i < m_eventStatus.size(); ++i) {
    cout << m_eventStatus[i] << ": " << hEvents->GetBinContent(i+1) << " Events" << endl;
  }
  cout << "=======================" << endl;
  cout << "Jets Summary" << endl;
  cout << "Constituents min: " << m_constituents_min << ", max: " << m_constituents_max << endl;
  cout << "nJets min: " << m_nJets_min << ", max: " << m_nJets_max << endl;
  cout << "Jet pT min: " << m_pt_min << " GeV, max: " << m_pt_max << " GeV" << endl;

  TFile output(m_outputFile.c_str(),"recreate");

  output.cd();

  output.mkdir("event");
  output.mkdir("zvtx");
  output.mkdir("jets");
  output.mkdir("bkg_checks");

  output.mkdir("CEMCBase");
  output.mkdir("CEMC");
  output.mkdir("IHCal");
  output.mkdir("OHCal");

  output.cd("event");
  hEvents->Write();

  output.cd("zvtx");
  hzvtxAll->Write();

  output.cd("jets");
  hjetPhiEtaPt->Write();
  hjetConstituentsVsPt->Write();
  hNJetsVsLeadPt->Write();

  output.cd("bkg_checks");
  h2ETVsFracCEMC->Write();
  h2FracOHCalVsFracCEMC->Write();
  h2ETVsFracCEMC_miss->Write();
  h2FracOHCalVsFracCEMC_miss->Write();

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

