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
#include <fun4all/PHTFileServer.h>
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
#include <calobase/TowerInfoDefs.h>
#include <calobase/TowerInfoContainer.h>

using std::cout;
using std::endl;

//____________________________________________________________________________..
JetValidation::JetValidation()
  : SubsysReco("JetValidation")
  , m_recoJetName_r02("AntiKt_Tower_r02_Sub1")
  , m_recoJetName_r04("AntiKt_Tower_r04_Sub1")
  , m_recoJetName_r06("AntiKt_Tower_r06_Sub1")
  , m_outputTreeFileName("test.root")
  , m_outputQAFileName("qa.root")
  , m_zvtx_max(30) /*cm*/
  , m_bins_pt(200)
  , m_pt_low(0) /*GeV*/
  , m_pt_high(100) /*GeV*/
  , m_bins_events(3)
  , m_events_low(-0.5)
  , m_events_high(2.5)
  , m_bins_zvtx(200)
  , m_zvtx_low(-50)
  , m_zvtx_high(50)
  , m_T(nullptr)
  , m_run(0)
  , m_globalEvent(0)
  , m_event(0)
  , m_eventZVtx(0)
  , m_eventZVtx30(0)
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
  std::cout << "JetValidation::JetValidation(const std::string &name) Calling ctor" << std::endl;
}

//____________________________________________________________________________..
JetValidation::~JetValidation()
{
  std::cout << "JetValidation::~JetValidation() Calling dtor" << std::endl;
}

//____________________________________________________________________________..
Int_t JetValidation::Init(PHCompositeNode *topNode)
{
  std::cout << "JetValidation::Init(PHCompositeNode *topNode) Initializing" << std::endl;
  PHTFileServer::get().open(m_outputTreeFileName, "RECREATE");
  std::cout << "JetValidation::Init - Output to " << m_outputTreeFileName << std::endl;

  // configure Tree
  m_T = new TTree("T", "T");
  m_T->Branch("event", &m_globalEvent, "event/I");
  m_T->Branch("run", &m_run, "run/I");
  m_T->Branch("zvtx", &m_zvtx);
  m_T->Branch("triggerVector", &m_triggerVector);

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

  PHTFileServer::get().open(m_outputQAFileName, "RECREATE");
  std::cout << "JetValidation::Init - Output to " << m_outputQAFileName << std::endl;

  hJetPt_r02 = new TH1F("hJetPt_r02", "Jet: R = 0.2 and |z| < 30 cm; p_{T} [GeV]; Counts", m_bins_pt, m_pt_low, m_pt_high);
  hJetPt_r04 = new TH1F("hJetPt_r04", "Jet: R = 0.4 and |z| < 30 cm; p_{T} [GeV]; Counts", m_bins_pt, m_pt_low, m_pt_high);
  hJetPt_r06 = new TH1F("hJetPt_r06", "Jet: R = 0.6 and |z| < 30 cm; p_{T} [GeV]; Counts", m_bins_pt, m_pt_low, m_pt_high);

  hEvents    = new TH1F("hEvents","Events; Status; Counts", m_bins_events, m_events_low, m_events_high);
  hZVtx      = new TH1F("hZVtx","Z Vertex; z [cm]; Counts", m_bins_zvtx, m_zvtx_low, m_zvtx_high);

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
Int_t JetValidation::process_event(PHCompositeNode *topNode)
{
  if (m_event % 20 == 0) std::cout << "Progress: " << m_event << std::endl;
  ++m_event;
  hEvents->Fill(0);

  EventHeader* eventInfo = findNode::getClass<EventHeader>(topNode,"EventHeader");
  if(!eventInfo)
  {
    std::cout << PHWHERE << "JetValidation::process_event - Fatal Error - EventHeader node is missing. " << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  m_globalEvent = eventInfo->get_EvtSequence();
  m_run         = eventInfo->get_RunNumber();

  // zvertex
  GlobalVertexMap *vertexmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");
  if (!vertexmap) {
    std::cout << "JetValidation::process_event - Error can not find global vertex node " << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }
  if (vertexmap->empty()) {
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  GlobalVertex *vtx = vertexmap->begin()->second;
  m_zvtx = vtx->get_z();
  hZVtx->Fill(m_zvtx);
  ++m_eventZVtx;
  hEvents->Fill(1);

  if(abs(m_zvtx) >= m_zvtx_max) return Fun4AllReturnCodes::ABORTEVENT;
  ++m_eventZVtx30;
  hEvents->Fill(2);

  // interface to reco jets
  JetContainer *jets_r02 = findNode::getClass<JetContainer>(topNode, m_recoJetName_r02);
  JetContainer *jets_r04 = findNode::getClass<JetContainer>(topNode, m_recoJetName_r04);
  JetContainer *jets_r06 = findNode::getClass<JetContainer>(topNode, m_recoJetName_r06);
  if (!jets_r02 || !jets_r04 || !jets_r06) {
    std::cout
        << "JetValidation::process_event - Error can not find one of DST Reco JetContainer node "
        << m_recoJetName_r02 << ", "
        << m_recoJetName_r04 << ", "
        << m_recoJetName_r06 << ", " << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  // get reco jets
  m_nJet_r02 = 0;
  m_nJet_r04 = 0;
  m_nJet_r06 = 0;

  // R = 0.2
  for (auto jet : *jets_r02) {
    if (jet->get_pt() < 1) continue;  // to remove noise jets

    m_id_r02.push_back(jet->get_id());
    m_nComponent_r02.push_back(jet->size_comp());
    m_eta_r02.push_back(jet->get_eta());
    m_phi_r02.push_back(jet->get_phi());
    m_e_r02.push_back(jet->get_e());
    m_pt_r02.push_back(jet->get_pt());

    hJetPt_r02->Fill(jet->get_pt());

    ++m_nJet_r02;
  }

  // R = 0.4
  for (auto jet : *jets_r04) {
    if (jet->get_pt() < 1) continue;  // to remove noise jets

    m_id_r04.push_back(jet->get_id());
    m_nComponent_r04.push_back(jet->size_comp());
    m_eta_r04.push_back(jet->get_eta());
    m_phi_r04.push_back(jet->get_phi());
    m_e_r04.push_back(jet->get_e());
    m_pt_r04.push_back(jet->get_pt());

    hJetPt_r04->Fill(jet->get_pt());

    ++m_nJet_r04;
  }

  // R = 0.6
  for (auto jet : *jets_r06) {
    if (jet->get_pt() < 1) continue;  // to remove noise jets

    m_id_r06.push_back(jet->get_id());
    m_nComponent_r06.push_back(jet->size_comp());
    m_eta_r06.push_back(jet->get_eta());
    m_phi_r06.push_back(jet->get_phi());
    m_e_r06.push_back(jet->get_e());
    m_pt_r06.push_back(jet->get_pt());

    hJetPt_r06->Fill(jet->get_pt());

    ++m_nJet_r06;
  }

  // grab the gl1 data
  Gl1Packet *gl1PacketInfo = findNode::getClass<Gl1Packet>(topNode, "GL1Packet");
  if (!gl1PacketInfo)
  {
    std::cout << PHWHERE << "caloTreeGen::process_event: GL1Packet node is missing. Output related to this node will be empty" << std::endl;
  }

  if (gl1PacketInfo)
  {
    uint64_t triggervec = gl1PacketInfo->getTriggerVector();
    for (Int_t i = 0; i < 64; ++i)
    {
      bool trig_decision = ((triggervec & 0x1U) == 0x1U);
      m_triggerVector.push_back(trig_decision);
      triggervec = (triggervec >> 1U) & 0xffffffffU;
    }
  }

  // fill the tree
  m_T->Fill();

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

  m_triggerVector.clear();

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
Int_t JetValidation::End(PHCompositeNode *topNode)
{
  std::cout << "Events With Z Vtx: " << m_eventZVtx << ", " << m_eventZVtx * 100./m_event << " %" << std::endl;
  std::cout << "Events With |Z| < 30 cm: " << m_eventZVtx30 << ", " << m_eventZVtx30 * 100./m_event << " %" << std::endl;
  std::cout << "Jets (R=0.2): " << m_nJets_r02 << std::endl;
  std::cout << "Jets (R=0.4): " << m_nJets_r04 << std::endl;
  std::cout << "Jets (R=0.6): " << m_nJets_r06 << std::endl;

  std::cout << "JetValidation::End - Output to " << m_outputTreeFileName << std::endl;
  PHTFileServer::get().cd(m_outputTreeFileName);

  m_T->Write();

  std::cout << "JetValidation::End - Output to " << m_outputQAFileName << std::endl;
  PHTFileServer::get().cd(m_outputQAFileName);

  hEvents->Write();
  hZVtx->Write();
  hJetPt_r02->Write();
  hJetPt_r04->Write();
  hJetPt_r06->Write();

  std::cout << "JetValidation::End(PHCompositeNode *topNode) This is the End..." << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}
