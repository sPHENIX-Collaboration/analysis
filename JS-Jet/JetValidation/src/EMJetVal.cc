// Modified version of JetValidation.cc for the purpose of subjet analysis -- Jennifer James jennifer.l.james@vanderbilt.eduuti
//module for producing a TTree with jet information for doing jet validation studies
// for questions/bugs please contact Virginia Bailey vbailey13@gsu.edu and myself
#include <EMJetVal.h>
#include <fun4all/Fun4AllBase.h>
#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/PHTFileServer.h>
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>
#include <jetbase/JetMap.h>
#include <jetbase/JetContainer.h>
#include <jetbase/Jetv2.h>
#include <jetbase/Jetv1.h>
#include <jetbase/Jet.h>
#include <jetbase/JetAlgo.h>
#include <jetbase/FastJetAlgo.h>
#include <jetbase/JetInput.h>
#include <jetbase/TowerJetInput.h>
#include <jetbase/JetMapv1.h>
#include <jetbase/JetContainer.h>
#include <jetbase/JetContainerv1.h>
#include <centrality/CentralityInfo.h>
#include <globalvertex/GlobalVertex.h>
#include <globalvertex/GlobalVertexMapv1.h>
#include <calobase/RawTower.h>
#include <calobase/RawTowerContainer.h>
#include <calobase/RawTowerGeom.h>
#include <calobase/RawTowerGeomContainer.h>
#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfo.h>

#include <mbd/MbdOut.h>
#include "fastjet/AreaDefinition.hh"
#include "fastjet/ClusterSequenceArea.hh"
#include "fastjet/Selector.hh"
#include "fastjet/tools/BackgroundEstimatorBase.hh"
#include "fastjet/tools/JetMedianBackgroundEstimator.hh"
#include <fastjet/JetDefinition.hh>
#include "fastjet/ClusterSequence.hh"
#include "fastjet/contrib/SoftDrop.hh" // In external code, this should be fastjet/contrib/SoftDrop.hh  
#include <map>
#include <utility>
#include <cstdlib>  // for exit                                                                                                                                                                 
#include <memory>  // for allocator_traits<>::value_type        
#include <Pythia8/Pythia.h> // Include the Pythia header
#include <jetbackground/TowerBackground.h>

#include <TTree.h>
#include <iostream>
#include <fastjet/PseudoJet.hh>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <vector>

using namespace fastjet;

// ROOT, for histogramming. 
#include "TH1.h"
#include "TH2.h"

// ROOT, for interactive graphics.
#include "TVirtualPad.h"
#include "TApplication.h"

// ROOT, for saving file.
#include "TFile.h"

 

using std::cout;
using std::endl;

//____________________________________________________________________________..
EMJetVal::EMJetVal(const std::string& recojetname, const std::string& truthjetname, const std::string& outputfilename)
  :SubsysReco("EMJetVal_" + recojetname + "_" + truthjetname)
  , m_recoJetName(recojetname)
  , m_vtxZ_cut(10.0)
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
  std::cout << "EMJetVal::EMJetVal(const std::string &name) Calling ctor" << std::endl;
}


//____________________________________________________________________________..
EMJetVal::~EMJetVal()
{
  std::cout << "EMJetVal::~EMJetVal() Calling dtor" << std::endl;
}

//____________________________________________________________________________..
int EMJetVal::Init(PHCompositeNode *topNode)
{
  std::cout << "EMJetVal::Init(PHCompositeNode *topNode) Initializing" << std::endl;
  //PHTFileServer::get().open(m_outputFileName, "RECREATE");
  std::cout << "EMJetVal::Init - Output to " << m_outputFileName << std::endl;
  //Analysis hists
  outFile = new TFile(m_outputFileName.c_str(), "RECREATE");
  _h_R04_z_sj_10_20= new TH1F("R04_z_sj_10_20","z_sj in subjets 1 & 2", 10, 0, 0.5);
  _h_R04_theta_sj_10_20= new TH1F("R04_theta_sj_10_20","theta_sj in subjets 1 & 2", 10, 0, 0.5);
  // softdrop hists
   _h_R04_z_g_10_20= new TH1F("R04_z_g_10_20","z_g in subjets 1 & 2", 10, 0, 0.5);
   _h_R04_theta_g_10_20= new TH1F("R04_theta_g_10_20","theta_g in subjets 1 & 2", 10, 0, 0.5);
  // multiplicity hists
  _hmult_R04= new TH1F("mult_R04","total number of constituents inside R=0.4 jets", 30, 0, 30);
  _hmult_R04_pT_10_20GeV= new TH1F("mult_R04_pT_10_20GeV","total number of constituents inside R=0.4 jets with 10 < p_{T} < 20", 30, 0, 30);
  _hjetpT_R04 = new TH1F("jetpT_R04","jet transverse momentum for R=0.4 jets", 100, 0, 100);
  _hjeteta_R04 = new TH1F("jeteta_R04","jet pseudorapidity for R=0.4 jets", 50, -0.6, 0.6);
  // corellation hists
  correlation_theta_10_20 = new TH2D("correlation_theta_10_20", "Correlation Plot 10 < p_{T} < 20 [GeV/c]; R_{g}; #theta_{sj}", 20, 0, 0.5, 20, 0, 0.5);
  correlation_z_10_20 = new TH2D("correlation_z_10_20", "Correlation Plot; z_{g}; z_{sj}", 20, 0, 0.5, 20, 0, 0.5);   
  // configure Tree

  //PHTFileServer::get().open("hist_jets.root", "RECREATE");
  
  m_T = new TTree("T", "MyJetAnalysis Tree");
  m_T->Branch("m_event", &m_event, "event/I");
  m_T->Branch("nJet", &m_nJet, "nJet/I");
  m_T->Branch("cent", &m_centrality);
  m_T->Branch("b", &m_impactparam);
  m_T->Branch("id", &m_id);
  m_T->Branch("nComponent", &m_nComponent);

  m_T->Branch("eta", &m_eta);
  m_T->Branch("phi", &m_phi);
  m_T->Branch("e", &m_e);
  m_T->Branch("pt", &m_pt);
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
 
  //  std::cout << "finished declaring histos" << std::endl;

  return Fun4AllReturnCodes::EVENT_OK;
}
//____________________________________________________________________________..

int EMJetVal::retrieveEvent(const fastjet::PseudoJet& jet) {
  // Add the PseudoJet to the event vector
  eventVector.push_back(jet);
    
  return 0; // Return 0 for success
}

//____________________________________________________________________________..
int EMJetVal::InitRun(PHCompositeNode *topNode)
{
  std::cout << "EMJetVal::InitRun(PHCompositeNode *topNode) Initializing for Run XXX" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int EMJetVal::process_event(PHCompositeNode *topNode)
{
  // std::cout << "EMJetVal::process_event(PHCompositeNode *topNode) Processing Event " << m_event << std::endl;
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
  JetMap* jetsMC = findNode::getClass<JetMap>(topNode, m_truthJetName);
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
    //  std::cout<<"Can't get background. Exiting"<<std::endl;
    return Fun4AllReturnCodes::EVENT_OK;
  }

  //get the event centrality/impact parameter from HIJING
  m_centrality =  cent_node->get_centile(CentralityInfo::PROP::bimp);
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
  for (auto jet : *jets)// jets //JetMap::Iter iter = jets->begin(); iter != jets->end(); ++iter)
    {

      // std::cout << "working on original jet " << jet->get_id() << " out of " << jets->size() << std::endl;

      if(jet->get_pt() < 1) continue; // to remove noise jets

      
      m_id.push_back(jet->get_id());
      m_nComponent.push_back(jet->size_comp());
      m_eta.push_back(jet->get_eta());
      m_phi.push_back(jet->get_phi());
      m_e.push_back(jet->get_e());
      m_pt.push_back(jet->get_pt());
      
      std::vector<PseudoJet> particles;
      particles.clear();
      
	  float totalPx = 0;
	  float totalPy = 0;
	  float totalPz = 0;
	  float totalE = 0;
	  int nconst = 0;

	  for (auto comp : jet->get_comp_vec()) //Jet::ConstIter comp = jet->begin_comp(); comp != jet->end_comp(); ++comp)
	    {
	      TowerInfo *tower;
	      nconst++;
	      unsigned int channel = comp.second;
	      
	      if (comp.first == 15 || comp.first == 30)
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
		  double ETmp = tower->get_energy();
		  double pt = tower->get_energy() / cosh(tower_eta);
		  if(m_doUnsubJet){
		    ETmp += UE;
		    pt = ETmp / cosh(tower_eta);
		  }
		  double pxTmp = pt * cos(tower_phi);
		  double pyTmp = pt * sin(tower_phi);
		  double pzTmp = pt * sinh(tower_eta);

		  totalE += ETmp;
		  totalPx += pxTmp;
		  totalPy += pyTmp;
		  totalPz += pzTmp;


		  particles.push_back( PseudoJet( pxTmp, pyTmp, pzTmp, ETmp) );

		}//end if over sources

	    }//end loop over constituents
	  //Insert jenn's analysis
		  double radius[5] = {0.05, 0.1, 0.2, 0.4, 0.6}; // jet radius
		  double pseudorapidity = -999.; // pseudorapidity
		  double theta_sj = -1.; // delta radius (value describes an unachievable value)
		  double z_sj = -1.; // delta radius (value describes an unachievable value)
		   
		  //  std::cout << "passed here -397 -my jet defs" << std::endl
		  // Set up FastJet jet finders and modified mass-drop tagger.
		  JetDefinition jetDefAKT_R01( antikt_algorithm, radius[1]);
		  JetDefinition jetDefAKT_R04( antikt_algorithm, radius[3]);
		  JetDefinition jetDefCA(cambridge_algorithm, radius[3]);

		  //  vector<PseudoJet> particles;

		  // Run Fastjet anti-kT algorithm and sort jets in pT order.
		  ClusterSequence clustSeq_R04( particles, jetDefAKT_R04 );
		  std::vector<PseudoJet> sortedJets_R04 = sorted_by_pt( clustSeq_R04.inclusive_jets() );
    
		  //! create a loop to run over the jets -
		  
		  for (int j = 0; j < (int)sortedJets_R04.size(); j++) {
		    
		    //	    std::cout << "working on reclustered jet " << j << " of " << sortedJets_R04.size() << std::endl;

		    PseudoJet jet_reco = sortedJets_R04.at(j);
		    if(fabs(jet_reco.eta()) > 0.6)
		      continue;
      
		    // std::cout << "jet within eta of 0.6" << std::endl; 

		    ClusterSequence clustSeq_R01_con(jet_reco.constituents() , jetDefAKT_R01 );
		    //  std::cout << "made R=0.1 cluster sequence" << std::endl;
		    std:: vector<PseudoJet> sortedJets_R01_con = sorted_by_pt( clustSeq_R01_con.inclusive_jets() );
		    //  std::cout << "ran R=0.1 and sorted by pT" << std::endl;
		    // std::cout << "number of subjets: " << sortedJets_R01_con.size() << std::endl;
		    // std::cout << "passed here -476 -pseudojet" << std::endl
		    if (sortedJets_R01_con.size() < 2){
		      //  std::cout << "not enough subjets" << std::endl;
		      continue;
		    }		      

		    //std:: cout << "at least 2 subjets" << std::endl;

		    PseudoJet sj1 = sortedJets_R01_con.at(0);
		    PseudoJet sj2 = sortedJets_R01_con.at(1);

		    // std::cout << "sj1 pt=" << sj1.pt() << std::endl;
		    // std::cout << "sj2 pt=" << sj2.pt() << std::endl;
		    if (sj1.pt() < 3 || sj2.pt() < 3 )
		      continue;
		    // std::cout << "sj1 pt=" << sj1.pt() << std::endl;
		    // std::cout << "sj2 pt=" << sj2.pt() << std::endl;
		    // std::cout << "both are above 3" << std::endl;
		    
		    theta_sj = sj1.delta_R(sj2);
		    z_sj = sj2.pt()/(sj2.pt()+sj1.pt());

		    // std::cout << "theta_sj = " << theta_sj << "   z_sj = " << z_sj << std::endl;
		   
		    // 10 to 20 pT
		    if (jet_reco.pt() > 10 && jet_reco.pt() < 20 ){
		      // std::cout<<"sorted jets at "<<j<<" the pT = " << jet.pt()<<endl;
		      // std::cout << "jet pt >10 & <20: " << jet_reco.pt() << std::endl;
		      _hjetpT_R04->Fill(jet_reco.perp());
		      pseudorapidity = jet_reco.eta();
		      _hjeteta_R04->Fill(pseudorapidity);
		      _hmult_R04->Fill(jet_reco.constituents().size());

		      ClusterSequence clustSeqCA(jet_reco.constituents(), jetDefCA);
		      std::vector<PseudoJet> cambridgeJets = sorted_by_pt(clustSeqCA.inclusive_jets());

		      // std::cout << "have CA sort jets: " << cambridgeJets.size() << std::endl;

		      // SoftDrop parameters
		      double z_cut = 0.10;
		      double beta = 0.0;
		      contrib::SoftDrop sd(beta, z_cut);
		      //! get subjets
		      if (!isnan(theta_sj) && !isnan(z_sj) && !isinf(theta_sj) && !isinf(z_sj)){
			  _h_R04_z_sj_10_20->Fill(z_sj);
			  _h_R04_theta_sj_10_20->Fill(theta_sj);
			}
		    
		      // std::cout << "filled some histos" << std::endl;
		      // Apply SoftDrop to the jet
		      PseudoJet sd_jet = sd(jet_reco);
		      // std::cout << "ran sd" << std::endl;
		      if (sd_jet == 0)

			continue;
		      // std::cout << "sd jet exists" << std::endl;
		       double delta_R_subjets = sd_jet.structure_of<contrib::SoftDrop>().delta_R();
		       double z_subjets = sd_jet.structure_of<contrib::SoftDrop>().symmetry();

		      _h_R04_z_g_10_20->Fill(z_subjets);
		      _h_R04_theta_g_10_20->Fill(delta_R_subjets);

		      correlation_theta_10_20->Fill(delta_R_subjets, theta_sj);
		      correlation_z_10_20->Fill(z_subjets, z_sj);
		          
		      // SoftDrop failed, handle the case as needed
		      // e.g., skip this jet or perform alternative analysis
		    } else {
		      _hmult_R04_pT_10_20GeV->Fill(jet_reco.constituents().size());
		    }
    
		    //! jet loop
		    // Rjet = 0.4 End

		    //filled nEvent in histogram
		    _hmult_R04->Fill(m_nJet);
		    
		    //		    std::cout << "iZ = " << nEvent << std::endl;
		    // std::cout << "finished jet " << j << std::endl;

		  }//! event loop ends for pT
		  
		  
		  //std::cout << "finished loop over reclustered jets" << std::endl;

		  // End of event loop. Statistics. Histograms. Done.
		  
		  
		  m_nJet++;
		  // std::cout << "m_nJet: " << m_nJet << std::endl;
    }

  // std::cout << "finised loop over original jets" << std::endl;
  
//get truth jets
if(m_doTruthJets)
  {
    m_nTruthJet = 0;
    for (JetMap::Iter iter = jetsMC->begin(); iter != jetsMC->end(); ++iter)
      {
	Jet* truthjet = iter->second;

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
	  int passesCut = jet->get_property(Jet::PROPERTY::prop_SeedItr);
	  m_eta_rawseed.push_back(jet->get_eta());
	  m_phi_rawseed.push_back(jet->get_phi());
	  m_e_rawseed.push_back(jet->get_e());
	  m_pt_rawseed.push_back(jet->get_pt());
	  m_rawseed_cut.push_back(passesCut);
	}

      for (auto jet : *seedjetssub) //JetMap::Iter iter = seedjetssub->begin(); iter != seedjetssub->end(); ++iter)
	{
	  int passesCut = jet->get_property(Jet::PROPERTY::prop_SeedItr);
	  m_eta_subseed.push_back(jet->get_eta());
	  m_phi_subseed.push_back(jet->get_phi());
	  m_e_subseed.push_back(jet->get_e());
	  m_pt_subseed.push_back(jet->get_pt());
	  m_subseed_cut.push_back(passesCut);
	}
    }
 
  //fill the tree

  m_T->Fill();
  //  std::cout << "filled TTree" << std::endl;

  return Fun4AllReturnCodes::EVENT_OK;
}


    //____________________________________________________________________________..
  int EMJetVal::ResetEvent(PHCompositeNode *topNode)
  {
    //std::cout << "EMJetVal::ResetEvent(PHCompositeNode *topNode) Resetting internal structures, prepare for next event" << std::endl;
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
  return Fun4AllReturnCodes::EVENT_OK;

  }
  //____________________________________________________________________________..
  int EMJetVal::EndRun(const int runnumber)
  {
    std::cout << "EMJetVal::EndRun(const int runnumber) Ending Run for Run " << runnumber << std::endl;
    return Fun4AllReturnCodes::EVENT_OK;
  }

  //____________________________________________________________________________..
  int EMJetVal::End(PHCompositeNode *topNode)
  {

    outFile->cd();
    outFile->Write();
    outFile->Close();
    
    std::cout << "EMJetVal::End - Output to " << m_outputFileName << std::endl;


    // PHTFileServer::get().cd(m_outputFileName);

    //m_T->Write();
    std::cout << "EMJetVal::End(PHCompositeNode *topNode) This is the End..." << std::endl;
    return Fun4AllReturnCodes::EVENT_OK;
  }

  //____________________________________________________________________________..
  int EMJetVal::Reset(PHCompositeNode *topNode)
  {
    std::cout << "EMJetVal::Reset(PHCompositeNode *topNode) being Reset" << std::endl;
    return Fun4AllReturnCodes::EVENT_OK;
  }

  //____________________________________________________________________________..
  void EMJetVal::Print(const std::string &what) const
  {
     std::cout << "EMJetVal::Print(const std::string &what) const Printing info for " << what << std::endl;
  }
    

