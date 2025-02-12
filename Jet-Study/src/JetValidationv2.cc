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
#include <phparameter/PHParameters.h>
#include <pdbcalbase/PdbParameterMap.h>
// -- root
#include <TFile.h>
// -- Tower stuff
#include <calobase/TowerInfoDefs.h>
#include <calobase/RawTowerDefs.h>
#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfo.h>
#include <calobase/RawTowerGeomContainer.h>
#include <calobase/RawTowerGeom.h>
// -- Trigger
#include <calotrigger/TriggerRunInfo.h>

// Jet Utils
#include "JetUtils.h"

using std::cout;
using std::endl;
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
  , m_emcGeomNode("TOWERGEOM_CEMC")
  , m_ihcGeomNode("TOWERGEOM_HCALIN")
  , m_ohcGeomNode("TOWERGEOM_HCALOUT")
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
  , m_bins_frac2(100)
  , m_frac2_low(0)
  , m_frac2_high(1)
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

  h2LeadTowPtFracVsJetPt = new TH2F("h2LeadTowPtFracVsJetPt","Jet; Jet p_{T} [GeV]; Leading Tower p_{T}/#sum Tower p_{T}"
                                   , m_bins_pt, m_pt_low, m_pt_high, m_bins_frac2, m_frac2_low, m_frac2_high);

  h2LeadTowPtFracVsJetPt_miss = new TH2F("h2LeadTowPtFracVsJetPt_miss","Jet; Jet p_{T} [GeV]; Leading Tower p_{T}/#sum Tower p_{T}"
                                   , m_bins_pt, m_pt_low, m_pt_high, m_bins_frac2, m_frac2_low, m_frac2_high);

  h2LeadTowPtFracVsJetPtCEMC = new TH2F("h2LeadTowPtFracVsJetPtCEMC","Jet: Leading Tower p_{T} from CEMC; Jet p_{T} [GeV]; Leading Tower p_{T}/#sum Tower p_{T}"
                                   , m_bins_pt, m_pt_low, m_pt_high, m_bins_frac2, m_frac2_low, m_frac2_high);

  h2LeadTowPtFracVsJetPtIHCal = new TH2F("h2LeadTowPtFracVsJetPtIHCal","Jet: Leading Tower p_{T} from IHCal; Jet p_{T} [GeV]; Leading Tower p_{T}/#sum Tower p_{T}"
                                   , m_bins_pt, m_pt_low, m_pt_high, m_bins_frac2, m_frac2_low, m_frac2_high);

  h2LeadTowPtFracVsJetPtOHCal = new TH2F("h2LeadTowPtFracVsJetPtOHCal","Jet: Leading Tower p_{T} from OHCal; Jet p_{T} [GeV]; Leading Tower p_{T}/#sum Tower p_{T}"
                                   , m_bins_pt, m_pt_low, m_pt_high, m_bins_frac2, m_frac2_low, m_frac2_high);

  h2LeadTowPtBySubLeadTowPtFracVsJetPt = new TH2F("h2LeadTowPtBySubLeadTowPtFracVsJetPt","Jet; Jet p_{T} [GeV]; p_{T,Tower Sublead}/p_{T, Tower Lead}"
                                   , m_bins_pt, m_pt_low, m_pt_high, m_bins_frac2, m_frac2_low, m_frac2_high);

  h2LeadTowPtBySubLeadTowPtFracVsJetPt_miss = new TH2F("h2LeadTowPtBySubLeadTowPtFracVsJetPt_miss","Jet; Jet p_{T} [GeV]; p_{T,Tower Sublead}/p_{T, Tower Lead}"
                                   , m_bins_pt, m_pt_low, m_pt_high, m_bins_frac2, m_frac2_low, m_frac2_high);

  h2XjVsJetPt = new TH2F("h2XjVsJetPt","Event; Lead Jet p_{T} [GeV]; x_{j}"
                                   , m_bins_pt, m_pt_low, m_pt_high, m_bins_frac2, m_frac2_low, m_frac2_high);

  h2XjVsJetPt_miss = new TH2F("h2XjVsJetPt_miss","Event; Lead Jet p_{T} [GeV]; x_{j}"
                                   , m_bins_pt, m_pt_low, m_pt_high, m_bins_frac2, m_frac2_low, m_frac2_high);

  // jet background QA
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

  // Get TowerInfoContainer
  TowerInfoContainer* towersCEMCBase = findNode::getClass<TowerInfoContainer>(topNode, m_emcTowerBaseNode.c_str());
  TowerInfoContainer* towersCEMC     = findNode::getClass<TowerInfoContainer>(topNode, m_emcTowerNode.c_str());
  TowerInfoContainer* towersIHCal    = findNode::getClass<TowerInfoContainer>(topNode, m_ihcalTowerNode.c_str());
  TowerInfoContainer* towersOHCal    = findNode::getClass<TowerInfoContainer>(topNode, m_ohcalTowerNode.c_str());

  // Get Geometry Containers
  RawTowerGeomContainer* geomCEMC  = findNode::getClass<RawTowerGeomContainer>(topNode, m_emcGeomNode.c_str());
  RawTowerGeomContainer* geomIHCAL = findNode::getClass<RawTowerGeomContainer>(topNode, m_ihcGeomNode.c_str());
  RawTowerGeomContainer* geomOHCAL = findNode::getClass<RawTowerGeomContainer>(topNode, m_ohcGeomNode.c_str());

  if (!towersCEMCBase || !towersCEMC || !towersIHCal || !towersOHCal) {
    cout << PHWHERE << "JetValidation::process_event Could not find one of "
         << m_emcTowerBaseNode  << ", "
         << m_emcTowerNode      << ", "
         << m_ihcalTowerNode    << ", "
         << m_ohcalTowerNode    << ", "
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

  string m_pdbNode = "JetCutParams";
  PdbParameterMap* pdb = findNode::getClass<PdbParameterMap>(topNode, m_pdbNode.c_str());
  if (!pdb) {
    cout << "JetValidationv2::process_event - Error can not find PdbParameterMap Node " << m_pdbNode << endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  PHParameters pdb_params("Jet_Cut_Params");
  pdb_params.FillFrom(pdb);

  Bool_t failsLoEmJetCut = pdb_params.get_int_param("failsLoEmJetCut");
  Bool_t failsHiEmJetCut = pdb_params.get_int_param("failsHiEmJetCut");
  Bool_t failsIhJetCut   = pdb_params.get_int_param("failsIhJetCut");
  Bool_t failsAnyJetCut  = pdb_params.get_int_param("failsAnyJetCut");

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
    Jet::TYPE_comp_vec comp_vec = jet->get_comp_vec();
    Int_t constituents = comp_vec.size();

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

    ++nJets;
    // exclude jets near the edge of the detector
    if(JetUtils::check_bad_jet_eta(eta, m_zvtx, m_R)) continue;

    hjetPhiEtaPt->Fill(phi, eta, pt);

    hjetConstituentsVsPt->Fill(pt, constituents);

    m_constituents_min = min(m_constituents_min, constituents);
    m_constituents_max = max(m_constituents_max, constituents);

    m_pt_min = min(m_pt_min, (Int_t)pt);
    m_pt_max = max(m_pt_max, (Int_t)pt);

    if(pt >= m_pt_background) {
      hasBkg = true;
    }

    Float_t totalPt = 0;
    Float_t towPtSublead = 0;
    Float_t towPtLead = 0;
    string  towDetLead = "";

    // loop over constituents
    for (auto comp : comp_vec) {
      TowerInfoContainer* towers = nullptr;
      UInt_t towerIndex = comp.second;
      RawTowerGeomContainer* geom = nullptr;
      RawTowerDefs::CalorimeterId geocaloid{RawTowerDefs::CalorimeterId::NONE};
      string det = "";

      if (comp.first == Jet::CEMC_TOWERINFO_RETOWER) {
        towers = towersCEMC;
        // using IHCal geom since there is no EMCal Retowered Geom
        geocaloid = RawTowerDefs::CalorimeterId::HCALIN;
        geom = geomIHCAL;
        det  = "CEMC";
      }
      if (comp.first == Jet::HCALIN_TOWERINFO) {
        towers = towersIHCal;
        geocaloid = RawTowerDefs::CalorimeterId::HCALIN;
        geom = geomIHCAL;
        det  = "IHCal";
      }
      if (comp.first == Jet::HCALOUT_TOWERINFO) {
        towers = towersOHCal;
        geocaloid = RawTowerDefs::CalorimeterId::HCALOUT;
        geom = geomOHCAL;
        det  = "OHCal";
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
      Float_t r = tower_geom->get_center_radius();

      // updating to the correct radius of the EMCal
      if(comp.first == Jet::CEMC_TOWERINFO_RETOWER) {
          const RawTowerDefs::keytype EMCal_key = RawTowerDefs::encode_towerid(RawTowerDefs::CalorimeterId::CEMC, 0, 0);
          RawTowerGeom* EMCal_tower_geom = geomCEMC->get_tower_geometry(EMCal_key);
          r = EMCal_tower_geom->get_center_radius();
      }

      TowerInfo* tower = towers->get_tower_at_channel(towerIndex);
      // ensure tower is flagged as good
      if(!tower->get_isGood()) {
        cout << "Bad Tower in Jet" << endl;
        continue;
      }

      Float_t tower_energy = tower->get_energy();

      // Float_t phi = atan2(tower_geom->get_center_y(), tower_geom->get_center_x());
      Float_t towereta = tower_geom->get_eta();
      Float_t z0 = sinh(towereta) * r;
      Float_t z = z0 - m_zvtx;
      Float_t tower_eta = asinh(z / r);  // eta after shift from vertex
      Float_t tower_pt = tower_energy / cosh(tower_eta);
      // Float_t px = pt * cos(phi);
      // Float_t py = pt * sin(phi);
      // Float_t pz = pt * sinh(eta);
      // Float_t et = energy*pt/sqrt(px*px+py*py+pz*pz);

      totalPt += tower_pt;
      if(tower_pt > towPtSublead) {
        if(tower_pt > towPtLead) {
          towPtSublead = towPtLead;
          towPtLead = tower_pt;
          towDetLead = det;
        }
        else {
          towPtSublead = tower_pt;
        }
      }
    }

    if(towPtLead != 0) {
      h2LeadTowPtBySubLeadTowPtFracVsJetPt->Fill(pt, towPtSublead/towPtLead);
      if(!failsAnyJetCut) {
        h2LeadTowPtBySubLeadTowPtFracVsJetPt_miss->Fill(pt, towPtSublead/towPtLead);
      }
    }

    if(totalPt != 0) {
      h2LeadTowPtFracVsJetPt->Fill(pt, towPtLead/totalPt);
      if(towDetLead == "CEMC") {
        h2LeadTowPtFracVsJetPtCEMC->Fill(pt, towPtLead/totalPt);
      }
      if(towDetLead == "IHCal") {
        h2LeadTowPtFracVsJetPtIHCal->Fill(pt, towPtLead/totalPt);
      }
      if(towDetLead == "OHCal") {
        h2LeadTowPtFracVsJetPtOHCal->Fill(pt, towPtLead/totalPt);
      }
      if(!failsAnyJetCut) {
        h2LeadTowPtFracVsJetPt_miss->Fill(pt, towPtLead/totalPt);
      }
    }

    // DEBUG
    // if(pt >= 10) {
    //   cout << "jet pt: " << pt << " jet et: " << jet->get_et() << ", total tower pt: "
    //        << totalPt << ", lead tower pt: " << towPtLead << ", ratio pt: " << towPtLead*100./totalPt << ", lead tower det: " << towDetLead << endl;
    // }
  }

  if(jetPtLead != 0) {
    Float_t xj = jetPtSubLead*1./jetPtLead;
    h2XjVsJetPt->Fill(jetPtLead, xj);
    if(!failsAnyJetCut) {
      h2XjVsJetPt_miss->Fill(jetPtLead, xj);
    }
    // DEBUG
    // if(xj < 0) {
    //   cout << "xj: " << xj << ", pt lead: " << jetPtLead << " GeV, pt sublead: " << jetPtSubLead << " GeV" << endl;
    // }
  }

  hNJetsVsLeadPt->Fill(jetPtLead, nJets);

  m_nJets_min = min(m_nJets_min, nJets);
  m_nJets_max = max(m_nJets_max, nJets);

  Bool_t isDijet   = pdb_params.get_int_param("isDijet");
  Float_t frcem    = pdb_params.get_double_param("frcem");
  Float_t frcoh    = pdb_params.get_double_param("frcoh");
  Float_t maxJetET = pdb_params.get_double_param("maxJetET");
  Float_t dPhi     = pdb_params.get_double_param("dPhi");

  h2ETVsFracCEMC->Fill(frcem, maxJetET);

  // if event doesn't have a high pt jet then skip to the next event
  if(!hasBkg) return Fun4AllReturnCodes::ABORTEVENT;

  h2FracOHCalVsFracCEMC->Fill(frcem, frcoh);

  cout << "Background Lead Jet: " << jetPtLead << " GeV, Sublead Jet: " << jetPtSubLead << " GeV, Event: " << m_globalEvent << endl;

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
  h2LeadTowPtFracVsJetPt->Write();
  h2LeadTowPtFracVsJetPt_miss->Write();
  h2LeadTowPtFracVsJetPtCEMC->Write();
  h2LeadTowPtFracVsJetPtIHCal->Write();
  h2LeadTowPtFracVsJetPtOHCal->Write();
  h2LeadTowPtBySubLeadTowPtFracVsJetPt->Write();
  h2LeadTowPtBySubLeadTowPtFracVsJetPt_miss->Write();
  h2XjVsJetPt->Write();
  h2XjVsJetPt_miss->Write();

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

