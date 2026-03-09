#include "RandomConeTree.h"

// random cones
#include <randomcones/RandomCone.h>
#include <randomcones/RandomConev1.h>

// tower rho
#include <towerrho/TowerRho.h>
#include <towerrho/TowerRhov1.h>

// fun4all includes
#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/PHTFileServer.h>

// phool includes
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

// jet reco
#include <jetbase/Jet.h>
#include <jetbase/JetMap.h>
#include <jetbase/Jetv1.h>
#include <jetbase/JetMapv1.h>
#include <jetbase/JetContainer.h>
#include <jetbase/JetContainerv1.h>
#include <jetbase/Jetv2.h>


// centrality
#include <centrality/CentralityInfo.h>

// global vertex
#include <globalvertex/GlobalVertex.h>
#include <globalvertex/GlobalVertexMap.h>
#include <globalvertex/GlobalVertexMapv1.h>

// mbd 
#include <mbd/MbdOut.h>

// root includes
#include <TTree.h>

// standard includes
#include <cmath>
#include <utility>
#include <cstdlib>  
#include <iostream>
#include <vector>
#include <string>
#include <array>


RandomConeTree::RandomConeTree(const std::string& outputfilename)
 : SubsysReco("RandomConeTree")
  , m_outputfile_name(outputfilename)
{
}

RandomConeTree::~RandomConeTree()
{
  // probably don't need to delete these but
  if(m_run_info_tree) delete m_run_info_tree;
  if(m_event_tree) delete m_event_tree;
}

int RandomConeTree::Init(PHCompositeNode *topNode)
{ 
  // create output file
  PHTFileServer::get().open(m_outputfile_name, "RECREATE");

  // create trees
  m_run_info_tree = new TTree("run_info", "RunInfo");
  m_event_tree = new TTree("event_info", "EventInfo");

  // run info tree
  if(m_do_gvtx_cut)
  {
    m_run_info_tree->Branch("zvtx_low", &m_gvtx_cut.first, "zvtx_low/F");
    m_run_info_tree->Branch("zvtx_high", &m_gvtx_cut.second, "zvtx_high/F");
  }
  if(m_MC_do_event_selection)
  {
    m_run_info_tree->Branch("MC_event_selection_jetpT_low", &m_MC_event_selection_jetpT_range.first, "MC_event_selection_jetpT_low/F");
    m_run_info_tree->Branch("MC_event_selection_jetpT_high", &m_MC_event_selection_jetpT_range.second, "MC_event_selection_jetpT_high/F");
  }
  if(m_MC_add_weight_to_ttree)
  {
    m_run_info_tree->Branch("MC_event_weight", &m_MC_event_weight, "MC_event_weight/D");
  }

  // fill run info tree
  m_run_info_tree->Fill();

  // event info tree
  m_event_tree->Branch("event_id", &m_event_id, "event_id/I");
  if(m_do_cent_info)
  {
    m_event_tree->Branch("centrality", &m_centrality, "centrality/I");
    if(!m_do_data) m_event_tree->Branch("impactparam", &m_impactparam, "impactparam/F");
    m_event_tree->Branch("mbd_NS", &m_mbd_NS, "mbd_NS/D");
  }
  m_event_tree->Branch("zvtx", &m_zvtx, "zvtx/F");

  // add rho nodes
  for(unsigned int i = 0; i < m_rho_nodes.size(); ++i)
  {
    std::string name = m_rho_nodes.at(i)+"_rho";
    m_event_tree->Branch(name.c_str(), &m_rhos[i], (name + "/F").c_str());
    name = m_rho_nodes.at(i)+"_sigma";
    m_event_tree->Branch(name.c_str(), &m_sigma_rhos[i], (name + "/F").c_str());
  }

  // add random cone nodes
  for(unsigned int i = 0; i < m_random_cone_nodes.size(); ++i)
  {
    std::string name = m_random_cone_nodes.at(i)+"_R";
    m_event_tree->Branch(name.c_str(), &m_cone_R[i], (name + "/F").c_str());
    name = m_random_cone_nodes.at(i)+"_eta";
    m_event_tree->Branch(name.c_str(), &m_cone_etas[i], (name + "/F").c_str());
    name = m_random_cone_nodes.at(i)+"_phi";
    m_event_tree->Branch(name.c_str(), &m_cone_phis[i], (name + "/F").c_str());
    name = m_random_cone_nodes.at(i)+"_pt";
    m_event_tree->Branch(name.c_str(), &m_cone_pts[i], (name + "/F").c_str());
    name = m_random_cone_nodes.at(i)+"_nclustered";
    m_event_tree->Branch(name.c_str(), &m_cone_nclustereds[i], (name + "/I").c_str());
  }


  m_event_id = -1;


  return Fun4AllReturnCodes::EVENT_OK;

}

int RandomConeTree::ResetEvent(PHCompositeNode *topNode)
{
  // reset event variables
  m_centrality = -1;
  m_impactparam = NAN;
  m_zvtx = NAN;
  m_mbd_NS = NAN;

  for(unsigned int i = 0; i < m_rho_nodes.size(); ++i)
  {
    m_rhos[i] = NAN;
    m_sigma_rhos[i] = NAN;
  }

  for(unsigned int i = 0; i < m_random_cone_nodes.size(); ++i)
  {
    m_cone_R[i] = NAN;
    m_cone_etas[i] = NAN;
    m_cone_phis[i] = NAN;
    m_cone_pts[i] = NAN;
    m_cone_nclustereds[i] = -1;
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

int RandomConeTree::process_event(PHCompositeNode *topNode)
{

  ++m_event_id;

  // event selection
  if(!GoodEvent(topNode)) return Fun4AllReturnCodes::ABORTEVENT;

  // Centrality info
  if(m_do_cent_info)  GetCentInfo(topNode);
  

  // rho info
  for(unsigned int i = 0; i < m_rho_nodes.size(); ++i)
  {
    TowerRho* towerrho = findNode::getClass<TowerRhov1>(topNode, m_rho_nodes.at(i));
    if (!towerrho)
    {
      std::cout << "RandomConeTree - Error can not find towerrho " << m_rho_nodes.at(i) << std::endl;
      exit(-1);
    }
    m_rhos[i] = towerrho->get_rho();
    m_sigma_rhos[i] = towerrho->get_sigma();
  }

  // random cone info
  for(unsigned int i = 0; i < m_random_cone_nodes.size(); ++i)
  {
    RandomCone* rcone = findNode::getClass<RandomConev1>(topNode, m_random_cone_nodes.at(i));
    if (!rcone)
    {
      std::cout << "RandomConeTree - Error can not find random cone " << m_random_cone_nodes.at(i) << std::endl;
      exit(-1);
    }
    m_cone_R[i] = rcone->get_R();
    m_cone_etas[i] = rcone->get_eta();
    m_cone_phis[i] = rcone->get_phi();
    m_cone_pts[i] = rcone->get_pt();
    m_cone_nclustereds[i] = rcone->n_clustered();
  }

  // fill event tree
  m_event_tree->Fill();

  return Fun4AllReturnCodes::EVENT_OK;



  // //==================================
  // //get towerrho

  // TowerRho* towerrho_area = findNode::getClass<TowerRhov1>(topNode, "TowerRho_AREA");
  // if (!towerrho_area)
  // {
  //   std::cout << "RandomConeTree - Error can not find towerrho area" << std::endl;
  //   exit(-1);
  // }

  // TowerRho* towerrho_mult = findNode::getClass<TowerRhov1>(topNode, "TowerRho_MULT");
  // if (!towerrho_mult)
  // {
  //   std::cout << "RandomConeTree - Error can not find towerrho mult" << std::endl;
  //   exit(-1);
  // }

  // m_rho_area = towerrho_area->get_rho();
  // m_rho_area_sigma = towerrho_area->get_sigma();
  // m_rho_mult = towerrho_mult->get_rho();
  // m_rho_mult_sigma = towerrho_mult->get_sigma();

 
  // RandomCone* rcone_basic = findNode::getClass<RandomConev1>(topNode, "RandomCone_TOWER_basic_r04");
  // RandomCone* rcone_randomize_etaphi = findNode::getClass<RandomConev1>(topNode, "RandomCone_TOWER_randomize_etaphi_r04");
  // RandomCone* rcone_avoid_lead_jet = findNode::getClass<RandomConev1>(topNode, "RandomCone_TOWER_avoid_lead_jet_r04");
  // RandomCone* rcone_sub_basic = findNode::getClass<RandomConev1>(topNode, "RandomCone_TOWER_SUB1_basic_r04");
  // RandomCone* rcone_sub_randomize_etaphi = findNode::getClass<RandomConev1>(topNode, "RandomCone_TOWER_SUB1_randomize_etaphi_r04");
  // RandomCone* rcone_sub_avoid_lead_jet = findNode::getClass<RandomConev1>(topNode, "RandomCone_TOWER_SUB1_avoid_lead_jet_r04");

  // if(!rcone_basic){
  //   std::cout << "RandomConeTree::process_event - Error can not find basic random cone" << std::endl;
  //   exit(-1);
  // }
  // if(!rcone_randomize_etaphi){
  //   std::cout << "RandomConeTree::process_event - Error can not find randomize etaphi random cone" << std::endl;
  //   exit(-1);
  // }
  // if(!rcone_avoid_lead_jet){
  //   std::cout << "RandomConeTree::process_event - Error can not find avoid leading jet random cone" << std::endl;
  //   exit(-1);
  // }
  // if(!rcone_sub_basic){
  //   std::cout << "RandomConeTree::process_event - Error can not find basic random cone" << std::endl;
  //   exit(-1);
  // }
  // if(!rcone_sub_randomize_etaphi){
  //   std::cout << "RandomConeTree::process_event - Error can not find randomize etaphi random cone" << std::endl;
  //   exit(-1);
  // }
  // if(!rcone_sub_avoid_lead_jet){
  //   std::cout << "RandomConeTree::process_event - Error can not find avoid leading jet random cone" << std::endl;
  //   exit(-1);
  // }

  // m_cone_area = rcone_basic->get_area();

  // std::vector<Jet *> inputs;  // owns memory
  // for (auto &_input : _inputs)
  // {
  //   std::vector<Jet *> parts = _input->get_input(topNode);
  //   for (auto &part : parts)
  //   {
  //     inputs.push_back(part);
  //     inputs.back()->set_id(inputs.size() - 1);  // unique ids ensured
  //   }
  // }

  // std::vector<Jet *> iter_inputs;  // owns memory
  // for (auto &_iter_input : _iter_inputs)
  // {
  //   std::vector<Jet *> parts = _iter_input->get_input(topNode);
  //   for (auto &part : parts)
  //   {
  //     iter_inputs.push_back(part);
  //     iter_inputs.back()->set_id(iter_inputs.size() - 1);  // unique ids ensured
  //   }
  // }



  // // get pseudojets
  // std::vector<fastjet::PseudoJet> pseudojets = jets_to_pseudojets(inputs, false);
  // std::vector<fastjet::PseudoJet> iter_pseudojets = jets_to_pseudojets(iter_inputs, false);

  // // select random cone
  // m_cone_eta = rcone_basic->get_eta();
  // m_cone_phi = rcone_basic->get_phi();
  // m_cone_from_RC_pt = rcone_basic->get_pt();
  // m_cone_nclustered_from_RC = rcone_basic->n_clustered();

  // // control cone
  // m_cone_nclustered = 0;
  // m_cone_pt = 0;
  // for (unsigned int ipart = 0; ipart < pseudojets.size(); ++ipart)
  // {
  //   // calculate the distance between the particle and the cone center
  //   float deta = m_cone_eta - pseudojets[ipart].eta();
  //   float dphi = m_cone_phi - pseudojets[ipart].phi_std();
  //   if (dphi > M_PI) dphi -= 2 * M_PI;
  //   if (dphi < -M_PI) dphi += 2 * M_PI;
  //   float dr = sqrt(deta * deta + dphi * dphi);
  //   if (dr < rcone_basic->get_R())
  //   {
  //     m_cone_pt += pseudojets[ipart].pt();
  //     m_cone_nclustered++;
  //   }
  // }

  // // randomize cone
  // m_randomized_cone_eta = rcone_randomize_etaphi->get_eta();
  // m_randomized_cone_phi = rcone_randomize_etaphi->get_phi();
  // m_randomized_cone_from_RC_pt = rcone_randomize_etaphi->get_pt();
  // m_randomized_cone_nclustered_from_RC = rcone_randomize_etaphi->n_clustered();

  // // avoid leading jet cone
  // m_avoid_leading_cone_eta = rcone_avoid_lead_jet->get_eta();
  // m_avoid_leading_cone_phi = rcone_avoid_lead_jet->get_phi();
  // m_avoid_leading_cone_from_RC_pt = rcone_avoid_lead_jet->get_pt();
  // m_avoid_leading_cone_nclustered_from_RC = rcone_avoid_lead_jet->n_clustered();

  // // iter sub cone
  // m_cone_eta_iter = rcone_sub_basic->get_eta();
  // m_cone_phi_iter = rcone_sub_basic->get_phi();
  // m_cone_from_RC_pt_iter = rcone_sub_basic->get_pt();
  // m_cone_nclustered_from_RC_iter = rcone_sub_basic->n_clustered();


  // // iter sub cone
  // m_cone_pt_iter = 0;
  // for (unsigned int ipart = 0; ipart < iter_pseudojets.size(); ++ipart)
  // {
  //   // calculate the distance between the particle and the cone center
  //   float deta = m_cone_eta_iter - iter_pseudojets[ipart].eta();
  //   float dphi = m_cone_phi_iter - iter_pseudojets[ipart].phi_std();
  //   if (dphi > M_PI) dphi -= 2 * M_PI;
  //   if (dphi < -M_PI) dphi += 2 * M_PI;
  //   float dr = sqrt(deta * deta + dphi * dphi);
  //   if (dr < rcone_sub_basic->get_R())
  //   {
  //     m_cone_pt_iter += iter_pseudojets[ipart].pt();
  //     m_cone_nclustered_iter++;
  //   }

  // }

  // // randomize cone
  // m_randomized_cone_eta_iter = rcone_sub_randomize_etaphi->get_eta();
  // m_randomized_cone_phi_iter = rcone_sub_randomize_etaphi->get_phi();
  // m_randomized_cone_from_RC_pt_iter = rcone_sub_randomize_etaphi->get_pt();
  // m_randomized_cone_nclustered_from_RC_iter = rcone_sub_randomize_etaphi->n_clustered();

  // // avoid leading jet cone
  // m_avoid_leading_cone_eta_iter = rcone_sub_avoid_lead_jet->get_eta();
  // m_avoid_leading_cone_phi_iter = rcone_sub_avoid_lead_jet->get_phi();
  // m_avoid_leading_cone_from_RC_pt_iter = rcone_sub_avoid_lead_jet->get_pt();
  // m_avoid_leading_cone_nclustered_from_RC_iter = rcone_sub_avoid_lead_jet->n_clustered();


  // //==================================
  // // Fill tree
  // //==================================
  // m_tree->Fill();

  // return Fun4AllReturnCodes::EVENT_OK;
}

int RandomConeTree::End(PHCompositeNode *topNode)
{
  std::cout << "RandomConeTree::End - Output to " << m_outputfile_name << std::endl;
    // write tree to file
  PHTFileServer::get().cd(m_outputfile_name);
  // m_tree->Write();
  // write file to disk
  PHTFileServer::get().write(m_outputfile_name);
  std::cout << "RandomConeTree::End(PHCompositeNode *topNode) This is the End..." << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}


//===========================================================
// Event selection
bool RandomConeTree::GoodEvent(PHCompositeNode *topNode)
{ 
  // MC event selection based on leading R = 0.4 truth jet pT
  if(m_MC_do_event_selection)
  {
    // get truth jet nodes
    JetMap *truthjets = findNode::getClass<JetMap>(topNode, "AntiKt_Truth_r04");
    if(!truthjets) 
    {
      std::cout << "RandomConeTree::GoodEvent(PHCompositeNode *topNode) Could not find truth jet nodes" << std::endl;
      exit(-1); // this is a fatal error
    }

    // get leading truth jet pT
    float leading_truth_pt = -1;
    for(JetMap::Iter iter = truthjets->begin(); iter != truthjets->end(); ++iter)
    {

      Jet *jet = iter->second;
      
      if(jet->get_pt() > leading_truth_pt) leading_truth_pt = jet->get_pt();
    
    }

    // check if event passes selection
    if( (leading_truth_pt < m_MC_event_selection_jetpT_range.first) || 
        (leading_truth_pt > m_MC_event_selection_jetpT_range.second)
    )
    {

      if(Verbosity() > 0) std::cout << "RandomConeTree::GoodEvent(PHCompositeNode *topNode) Event failed MC event selection" << std::endl;
      return false;
    
    }
  }

  // zvtx cut 
  if(m_do_gvtx_cut)
  {
    // get global vertex map node
    GlobalVertexMap *vtxMap = findNode::getClass<GlobalVertexMapv1>(topNode,"GlobalVertexMap");
    if (!vtxMap)
    {
      std::cout << "RandomConeTree::GoodEvent(PHCompositeNode *topNode) Could not find global vertex map node" << std::endl;
      exit(-1); // this is a fatal error
    }

    if (!vtxMap->get(0))
    {
      if(Verbosity() > 0) std::cout << "RandomConeTree::GoodEvent(PHCompositeNode *topNode) No primary vertex" << std::endl;
      return false;
    }

    // get zvtx
    m_zvtx = vtxMap->get(0)->get_z();

    if( (m_zvtx < m_gvtx_cut.first) || (m_zvtx > m_gvtx_cut.second))
    {
      if(Verbosity() > 0) std::cout << "RandomConeTree::GoodEvent(PHCompositeNode *topNode) Vertex cut failed" << std::endl;
      return false;
    }   

  }

  return true;

}

//===========================================================
// Cent info  
void RandomConeTree::GetCentInfo(PHCompositeNode *topNode)
{
  //==================================
  // Get centrality info
  //==================================
  CentralityInfo* cent_node = findNode::getClass<CentralityInfo>(topNode, "CentralityInfo");
  if (!cent_node)
  {
      std::cout << "RandomConeTree::process_event() ERROR: Can't find CentralityInfo" << std::endl;
      exit(-1); // this is a fatal error
  }
  
  if (!m_do_data)
  {
      m_centrality = cent_node->get_centile(CentralityInfo::PROP::bimp);
      m_impactparam = cent_node->get_quantity(CentralityInfo::PROP::bimp);
      m_mbd_NS = cent_node->get_quantity(CentralityInfo::PROP::mbd_NS);
  }
  else
  {
      m_centrality = (int)(100*cent_node->get_centile(CentralityInfo::PROP::mbd_NS));

      PHNodeIterator iter(topNode);
      PHCompositeNode *mbdNode = dynamic_cast<PHCompositeNode *>(iter.findFirst("PHCompositeNode", "MBD"));
      if(!mbdNode)
	    {
	        std::cerr << Name() << "::" <<  __PRETTY_FUNCTION__ << "MBD Node missing, doing nothing." << std::endl;
	        throw std::runtime_error("Failed to find MBD node in RandomConeTree::GetCentInfo");
	    } 

      MbdOut *_data_MBD = findNode::getClass<MbdOut>(mbdNode, "MbdOut");
      
      if(!_data_MBD)
	    {
	      std::cerr << Name() << "::" <<  __PRETTY_FUNCTION__ << "MbdOut Node missing, doing nothing." << std::endl;
	      throw std::runtime_error("Failed to find MbdOut node in RandomConeTree::GetCentInfo");
	    }

      m_mbd_NS = _data_MBD->get_q(0) + _data_MBD->get_q(1);
    }
    
    return ;
}