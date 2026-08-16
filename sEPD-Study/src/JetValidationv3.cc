#include "JetValidationv3.h"

// -- c++
#include <format>
#include <iostream>

// -- event
#include <ffaobjects/EventHeader.h>

// -- Fun4All
#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/Fun4AllServer.h>

// -- Nodes
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

// -- jet
#include <jetbase/JetContainer.h>

// -- jetbackground
#include <jetbackground/TowerBackground.h>

#include <treefiller/TreeFiller.h>

// -- ROOT
#include <TH1.h>
#include <TH2.h>
#include <TProfile2D.h>
#include <TTree.h>

//____________________________________________________________________________..
JetValidationv3::JetValidationv3(const std::string &name)
  : SubsysReco(name)
{
}

//____________________________________________________________________________..
int JetValidationv3::Init([[maybe_unused]] PHCompositeNode *topNode)
{
  Fun4AllServer *se = Fun4AllServer::instance();
  se->Print("NODETREE");

  TTree* tree = TreeFiller::getTree();
  if (!tree)
  {
    std::cout << "JetValidationv3: Failed to get/create TTree" << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  tree->Branch("seeds_iter", &m_data.seeds_iter);
  tree->Branch("seeds_mult", &m_data.seeds_mult);

  tree->Branch("calo_v2_iter", &m_data.calo_v2_iter);
  tree->Branch("calo_v2_mult", &m_data.calo_v2_mult);

  tree->Branch("is_flow_failure_iter", &m_data.is_flow_failure_iter);
  tree->Branch("is_flow_failure_mult", &m_data.is_flow_failure_mult);

  tree->Branch("max_pt_iter_r02", &m_data.iter_r02.max_pt);
  tree->Branch("max_pt_iter_r03", &m_data.iter_r03.max_pt);

  tree->Branch("max_pt_mult_r02", &m_data.mult_r02.max_pt);
  tree->Branch("max_pt_mult_r03", &m_data.mult_r03.max_pt);
  tree->Branch("max_pt_unsub_r02", &m_data.unsub_r02.max_pt);
  tree->Branch("max_pt_unsub_r03", &m_data.unsub_r03.max_pt);

  tree->Branch("pt_iter_r02", &m_data.iter_r02.pt);
  tree->Branch("pt_calib_iter_r02", &m_data.iter_r02.pt_calib);
  tree->Branch("e_iter_r02", &m_data.iter_r02.e);
  tree->Branch("phi_iter_r02", &m_data.iter_r02.phi);
  tree->Branch("eta_iter_r02", &m_data.iter_r02.eta);

  tree->Branch("pt_mult_r02", &m_data.mult_r02.pt);
  tree->Branch("pt_calib_mult_r02", &m_data.mult_r02.pt_calib);
  tree->Branch("e_mult_r02", &m_data.mult_r02.e);
  tree->Branch("phi_mult_r02", &m_data.mult_r02.phi);
  tree->Branch("eta_mult_r02", &m_data.mult_r02.eta);

  tree->Branch("pt_iter_r03", &m_data.iter_r03.pt);
  tree->Branch("pt_calib_iter_r03", &m_data.iter_r03.pt_calib);
  tree->Branch("e_iter_r03", &m_data.iter_r03.e);
  tree->Branch("phi_iter_r03", &m_data.iter_r03.phi);
  tree->Branch("eta_iter_r03", &m_data.iter_r03.eta);

  tree->Branch("pt_mult_r03", &m_data.mult_r03.pt);
  tree->Branch("pt_calib_mult_r03", &m_data.mult_r03.pt_calib);
  tree->Branch("e_mult_r03", &m_data.mult_r03.e);
  tree->Branch("phi_mult_r03", &m_data.mult_r03.phi);
  tree->Branch("eta_mult_r03", &m_data.mult_r03.eta);

  tree->Branch("pt_unsub_r02", &m_data.unsub_r02.pt);
  tree->Branch("pt_calib_unsub_r02", &m_data.unsub_r02.pt_calib);
  tree->Branch("e_unsub_r02", &m_data.unsub_r02.e);
  tree->Branch("phi_unsub_r02", &m_data.unsub_r02.phi);
  tree->Branch("eta_unsub_r02", &m_data.unsub_r02.eta);

  tree->Branch("pt_unsub_r03", &m_data.unsub_r03.pt);
  tree->Branch("pt_calib_unsub_r03", &m_data.unsub_r03.pt_calib);
  tree->Branch("e_unsub_r03", &m_data.unsub_r03.e);
  tree->Branch("phi_unsub_r03", &m_data.unsub_r03.phi);
  tree->Branch("eta_unsub_r03", &m_data.unsub_r03.eta);

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int JetValidationv3::process_UE(PHCompositeNode *topNode)
{
  TowerBackground* towerBkg_iter = findNode::getClass<TowerBackground>(topNode, "TowerInfoBackground_Sub2"); 
  TowerBackground* towerBkg_mult = findNode::getClass<TowerBackground>(topNode, "TowerInfoBackground_MultSub2"); 

  if (!towerBkg_iter || !towerBkg_mult)
  {
    std::cout << "Aborting Run: Background Info null" << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  m_data.is_flow_failure_iter = towerBkg_iter->get_flow_failure_flag();
  m_data.is_flow_failure_mult = towerBkg_mult->get_flow_failure_flag();

  m_data.calo_v2_iter = towerBkg_iter->get_v2();
  m_data.calo_v2_mult = towerBkg_mult->get_v2();

  m_data.seeds_iter = towerBkg_iter->get_nHIRecoSeedsSub();
  m_data.seeds_mult = towerBkg_mult->get_nHIRecoSeedsSub();

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int JetValidationv3::process_jets(PHCompositeNode *topNode)
{
  JetContainer *jets_iter_r02 = findNode::getClass<JetContainer>(topNode, m_recoJetName_iter_r02);
  JetContainer *jets_iter_calib_r02 = findNode::getClass<JetContainer>(topNode, m_recoJetName_iter_calib_r02);

  JetContainer *jets_mult_r02 = findNode::getClass<JetContainer>(topNode, m_recoJetName_mult_r02);
  JetContainer *jets_mult_calib_r02 = findNode::getClass<JetContainer>(topNode, m_recoJetName_mult_calib_r02);

  JetContainer *jets_iter_r03 = findNode::getClass<JetContainer>(topNode, m_recoJetName_iter_r03);
  JetContainer *jets_iter_calib_r03 = findNode::getClass<JetContainer>(topNode, m_recoJetName_iter_calib_r03);

  JetContainer *jets_mult_r03 = findNode::getClass<JetContainer>(topNode, m_recoJetName_mult_r03);
  JetContainer *jets_mult_calib_r03 = findNode::getClass<JetContainer>(topNode, m_recoJetName_mult_calib_r03);

  JetContainer *jets_unsub_r02 = findNode::getClass<JetContainer>(topNode, m_recoJetName_unsub_r02);
  JetContainer *jets_unsub_calib_r02 = findNode::getClass<JetContainer>(topNode, m_recoJetName_unsub_calib_r02);

  JetContainer *jets_unsub_r03 = findNode::getClass<JetContainer>(topNode, m_recoJetName_unsub_r03);
  JetContainer *jets_unsub_calib_r03 = findNode::getClass<JetContainer>(topNode, m_recoJetName_unsub_calib_r03);

  for(auto* jets : {jets_iter_r02, jets_iter_r03, jets_iter_calib_r02, jets_iter_calib_r03,
                    jets_mult_r02, jets_mult_r03, jets_mult_calib_r02, jets_mult_calib_r03,
                    jets_unsub_r02, jets_unsub_r03, jets_unsub_calib_r02, jets_unsub_calib_r03})
  {
    if (!jets)
    {
      std::cout << "Aborting Run: Jets Info null" << std::endl;
      return Fun4AllReturnCodes::ABORTRUN;
    }
  }

  auto fill_jets = [&](JetContainer* jets, JetContainer* jets_calib, JetData& jd, double eta_max_cut) 
  {
    for (unsigned int i = 0; i < std::min(jets->size(), jets_calib->size()); ++i)
    {
      auto *jet = jets->get_jet(i);
      auto *jet_calib = jets_calib->get_jet(i);

      if (!jet || !jet_calib)
      {
        continue;
      }

      double pt = jet->get_pt();
      double pt_calib = jet_calib->get_pt();
      double energy = jet->get_e();
      double phi = jet->get_phi();
      double eta = jet->get_eta();

      // map [-pi,pi] -> [0,2pi]
      if (phi < 0)
      {
        phi += 2.0 * std::numbers::pi;
      }

      if (pt_calib >= m_jet_pt_min_cut && std::abs(eta) < eta_max_cut)
      {
        jd.pt.push_back(pt);
        jd.pt_calib.push_back(pt_calib);
        jd.e.push_back(energy);
        jd.phi.push_back(phi);
        jd.eta.push_back(eta);

        if (energy > 0)
        {
          jd.max_pt = std::max(jd.max_pt, pt_calib);
        }
      }
    }
  };

  // R = 0.2 Iterative
  fill_jets(jets_iter_r02, jets_iter_calib_r02, m_data.iter_r02, m_jet_eta_max_cut_r02);

  // R = 0.3 Iterative
  fill_jets(jets_iter_r03, jets_iter_calib_r03, m_data.iter_r03, m_jet_eta_max_cut_r03);

  // R = 0.2 Multiplicity
  fill_jets(jets_mult_r02, jets_mult_calib_r02, m_data.mult_r02, m_jet_eta_max_cut_r02);

  // R = 0.3 Multiplicity
  fill_jets(jets_mult_r03, jets_mult_calib_r03, m_data.mult_r03, m_jet_eta_max_cut_r03);

  // R = 0.2 Unsubtracted
  fill_jets(jets_unsub_r02, jets_unsub_calib_r02, m_data.unsub_r02, m_jet_eta_max_cut_r02);

  // R = 0.3 Unsubtracted
  fill_jets(jets_unsub_r03, jets_unsub_calib_r03, m_data.unsub_r03, m_jet_eta_max_cut_r03);

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int JetValidationv3::process_event(PHCompositeNode *topNode)
{
  int ret = process_UE(topNode);
  if (ret)
  {
    return ret;
  }

  ret = process_jets(topNode);
  if (ret)
  {
    return ret;
  }

  // TTree filling is managed by TreeFiller

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int JetValidationv3::ResetEvent([[maybe_unused]] PHCompositeNode *topNode)
{
  // UE
  m_data.seeds_iter = 0;
  m_data.seeds_mult = 0;

  m_data.calo_v2_iter = 9999;
  m_data.calo_v2_mult = 9999;

  m_data.is_flow_failure_iter = false;
  m_data.is_flow_failure_mult = false;

  // Jets
  m_data.iter_r02.clear();
  m_data.iter_r03.clear();
  m_data.mult_r02.clear();
  m_data.mult_r03.clear();
  m_data.unsub_r02.clear();
  m_data.unsub_r03.clear();

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int JetValidationv3::End([[maybe_unused]] PHCompositeNode *topNode)
{
  std::cout << "JetValidationv3::End" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}
