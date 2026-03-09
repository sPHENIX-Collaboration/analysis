#include "MyJetAnalysis.h"
#include <vector>
#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/PHTFileServer.h>
//#include <sPHAnalysis.h>

#include <HepMC/GenVertex.h>
#include <HepMC/IteratorRange.h>
#include <HepMC/SimpleVector.h>

#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

//#include <g4eval/JetEvalStack.h>

#include <trackbase_historic/SvtxTrackMap.h>
#include <g4jets/FastJetAlgo.h>
#include <jetbackground/FastJetAlgoSub.h>
#include <g4jets/JetMap.h>
#include <g4jets/Jetv1.h>

#include <g4vertex/GlobalVertex.h>
#include <g4vertex/GlobalVertexMap.h>

#include <calobase/RawTowerContainer.h>
#include <calobase/RawTower.h> 
#include <calobase/RawTowerGeomContainer.h>
#include <calobase/RawTowerGeom.h>
#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfo.h>

#include <g4main/PHG4Particle.h>
#include <g4main/PHG4TruthInfoContainer.h>

#include <g4eval/CaloRawTowerEval.h>
#include <g4eval/CaloEvalStack.h>

#include <ffaobjects/EventHeader.h>
#include <ffaobjects/EventHeaderv1.h>
#include <centrality/CentralityInfo.h>
// fastjet includes
#include <fastjet/ClusterSequence.hh>
#include <fastjet/JetDefinition.hh>
#include <fastjet/PseudoJet.hh>
#include <jetbackground/TowerBackground.h> 
#include <phhepmc/PHHepMCGenEvent.h>
#include <phhepmc/PHHepMCGenEventMap.h>

#include <TFile.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TString.h>
#include <TTree.h>
#include <TVector3.h>
#include <TLorentzVector.h>
#include <TArrayF.h>

#include <fstream>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>
//#include <memory> 

double DeltaPhi(double phi1, double phi2){
   double_t dPhi_temp = phi1 - phi2;
   if(dPhi_temp<-TMath::Pi()) dPhi_temp += TMath::TwoPi();
   if(dPhi_temp>=TMath::Pi()) dPhi_temp -= TMath::TwoPi();
   return dPhi_temp;
   }

//#include "/sphenix/user/stapiaar/jet_correlator/EnergyEnergyCorrelators/eec/include/EEC.hh"

using namespace std;
 
MyJetAnalysis::MyJetAnalysis(const std::string& recojetname, const std::string& truthjetname, const std::string& outputfilename)
  : SubsysReco("MyJetAnalysis_" + recojetname + "_" + truthjetname)
  , m_recoJetName(recojetname)
  , m_truthJetName(truthjetname)
  , m_outputFileName(outputfilename)
  , m_etaRange(-1.1, 1.1)
  , m_ptRange(5, 100)
  , m_trackJetMatchingRadius(.7)
  , m_hInclusiveE(nullptr)
  , m_hInclusiveEta(nullptr)
  , m_hInclusivePhi(nullptr)
  , m_T(nullptr)
  , m_event(-1)
  , _caloevalstackHCALIN(nullptr)
  , _caloevalstackHCALOUT(nullptr)
  , _caloevalstackCEMC(nullptr)
  /* , m_id(-1)
  , m_nComponent(-1)
  , m_eta(numeric_limits<float>::signaling_NaN())
  , m_phi(numeric_limits<float>::signaling_NaN())
 // , m_e(numeric_limits<vector>::signaling_NaN())
  , m_pt(numeric_limits<float>::signaling_NaN())
 // , m_px(numeric_limits<vector>::signaling_NaN())
 // , m_py(numeric_limits<vector>::signaling_NaN())
 // , m_pz(numeric_limits<vector>::signaling_NaN())
    //m_v4(numeric_limits<float>::signaling_NaN())
  , //m_truthID(-1)
  , m_truthNComponent(-1)
  , m_truthEta(numeric_limits<float>::signaling_NaN())
  , m_truthPhi(numeric_limits<float>::signaling_NaN())
 // , m_truthE(numeric_limits<vector>::signaling_NaN())
  , m_truthPt(numeric_limits<float>::signaling_NaN())
 // , m_truthPx(numeric_limits<vector>::signaling_NaN())
 // , m_truthPy(numeric_limits<vector>::signaling_NaN())
 /  , m_truthPz(numeric_limits<vecto>::signaling_NaN())
  */
  , m_nMatchedTrack(-1)

{
  m_trackdR.fill(numeric_limits<float>::signaling_NaN());
  m_trackpT.fill(numeric_limits<float>::signaling_NaN());
  m_trackPID.fill(numeric_limits<float>::signaling_NaN());
}

MyJetAnalysis::~MyJetAnalysis()
{
}

int MyJetAnalysis::Init(PHCompositeNode* topNode)
{
  if (Verbosity() >= MyJetAnalysis::VERBOSITY_SOME)
    cout << "MyJetAnalysis::Init - Outoput to " << m_outputFileName << endl;

  PHTFileServer::get().open(m_outputFileName, "RECREATE");
  
  cout << "MyJetAnalysis::Init - Outoput to " << m_outputFileName << endl;

  
  // Histograms
  m_hInclusiveE = new TH1F(
      "hInclusive_E",  //
      TString(m_recoJetName) + " inclusive jet E;Total jet energy (GeV)", 100, 0, 100);

  m_hInclusiveEta =
      new TH1F(
          "hInclusive_eta",  //
          TString(m_recoJetName) + " inclusive jet #eta;#eta;Jet energy density", 50, -1, 1);
  m_hInclusivePhi =
      new TH1F(
          "hInclusive_phi",  //
          TString(m_recoJetName) + " inclusive jet #phi;#phi;Jet energy density", 50, -M_PI, M_PI);

  initializeTrees();
  return Fun4AllReturnCodes::EVENT_OK;
}

int MyJetAnalysis::End(PHCompositeNode* topNode)
{
  PHTFileServer::get().cd(m_outputFileName);
  m_T->Write();
  cout << "MyJetAnalysis::End - Output to " << m_outputFileName << endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

int MyJetAnalysis::InitRun(PHCompositeNode* topNode)
{
return Fun4AllReturnCodes::EVENT_OK;
}

int MyJetAnalysis::process_event(PHCompositeNode* topNode)
{
  //cout << "MyJetAnalysis::process_event" << endl;
  if (Verbosity() >= MyJetAnalysis::VERBOSITY_SOME)
    cout << "MyJetAnalysis::process_event() entered" << endl;

  ++m_event;
  if( (m_event % 10) == 0 ) cout << "Event number = "<< m_event << endl;

  // Making Jets (Needed if not using JetMap)
  GlobalVertexMap *vertexmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");

  // interface to jets (JetMap)
  //JetMap* jets = findNode::getClass<JetMap>(topNode, m_recoJetName);
  JetMap* jetsMC = findNode::getClass<JetMap>(topNode, m_truthJetName);
  
  // Making Jets (Non JetMap) 
  
  RawTowerContainer *ihcal_towers_sub = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_HCALIN_SUB1");
  RawTowerContainer *ihcal_towers = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_HCALIN");
  RawTowerContainer *cemc_towers_sub = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_CEMC_RETOWER_SUB1");
  RawTowerContainer *cemc_towers = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_CEMC");
  RawTowerContainer *ohcal_towers_sub = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_HCALOUT_SUB1");
  RawTowerContainer *ohcal_towers = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_HCALOUT");
  
  //TowerInfoContainer *ihcal_towers_sub = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALIN_SUB1");
  //TowerInfoContainer *ihcal_towers = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALIN");
  //TowerInfoContainer *cemc_towers_sub = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_CEMC_RETOWER_SUB1");
  //TowerInfoContainer *cemc_towers = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_CEMC");
  //TowerInfoContainer *ohcal_towers_sub = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALOUT_SUB1");
  //TowerInfoContainer *ohcal_towers = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALOUT");
  
  RawTowerGeomContainer *geomIH = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALIN"); 
  RawTowerGeomContainer *geomOH = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALOUT");
  RawTowerGeomContainer *geomEC = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_CEMC");

  if (!geomIH || !geomOH ){
     cout << "Cannot find Tower Geom Nodes" << endl;
     exit(-1);
  }
 
  PHG4TruthInfoContainer *truthinfo = findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");
  if (!truthinfo){
     cout << "Error cant find G4 Truth Info" << endl;
     exit(-1);
  }

   CentralityInfo* cent_node = findNode::getClass<CentralityInfo>(topNode, "CentralityInfo");
  if (!cent_node){
     cout << "Error can't find CentralityInfo" << endl;
     exit(-1);
  }

  float cent_mbd = cent_node->get_centile(CentralityInfo::PROP::mbd_NS);
  m_cent.push_back(cent_mbd);
  
  /*
  std::vector<fastjet::PseudoJet> pseudojet_truth;
  PHG4TruthInfoContainer::ConstRange range = truthinfo->GetPrimaryParticleRange();
     for(PHG4TruthInfoContainer::ConstIterator iter = range.first; iter != range.second; ++iter){
        PHG4Particle *part = iter->second;
        if ((abs(part->get_pid()) >= 12) && (abs(part->get_pid()) <= 16)) continue;
        if ((part->get_px() == 0.0) && (part->get_py() == 0.0)) continue;
        float eta = asinh(part->get_pz()/sqrt(pow(part->get_px(),2) + pow(part->get_py(),2)));
        if (eta < -1.1) continue;
        if (eta > 1.1) continue;
        float Part_ID = truthinfo->isEmbeded(part->get_track_id());
        if (Part_ID != 2) continue;
        m_truthConstitID.push_back(Part_ID);
        m_truthConstitE.push_back(part->get_e());
        //m_truthConstitPt.push_back(part->get_pt());
        if (Part_ID != 2){
        cout << "TruthJet has a particle with ID = " << Part_ID << endl;
        }
        fastjet::PseudoJet jet_truth(part->get_px(), part->get_py(), part->get_pz(), part->get_e());
        //jet_truth.set_user_index(Part_ID);
        pseudojet_truth.push_back(jet_truth);
     }
  */
  GlobalVertex *vtx = vertexmap->begin()->second;
  float vtxz = NAN;
  if (vtx) vtxz = vtx->get_z();
  
  std::vector<fastjet::PseudoJet> pseudojet_reco;
  std::vector<fastjet::PseudoJet> pseudojet_reco_truth; 
  CaloRawTowerEval* towerevalHCALIN = new CaloRawTowerEval(topNode, "HCALIN");
  CaloRawTowerEval* towerevalHCALOUT = new CaloRawTowerEval(topNode, "HCALOUT");
  CaloRawTowerEval* towerevalCEMC = new CaloRawTowerEval(topNode, "CEMC");

  RawTowerContainer::ConstRange ICB_begin_end = ihcal_towers->getTowers();
  RawTowerContainer::ConstIterator ICB_rtiter;
  for (ICB_rtiter = ICB_begin_end.first; ICB_rtiter != ICB_begin_end.second; ++ICB_rtiter){
     float Tower_Embedded = 0;
     RawTower *towerB = ICB_rtiter->second;
     if (towerB->get_energy() <= 0) continue;
     PHG4Particle* primary = towerevalHCALIN->max_truth_primary_particle_by_energy(towerB);
     if (primary){
        if (truthinfo->isEmbeded(primary->get_track_id()) == 2){
           Tower_Embedded = 0;
           }
        else{
           Tower_Embedded = 1;
           }
        }
     else Tower_Embedded = -10;
     towers_id.push_back(towerB->get_id());
     towers_primary.push_back(Tower_Embedded);
  }

  RawTowerContainer::ConstRange IC_begin_end = ihcal_towers_sub->getTowers();
  RawTowerContainer::ConstIterator IC_rtiter; 
  for (IC_rtiter = IC_begin_end.first; IC_rtiter != IC_begin_end.second; ++IC_rtiter){
     RawTower *tower = IC_rtiter->second;
     RawTowerGeom *tower_geom = geomIH->get_tower_geometry(tower->get_key());
     assert(tower_geom);
     if(tower->get_energy() <= 0) continue;
     float HCALIN_Embedded = -2;
     //for (ICB_rtiter = ICB_begin_end.first; ICB_rtiter != ICB_begin_end.second; ++ICB_rtiter){
        //RawTower *towerB = ICB_rtiter->second;
        //if (towerB->get_id() != tower->get_id()) continue;
     for (int j = 0; j < (int)towers_id.size(); ++j){
        if (tower->get_id() != towers_id.at(j)) continue;  
        HCALIN_Embedded = towers_primary.at(j); 
     }
     
     double r = tower_geom->get_center_radius();
     double phi = atan2(tower_geom->get_center_y(), tower_geom->get_center_x());
     double z0 = tower_geom->get_center_z();
     double z = z0 - vtxz;      

     double eta = asinh(z/r);
     double pt = tower->get_energy()/cosh(eta);
     double px = pt*cos(phi);
     double py = pt*sin(phi);
     double pz = pt*sinh(eta);

     m_IHCAL_Tower_Energy.push_back(tower->get_energy());
     m_IHCAL_Cent.push_back(cent_mbd);
     //if (HCALIN_Embedded == -10 || HCALIN_Embedded == 1) continue;
     fastjet::PseudoJet pseudojet(px, py, pz, tower->get_energy());
     pseudojet.set_user_index(HCALIN_Embedded);  
     pseudojet_reco.push_back(pseudojet);  
     if (HCALIN_Embedded == -10 || HCALIN_Embedded == 1) continue;
     pseudojet_reco_truth.push_back(pseudojet);
  }
  
  towers_id.clear();
  towers_primary.clear();
   
  RawTowerContainer::ConstRange EC_begin_end = cemc_towers_sub->getTowers();
  RawTowerContainer::ConstRange ECB_begin_end = cemc_towers->getTowers();
  RawTowerContainer::ConstIterator EC_rtiter;
  RawTowerContainer::ConstIterator ECB_rtiter;
  for (EC_rtiter = EC_begin_end.first; EC_rtiter != EC_begin_end.second; ++EC_rtiter){
     RawTower *tower = EC_rtiter->second;
     RawTowerGeom *tower_geom = geomIH->get_tower_geometry(tower->get_key());  
     assert(tower_geom);
     if(tower->get_energy() <= 0) continue;
     int this_ECetabin = geomIH->get_etabin(tower_geom->get_eta());
     int this_ECphibin = geomIH->get_phibin(tower_geom->get_phi());
     float HCEMC_Embedded = -2;
     float Tower_Energy = 0;
     for (ECB_rtiter = ECB_begin_end.first; ECB_rtiter != ECB_begin_end.second; ++ECB_rtiter){
        RawTower *towerB = ECB_rtiter->second;
        RawTowerGeom *towerB_geom = geomEC->get_tower_geometry(towerB->get_key());
        int this_ECBetabin = geomIH->get_etabin(towerB_geom->get_eta());
        int this_ECBphibin = geomIH->get_phibin(towerB_geom->get_phi());
        if (this_ECBetabin != this_ECetabin && this_ECBphibin != this_ECphibin) continue; 
        if(towerB->get_energy() < Tower_Energy) continue;
        PHG4Particle* primaryEC = towerevalCEMC->max_truth_primary_particle_by_energy(towerB);
        if (primaryEC){
           if (truthinfo->isEmbeded(primaryEC->get_track_id()) == 2){
           HCEMC_Embedded = 2;
           }
           else{
           HCEMC_Embedded = 3;
           }
        } 
        else{
           HCEMC_Embedded = -11;
        }
        Tower_Energy = towerB->get_energy();
     }
     
      
     double r = tower_geom->get_center_radius();
     double phi = atan2(tower_geom->get_center_y(), tower_geom->get_center_x());
     double z0 = tower_geom->get_center_z();
     double z = z0 - vtxz;

     double eta = asinh(z/r);
     double pt = tower->get_energy()/cosh(eta);
     double px = pt*cos(phi);
     double py = pt*sin(phi);
     double pz = pt*sinh(eta);

     m_EMCAL_Tower_Energy.push_back(tower->get_energy());
     m_EMCAL_Cent.push_back(cent_mbd);
     //if(HCEMC_Embedded == -11  || HCEMC_Embedded == 3) continue;
     fastjet::PseudoJet pseudojet(px, py, pz, tower->get_energy());
     pseudojet.set_user_index(HCEMC_Embedded);
     pseudojet_reco.push_back(pseudojet);
     if(HCEMC_Embedded == -11  || HCEMC_Embedded == 3) continue;
     pseudojet_reco_truth.push_back(pseudojet);
  }
   
  RawTowerContainer::ConstRange OCB_begin_end = ohcal_towers->getTowers();
  RawTowerContainer::ConstIterator OCB_rtiter;
  for (OCB_rtiter = OCB_begin_end.first; OCB_rtiter != OCB_begin_end.second; ++OCB_rtiter){
     float Tower_Embedded = 0;
     RawTower *towerB = OCB_rtiter->second;
     if (towerB->get_energy() <= 0) continue;
     PHG4Particle* primary = towerevalHCALOUT->max_truth_primary_particle_by_energy(towerB);
     if (primary){
        if (truthinfo->isEmbeded(primary->get_track_id()) == 2){
           Tower_Embedded = 4;
           }
        else{
           Tower_Embedded = 5;
           }
        }
     else Tower_Embedded = -12;
     towers_id.push_back(towerB->get_id());
     towers_primary.push_back(Tower_Embedded);
  }

  RawTowerContainer::ConstRange OC_begin_end = ohcal_towers_sub->getTowers();
  RawTowerContainer::ConstIterator OC_rtiter;
  for (OC_rtiter = OC_begin_end.first; OC_rtiter != OC_begin_end.second; ++OC_rtiter){
     RawTower *tower = OC_rtiter->second;
     RawTowerGeom *tower_geom = geomOH->get_tower_geometry(tower->get_key());
     assert(tower_geom);
     if(tower->get_energy() <= 0) continue;
     float HCALOUT_Embedded = -1;  
     for (int j = 0; j < (int)towers_id.size(); ++j){
        if (tower->get_id() != towers_id.at(j)) continue;
        HCALOUT_Embedded = towers_primary.at(j);
     }
     double r = tower_geom->get_center_radius();
     double phi = atan2(tower_geom->get_center_y(), tower_geom->get_center_x());
     double z0 = tower_geom->get_center_z();
     double z = z0 - vtxz;

     double eta = asinh(z/r);
     double pt = tower->get_energy()/cosh(eta);
     double px = pt*cos(phi);
     double py = pt*sin(phi);
     double pz = pt*sinh(eta);

     m_OHCAL_Tower_Energy.push_back(tower->get_energy());
     m_OHCAL_Cent.push_back(cent_mbd);
     //if (HCALOUT_Embedded == -12 || HCALOUT_Embedded == 5) continue;
     fastjet::PseudoJet pseudojet(px, py, pz, tower->get_energy());
     pseudojet.set_user_index(HCALOUT_Embedded);
     pseudojet_reco.push_back(pseudojet);
     if (HCALOUT_Embedded == -12 || HCALOUT_Embedded == 5) continue;
     pseudojet_reco_truth.push_back(pseudojet);
  }
  
  towers_id.clear();
  towers_primary.clear();

  fastjet::JetDefinition jetDef(fastjet::antikt_algorithm, 0.2, fastjet::E_scheme,fastjet::Best);
  fastjet::ClusterSequence jetFinder_reco_truth(pseudojet_reco_truth, jetDef);
  fastjet::ClusterSequence jetFinder_reco(pseudojet_reco, jetDef);
  std::vector<fastjet::PseudoJet> fastjets_reco_truth = jetFinder_reco_truth.inclusive_jets();
  std::vector<fastjet::PseudoJet> fastjets_reco = jetFinder_reco.inclusive_jets();
  
  pseudojet_reco_truth.clear();
  pseudojet_reco.clear();
   
  //Saves Truth Jet Info
  for (JetMap::Iter iter = jetsMC->begin(); iter != jetsMC->end(); ++iter){
  //for (int j = 0; j < (int)fastjets.size(); ++j){    
    Jet* truthjet = iter->second;
    assert(truthjet);
    bool eta_cut = (truthjet->get_eta() >= m_etaRange.first) and (truthjet->get_eta() <= m_etaRange.second);
    if (not eta_cut) continue;    
    //cout << "Jet Truth = " << j << endl;
    /*
    vector<fastjet::PseudoJet> consts = jetFinder.constituents(fastjets[j]);
    //cout << "Size of Truth Jet" << (int)consts.size() << endl;
    for (int i = 0; i < (int)consts.size(); i++){
       if (consts[i].user_index() != -10){
          //cout << consts[i].user_index() << endl;
       }
    } 
    
    for(Jet::ConstIter comp = truthjet->begin_comp(); comp != truthjet->end_comp(); ++comp){
       const int this_embed_id = truthinfo->isEmbeded((*comp).second);
       if (this_embed_id != 2){
          cout << "Truth jet has non embedded particle with id = " << this_embed_id << endl;
       }
    }
    */
    m_truthEta.push_back(truthjet->get_eta());
    m_truthPhi.push_back(truthjet->get_phi());
    m_truthE.push_back(truthjet->get_e());
    m_truthPt.push_back(truthjet->get_pt());
    m_truthPx.push_back(truthjet->get_px());
    m_truthPy.push_back(truthjet->get_py());
    m_truthPz.push_back(truthjet->get_pz());
    m_truthNComponent.push_back(truthjet->size_comp());
    
  }
    //Saving Reco Truth Jets
    for (int j = 0; j < (int)fastjets_reco_truth.size(); ++j){
       
       vector<fastjet::PseudoJet> consts_reco_truth = jetFinder_reco_truth.constituents(fastjets_reco_truth[j]);

       m_recotruthnComponent.push_back((int)consts_reco_truth.size());
       m_recotruthEta.push_back(fastjets_reco_truth[j].eta());
       m_recotruthPhi.push_back(fastjets_reco_truth[j].phi_std());
       m_recotruthPt.push_back(fastjets_reco_truth[j].perp());
       m_recotruthPx.push_back(fastjets_reco_truth[j].px());
       m_recotruthPy.push_back(fastjets_reco_truth[j].py());
       m_recotruthPz.push_back(fastjets_reco_truth[j].pz());
       m_recotruthE.push_back(fastjets_reco_truth[j].e());

    }
    //saving closest recojets
    //for (JetMap::Iter iter = jets->begin(); iter != jets->end(); ++iter)
    for (int j = 0; j < (int)fastjets_reco.size(); ++j)
    {
        vector<fastjet::PseudoJet> consts = jetFinder_reco.constituents(fastjets_reco[j]);
        
        //Jet* jet = iter->second;
        
        //Needed For Making Jets
        /* 
        float nEmbedded = 0;
        float nTotal = 0;
        float nUnidentified_IHCAL = 0;
        float nUnidentified_OHCAL = 0;
        float nIHCAL_EM = 0;
        float nIHCAL_BC = 0;
        float nCEMC_EM = 0;
        float nCEMC_BC = 0;
        float nOHCAL_EM = 0;
        float nOHCAL_BC = 0;
        */
        //ID ordered by ID = 0 embedded Inner, ID = 1 nonembedded Inner, continues like this for EMCal then Outer
        for (int i = 0; i < (int)consts.size(); i++){
            m_CAL_ID.push_back(consts[i].user_index());
            m_Constit_E.push_back(consts[i].e());
            m_Constit_Cent.push_back(cent_mbd);
            /*
            if (consts[i].user_index() == 0 || consts[i].user_index() == 2 || consts[i].user_index() == 4){
               nEmbedded = nEmbedded + 1;
               if (consts[i].user_index() == 0){
                  nIHCAL_EM = nIHCAL_EM + 1;
               }
               if (consts[i].user_index() == 2){
                  nCEMC_EM = nCEMC_EM + 1;
               }
               if (consts[i].user_index() == 4){
                  nOHCAL_EM = nOHCAL_EM + 1;
               }
            }
            else{
               if (consts[i].user_index() == 1){
                  nIHCAL_BC = nIHCAL_BC + 1;
               }
               if (consts[i].user_index() == 3){
                  nCEMC_BC = nCEMC_BC + 1;
               }
               if (consts[i].user_index() == 5){
                  nOHCAL_BC = nOHCAL_BC + 1;
               }
            }
   
            if (consts[i].user_index() == -11){
               nUnidentified_IHCAL = nUnidentified_IHCAL + 1;
            }
            if (consts[i].user_index() == -10){
               nUnidentified_OHCAL = nUnidentified_OHCAL + 1; 
            }
            nTotal = nTotal + 1;
        */
        }       
        /*
        m_Embedded_IHCAL.push_back(nIHCAL_EM);
        m_Embedded_CEMC.push_back(nCEMC_EM);
        m_Embedded_OHCAL.push_back(nOHCAL_EM);
        m_Background_IHCAL.push_back(nIHCAL_BC);
        m_Background_CEMC.push_back(nCEMC_BC);
        m_Background_OHCAL.push_back(nOHCAL_BC);
        m_Embedded_Count.push_back(nEmbedded);
        m_Unidentified_IHCAL.push_back(nUnidentified_IHCAL);
        m_Unidentified_OHCAL.push_back(nUnidentified_OHCAL);
        m_Total_Count.push_back(nTotal);
        */
        // Save reco jet information (In code jet reconstruction)
        m_eta.push_back(fastjets_reco[j].eta());
        m_phi.push_back(fastjets_reco[j].phi_std());
        m_pt.push_back(fastjets_reco[j].perp());
        m_px.push_back(fastjets_reco[j].px());
        m_py.push_back(fastjets_reco[j].py());
        m_pz.push_back(fastjets_reco[j].pz());
        m_e.push_back(fastjets_reco[j].e());
       
        //Save the reco jet Information (Jet Map)
        
        //m_id.push_back(jet->get_id());
        //m_nComponent.push_back(jet->size_comp());
        //m_eta.push_back(jet->get_eta());
        //m_phi.push_back(jet->get_phi());
        //m_pt.push_back(jet->get_pt());
        //m_px.push_back(jet->get_px());
        //m_py.push_back(jet->get_py());
        //m_pz.push_back(jet->get_pz());
        //m_e.push_back(jet->get_e());
        
        float m_Loop_dR = 100000;// dR used for Loop to save closer truth jets
        
        //for (int i = 0; i < (int)fastjets.size(); ++i){
        for(std::size_t i = 0, max = (int)m_truthPhi.size(); i != max; ++i){     
	     //if(m_truthE.at(i) < 0) continue;
             
             double dEta_temp = fastjets_reco[j].eta() - m_truthEta.at(i);
             double dPhi_temp = DeltaPhi(fastjets_reco[j].phi_std(), m_truthPhi.at(i));
             
             //double dEta_temp = jet->get_eta() - m_truthEta.at(i);
             //double dPhi_temp = DeltaPhi(jet->get_phi(), m_truthPhi.at(i));
             
             float dR_temp = sqrt(dEta_temp * dEta_temp + dPhi_temp * dPhi_temp);
	     if(dR_temp > m_Loop_dR) continue;
           
             // Saving truth R = 0.2 matched Info 
             temp_TE_Matched = m_truthE.at(i);
             temp_TPhi_Matched = m_truthPhi.at(i);
             temp_TEta_Matched = m_truthEta.at(i);            
             
             
             m_Loop_dR = dR_temp;//Resets cutoff dR to mae sure to save only closer truth jets
          } 
      
      m_E_Matched.push_back(fastjets_reco[j].e());
      m_Phi_Matched.push_back(fastjets_reco[j].phi_std());
      m_Eta_Matched.push_back(fastjets_reco[j].eta());
     
      m_TE_Matched.push_back(temp_TE_Matched);
      m_TPhi_Matched.push_back(temp_TPhi_Matched);
      m_TEta_Matched.push_back(temp_TEta_Matched);
      m_dR.push_back(m_Loop_dR);
    }    
   
   m_T->Fill();
   m_IHCAL_Tower_Energy.clear();
   m_IHCAL_Cent.clear();
   m_EMCAL_Tower_Energy.clear();
   m_EMCAL_Cent.clear();
   m_OHCAL_Tower_Energy.clear();
   m_OHCAL_Cent.clear();
   //clearing truth info
   //m_truthID.clear();
   //m_truthNComponent.clear();
   m_truthEta.clear();
   m_truthPhi.clear();
   m_truthE.clear();
   m_truthPt.clear();
   m_truthPx.clear();
   m_truthPy.clear();
   m_truthPz.clear();
   //m_truthConstitE.clear();
   //m_truthConstitID.clear();   
   //m_truthConstitPt.clear();
   m_cent.clear();

   //clearing reco info
   //m_id.clear();
   m_nComponent.clear();
   m_eta.clear();
   m_phi.clear();
   m_pt.clear();
   m_e.clear();
   m_px.clear();
   m_py.clear();
   m_pz.clear();
    
   m_recotruthnComponent.clear();
   m_recotruthEta.clear();
   m_recotruthPhi.clear();
   m_recotruthPt.clear();
   m_recotruthE.clear();
   m_recotruthPx.clear();
   m_recotruthPy.clear();
   m_recotruthPz.clear();

   //MAtched infor clear
   m_TE_Matched.clear();
   m_TPhi_Matched.clear();
   m_TEta_Matched.clear();
   
   m_E_Matched.clear();
   m_Phi_Matched.clear();
   m_Eta_Matched.clear();
 
   m_dR.clear();
   m_cent.clear(); 
   m_CAL_ID.clear();
   m_Constit_E.clear();
   m_Constit_Cent.clear();
   /*
   m_Embedded_IHCAL.clear();
   m_Embedded_CEMC.clear();
   m_Embedded_OHCAL.clear();
   m_Background_IHCAL.clear();
   m_Background_CEMC.clear();
   m_Background_OHCAL.clear();  
   m_Embedded_Count.clear();
   m_Total_Count.clear();
   m_Unidentified_IHCAL.clear();
   m_Unidentified_OHCAL.clear();
   */
   Clean();

   return Fun4AllReturnCodes::EVENT_OK;
}

void MyJetAnalysis::initializeTrees()
{

m_T = new TTree("T", "MyJetAnalysis Tree");

//Storing All Reco Info
m_T->Branch("m_event", &m_event, "event/I");
m_T->Branch("id", &m_id);
m_T->Branch("nComponent", &m_nComponent);
m_T->Branch("eta", &m_eta);
m_T->Branch("phi", &m_phi);
m_T->Branch("e", &m_e);
m_T->Branch("pt", &m_pt);
m_T->Branch("px", &m_px);
m_T->Branch("py", &m_py);
m_T->Branch("pz", &m_pz);

m_T->Branch("IHCAL_Tower_Energy",&m_IHCAL_Tower_Energy);
m_T->Branch("IHCAL_Cent",&m_IHCAL_Cent);
m_T->Branch("EMCAL_Tower_Energy",&m_EMCAL_Tower_Energy);
m_T->Branch("EMCAL_Cent",&m_EMCAL_Cent);
m_T->Branch("OHCAL_Tower_Energy",&m_OHCAL_Tower_Energy);
m_T->Branch("OHCAL_Cent",&m_OHCAL_Cent);


//Storing All Reco Truth Info
m_T->Branch("recotruthnComponent", &m_recotruthnComponent);
m_T->Branch("recotruthEta", &m_recotruthEta);
m_T->Branch("recotruthPhi", &m_recotruthPhi);
m_T->Branch("recotruthPt", &m_recotruthPt);
m_T->Branch("recotruthPx", &m_recotruthPx);
m_T->Branch("recotruthPy", &m_recotruthPy);
m_T->Branch("recotruthPz", &m_recotruthPz);
m_T->Branch("recotruthE", &m_recotruthE);

//Storing Matched Jets Info
m_T->Branch("dR", &m_dR);

//Storing Reco Jet Info
m_T->Branch("cent", &m_cent);
m_T->Branch("CAL_ID", &m_CAL_ID);
//m_T->Branch("CAL_ID", &m_CAL_ID_RT);
m_T->Branch("Constit_Energy", &m_Constit_E);
//m_T->Branch("Constit_Energy_RT", &m_Constit_RT);
m_T->Branch("Constit_Cent", &m_Constit_Cent);
//m_T->Branch("Constit_Cent_RT", &m_Constit_Cent_RT);
//m_T->Branch("Embedded_Count", &m_Embedded_Count);
//m_T->Branch("Embedded_nIHCAL", &m_Embedded_IHCAL);
//m_T->Branch("Embedded_nCEMC", &m_Embedded_CEMC);
//m_T->Branch("Embedded_nOHCAL", &m_Embedded_OHCAL);
//m_T->Branch("Background_nIHCAL", &m_Background_IHCAL);
//m_T->Branch("Background_nCEMC", &m_Background_CEMC);
//m_T->Branch("Background_nOHCAL", &m_Background_OHCAL);
//m_T->Branch("Unidentified_IHCAL", &m_Unidentified_IHCAL);
//m_T->Branch("Unidentified_OHCAL", &m_Unidentified_OHCAL);
//m_T->Branch("Total_Count", &m_Total_Count);
//m_T->Branch("truthID", &m_truthID);
//m_T->Branch("truthNComponent", &m_truthNComponent);
//m_T->Branch("truthConstitE", &m_truthConstitE);
//m_T->Branch("truthConstitID", &m_truthConstitID);
m_T->Branch("truthEta", &m_truthEta);
m_T->Branch("truthPhi", &m_truthPhi);
m_T->Branch("truthE", &m_truthE);
m_T->Branch("truthPt", &m_truthPt);
m_T->Branch("truthPx", &m_truthPx);
m_T->Branch("truthPy", &m_truthPy);
m_T->Branch("truthPz", &m_truthPz);
//m_T->Branch("truthdR", &m_truthdR, "truthdR/F");

}

 void MyJetAnalysis::Clean()
{
//m_cent.clear();
}







