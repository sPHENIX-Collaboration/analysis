//module for producing a TTree with jet information for doing jet validation studies
// for questions/bugs please contact Virginia Bailey vbailey13@gsu.edu
#include <fun4all/Fun4AllBase.h>
#include <JetValidation.h>
#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/PHTFileServer.h>
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
#include <ffarawobjects/Gl1Packet.h>
#include <jetbackground/TowerBackground.h>
#include <calobase/RawCluster.h>
#include <calobase/RawClusterContainer.h>
#include <calobase/RawClusterUtility.h>
#include <calobase/RawTowerDefs.h>

#include <TTree.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <vector>

#include "TH1.h"
#include "TH2.h"
#include "TFile.h"
#include <TTree.h>

//____________________________________________________________________________..
JetValidation::JetValidation(const std::string& recojetname, const std::string& truthjetname, const std::string& outputfilename):
  SubsysReco("JetValidation_" + recojetname + "_" + truthjetname)
  , m_recoJetName(recojetname)
  , m_truthJetName(truthjetname)
  , m_outputFileName(outputfilename)
  , m_etaRange(-1, 1)
  , m_ptRange(5, 100)
  , m_doTruthJets(0)
  , m_doSeeds(0)
  , m_doUnsubJet(0)
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
  , m_cleta()
  , m_clphi()
  , m_cle()
  , m_clecore()
  , m_clpt()
  , m_clprob()
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
  std::cout << "JetValidation::JetValidation(const std::string &name) Calling ctor" << std::endl;
}

//____________________________________________________________________________..
JetValidation::~JetValidation()
{
  std::cout << "JetValidation::~JetValidation() Calling dtor" << std::endl;
}

//____________________________________________________________________________..
//____________________________________________________________________________..
int JetValidation::Init(PHCompositeNode *topNode)
{
  std::cout << "JetValidation::Init(PHCompositeNode *topNode) Initializing" << std::endl;
  PHTFileServer::get().open(m_outputFileName, "RECREATE");
  std::cout << "JetValidation::Init - Output to " << m_outputFileName << std::endl;

  // configure Tree
  m_T = new TTree("T", "MyJetAnalysis Tree");
  m_T->Branch("m_event", &m_event, "event/I");
  m_T->Branch("nJet", &m_nJet, "nJet/I");
  m_T->Branch("cent", &m_centrality);
  m_T->Branch("zvtx", &m_zvtx);
  m_T->Branch("b", &m_impactparam);
  m_T->Branch("id", &m_id);
  m_T->Branch("nComponent", &m_nComponent);
  m_T->Branch("triggerVector", &m_triggerVector);

  m_T->Branch("eta", &m_eta);
  m_T->Branch("phi", &m_phi);
  m_T->Branch("e", &m_e);
  m_T->Branch("pt", &m_pt);

  m_T->Branch("cleta", &m_cleta);
  m_T->Branch("clphi", &m_clphi);
  m_T->Branch("cle", &m_cle);
  m_T->Branch("clecore", &m_clecore);
  m_T->Branch("clpt", &m_clpt);
  m_T->Branch("clprob", &m_clprob);

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
int JetValidation::InitRun(PHCompositeNode *topNode)
{
  std::cout << "JetValidation::InitRun(PHCompositeNode *topNode) Initializing for Run XXX" << std::endl;
  
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int JetValidation::process_event(PHCompositeNode *topNode)
{
  //  std::cout << "JetValidation::process_event(PHCompositeNode *topNode) Processing Event" << std::endl;
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
  else
    {
      GlobalVertex *vtx = vertexmap->begin()->second;
      m_zvtx = vtx->get_z();
    }

  //calorimeter towers
  TowerInfoContainer *towersEM3 = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_CEMC_RETOWER_SUB1");
  TowerInfoContainer *towersIH3 = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALIN_SUB1");
  TowerInfoContainer *towersOH3 = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALOUT_SUB1");
  RawTowerGeomContainer *tower_geom = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALIN");
  RawTowerGeomContainer *tower_geomOH = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALOUT");
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
  //m_centrality =  cent_node->get_centile(CentralityInfo::PROP::mbd_NS);
  m_centrality = (int)(100*cent_node->get_centile(CentralityInfo::PROP::mbd_NS));
  m_impactparam =  cent_node->get_quantity(CentralityInfo::PROP::bimp);

  //get reco jets
  m_nJet = 0;
  float background_v2 = 0;
  float background_Psi2 = 0;
  if(m_doUnsubJet)
    {
      background_v2 = background->get_v2();
      background_Psi2 = background->get_Psi2();
    }

  for (auto jet : *jets)
    {

      if(jet->get_pt() < 1) continue; // to remove noise jets

      m_id.push_back(jet->get_id());
      m_nComponent.push_back(jet->size_comp());
      m_eta.push_back(jet->get_eta());
      m_phi.push_back(jet->get_phi());
      m_e.push_back(jet->get_e());
      m_pt.push_back(jet->get_pt());
      
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

  //grab the gl1 data
  Gl1Packet *gl1PacketInfo = findNode::getClass<Gl1Packet>(topNode, "GL1Packet");
  if (!gl1PacketInfo)
    {
      std::cout << PHWHERE << "caloTreeGen::process_event: GL1Packet node is missing. Output related to this node will be empty" << std::endl;
    }

  if (gl1PacketInfo)
    {
      uint64_t triggervec = gl1PacketInfo->getTriggerVector();
      for (int i = 0; i < 64; i++)
	{
	  bool trig_decision = ((triggervec & 0x1U) == 0x1U);
	  m_triggerVector.push_back(trig_decision);
	  triggervec = (triggervec >> 1U) & 0xffffffffU;
	}
    }

  //get clusters
  RawClusterContainer *clusterContainer = findNode::getClass<RawClusterContainer>(topNode, "CLUSTERINFO_CEMC");
  if (!clusterContainer)
    {
      std::cout << PHWHERE << "caloTreeGen::process_event: CLUSTERINFO_CEMC node is missing. Output related to this node will be empty" << std::endl;
      return 0;
    }
  RawClusterContainer::ConstRange clusterEnd = clusterContainer->getClusters();
  RawClusterContainer::ConstIterator clusterIter;
  for (clusterIter = clusterEnd.first; clusterIter != clusterEnd.second; clusterIter++)
    {
      RawCluster *recoCluster = clusterIter->second;

      CLHEP::Hep3Vector vertex(0, 0, 0);
      if (m_zvtx != -9999)
	{
	  vertex.setZ(m_zvtx);
	}
      CLHEP::Hep3Vector E_vec_cluster = RawClusterUtility::GetECoreVec(*recoCluster, vertex);
      CLHEP::Hep3Vector E_vec_cluster_Full = RawClusterUtility::GetEVec(*recoCluster, vertex);

      float clusE = E_vec_cluster_Full.mag();
      float clusEcore = E_vec_cluster.mag();
      float clus_eta = E_vec_cluster.pseudoRapidity();
      float clus_phi = E_vec_cluster.phi();
      float clus_pt = E_vec_cluster.perp();
      float clus_prob = recoCluster->get_prob();
      
      m_cle.push_back(clusE);
      m_clecore.push_back(clusEcore);
      m_cleta.push_back(clus_eta);
      m_clphi.push_back(clus_phi);
      m_clpt.push_back(clus_pt);
      m_clprob.push_back(clus_prob);
    }
  
  //fill the tree
  m_T->Fill();

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int JetValidation::ResetEvent(PHCompositeNode *topNode)
{
  //std::cout << "JetValidation::ResetEvent(PHCompositeNode *topNode) Resetting internal structures, prepare for next event" << std::endl;
  m_id.clear();
  m_nComponent.clear();
  m_eta.clear();
  m_phi.clear();
  m_e.clear();
  m_pt.clear();
  m_unsub_pt.clear();
  m_sub_et.clear();

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
  
  m_triggerVector.clear();

  m_cle.clear();
  m_clecore.clear();
  m_cleta.clear();
  m_clphi.clear();
  m_clpt.clear();
  m_clprob.clear();
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int JetValidation::EndRun(const int runnumber)
{
  std::cout << "JetValidation::EndRun(const int runnumber) Ending Run for Run " << runnumber << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int JetValidation::End(PHCompositeNode *topNode)
{
  std::cout << "JetValidation::End - Output to " << m_outputFileName << std::endl;
  PHTFileServer::get().cd(m_outputFileName);

  m_T->Write();
  std::cout << "JetValidation::End(PHCompositeNode *topNode) This is the End..." << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int JetValidation::Reset(PHCompositeNode *topNode)
{
  std::cout << "JetValidation::Reset(PHCompositeNode *topNode) being Reset" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
void JetValidation::Print(const std::string &what) const
{
  std::cout << "JetValidation::Print(const std::string &what) const Printing info for " << what << std::endl;
}
