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


JetBkgdSub::JetBkgdSub(const double jet_R, const std::string& outputfilename)
 : SubsysReco("JetBkgdSub")
  , m_jet_R(jet_R)
  , m_outputfilename(outputfilename)
  , m_etaRange(-1.1, 1.1)
  , m_ptRange(0.0, 100000.0)
  , _minrecopT(5.0)
  , _doIterative(false)
  , _doAreaSub(false)
  , _doMultSub(false)
  , _doTruth(false)
  , m_event(-1)
  , m_rhoA_jets(0)
  , m_mult_jets(0)
  , m_iter_jets(0)
  , m_truth_jets(0)
  , m_centrality(-1)
  , m_rho_area(-1)
  , m_rho_area_sigma(-1)
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

  return Fun4AllReturnCodes::EVENT_OK;
}

int JetBkgdSub::process_event(PHCompositeNode *topNode)
{
  ++m_event;

  // min reco jet pt cut 
  // sets min for all subtraction types
  double min_reco_jet_pt = _minrecopT;

  //==================================
  // Get centrality info
  //==================================
  GetCentInfo(topNode);

  // Leading truth jet pt (R = 0.4) (for simulation event selection)
  m_event_leading_truth_pt = LeadingR04TruthJet(topNode);

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
    JetMap *iterjets = findNode::getClass<JetMap>(topNode, m_iter_input);
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
    for(JetMap::Iter iter = iterjets->begin(); iter != iterjets->end(); ++iter){

      Jet *jet = iter->second;
      if(jet->get_pt() < 1.0) continue;
      m_iter_eta.push_back(jet->get_eta());
      m_iter_phi.push_back(jet->get_phi());
      m_iter_pt.push_back(jet->get_pt());

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
    fastjet::Selector selector_rm2 = jetrap * (!fastjet::SelectorNHardest(2));
    fastjet::JetMedianBackgroundEstimator bge_rm2 {selector_rm2, jet_def_bkgd, area_def};
    // set particles for background estimation
    bge_rm2.set_particles(calo_pseudojets);
    
    // rho and sigma
    m_rho_area = bge_rm2.rho();
    m_rho_area_sigma = bge_rm2.sigma();
    
    // cluster jets
    fastjet::ClusterSequenceArea clustSeq(calo_pseudojets, jet_def_antikt, area_def);
    std::vector<fastjet::PseudoJet> jets =  fastjet::sorted_by_pt( selection( clustSeq.inclusive_jets(5.) ));

    m_rhoA_jets = 0; // loop over rhoA jets
    for (auto jet : jets) {
      if (jet.pt() < min_reco_jet_pt) continue; // only consider jets with pt > 5.0
      m_rhoA_eta.push_back(jet.eta());
      m_rhoA_phi.push_back(jet.phi_std());
      m_rhoA_pt_unsub.push_back(jet.pt());
      m_rhoA_area.push_back(jet.area());
      m_rhoA_pt.push_back(jet.pt() - jet.area() * m_rho_area); // subtract rhoA
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
    JetMap *multjets = findNode::getClass<JetMap>(topNode, m_raw_input);
    if(!multjets) 
    {
      std::cout << "JetTree::process_event(PHCompositeNode *topNode) Could not find mult jet nodes" << std::endl;
      exit(-1);
    }

    // loop over mult jets
    m_mult_jets = 0;
    for(JetMap::Iter iter = multjets->begin(); iter != multjets->end(); ++iter){

      Jet *jet = iter->second;
      if(jet->get_pt() < min_reco_jet_pt) continue;
      m_mult_ncomponent.push_back(jet->size_comp());
      m_mult_eta.push_back(jet->get_eta());
      m_mult_phi.push_back(jet->get_phi());
      m_mult_pt_unsub.push_back(jet->get_pt());
      float nsignal = NSignalCorrection(jet->get_pt(), m_centrality);
      m_mult_nsignal.push_back(nsignal);
      m_mult_pt.push_back(jet->get_pt() - m_rho_mult*(jet->size_comp() - nsignal));
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
  m_centrality = cent_node->get_centile(CentralityInfo::PROP::bimp);
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
 
  // r 0.2
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

  // r 0.4
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




