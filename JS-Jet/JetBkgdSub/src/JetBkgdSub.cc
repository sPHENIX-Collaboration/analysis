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
#include <jetbase/JetMapv1.h>
#include <jetbase/JetContainer.h>
#include <jetbase/JetContainerv1.h>
#include <jetbase/Jetv2.h>

#include <calobase/RawTower.h>
#include <calobase/RawTowerContainer.h>
#include <calobase/RawTowerGeom.h>
#include <calobase/RawTowerGeomContainer.h>
#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfo.h>

#include <globalvertex/GlobalVertexMap.h>
#include <globalvertex/GlobalVertexMapv1.h>

#include <jetbackground/TowerBackground.h>

#include <mbd/MbdOut.h>

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


JetBkgdSub::JetBkgdSub(const double jet_R, const std::string& outputfilename)
 : SubsysReco("JetBkgdSub")
  , m_jet_R(jet_R)
  , m_outputfilename(outputfilename)
  , m_etaRange(-1.1, 1.1)
  , m_ptRange(0.0, 100000.0)
  , _minrecopT(5.0)
  , m_vtxZ_cut(10.0)
  , _doIterative(false)
  , _doAreaSub(false)
  , _doMultSub(false)
  , _doTruth(false)
  , _doData(false)
  , _doEmbed(false)
  , _doTowerECut(false)
  , m_event(-1)
  , m_rhoA_jets(0)
  , m_mult_jets(0)
  , m_iter_jets(0)
  , m_truth_jets(0)
  , m_centrality(-1)
  , m_mbd_NS(0.0)
  , m_rho_area(-1)
  , m_rho_area_sigma(-1)
  , m_rho_area_CEMC(-1)
  , m_rho_area_sigma_CEMC(-1)
  , m_rho_area_IHCAL(-1)
  , m_rho_area_sigma_IHCAL(-1)
  , m_rho_area_OHCAL(-1)
  , m_rho_area_sigma_OHCAL(-1)
  , m_event_leading_truth_pt(-1)
  , m_tree(nullptr)
  , m_iter_eta()
  , m_iter_phi()
  , m_iter_pt()
  , m_iter_pt_unsub()
  , m_rhoA_eta()
  , m_rhoA_phi()
  , m_rhoA_pt()
  , m_rhoA_area()
  , m_rhoA_pt_unsub()
  , m_mult_ncomponent()
  , m_mult_nsignal()
  , m_mult_eta()
  , m_mult_phi()
  , m_mult_pt()
  , m_mult_pt_unsub()
  , m_truth_ncomponent()
  , m_truth_eta()
  , m_truth_phi()
  , m_truth_pt()
{
}

JetBkgdSub::~JetBkgdSub()
{
  for (auto & _input : _inputs)
  {
    delete _input;
  }
  _inputs.clear();
}

int JetBkgdSub::Init(PHCompositeNode *topNode)
{
  // set up jet inputs
  if(_doTruth)
  {
      m_truth_input = "AntiKt_Truth_r0" + std::to_string((int) (m_jet_R * 10));
  }
  // if(_doAreaSub)
  // {
  //   // these jets don't exist yet on the node tree
  // }
  if(_doMultSub){
    // raw jet input
    m_raw_input = "AntiKt_Tower_r0" + std::to_string((int) (m_jet_R * 10));
  }
  if(_doIterative)
  {
    m_iter_input = "AntiKt_Tower_r0" + std::to_string((int) (m_jet_R * 10))+ "_Sub1";
  }


  // create output tree
  PHTFileServer::get().open(m_outputfilename, "RECREATE");
  m_tree = new TTree("T", "JetBkgdSub");
  m_tree->Branch("event", &m_event, "event/I");
  m_tree->Branch("event_leading_truth_pt", &m_event_leading_truth_pt, "event_leading_truth_pt/F");
  m_tree->Branch("centrality", &m_centrality, "centrality/I");
  m_tree->Branch("mbd_NS", &m_mbd_NS, "mbd_NS/D");
  if(_doTruth)
  {
    m_tree->Branch("truth_jets", &m_truth_jets, "truth_jets/I");
    m_tree->Branch("truth_ncomponent", &m_truth_ncomponent);
    m_tree->Branch("truth_eta", &m_truth_eta);
    m_tree->Branch("truth_phi", &m_truth_phi);
    m_tree->Branch("truth_pt", &m_truth_pt);
  }
  if(_doAreaSub)
  {
    m_tree->Branch("rho_area", &m_rho_area, "rho_area/D");
    m_tree->Branch("rho_area_sigma", &m_rho_area_sigma, "rho_area_sigma/D");
    m_tree->Branch("rho_area_CEMC", &m_rho_area_CEMC, "rho_area_CEMC/D");
    m_tree->Branch("rho_area_sigma_CEMC", &m_rho_area_sigma_CEMC, "rho_area_sigma_CEMC/D");
    m_tree->Branch("rho_area_IHCAL", &m_rho_area_IHCAL, "rho_area_IHCAL/D");
    m_tree->Branch("rho_area_sigma_IHCAL", &m_rho_area_sigma_IHCAL, "rho_area_sigma_IHCAL/D");
    m_tree->Branch("rho_area_OHCAL", &m_rho_area_OHCAL, "rho_area_OHCAL/D");
    m_tree->Branch("rho_area_sigma_OHCAL", &m_rho_area_sigma_OHCAL, "rho_area_sigma_OHCAL/D");
    m_tree->Branch("rhoA_jets", &m_rhoA_jets, "rhoA_jets/I");
    m_tree->Branch("rhoA_eta", &m_rhoA_eta);
    m_tree->Branch("rhoA_phi", &m_rhoA_phi);
    m_tree->Branch("rhoA_pt", &m_rhoA_pt);
    m_tree->Branch("rhoA_area", &m_rhoA_area);
    m_tree->Branch("rhoA_pt_unsub", &m_rhoA_pt_unsub);

  }
  if(_doMultSub)
  {
    m_tree->Branch("rho_mult", &m_rho_mult, "rho_mult/D");
    m_tree->Branch("mult_jets", &m_mult_jets, "mult_jets/I");
    m_tree->Branch("mult_ncomponent", &m_mult_ncomponent);
    m_tree->Branch("mult_nsignal", &m_mult_nsignal);
    m_tree->Branch("mult_eta", &m_mult_eta);
    m_tree->Branch("mult_phi", &m_mult_phi);
    m_tree->Branch("mult_pt", &m_mult_pt);
    m_tree->Branch("mult_pt_unsub", &m_mult_pt_unsub);
  }
  if(_doIterative)
  {
    m_tree->Branch("iter_jets", &m_iter_jets, "iter_jets/I");
    m_tree->Branch("iter_eta", &m_iter_eta);
    m_tree->Branch("iter_phi", &m_iter_phi);
    m_tree->Branch("iter_pt", &m_iter_pt);
    m_tree->Branch("iter_pt_unsub", &m_iter_pt_unsub);
  }

  std::cout << "JetBkgdSub::Init(PHCompositeNode *topNode) Initialization successfull" << std::endl;

  //cout all settings
  std::cout << "JetBkgdSub::Init(PHCompositeNode *topNode) Settings:" << std::endl;
  std::cout << "JetBkgdSub::Init(PHCompositeNode *topNode) Jet R: " << m_jet_R << std::endl;
  std::cout << "JetBkgdSub::Init(PHCompositeNode *topNode) Output file: " << m_outputfilename << std::endl;
  std::cout << "JetBkgdSub::Init(PHCompositeNode *topNode) Eta range: " << m_etaRange.first << " - " << m_etaRange.second << std::endl;
  std::cout << "JetBkgdSub::Init(PHCompositeNode *topNode) Pt range: " << m_ptRange.first << " - " << m_ptRange.second << std::endl;
  std::cout << "JetBkgdSub::Init(PHCompositeNode *topNode) Min reco jet pt: " << _minrecopT << std::endl;
  std::cout << "JetBkgdSub::Init(PHCompositeNode *topNode) Do iterative subtraction: " << _doIterative << std::endl;
  std::cout << "JetBkgdSub::Init(PHCompositeNode *topNode) Do area subtraction: " << _doAreaSub << std::endl;
  std::cout << "JetBkgdSub::Init(PHCompositeNode *topNode) Do mult subtraction: " << _doMultSub << std::endl;
  std::cout << "JetBkgdSub::Init(PHCompositeNode *topNode) Do truth jets: " << _doTruth << std::endl;
  std::cout << "JetBkgdSub::Init(PHCompositeNode *topNode) Truth jet input: " << m_truth_input << std::endl;
  std::cout << "JetBkgdSub::Init(PHCompositeNode *topNode) Raw jet input: " << m_raw_input << std::endl;
  std::cout << "JetBkgdSub::Init(PHCompositeNode *topNode) Iterative jet input: " << m_iter_input << std::endl;
  std::cout << "JetBkgdSub::Init(PHCompositeNode *topNode) Output tree: " << m_tree->GetName() << std::endl;

  return Fun4AllReturnCodes::EVENT_OK;
}

int JetBkgdSub::process_event(PHCompositeNode *topNode)
{
  // std::cout << "JetBkgdSub::process_event(PHCompositeNode *topNode) Processing event " << m_event << std::endl;
  ++m_event;

  GlobalVertexMap *vtxMap = findNode::getClass<GlobalVertexMapv1>(topNode,"GlobalVertexMap");
  if (!vtxMap)
    {
      if(Verbosity()) std::cout << "JetBkgdSub::processEvent(PHCompositeNode *topNode) Could not find global vertex map node" << std::endl;
      exit(-1);
    }
  if (!vtxMap->get(0))
    {
      if(Verbosity()) std::cout << "no vertex found" << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }
  if (fabs(vtxMap->get(0)->get_z()) > m_vtxZ_cut)
    {
      if(Verbosity()) std::cout << "vertex not in range" << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }

  // min reco jet pt cut 
  // sets min for all subtraction types
  double min_reco_jet_pt = _minrecopT;

  //==================================
  // Get centrality info
  //==================================
  GetCentInfo(topNode);
  // std::cout << "JetBkgdSub::process_event(PHCompositeNode *topNode) Centrality: " << m_centrality << std::endl;

  // Leading truth jet pt (R = 0.4) (for simulation event selection)
  if (_doData || _doEmbed || !_doTruth)
    {
      m_event_leading_truth_pt = -1.0;
    }
  else
    {
      m_event_leading_truth_pt = LeadingR04TruthJet(topNode);
    }
  // std::cout << "JetBkgdSub::process_event(PHCompositeNode *topNode) Leading truth jet pt: " << m_event_leading_truth_pt << std::endl;

  // ==================================
  // Get truth jet info
  //==================================
  if(_doTruth)
  {
    JetMap *truthjets = findNode::getClass<JetMap>(topNode, m_truth_input);
    if(!truthjets) 
    {
      std::cout << "JetBkgdSub::process_event(PHCompositeNode *topNode) Could not find truth jet nodes" << std::endl;
      exit(-1);
    }

    m_truth_jets = 0; // loop over truth jets
    for(JetMap::Iter iter = truthjets->begin(); iter != truthjets->end(); ++iter)
    {
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

  }
  
  //==================================
  // Get iter jet info
  //==================================
  if(_doIterative)
  { 
    std::cout << "JetBkgdSub::process_event(PHCompositeNode *topNode) Iterative subtraction" << std::endl;
    JetContainer *iterjets = findNode::getClass<JetContainer>(topNode, m_iter_input);
    if(!iterjets) 
    {
      std::cout << "JetTree::process_event(PHCompositeNode *topNode) Could not find iter jet nodes" << std::endl;
      exit(-1);
    }
    
    
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

    m_iter_jets = 0;
    unsigned int n_ijets_in_event = iterjets->size();
    std::cout << "JetBkgdSub::process_event(PHCompositeNode *topNode) Number of iter jets: " << n_ijets_in_event << std::endl;
    for (unsigned int ijet = 0; ijet<=n_ijets_in_event; ++ijet){
      Jet *jet = iterjets->get_jet(ijet);
      // for (auto ijet: *iterjets){

      // Jet *jet = iter->second;
      std::cout << "JetBkgdSub::process_event(PHCompositeNode *topNode) Iter jet pt: " << jet->get_pt() << std::endl;
      if(jet->get_pt() < min_reco_jet_pt) continue;
      m_iter_eta.push_back(jet->get_eta());
      m_iter_phi.push_back(jet->get_phi());
      m_iter_pt.push_back(jet->get_pt());

      Jet* unsubjet = new Jetv1();
      float totalPx = 0;
      float totalPy = 0;
      float totalPz = 0;
      float totalE = 0;
      
      auto comp_vec = jet->get_comp_vec();
      for (auto comp = comp_vec.begin(); comp != comp_vec.end(); ++comp)
      {
        // for (Jet::ConstIter comp = jet->begin_comp(); comp != jet->end_comp(); ++comp)
        // {
        TowerInfo *tower;
        // unsigned int channel = (*comp).second;
        unsigned int channel = comp->second;
        unsigned int calo_src = comp->first;
        if(calo_src == 15 || calo_src == 30)
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
            double pt = (tower->get_energy() + UE) / cosh(tower_eta);
            totalPx += pt * cos(tower_phi);
            totalPy += pt * sin(tower_phi);
            totalPz += pt * sinh(tower_eta);
        }
        else if (calo_src == 16 || calo_src == 31)
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
            double pt = (tower->get_energy() + UE) / cosh(tower_eta);
            totalPx += pt * cos(tower_phi);
            totalPy += pt * sin(tower_phi);
            totalPz += pt * sinh(tower_eta);
        }
        else if (calo_src == 14 || calo_src == 29)
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
      m_iter_pt_unsub.push_back(unsubjet->get_pt());
      m_iter_jets++;
    }

  }

  //==================================
  // Get rhoA jet info
  //==================================
  if(_doAreaSub)
  {
    // get fastjet input from tower nodes
    std::vector<fastjet::PseudoJet> calo_pseudojets;
    std::vector<fastjet::PseudoJet> CEMC_pseudojets;
    std::vector<fastjet::PseudoJet> IHCAL_pseudojets;
    std::vector<fastjet::PseudoJet> OHCAL_pseudojets;
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
	if(_doTowerECut)
	{
	  if(this_e < m_towerThreshold) continue;
	  if (this_e < 0)
	  {
	    // make energy = +1 MeV for purposes of clustering
	    float e_ratio = 0.001 / this_e;
	    this_e  = this_e * e_ratio;
	    this_px = this_px * e_ratio;
	    this_py = this_py * e_ratio;
	    this_pz = this_pz * e_ratio;
	  }
	}
        fastjet::PseudoJet pseudojet(this_px, this_py, this_pz, this_e);
	
	if(_input->get_src() == Jet::SRC::CEMC_TOWERINFO ||
	   _input->get_src() == Jet::SRC::CEMC_TOWERINFO_EMBED ||
	   _input->get_src() == Jet::SRC::CEMC_TOWERINFO_SIM ||
	   _input->get_src() == Jet::SRC::CEMC_TOWERINFO_RETOWER)
	  {
	    CEMC_pseudojets.push_back(pseudojet);
	  }
	if(_input->get_src() == Jet::SRC::HCALIN_TOWERINFO ||
	   _input->get_src() == Jet::SRC::HCALIN_TOWERINFO_EMBED ||
	   _input->get_src() == Jet::SRC::HCALIN_TOWERINFO_SIM)
	  {
	    IHCAL_pseudojets.push_back(pseudojet);
	  }
	if(_input->get_src() == Jet::SRC::HCALOUT_TOWERINFO ||
	   _input->get_src() == Jet::SRC::HCALOUT_TOWERINFO_EMBED ||
	   _input->get_src() == Jet::SRC::HCALOUT_TOWERINFO_SIM)
	  {
	    OHCAL_pseudojets.push_back(pseudojet);
	  }
        calo_pseudojets.push_back(pseudojet);
      }
      for (auto &p : parts) delete p;

    }

    // ghost definition
    const double ghost_max_rap = 1.1;
    const double ghost_R = 0.01;
    const double max_jet_rap = 1.1; // - m_jet_R;

    fastjet::Selector jetrap         = fastjet::SelectorAbsEtaMax(max_jet_rap);
    fastjet::Selector not_pure_ghost = !fastjet::SelectorIsPureGhost();

    // fastjet selectors
    fastjet::Selector selection  = jetrap && not_pure_ghost;
    fastjet::AreaDefinition area_def(fastjet::active_area_explicit_ghosts, fastjet::GhostedAreaSpec(ghost_max_rap, 1, ghost_R));
    fastjet::JetDefinition jet_def_antikt(fastjet::antikt_algorithm, m_jet_R);
    fastjet::JetDefinition jet_def_bkgd(fastjet::kt_algorithm, m_jet_R); 
    fastjet::Selector selector_rm_jets;
    if (_doEmbed) selector_rm_jets = jetrap * (!fastjet::SelectorNHardest(4));
    else selector_rm_jets = jetrap * (!fastjet::SelectorNHardest(2));
    fastjet::JetMedianBackgroundEstimator bge {selector_rm_jets, jet_def_bkgd, area_def};
    fastjet::JetMedianBackgroundEstimator bge_CEMC {selector_rm_jets, jet_def_bkgd, area_def};
    fastjet::JetMedianBackgroundEstimator bge_IHCAL {selector_rm_jets, jet_def_bkgd, area_def};
    fastjet::JetMedianBackgroundEstimator bge_OHCAL {selector_rm_jets, jet_def_bkgd, area_def};
    // set particles for background estimation
    bge.set_particles(calo_pseudojets);
    bge_CEMC.set_particles(CEMC_pseudojets);
    bge_IHCAL.set_particles(IHCAL_pseudojets);
    bge_OHCAL.set_particles(OHCAL_pseudojets);
    
    // rho and sigma
    m_rho_area = bge.rho();
    m_rho_area_sigma = bge.sigma();
    m_rho_area_CEMC = bge_CEMC.rho();
    m_rho_area_sigma_CEMC = bge_CEMC.sigma();
    m_rho_area_IHCAL = bge_IHCAL.rho();
    m_rho_area_sigma_IHCAL = bge_IHCAL.sigma();
    m_rho_area_OHCAL = bge_OHCAL.rho();
    m_rho_area_sigma_OHCAL = bge_OHCAL.sigma();
    
    // cluster jets
    fastjet::ClusterSequenceArea clustSeq(calo_pseudojets, jet_def_antikt, area_def);
    std::vector<fastjet::PseudoJet> jets =  fastjet::sorted_by_pt( selection( clustSeq.inclusive_jets(5.) ));

    m_rhoA_jets = 0; // loop over rhoA jets
    for (auto jet : jets) {
      float pt = jet.pt() - jet.area() * m_rho_area; // subtract rhoA
      if(pt < min_reco_jet_pt) continue;
      m_rhoA_eta.push_back(jet.eta());
      m_rhoA_phi.push_back(jet.phi_std());
      m_rhoA_pt_unsub.push_back(jet.pt());
      m_rhoA_area.push_back(jet.area());
      m_rhoA_pt.push_back(pt); // subtract rhoA
      m_rhoA_jets++;
    }


  }

  //==================================
  // Get mult jet info
  //==================================
  if(_doMultSub)
  {
    // estimate rho
    EstimateRhoMult(topNode);
    if(m_rho_mult < 0 ) return Fun4AllReturnCodes::EVENT_OK; // skip event if rho_mult < 0

    // get raw tower jets
    JetContainer *multjets = findNode::getClass<JetContainer>(topNode, m_raw_input);
    if(!multjets) 
    {
      std::cout << "JetTree::process_event(PHCompositeNode *topNode) Could not find mult jet nodes" << std::endl;
      exit(-1);
    }

    // loop over mult jets
    m_mult_jets = 0;
    // for(JetMap::Iter iter = multjets->begin(); iter != multjets->end(); ++iter){
    for(auto jet : *multjets)
    {
      // Jet *jet = iter->second;
      float nsignal = NSignalCorrection(jet->get_pt(), m_centrality);
      float pt = jet->get_pt() - m_rho_mult*(jet->size_comp() - nsignal);
      if(pt < min_reco_jet_pt) continue;
      m_mult_ncomponent.push_back(jet->size_comp());
      m_mult_eta.push_back(jet->get_eta());
      m_mult_phi.push_back(jet->get_phi());
      m_mult_pt_unsub.push_back(jet->get_pt());
      m_mult_nsignal.push_back(nsignal);
      m_mult_pt.push_back(pt);
      m_mult_jets++;
    }
  }
  
  //==================================
  // Fill tree
  //==================================
  m_tree->Fill();


  return Fun4AllReturnCodes::EVENT_OK;
}

void JetBkgdSub::GetCentInfo(PHCompositeNode *topNode)
{
  //==================================
  // Get centrality info
  //==================================
  CentralityInfo* cent_node = findNode::getClass<CentralityInfo>(topNode, "CentralityInfo");
  if (!cent_node)
  {
      std::cout << "JetBkgdSub::process_event() ERROR: Can't find CentralityInfo" << std::endl;
      exit(-1);
  }
  
  if (!_doData)
    {
      m_centrality = cent_node->get_centile(CentralityInfo::PROP::bimp);
      m_mbd_NS = cent_node->get_quantity(CentralityInfo::PROP::mbd_NS);
    }
  else
    {
      m_centrality = (int)(100*cent_node->get_centile(CentralityInfo::PROP::mbd_NS));

      PHNodeIterator iter(topNode);
      PHCompositeNode *mbdNode = dynamic_cast<PHCompositeNode *>(iter.findFirst("PHCompositeNode", "MBD"));
      if(!mbdNode)
	{
	  std::cerr << Name() << "::" <<  __PRETTY_FUNCTION__
		    << "MBD Node missing, doing nothing." << std::endl;
	  throw std::runtime_error(
				   "Failed to find MBD node in JetBkgdSub::GetCentInfo");
	}
      MbdOut *_data_MBD = findNode::getClass<MbdOut>(mbdNode, "MbdOut");
      if(!_data_MBD)
	{
	  std::cerr << Name() << "::" <<  __PRETTY_FUNCTION__
		    << "MbdOut Node missing, doing nothing." << std::endl;
	  throw std::runtime_error(
				   "Failed to find MbdOut node in JetBkgdSub::GetCentInfo");
	}

      m_mbd_NS = _data_MBD->get_q(0) + _data_MBD->get_q(1);
    }
  return ;
}

int JetBkgdSub::ResetEvent(PHCompositeNode *topNode)
{
  
  m_rhoA_eta.clear();
  m_rhoA_phi.clear();
  m_rhoA_pt.clear();
  m_rhoA_area.clear();
  m_rhoA_pt_unsub.clear();

  m_iter_eta.clear();
  m_iter_phi.clear();
  m_iter_pt.clear();
  m_iter_pt_unsub.clear();

  // mult jet variables
  m_mult_ncomponent.clear();
  m_mult_nsignal.clear();
  m_mult_eta.clear();
  m_mult_phi.clear();
  m_mult_pt.clear();
  m_mult_pt_unsub.clear();

  // truth jet variables
  m_truth_ncomponent.clear();
  m_truth_eta.clear();
  m_truth_phi.clear();
  m_truth_pt.clear();


  return Fun4AllReturnCodes::EVENT_OK;
}

float JetBkgdSub::LeadingR04TruthJet(PHCompositeNode *topNode)
{

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

void JetBkgdSub::EstimateRhoMult(PHCompositeNode *topNode)
{

  //==================================
  // Estimate rho
  //==================================
  m_rho_mult = -1;
   
  // iternstruct kT jets
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

float JetBkgdSub::NSignalCorrection(float jet_pt, int cent)
{
 
  // R = 0.2
  const float cent0_r02[] = {3.8251452063612823,3.7738140343084097,3.733293410634983,3.707461805783227,3.6915017720491736,3.6922823383705716,3.7241631866886795,3.753290543017409,3.8080431073417977,3.8831575939079643,3.970800756192729,4.093121177200603,4.235835572672623,4.41611632243862,4.6280609309573615,4.854741146325462,5.119514978349617,5.368199874163043,5.62131189182852,5.840786146732615,6.015106735729953,6.181409686063714,6.298933998639147,6.411129964771125,6.4886010790899755,6.5754417049938425,6.633671604332052,6.68310821464235,6.72508052070053,6.751945982781292,6.79754436278903,6.826121952177156,6.835269254852849,6.856927135102072,6.870657188588645,6.885690130817834,6.916763547721732,6.92603550295858,6.980281336347652,6.930556704442055,7.041564400811013,7.088775510204082,7.115414710485133,7.174400210914843,7.209893992932862,7.377040261153428,7.644329896907217,7.644329896907217};
  const float cent1_r02[] = {3.7065978059441242,3.6723497705290886,3.665855452441214,3.675928223079407,3.7239803884289424,3.789608204008013,3.876459658253902,3.99293055023618,4.140317076494414,4.31916235446313,4.534399523771498,4.785910831966329,5.061214539204467,5.344495005867033,5.608929588441593,5.838971980262184,6.039250738380227,6.1957765703336225,6.316886928637253,6.416785684970861,6.503754015027471,6.59440586909359,6.655500774024015,6.703071011770778,6.755849731375503,6.796798781877696,6.822414715003743,6.862929936305733,6.8525437262941296,6.879622373969178,6.867880355021315,6.887583095916429,6.900340780007573,6.945897563401293,7.019795330988168,6.938309415470774,7.046840958605665,7.088823891625616,7.136075280186086,7.130385487528344,7.202602230483271,7.293989071038252,7.350354609929078,7.566074950690335,7.494743758212878,7.636120042872454,8.366666666666667,8.366666666666667};
  const float cent2_r02[] = {3.636051343073952,3.654766167093252,3.696215730070566,3.7811951029574224,3.876780324305565,4.009315182937317,4.180881201568081,4.375538832585154,4.6189117564554945,4.888073643168408,5.191556631790561,5.467030350156732,5.72587166496119,5.94590483082678,6.119981861687683,6.266022610483042,6.382807796409631,6.474538932218727,6.5585689377757594,6.634778230011501,6.689494598286353,6.73305573997633,6.7916738169367425,6.820715076608591,6.848742541682931,6.875149503446979,6.883444030455208,6.883787835811824,6.88883716118353,6.910818952814434,6.945971744940817,6.959102607138861,6.982768865121806,7.061872237846525,7.080995235574378,7.078928046989721,7.1728110599078345,7.274116847826087,7.202974828375286,7.445420326223338,7.31763698630137,7.401190476190476,7.638535031847134,7.361047835990888,7.343076923076923,7.39119804400978,7.291666666666667,7.291666666666667};
  const float cent3_r02[] = {3.646815167062946,3.7057295347751404,3.8027127219795016,3.9236056017006726,4.079834167631868,4.275482583814118,4.515939895251131,4.785690358548578,5.080784692433804,5.3661578960079215,5.645186807226192,5.876026381716857,6.059288801289355,6.219357753168642,6.34256054468226,6.454583918050278,6.542351739019366,6.616209381279013,6.687453290817597,6.737295107253769,6.772662086831819,6.805752693398853,6.85079003514703,6.856583770930348,6.865479394067669,6.882604025851373,6.9005158786501966,6.87317678387702,6.890618902676969,6.9248992706686385,6.9844682582745525,6.982917395708629,6.989879547994536,7.026908846283216,7.069955472228732,7.206963249516441,7.210196779964222,7.235757575757575,7.349706129303106,7.349416755037115,7.54601226993865,7.713552361396304,7.585492227979275,7.570631970260223,7.655913978494624,7.513392857142857,6.75,7.513392857142857};
  const float cent4_r02[] = {3.6881612459844964,3.782492649585704,3.918155100901578,4.076533692030552,4.2861348739029,4.53009306711492,4.804634447394636,5.100639406201793,5.395386548732449,5.656524312557057,5.897045202398037,6.084277483665776,6.22557149374018,6.359521146807645,6.461032796020722,6.541121505399522,6.621424640123467,6.692601635941887,6.73947844836354,6.781273220811913,6.820771199692499,6.852320393825957,6.868454834966796,6.869122245215474,6.890105418551394,6.89313639280367,6.899388939810572,6.90846286701209,6.908651319247346,6.940233852222301,7.037818320030406,7.0364569961489085,7.050799289520426,7.07803605313093,7.260142810775722,7.271317829457364,7.252596212583995,7.294453507340946,7.488662131519274,7.577966101694916,7.462703962703963,7.403485254691689,7.817647058823529,7.135838150289017,7.596774193548387,7.914925373134328,8.25,8.25};
  const float cent5_r02[] = {3.739883799727122,3.8614913650343032,4.0148543710438735,4.214982937654399,4.451457056933951,4.717233781573687,5.012269605993172,5.299391909546545,5.596082889567004,5.824244590094132,6.019680118685316,6.193613695383607,6.309736012827582,6.426449549347013,6.524862635913985,6.61337141974832,6.6627362672435195,6.735529989441022,6.767011034772956,6.824258249068653,6.838752175477147,6.8588643154627995,6.867730749332327,6.889102419172508,6.898190543277274,6.879770885722269,6.895148557485288,6.912820512820513,6.938162993076288,6.921184608733247,6.999823217442546,7.047386381262603,7.131032244103008,7.252519264967398,7.240930599369086,7.2823529411764705,7.16243050039714,7.541289023162135,7.472972972972973,7.5199600798403194,7.239612188365651,7.418367346938775,7.514423076923077,7.515873015873016,8.070093457943925,7.398039215686275,8.25,8.25};
  const float cent6_r02[] = {3.784296221476128,3.9080341145412842,4.086776802041398,4.313847968007656,4.553657018362901,4.833074918619659,5.131960455291651,5.4332554073433785,5.701482758093218,5.915938147714913,6.095024336373676,6.252800155195475,6.372104074479066,6.474465453260294,6.56196353375766,6.641740605949803,6.708003147650635,6.770498375895296,6.803839668914776,6.832526552830824,6.862064798175647,6.859180771233033,6.864873212465639,6.87573958831977,6.907649253731344,6.897221087045362,6.902545016010313,6.880702702702703,6.932583443611233,7.013037420382165,7.063716342308718,7.104460966542751,7.077521324636227,7.279003677699766,7.314542107685228,7.40609756097561,7.3172992056487205,7.527906976744186,7.169367909238249,7.390495867768595,7.457746478873239,7.155737704918033,7.619565217391305,7.4296875,7.711111111111111,7.723744292237443,8.0,8.0};
  const float cent7_r02[] = {3.80700406049955,3.939982558902291,4.1344687876907225,4.365691573926868,4.620755232411533,4.909708500040827,5.203237491922199,5.495214658819291,5.760611396527316,5.9642108701660925,6.153523540376406,6.2887850653193365,6.40536482185439,6.497977822262293,6.579897864931149,6.652209289185219,6.717449276787141,6.773327330702105,6.837096049921803,6.851690795159897,6.854491501988896,6.892440144730426,6.880735971722527,6.8995568243067895,6.891392875996079,6.878107183580387,6.926567333246199,6.929415170816445,6.91541822721598,6.988761616598228,7.096851956891662,7.08275462962963,7.194997310381925,7.258607922991485,7.213649851632048,7.418086500655308,7.507575757575758,7.470979443772673,7.418544194107453,7.389423076923077,7.811418685121107,7.524630541871921,7.560402684563758,7.706185567010309,8.166666666666666,7.826633165829146,8.166666666666666,8.166666666666666};
  const float cent8_r02[] = {3.8223640211603827,3.95970793015194,4.157570485473915,4.393264709171272,4.657463197352976,4.940888738770282,5.242485676106098,5.531004505052965,5.786519100606861,5.996922628968453,6.169426729325749,6.297214581319852,6.422726654544173,6.517201038090567,6.588281396281931,6.6612532173200005,6.721018592371378,6.779185619855154,6.830495920238907,6.861491364342502,6.879140825896234,6.8789677859033835,6.895506379135147,6.902212961632419,6.891369320714155,6.893784315054565,6.887900196744768,6.974844851479183,6.927258927858113,6.989872872225813,7.0900060204695965,7.18180900464365,7.083625100996499,7.127863312054075,7.33143153526971,7.369165487977369,7.460238568588469,7.5210084033613445,7.675406871609403,7.615681233933162,7.822259136212624,7.383177570093458,7.689873417721519,7.34,6.914285714285715,7.747058823529412,7.833333333333333,7.833333333333333};
  const float cent9_r02[] = {3.8185209968687,3.964183302355068,4.169599046936462,4.405123080846118,4.672717886948292,4.960117959793986,5.253600989176271,5.547660968869978,5.806118060105327,6.004443324937028,6.181509943506191,6.317663048909676,6.4279691640568535,6.516193832780181,6.601890975692045,6.678747832258286,6.728504356574608,6.784634830873801,6.824405184799255,6.8553873667908105,6.876191829951015,6.876413398148765,6.9028059691798065,6.907913136464589,6.910601952991341,6.907313911441632,6.896883634551796,6.8842070338420704,6.974084872044056,7.023088735329604,7.073772982829357,7.1059322033898304,7.152513966480447,7.22109375,7.244114636642784,7.480225988700565,7.262366634335597,7.587786259541985,7.508741258741258,7.369879518072289,7.7064056939501775,7.831838565022421,7.206896551724138,7.6415929203539825,7.137362637362638,7.942528735632184,7.9,7.9};

  // R = 0.3
  const float cent0_r03[] = {5.017472437570611,5.0073614628492935,4.968102965864578,4.960699437515285,4.93375635771073,4.9188332122972644,4.925436965414653,4.887911900290251,4.85881417537483,4.837608674093419,4.81364477719148,4.807321951900265,4.810242267944205,4.824568282818963,4.852399837299166,4.881892200992582,4.925091373779076,4.971463499825358,5.03807015732039,5.097246437058316,5.161151897425292,5.242892793673601,5.34639564992232,5.458050179647395,5.588088726068301,5.75531914893617,5.943526616450129,6.141970253754107,6.371356559103985,6.622870224819859,6.890376785609469,7.1289121440739835,7.404076816489131,7.650860167622408,7.866561717665322,8.072036501076328,8.212455119819644,8.363717963192954,8.482554747477081,8.59422537910548,8.653828848533326,8.710455693190381,8.770654424913241,8.801786043085988,8.838815984799002,8.961347934209673,9.186291687878107,9.186291687878107};
  const float cent1_r03[] = {4.861862931307376,4.860684839638645,4.805444887118194,4.777471730936504,4.743862083689154,4.729691236095229,4.720253978460055,4.72978158205431,4.758378378378378,4.804710595172856,4.84170119734601,4.9206885658418615,4.995374243750514,5.076278867257266,5.180227449389976,5.282340340407088,5.426072059846029,5.590293993590496,5.770970810027401,6.00530344712102,6.278853693950754,6.597082778812883,6.914215118523666,7.23916540127102,7.576955910557677,7.854012848787036,8.102132488665577,8.29522905607487,8.457117364148766,8.581093851835758,8.673837736782778,8.734667910049492,8.791577602298892,8.863771464252263,8.892748042699827,8.928937259923176,8.925151295690466,8.966742878917787,8.96455573936117,8.996038629831965,9.001907000638262,9.016328765054554,9.044127818508784,9.073084633487317,9.110801700131907,9.176924867601699,9.58114946683535,9.58114946683535};
  const float cent2_r03[] = {4.729900560668571,4.651396061809428,4.641706096657647,4.64519023111388,4.669745693709931,4.715274319824819,4.795603945115628,4.881701591005254,4.9752858587573545,5.0920878115710035,5.214936973915938,5.352592844000643,5.526414736402785,5.722291850978079,5.983503966309064,6.278818803765502,6.625754668161628,7.0138905571666355,7.38051164786588,7.7187929879016615,8.016984205843952,8.243567120839849,8.424951719227076,8.571838568780048,8.6801185378266,8.760139744061915,8.824268679309196,8.856763641754007,8.91560932428445,8.944680509644083,8.962513631406761,8.967516205067767,8.99542890303186,9.008994184211712,9.003796948758295,9.035084924800053,9.04688026271472,9.0712954333644,9.02780504315451,9.11214810947222,9.19989028122654,9.17055829965574,9.16421296763134,9.283985346068297,9.355018587360595,9.444071728635024,9.521194605009635,9.521194605009635};
  const float cent3_r03[] = {4.58333717287136,4.581091784438577,4.636866602265742,4.718030909405942,4.816284491177469,4.933031597568549,5.062837147834444,5.204348784317807,5.376941880392292,5.571956073727177,5.808258763648305,6.0984107350350465,6.452751059732295,6.848463467257566,7.246553631122673,7.640609223011746,7.945740826753368,8.208925454998754,8.404680779286497,8.560516632635698,8.674224319195856,8.756013633415945,8.828220441144575,8.872865803365226,8.913300328773397,8.951160410760707,8.96881380188673,8.981189766757566,9.003615944926377,9.015209438072922,9.01078381115515,9.049531521469348,9.034909247716492,9.007216236532198,9.06503081340948,9.063395293315143,9.126132973033835,9.177002777986335,9.163,9.25034162339437,9.302545454545454,9.320862783329272,9.359344262295082,9.433737548722391,9.39115250291036,9.597664115988723,9.502695417789758,9.502695417789758};
  const float cent4_r03[] = {4.564552672992244,4.643442806577355,4.76368035510709,4.892989525232449,5.0520568655853975,5.220604918064461,5.414025309334614,5.624848361246585,5.899536843326277,6.209686496346824,6.603466580244292,7.014481064178289,7.4180046095550605,7.797948127844435,8.095813918626876,8.31734970819709,8.501779005524861,8.623586651666987,8.717583628727175,8.795213670231885,8.87800715809827,8.91435710812694,8.941472159917314,8.957605825491177,8.999156159384876,8.983000808945272,8.993409486412922,9.031545604443288,8.99445810326065,9.026620962515775,9.006571087216248,9.034326844522072,9.073233071566158,9.109258066114883,9.144717704614571,9.248301751877012,9.223429105349217,9.27410071942446,9.3826803682295,9.368795355587809,9.406735751295336,9.630722278738556,9.660726294552791,9.6005859375,9.816037735849056,9.649957519116398,9.505050505050505,9.505050505050505};
  const float cent5_r03[] = {4.633798712816095,4.770795607984341,4.92398967187908,5.105598626742664,5.306167022635313,5.5112621527348296,5.757637455675894,6.062445075901201,6.418420786519844,6.834793202168484,7.266026490066225,7.661719812910781,7.97825340996826,8.230040759099454,8.442257035102989,8.570136305978973,8.677587237880953,8.77294690658884,8.841328961622901,8.881408138160593,8.943087958631375,8.972964236626085,8.996537692962491,9.002922770098342,9.003621520796493,9.017333903676771,9.005143566365133,9.00286731860437,9.001051534855003,9.013996247654784,9.097911917269634,9.06609258688279,9.096795175023653,9.2084058888713,9.283264309227992,9.301795173631549,9.404941531497547,9.444187242798353,9.453917050691244,9.46714484035169,9.672258064516129,9.49914089347079,9.539772727272727,9.432835820895523,9.755020080321286,9.599426386233269,9.723300970873787};
  const float cent6_r03[] = {4.711999086943289,4.872081487633813,5.061556153749259,5.260980162881818,5.491754552453938,5.739936102236422,6.036334783744184,6.391319346022815,6.804024325917652,7.24412452737807,7.643576440806449,7.97481092080719,8.230018116993223,8.431446199378248,8.567028945691114,8.675012964874176,8.763863252562542,8.834538402604865,8.89835716077251,8.933822937625754,8.985285301359598,9.007918195495876,9.007069831096416,9.023529082729357,8.995920488002001,9.005649014238124,9.00134530481188,8.993512458376392,9.01536684577914,9.05950184501845,9.09920652413489,9.125241468126207,9.211063899329956,9.314758380496299,9.28144060412431,9.26158687774175,9.446621088614252,9.53042233357194,9.583488805970148,9.444761904761904,9.376279863481228,9.571817192600653,9.591032608695652,9.657057654075548,9.53921568627451,9.484349258649093,9.169902912621358};
  const float cent7_r03[] = {4.771161060531802,4.954674698218986,5.162300086730269,5.3752600664088765,5.613688328845079,5.88654715856637,6.215037369782899,6.60349453080023,7.053333462650088,7.458125293095363,7.833880010155837,8.117166060111483,8.348751054542786,8.498211799857396,8.62904102288106,8.72239350394957,8.813134465264282,8.86606872663448,8.91892426198427,8.955267227872907,9.000304141253062,9.00329611941405,9.027252692567448,9.026359499334763,8.998103798688373,9.01244959021725,9.023479612421045,8.997454561209679,9.016141900555635,9.060857827293814,9.111185360337206,9.176809267918207,9.236038683985967,9.315273989585917,9.290420197129517,9.376702839990765,9.460814563406355,9.37248872488725,9.574826296098344,9.63065693430657,9.833333333333334,9.784960422163588,10.016014234875446,9.578758949880669,9.669753086419753,9.548016701461378,9.23015873015873};
  const float cent8_r03[] = {4.815898325147251,4.999133966692359,5.203251381813731,5.443445060642457,5.6793143676149525,5.9538541791235575,6.317346565629888,6.707981705340744,7.17542237849685,7.564588267159297,7.907591448996238,8.172210268852716,8.403134992142801,8.540386612616839,8.659492422669711,8.753021901790017,8.81469955973603,8.880056359797933,8.93048443889962,8.976703392659937,8.998698267348955,9.021977636007177,9.012949511788522,9.037900406649069,8.999215608290248,9.013755731554815,8.999509690302565,8.996153041537854,9.049537195749057,9.088271706833938,9.139031466190582,9.110939733654353,9.293969849246231,9.280704111797904,9.31070185083379,9.407388916625063,9.570360299116247,9.459891843172601,9.444877171959257,9.575806451612904,9.662610619469026,9.657352941176471,9.716911764705882,9.96938775510204,9.788079470198676,9.62,9.160377358490566,9.160377358490566};
  const float cent9_r03[] = {4.8285967722117915,5.015595504061239,5.231521210560943,5.459718586113152,5.710556874263563,6.000885724505612,6.352445352059847,6.774801191316897,7.193818543010313,7.6151826556431885,7.952630486665975,8.211758795281344,8.402184010637717,8.561349181640278,8.666857798165138,8.751782470259606,8.836929816701955,8.872697852587086,8.929820759907873,8.983953580185347,8.997896638050275,9.014364197490588,9.015632058929379,9.009215405514558,8.998648802089248,8.994712472847834,9.01810147178463,9.017295219793418,8.99518397225968,9.076043863348797,9.12769909449733,9.197026751392485,9.275034293552812,9.347294185377088,9.352285824049174,9.392612338156892,9.479522184300341,9.502752293577982,9.587740384615385,9.539389067524116,9.713362068965518,9.691198786039454,9.382917466410749,9.510840108401084,9.353242320819113,9.530837004405287,9.532786885245901};

  // R = 0.4
  const float cent0_r04[] = {6.437261210536899,6.367542485235627,6.345103774899011,6.333566760037348,6.274586742360995,6.276902173913044,6.307892004153686,6.266269567785764,6.270335419017335,6.254031015713259,6.255421559293346,6.234196011879508,6.222829449371004,6.249794323323735,6.272910323150968,6.2081303143212,6.190593837031713,6.1806678841638405,6.1776853015075375,6.18087107509767,6.176364340085151,6.177053987236245,6.169554026629709,6.1642863363232605,6.18120306342479,6.162568789624433,6.2024579199643295,6.236923598006483,6.278372412644205,6.302173036754736,6.34861662673537,6.413548330558234,6.4593853634045475,6.513325615329884,6.616408489187773,6.695192297716497,6.760348467308085,6.88888480414683,7.016039803764657,7.159919433532664,7.29825767996332,7.450839689936859,7.620043844729183,7.802517728668527,7.993594326403386,9.002285171964493,10.368990129758284,10.953269207431962};
  const float cent1_r04[] = {6.20449323461833,6.208372530573848,6.148215002005616,6.128609692809911,6.1134307585247045,6.066381061743662,6.1156059580230195,6.12706511175899,6.050256677330556,6.034791524265208,6.009258174147428,6.009007648953301,5.989005716193099,5.973349381017882,6.016945039257673,6.024722292787695,6.044250738306182,6.08633712592026,6.153279132791328,6.189399322097073,6.267717821984396,6.343461108434611,6.419195648428462,6.520693550726297,6.606645692367366,6.726991543209452,6.840957528034684,6.987287969087694,7.156511346189974,7.3496631339410134,7.588446266171385,7.8224243742920585,8.09313235920891,8.381951051317579,8.663572465428986,8.949962935507784,9.206851658511427,9.482878234248469,9.691533973876686,9.900353328712251,10.068180564662912,10.197826877221951,10.322236550523957,10.415794922261913,10.504910244979973,10.71878573666484,10.95193923688291,10.865540963101939};
  const float cent2_r04[] = {6.013905263887926,6.032877205617573,5.9468202131316605,5.92171576801927,5.90678950425004,5.870306056001737,5.8319298729983435,5.813235447256066,5.841442646879941,5.876920541753637,5.946839983728412,6.022686098474174,6.099004113080269,6.176823812675944,6.28142264744498,6.377987481031867,6.492076148986181,6.6115590527383805,6.744652591826067,6.9043446925691665,7.085225029609159,7.317948776116694,7.587229243997557,7.866528880002944,8.216598986369503,8.581674139738947,8.945074574983746,9.303314221787163,9.621661832366474,9.900159399392821,10.101618875807064,10.293685321824679,10.408989125317227,10.556569468661099,10.615306311308533,10.690937356947096,10.728096340210652,10.756738094766124,10.802694158858543,10.818670293234675,10.836606085927164,10.854315872546119,10.892822579726976,10.86770297772065,10.915204861546172,10.954623153984599,11.087736012471852,10.70233918128655};
  const float cent3_r04[] = {5.823382561622957,5.700560951105158,5.716749671545113,5.682422854650529,5.712070320324176,5.763783648485676,5.84874635366023,5.948668725482013,6.07773322873994,6.189377633149374,6.321411153420494,6.468099780292853,6.614064535635997,6.774895009994681,6.954964958324921,7.17555239435313,7.454820155177827,7.74758606467891,8.167488296675995,8.593856678685514,9.012263952263952,9.425886668247305,9.774439994066162,10.065182937435976,10.283340242385817,10.432554122524182,10.580757350163337,10.660249779303847,10.727975112146307,10.772275345145912,10.793284395251476,10.837958870117122,10.849986304389944,10.867698309492848,10.881716037632552,10.902978061029692,10.90374309887846,10.900309852471143,10.92239655437625,10.903251998228484,10.94469646430954,10.931414884285811,10.973511222592224,11.025236061151078,10.987926605504587,11.103158205430933,10.680731364275669,10.584257206208425};
  const float cent4_r04[] = {5.562315265462142,5.568743240935739,5.622679897886716,5.727347841918046,5.863090748089601,6.00689315048568,6.19406206793694,6.354532860971068,6.524934921570236,6.7070986396538,6.907834401597142,7.130073753279612,7.413481087720784,7.774197339633672,8.165893427466802,8.646908031688353,9.122237415211597,9.552657251518593,9.903730821857987,10.189937291454852,10.38033332919054,10.562182931761715,10.660447017638347,10.72428663156543,10.785917172921994,10.808783625849856,10.848059077930802,10.865086969755042,10.89666546570424,10.860491697520475,10.886884661791738,10.888563996338938,10.88546473111031,10.896553713049748,10.906409677710572,10.926791777553454,10.906280696532223,10.956615120274915,11.024452536286855,11.004129993229519,11.077172737107825,11.161182277588436,11.141154328732748,11.12675026123302,11.18565976008724,11.147576684273933,10.525522041763342,11.150602409638553};
  const float cent5_r04[] = {5.516633399866933,5.600668908457519,5.771022461287095,5.968385569213502,6.182346561600159,6.394144314815974,6.6021211642364115,6.812360886555932,7.022434180709489,7.2958907812922416,7.622516404612759,8.04633083329991,8.508512578855361,9.017566760236024,9.493931310436812,9.85837585583258,10.15560972691652,10.381256040606392,10.535846965249272,10.639855602874634,10.7151051035617,10.779149212662377,10.818725503936285,10.867250180556852,10.876723676761591,10.883403973584885,10.906749351760784,10.90188450071773,10.87924448960998,10.870282540191209,10.87425398099826,10.86929390219087,10.88354474982382,10.93986905582357,10.972435607772255,10.99749799839872,11.092325330455894,11.061805804371193,11.143495051894762,11.271134020618558,11.179891981719983,11.364689265536724,11.263265306122449,11.13756345177665,11.330281690140845,10.897805212620028,10.59779792746114,10.594936708860759};
  const float cent6_r04[] = {5.565658821857169,5.7497570266330165,5.975227065536685,6.236272678052246,6.473447861086266,6.7113510227295015,6.934869982438996,7.184461358518234,7.488085820321003,7.867744329574751,8.351450611592599,8.882344942464263,9.367406167651877,9.786715990369576,10.0840124719546,10.336942760541563,10.480982189037572,10.601383632923367,10.700365866781448,10.763826178062713,10.819993486527649,10.870423848342138,10.89721235418221,10.90926434837755,10.910073349053809,10.912941497340789,10.883324687114973,10.882101374841973,10.870613279519613,10.861449002043061,10.878264052712925,10.90623990524389,10.946785196705665,11.03085665977832,11.08982552565987,11.068565037852718,11.16498393758605,11.243005656627426,11.24375503626108,11.109938922820655,11.25498368974266,11.273485932284215,10.9573055028463,11.18026101141925,11.032710280373832,10.828538550057537,10.446754194018965,11.101503759398497};
  const float cent7_r04[] = {5.655077584185082,5.884161040308097,6.160752369668247,6.432122357044249,6.674835339541222,6.908316677694632,7.159924064158038,7.454201259009214,7.851275092207735,8.337061652553677,8.857024736738383,9.342908446181262,9.770152621573121,10.101248645060458,10.32003631961259,10.489547924306095,10.61357652581041,10.698197189108463,10.74176635002541,10.814120072026466,10.85242897688132,10.88715850100413,10.915456410464738,10.904519907775391,10.91076027608942,10.909626470880958,10.861926309589476,10.855464208115567,10.878489139838805,10.894103465992174,10.918382564558751,10.92888858054669,10.9503569415743,11.146585335177965,11.099150743099788,11.093659621802002,11.216815522020326,11.279580936729664,11.264931506849315,11.133895818048423,11.264988009592326,11.150162866449511,11.31391712275215,11.253112033195022,11.143258426966293,10.744673295454545,10.453469387755103,10.82967032967033};
  const float cent8_r04[] = {5.718746819208924,5.979390813364243,6.263076029579198,6.5391882192224875,6.785848353615001,7.031408621406993,7.283321947320715,7.618607077400254,8.05457970003488,8.605848612446536,9.084266320514901,9.569079856313898,9.926855624446413,10.20728184984073,10.418210408251534,10.553831910761867,10.644676422317911,10.71479922847624,10.793701107629005,10.82508567354355,10.873875101004272,10.898287918977575,10.914777814429103,10.907703979421283,10.928712761688644,10.894618237456136,10.872828438948995,10.842122993592708,10.867614469332787,10.871293540251301,10.931804520317383,10.945067175451149,11.04727468969239,11.094153936545242,11.179988129389653,11.05929158110883,11.252167880679847,11.313987022350396,11.323602100710534,11.158322903629537,11.311192764273601,11.331426392067124,11.024606299212598,11.315568862275448,11.262996941896024,10.708066339992461,10.481196581196581,10.81578947368421};
  const float cent9_r04[] = {5.7527460363967835,6.021982428261558,6.313316302820014,6.590778131462162,6.824678468369187,7.070772259077557,7.348284743404757,7.7068102361642925,8.16911494944455,8.697070424463325,9.198287693313807,9.641232739047902,10.002443740741727,10.264701097345709,10.451063323453692,10.557772841019355,10.638420595631434,10.735432292515897,10.788647245374882,10.833850509886974,10.870995380718224,10.903300938502124,10.924090969899666,10.905357273547851,10.896124873191404,10.875570895652476,10.870465982615515,10.82852492141895,10.876561850170877,10.872076763585515,10.914245339938395,10.96162998215348,11.02595275047422,11.049626131444313,11.157902908601594,11.205124653739611,11.359181374389786,11.212637913741224,11.143853820598007,11.002636203866432,11.331585081585082,11.320590461771385,11.146414342629482,10.94910941475827,10.928134556574923,10.578106964217007,10.559227467811159,10.448453608247423};

  // R = 0.5
  const float cent0_r05[] = {7.662573826546472,7.68285412421704,7.594485392061096,7.58304152076038,7.518518518518518,7.529926065015843,7.523297721714506,7.545503211991434,7.43074560178721,7.476760866446708,7.452496954933008,7.424572683079818,7.458157689305231,7.3967328845838125,7.445550254976148,7.385383980181668,7.400960080848914,7.40987487318228,7.498826881179823,7.450464919695689,7.4343880099916735,7.429712981082844,7.4276879162702185,7.455978869857532,7.415401644528779,7.404966938336153,7.391537185095792,7.395065229722064,7.316567226307757,7.413678618857902,7.403258398585502,7.409953760038939,7.365626740947075,7.324371619471842,7.358689234402677,7.39794776119403,7.355522188855522,7.352829327654396,7.422842709847069,7.386701360586871,7.394363781868069,7.419076336271326,7.4079347525573676,7.465414652287095,7.46709553386572,7.890584791919337,9.716515697229827,9.716515697229827};
  const float cent1_r05[] = {7.445396825396825,7.388260787638967,7.32907974160754,7.302562609202097,7.253042843232716,7.270131578947368,7.293784925718959,7.286035728896768,7.256749116607774,7.229465904336912,7.250035009102366,7.175819074147765,7.280623020063358,7.17583471920174,7.226209930861094,7.206648070664807,7.203636941999788,7.190087200561291,7.188872687505696,7.19873417721519,7.185962156906738,7.194233638191607,7.165704747354722,7.199657006511905,7.166995496461506,7.209183673469388,7.254637057955202,7.261627249066425,7.287090486543219,7.373323170731707,7.373536756672389,7.45960388444305,7.531399121969842,7.628882714243931,7.72983564764591,7.815050471829382,7.924251015972025,8.019230554407445,8.196913771215822,8.327856628351713,8.498456141722281,8.677688326624496,8.854076372097268,9.058422358718035,9.266875553436414,10.388293480848853,11.850442976971669,11.850442976971669 };
  const float cent2_r05[] = {7.149295774647888,7.11504656272889,7.09435566133968,7.027067843866171,7.090699870175853,7.033424751456977,6.996993353729296,7.055312810327706,7.018138012546595,7.010571081409478,6.971527297857636,6.924756573960702,6.939894001445435,6.930703197079707,6.965471884248602,6.963530180511313,7.030566549813412,7.062280634823745,7.168823654056169,7.232488874628573,7.3008389386681225,7.411449583337969,7.470840167669036,7.58855981416957,7.691174917259348,7.805007446016381,7.939233612455842,8.132871406574916,8.300979686375161,8.505225715452857,8.754193637663557,9.003339734810519,9.280525385579843,9.59012024481337,9.879632023281065,10.181289167412713,10.482722075920098,10.730948377155643,10.9585316064606,11.193823826744193,11.38756303089783,11.525274072473943,11.682777300609349,11.796907272901363,11.887041683358284,12.178207126485491,12.497467851927466,12.497467851927466};
  const float cent3_r05[] = {6.909714175357281,6.941482634497519,6.889185762159225,6.781812440795706,6.780044976519611,6.7530051813471506,6.715043783543844,6.732854716689825,6.737302997985544,6.8084974112494665,6.877395460685499,6.974151474188009,7.064350477822313,7.176936019481957,7.301900886868623,7.419471180038499,7.5277760636291955,7.69235199110358,7.827313769751693,8.001995410883701,8.213716947618888,8.45238619778041,8.740402219730624,9.043759409539897,9.408735136596835,9.790658287991416,10.21671383791351,10.566747391395342,10.908608254550483,11.206015811549854,11.453054858615312,11.662973065121037,11.80972523291578,11.931816041142081,12.04509100069274,12.113109608884598,12.170753272023298,12.233087330873309,12.273026170616532,12.313517470618496,12.351997221257381,12.36494088435836,12.41482415626672,12.436343940589348,12.456833381797034,12.545529203539823,12.66328564539463,12.66328564539463};
  const float cent4_r05[] = {6.6537397080190965,6.550904047667968,6.534577546296297,6.530736989172197,6.574657132184215,6.658792910324833,6.740204708290686,6.875387240528138,7.030754896051985,7.183723155041517,7.339947578883699,7.514065379248639,7.671223196110074,7.864639733444398,8.083751123370053,8.319310478874609,8.599123778565671,8.95921411157321,9.360067548810957,9.811825073739122,10.28822343553129,10.760978607905093,11.120539702870877,11.45501388134468,11.690235643316008,11.860371959942775,12.003539283272035,12.103732065839274,12.15643713005413,12.225219724462944,12.28712897448695,12.318864553491668,12.359639081321834,12.375937375709585,12.419167706218559,12.425986551464366,12.451473927830234,12.461817315493285,12.495400340715502,12.575794992591467,12.56773068037154,12.582224980788556,12.622244935885524,12.67392336546223,12.637973287587306,12.778374140634423,12.09920709835756,12.09920709835756 };
  const float cent5_r05[] = {6.370172237427774,6.387955086181812,6.463972826275649,6.572961409537285,6.741569942972157,6.935165302196573,7.126566823225908,7.341741809778422,7.557680816879341,7.7776183026098,7.9980398260649626,8.247249842193005,8.552492890180156,8.929469672912036,9.371637387044268,9.890204825889143,10.399241308812634,10.892989471888711,11.3022248909677,11.614714529115023,11.80687340370228,11.970438414779094,12.06999089028954,12.121218074656188,12.206894492873193,12.252169766586926,12.313560236252327,12.348481273629158,12.360680700517303,12.409177049312527,12.42852619129373,12.442154897820407,12.473504218869854,12.50868257600523,12.482569962234917,12.579606949587012,12.572649572649572,12.624463233580773,12.673131295281852,12.707543929941044,12.811356442522968,12.853909465020577,12.873118279569892,12.833275230956946,12.879572118702553,12.813240877043464,11.166431095406361,11.166431095406361 };
  const float cent6_r05[] = {6.308386175913143,6.42338394114885,6.602109265702909,6.812140313781032,7.033938487731509,7.312504844229961,7.573017153261356,7.835866945739278,8.10513968226249,8.381854817043683,8.720662434376077,9.142901073218734,9.661213024124066,10.209898268639762,10.759207523831865,11.251177105944778,11.589754006088667,11.797303221055216,11.930286027837672,12.02550382964854,12.105038184648311,12.183201197538393,12.253146420061512,12.305882352941177,12.336001961050204,12.380913486311957,12.415455422004229,12.460271429834163,12.472172572944718,12.450414242737704,12.510211995863495,12.517935978319468,12.554834697703773,12.563602869885592,12.648738755740077,12.744661884821095,12.765414204482106,12.74331947743468,12.846362114797676,12.989305168061644,12.849851164419542,12.822821770614343,12.788282504012841,12.835341365461847,12.883333333333333,12.5300478784877,11.07112970711297,11.07112970711297};
  const float cent7_r05[] = {6.35220056325584,6.534844176553461,6.775140753001043,7.040294427677645,7.337199191973705,7.624915418787326,7.921132152279257,8.203281192771191,8.520163885658652,8.90549118115766,9.398116871799553,9.93488646445738,10.526279874660174,11.046758920453286,11.475193698899876,11.747382237560098,11.91085747402839,12.0250544008404,12.088923759256136,12.144368655829279,12.179271803821775,12.229291368990152,12.29414405621706,12.373211352917837,12.384045752335947,12.432011851851852,12.475417062485905,12.47304750044476,12.489549178297953,12.524902143522834,12.544368871867402,12.584742945014755,12.61931509823076,12.662184691214845,12.7568570034163,12.75531914893617,12.855204778156997,12.866132990534066,12.894019623111665,12.964776990412672,12.922590615341388,12.867652564582553,12.81560807483998,13.152813299232736,12.768835616438356,12.389427516158818,10.839723584485064,10.839723584485064 };
  const float cent8_r05[] = {6.39862619947977,6.6324545135136335,6.896933883331116,7.187541633892717,7.512587699710429,7.817489765057353,8.11328125,8.42758902091917,8.801430713482445,9.25820075577149,9.827220431249156,10.398661230786182,10.944210429398428,11.375279799331857,11.732005876488797,11.909265045004835,12.007258539576995,12.064727760418107,12.11060194717247,12.159109910084846,12.220750429808069,12.27575519659586,12.351493096173895,12.39307298691404,12.433498868281477,12.454411463139547,12.477500812995833,12.503007587951252,12.503443494337878,12.50093015015281,12.590824152294935,12.635046264433738,12.605706457396069,12.786511795425897,12.796721411563588,12.790944753187317,12.791128052148935,12.902248503431158,13.050380042876633,12.96520618556701,12.9984520123839,12.923759672280383,12.836996336996338,13.071311475409836,12.797777777777778,12.204488430443268,11.071493624772314,11.071493624772314 };
  const float cent9_r05[] = {6.438746048734351,6.671621502004274,6.962467414177581,7.277178700021744,7.589854316373239,7.902577279072988,8.200754209968277,8.53391336739038,8.955388275276126,9.439221484818265,10.030604055847643,10.61748926420981,11.11515634896432,11.534229274978197,11.786037899788987,11.96437355324074,12.02948405833405,12.06297536667541,12.124379134201172,12.162845565441868,12.231502895886457,12.296116175329713,12.342636176900042,12.393456204818332,12.444857864887211,12.452409853864465,12.460457613618994,12.511717783517907,12.495036378944194,12.514755555555556,12.590110170031204,12.627383161801923,12.650988549758845,12.754102858283035,12.814289582346172,12.78830058224163,12.811734012862516,12.959180327868852,12.958378145837814,12.911415654139587,12.867330988667447,12.818383440118286,13.010978043912175,12.674868189806679,12.702718006795017,12.1264647042012,11.10258121940365,11.10258121940365};

  // get nsignal correction
  // const int nsig_N = sizeof(cent1_r02)/sizeof(float) -   1;
  const float pt_range[] = {5.0,6.0,7.0,8.0,9.0,10.0,11.0,12.0,13.0,14.0,15.0,16.0,17.0,18.0,19.0,20.0,21.0,22.0,23.0,24.0,25.0,26.0,27.0,28.0,29.0,30.0,31.0,32.0,33.0,34.0,35.0,36.0,37.0,38.0,39.0,40.0,41.0,42.0,43.0,44.0,45.0,46.0,47.0,48.0,49.0,50.0,60.0,80.0,100.0};
  const int pt_N = sizeof(pt_range)/sizeof(float) - 1;

  // get pt bin
  int iptbin = -1;
  if (jet_pt >= pt_range[pt_N-1]) iptbin = pt_N-1;
  if(jet_pt < pt_range[0]) iptbin = 0;
  if(iptbin < 0)
  {
    for (int ipt = 0; ipt < pt_N; ++ipt)
    {
      if (jet_pt >= pt_range[ipt] && jet_pt < pt_range[ipt+1])
      {
        iptbin = ipt;
        break;
      }
    }
  }

  // get cent bin
  const float cent_bins[] =  {0,10,20,30,40,50,60,70,80,90,100};
  const int cent_N = sizeof(cent_bins)/sizeof(float) - 1;
  int icentbin = -1;
  for (int icent = 0; icent < cent_N; ++icent)
  {
    if (cent >= cent_bins[icent] && cent < cent_bins[icent+1])
    {
      icentbin = icent;
      break;
    }
  }

  float nsignal = -1;
  if (m_jet_R == 0.2)
  {
    if (icentbin == 0) nsignal = cent0_r02[iptbin];
    if (icentbin == 1) nsignal = cent1_r02[iptbin];
    if (icentbin == 2) nsignal = cent2_r02[iptbin];
    if (icentbin == 3) nsignal = cent3_r02[iptbin];
    if (icentbin == 4) nsignal = cent4_r02[iptbin];
    if (icentbin == 5) nsignal = cent5_r02[iptbin];
    if (icentbin == 6) nsignal = cent6_r02[iptbin];
    if (icentbin == 7) nsignal = cent7_r02[iptbin];
    if (icentbin == 8) nsignal = cent8_r02[iptbin];
    if (icentbin == 9) nsignal = cent9_r02[iptbin];

  }
  else if (m_jet_R == 0.3)
  {
    if (icentbin == 0) nsignal = cent0_r03[iptbin];
    if (icentbin == 1) nsignal = cent1_r03[iptbin];
    if (icentbin == 2) nsignal = cent2_r03[iptbin];
    if (icentbin == 3) nsignal = cent3_r03[iptbin];
    if (icentbin == 4) nsignal = cent4_r03[iptbin];
    if (icentbin == 5) nsignal = cent5_r03[iptbin];
    if (icentbin == 6) nsignal = cent6_r03[iptbin];
    if (icentbin == 7) nsignal = cent7_r03[iptbin];
    if (icentbin == 8) nsignal = cent8_r03[iptbin];
    if (icentbin == 9) nsignal = cent9_r03[iptbin];
  }
  else if (m_jet_R == 0.4)
  {
    if (icentbin == 0) nsignal = cent0_r04[iptbin];
    if (icentbin == 1) nsignal = cent1_r04[iptbin];
    if (icentbin == 2) nsignal = cent2_r04[iptbin];
    if (icentbin == 3) nsignal = cent3_r04[iptbin];
    if (icentbin == 4) nsignal = cent4_r04[iptbin];
    if (icentbin == 5) nsignal = cent5_r04[iptbin];
    if (icentbin == 6) nsignal = cent6_r04[iptbin];
    if (icentbin == 7) nsignal = cent7_r04[iptbin];
    if (icentbin == 8) nsignal = cent8_r04[iptbin];
    if (icentbin == 9) nsignal = cent9_r04[iptbin];
  }
  else if (m_jet_R == 0.5)
  {
    if (icentbin == 0) nsignal = cent0_r05[iptbin];
    if (icentbin == 1) nsignal = cent1_r05[iptbin];
    if (icentbin == 2) nsignal = cent2_r05[iptbin];
    if (icentbin == 3) nsignal = cent3_r05[iptbin];
    if (icentbin == 4) nsignal = cent4_r05[iptbin];
    if (icentbin == 5) nsignal = cent5_r05[iptbin];
    if (icentbin == 6) nsignal = cent6_r05[iptbin];
    if (icentbin == 7) nsignal = cent7_r05[iptbin];
    if (icentbin == 8) nsignal = cent8_r05[iptbin];
    if (icentbin == 9) nsignal = cent9_r05[iptbin];
  }
  else
  {
    std::cout << "JetBkgdSub::get_nsignal() - Error: invalid jet R = " << m_jet_R << std::endl;
    exit(1);
  }

  if (nsignal < 0)
  {
    std::cout << "JetBkgdSub::get_nsignal() - Error: nsignal = " << nsignal << std::endl;
    exit(1);
  }
  return nsignal;
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




