// module for producing a TTree with jet information for doing jet validation studies
//  for questions/bugs please contact Virginia Bailey vbailey13@gsu.edu
#include <JetValidation.h>
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
// -- jet
#include <jetbase/JetContainer.h>
// -- DST node
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>
// -- root
#include <TFile.h>
#include <TTree.h>
// -- Tower stuff
#include <calobase/TowerInfo.h>
#include <calobase/RawTowerDefs.h>
#include <calobase/TowerInfoDefs.h>
#include <calobase/TowerInfoContainer.h>

using std::cout;
using std::endl;
using std::string;
using std::to_string;
using std::stringstream;

//____________________________________________________________________________..
JetValidation::JetValidation()
  : SubsysReco("JetValidation")
  , m_recoJetName_r02("AntiKt_Tower_r02_Sub1")
  , m_recoJetName_r04("AntiKt_Tower_r04_Sub1")
  , m_recoJetName_r06("AntiKt_Tower_r06_Sub1")
  , m_outputTreeFile(nullptr)
  , m_outputQAFile(nullptr)
  , m_outputTreeFileName("test.root")
  , m_outputQAFileName("qa.root")
  , m_emcTowerNodeBase("TOWERINFO_CALIB_CEMC")
  , m_emcTowerNode("TOWERINFO_CALIB_CEMC_RETOWER")
  , m_ihcalTowerNode("TOWERINFO_CALIB_HCALIN")
  , m_ohcalTowerNode("TOWERINFO_CALIB_HCALOUT")
  , m_emcTowerNodeSub("TOWERINFO_CALIB_CEMC_RETOWER_SUB1")
  , m_ihcalTowerNodeSub("TOWERINFO_CALIB_HCALIN_SUB1")
  , m_ohcalTowerNodeSub("TOWERINFO_CALIB_HCALOUT_SUB1")
  , m_saveTree(false)
  , m_lowPtThreshold(10) /*GeV*/
  , m_highPtThreshold(40) /*GeV*/
  , m_subLeadPtThreshold(5) /*GeV*/
  , m_highPtJetCtr(0)
  , m_bins_phi(64)
  , m_bins_eta(24)
  , m_eta_low(-1)
  , m_eta_high(1)
  , m_bins_pt(200)
  , m_pt_low(0) /*GeV*/
  , m_pt_high(200) /*GeV*/
  , m_bins_events(18)
  , m_bins_zvtx(400)
  , m_zvtx_low(-100)
  , m_zvtx_high(100)
  , m_bkg_tower_energy(0.6) /*GeV*/
  , m_bkg_tower_neighbor_energy(0.06) /*GeV*/
  , m_bkg_towers(6)
  , m_T(nullptr)
  , m_run(0)
  , m_globalEvent(0)
  , m_event(0)
  , m_nJets_r02(0)
  , m_nJets_r04(0)
  , m_nJets_r06(0)
  , m_nJet_r02(0)
  , m_id_r02()
  , m_nComponent_r02()
  , m_eta_r02()
  , m_phi_r02()
  , m_e_r02()
  , m_pt_r02()
  , m_nJet_r04(0)
  , m_id_r04()
  , m_nComponent_r04()
  , m_eta_r04()
  , m_phi_r04()
  , m_e_r04()
  , m_pt_r04()
  , m_nJet_r06(0)
  , m_id_r06()
  , m_nComponent_r06()
  , m_eta_r06()
  , m_phi_r06()
  , m_e_r06()
  , m_pt_r06()
{
  cout << "JetValidation::JetValidation(const std::string &name) Calling ctor" << endl;
}

//____________________________________________________________________________..
JetValidation::~JetValidation()
{
  cout << "JetValidation::~JetValidation() Calling dtor" << endl;
}

//____________________________________________________________________________..
Int_t JetValidation::Init(PHCompositeNode *topNode)
{
  cout << "JetValidation::Init(PHCompositeNode *topNode) Initializing" << endl;

  if(m_saveTree) {
    m_outputTreeFile = new TFile(m_outputTreeFileName.c_str(),"RECREATE");
    cout << "JetValidation::Init - Output to " << m_outputTreeFileName << endl;

    // configure Tree
    m_T = new TTree("T", "T");
    m_T->Branch("event", &m_globalEvent, "event/I");
    m_T->Branch("run", &m_run, "run/I");
    m_T->Branch("zvtx", &m_zvtx);
    m_T->Branch("scaledVector", &m_scaledVector);

    m_T->Branch("towersCEMCBase_isGood", &m_towersCEMCBase_isGood);
    m_T->Branch("towersCEMCBase_energy", &m_towersCEMCBase_energy);
    m_T->Branch("towersCEMCBase_time",   &m_towersCEMCBase_time);

    m_T->Branch("towersCEMC_isGood",  &m_towersCEMC_isGood);
    m_T->Branch("towersIHCal_isGood", &m_towersIHCal_isGood);
    m_T->Branch("towersOHCal_isGood", &m_towersOHCal_isGood);

    m_T->Branch("towersCEMC_energy",  &m_towersCEMC_energy);
    m_T->Branch("towersIHCal_energy", &m_towersIHCal_energy);
    m_T->Branch("towersOHCal_energy", &m_towersOHCal_energy);

    m_T->Branch("towersCEMCSub_energy",  &m_towersCEMCSub_energy);
    m_T->Branch("towersIHCalSub_energy", &m_towersIHCalSub_energy);
    m_T->Branch("towersOHCalSub_energy", &m_towersOHCalSub_energy);

    m_T->Branch("nJet_r02", &m_nJet_r02, "nJet_r02/I");
    m_T->Branch("nComponent_r02", &m_nComponent_r02);
    m_T->Branch("id_r02", &m_id_r02);
    m_T->Branch("eta_r02", &m_eta_r02);
    m_T->Branch("phi_r02", &m_phi_r02);
    m_T->Branch("e_r02", &m_e_r02);
    m_T->Branch("pt_r02", &m_pt_r02);

    m_T->Branch("nJet_r04", &m_nJet_r04, "nJet_r04/I");
    m_T->Branch("nComponent_r04", &m_nComponent_r04);
    m_T->Branch("id_r04", &m_id_r04);
    m_T->Branch("eta_r04", &m_eta_r04);
    m_T->Branch("phi_r04", &m_phi_r04);
    m_T->Branch("e_r04", &m_e_r04);
    m_T->Branch("pt_r04", &m_pt_r04);

    m_T->Branch("nJet_r06", &m_nJet_r06, "nJet_r06/I");
    m_T->Branch("nComponent_r04", &m_nComponent_r04);
    m_T->Branch("id_r06", &m_id_r06);
    m_T->Branch("eta_r06", &m_eta_r06);
    m_T->Branch("phi_r06", &m_phi_r06);
    m_T->Branch("e_r06", &m_e_r06);
    m_T->Branch("pt_r06", &m_pt_r06);
  }

  m_outputQAFile = new TFile(m_outputQAFileName.c_str(),"RECREATE");
  cout << "JetValidation::Init - Output to " << m_outputQAFileName << endl;

  for(auto suffix : JetEvent_Status_vec) {
    // pt
    hJetPt_r02.push_back(new TH1F(("hJetPt_r02_"+suffix).c_str(), "Jet: R = 0.2; p_{T} [GeV]; Counts", m_bins_pt, m_pt_low, m_pt_high));
    hJetPt_r04.push_back(new TH1F(("hJetPt_r04_"+suffix).c_str(), "Jet: R = 0.4; p_{T} [GeV]; Counts", m_bins_pt, m_pt_low, m_pt_high));
    hJetPt_r06.push_back(new TH1F(("hJetPt_r06_"+suffix).c_str(), "Jet: R = 0.6; p_{T} [GeV]; Counts", m_bins_pt, m_pt_low, m_pt_high));

    hJetPt_r02_bkg.push_back(new TH1F(("hJetPt_r02_"+suffix+"_bkg").c_str(), "Jet: R = 0.2; p_{T} [GeV]; Counts", m_bins_pt, m_pt_low, m_pt_high));
    hJetPt_r04_bkg.push_back(new TH1F(("hJetPt_r04_"+suffix+"_bkg").c_str(), "Jet: R = 0.4; p_{T} [GeV]; Counts", m_bins_pt, m_pt_low, m_pt_high));
    hJetPt_r06_bkg.push_back(new TH1F(("hJetPt_r06_"+suffix+"_bkg").c_str(), "Jet: R = 0.6; p_{T} [GeV]; Counts", m_bins_pt, m_pt_low, m_pt_high));

    hJetPt_r02_nobkg.push_back(new TH1F(("hJetPt_r02_"+suffix+"_nobkg").c_str(), "Jet: R = 0.2; p_{T} [GeV]; Counts", m_bins_pt, m_pt_low, m_pt_high));
    hJetPt_r04_nobkg.push_back(new TH1F(("hJetPt_r04_"+suffix+"_nobkg").c_str(), "Jet: R = 0.4; p_{T} [GeV]; Counts", m_bins_pt, m_pt_low, m_pt_high));
    hJetPt_r06_nobkg.push_back(new TH1F(("hJetPt_r06_"+suffix+"_nobkg").c_str(), "Jet: R = 0.6; p_{T} [GeV]; Counts", m_bins_pt, m_pt_low, m_pt_high));

    // deltaPhi
    hJetDeltaPhi_r02.push_back(new TH1F(("hJetDeltaPhi_r02_"+suffix).c_str(), "Jet: R = 0.2; |#Delta #phi|; Counts", m_bins_phi, 0, M_PI));
    hJetDeltaPhi_r04.push_back(new TH1F(("hJetDeltaPhi_r04_"+suffix).c_str(), "Jet: R = 0.4; |#Delta #phi|; Counts", m_bins_phi, 0, M_PI));
    hJetDeltaPhi_r06.push_back(new TH1F(("hJetDeltaPhi_r06_"+suffix).c_str(), "Jet: R = 0.6; |#Delta #phi|; Counts", m_bins_phi, 0, M_PI));

    hJetDeltaPhi_r02_bkg.push_back(new TH1F(("hJetDeltaPhi_r02_"+suffix+"_bkg").c_str(), "Jet: R = 0.2; |#Delta #phi|; Counts", m_bins_phi, 0, M_PI));
    hJetDeltaPhi_r04_bkg.push_back(new TH1F(("hJetDeltaPhi_r04_"+suffix+"_bkg").c_str(), "Jet: R = 0.4; |#Delta #phi|; Counts", m_bins_phi, 0, M_PI));
    hJetDeltaPhi_r06_bkg.push_back(new TH1F(("hJetDeltaPhi_r06_"+suffix+"_bkg").c_str(), "Jet: R = 0.6; |#Delta #phi|; Counts", m_bins_phi, 0, M_PI));

    hJetDeltaPhi_r02_nobkg.push_back(new TH1F(("hJetDeltaPhi_r02_"+suffix+"_nobkg").c_str(), "Jet: R = 0.2; |#Delta #phi|; Counts", m_bins_phi, 0, M_PI));
    hJetDeltaPhi_r04_nobkg.push_back(new TH1F(("hJetDeltaPhi_r04_"+suffix+"_nobkg").c_str(), "Jet: R = 0.4; |#Delta #phi|; Counts", m_bins_phi, 0, M_PI));
    hJetDeltaPhi_r06_nobkg.push_back(new TH1F(("hJetDeltaPhi_r06_"+suffix+"_nobkg").c_str(), "Jet: R = 0.6; |#Delta #phi|; Counts", m_bins_phi, 0, M_PI));

    // eta_phi
    h2JetEtaPhi_r02.push_back(new TH2F(("h2JetEtaPhi_r02_"+suffix).c_str(), "Jet: R = 0.2; #phi; #eta", m_bins_phi, -M_PI, M_PI, m_bins_eta, m_eta_low, m_eta_high));
    h2JetEtaPhi_r04.push_back(new TH2F(("h2JetEtaPhi_r04_"+suffix).c_str(), "Jet: R = 0.4; #phi; #eta", m_bins_phi, -M_PI, M_PI, m_bins_eta, m_eta_low, m_eta_high));
    h2JetEtaPhi_r06.push_back(new TH2F(("h2JetEtaPhi_r06_"+suffix).c_str(), "Jet: R = 0.6; #phi; #eta", m_bins_phi, -M_PI, M_PI, m_bins_eta, m_eta_low, m_eta_high));

    h2JetEtaPhi_r02_bkg.push_back(new TH2F(("h2JetEtaPhi_r02_"+suffix+"_bkg").c_str(), "Jet: R = 0.2; #phi; #eta", m_bins_phi, -M_PI, M_PI, m_bins_eta, m_eta_low, m_eta_high));
    h2JetEtaPhi_r04_bkg.push_back(new TH2F(("h2JetEtaPhi_r04_"+suffix+"_bkg").c_str(), "Jet: R = 0.4; #phi; #eta", m_bins_phi, -M_PI, M_PI, m_bins_eta, m_eta_low, m_eta_high));
    h2JetEtaPhi_r06_bkg.push_back(new TH2F(("h2JetEtaPhi_r06_"+suffix+"_bkg").c_str(), "Jet: R = 0.6; #phi; #eta", m_bins_phi, -M_PI, M_PI, m_bins_eta, m_eta_low, m_eta_high));

    h2JetEtaPhi_r02_nobkg.push_back(new TH2F(("h2JetEtaPhi_r02_"+suffix+"_nobkg").c_str(), "Jet: R = 0.2; #phi; #eta", m_bins_phi, -M_PI, M_PI, m_bins_eta, m_eta_low, m_eta_high));
    h2JetEtaPhi_r04_nobkg.push_back(new TH2F(("h2JetEtaPhi_r04_"+suffix+"_nobkg").c_str(), "Jet: R = 0.4; #phi; #eta", m_bins_phi, -M_PI, M_PI, m_bins_eta, m_eta_low, m_eta_high));
    h2JetEtaPhi_r06_nobkg.push_back(new TH2F(("h2JetEtaPhi_r06_"+suffix+"_nobkg").c_str(), "Jet: R = 0.6; #phi; #eta", m_bins_phi, -M_PI, M_PI, m_bins_eta, m_eta_low, m_eta_high));
  }

  hEvents    = new TH1F("hEvents","Events; Status; Counts", m_bins_events, 0, m_bins_events);
  hEventsBkg = new TH1F("hEventsBkg","Background Events; Status; Counts", m_bins_events, 0, m_bins_events);
  hZVtx      = new TH1F("hZVtx","Z Vertex; z [cm]; Counts", m_bins_zvtx, m_zvtx_low, m_zvtx_high);

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
Int_t JetValidation::process_event(PHCompositeNode *topNode)
{
  EventHeader* eventInfo = findNode::getClass<EventHeader>(topNode,"EventHeader");
  if(!eventInfo)
  {
    cout << PHWHERE << "JetValidation::process_event - Fatal Error - EventHeader node is missing. " << endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  m_globalEvent = eventInfo->get_EvtSequence();
  m_run         = eventInfo->get_RunNumber();

  if (m_event % 20 == 0) cout << "Progress: " << m_event << ", Global: " << m_globalEvent << endl;
  ++m_event;

  // zvertex
  GlobalVertexMap* vertexmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");
  if (!vertexmap) {
    cout << "JetValidation::process_event - Error can not find global vertex node " << endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }
  if(!vertexmap->empty()) {
    GlobalVertex* vtx = vertexmap->begin()->second;
    m_zvtx = vtx->get_z();
  }
  else m_zvtx = -9999;

  hZVtx->Fill(m_zvtx);

  // grab the gl1 data
  Gl1Packet* gl1PacketInfo = findNode::getClass<Gl1Packet>(topNode, "GL1Packet");
  if (!gl1PacketInfo)
  {
    cout << PHWHERE << "JetValidation::process_event: GL1Packet node is missing" << endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  uint64_t scaledvec = gl1PacketInfo->getScaledVector();
  // uint64_t scaledvec = gl1PacketInfo->getTriggerVector();
  for (Int_t i = 0; i < 64; ++i)
  {
    Bool_t trig_decision = ((scaledvec & 0x1U) == 0x1U);
    m_scaledVector.push_back(trig_decision);
    scaledvec = (scaledvec >> 1U) & 0xffffffffU;
  }

  // Get TowerInfoContainer
  // Base EMCal Towers
  TowerInfoContainer* towersCEMCBase = findNode::getClass<TowerInfoContainer>(topNode, m_emcTowerNodeBase.c_str());

  // unsubtracted
  TowerInfoContainer* towersCEMC  = findNode::getClass<TowerInfoContainer>(topNode, m_emcTowerNode.c_str());
  TowerInfoContainer* towersIHCal = findNode::getClass<TowerInfoContainer>(topNode, m_ihcalTowerNode.c_str());
  TowerInfoContainer* towersOHCal = findNode::getClass<TowerInfoContainer>(topNode, m_ohcalTowerNode.c_str());

  // subtracted
  TowerInfoContainer* towersCEMCSub  = findNode::getClass<TowerInfoContainer>(topNode, m_emcTowerNodeSub.c_str());
  TowerInfoContainer* towersIHCalSub = findNode::getClass<TowerInfoContainer>(topNode, m_ihcalTowerNodeSub.c_str());
  TowerInfoContainer* towersOHCalSub = findNode::getClass<TowerInfoContainer>(topNode, m_ohcalTowerNodeSub.c_str());

  if (!towersCEMCBase || !towersCEMC    || !towersIHCal    || !towersOHCal    ||
      !towersCEMCSub  || !towersIHCalSub || !towersOHCalSub) {
    cout << PHWHERE << "JetValidation::process_event Could not find one of "
         << m_emcTowerNodeBase  << ", "
         << m_emcTowerNode      << ", "
         << m_ihcalTowerNode    << ", "
         << m_ohcalTowerNode    << ", "
         << m_emcTowerNodeSub   << ", "
         << m_ihcalTowerNodeSub << ", "
         << m_ohcalTowerNodeSub
         << endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  std::vector<Float_t> towerEnergy(towersCEMC->size());
  std::vector<Float_t> towerEnergySub(towersCEMC->size());

  // loop over towers
  for(UInt_t towerIndex = 0; towerIndex < towersCEMC->size(); ++towerIndex) {
    // unsubtracted
    TowerInfo* tower = towersCEMC->get_tower_at_channel(towerIndex);

    Float_t energy = (tower->get_isGood()) ? tower->get_energy() : 0;

    m_towersCEMC_energy.push_back(tower->get_energy());
    m_towersCEMC_isGood.push_back(tower->get_isGood());

    tower = towersIHCal->get_tower_at_channel(towerIndex);
    m_towersIHCal_energy.push_back(tower->get_energy());
    m_towersIHCal_isGood.push_back(tower->get_isGood());

    energy += (tower->get_isGood()) ? tower->get_energy() : 0;

    tower = towersOHCal->get_tower_at_channel(towerIndex);
    m_towersOHCal_energy.push_back(tower->get_energy());
    m_towersOHCal_isGood.push_back(tower->get_isGood());

    energy += (tower->get_isGood()) ? tower->get_energy() : 0;

    towerEnergy[towerIndex] = energy;

    // subtracted
    tower = towersCEMCSub->get_tower_at_channel(towerIndex);
    m_towersCEMCSub_energy.push_back(tower->get_energy());

    energy = (tower->get_isGood()) ? tower->get_energy() : 0;

    tower = towersIHCalSub->get_tower_at_channel(towerIndex);
    m_towersIHCalSub_energy.push_back(tower->get_energy());

    energy += (tower->get_isGood()) ? tower->get_energy() : 0;

    tower = towersOHCalSub->get_tower_at_channel(towerIndex);
    m_towersOHCalSub_energy.push_back(tower->get_energy());

    energy += (tower->get_isGood()) ? tower->get_energy() : 0;

    towerEnergySub[towerIndex] = energy;
  }

  Bool_t m_hasBkg = isBackgroundEvent(towerEnergy);

  hEvents->Fill((Int_t)EventStatus::ALL);
  if(m_hasBkg) {
    hEventsBkg->Fill((Int_t)EventStatus::ALL);
  }

  if(m_zvtx != -9999) {
    hEvents->Fill((Int_t)EventStatus::ZVTX);
    if(m_hasBkg) {
      hEventsBkg->Fill((Int_t)EventStatus::ZVTX);
    }
  }
  if(fabs(m_zvtx) < m_zvtx_max[0]) {
    hEvents->Fill((Int_t)EventStatus::ZVTX60);
    if(m_hasBkg) {
      hEventsBkg->Fill((Int_t)EventStatus::ZVTX60);
    }
  }
  if(fabs(m_zvtx) < m_zvtx_max[1]) {
    hEvents->Fill((Int_t)EventStatus::ZVTX50);
    if(m_hasBkg) {
      hEventsBkg->Fill((Int_t)EventStatus::ZVTX50);
    }
  }
  if(fabs(m_zvtx) < m_zvtx_max[2]) {
    hEvents->Fill((Int_t)EventStatus::ZVTX30);
    if(m_hasBkg) {
      hEventsBkg->Fill((Int_t)EventStatus::ZVTX30);
    }
  }
  if(fabs(m_zvtx) < m_zvtx_max[3]) {
    hEvents->Fill((Int_t)EventStatus::ZVTX20);
    if(m_hasBkg) {
      hEventsBkg->Fill((Int_t)EventStatus::ZVTX20);
    }
  }
  if(fabs(m_zvtx) < m_zvtx_max[4]) {
    hEvents->Fill((Int_t)EventStatus::ZVTX10);
    if(m_hasBkg) {
      hEventsBkg->Fill((Int_t)EventStatus::ZVTX10);
    }
  }

  std::vector<Bool_t> eventTrig_status(JetEvent_Status_vec.size());

  // Fill Event Status
  if(m_scaledVector[(Int_t)Trigger::MBD_NS_1]) {
    // ALL
    hEvents->Fill((Int_t)EventStatus::ALL_MBDNS1);
    eventTrig_status[(Int_t)JetEvent_Status::ALL_MBDNS1] = true;
    if(m_hasBkg) {
      hEventsBkg->Fill((Int_t)EventStatus::ALL_MBDNS1);
    }

    // ZVTX60
    if(fabs(m_zvtx) < m_zvtx_max[0]) {
      hEvents->Fill((Int_t)EventStatus::ZVTX60_MBDNS1);
      eventTrig_status[(Int_t)JetEvent_Status::ZVTX60_MBDNS1] = true;
      if(m_hasBkg) {
        hEventsBkg->Fill((Int_t)EventStatus::ZVTX60_MBDNS1);
      }
    }

    // Jet 8 GeV
    if(m_scaledVector[(Int_t)Trigger::JET_8]) {
      hEvents->Fill((Int_t)EventStatus::ALL_MBDNS1_JET8);
      eventTrig_status[(Int_t)JetEvent_Status::ALL_MBDNS1_JET8] = true;

      if(m_hasBkg) {
        hEventsBkg->Fill((Int_t)EventStatus::ALL_MBDNS1_JET8);
      }

      if(fabs(m_zvtx) < m_zvtx_max[0]) {
        hEvents->Fill((Int_t)EventStatus::ZVTX60_MBDNS1_JET8);
        eventTrig_status[(Int_t)JetEvent_Status::ZVTX60_MBDNS1_JET8] = true;

        if(m_hasBkg) {
          hEventsBkg->Fill((Int_t)EventStatus::ZVTX60_MBDNS1_JET8);
        }
      }
    }

    // Jet 10 GeV
    if(m_scaledVector[(Int_t)Trigger::JET_10]) {
      hEvents->Fill((Int_t)EventStatus::ALL_MBDNS1_JET10);
      eventTrig_status[(Int_t)JetEvent_Status::ALL_MBDNS1_JET10] = true;

      if(m_hasBkg) {
        hEventsBkg->Fill((Int_t)EventStatus::ALL_MBDNS1_JET10);
      }

      if(fabs(m_zvtx) < m_zvtx_max[0]) {
        hEvents->Fill((Int_t)EventStatus::ZVTX60_MBDNS1_JET10);
        eventTrig_status[(Int_t)JetEvent_Status::ZVTX60_MBDNS1_JET10] = true;

        if(m_hasBkg) {
          hEventsBkg->Fill((Int_t)EventStatus::ZVTX60_MBDNS1_JET10);
        }
      }
    }

    // Jet 12 GeV
    if(m_scaledVector[(Int_t)Trigger::JET_12]) {
      hEvents->Fill((Int_t)EventStatus::ALL_MBDNS1_JET12);
      eventTrig_status[(Int_t)JetEvent_Status::ALL_MBDNS1_JET12] = true;

      if(m_hasBkg) {
        hEventsBkg->Fill((Int_t)EventStatus::ALL_MBDNS1_JET12);
      }

      if(fabs(m_zvtx) < m_zvtx_max[0]) {
        hEvents->Fill((Int_t)EventStatus::ZVTX60_MBDNS1_JET12);
        eventTrig_status[(Int_t)JetEvent_Status::ZVTX60_MBDNS1_JET12] = true;

        if(m_hasBkg) {
          hEventsBkg->Fill((Int_t)EventStatus::ZVTX60_MBDNS1_JET12);
        }
      }
    }
  }

  if(fabs(m_zvtx) < m_zvtx_max[0]) {
    eventTrig_status[(Int_t)JetEvent_Status::ZVTX60] = true;

    // Jet 8 GeV
    if(m_scaledVector[(Int_t)Trigger::JET_8]) {
      eventTrig_status[(Int_t)JetEvent_Status::ZVTX60_JET8] = true;
      hEvents->Fill((Int_t)EventStatus::ZVTX60_JET8);

      if(m_hasBkg) {
        hEventsBkg->Fill((Int_t)EventStatus::ZVTX60_JET8);
      }
    }

    // Jet 10 GeV
    if(m_scaledVector[(Int_t)Trigger::JET_10]) {
      eventTrig_status[(Int_t)JetEvent_Status::ZVTX60_JET10] = true;
      hEvents->Fill((Int_t)EventStatus::ZVTX60_JET10);

      if(m_hasBkg) {
        hEventsBkg->Fill((Int_t)EventStatus::ZVTX60_JET10);
      }
    }

    // Jet 12 GeV
    if(m_scaledVector[(Int_t)Trigger::JET_12]) {
      eventTrig_status[(Int_t)JetEvent_Status::ZVTX60_JET12] = true;
      hEvents->Fill((Int_t)EventStatus::ZVTX60_JET12);

      if(m_hasBkg) {
        hEventsBkg->Fill((Int_t)EventStatus::ZVTX60_JET12);
      }
    }
  }

  // interface to reco jets
  JetContainer* jets_r02 = findNode::getClass<JetContainer>(topNode, m_recoJetName_r02);
  JetContainer* jets_r04 = findNode::getClass<JetContainer>(topNode, m_recoJetName_r04);
  JetContainer* jets_r06 = findNode::getClass<JetContainer>(topNode, m_recoJetName_r06);
  if (!jets_r02 || !jets_r04 || !jets_r06) {
    cout << "JetValidation::process_event - Error can not find one of DST Reco JetContainer node "
         << m_recoJetName_r02 << ", "
         << m_recoJetName_r04 << ", "
         << m_recoJetName_r06 << ", " << endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  // get reco jets
  m_nJet_r02 = 0;
  m_nJet_r04 = 0;
  m_nJet_r06 = 0;

  // R = 0.2
  for (auto jet : *jets_r02) {
    Float_t jet_pt = jet->get_pt();
    if (jet_pt < m_lowPtThreshold || fabs(jet->get_eta()) > m_eta_high-0.2) continue;  // to remove noise jets

    m_id_r02.push_back(jet->get_id());
    m_nComponent_r02.push_back(jet->size_comp());
    m_eta_r02.push_back(jet->get_eta());
    m_phi_r02.push_back(jet->get_phi());
    m_e_r02.push_back(jet->get_e());
    m_pt_r02.push_back(jet_pt);

    for (UInt_t i = 0; i < eventTrig_status.size(); ++i) {
      if (eventTrig_status[i]) {
        hJetPt_r02[i]->Fill(jet_pt);
        if(m_hasBkg) hJetPt_r02_bkg[i]->Fill(jet_pt);
        else         hJetPt_r02_nobkg[i]->Fill(jet_pt);
      }
    }

    ++m_nJet_r02;
  }

  Bool_t hasHighPtJet = false;
  Float_t leadPt = 0;
  Float_t subLeadPt = 0;
  Float_t leadPhi = 0;
  Float_t subLeadPhi = 0;
  // R = 0.4
  for (auto jet : *jets_r04) {

    if (fabs(jet->get_eta()) > m_eta_high-0.4) continue;  // to remove noise jets

    Float_t jet_pt = jet->get_pt();

    if(jet_pt > subLeadPt) {
        if(jet_pt > leadPt) {
            subLeadPt = leadPt;
            subLeadPhi = leadPhi;
            leadPt = jet_pt;
            leadPhi = jet->get_phi();
        }
        else {
          subLeadPt = jet_pt;
          subLeadPhi = jet->get_phi();
        }
    }

    if (jet_pt < m_lowPtThreshold) continue;  // to remove noise jets

    m_id_r04.push_back(jet->get_id());
    m_nComponent_r04.push_back(jet->size_comp());
    m_eta_r04.push_back(jet->get_eta());
    m_phi_r04.push_back(jet->get_phi());
    m_e_r04.push_back(jet->get_e());
    m_pt_r04.push_back(jet_pt);

    for (UInt_t i = 0; i < eventTrig_status.size(); ++i) {
      if (eventTrig_status[i]) {
        hJetPt_r04[i]->Fill(jet_pt);
        h2JetEtaPhi_r04[i]->Fill(jet->get_phi(), jet->get_eta());
        if(m_hasBkg) {
          hJetPt_r04_bkg[i]->Fill(jet_pt);
          h2JetEtaPhi_r04_bkg[i]->Fill(jet->get_phi(), jet->get_eta());
        }
        else{
          hJetPt_r04_nobkg[i]->Fill(jet_pt);
          h2JetEtaPhi_r04_nobkg[i]->Fill(jet->get_phi(), jet->get_eta());
        }
      }
    }

    if(jet_pt >= m_highPtThreshold) {
      hasHighPtJet = true;
      ++m_highPtJetCtr;
    }

    ++m_nJet_r04;
  }

  if(subLeadPt >= m_subLeadPtThreshold && leadPt >= m_lowPtThreshold) {
    // ensure that |leadPhi - subLeadPhi|  <= pi
    Float_t deltaPhi = (fabs(leadPhi-subLeadPhi) > M_PI) ? 2*M_PI-fabs(leadPhi-subLeadPhi) : fabs(leadPhi-subLeadPhi);
    for (UInt_t i = 0; i < eventTrig_status.size(); ++i) {
      if (eventTrig_status[i]) {
        hJetDeltaPhi_r04[i]->Fill(deltaPhi);
        if(m_hasBkg) hJetDeltaPhi_r04_bkg[i]->Fill(deltaPhi);
        else         hJetDeltaPhi_r04_nobkg[i]->Fill(deltaPhi);
      }
    }
  }

  if(hasHighPtJet && !m_hasBkg) {
    cout << "High Pt Jet Found, Event: " << m_globalEvent << ", Jet Pt: " << (Int_t)leadPt << " GeV" << endl;

    stringstream name;
    stringstream title;

    name << "h2TowerEnergy_" << m_run << "_" << m_globalEvent << "_" << (Int_t)leadPt;
    title << "Tower Energy, Run: " << m_run << ", Event: " << m_globalEvent << ", Z: " << (Int_t)(m_zvtx*10)/10. << " cm, Jet p_{T} = " <<  (Int_t)leadPt << " GeV; Tower Index #phi; Tower Index #eta";

    auto h2 = new TH2F(name.str().c_str(), title.str().c_str(), m_bins_phi, -0.5, m_bins_phi-0.5, m_bins_eta, -0.5, m_bins_eta-0.5);

    name.str("");
    title.str("");

    name << "h2TowerEnergySub_" << m_run << "_" << m_globalEvent << "_" << (Int_t)leadPt;
    title << "Tower Energy, Run: " << m_run << ", Event: " << m_globalEvent << ", Z: " << (Int_t)(m_zvtx*10)/10. << " cm, Jet p_{T} = " <<  (Int_t)leadPt << " GeV; Tower Index #phi; Tower Index #eta";

    auto h2Sub = new TH2F(name.str().c_str(), title.str().c_str(), m_bins_phi, -0.5, m_bins_phi-0.5, m_bins_eta, -0.5, m_bins_eta-0.5);

    for(UInt_t towerIndex = 0; towerIndex < towersCEMC->size(); ++towerIndex) {
      UInt_t key       = TowerInfoDefs::encode_hcal(towerIndex);
      UInt_t iphi      = TowerInfoDefs::getCaloTowerPhiBin(key);
      UInt_t ieta      = TowerInfoDefs::getCaloTowerEtaBin(key);

      h2->SetBinContent(iphi+1,ieta+1, towerEnergy[towerIndex]);
      h2Sub->SetBinContent(iphi+1,ieta+1, towerEnergySub[towerIndex]);
    }

    h2TowerEnergy.push_back(h2);
    h2TowerEnergySub.push_back(h2Sub);
  }

  if(m_saveTree && hasHighPtJet) {
    // loop over base towers
    for(UInt_t towerIndex = 0; towerIndex < towersCEMCBase->size(); ++towerIndex) {
      TowerInfo* tower = towersCEMCBase->get_tower_at_channel(towerIndex);
      m_towersCEMCBase_isGood.push_back(tower->get_isGood());
      m_towersCEMCBase_time.push_back(tower->get_time_float());
      m_towersCEMCBase_energy.push_back(tower->get_energy());
    }
  }

  // R = 0.6
  for (auto jet : *jets_r06) {
    Float_t jet_pt = jet->get_pt();
    if (jet_pt < m_lowPtThreshold || fabs(jet->get_eta()) > m_eta_high-0.6) continue;  // to remove noise jets

    m_id_r06.push_back(jet->get_id());
    m_nComponent_r06.push_back(jet->size_comp());
    m_eta_r06.push_back(jet->get_eta());
    m_phi_r06.push_back(jet->get_phi());
    m_e_r06.push_back(jet->get_e());
    m_pt_r06.push_back(jet_pt);

    for (UInt_t i = 0; i < eventTrig_status.size(); ++i) {
      if (eventTrig_status[i]) {
        hJetPt_r06[i]->Fill(jet_pt);
        if(m_hasBkg) hJetPt_r06_bkg[i]->Fill(jet_pt);
        else         hJetPt_r06_nobkg[i]->Fill(jet_pt);
      }
    }

    ++m_nJet_r06;
  }

  if(m_saveTree) {
    // fill the tree
    m_T->Fill();
  }

  m_nJets_r02 += m_nJet_r02;
  m_nJets_r04 += m_nJet_r04;
  m_nJets_r06 += m_nJet_r06;

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
Int_t JetValidation::ResetEvent(PHCompositeNode *topNode)
{
  m_id_r02.clear();
  m_nComponent_r02.clear();
  m_eta_r02.clear();
  m_phi_r02.clear();
  m_e_r02.clear();
  m_pt_r02.clear();

  m_id_r04.clear();
  m_nComponent_r04.clear();
  m_eta_r04.clear();
  m_phi_r04.clear();
  m_e_r04.clear();
  m_pt_r04.clear();

  m_id_r06.clear();
  m_nComponent_r06.clear();
  m_eta_r06.clear();
  m_phi_r06.clear();
  m_e_r06.clear();
  m_pt_r06.clear();

  m_scaledVector.clear();

  m_towersCEMCBase_isGood.clear();
  m_towersCEMCBase_energy.clear();
  m_towersCEMCBase_time.clear();

  m_towersCEMC_isGood.clear();
  m_towersIHCal_isGood.clear();
  m_towersOHCal_isGood.clear();

  m_towersCEMC_energy.clear();
  m_towersIHCal_energy.clear();
  m_towersOHCal_energy.clear();

  m_towersCEMCSub_energy.clear();
  m_towersIHCalSub_energy.clear();
  m_towersOHCalSub_energy.clear();

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
Int_t JetValidation::End(PHCompositeNode *topNode)
{
  cout << "Events" << endl;
  cout << "All: " << hEvents->GetBinContent((Int_t)EventStatus::ALL+1) << endl;
  cout << endl;

  cout << "ZVTX: " << hEvents->GetBinContent((Int_t)EventStatus::ZVTX+1) << ", " << hEvents->GetBinContent((Int_t)EventStatus::ZVTX+1)*100./hEvents->GetBinContent((Int_t)EventStatus::ALL+1) << " %" << endl;
  cout << "ZVTX60: " << hEvents->GetBinContent((Int_t)EventStatus::ZVTX60+1) << ", " << hEvents->GetBinContent((Int_t)EventStatus::ZVTX60+1)*100./hEvents->GetBinContent((Int_t)EventStatus::ZVTX+1) << " %" << endl;
  cout << "ZVTX50: " << hEvents->GetBinContent((Int_t)EventStatus::ZVTX50+1) << ", " << hEvents->GetBinContent((Int_t)EventStatus::ZVTX50+1)*100./hEvents->GetBinContent((Int_t)EventStatus::ZVTX+1) << " %" << endl;
  cout << "ZVTX30: " << hEvents->GetBinContent((Int_t)EventStatus::ZVTX30+1) << ", " << hEvents->GetBinContent((Int_t)EventStatus::ZVTX30+1)*100./hEvents->GetBinContent((Int_t)EventStatus::ZVTX+1) << " %" << endl;
  cout << "ZVTX20: " << hEvents->GetBinContent((Int_t)EventStatus::ZVTX20+1) << ", " << hEvents->GetBinContent((Int_t)EventStatus::ZVTX20+1)*100./hEvents->GetBinContent((Int_t)EventStatus::ZVTX+1) << " %" << endl;
  cout << "ZVTX10: " << hEvents->GetBinContent((Int_t)EventStatus::ZVTX10+1) << ", " << hEvents->GetBinContent((Int_t)EventStatus::ZVTX10+1)*100./hEvents->GetBinContent((Int_t)EventStatus::ZVTX+1) << " %" << endl;
  cout << endl;

  cout << "ALL_MBDNS1: " << hEvents->GetBinContent((Int_t)EventStatus::ALL_MBDNS1+1) << ", " << hEvents->GetBinContent((Int_t)EventStatus::ALL_MBDNS1+1)*100./hEvents->GetBinContent((Int_t)EventStatus::ALL+1) << " %" << endl;
  cout << "ALL_MBDNS1_JET8: " << hEvents->GetBinContent((Int_t)EventStatus::ALL_MBDNS1_JET8+1) << ", " << hEvents->GetBinContent((Int_t)EventStatus::ALL_MBDNS1_JET8+1)*100./hEvents->GetBinContent((Int_t)EventStatus::ALL_MBDNS1+1) << " %" << endl;
  cout << "ALL_MBDNS1_JET10: " << hEvents->GetBinContent((Int_t)EventStatus::ALL_MBDNS1_JET10+1) << ", " << hEvents->GetBinContent((Int_t)EventStatus::ALL_MBDNS1_JET10+1)*100./hEvents->GetBinContent((Int_t)EventStatus::ALL_MBDNS1+1) << " %" << endl;
  cout << "ALL_MBDNS1_JET12: " << hEvents->GetBinContent((Int_t)EventStatus::ALL_MBDNS1_JET12+1) << ", " << hEvents->GetBinContent((Int_t)EventStatus::ALL_MBDNS1_JET12+1)*100./hEvents->GetBinContent((Int_t)EventStatus::ALL_MBDNS1+1) << " %" << endl;
  cout << endl;

  cout << "ZVTX60_MBDNS1: " << hEvents->GetBinContent((Int_t)EventStatus::ZVTX60_MBDNS1+1) << ", " << hEvents->GetBinContent((Int_t)EventStatus::ZVTX60_MBDNS1+1)*100./hEvents->GetBinContent((Int_t)EventStatus::ZVTX60+1) << " %" << endl;
  cout << "ZVTX60_MBDNS1_JET8: " << hEvents->GetBinContent((Int_t)EventStatus::ZVTX60_MBDNS1_JET8+1) << ", " << hEvents->GetBinContent((Int_t)EventStatus::ZVTX60_MBDNS1_JET8+1)*100./hEvents->GetBinContent((Int_t)EventStatus::ZVTX60+1) << " %" << endl;
  cout << "ZVTX60_MBDNS1_JET10: " << hEvents->GetBinContent((Int_t)EventStatus::ZVTX60_MBDNS1_JET10+1) << ", " << hEvents->GetBinContent((Int_t)EventStatus::ZVTX60_MBDNS1_JET10+1)*100./hEvents->GetBinContent((Int_t)EventStatus::ZVTX60+1) << " %" << endl;
  cout << "ZVTX60_MBDNS1_JET12: " << hEvents->GetBinContent((Int_t)EventStatus::ZVTX60_MBDNS1_JET12+1) << ", " << hEvents->GetBinContent((Int_t)EventStatus::ZVTX60_MBDNS1_JET12+1)*100./hEvents->GetBinContent((Int_t)EventStatus::ZVTX60+1) << " %" << endl;
  cout << endl;

  cout << "ZVTX60: " << hEvents->GetBinContent((Int_t)EventStatus::ZVTX60+1) << ", " << hEvents->GetBinContent((Int_t)EventStatus::ZVTX60+1)*100./hEvents->GetBinContent((Int_t)EventStatus::ZVTX+1) << " %" << endl;
  cout << "ZVTX60_JET8: " << hEvents->GetBinContent((Int_t)EventStatus::ZVTX60_JET8+1) << ", " << hEvents->GetBinContent((Int_t)EventStatus::ZVTX60_JET8+1)*100./hEvents->GetBinContent((Int_t)EventStatus::ZVTX60+1) << " %" << endl;
  cout << "ZVTX60_JET10: " << hEvents->GetBinContent((Int_t)EventStatus::ZVTX60_JET10+1) << ", " << hEvents->GetBinContent((Int_t)EventStatus::ZVTX60_JET10+1)*100./hEvents->GetBinContent((Int_t)EventStatus::ZVTX60+1) << " %" << endl;
  cout << "ZVTX60_JET12: " << hEvents->GetBinContent((Int_t)EventStatus::ZVTX60_JET12+1) << ", " << hEvents->GetBinContent((Int_t)EventStatus::ZVTX60_JET12+1)*100./hEvents->GetBinContent((Int_t)EventStatus::ZVTX60+1) << " %" << endl;
  cout << endl;

  cout << "Jets" << endl;
  cout << "R=0.2: " << m_nJets_r02 << endl;
  cout << "R=0.4: " << m_nJets_r04 << endl;
  cout << "R=0.4 with pT >= " << m_highPtThreshold << ": " << m_highPtJetCtr << endl;
  cout << "R=0.6: " << m_nJets_r06 << endl;

  if(m_saveTree) {
    cout << "JetValidation::End - Output to " << m_outputTreeFileName << endl;
    m_outputTreeFile->cd();

    m_T->Write();

    m_outputTreeFile->Close();
    delete m_outputTreeFile;
  }
  cout << "JetValidation::End - Output to " << m_outputQAFileName << endl;

  m_outputQAFile->cd();

  hEvents->Write();
  hEventsBkg->Write();
  hZVtx->Write();

  m_outputQAFile->mkdir("hJet_r02/pt");
  m_outputQAFile->mkdir("hJet_r04/pt");
  m_outputQAFile->mkdir("hJet_r06/pt");

  m_outputQAFile->mkdir("hJet_r02/deltaPhi");
  m_outputQAFile->mkdir("hJet_r04/deltaPhi");
  m_outputQAFile->mkdir("hJet_r06/deltaPhi");

  m_outputQAFile->mkdir("hJet_r02/eta_phi");
  m_outputQAFile->mkdir("hJet_r04/eta_phi");
  m_outputQAFile->mkdir("hJet_r06/eta_phi");

  m_outputQAFile->mkdir("TowerEnergy/base");
  m_outputQAFile->mkdir("TowerEnergy/sub");

  for(UInt_t i = 0; i < JetEvent_Status_vec.size(); ++i) {
    m_outputQAFile->cd("hJet_r02/pt");
    hJetPt_r02[i]->Write();
    hJetPt_r02_bkg[i]->Write();
    hJetPt_r02_nobkg[i]->Write();

    m_outputQAFile->cd("hJet_r04/pt");
    hJetPt_r04[i]->Write();
    hJetPt_r04_bkg[i]->Write();
    hJetPt_r04_nobkg[i]->Write();

    m_outputQAFile->cd("hJet_r06/pt");
    hJetPt_r06[i]->Write();
    hJetPt_r06_bkg[i]->Write();
    hJetPt_r06_nobkg[i]->Write();

    m_outputQAFile->cd("hJet_r04/deltaPhi");
    hJetDeltaPhi_r04[i]->Write();
    hJetDeltaPhi_r04_bkg[i]->Write();
    hJetDeltaPhi_r04_nobkg[i]->Write();

    m_outputQAFile->cd("hJet_r04/eta_phi");
    h2JetEtaPhi_r04[i]->Write();
    h2JetEtaPhi_r04_bkg[i]->Write();
    h2JetEtaPhi_r04_nobkg[i]->Write();
  }

  for(UInt_t i = 0; i < h2TowerEnergy.size(); ++i) {
    m_outputQAFile->cd("TowerEnergy/base");
    h2TowerEnergy[i]->Write();

    m_outputQAFile->cd("TowerEnergy/sub");
    h2TowerEnergySub[i]->Write();
  }

  m_outputQAFile->Close();
  delete m_outputQAFile;

  cout << "JetValidation::End(PHCompositeNode *topNode) This is the End..." << endl;
  return Fun4AllReturnCodes::EVENT_OK;
}


//____________________________________________________________________________..
Bool_t JetValidation::isBackgroundEvent(std::vector<Float_t> &towerEnergy) {
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
