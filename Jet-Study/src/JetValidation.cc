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
// -- Cluster stuff
#include <calobase/RawCluster.h>
#include <calobase/RawClusterContainer.h>
#include <calobase/RawClusterUtility.h>

using std::cout;
using std::endl;
using std::string;
using std::to_string;

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
  , m_clusterNode("CLUSTERINFO_CEMC")
  , m_emcTowerNodeBase("TOWERINFO_CALIB_CEMC")
  , m_emcTowerNode("TOWERINFO_CALIB_CEMC_RETOWER")
  , m_ihcalTowerNode("TOWERINFO_CALIB_HCALIN")
  , m_ohcalTowerNode("TOWERINFO_CALIB_HCALOUT")
  , m_emcTowerNodeSub("TOWERINFO_CALIB_CEMC_RETOWER_SUB1")
  , m_ihcalTowerNodeSub("TOWERINFO_CALIB_HCALIN_SUB1")
  , m_ohcalTowerNodeSub("TOWERINFO_CALIB_HCALOUT_SUB1")
  , m_zvtx_max(30) /*cm*/
  , m_zvtx_max2(20) /*cm*/
  , m_zvtx_max3(10) /*cm*/
  , m_lowEnergyThreshold(1) /*GeV*/
  , m_lowPtThreshold(10) /*GeV*/
  , m_highPtThreshold(20) /*GeV*/
  , m_highPtJetCtr(0) /*GeV*/
  , m_saveTowerInfo(false)
  , m_interestEvent(0)
  , m_neighbors(0)
  , m_bins_pt(200)
  , m_pt_low(0) /*GeV*/
  , m_pt_high(200) /*GeV*/
  , m_bins_events(5)
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
  m_outputTreeFile = new TFile(m_outputTreeFileName.c_str(),"RECREATE");
  cout << "JetValidation::Init - Output to " << m_outputTreeFileName << endl;

  // configure Tree
  m_T = new TTree("T", "T");
  m_T->Branch("event", &m_globalEvent, "event/I");
  m_T->Branch("run", &m_run, "run/I");
  m_T->Branch("zvtx", &m_zvtx);
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

  m_T->Branch("towersCEMCSub_energy",  &m_towersCEMCSub_energy);
  m_T->Branch("towersIHCalSub_energy", &m_towersIHCalSub_energy);
  m_T->Branch("towersOHCalSub_energy", &m_towersOHCalSub_energy);

  m_T->Branch("cluster_energy", &m_cluster_energy);
  m_T->Branch("cluster_eta",    &m_cluster_eta);
  m_T->Branch("cluster_phi",    &m_cluster_phi);
  m_T->Branch("cluster_chi",    &m_cluster_chi);
  m_T->Branch("cluster_towerIndex", &m_cluster_towerIndex);

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

  m_outputQAFile = new TFile(m_outputQAFileName.c_str(),"RECREATE");
  cout << "JetValidation::Init - Output to " << m_outputQAFileName << endl;

  hJetPt_r02 = new TH1F("hJetPt_r02", "Jet: R = 0.2 and |z| < 30 cm; p_{T} [GeV]; Counts", m_bins_pt, m_pt_low, m_pt_high);
  hJetPt_r04 = new TH1F("hJetPt_r04", "Jet: R = 0.4 and |z| < 30 cm; p_{T} [GeV]; Counts", m_bins_pt, m_pt_low, m_pt_high);
  hJetPt_r06 = new TH1F("hJetPt_r06", "Jet: R = 0.6 and |z| < 30 cm; p_{T} [GeV]; Counts", m_bins_pt, m_pt_low, m_pt_high);

  hEvents    = new TH1F("hEvents","Events; Status; Counts", m_bins_events, 0, m_bins_events);
  hZVtx      = new TH1F("hZVtx","Z Vertex; z [cm]; Counts", m_bins_zvtx, m_zvtx_low, m_zvtx_high);

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
Int_t JetValidation::process_event(PHCompositeNode *topNode)
{
  if (m_event % 20 == 0) cout << "Progress: " << m_event << endl;
  ++m_event;
  hEvents->Fill(0);

  EventHeader* eventInfo = findNode::getClass<EventHeader>(topNode,"EventHeader");
  if(!eventInfo)
  {
    cout << PHWHERE << "JetValidation::process_event - Fatal Error - EventHeader node is missing. " << endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  m_globalEvent = eventInfo->get_EvtSequence();
  m_run         = eventInfo->get_RunNumber();

  if(m_saveTowerInfo && abs(m_globalEvent-m_interestEvent) > m_neighbors) {
    if(m_globalEvent > m_interestEvent) return Fun4AllReturnCodes::ABORTRUN;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  // zvertex
  GlobalVertexMap *vertexmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");
  if (!vertexmap) {
    cout << "JetValidation::process_event - Error can not find global vertex node " << endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }
  if (vertexmap->empty() && !m_saveTowerInfo) {
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  if(!vertexmap->empty()) {
    GlobalVertex *vtx = vertexmap->begin()->second;
    m_zvtx = vtx->get_z();
  }
  else m_zvtx = -9999;

  hZVtx->Fill(m_zvtx);
  ++m_eventZVtx;
  hEvents->Fill(1);

  if(abs(m_zvtx) >= m_zvtx_max && !m_saveTowerInfo) return Fun4AllReturnCodes::ABORTEVENT;

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

  //Information on clusters
  RawClusterContainer* clusterContainer = findNode::getClass<RawClusterContainer>(topNode,m_clusterNode.c_str());

  if(!clusterContainer) {
    cout << PHWHERE << "JetValidation::process_event Could not find node: " << m_clusterNode << endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  // interface to reco jets
  JetContainer *jets_r02 = findNode::getClass<JetContainer>(topNode, m_recoJetName_r02);
  JetContainer *jets_r04 = findNode::getClass<JetContainer>(topNode, m_recoJetName_r04);
  JetContainer *jets_r06 = findNode::getClass<JetContainer>(topNode, m_recoJetName_r06);
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
    if (jet->get_pt() < m_lowPtThreshold) continue;  // to remove noise jets

    m_id_r02.push_back(jet->get_id());
    m_nComponent_r02.push_back(jet->size_comp());
    m_eta_r02.push_back(jet->get_eta());
    m_phi_r02.push_back(jet->get_phi());
    m_e_r02.push_back(jet->get_e());
    m_pt_r02.push_back(jet->get_pt());

    hJetPt_r02->Fill(jet->get_pt());

    ++m_nJet_r02;
  }

  Bool_t hasHighPtJet = false;
  // R = 0.4
  for (auto jet : *jets_r04) {
    if (jet->get_pt() < m_lowPtThreshold) continue;  // to remove noise jets

    m_id_r04.push_back(jet->get_id());
    m_nComponent_r04.push_back(jet->size_comp());
    m_eta_r04.push_back(jet->get_eta());
    m_phi_r04.push_back(jet->get_phi());
    m_e_r04.push_back(jet->get_e());
    m_pt_r04.push_back(jet->get_pt());

    hJetPt_r04->Fill(jet->get_pt());

    if(jet->get_pt() >= m_highPtThreshold) {
      hasHighPtJet = true;
      ++m_highPtJetCtr;
    }

    ++m_nJet_r04;
  }

  if(hasHighPtJet || m_saveTowerInfo) {
    // loop over base towers
    for(UInt_t towerIndex = 0; towerIndex < towersCEMCBase->size(); ++towerIndex) {
      TowerInfo* tower = towersCEMCBase->get_tower_at_channel(towerIndex);
      m_towersCEMCBase_isGood.push_back(tower->get_isGood());
      m_towersCEMCBase_time.push_back(tower->get_time_float());
      m_towersCEMCBase_energy.push_back(tower->get_energy());
    }

    // loop over towers
    for(UInt_t towerIndex = 0; towerIndex < towersCEMC->size(); ++towerIndex) {
      // unsubtracted
      TowerInfo* tower = towersCEMC->get_tower_at_channel(towerIndex);

      m_towersCEMC_energy.push_back(tower->get_energy());
      m_towersCEMC_isGood.push_back(tower->get_isGood());

      tower = towersIHCal->get_tower_at_channel(towerIndex);
      m_towersIHCal_energy.push_back(tower->get_energy());
      m_towersIHCal_isGood.push_back(tower->get_isGood());

      tower = towersOHCal->get_tower_at_channel(towerIndex);
      m_towersOHCal_energy.push_back(tower->get_energy());
      m_towersOHCal_isGood.push_back(tower->get_isGood());

      // subtracted
      tower = towersCEMCSub->get_tower_at_channel(towerIndex);
      m_towersCEMCSub_energy.push_back(tower->get_energy());

      tower = towersIHCalSub->get_tower_at_channel(towerIndex);
      m_towersIHCalSub_energy.push_back(tower->get_energy());

      tower = towersOHCalSub->get_tower_at_channel(towerIndex);
      m_towersOHCalSub_energy.push_back(tower->get_energy());
    }

    RawClusterContainer::ConstRange clusterEnd = clusterContainer->getClusters();
    RawClusterContainer::ConstIterator clusterIter;

    CLHEP::Hep3Vector vertex(0,0,m_zvtx);
    // loop over all clusters in event
    for(clusterIter = clusterEnd.first; clusterIter != clusterEnd.second; ++clusterIter) {
      RawCluster* recoCluster = clusterIter -> second;

      CLHEP::Hep3Vector E_vec_cluster = RawClusterUtility::GetECoreVec(*recoCluster, vertex);
      // CLHEP::Hep3Vector E_vec_cluster_Full = RawClusterUtility::GetEVec(*recoCluster, vertex);

      Float_t clusE    = E_vec_cluster.mag();
      Float_t clus_eta = E_vec_cluster.pseudoRapidity();
      Float_t clus_phi = E_vec_cluster.phi();
      Float_t clus_chi = recoCluster->get_chi2();

      // ignore noise clusters
      if(clusE < m_lowEnergyThreshold) continue;

      m_cluster_energy.push_back(clusE);
      m_cluster_eta.push_back(clus_eta);
      m_cluster_phi.push_back(clus_phi);
      m_cluster_chi.push_back(clus_chi);

      std::vector<Double_t> cluster_towerIndex;
      RawCluster::TowerConstRange towers = recoCluster->get_towers();
      RawCluster::TowerConstIterator toweriter;

      // loop over towers in the cluster
      for (toweriter = towers.first; toweriter != towers.second; toweriter++) {
          cluster_towerIndex.push_back(TowerInfoDefs::decode_emcal(TowerInfoDefs::encode_emcal(RawTowerDefs::decode_index1(toweriter->first), RawTowerDefs::decode_index2(toweriter->first))));
      }
      m_cluster_towerIndex.push_back(cluster_towerIndex);
    }
  }

  // R = 0.6
  for (auto jet : *jets_r06) {
    if (jet->get_pt() < m_lowPtThreshold) continue;  // to remove noise jets

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
    cout << PHWHERE << "JetValidation::process_event: GL1Packet node is missing. Output related to this node will be empty" << endl;
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

  m_cluster_energy.clear();
  m_cluster_eta.clear();
  m_cluster_phi.clear();
  m_cluster_chi.clear();

  m_cluster_towerIndex.clear();

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
Int_t JetValidation::End(PHCompositeNode *topNode)
{
  cout << "Events With Z Vtx: " << m_eventZVtx << ", " << m_eventZVtx * 100./m_event << " %" << endl;
  cout << "Events With |Z| < 30 cm: " << m_eventZVtx30 << ", " << m_eventZVtx30 * 100./m_event << " %" << endl;
  cout << "Jets (R=0.2): " << m_nJets_r02 << endl;
  cout << "Jets (R=0.4): " << m_nJets_r04 << endl;
  cout << "Jets (R=0.6): " << m_nJets_r06 << endl;
  cout << "Jets (R=0.4) with pT >= " << m_highPtThreshold << ": " << m_highPtJetCtr << endl;

  cout << "JetValidation::End - Output to " << m_outputTreeFileName << endl;
  m_outputTreeFile->cd();

  m_T->Write();

  m_outputTreeFile->Close();
  delete m_outputTreeFile;

  cout << "JetValidation::End - Output to " << m_outputQAFileName << endl;

  m_outputQAFile->cd();

  hEvents->Write();
  hZVtx->Write();
  hJetPt_r02->Write();
  hJetPt_r04->Write();
  hJetPt_r06->Write();

  m_outputQAFile->Close();
  delete m_outputQAFile;

  cout << "JetValidation::End(PHCompositeNode *topNode) This is the End..." << endl;
  return Fun4AllReturnCodes::EVENT_OK;
}
