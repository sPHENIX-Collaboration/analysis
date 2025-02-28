
//module for producing a TTree with jet information for doing jet validation studies
// for questions/bugs please contact Virginia Bailey vbailey13@gsu.edu
#include <fun4all/Fun4AllBase.h>
#include <JetValidationTC.h>
#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/PHTFileServer.h>
#include <fun4all/Fun4AllHistoManager.h>
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>
#include <jetbase/JetMap.h>
#include <jetbase/JetContainer.h>
#include <jetbase/Jetv2.h>
#include <jetbase/Jetv1.h>
#include <centrality/CentralityInfo.h>
#include <globalvertex/GlobalVertex.h>
#include <globalvertex/GlobalVertexMap.h>
#include <calobase/RawTower.h>
#include <calobase/RawTowerContainer.h>
#include <calobase/RawTowerGeom.h>
#include <calobase/RawTowerGeomContainer.h>
#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfo.h>
#include <calobase/RawClusterContainer.h>
#include <calobase/RawCluster.h>
#include <calobase/RawClusterUtility.h>

#include <ffarawobjects/Gl1Packet.h>

#include <jetbackground/TowerBackground.h>

#include <TTree.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <vector>
#include <TLorentzVector.h>

#include "fastjet/ClusterSequence.hh"
#include "fastjet/contrib/SoftDrop.hh"                                                                                          

using namespace fastjet;

// ROOT, for histogramming.                                                                                                                                                                                 

#include "TH1.h"
#include "TH2.h"
#include "TFile.h"
#include <TTree.h>

//____________________________________________________________________________..
JetValidationTC::JetValidationTC(const std::string& recojetname, const std::string& truthjetname, const std::string& outputfilename):
  SubsysReco("JetValidation_" + recojetname + "_" + truthjetname)
  , m_recoJetName(recojetname)
  , m_truthJetName(truthjetname)
  , m_outputFileName(outputfilename)
  , m_etaRange(-1, 1)
  , m_ptRange(5, 100)
  , m_doTruthJets(0)
  , m_doSeeds(0)
  , m_doUnsubJet(0)
  , m_removeJetClusters(0)
  , m_T(nullptr)
  , m_event(-1)
  , m_nTruthJet(-1)
  , m_nJet(-1)
  , m_id()
  , m_nComponent()
  , m_eta()
  , m_phi()
  , m_e()
  , m_pt()
  , m_zg()
  , m_rg()
  , m_fe()
  , m_fh()
  , m_sub_et()
  , m_truthID()
  , m_truthNComponent()
  , m_truthEta()
  , m_truthPhi()
  , m_truthE()
  , m_truthPt()
  , m_eta_rawseed()
  , m_phi_rawseed()
  , m_pt_rawseed()
  , m_e_rawseed()
  , m_rawseed_cut()
  , m_eta_subseed()
  , m_phi_subseed()
  , m_pt_subseed()
  , m_e_subseed()
  , m_subseed_cut()
  
{
  std::cout << "JetValidationTC::JetValidationTC(const std::string &name) Calling ctor" << std::endl;
}

//____________________________________________________________________________..
JetValidationTC::~JetValidationTC()
{
  std::cout << "JetValidationTC::~JetValidationTC() Calling dtor" << std::endl;
}

//____________________________________________________________________________..
//____________________________________________________________________________..
int JetValidationTC::Init(PHCompositeNode *topNode)
{
  std::cout << "JetValidationTC::Init(PHCompositeNode *topNode) Initializing" << std::endl;
  //PHTFileServer::get().open(m_outputFileName, "RECREATE");
  m_histoFileName =  m_outputFileName + "_histos.root"; 
  m_outputFileName = m_outputFileName + ".root";
  hm = new Fun4AllHistoManager(Name());
  outfile = new TFile(m_outputFileName.c_str(),"RECREATE");
  std::cout << "JetValidationTC::Init - Output to " << m_outputFileName << std::endl;
  m_hasJetAboveThreshold = 0;

  
  hm->setOutfileName("JetValidation_histos.root");

  h_pi0M = new TH1F("h_pi0M","h_pi0M",4000,0,20);
  h_npions = new TH1F("h_npions","h_npions",100,0,100);
  h_eta_phi_clusters = new TH2F("h_eta_phi_clusters","h_eta_phi_clusters",22,-1.1,1.1,63,-3.14,3.14);
  h_jetPionMult = new TH1F("h_jetPionMult","h_jetPionMult",100,0,100);
  h_photonEnergy = new TH1F("h_photonEnergy","h_photonEnergy",400,0,20);


  

  // configure Tree
  m_T = new TTree("T", "MyJetAnalysis Tree");
  m_T->Branch("m_event", &m_event, "event/I");
  m_T->Branch("nJet", &m_nJet, "nJet/I");
  m_T->Branch("cent", &m_centrality);
  m_T->Branch("zvtx", &m_zvtx);
  m_T->Branch("b", &m_impactparam);
  m_T->Branch("id", &m_id);
  m_T->Branch("nComponent", &m_nComponent);

  m_T->Branch("eta", &m_eta);
  m_T->Branch("phi", &m_phi);
  m_T->Branch("e", &m_e);
  m_T->Branch("pt", &m_pt);
  m_T->Branch("zg", &m_zg);
  m_T->Branch("rg", &m_rg);
  m_T->Branch("triggers",&m_triggers);

  if (m_doClusters) {
    m_T->Branch("fe",&m_fe);
    m_T->Branch("fh",&m_fh);
    m_T->Branch("f_Et_emcal", &m_fEt_emcal);
    m_T->Branch("f_Et_hcal", &m_fEt_hcal);
    m_T->Branch("constE",&m_constE);
    m_T->Branch("hcalE",&m_hcalE);
    m_T->Branch("ecalE",&m_ecalE);
    m_T->Branch("constEt",&m_constEt);
    m_T->Branch("hcalClusterEt",&m_cluster_hcalEt);
    m_T->Branch("ecalClusterEt",&m_cluster_ecalEt);
    m_T->Branch("Et_emcal",&m_Et_emcal);
    m_T->Branch("Et_hcal",&m_Et_hcal);
    m_T->Branch("ClusterTowerE",&m_ClusterTowE);
    m_T->Branch("EMCalClusterMult",&m_emcal_cluster_mult);
    m_T->Branch("HCalClusterMult",&m_hcal_cluster_mult);
    m_T->Branch("ClusterMult",&m_cluster_mult);
    m_T->Branch("HasJetAboveThreshold", &m_hasJetAboveThreshold);
    m_T->Branch("pion_Z",&m_pionZ);
    m_T->Branch("jetPionMult", &m_jetPionMult);
    m_T->Branch("jetPionPt", &m_jetPionPt);
    m_T->Branch("jetPionEta",&m_jetPionEta);
    m_T->Branch("jetPionPhi",&m_jetPionPhi);
    m_T->Branch("jetPionMass",&m_jetPionMass);
    m_T->Branch("jetPionEnergy",&m_jetPionEnergy);
    //m_T->Branch("pionMassJetsRemoved", &m_pionMassJetsRemoved);
    m_T->Branch("pi0Mass",&pi0cand_mass);
    m_T->Branch("pi0pt",&pi0cand_pt);
    m_T->Branch("pi0eta",&pi0cand_eta);
    m_T->Branch("pi0phi",&pi0cand_phi);
    m_T->Branch("pi0energy",&pi0cand_energy);
  }
  if(m_doUnsubJet)
    {
      m_T->Branch("pt_unsub", &m_unsub_pt);
      m_T->Branch("subtracted_et", &m_sub_et);
    }
  if(m_doTruthJets){
    m_T->Branch("nTruthJet", &m_nTruthJet);
    m_T->Branch("truthID", &m_truthID);
    m_T->Branch("truthNComponent", &m_truthNComponent);
    m_T->Branch("truthEta", &m_truthEta);
    m_T->Branch("truthPhi", &m_truthPhi);
    m_T->Branch("truthE", &m_truthE);
    m_T->Branch("truthPt", &m_truthPt);
  }

  if(m_doSeeds){
    m_T->Branch("rawseedEta", &m_eta_rawseed);
    m_T->Branch("rawseedPhi", &m_phi_rawseed);
    m_T->Branch("rawseedPt", &m_pt_rawseed);
    m_T->Branch("rawseedE", &m_e_rawseed);
    m_T->Branch("rawseedCut", &m_rawseed_cut);
    m_T->Branch("subseedEta", &m_eta_subseed);
    m_T->Branch("subseedPhi", &m_phi_subseed);
    m_T->Branch("subseedPt", &m_pt_subseed);
    m_T->Branch("subseedE", &m_e_subseed);
    m_T->Branch("subseedCut", &m_subseed_cut);
  }
  
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int JetValidationTC::InitRun(PHCompositeNode *topNode)
{
  std::cout << "JetValidationTC::InitRun(PHCompositeNode *topNode) Initializing for Run XXX" << std::endl;
  
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int JetValidationTC::process_event(PHCompositeNode *topNode)
{
  //  std::cout << "JetValidationTC::process_event(PHCompositeNode *topNode) Processing Event" << std::endl;
  ++m_event;

  // interface to reco jets
  JetContainer* jets = findNode::getClass<JetContainer>(topNode, m_recoJetName);
  if (!jets)
    {
      std::cout
	<< "MyJetAnalysis::process_event - Error can not find DST Reco JetContainer node "
	<< m_recoJetName << std::endl;
      exit(-1);
    }

  //interface to truth jets
  //JetMap* jetsMC = findNode::getClass<JetMap>(topNode, m_truthJetName);
  JetContainer* jetsMC = findNode::getClass<JetContainer>(topNode, m_truthJetName);
  if (!jetsMC && m_doTruthJets)
    {
      std::cout
	<< "MyJetAnalysis::process_event - Error can not find DST Truth JetMap node "
	<< m_truthJetName << std::endl;
      exit(-1);
    }
  
  // interface to jet seeds
  JetContainer* seedjetsraw = findNode::getClass<JetContainer>(topNode, "AntiKt_TowerInfo_HIRecoSeedsRaw_r02");
  if (!seedjetsraw && m_doSeeds)
    {
      std::cout
	<< "MyJetAnalysis::process_event - Error can not find DST raw seed jets "
<< std::endl;
      exit(-1);
    }

  JetContainer* seedjetssub = findNode::getClass<JetContainer>(topNode, "AntiKt_TowerInfo_HIRecoSeedsSub_r02");
  if (!seedjetssub && m_doSeeds)
    {
      std::cout
<< "MyJetAnalysis::process_event - Error can not find DST subtracted seed jets "
<< std::endl;
      exit(-1);
    }

  //centrality
  CentralityInfo* cent_node = findNode::getClass<CentralityInfo>(topNode, "CentralityInfo");
  if (!cent_node)
    {
      std::cout
        << "MyJetAnalysis::process_event - Error can not find centrality node "
        << std::endl;
      exit(-1);
    }
  
  //zvertex
  GlobalVertexMap *vertexmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");
  if (!vertexmap)
    {
      std::cout
        << "MyJetAnalysis::process_event - Error can not find global vertex  node "
        << std::endl;
      exit(-1);
    }
  if (vertexmap->empty())
    {
      std::cout
        << "MyJetAnalysis::process_event - global vertex node is empty "
        << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }

    GlobalVertex *vtx = vertexmap->begin()->second;
  m_zvtx = vtx->get_z();
  CLHEP::Hep3Vector vertex;
  vertex.set(vtx->get_x(),vtx->get_y(),vtx->get_z());

  
 
  //calorimeter towers
  TowerInfoContainer *towersEM3 = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_CEMC_RETOWER_SUB1");
  TowerInfoContainer *towersIH3 = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALIN_SUB1");
  TowerInfoContainer *towersOH3 = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALOUT_SUB1");
  RawTowerGeomContainer *tower_geom = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALIN");
  RawTowerGeomContainer *tower_geomOH = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALOUT");
  RawClusterContainer *clustersHCAL = findNode::getClass<RawClusterContainer>(topNode, "TOPOCLUSTER_HCAL");
  RawClusterContainer *clustersEMCAL = findNode::getClass<RawClusterContainer>(topNode, "TOPOCLUSTER_EMCAL");

  if(!towersEM3 || !towersIH3 || !towersOH3){
    std::cout
      <<"MyJetAnalysis::process_event - Error can not find raw tower node "
      << std::endl;
    exit(-1);
  }

  if(!tower_geom || !tower_geomOH){
    std::cout
      <<"MyJetAnalysis::process_event - Error can not find raw tower geometry "
      << std::endl;
    exit(-1);
  }

  //underlying event
  TowerBackground *background = findNode::getClass<TowerBackground>(topNode, "TowerInfoBackground_Sub2");
  if(!background){
    std::cout<<"Can't get background. Exiting"<<std::endl;
    return Fun4AllReturnCodes::EVENT_OK;
  }

  //get the event centrality/impact parameter from HIJING
  m_centrality =  cent_node->get_centile(CentralityInfo::PROP::bimp);
  m_impactparam =  cent_node->get_quantity(CentralityInfo::PROP::bimp);

  //get reco jets
  m_nJet = 0;
  m_nPionJets = 0;
  float background_v2 = 0;
  float background_Psi2 = 0;
  if(m_doUnsubJet)
    {
      background_v2 = background->get_v2();
      background_Psi2 = background->get_Psi2();
    }

  reconstruct_pi0s(topNode);

  for (auto jet : *jets)
    {

      if(jet->get_pt() < 1) continue; // to remove noise jets
      if(jet->get_pt() < m_ptRange.first) continue; // remove jets outside of pt range

      m_id.push_back(jet->get_id());
      m_nComponent.push_back(jet->size_comp());
      m_eta.push_back(jet->get_eta());
      m_phi.push_back(jet->get_phi());
      m_e.push_back(jet->get_e());
      m_pt.push_back(jet->get_pt());
      m_zg.push_back(jet->get_property(jets->property_index(Jet::PROPERTY::prop_zg)));
      m_rg.push_back(jet->get_property(jets->property_index(Jet::PROPERTY::prop_Rg)));
      m_Et.push_back(jet->get_et());

      //get the pions within dR of the jet axis
      std::cout << "JetValidationTC::process_event: looking for pions" << std::endl;
      findJetPions(jet);

      //get the jet constituents and calculate signal ratios

      float _fe = 0.;
      float _fh = 0.;
      float _fEt_h = 0.;
      float _fEt_e = 0.;

      if (m_doClusters) {

        

        if (!clustersEMCAL || !clustersHCAL) {
          std::cout << "JetValidationTC::process_event - Error can not find RawClusterContainer"
          << std::endl;
          exit(-1);
        }

        std::vector<float> this_jet_hcal_et;
        std::vector<float> this_jet_emcal_et;
        std::vector<float> this_jet_const_et;

        int this_hcal_cluster_mult = 0;
        int this_emcal_cluster_mult = 0;
        int this_cluster_mult = 0;

        for (auto comp: jet->get_comp_vec()) {

          this_cluster_mult++;

          auto clusterType = comp.first;

          unsigned int clusterID = comp.second;

          const RawCluster *cluster;

          float cluster_energy;
          float cluster_Et;

          

          if (clusterType == Jet::SRC::HCAL_TOPO_CLUSTER) {
            this_hcal_cluster_mult++;
            
            cluster = clustersHCAL->getCluster(clusterID);
            cluster_energy = cluster->get_energy();
            cluster_Et = RawClusterUtility::GetET(*cluster,vertex);

            _fh += cluster_energy;
            _fEt_h += cluster_Et;
            
            m_hcalE.push_back(cluster_energy);
            this_jet_hcal_et.push_back(cluster_Et);
            m_constE.push_back(cluster_energy);
            this_jet_const_et.push_back(cluster_Et);

            RawCluster::TowerConstRange towers = cluster -> get_towers();
            RawCluster::TowerConstIterator toweriter;

            for(toweriter = towers.first; toweriter != towers.second; toweriter++) {
              float towE = toweriter -> second;
              m_ClusterTowE.push_back(towE);
            }

          } else if (clusterType == Jet::SRC::ECAL_TOPO_CLUSTER) {
            this_emcal_cluster_mult++;

            cluster = clustersEMCAL->getCluster(clusterID);
            cluster_energy = cluster->get_energy();
            cluster_Et = RawClusterUtility::GetET(*cluster,vertex);

            _fe += cluster_energy;
            _fEt_e += cluster_Et;

            m_ecalE.push_back(cluster_energy);
            this_jet_emcal_et.push_back(cluster_Et);
            m_constE.push_back(cluster_energy);
            this_jet_const_et.push_back(cluster_Et);

            RawCluster::TowerConstRange towers = cluster -> get_towers();
            RawCluster::TowerConstIterator toweriter;

            for(toweriter = towers.first; toweriter != towers.second; toweriter++) {
              float towE = toweriter -> second;
              m_ClusterTowE.push_back(towE);
            }



          }

          

          
          

        }

        m_emcal_cluster_mult.push_back(this_emcal_cluster_mult);
        m_hcal_cluster_mult.push_back(this_hcal_cluster_mult);
        m_cluster_mult.push_back(this_cluster_mult);

        float jet_energy = jet->get_e();
        float jet_et = jet->get_et();

        m_fe.push_back(_fe / jet_energy);
        m_fh.push_back(_fh / jet_energy);
        m_fEt_emcal.push_back(_fEt_e / jet_et);
        m_fEt_hcal.push_back(_fEt_h / jet_et);

        m_Et_emcal.push_back(_fEt_e);
        m_Et_hcal.push_back(_fEt_h);

        m_constEt.push_back(this_jet_const_et);
        m_cluster_hcalEt.push_back(this_jet_hcal_et);
        m_cluster_ecalEt.push_back(this_jet_emcal_et);

        




      }
      
      if(m_doUnsubJet)
	{
	  Jet* unsubjet = new Jetv1();
      
	  float totalPx = 0;
	  float totalPy = 0;
	  float totalPz = 0;
	  float totalE = 0;
	  int nconst = 0;
	    
	  for (auto comp: jet->get_comp_vec())
	    {
	      TowerInfo *tower;
	      nconst++;
	      unsigned int channel = comp.second;
	            
	      if (comp.first == 15 ||  comp.first == 30)
		{
		  tower = towersIH3->get_tower_at_channel(channel);
		  if(!tower || !tower_geom){
		    continue;
		  }
		  unsigned int calokey = towersIH3->encode_key(channel);
		  int ieta = towersIH3->getTowerEtaBin(calokey);
		  int iphi = towersIH3->getTowerPhiBin(calokey);
		  const RawTowerDefs::keytype key = RawTowerDefs::encode_towerid(RawTowerDefs::CalorimeterId::HCALIN, ieta, iphi);
		  float UE = background->get_UE(1).at(ieta);
		  float tower_phi = tower_geom->get_tower_geometry(key)->get_phi();
		  float tower_eta = tower_geom->get_tower_geometry(key)->get_eta();

		  UE = UE * (1 + 2 * background_v2 * cos(2 * (tower_phi - background_Psi2)));
		  totalE += tower->get_energy() + UE;
		  double pt = (tower->get_energy() + UE) / cosh(tower_eta);
		  totalPx += pt * cos(tower_phi);
		  totalPy += pt * sin(tower_phi);
		  totalPz += pt * sinh(tower_eta);
		}
	      else if (comp.first == 16 || comp.first == 31)
		{
		  tower = towersOH3->get_tower_at_channel(channel);
		  if(!tower || !tower_geomOH)
		    {
		      continue;
		    }
		    
		  unsigned int calokey = towersOH3->encode_key(channel);
		  int ieta = towersOH3->getTowerEtaBin(calokey);
		  int iphi = towersOH3->getTowerPhiBin(calokey);
		  const RawTowerDefs::keytype key = RawTowerDefs::encode_towerid(RawTowerDefs::CalorimeterId::HCALOUT, ieta, iphi);
		  float UE = background->get_UE(2).at(ieta);
		  float tower_phi = tower_geomOH->get_tower_geometry(key)->get_phi();
		  float tower_eta = tower_geomOH->get_tower_geometry(key)->get_eta();
		    
		  UE = UE * (1 + 2 * background_v2 * cos(2 * (tower_phi - background_Psi2)));
		  totalE +=tower->get_energy() + UE;
		  double pt = (tower->get_energy() + UE) / cosh(tower_eta);
		  totalPx += pt * cos(tower_phi);
		  totalPy += pt * sin(tower_phi);
		  totalPz += pt * sinh(tower_eta);
		}
	      else if (comp.first == 14 || comp.first == 29)
		{
		  tower = towersEM3->get_tower_at_channel(channel);
		  if(!tower || !tower_geom)
		    {
		      continue;
		    }
		    
		  unsigned int calokey = towersEM3->encode_key(channel);
		  int ieta = towersEM3->getTowerEtaBin(calokey);
		  int iphi = towersEM3->getTowerPhiBin(calokey);
		  const RawTowerDefs::keytype key = RawTowerDefs::encode_towerid(RawTowerDefs::CalorimeterId::HCALIN, ieta, iphi);
		  float UE = background->get_UE(0).at(ieta);
		  float tower_phi = tower_geom->get_tower_geometry(key)->get_phi();
		  float tower_eta = tower_geom->get_tower_geometry(key)->get_eta();
		    
		  UE = UE * (1 + 2 * background_v2 * cos(2 * (tower_phi - background_Psi2)));
		  totalE +=tower->get_energy() + UE;
		  double pt = (tower->get_energy() + UE) / cosh(tower_eta);
		  totalPx += pt * cos(tower_phi);
		  totalPy += pt * sin(tower_phi);
		  totalPz += pt * sinh(tower_eta);
		    
		}
	    }
	  //get unsubtracted jet
	  unsubjet->set_px(totalPx);
	  unsubjet->set_py(totalPy);
	  unsubjet->set_pz(totalPz);
	  unsubjet->set_e(totalE);
	  m_unsub_pt.push_back(unsubjet->get_pt());
	  m_sub_et.push_back(unsubjet->get_et() - jet->get_et());
	}
   
  
      m_nJet++;
    }

  //get truth jets
  if(m_doTruthJets)
    {
      m_nTruthJet = 0;
      //for (JetMap::Iter iter = jetsMC->begin(); iter != jetsMC->end(); ++iter)
      for (auto truthjet : *jetsMC)
	{
	  //Jet* truthjet = iter->second;
	    
	  bool eta_cut = (truthjet->get_eta() >= m_etaRange.first) and (truthjet->get_eta() <= m_etaRange.second);
	  bool pt_cut = (truthjet->get_pt() >= m_ptRange.first) and (truthjet->get_pt() <= m_ptRange.second);
	  if ((not eta_cut) or (not pt_cut)) continue;
	  m_truthID.push_back(truthjet->get_id());
	  m_truthNComponent.push_back(truthjet->size_comp());
	  m_truthEta.push_back(truthjet->get_eta());
	  m_truthPhi.push_back(truthjet->get_phi());
	  m_truthE.push_back(truthjet->get_e());
	  m_truthPt.push_back(truthjet->get_pt());
	  m_nTruthJet++;
	}
    }
  
  //get seed jets
  if(m_doSeeds)
    {
      for (auto jet : *seedjetsraw)
	{
	  int passesCut = jet->get_property(seedjetsraw->property_index(Jet::PROPERTY::prop_SeedItr));
	  m_eta_rawseed.push_back(jet->get_eta());
	  m_phi_rawseed.push_back(jet->get_phi());
	  m_e_rawseed.push_back(jet->get_e());
	  m_pt_rawseed.push_back(jet->get_pt());
	  m_rawseed_cut.push_back(passesCut);
	}
      
      for (auto jet : *seedjetssub)
	{
	  int passesCut = jet->get_property(seedjetssub->property_index(Jet::PROPERTY::prop_SeedItr));
	  m_eta_subseed.push_back(jet->get_eta());
	  m_phi_subseed.push_back(jet->get_phi());
	  m_e_subseed.push_back(jet->get_e());
	  m_pt_subseed.push_back(jet->get_pt());
	  m_subseed_cut.push_back(passesCut);
	}
    }

  Gl1Packet *gl1Packet = findNode::getClass<Gl1Packet>(topNode, "GL1Packet");

  if(gl1Packet)
  {
    auto scaled_vector = gl1Packet->getScaledVector();
    for(int i = 0; i < 32; i++)
    {
      if((scaled_vector & (int)std::pow(2,i)) != 0)
      {
        m_triggers.push_back(i);
      }
    }
  }
  else
  {
    std::cout << "gl1Packet not found" << std::endl;
  }
  //fill the tree
  m_T->Fill();

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int JetValidationTC::ResetEvent(PHCompositeNode *topNode)
{

  m_hasJetAboveThreshold = 0;
  //std::cout << "JetValidationTC::ResetEvent(PHCompositeNode *topNode) Resetting internal structures, prepare for next event" << std::endl;
  m_id.clear();
  m_nComponent.clear();
  m_eta.clear();
  m_phi.clear();
  m_e.clear();
  m_pt.clear();
  m_zg.clear();
  m_rg.clear();
  
  m_unsub_pt.clear();
  m_sub_et.clear();

  m_fe.clear();
  m_fh.clear();
  m_constE.clear();
  m_hcalE.clear();
  m_ecalE.clear();
  m_fEt_emcal.clear();
  m_fEt_hcal.clear();
  m_constEt.clear();
  m_cluster_hcalEt.clear();
  m_cluster_ecalEt.clear();
  m_Et_emcal.clear();
  m_Et_hcal.clear();
  m_ClusterTowE.clear();
  m_emcal_cluster_mult.clear();
  m_hcal_cluster_mult.clear();
  m_cluster_mult.clear();

  cluster_energy.clear();
  cluster_eta.clear();
  cluster_phi.clear();
  cluster_prob.clear();
  cluster_chi2.clear();

  pi0cand_pt.clear();
  pi0cand_eta.clear();
  pi0cand_phi.clear();
  pi0cand_mass.clear();
  pi0cand_energy.clear();
  m_jetPionMult.clear();
  m_pionZ.clear();
  m_jetPionPt.clear();
  m_jetPionEta.clear();
  m_jetPionPhi.clear();
  m_jetPionMass.clear();
  m_jetPionEnergy.clear();
  //m_pionMassJetsRemoved.clear();

  m_triggers.clear();

  m_truthID.clear();
  m_truthNComponent.clear();
  m_truthEta.clear();
  m_truthPhi.clear();
  m_truthE.clear();
  m_truthPt.clear();
  m_truthdR.clear();

  m_eta_subseed.clear();
  m_phi_subseed.clear();
  m_e_subseed.clear();
  m_pt_subseed.clear();
  m_subseed_cut.clear();

  m_eta_rawseed.clear();
  m_phi_rawseed.clear();
  m_e_rawseed.clear();
  m_pt_rawseed.clear();
  m_rawseed_cut.clear();
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int JetValidationTC::EndRun(const int runnumber)
{
  std::cout << "JetValidationTC::EndRun(const int runnumber) Ending Run for Run " << runnumber << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int JetValidationTC::End(PHCompositeNode *topNode)
{
  
  std::cout << "JetValidationTC::End: Dumping Histograms" << std::endl;

  //h_pi0M->SetDirectory(nullptr);
  //h_npions->SetDirectory(nullptr);

  hm->registerHisto(h_pi0M);
  hm->registerHisto(h_npions);
  hm->registerHisto(h_eta_phi_clusters);
  hm->registerHisto(h_jetPionMult);
  hm->registerHisto(h_photonEnergy);

  

  hm->dumpHistos(m_histoFileName.c_str(),"RECREATE");
  
  std::cout << "JetValidationTC::End - Output to " << m_outputFileName << std::endl;
  outfile->cd();
  m_T->Write();
  //m_T->cd();
  outfile->Write();
  outfile->Close();
  //delete outfile;
  //PHTFileServer::get().cd(m_outputFileName);

 
  

  
  std::cout << "JetValidationTC::End(PHCompositeNode *topNode) This is the End..." << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int JetValidationTC::Reset(PHCompositeNode *topNode)
{
  std::cout << "JetValidationTC::Reset(PHCompositeNode *topNode) being Reset" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
void JetValidationTC::Print(const std::string &what) const
{
  std::cout << "JetValidationTC::Print(const std::string &what) const Printing info for " << what << std::endl;
}


void JetValidationTC::reconstruct_pi0s(PHCompositeNode *topNode) {
  
  m_npi0 = 0;

  //check whether there are any jets above jet threshold in event

  JetContainer* jets = findNode::getClass<JetContainer>(topNode, m_recoJetName);
  if (!jets)
  {
    std::cout
    << "MyJetAnalysis::process_event - Error can not find DST Reco JetContainer node "
    << m_recoJetName << std::endl;
    exit(-1);
  }

  std::vector<float> these_jetsEta;
  std::vector<float> these_jetsPhi;

  for (auto jet : *jets) {
    if(jet->get_pt() < 1) continue; // to remove noise jets
    if(jet->get_pt() < m_ptRange.first) continue; // remove jets outside of pt range

    if (jet->get_pt() > m_pi0_jetThreshold) {
      m_hasJetAboveThreshold = 1;
      continue;
    }

  }
  
  if (m_hasJetAboveThreshold != 1) {
    std::cout << "No jets above threshold. Not reconstructing pi0s" << std::endl;
    return;  
  }

  nclus = 0;
  
  //reconstruct pions now that we've found a jet > threshold
  RawClusterContainer *clustersEMCAL = findNode::getClass<RawClusterContainer>(topNode, m_clusterType);
  //RawClusterContainer *clustersEMCAL = findNode::getClass<RawClusterContainer>(topNode,"CLUSTERINFO_POS_COR_CEMC");
  //zvertex
  GlobalVertexMap *vertexmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");
  if (!vertexmap)
  {
    std::cout
    << "MyJetAnalysis::reconstruct_pi0s - Error can not find global vertex  node "
    << std::endl;
    exit(-1);
  }
  if (vertexmap->empty())
  {
    std::cout
    << "MyJetAnalysis::reconstruct_pi0s - global vertex node is empty "
    << std::endl;
    exit(-1);
  }

  GlobalVertex *vtx = vertexmap->begin()->second;
  m_zvtx = vtx->get_z();
  CLHEP::Hep3Vector vertex;
  vertex.set(vtx->get_x(),vtx->get_y(),vtx->get_z());


  auto clusters = clustersEMCAL->getClusters();
  RawClusterContainer::ConstIterator clusIter;

  for (clusIter = clusters.first; clusIter != clusters.second; clusIter++) {
    const RawCluster *cluster = clusIter->second;

    if (cluster->get_energy() > m_minClusterE) {
      CLHEP::Hep3Vector cluster_vector = RawClusterUtility::GetECoreVec(*cluster, vertex);
      /*
      cluster_energy.push_back(cluster->get_energy());
      cluster_eta.push_back(cluster_vector.pseudoRapidity());
      std::cout << "JetValidationTC::reconstruct_pi0: cluster eta = " << cluster_vector.pseudoRapidity() << std::endl;
      cluster_phi.push_back(cluster->get_phi());
      cluster_prob.push_back(cluster->get_prob());
      cluster_chi2.push_back(cluster->get_chi2());*/

      float this_eta = RawClusterUtility::GetPseudorapidity(*cluster,vertex);
      float this_phi = RawClusterUtility::GetAzimuthAngle(*cluster,vertex);

      bool clusterIsWithinJetCone = false;

      for(auto jet: *jets) {
        float this_jet_eta = jet->get_eta();
        float this_jet_phi = jet->get_phi();

        float dR = sqrt(pow((this_eta - this_jet_eta),2) + pow((this_phi - this_jet_phi),2));

        if (dR < m_maxdR) {
          clusterIsWithinJetCone = true;
          continue;
        }

      }

      if (m_removeJetClusters && clusterIsWithinJetCone) {
        continue;
      }

      nclus++;

      cluster_energy.push_back(cluster->get_energy());
      h_photonEnergy->Fill(cluster->get_energy());
      
      cluster_eta.push_back(this_eta);
      cluster_phi.push_back(this_phi);

      h_eta_phi_clusters->Fill(this_eta,this_phi);

      std::cout << "JetValidationTC::reconstruct_pi0: cluster eta = " << RawClusterUtility::GetPseudorapidity(*cluster,vertex) << std::endl;
      std::cout << "JetValidationTC::reconstruct_pi0: cluster phi = " << RawClusterUtility::GetAzimuthAngle(*cluster,vertex) << std::endl;
      
    }
    
  }


  for (int i = 0; i < nclus; i++) {
    TLorentzVector v1;
    v1.SetPtEtaPhiM(cluster_energy[i] / cosh(cluster_eta[i]), cluster_eta[i], cluster_phi[i], 0.0);
    
    for (int j = i + 1; j < nclus; j++) {
      /*float alpha = fabs(cluster_energy[i] - cluster_energy[j]) / (cluster_energy[i] + cluster_energy[j]);

      
      if (alpha > 0.5) {
        continue;
      }*/

      TLorentzVector v2;
      v2.SetPtEtaPhiM(cluster_energy[j] / cosh(cluster_eta[j]), cluster_eta[j], cluster_phi[j], 0.0);
      
      if (v1.DeltaR(v2) < m_mindR) {
        continue;
      }

      TLorentzVector res;
      res = v1 + v2;

      pi0cand_pt.push_back(res.Pt());
      pi0cand_eta.push_back(res.Eta());
      pi0cand_phi.push_back(res.Phi());
      pi0cand_mass.push_back(res.M());
      pi0cand_energy.push_back(res.E());
      h_pi0M->Fill(res.M());
      //m_pionMassJetsRemoved.push_back(res.M()); // get all of the masses and remove those in jets later
      std::cout << "JetValidationTC::reconstruct_pi0: pi0 candidate mass = " << res.M() << std::endl;
      std::cout << "JetValidationTC::reconstruct_pi0: pi0 candidate energy = " << res.E() << std::endl;
      
      double mag = res.Mag2();
      
      std::cout << "JetValidationTC::reconstruct_pi0: pi0 candidate mag2 =  " << mag << std::endl;
      m_npi0++;

    }
  }

  h_npions->Fill(m_npi0);

}


void JetValidationTC::findJetPions(Jet* jet) {
  std::cout << "JetValidationTC::findJetPions: looking for pions in jet with pT = " << jet->get_pt() << std::endl;
  int pions_in_this_jet = 0;

  float jetEta = jet->get_eta();
  float jetPhi = jet->get_phi();
  float jetpT = jet->get_pt();

  for (int pion = 0; pion < m_npi0; pion++){
    float dR = sqrt(pow((jetEta - pi0cand_eta[pion]),2) + pow((jetPhi - pi0cand_phi[pion]),2));
    if(dR < m_maxdR){
      float z = pi0cand_pt[pion] / jetpT;

      m_pionZ.push_back(z);

      m_jetPionPt.push_back(pi0cand_pt[pion]);
      m_jetPionEta.push_back(pi0cand_eta[pion]);
      m_jetPionPhi.push_back(pi0cand_phi[pion]);
      m_jetPionMass.push_back(pi0cand_mass[pion]);
      m_jetPionEnergy.push_back(pi0cand_energy[pion]);

      /*
      auto pionIdx = std::find(m_pionMassJetsRemoved.begin(),m_pionMassJetsRemoved.end(),pi0cand_mass[pion]);

      if (pionIdx != m_pionMassJetsRemoved.end()) {
        std::cout << "Found pion in jet" << std::endl;
        m_pionMassJetsRemoved.erase(pionIdx);
      } else {
        std::cout << "Could not find the pion!" << std::endl;
      }*/


      pions_in_this_jet++;
    }
  }
  if( pions_in_this_jet != 0) {
    m_nPionJets ++;
  }

  std::cout << "JetValidationTC::findJetPions: Number of pions associated with jet = " << pions_in_this_jet << std::endl;
  

  m_jetPionMult.push_back(pions_in_this_jet);
  h_jetPionMult->Fill(pions_in_this_jet);
}
