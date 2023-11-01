#include "JetBkgdSub.h"

#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/PHTFileServer.h>

#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

#include <jetbase/Jet.h>
#include <jetbase/JetMap.h>
#include <jetbase/Jetv1.h>
#include <jetbase/JetAlgo.h>
#include <jetbase/FastJetAlgo.h>
#include <jetbase/JetInput.h>
#include <jetbase/TowerJetInput.h>

#include <calobase/RawTower.h>
#include <calobase/RawTowerContainer.h>
#include <calobase/RawTowerGeom.h>
#include <calobase/RawTowerGeomContainer.h>
#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfo.h>

#include <jetbackground/TowerBackground.h>

#include "fastjet/AreaDefinition.hh"
#include "fastjet/ClusterSequenceArea.hh"
#include "fastjet/Selector.hh"
#include "fastjet/tools/BackgroundEstimatorBase.hh"
#include "fastjet/tools/JetMedianBackgroundEstimator.hh"
#include <fastjet/ClusterSequence.hh>
#include <fastjet/JetDefinition.hh>
#include <fastjet/PseudoJet.hh>

#include <centrality/CentralityInfo.h>

#include <cmath>
#include <map>
#include <utility>
#include <cstdlib>  // for exit
#include <iostream>
#include <memory>  // for allocator_traits<>::value_type
#include <vector>

#include <TTree.h>


//____________________________________________________________________________..
JetBkgdSub::JetBkgdSub(const float jet_R, 
const std::string& recojetname, 
const std::string& rawjetname,
const std::string& truthjetname,
 const std::string& outputfilename)
 : SubsysReco("JetBkgdSub")
  , m_jet_R(jet_R)
  , m_reco_input(recojetname)
  , m_raw_input(rawjetname)
  , m_truth_input(truthjetname)
  , m_outputfilename(outputfilename)
  , m_etaRange(-1.0, 1.0)
  , m_ptRange(0.0, 100000.0)
  , m_event(-1)
  , m_rhoA_jets(0)
  , m_raw_jets(0)
  , m_reco_jets(0)
  , m_truth_jets(0)
  , m_centrality(-1)
  , m_rho_area(-1)
  , m_rho_area_sigma(-1)
  , m_event_leading_truth_pt(-1)
  , m_tree(nullptr)
  , m_reco_eta()
  , m_reco_phi()
  , m_reco_pt()
  , m_reco_pt_unsub()
  , m_rhoA_eta()
  , m_rhoA_phi()
  , m_rhoA_pt()
  , m_rhoA_area()
  , m_rhoA_pt_unsub()
  , m_raw_ncomponent()
  , m_raw_eta()
  , m_raw_phi()
  , m_raw_pt()
  , m_truth_ncomponent()
  , m_truth_eta()
  , m_truth_phi()
  , m_truth_pt()
{
}

//____________________________________________________________________________..
JetBkgdSub::~JetBkgdSub()
{
  for (auto & _input : _inputs)
  {
    delete _input;
  }
  _inputs.clear();
}

//____________________________________________________________________________..
int JetBkgdSub::Init(PHCompositeNode *topNode)
{
  // create output tree
  
  PHTFileServer::get().open(m_outputfilename, "RECREATE");
  m_tree = new TTree("T", "JetBkgdSub");
  m_tree->Branch("event", &m_event, "event/I");
  m_tree->Branch("raw_jets", &m_raw_jets, "raw_jets/I");
  m_tree->Branch("reco_jets", &m_reco_jets, "reco_jets/I");
  m_tree->Branch("rhoA_jets", &m_rhoA_jets, "rhoA_jets/I");
  m_tree->Branch("truth_jets", &m_truth_jets, "truth_jets/I");
  m_tree->Branch("centrality", &m_centrality, "centrality/I");
  m_tree->Branch("rho_area", &m_rho_area, "rho_area/D");
  m_tree->Branch("rho_area_sigma", &m_rho_area_sigma, "rho_area_sigma/D");
  m_tree->Branch("rho_mult", &m_rho_mult, "rho_mult/D");
  m_tree->Branch("event_leading_truth_pt", &m_event_leading_truth_pt, "event_leading_truth_pt/F");
  m_tree->Branch("reco_eta", &m_reco_eta);
  m_tree->Branch("reco_phi", &m_reco_phi);
  m_tree->Branch("reco_pt", &m_reco_pt);
  m_tree->Branch("reco_pt_unsub", &m_reco_pt_unsub);
  m_tree->Branch("raw_ncomponent", &m_raw_ncomponent);
  m_tree->Branch("raw_eta", &m_raw_eta);
  m_tree->Branch("raw_phi", &m_raw_phi);
  m_tree->Branch("raw_pt", &m_raw_pt);
  m_tree->Branch("rhoA_eta", &m_rhoA_eta);
  m_tree->Branch("rhoA_phi", &m_rhoA_phi);
  m_tree->Branch("rhoA_pt", &m_rhoA_pt);
  m_tree->Branch("rhoA_area", &m_rhoA_area);
  m_tree->Branch("rhoA_pt_unsub", &m_rhoA_pt_unsub);
  m_tree->Branch("truth_ncomponent", &m_truth_ncomponent);
  m_tree->Branch("truth_eta", &m_truth_eta);
  m_tree->Branch("truth_phi", &m_truth_phi);
  m_tree->Branch("truth_pt", &m_truth_pt);
  
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int JetBkgdSub::process_event(PHCompositeNode *topNode)
{
  //std::cout << "JetBkgdSub::process_event(PHCompositeNode *topNode) Processing Event" << std::endl;
  ++m_event;

  // Get centrality info
  //==================================
  CentralityInfo* cent_node = findNode::getClass<CentralityInfo>(topNode, "CentralityInfo");
  if (!cent_node)
  {
      std::cout << "JetBkgdSub::process_event() ERROR: Can't find CentralityInfo" << std::endl;
      exit(-1);
  }
  m_centrality = cent_node->get_centile(CentralityInfo::PROP::bimp);

  // Leading truth jet pt (R = 0.4)
  m_event_leading_truth_pt = 0;
  m_event_leading_truth_pt = LeadingR04TruthJet(topNode);

  EstimateRhoMult(topNode);
  if(m_rho_mult < 0 ) return Fun4AllReturnCodes::EVENT_OK;
  // ==================================
  // Get truth jet info
  //==================================

  JetMap *truthjets = findNode::getClass<JetMap>(topNode, m_truth_input);
  if(!truthjets) 
  {
    std::cout << "JetBkgdSub::process_event(PHCompositeNode *topNode) Could not find truth jet nodes" << std::endl;
    exit(-1);
  }

  m_truth_jets = 0;
  
  for(JetMap::Iter iter = truthjets->begin(); iter != truthjets->end(); ++iter){

    Jet *jet = iter->second;
    bool eta_cut = (jet->get_eta() >= m_etaRange.first) and (jet->get_eta() <= m_etaRange.second);
    bool pt_cut = (jet->get_pt() >= m_ptRange.first) and (jet->get_pt() <= m_ptRange.second);
    if (not eta_cut or not pt_cut) continue;
    m_truth_eta.push_back(jet->get_eta());
    m_truth_phi.push_back(jet->get_phi());
    m_truth_pt.push_back(jet->get_pt());
    m_truth_ncomponent.push_back(jet->size_comp());
    m_truth_jets++;
  }

  //==================================
  // Get reco jet info
  //==================================
  JetMap *recojets = findNode::getClass<JetMap>(topNode, m_reco_input);
  if(!recojets) 
  {
    std::cout << "JetTree::process_event(PHCompositeNode *topNode) Could not find reco jet nodes" << std::endl;
    exit(-1);
  }
  
  
  // get tower nodes and geometry
  TowerInfoContainer *towersEM3 = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_CEMC_RETOWER_SUB1");
  TowerInfoContainer *towersIH3 = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALIN_SUB1");
  TowerInfoContainer *towersOH3 = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALOUT_SUB1");
  RawTowerGeomContainer *tower_geom = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALIN");
  RawTowerGeomContainer *tower_geomOH = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALOUT");
  if(!towersEM3 || !towersIH3 || !towersOH3){
    std::cout <<  "JetTree::process_event(PHCompositeNode *topNode) Could not find tower nodes" << std::endl;
    exit(-1);
  }
  if(!tower_geom || !tower_geomOH){
    std::cout <<  "JetTree::process_event(PHCompositeNode *topNode) Could not find tower geometry nodes" << std::endl;
    exit(-1);
  }
  //underlying event
  TowerBackground *background = findNode::getClass<TowerBackground>(topNode, "TowerInfoBackground_Sub2");
  if(!background){
    std::cout <<  "JetTree::process_event(PHCompositeNode *topNode) Could not find background node" << std::endl;
    return Fun4AllReturnCodes::EVENT_OK;
  }
  float background_v2 = background->get_v2();
  float background_Psi2 = background->get_Psi2();

  m_reco_jets = 0;
  for(JetMap::Iter iter = recojets->begin(); iter != recojets->end(); ++iter){

    Jet *jet = iter->second;
    if(jet->get_pt() < 1.0) continue;
    m_reco_eta.push_back(jet->get_eta());
    m_reco_phi.push_back(jet->get_phi());
    m_reco_pt.push_back(jet->get_pt());

    Jet* unsubjet = new Jetv1();
	  float totalPx = 0;
	  float totalPy = 0;
	  float totalPz = 0;
	  float totalE = 0;

	  for (Jet::ConstIter comp = jet->begin_comp(); comp != jet->end_comp(); ++comp)
	  {
      TowerInfo *tower;
      unsigned int channel = (*comp).second;
      
      if ((*comp).first == 15 ||  (*comp).first == 30)
      {

        tower = towersIH3->get_tower_at_channel(channel);
        if(!tower || !tower_geom) continue;
  
        unsigned int calokey = towersIH3->encode_key(channel);
        int ieta = towersIH3->getTowerEtaBin(calokey);
        int iphi = towersIH3->getTowerPhiBin(calokey);
        const RawTowerDefs::keytype key = RawTowerDefs::encode_towerid(RawTowerDefs::CalorimeterId::HCALIN, ieta, iphi);
        float UE = background->get_UE(1).at(ieta);
        float tower_phi = tower_geom->get_tower_geometry(key)->get_phi();
        float tower_eta = tower_geom->get_tower_geometry(key)->get_eta();

		    UE = UE * (1 + 2 * background_v2 * cos(2 * (tower_phi - background_Psi2)));
        totalE += tower->get_energy() + UE;
        double pt = tower->get_energy() / cosh(tower_eta);
        totalPx += pt * cos(tower_phi);
        totalPy += pt * sin(tower_phi);
        totalPz += pt * sinh(tower_eta);
		  }
	    else if ((*comp).first == 16 || (*comp).first == 31)
		  {
		    tower = towersOH3->get_tower_at_channel(channel);
		    if(!tower || !tower_geomOH)  continue;
		  
        unsigned int calokey = towersOH3->encode_key(channel);
        int ieta = towersOH3->getTowerEtaBin(calokey);
        int iphi = towersOH3->getTowerPhiBin(calokey);
        const RawTowerDefs::keytype key = RawTowerDefs::encode_towerid(RawTowerDefs::CalorimeterId::HCALOUT, ieta, iphi);
        float UE = background->get_UE(2).at(ieta);
        float tower_phi = tower_geomOH->get_tower_geometry(key)->get_phi();
        float tower_eta = tower_geomOH->get_tower_geometry(key)->get_eta();

        UE = UE * (1 + 2 * background_v2 * cos(2 * (tower_phi - background_Psi2)));
        totalE +=tower->get_energy() + UE;
        double pt = tower->get_energy() / cosh(tower_eta);
        totalPx += pt * cos(tower_phi);
        totalPy += pt * sin(tower_phi);
        totalPz += pt * sinh(tower_eta);
		  }
	    else if ((*comp).first == 14 || (*comp).first == 29)
		  {
		    tower = towersEM3->get_tower_at_channel(channel);
		    if(!tower || !tower_geom) continue;
		    unsigned int calokey = towersEM3->encode_key(channel);
        int ieta = towersEM3->getTowerEtaBin(calokey);
        int iphi = towersEM3->getTowerPhiBin(calokey);
        const RawTowerDefs::keytype key = RawTowerDefs::encode_towerid(RawTowerDefs::CalorimeterId::HCALIN, ieta, iphi);
        float UE = background->get_UE(0).at(ieta);
        float tower_phi = tower_geom->get_tower_geometry(key)->get_phi();
        float tower_eta = tower_geom->get_tower_geometry(key)->get_eta();

        UE = UE * (1 + 2 * background_v2 * cos(2 * (tower_phi - background_Psi2)));
        totalE +=tower->get_energy() + UE;
        double pt = tower->get_energy() / cosh(tower_eta);
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
	  m_reco_pt_unsub.push_back(unsubjet->get_pt());
    m_reco_jets++;
  }


  //==================================
  // Get raw jet info
  //==================================

  JetMap *rawjets = findNode::getClass<JetMap>(topNode, m_raw_input);
  if(!rawjets) 
  {
    std::cout << "JetTree::process_event(PHCompositeNode *topNode) Could not find raw jet nodes" << std::endl;
    exit(-1);
  }
  m_raw_jets = 0;
  for(JetMap::Iter iter = rawjets->begin(); iter != rawjets->end(); ++iter){

    Jet *jet = iter->second;
    if(jet->get_pt() < 1.0) continue;
    m_raw_ncomponent.push_back(jet->size_comp());
    m_raw_eta.push_back(jet->get_eta());
    m_raw_phi.push_back(jet->get_phi());
    m_raw_pt.push_back(jet->get_pt());
    m_raw_jets++;
  }

  //==================================
  // Get rhoA jet info
  //==================================

  std::vector<fastjet::PseudoJet> calo_pseudojets;
  for (auto & _input : _inputs)
  {
    std::vector<Jet *> parts = _input->get_input(topNode);
    for (unsigned int i = 0; i < parts.size(); ++i)
    {
      auto& part = parts[i];
      float this_e = part->get_e();
      if (this_e == 0.) continue;
      float this_px = parts[i]->get_px();
      float this_py = parts[i]->get_py();
      float this_pz = parts[i]->get_pz();
      if (this_e < 0)
      {
        // make energy = +1 MeV for purposes of clustering
        float e_ratio = 0.001 / this_e;
        this_e  = this_e * e_ratio;
        this_px = this_px * e_ratio;
        this_py = this_py * e_ratio;
        this_pz = this_pz * e_ratio;
      }
      fastjet::PseudoJet pseudojet(this_px, this_py, this_pz, this_e);
      calo_pseudojets.push_back(pseudojet);
    }
    for (auto &p : parts) delete p;

  }

  const double ghost_max_rap = 1.1;
  const double ghost_R = 0.01;
  const double max_jet_rap = 1.1; // - m_jet_R;

  fastjet::Selector jetrap         = fastjet::SelectorAbsEtaMax(max_jet_rap);
  fastjet::Selector not_pure_ghost = !fastjet::SelectorIsPureGhost();

  fastjet::Selector selection  = jetrap && not_pure_ghost;
  fastjet::AreaDefinition area_def(fastjet::active_area_explicit_ghosts, fastjet::GhostedAreaSpec(ghost_max_rap, 1, ghost_R));
  fastjet::JetDefinition jet_def_antikt(fastjet::antikt_algorithm, m_jet_R);
  fastjet::JetDefinition jet_def_bkgd(fastjet::kt_algorithm, m_jet_R); 
  fastjet::Selector selector_rm2 = jetrap * (!fastjet::SelectorNHardest(2));
  fastjet::JetMedianBackgroundEstimator bge_rm2 {selector_rm2, jet_def_bkgd, area_def};
  bge_rm2.set_particles(calo_pseudojets);
  // rho and sigma
  m_rho_area = bge_rm2.rho();
  m_rho_area_sigma = bge_rm2.sigma();

  fastjet::ClusterSequenceArea clustSeq(calo_pseudojets, jet_def_antikt, area_def);
  std::vector<fastjet::PseudoJet> jets =  fastjet::sorted_by_pt( selection( clustSeq.inclusive_jets(5.) ));

  m_rhoA_jets = 0;
  for (auto jet : jets) {
    if (jet.pt() < 1.0) continue;
    m_rhoA_eta.push_back(jet.eta());
    m_rhoA_phi.push_back(jet.phi_std());
    m_rhoA_pt_unsub.push_back(jet.pt());
    m_rhoA_area.push_back(jet.area());
    m_rhoA_pt.push_back(jet.pt() - jet.area() * m_rho_area);
    m_rhoA_jets++;
  }


  //==================================
  // Fill tree
  //==================================
  m_tree->Fill();


  return Fun4AllReturnCodes::EVENT_OK;
}


int JetBkgdSub::ResetEvent(PHCompositeNode *topNode)
{
  
  m_rhoA_eta.clear();
  m_rhoA_phi.clear();
  m_rhoA_pt.clear();
  m_rhoA_area.clear();
  m_rhoA_pt_unsub.clear();

  m_reco_eta.clear();
  m_reco_phi.clear();
  m_reco_pt.clear();
  m_reco_pt_unsub.clear();

  // raw jet variables
  m_raw_ncomponent.clear();
  m_raw_eta.clear();
  m_raw_phi.clear();
  m_raw_pt.clear();

  // truth jet variables
  m_truth_ncomponent.clear();
  m_truth_eta.clear();
  m_truth_phi.clear();
  m_truth_pt.clear();


  return Fun4AllReturnCodes::EVENT_OK;
}

float JetBkgdSub::LeadingR04TruthJet(PHCompositeNode *topNode){

  // get truth jet nodes
  JetMap *truthjets = findNode::getClass<JetMap>(topNode, "AntiKt_Truth_r04");
  if(!truthjets) 
  {
    std::cout << "JetTree::LeadingR04TruthJet(PHCompositeNode *topNode) Could not find truth jet nodes" << std::endl;
    exit(-1);
  }

  float leading_truth_pt = -1;
  for(JetMap::Iter iter = truthjets->begin(); iter != truthjets->end(); ++iter){

    Jet *jet = iter->second;
    if(jet->get_pt() > leading_truth_pt) leading_truth_pt = jet->get_pt();
  }

  return leading_truth_pt;

}

void JetBkgdSub::EstimateRhoMult(PHCompositeNode *topNode){

  //==================================
  // Estimate rho
  //==================================
  m_rho_mult = -1;
   
  // reconstruct kT jets
  std::vector<Jet *> inputs;  // owns memory
  for (auto & _input : _inputs)
  {
    std::vector<Jet *> parts = _input->get_input(topNode);
    for (auto & part : parts)
    {
      inputs.push_back(part);
      inputs.back()->set_id(inputs.size() - 1);  // unique ids ensured
    }
  }

  // define kT jet algorithm
  JetAlgo *kt_algo = new FastJetAlgo(Jet::KT, 0.4);
  std::vector<Jet *> kt_jets = kt_algo->get_jets(inputs);  // owns memory

  if(kt_jets.size() < 2)
  {
    std::cout << "JetTree::process_event(PHCompositeNode *topNode) No kT jets found" << std::endl;
    return;
  }

  // estimate rho
  std::vector<double> pt_over_nConstituents;
  // loop over kT jets
  int nktjets = 0;
  for(unsigned int iktjet = 0; iktjet < kt_jets.size(); iktjet++)
  {
    if (iktjet <= 1) continue; // skip the two leading jets

    double jet_pt = kt_jets.at(iktjet)->get_pt();
    int jet_size = kt_jets.at(iktjet)->size_comp();
    if(jet_pt < 1.0) continue; // only consider jets with pt > 1.0
    if(jet_size == 0) continue; // only consider jets with constituents
    pt_over_nConstituents.push_back(jet_pt/jet_size);
    nktjets++;
  } 

  if(nktjets == 0)
  {
    std::cout << "JetTree::process_event(PHCompositeNode *topNode) No kT jets found with pt > 1.0" << std::endl;
    return;
  }

  // get median of all vectors
  m_rho_mult = GetMedian(pt_over_nConstituents);
  return;
}

double JetBkgdSub::GetMedian(std::vector<double> &vec)
{
  double median = 0;
  if(vec.size() == 0) return median;
  // sort vector
  std::sort(vec.begin(), vec.end());
  int size = vec.size();
  if(size % 2 == 0)
  {
    median = (vec.at(size/2) + vec.at(size/2 - 1))/2;
  }
  else
  {
    median = vec.at(size/2);
  }
  return median;
}

int JetBkgdSub::End(PHCompositeNode *topNode)
{
  std::cout << "JetBkgdSub::End - Output to " << m_outputfilename << std::endl;
    // write tree to file
  PHTFileServer::get().cd(m_outputfilename);
  // m_tree->Write();
  // write file to disk
  PHTFileServer::get().write(m_outputfilename);
  std::cout << "JetBkgdSub::End(PHCompositeNode *topNode) This is the End..." << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}



