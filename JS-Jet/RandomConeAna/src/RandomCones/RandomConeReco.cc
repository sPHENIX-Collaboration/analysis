#include "RandomConeReco.h"
#include "RandomCone.h"
#include "RandomConev1.h"

// fun4all includes
#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/SubsysReco.h>

// jetbase includes
#include <jetbase/Jet.h>
#include <jetbase/Jetv2.h>
#include <jetbase/JetMap.h>
#include <jetbase/JetMapv1.h>
#include <jetbase/JetInput.h>
#include <jetbase/JetContainer.h>
#include <jetbase/JetContainerv1.h>

// phool includes
#include <phool/PHCompositeNode.h>
#include <phool/PHIODataNode.h>
#include <phool/PHNode.h>
#include <phool/PHNodeIterator.h>
#include <phool/PHObject.h>
#include <phool/getClass.h>
#include <phool/phool.h>

// fastjet includes
#include <fastjet/Selector.hh>
#include <fastjet/ClusterSequence.hh>
#include <fastjet/JetDefinition.hh>
#include <fastjet/PseudoJet.hh>

//root includes
#include <TRandom3.h>
#include <TTimeStamp.h>

// standard includes
#include <cstdlib> 
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>



RandomConeReco::RandomConeReco(const std::string &name)
  : SubsysReco(name)
    // everything else should be initialized but the main options are here for clarity
  , m_do_basic_reconstruction(true) // do basic random cone reconstruction
  , m_do_randomize_etaphi_of_inputs(false) // randomize eta, phi of the input particles
  , m_do_avoid_leading_jet(false) // avoid leading jet of user-defined JetMap or JetContainer or reconstructed jet
{
}

RandomConeReco::~RandomConeReco()
{
  // memory cleanup
  for (auto & _input : m_inputs) { delete _input; }
  m_inputs.clear();

  if(m_random) delete m_random;

}

int RandomConeReco::Init(PHCompositeNode *topNode)
{

  std::cout << " RandomConeReco::InitRun - " << Name() << " - initialized" << std::endl;
  
  // set the random seed
  if(m_seed == 0) // use time as seed
  {
    TTimeStamp *t = new TTimeStamp();
    m_seed = static_cast<unsigned int>(t->GetNanoSec());
    delete t;
  }

  // create the random number generator
  m_random = new TRandom3(m_seed);

  // make sure eta range for the cone is set
  if(m_cone_max_abs_eta == 5.0)
  {
    m_cone_max_abs_eta = m_input_max_abs_eta - m_R;
  }

  // set up output node names

  // make r_string (e.g. 0.4 -> r04)
  std::string r_string = "r";
  if(m_R < 1.0) r_string += "0";
  r_string += std::to_string(static_cast<int>(m_R*10));

  if(m_do_basic_reconstruction){ m_output_basic_node = m_output_node_name + "_basic_" + r_string; }
  if(m_do_randomize_etaphi_of_inputs){ m_output_randomize_etaphi_node = m_output_node_name + "_randomize_etaphi_" + r_string; }
  if(m_do_avoid_leading_jet){ m_output_avoid_leading_jet_node = m_output_node_name + "_avoid_lead_jet_" + r_string; }

  // dR between the leading jet and the random cone
  if(m_do_avoid_leading_jet)
  {
    if(m_lead_jet_dR == 5.0)
    {
      m_lead_jet_dR = 1.0 + m_R;
    }
  }
  
  // print settings
  if(Verbosity() > 0) print_settings(std::cout);
  
  // create the nodes
  return CreateNode(topNode);

}



int RandomConeReco::process_event(PHCompositeNode *topNode)
{

  //==================================
  // Reconstruct Random Cones
  //==================================

  // get inputs 
  std::vector<Jet *> particles;  // owns memory
  for (auto & _input : m_inputs)
  {
    std::vector<Jet *> parts = _input->get_input(topNode);
    for (auto &part : parts)
    {
      particles.push_back(part);
      particles.back()->set_id(particles.size() - 1);  // unique ids ensured
    }
  }

  // generate random cone eta, phi
  float cone_eta = NAN;
  float cone_phi = NAN;
  GetConeAxis(topNode, cone_eta, cone_phi, false);

  // get pseudojets
  std::vector<fastjet::PseudoJet> pseudojets = JetsToPseudojets(particles, false);

  if(m_do_basic_reconstruction)
  {

    RandomCone * basic_reco = findNode::getClass<RandomConev1>(topNode, m_output_basic_node);
    if (!basic_reco)
    {
      std::cout << PHWHERE << "Can't find RandomCone node " << m_output_basic_node << std::endl;
      exit(-1); // fatal error
    }

    basic_reco->Reset();
    basic_reco->set_eta(cone_eta);
    basic_reco->set_phi(cone_phi);
    basic_reco->set_R(m_R);
    basic_reco->set_cone_type(RandomCone::ConeType::BASIC);

    // add constituents from the pseudojets
    for (auto &pseudojet : pseudojets)
    {
      float deta = pseudojet.eta() - cone_eta;
      float dphi = pseudojet.phi_std() - cone_phi;
      if(dphi > M_PI) dphi -= 2*M_PI;
      if(dphi < -M_PI) dphi += 2*M_PI;

      float dR = std::sqrt(deta*deta + dphi*dphi);
      if(dR < m_R)
      {
        basic_reco->add_constituent(particles[pseudojet.user_index()]);
      }

    }


    // delete basic_reco;    
  }

  if(m_do_avoid_leading_jet)
  {
    float cone_lead_jet_eta = NAN;
    float cone_lead_jet_phi = NAN;
    GetConeAxis(topNode, cone_lead_jet_eta, cone_lead_jet_phi, true);

    // get random cone object
    RandomCone * avoid_leading_jet = findNode::getClass<RandomConev1>(topNode, m_output_avoid_leading_jet_node);
    if (!avoid_leading_jet)
    {
      std::cout << PHWHERE << "Can't find RandomCone node " << m_output_avoid_leading_jet_node << std::endl;
      exit(-1); // fatal error
    }

    avoid_leading_jet->Reset();
    avoid_leading_jet->set_eta(cone_lead_jet_eta);
    avoid_leading_jet->set_phi(cone_lead_jet_phi);
    avoid_leading_jet->set_R(m_R);
    avoid_leading_jet->set_cone_type(RandomCone::ConeType::AVOID_LEAD_JET);

    // add constituents from the pseudojets
    for (auto &pseudojet : pseudojets)
    {

      float deta = pseudojet.eta() - cone_lead_jet_eta;
      float dphi = pseudojet.phi_std() - cone_lead_jet_phi;
      if(dphi > M_PI) dphi -= 2*M_PI;
      if(dphi < -M_PI) dphi += 2*M_PI;

      float dR = std::sqrt(deta*deta + dphi*dphi);
      if(dR < m_R)
      {
        avoid_leading_jet->add_constituent(particles[pseudojet.user_index()]);
      }

    }

    // delete avoid_leading_jet;

  }

  if(m_do_randomize_etaphi_of_inputs)
  {

    std::vector<fastjet::PseudoJet> randomize_pseudojets = JetsToPseudojets(particles, true);

    // get random cone object
    RandomCone * randomize_etaphi = findNode::getClass<RandomConev1>(topNode, m_output_randomize_etaphi_node);
    if (!randomize_etaphi)
    {
      std::cout << PHWHERE << "Can't find RandomCone node " << m_output_randomize_etaphi_node << std::endl;
      exit(-1); // fatal error
    }

    randomize_etaphi->Reset();
    randomize_etaphi->set_eta(cone_eta);
    randomize_etaphi->set_phi(cone_phi);
    randomize_etaphi->set_R(m_R);
    randomize_etaphi->set_cone_type(RandomCone::ConeType::RANDOMIZE_INPUT_ETAPHI);

    // add constituents from the pseudojets
    for (auto &pseudojet : randomize_pseudojets)
    {
      float deta = pseudojet.eta() - cone_eta;
      float dphi = pseudojet.phi_std() - cone_phi;
      if(dphi > M_PI) dphi -= 2*M_PI;
      if(dphi < -M_PI) dphi += 2*M_PI;

      float dR = std::sqrt(deta*deta + dphi*dphi);
      if(dR < m_R)
      {
        randomize_etaphi->add_constituent(particles[pseudojet.user_index()]);
      }
    }

    // delete randomize_etaphi;

  }

  // memory cleanup
  for (auto &part : particles) delete part;
  particles.clear();

  return Fun4AllReturnCodes::EVENT_OK;

}

int RandomConeReco::CreateNode(PHCompositeNode *topNode)
{
  PHNodeIterator iter(topNode);

  // Looking for the DST node
  PHCompositeNode *dstNode = dynamic_cast<PHCompositeNode *>(iter.findFirst("PHCompositeNode", "DST"));
  if (!dstNode)
  {
    std::cout << PHWHERE << "DST Node missing, doing nothing." << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  // store the jet background stuff under a sub-node directory
  PHCompositeNode *coneNode = dynamic_cast<PHCompositeNode *>(iter.findFirst("PHCompositeNode", "RANDOMCONE"));
  if (!coneNode)
  {
    coneNode = new PHCompositeNode("RANDOMCONE");
    dstNode->addNode(coneNode);
  }

  // create the RandomCone nodes
  if(m_do_basic_reconstruction)
  {
    RandomCone *basic_reco = new RandomConev1();
    PHIODataNode<PHObject> *node = new PHIODataNode<PHObject>(basic_reco, m_output_basic_node, "PHObject");
    coneNode->addNode(node);
  }

  if(m_do_randomize_etaphi_of_inputs)
  {
    RandomCone *randomize_etaphi = new RandomConev1();
    PHIODataNode<PHObject> *node = new PHIODataNode<PHObject>(randomize_etaphi, m_output_randomize_etaphi_node, "PHObject");
    coneNode->addNode(node);
  }

  if(m_do_avoid_leading_jet)
  {
    RandomCone *avoid_leading_jet = new RandomConev1();
    PHIODataNode<PHObject> *node = new PHIODataNode<PHObject>(avoid_leading_jet, m_output_avoid_leading_jet_node, "PHObject");
    coneNode->addNode(node);
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

// Get lead jet for avoiding leading jet mode
Jet * RandomConeReco::GetLeadJet(PHCompositeNode *topNode)
{
  // reconstruct jet from m_inputs (should be m_inputs unless user set different inputs)
  if(m_leading_jet_from_inputs) // jet from input
  {

    // get jet inputs
    std::vector<Jet *> particles;  // owns memory
    for (auto & _input : m_inputs)
    {
      std::vector<Jet *> parts = _input->get_input(topNode);
      for (auto &part : parts)
      {
        particles.push_back(part);
        particles.back()->set_id(particles.size() - 1);  // unique ids ensured
      }
    }


    //convert to pseudojets
    std::vector<fastjet::PseudoJet> pseudojets;
    for (unsigned int ipart = 0; ipart < particles.size(); ++ipart)
    {

      // check for invalid kinematics
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
        
        exit(-1); // fatal error
      }

      float this_e = particles[ipart]->get_e();
      if (this_e == 0.) continue;
      float this_px = particles[ipart]->get_px();
      float this_py = particles[ipart]->get_py();
      float this_pz = particles[ipart]->get_pz();


      if(this_e < 0)
      {
        // make energy +1 MeV for clustering purposes
        float e_ratio = 0.001 / this_e;
        this_e  = this_e * e_ratio;
        this_px = this_px * e_ratio;
        this_py = this_py * e_ratio;
        this_pz = this_pz * e_ratio;
      }

      // create pseudojet
      fastjet::PseudoJet pseudojet(this_px, this_py, this_pz, this_e);

      // do eta cut
      if (fabs(pseudojet.eta()) > m_input_max_abs_eta) continue;


      // do pT cut
      if (pseudojet.perp() < m_input_min_pT) continue;

      pseudojet.set_user_index(ipart); // keep track of the original index
      pseudojets.push_back(pseudojet);


    }

    // cluster jets
    fastjet::JetDefinition jet_def(fastjet::kt_algorithm, m_R, fastjet::E_scheme,  fastjet::Best);
    fastjet::ClusterSequence cs(pseudojets, jet_def);

    // select jets with |eta| < user-eta - R and pt > 1.0 only the hardest jet
    fastjet::Selector jet_selector = (
          (fastjet::SelectorNHardest(1)) * 
          ( fastjet::SelectorAbsEtaMax(m_cone_max_abs_eta) &&
            fastjet::SelectorPtMin(m_lead_jet_pT_threshold))
    );


    std::vector<fastjet::PseudoJet> jets = fastjet::sorted_by_pt(jet_selector(cs.inclusive_jets())); // only the hardest jet
 
    if(jets.size() == 0)
    { // no jet found
      std::cout << "RandomConeAna::GetLeadJetKinematics(PHCompositeNode *topNode) - Could not find leading jet above threshold" << std::endl;
      return nullptr;
    }

    // grab the leading jet
    fastjet::PseudoJet lead_jet = jets.at(0);

    // get constituents
    std::vector<fastjet::PseudoJet> constituents = lead_jet.constituents();

    // create a jet object
    auto jet = new Jetv2();
    jet->set_px(lead_jet.px());
    jet->set_py(lead_jet.py());
    jet->set_pz(lead_jet.pz());
    jet->set_e(lead_jet.e());
    jet->set_id(0); // unique id

    // memory cleanup
    for (auto & particle : particles) delete particle;
    particles.clear();

    return jet;
  }
  else if(m_leading_jet_from_jetmap) // jet from jetmap on DST tree
  {
    // get jet map
    JetMap *jetmap = findNode::getClass<JetMap>(topNode, m_leading_jet_node_name);
    if(!jetmap) 
    {
      std::cout << "RandomConeAna::GetLeadJetKinematics(PHCompositeNode *topNode) Could not find jet map node" << std::endl;
      exit(-1); // fatal error
    }

    // find lead jet
    float lead_pt = -1;
    unsigned int lead_id = 0;

    for(JetMap::Iter iter = jetmap->begin(); iter != jetmap->end(); ++iter)
    {
      Jet *jet = iter->second;
      if(jet->get_pt() > lead_pt)
      {
        lead_pt = jet->get_pt();
        lead_id = jet->get_id();
      }
    }

    if(lead_pt < m_lead_jet_pT_threshold)
    {
      std::cout << "RandomConeAna::GetLeadJetKinematics(PHCompositeNode *topNode) Could not find leading jet above threshold" << std::endl;
      return nullptr;
    }

    // get jet from jetmap
    auto jetv1 = jetmap->get(lead_id);

    // create a jetv2 object
    auto jet = new Jetv2();
    jet->set_px(jetv1->get_px());
    jet->set_py(jetv1->get_py());
    jet->set_pz(jetv1->get_pz());
    jet->set_e(jetv1->get_e());
    jet->set_id(jetv1->get_id());

  }
  else if(m_leading_jet_from_jetcont) // jet from jetcontainer on DST tree
  {
    // get jet container
    JetContainer *jetcont = findNode::getClass<JetContainer>(topNode, m_leading_jet_node_name);
    if(!jetcont) 
    {
      std::cout << "RandomConeAna::GetLeadJetKinematics(PHCompositeNode *topNode) Could not find jet container node" << std::endl;
      exit(-1); // fatal error
    }

    // find lead jet
    float lead_pt = -1;
    unsigned int lead_id = 0;
    unsigned int n_ijets_in_event = jetcont->size();
    for (unsigned int ijet = 0; ijet<=n_ijets_in_event; ++ijet)
    {
      Jet *jet = jetcont->get_jet(ijet);
      if(jet->get_pt() > lead_pt)
      {
        lead_pt = jet->get_pt();
        lead_id = jet->get_id();
      }

    }

    if(lead_pt < m_lead_jet_pT_threshold)
    {
      std::cout << "RandomConeAna::GetLeadJetKinematics(PHCompositeNode *topNode) Could not find leading jet above threshold" << std::endl;
      return nullptr;
    }

    // get jet from jetcontainer (all ready a Jetv2 object)
    auto jet = jetcont->get_jet(lead_id);

    return jet;
  }

  // if we get here, something went wrong
  return nullptr;

}

void RandomConeReco::GetConeAxis(PHCompositeNode *topNode,float &cone_eta, float &cone_phi, bool avoid_lead_jet)
{
  if(avoid_lead_jet)
  {
    Jet *lead_jet = GetLeadJet(topNode);
    if(lead_jet == nullptr)
    {
      std::cout << "RandomConeAna::GetConeAxis(PHCompositeNode *topNode) - Could not find leading jet." << std::endl;
      std::cout << "RandomConeAna::GetConeAxis(PHCompositeNode *topNode) - Disabling avoid leading jet mode for this event" << std::endl;
      
      cone_eta = m_random->Uniform(-m_cone_max_abs_eta, m_cone_max_abs_eta);
      cone_phi = m_random->Uniform(-M_PI, M_PI);
      return;
    }


    float jet_eta = lead_jet->get_eta();
    float jet_phi = lead_jet->get_phi();

    while(true)
    {
      // set jet_phi to 0 and select phi from dR - (2pi-dR) range
      float dphi = m_random->Uniform(m_lead_jet_dR, 2*M_PI - m_lead_jet_dR);
      cone_phi = jet_phi + dphi;
      if(cone_phi > M_PI) cone_phi -= 2*M_PI;
      if(cone_phi < -M_PI) cone_phi += 2*M_PI;

      cone_eta = m_random->Uniform(-m_cone_max_abs_eta, m_cone_max_abs_eta);

      float deta = cone_eta - jet_eta;
      float dR = std::sqrt(deta*deta + dphi*dphi);
      if(dR > m_lead_jet_dR) break;
    }

    // delete lead_jet;
  }
  else
  {
    cone_eta = m_random->Uniform(-m_cone_max_abs_eta, m_cone_max_abs_eta);
    cone_phi = m_random->Uniform(-M_PI, M_PI);
  }

  return;
}

std::vector<fastjet::PseudoJet> RandomConeReco::JetsToPseudojets(const std::vector<Jet *> &particles, bool randomize_etaphi)
{


  std::vector<fastjet::PseudoJet> pseudojets;

  // convert to pseudojets
  for (unsigned int ipart = 0; ipart < particles.size(); ++ipart)
  {

    // check for invalid kinematics
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
      
      exit(-1); // fatal error
    }

    float this_e = particles[ipart]->get_e();
    // if (this_e == 0.) continue;
    float this_px = particles[ipart]->get_px();
    float this_py = particles[ipart]->get_py();
    float this_pz = particles[ipart]->get_pz();

    // cut zero energy particles
    if(m_do_tower_cut)
    {
      if(this_e < m_tower_threshold) continue;
      if(this_e < 0)
      {
        // make energy +1 MeV for clustering purposes
        float e_ratio = 0.001 / this_e;
	      this_e  = this_e * e_ratio;
	      this_px = this_px * e_ratio;
	      this_py = this_py * e_ratio;
	      this_pz = this_pz * e_ratio;
      }
    
    }

    // create pseudojet
    fastjet::PseudoJet pseudojet(this_px, this_py, this_pz, this_e);

    // do eta cut
    if (fabs(pseudojet.eta()) > m_input_max_abs_eta) continue;

    // do pT cut
    // if (pseudojet.perp() < m_input_min_pT) continue;

   

    if(randomize_etaphi)
    {
      float eta = m_random->Uniform(-m_input_max_abs_eta, m_input_max_abs_eta);
      float phi = m_random->Uniform(-M_PI, M_PI);

      float E = pseudojet.E();
      float pT = pseudojet.perp();

      float px = pT * cos(phi);
      float py = pT * sin(phi);
      float pz = pT * sinh(eta);

      pseudojet.reset(px, py, pz, E);
    }

    pseudojet.set_user_index(ipart); // keep track of the original index
    pseudojets.push_back(pseudojet);
  }



  // return pseudojets
  return pseudojets;
}

void RandomConeReco::print_settings(std::ostream& os) const
{

  os << "RandomConeReco settings: " << std::endl;
  os << "  - Random seed: " << m_seed << std::endl;
  os << "  - Cone radius: " << m_R << std::endl;
  os << "  - Inputs: " ;
  for (auto & _input : m_inputs) {_input->identify(os); }
  os << std::endl;
  os << "  - Min input pT: " << m_input_min_pT << std::endl;
  os << "  - Max input |eta|: " << m_input_max_abs_eta << std::endl;
  os << "  - Cone max |eta|: " << m_cone_max_abs_eta << std::endl;
  if(!std::isnan(m_tower_threshold))  os << "  - Tower threshold: " << m_tower_threshold << std::endl;

  if(m_do_avoid_leading_jet)
  {
    os << "  - Avoid leading jet: true" << std::endl;
    if(m_leading_jet_from_inputs) os << "  -  - Leading jet from inputs" << std::endl;
    else
    {
      os << "  -  - Leading jet from JetMap or JetContainer" << std::endl;
      os << "  -  - Node name: " << m_leading_jet_node_name << std::endl;
    }
    os << "  - dR between leading jet and random cone: " << m_lead_jet_dR << std::endl;
    os << "  - output node name: " << m_output_avoid_leading_jet_node << std::endl;
  }
  else
  {
    os << "  - Avoid leading jet: false" << std::endl;
  }

  if(m_do_randomize_etaphi_of_inputs)
  {
    os << "  - Randomize eta, phi of inputs: true" << std::endl;
    os << "  - output node name: " << m_output_randomize_etaphi_node << std::endl;
  }
  else
  {
    os << "  - Randomize eta, phi of inputs: false" << std::endl;
  }

  if(m_do_basic_reconstruction)
  {
    os << "  - Basic reconstruction: true" << std::endl;
    os << "  - output node name: " << m_output_basic_node << std::endl;
  }
  else
  {
    os << "  - Basic reconstruction: false" << std::endl;
  }

  os << "===============================" << std::endl;

  return;

}

