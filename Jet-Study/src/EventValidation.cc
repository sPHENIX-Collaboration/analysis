#include <EventValidation.h>
// -- c++
#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <fstream>
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
#include <calobase/RawTowerDefs.h>
#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfo.h>
#include <calobase/RawTowerGeomContainer.h>
#include <calobase/RawTowerGeom.h>

// Jet Utils
#include "JetUtils.h"

using std::cout;
using std::endl;
using std::to_string;
using std::make_pair;
using std::stringstream;
using std::ofstream;

//____________________________________________________________________________..
EventValidation::EventValidation()
  : SubsysReco("EventValidation")
  , m_emcTowerNode("TOWERINFO_CALIB_CEMC_RETOWER")
  , m_ihcTowerNode("TOWERINFO_CALIB_HCALIN")
  , m_ohcTowerNode("TOWERINFO_CALIB_HCALOUT")
  , m_emcGeomNode("TOWERGEOM_CEMC")
  , m_ihcGeomNode("TOWERGEOM_HCALIN")
  , m_ohcGeomNode("TOWERGEOM_HCALOUT")
  , m_recoJetName_r04("AntiKt_unsubtracted_r04")
  , m_bins_phi(64)
  , m_phi_low(-M_PI)
  , m_phi_high(M_PI)
  , m_bins_eta(24)
  , m_eta_low(-1.1)
  , m_eta_high(1.1)
  , m_R(0.4)
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

  // so that the histos actually get written out
  Fun4AllServer *se = Fun4AllServer::instance();
  se->Print("NODETREE");

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

  int m_globalEvent = eventInfo->get_EvtSequence();
  int m_run         = eventInfo->get_RunNumber();

  // zvertex
  Double_t m_zvtx = -9999;
  GlobalVertexMap* vertexmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");

  if (!vertexmap) {
    cout << "JetValidationv2::process_event - Error can not find global vertex node " << endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  if(!vertexmap->empty()) {
    GlobalVertex* vtx = vertexmap->begin()->second;
    m_zvtx = vtx->get_z();
  }

  // interface to reco jets
  JetContainer* jets_r04 = findNode::getClass<JetContainer>(topNode, m_recoJetName_r04);
  if (!jets_r04) {
    cout << "EventValidation::process_event - Error can not find DST Reco JetContainer node " << m_recoJetName_r04 << endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  Int_t jetPtLead    = 0;
  Int_t jetPtSubLead = 0;

  Double_t jetPhiLead = 0;
  Double_t jetPhiSubLead = 0;

  Double_t jetEtaLead = 0;
  Double_t jetEtaSubLead = 0;

  Int_t jetIdxLead = 0;

  for (UInt_t i = 0; i < jets_r04->size(); ++i) {
    Jet* jet = jets_r04->get_jet(i);

    Double_t phi = jet->get_phi();
    Double_t eta = jet->get_eta();
    Double_t pt = jet->get_pt();

    // exclude jets near the edge of the detector
    if(JetUtils::check_bad_jet_eta(eta, m_zvtx, m_R)) continue;

    if((Int_t)pt > jetPtSubLead) {
      if((Int_t)pt > jetPtLead) {
        jetPtSubLead = jetPtLead;
        jetPtLead = pt;

        jetPhiSubLead = jetPhiLead;
        jetPhiLead = phi;

        jetEtaSubLead = jetEtaLead;
        jetEtaLead = eta;

        jetIdxLead = i;
      }
      else {
        jetPtSubLead  = pt;
        jetPhiSubLead = phi;
        jetEtaSubLead = eta;
      }
    }
  }

  cout << "==============" << endl;
  cout << "Event Jet Info" << endl;
  cout << "Z: " << m_zvtx << endl;
  cout << "Lead Jet pT: " << jetPtLead << ", phi: " << jetPhiLead << ", eta: " << jetEtaLead << ", idx: " << jetIdxLead << endl;
  cout << "SubLead Jet pT: " << jetPtSubLead << ", phi: " << jetPhiSubLead << ", eta: " << jetEtaSubLead << endl;
  cout << "==============" << endl;

  // Get TowerInfoContainer
  TowerInfoContainer* towersCEMC  = findNode::getClass<TowerInfoContainer>(topNode, m_emcTowerNode.c_str());
  TowerInfoContainer* towersIHCal = findNode::getClass<TowerInfoContainer>(topNode, m_ihcTowerNode.c_str());
  TowerInfoContainer* towersOHCal = findNode::getClass<TowerInfoContainer>(topNode, m_ohcTowerNode.c_str());

  RawTowerGeomContainer* geomCEMC  = findNode::getClass<RawTowerGeomContainer>(topNode, m_emcGeomNode.c_str());
  RawTowerGeomContainer* geomIHCAL = findNode::getClass<RawTowerGeomContainer>(topNode, m_ihcGeomNode.c_str());
  RawTowerGeomContainer* geomOHCAL = findNode::getClass<RawTowerGeomContainer>(topNode, m_ohcGeomNode.c_str());

  if (!towersCEMC || !towersIHCal || !towersOHCal) {
    cout << PHWHERE << "EventValidation::process_event Could not find one of "
         << m_emcTowerNode << ", "
         << m_ihcTowerNode << ", "
         << m_ohcTowerNode
         << endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  if (!geomCEMC || !geomIHCAL || !geomOHCAL) {
    cout << PHWHERE << "EventValidation::process_event Could not find one of "
         << m_emcGeomNode << ", "
         << m_ihcGeomNode << ", "
         << m_ohcGeomNode
         << endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  Jet::TYPE_comp_vec comp_vec = jets_r04->get_jet(jetIdxLead)->get_comp_vec();

  Double_t totalCEMCEnergy = 0;
  Double_t totalIHCalEnergy = 0;
  Double_t totalOHCalEnergy = 0;

  Double_t totalCEMCPx = 0;
  Double_t totalIHCalPx = 0;
  Double_t totalOHCalPx = 0;

  Double_t totalCEMCPy = 0;
  Double_t totalIHCalPy = 0;
  Double_t totalOHCalPy = 0;

  Double_t totalCEMCPz = 0;
  Double_t totalIHCalPz = 0;
  Double_t totalOHCalPz = 0;

  UInt_t nTowers = 0;

  stringstream name;
  stringstream nameSuffix;
  stringstream title;
  stringstream titleSuffix;

  titleSuffix << "Run: " << m_run << ", Event: " << m_globalEvent
              << "; #phi; #eta";

  nameSuffix << m_run << "_" << m_globalEvent;

  name << "CEMC_h2TowerPx_" << nameSuffix.str();
  title << "CEMC Tower Px: " << titleSuffix.str();

  auto h2TowerPx_ = new TH2F(name.str().c_str(), title.str().c_str(), m_bins_phi, m_phi_low, m_phi_high, m_bins_eta, m_eta_low, m_eta_high);

  name.str("");
  title.str("");
  name << "CEMC_h2TowerPy_" << nameSuffix.str();
  title << "CEMC Tower Py: " << titleSuffix.str();

  auto h2TowerPy_ = new TH2F(name.str().c_str(), title.str().c_str(), m_bins_phi, m_phi_low, m_phi_high, m_bins_eta, m_eta_low, m_eta_high);

  name.str("");
  title.str("");
  name << "CEMC_h2TowerEnergy_" << nameSuffix.str();
  title << "CEMC Tower Energy: " << titleSuffix.str();

  auto h2TowerEnergy_ = new TH2F(name.str().c_str(), title.str().c_str(), m_bins_phi, m_phi_low, m_phi_high, m_bins_eta, m_eta_low, m_eta_high);


  name.str("");
  name << "event-log-" << m_run << "-" << m_globalEvent << ".csv";

  ofstream event_log(name.str());
  event_log << "detector,iphi,ieta,phi,eta,energy,pt,px,py,pz" << endl;

  for (auto comp : comp_vec) {
    TowerInfoContainer* towers = nullptr;
    UInt_t towerIndex = comp.second;
    string det = "";
    Double_t* totalEnergy = nullptr;
    Double_t* totalPx = nullptr;
    Double_t* totalPy = nullptr;
    Double_t* totalPz = nullptr;
    RawTowerGeomContainer* geom = nullptr;
    RawTowerDefs::CalorimeterId geocaloid{RawTowerDefs::CalorimeterId::NONE};

    if (comp.first == Jet::CEMC_TOWERINFO_RETOWER) {
      towers = towersCEMC;
      det = "CEMC";
      totalEnergy = &totalCEMCEnergy;
      totalPx = &totalCEMCPx;
      totalPy = &totalCEMCPy;
      totalPz = &totalCEMCPz;
      geocaloid = RawTowerDefs::CalorimeterId::HCALIN;
      geom = geomIHCAL;
    }
    if (comp.first == Jet::HCALIN_TOWERINFO) {
      towers = towersIHCal;
      det = "IHCal";
      totalEnergy = &totalIHCalEnergy;
      totalPx = &totalIHCalPx;
      totalPy = &totalIHCalPy;
      totalPz = &totalIHCalPz;
      geocaloid = RawTowerDefs::CalorimeterId::HCALIN;
      geom = geomIHCAL;
    }
    if (comp.first == Jet::HCALOUT_TOWERINFO) {
      towers = towersIHCal;
      det = "OHCal";
      totalEnergy = &totalOHCalEnergy;
      totalPx = &totalOHCalPx;
      totalPy = &totalOHCalPy;
      totalPz = &totalOHCalPz;
      geocaloid = RawTowerDefs::CalorimeterId::HCALOUT;
      geom = geomOHCAL;
    }
    if(towers == nullptr) {
      cout << "Unknown Detector: " << comp.first << ", Index: " << towerIndex << endl;
      continue;
    }

    UInt_t calokey = towers->encode_key(towerIndex);
    Int_t ieta     = towers->getTowerEtaBin(calokey);
    Int_t iphi     = towers->getTowerPhiBin(calokey);

    // get the radius of the detector
    const RawTowerDefs::keytype key = RawTowerDefs::encode_towerid(geocaloid, ieta, iphi);
    RawTowerGeom* tower_geom = geom->get_tower_geometry(key);
    Double_t r = tower_geom->get_center_radius();

    if(det == "CEMC") {
        const RawTowerDefs::keytype EMCal_key = RawTowerDefs::encode_towerid(RawTowerDefs::CalorimeterId::CEMC, 0, 0);
        RawTowerGeom *EMCal_tower_geom = geomCEMC->get_tower_geometry(EMCal_key);
        r = EMCal_tower_geom->get_center_radius();
    }

    TowerInfo* tower = towers->get_tower_at_channel(towerIndex);

    Double_t energy = tower->get_energy();
    *totalEnergy = *totalEnergy + energy;

    Double_t phi = atan2(tower_geom->get_center_y(), tower_geom->get_center_x());
    Double_t towereta = tower_geom->get_eta();
    Double_t z0 = sinh(towereta) * r;
    Double_t z = z0 - m_zvtx;
    Double_t eta = asinh(z / r);  // eta after shift from vertex
    Double_t pt = energy / cosh(eta);
    Double_t px = pt * cos(phi);
    Double_t py = pt * sin(phi);
    Double_t pz = pt * sinh(eta);

    *totalPx = *totalPx + px;
    *totalPy = *totalPy + py;
    *totalPz = *totalPz + pz;

    if(det == "CEMC") {
      h2TowerEnergy_->Fill(phi, eta, energy);
      h2TowerPx_->Fill(phi, eta, px);
      h2TowerPy_->Fill(phi, eta, py);
    }

    // ensure tower is flagged as good
    if(!tower->get_isGood()) {
      cout << "Bad Tower in Jet" << endl;
    }

    if(det == "CEMC") {
      cout << "Detector: " << det << ", iphi: " << iphi << ", ieta: " << ieta << ", energy: " << energy
           << ", px: " << px << ", py: " << py << ", pz: " << pz << endl;
      ++nTowers;
    }
    event_log << det << "," << iphi << "," << ieta << "," << phi << "," << eta << "," << energy << "," << pt << "," << px << "," << py << "," << pz << endl;
  }

  event_log.close();

  h2TowerEnergy.push_back(h2TowerEnergy_);
  h2TowerPx.push_back(h2TowerPx_);
  h2TowerPy.push_back(h2TowerPy_);

  cout << "============" << endl;
  cout << "Total Energy" << endl;
  cout << "CEMC: " << totalCEMCEnergy << endl;
  cout << "IHCal: " << totalIHCalEnergy << endl;
  cout << "OHCal: " << totalOHCalEnergy << endl;
  cout << "============" << endl;

  Double_t totalPx = totalCEMCPx + totalIHCalPx + totalOHCalPx;
  Double_t totalPy = totalCEMCPy + totalIHCalPy + totalOHCalPy;
  Double_t totalPz = totalCEMCPz + totalIHCalPz + totalOHCalPz;

  Double_t totalPt = sqrt(totalPx*totalPx + totalPy*totalPy);
  Double_t phi     = atan2(totalPy, totalPx);
  Double_t eta     = asinh(totalPz / totalPt);

  cout << "============" << endl;
  cout << "Lead Jet: pt: " << (Int_t)totalPt << ", px: " << (Int_t)totalPx << ", py: " << (Int_t)totalPy << ", pz: " << (Int_t)totalPz
                           << ", phi: " << phi << ", eta: " << eta << ", Towers: " << nTowers << endl;
  cout << "============" << endl;


  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
Int_t EventValidation::ResetEvent(PHCompositeNode *topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
Int_t EventValidation::End(PHCompositeNode *topNode)
{
  cout << "EventValidation::End(PHCompositeNode *topNode) This is the End..." << endl;
  TFile output(m_outputFile.c_str(),"recreate");

  output.cd();

  output.mkdir("energy");
  output.mkdir("px");
  output.mkdir("py");

  for(UInt_t i = 0; i < h2TowerEnergy.size(); ++i) {
    output.cd("energy");
    h2TowerEnergy[i]->Write();

    output.cd("px");
    h2TowerPx[i]->Write();

    output.cd("py");
    h2TowerPy[i]->Write();
  }

  output.Close();

  cout << "EventValidation::End(PHCompositeNode *topNode) This is the End..." << endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

