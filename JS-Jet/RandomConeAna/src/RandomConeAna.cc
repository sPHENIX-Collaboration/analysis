#include "RandomConeAna.h"


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
#include <cassert>
#include <utility>
#include <cstdlib>  // for exit
#include <iostream>
#include <memory>  // for allocator_traits<>::value_type
#include <vector>

#include <TTree.h>
#include <TMath.h>
#include <TRandom3.h> // for seed
#include <TTimeStamp.h>


RandomConeAna::RandomConeAna(const std::string& outputfilename)
 : SubsysReco("RandomConeAna")
  , m_outputfilename(outputfilename)
  , _doEventSelection(false)
  , m_eventSelection(-1, 100000)
  , _doWeight(false)
  , m_seed(false)
  , m_tree(nullptr)
{
}

RandomConeAna::~RandomConeAna()
{

    
  for (auto & _input : _inputs)
  {
    delete _input;
  }
  _inputs.clear();

  for (auto & _iter_input : _iter_inputs)
  {
    delete _iter_input;
  }
}

int RandomConeAna::Init(PHCompositeNode *topNode)
{ 

  if(!m_user_set_seed){
    TTimeStamp* ts = new TTimeStamp();
    m_seed = static_cast<unsigned int>(ts->GetNanoSec());
    delete ts;
  }

  // initialize TRandom3
  m_random = new TRandom3(m_seed);

  PHTFileServer::get().open(m_outputfilename, "RECREATE");
  m_tree = new TTree("T", "RandomConeAna");
  m_tree->Branch("event", &m_event, "event/I");
  if(_doWeight) m_tree->Branch("weight", &weight, "weight/F");
  m_tree->Branch("seed", &m_seed, "seed/I");
  m_tree->Branch("centrality", &m_centrality, "centrality/I");
  m_tree->Branch("rho_area", &m_rho_area, "rho_area/F");
  m_tree->Branch("rho_area_sigma", &m_rho_area_sigma, "rho_area_sigma/F");
  m_tree->Branch("rho_mult", &m_rho_mult, "rho_mult/F");
  m_tree->Branch("rho_mult_sigma", &m_rho_mult_sigma, "rho_mult_sigma/F");
  m_tree->Branch("cone_area", &m_cone_area, "cone_area/F");
  m_tree->Branch("cone_nclustered", &m_cone_nclustered, "cone_nclustered/I");
  m_tree->Branch("cone_pt_raw", &m_cone_pt_raw, "cone_pt_raw/F");
  m_tree->Branch("cone_pt_iter_sub", &m_cone_pt_iter_sub, "cone_pt_iter_sub/F");
  m_tree->Branch("randomized_cone_pt_raw", &m_randomized_cone_pt_raw, "randomized_cone_pt_raw/F");
  m_tree->Branch("randomized_cone_pt_iter_sub", &m_randomized_cone_pt_iter_sub, "randomized_cone_pt_iter_sub/F");
  m_tree->Branch("avoid_leading_cone_pt_raw", &m_avoid_leading_cone_pt_raw, "avoid_leading_cone_pt_raw/F");
  m_tree->Branch("avoid_leading_cone_pt_iter_sub", &m_avoid_leading_cone_pt_iter_sub, "avoid_leading_cone_pt_iter_sub/F");

  return Fun4AllReturnCodes::EVENT_OK;
}

int RandomConeAna::process_event(PHCompositeNode *topNode)
{
  // std::cout << "RandomConeAna::process_event(PHCompositeNode *topNode) Processing event " << m_event << std::endl;
  ++m_event;

  //==================================
  // Get centrality info
  //==================================
  GetCentInfo(topNode);
  

  // ==================================
  // Event selection
  //==================================
  if(!EventSelect(topNode)) return Fun4AllReturnCodes::EVENT_OK;

  //==================================
  // Get Rho Nodes
  //==================================
  EstimateRhos(topNode);


  //==================================
  // Reconstruct Random Cones
  //==================================
  m_cone_area = 0.4*0.4*TMath::Pi();

  // get input jets
  std::vector<Jet *> inputs;  // owns memory
  for (auto &_input : _inputs)
  {
    std::vector<Jet *> parts = _input->get_input(topNode);
    for (auto &part : parts)
    {
      inputs.push_back(part);
      inputs.back()->set_id(inputs.size() - 1);  // unique ids ensured
    }
  }

  std::vector<Jet *> iter_inputs;  // owns memory
  for (auto &_iter_input : _iter_inputs)
  {
    std::vector<Jet *> parts = _iter_input->get_input(topNode);
    for (auto &part : parts)
    {
      iter_inputs.push_back(part);
      iter_inputs.back()->set_id(iter_inputs.size() - 1);  // unique ids ensured
    }
  }


  // get leading truth jet eta phi
  std::pair<float,float> leading_truth_eta_phi = get_leading_jet_eta_phi(topNode);
  if(std::isnan(leading_truth_eta_phi.first) || std::isnan(leading_truth_eta_phi.second)){
    std::cout << "RandomConeAna::process_event(PHCompositeNode *topNode) - leading truth jet eta phi is nan" << std::endl;
    return Fun4AllReturnCodes::EVENT_OK;
  }

  // get pseudojets
  std::vector<fastjet::PseudoJet> pseudojets = jets_to_pseudojets(inputs, false);
  std::vector<fastjet::PseudoJet> iter_pseudojets = jets_to_pseudojets(iter_inputs, false);

  // get randomize eta phi pseudojets
  std::vector<fastjet::PseudoJet> randomize_pseudojets = jets_to_pseudojets(inputs, true);
  std::vector<fastjet::PseudoJet> randomize_iter_pseudojets = jets_to_pseudojets(iter_inputs, true);


  // select random cone
  float cone_eta = m_random->Uniform(-0.7, 0.7);
  float cone_phi = m_random->Uniform(-M_PI, M_PI);
  float cone_phi_avoid_lead_jet = 0;
  
  float dR_from_leading = 1.4;
  float current_deta = cone_eta - leading_truth_eta_phi.first;
  float test_phi;
  while(true)
  {
    test_phi = m_random->Uniform(-M_PI, M_PI);
    float test_dphi = test_phi - leading_truth_eta_phi.second;
    if (test_dphi > M_PI) test_dphi -= 2 * M_PI;
    if (test_dphi < -M_PI) test_dphi += 2 * M_PI;
    float test_dr = sqrt((current_deta*current_deta) + (test_dphi*test_dphi));
    if(test_dr > dR_from_leading)
    {
      cone_phi_avoid_lead_jet = test_phi;
      break;
    }
  }

  // control cone
  m_cone_nclustered = 0;
  m_cone_pt_raw = 0;
  for (unsigned int ipart = 0; ipart < pseudojets.size(); ++ipart)
  {
    // calculate the distance between the particle and the cone center
    float deta = cone_eta - pseudojets[ipart].eta();
    float dphi = cone_phi - pseudojets[ipart].phi_std();
    if (dphi > M_PI) dphi -= 2 * M_PI;
    if (dphi < -M_PI) dphi += 2 * M_PI;
    float dr = sqrt(deta * deta + dphi * dphi);
    if (dr <= 0.4)
    {
      m_cone_pt_raw += pseudojets[ipart].pt();
      m_cone_nclustered++;
    }
  }

  // iter sub cone
  m_cone_pt_iter_sub = 0;
  for (unsigned int ipart = 0; ipart < iter_pseudojets.size(); ++ipart)
  {
    // calculate the distance between the particle and the cone center
    float deta = cone_eta - iter_pseudojets[ipart].eta();
    float dphi = cone_phi - iter_pseudojets[ipart].phi_std();
    if (dphi > M_PI) dphi -= 2 * M_PI;
    if (dphi < -M_PI) dphi += 2 * M_PI;
    float dr = sqrt(deta * deta + dphi * dphi);
    if (dr <= 0.4)
    {
      m_cone_pt_iter_sub += iter_pseudojets[ipart].pt();
    }
  }

  // randomize cone
  m_randomized_cone_pt_raw = 0;
  for (unsigned int ipart = 0; ipart < randomize_pseudojets.size(); ++ipart)
  {
    // calculate the distance between the particle and the cone center
    float deta = cone_eta - randomize_pseudojets[ipart].eta();
    float dphi = cone_phi - randomize_pseudojets[ipart].phi_std();
    if (dphi > M_PI) dphi -= 2 * M_PI;
    if (dphi < -M_PI) dphi += 2 * M_PI;
    float dr = sqrt(deta * deta + dphi * dphi);
    if (dr <= 0.4)
    {
      m_randomized_cone_pt_raw += randomize_pseudojets[ipart].pt();
    }
  }


  // randomize iter sub cone
  m_randomized_cone_pt_iter_sub = 0;
  for (unsigned int ipart = 0; ipart < randomize_iter_pseudojets.size(); ++ipart)
  {
    // calculate the distance between the particle and the cone center
    float deta = cone_eta - randomize_iter_pseudojets[ipart].eta();
    float dphi = cone_phi - randomize_iter_pseudojets[ipart].phi_std();
    if (dphi > M_PI) dphi -= 2 * M_PI;
    if (dphi < -M_PI) dphi += 2 * M_PI;
    float dr = sqrt(deta * deta + dphi * dphi);
    if (dr <= 0.4)
    {
      m_randomized_cone_pt_iter_sub += randomize_iter_pseudojets[ipart].pt();
    }
  }


  // avoid leading jet cone
  m_avoid_leading_cone_pt_raw = 0;
  for (unsigned int ipart = 0; ipart < pseudojets.size(); ++ipart)
  {
    // calculate the distance between the particle and the cone center
    float deta = cone_eta - pseudojets[ipart].eta();
    float dphi = cone_phi_avoid_lead_jet - pseudojets[ipart].phi_std();
    if (dphi > M_PI) dphi -= 2 * M_PI;
    if (dphi < -M_PI) dphi += 2 * M_PI;
    float dr = sqrt(deta * deta + dphi * dphi);
    if (dr <= 0.4)
    {
      m_avoid_leading_cone_pt_raw += pseudojets[ipart].pt();
    }
  }

  // avoid leading jet iter sub cone
  m_avoid_leading_cone_pt_iter_sub = 0;
  for (unsigned int ipart = 0; ipart < iter_pseudojets.size(); ++ipart)
  {
    // calculate the distance between the particle and the cone center
    float deta = cone_eta - iter_pseudojets[ipart].eta();
    float dphi = cone_phi_avoid_lead_jet - iter_pseudojets[ipart].phi_std();
    if (dphi > M_PI) dphi -= 2 * M_PI;
    if (dphi < -M_PI) dphi += 2 * M_PI;
    float dr = sqrt(deta * deta + dphi * dphi);
    if (dr <= 0.4)
    {
      m_avoid_leading_cone_pt_iter_sub += iter_pseudojets[ipart].pt();
    }
  }

  //==================================
  // Fill tree
  //==================================
  m_tree->Fill();


  return Fun4AllReturnCodes::EVENT_OK;
}

int RandomConeAna::End(PHCompositeNode *topNode)
{
  std::cout << "RandomConeAna::End - Output to " << m_outputfilename << std::endl;
    // write tree to file
  PHTFileServer::get().cd(m_outputfilename);
  // m_tree->Write();
  // write file to disk
  PHTFileServer::get().write(m_outputfilename);
  std::cout << "RandomConeAna::End(PHCompositeNode *topNode) This is the End..." << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//===========================================================
// Event triggers
bool RandomConeAna::EventSelect(PHCompositeNode *topNode)
{
  //==================================
  // Event selection
  //==================================
  float m_event_leading_truth_pt = LeadingR04TruthJet(topNode);
  if(_doEventSelection)
  {
    if(m_event_leading_truth_pt < m_eventSelection.first) return false;
    if(m_event_leading_truth_pt > m_eventSelection.second) return false;
  }
  return true;
}

float RandomConeAna::LeadingR04TruthJet(PHCompositeNode *topNode)
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

//===========================================================
// Cent info  
void RandomConeAna::GetCentInfo(PHCompositeNode *topNode)
{
  //==================================
  // Get centrality info
  //==================================
  CentralityInfo* cent_node = findNode::getClass<CentralityInfo>(topNode, "CentralityInfo");
  if (!cent_node)
  {
      std::cout << "RandomConeAna::process_event() ERROR: Can't find CentralityInfo" << std::endl;
      exit(-1);
  }
  m_centrality = cent_node->get_centile(CentralityInfo::PROP::bimp);
  return ;
}


std::vector<fastjet::PseudoJet> RandomConeAna::get_psuedojets(PHCompositeNode *topNode)
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

    return calo_pseudojets;
}


void RandomConeAna::EstimateRhos(PHCompositeNode *topNode)
{

  m_rho_mult = -1;
  m_rho_area = -1;
  //==================================
  // Estimate rhos
  //==================================s
  // fastjet input
  std::vector<fastjet::PseudoJet> calo_pseudojets = get_psuedojets(topNode);
  
  fastjet::AreaDefinition area_def(fastjet::active_area_explicit_ghosts, 
            fastjet::GhostedAreaSpec(1.5, 1, 0.01));
  fastjet::JetDefinition jet_def(fastjet::kt_algorithm,
            0.4,
            fastjet::E_scheme, 
            fastjet::Best);
  fastjet::Selector jet_eta_selector = fastjet::SelectorAbsEtaMax(1.1);
  fastjet::Selector jet_pt_selector = fastjet::SelectorPtMin(1.0);
  fastjet::Selector jet_selector = (jet_eta_selector && jet_pt_selector);
  fastjet::Selector bkgd_selector = jet_selector * (!fastjet::SelectorNHardest(2));


  // area rho
  fastjet::JetMedianBackgroundEstimator bge {bkgd_selector, jet_def, area_def};
  bge.set_particles(calo_pseudojets);
  m_rho_area = bge.rho();
  m_rho_area_sigma = bge.sigma();


  // mult rho
  fastjet::ClusterSequenceArea cs(calo_pseudojets, jet_def, area_def);
  std::vector<fastjet::PseudoJet> jets = fastjet::sorted_by_pt( bkgd_selector( cs.inclusive_jets(1.0) ) );
  fastjet::Selector not_pure_ghost = !fastjet::SelectorIsPureGhost();

  std::vector<float> pt_over_nConstituents;
  int nfj_jets = 0;
  int n_empty_jets = 0;
  float total_constituents = 0;
  for (auto jet : jets) {
    float jet_pt = jet.pt();
    std::vector<fastjet::PseudoJet> constituents = not_pure_ghost(jet.constituents());
    int jet_size = constituents.size();
    total_constituents += jet_size;
    if(jet_size == 0){
      n_empty_jets++;
      continue; // only consider jets with constituents
    }
    pt_over_nConstituents.push_back(jet_pt/jet_size);
    nfj_jets++;
  }

  int total_jets = nfj_jets + n_empty_jets;
  float mean_pT = total_constituents/total_jets;
  if (mean_pT < 0) mean_pT = 0;

  float med_tmp, std_tmp;
  _median_stddev(pt_over_nConstituents, n_empty_jets, med_tmp, std_tmp);
  m_rho_mult_sigma = std_tmp*sqrt(mean_pT);
  m_rho_mult = med_tmp;

  return;
}

float RandomConeAna::_percentile(const std::vector<float> & sorted_vec, 
        const float percentile,
        const float nempty) const 
{
  assert(percentile >= 0. && percentile <= 1.);

  int njets = sorted_vec.size();
  if(njets == 0) return 0;

  float total_njets = njets + nempty;
  float perc_pos = (total_njets)*percentile - nempty - 0.5;

  float result;
  if(perc_pos >= 0 && njets > 1){
    int pindex = int(perc_pos);
    // avoid out of range
    if(pindex + 1 > njets-1){
      pindex = njets-2;
      perc_pos = njets - 1;
    }

    result = sorted_vec.at(pindex)*(pindex+1-perc_pos) 
    + sorted_vec.at(pindex+1)*(perc_pos-pindex);
  }
  else if(perc_pos > -0.5 && njets >=1){
    result = sorted_vec.at(0);
  }
  else{
    result = 0;
  }

  return result;

}

void RandomConeAna::_median_stddev(const std::vector<float> & vec,
              float n_empty_jets,
              float & median,
              float & std_dev) const 
{
  if(vec.size() == 0){
    median = 0;
    std_dev = 0;
    return;
  }

  std::vector<float> sorted_vec = vec;
  std::sort(sorted_vec.begin(), sorted_vec.end());

  int njets = sorted_vec.size();
  if(n_empty_jets > njets/4.0){
    std::cout << "WARNING: n_empty_jets = " << n_empty_jets << " is too large, setting to " << njets/4.0 << std::endl;
    n_empty_jets = njets/4.0;
  }


  float posn[2] = {0.5, (1.0-0.6827)/2.0};
  float res[2];
  for (int i = 0; i < 2; i++) {
    res[i] = _percentile(sorted_vec, posn[i], n_empty_jets);
  }
  
  median = res[0];
  std_dev = res[0] - res[1];

}

std::pair<float,float> RandomConeAna::get_leading_jet_eta_phi(PHCompositeNode *topNode)
{

  JetMap *truthjets = findNode::getClass<JetMap>(topNode, "AntiKt_Truth_r04");
  if(!truthjets) 
  {
    std::cout << "JetTree::LeadingR04TruthJet(PHCompositeNode *topNode) Could not find truth jet nodes" << std::endl;
    exit(-1);
  }

  float leading_truth_pt = -1;
  float leading_truth_eta = NAN;
  float leading_truth_phi = NAN;
  for(JetMap::Iter iter = truthjets->begin(); iter != truthjets->end(); ++iter){

    Jet *jet = iter->second;
    if(jet->get_pt() > leading_truth_pt)
    {
      leading_truth_pt = jet->get_pt();
      leading_truth_eta = jet->get_eta();
      leading_truth_phi = jet->get_phi();
    }
  
  }

  return std::make_pair(leading_truth_eta, leading_truth_phi);

}

std::vector<fastjet::PseudoJet> RandomConeAna::jets_to_pseudojets(std::vector<Jet*>& particles, bool randomize_etaphi)
{

  std::vector<fastjet::PseudoJet> pseudojets;
  for (unsigned int ipart = 0; ipart < particles.size(); ++ipart)
  {
    if (!std::isfinite(particles[ipart]->get_px()) ||
        !std::isfinite(particles[ipart]->get_py()) ||
        !std::isfinite(particles[ipart]->get_pz()) ||
        !std::isfinite(particles[ipart]->get_e()))
    {
      std::cout << PHWHERE << " invalid particle kinematics:"
                << " px: " << particles[ipart]->get_px()
                << " py: " << particles[ipart]->get_py()
                << " pz: " << particles[ipart]->get_pz()
                << " e: " << particles[ipart]->get_e() << std::endl;
      exit(-1);
    }

    fastjet::PseudoJet pseudojet(particles[ipart]->get_px(),
                                 particles[ipart]->get_py(),
                                 particles[ipart]->get_pz(),
                                 particles[ipart]->get_e());

    // do eta cuts
    if(pseudojet.eta() < -0.7 || pseudojet.eta() > 0.7) continue;
    if(randomize_etaphi){

        // randomize eta phi within acceptance
        float eta = m_random->Uniform(-0.7, 0.7);
        float phi = m_random->Uniform(-M_PI, M_PI);

        float E = pseudojet.e();
        float pT  = E/cosh(eta);
        float px = pT * cos(phi);
        float py = pT * sin(phi);
        float pz = pT * sinh(eta);
        pseudojet.reset(px, py, pz, E);

    }

    // do this after so the randomization is not affected
    if (particles[ipart]->get_e() == 0.) continue;
    
    pseudojet.set_user_index(ipart);
    pseudojets.push_back(pseudojet);

  }
  
  return pseudojets;
}







