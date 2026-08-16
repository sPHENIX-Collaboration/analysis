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

// -- flags
#include <pdbcalbase/PdbParameterMap.h>
#include <phparameter/PHParameters.h>

// -- Calo
#include <calobase/TowerInfo.h>
#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfoDefs.h>

// -- Vtx
#include <globalvertex/GlobalVertex.h>
#include <globalvertex/GlobalVertexMap.h>

// -- MB
#include <calotrigger/MinimumBiasClassifier.h>
#include <calotrigger/MinimumBiasInfo.h>
#include <centrality/CentralityInfo.h>

// -- Event Plane
#include <eventplaneinfo/EventplaneinfoMap.h>
#include <eventplaneinfo/Eventplaneinfo.h>

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

  m_tree = TreeFiller::getTree();
  if (!m_tree)
  {
    std::cout << "JetValidationv3: Failed to get/create TTree" << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  m_tree->Branch("emcal_energy", &m_data.emcal_energy);
  m_tree->Branch("ihcal_energy", &m_data.ihcal_energy);
  m_tree->Branch("ohcal_energy", &m_data.ohcal_energy);

  m_tree->Branch("psi2_raw_S", &m_data.psi2_raw_S);
  m_tree->Branch("psi2_raw_N", &m_data.psi2_raw_N);
  m_tree->Branch("psi2_raw_NS", &m_data.psi2_raw_NS);

  m_tree->Branch("psi2_S", &m_data.psi2_S);
  m_tree->Branch("psi2_N", &m_data.psi2_N);
  m_tree->Branch("psi2_NS", &m_data.psi2_NS);

  m_tree->Branch("seeds_iter", &m_data.seeds_iter);
  m_tree->Branch("seeds_mult", &m_data.seeds_mult);

  m_tree->Branch("calo_v2_iter", &m_data.calo_v2_iter);
  m_tree->Branch("calo_v2_mult", &m_data.calo_v2_mult);

  m_tree->Branch("is_flow_failure_iter", &m_data.is_flow_failure_iter);
  m_tree->Branch("is_flow_failure_mult", &m_data.is_flow_failure_mult);

  m_tree->Branch("max_pt_iter_r02", &m_data.iter_r02.max_pt);
  m_tree->Branch("max_pt_iter_r03", &m_data.iter_r03.max_pt);

  m_tree->Branch("max_pt_mult_r02", &m_data.mult_r02.max_pt);
  m_tree->Branch("max_pt_mult_r03", &m_data.mult_r03.max_pt);
  m_tree->Branch("max_pt_unsub_r02", &m_data.unsub_r02.max_pt);
  m_tree->Branch("max_pt_unsub_r03", &m_data.unsub_r03.max_pt);

  m_tree->Branch("pt_iter_r02", &m_data.iter_r02.pt);
  m_tree->Branch("pt_calib_iter_r02", &m_data.iter_r02.pt_calib);
  m_tree->Branch("e_iter_r02", &m_data.iter_r02.e);
  m_tree->Branch("phi_iter_r02", &m_data.iter_r02.phi);
  m_tree->Branch("eta_iter_r02", &m_data.iter_r02.eta);

  m_tree->Branch("pt_mult_r02", &m_data.mult_r02.pt);
  m_tree->Branch("pt_calib_mult_r02", &m_data.mult_r02.pt_calib);
  m_tree->Branch("e_mult_r02", &m_data.mult_r02.e);
  m_tree->Branch("phi_mult_r02", &m_data.mult_r02.phi);
  m_tree->Branch("eta_mult_r02", &m_data.mult_r02.eta);

  m_tree->Branch("pt_iter_r03", &m_data.iter_r03.pt);
  m_tree->Branch("pt_calib_iter_r03", &m_data.iter_r03.pt_calib);
  m_tree->Branch("e_iter_r03", &m_data.iter_r03.e);
  m_tree->Branch("phi_iter_r03", &m_data.iter_r03.phi);
  m_tree->Branch("eta_iter_r03", &m_data.iter_r03.eta);

  m_tree->Branch("pt_mult_r03", &m_data.mult_r03.pt);
  m_tree->Branch("pt_calib_mult_r03", &m_data.mult_r03.pt_calib);
  m_tree->Branch("e_mult_r03", &m_data.mult_r03.e);
  m_tree->Branch("phi_mult_r03", &m_data.mult_r03.phi);
  m_tree->Branch("eta_mult_r03", &m_data.mult_r03.eta);

  m_tree->Branch("pt_unsub_r02", &m_data.unsub_r02.pt);
  m_tree->Branch("pt_calib_unsub_r02", &m_data.unsub_r02.pt_calib);
  m_tree->Branch("e_unsub_r02", &m_data.unsub_r02.e);
  m_tree->Branch("phi_unsub_r02", &m_data.unsub_r02.phi);
  m_tree->Branch("eta_unsub_r02", &m_data.unsub_r02.eta);

  m_tree->Branch("pt_unsub_r03", &m_data.unsub_r03.pt);
  m_tree->Branch("pt_calib_unsub_r03", &m_data.unsub_r03.pt_calib);
  m_tree->Branch("e_unsub_r03", &m_data.unsub_r03.e);
  m_tree->Branch("phi_unsub_r03", &m_data.unsub_r03.phi);
  m_tree->Branch("eta_unsub_r03", &m_data.unsub_r03.eta);

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int JetValidationv3::process_Calo(PHCompositeNode *topNode)
{
  auto* towersCEMC  = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_CEMC_RETOWER");
  auto* towersIHCal = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALIN");
  auto* towersOHCal = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALOUT");

  if (!towersCEMC || !towersIHCal || !towersOHCal)
  {
    std::cout << "Aborting Run: Calo Towers null" << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  size_t nTowersCEMC = towersCEMC->size();
  size_t nTowersIHCal = towersIHCal->size();
  size_t nTowersOHCal = towersOHCal->size();

  if(nTowersCEMC != nTowersIHCal || nTowersCEMC != nTowersOHCal)
  {
    std::cout << "Calo Contains Missing Towers!" << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  for (unsigned int towerIndex = 0; towerIndex < towersCEMC->size(); ++towerIndex)
  {
    auto* towerCEMC = towersCEMC->get_tower_at_channel(towerIndex);
    if(towerCEMC && towerCEMC->get_isGood())
    {
      float energy = towerCEMC->get_energy();
      m_data.emcal_energy += energy;
    }

    auto* towerIHCal = towersIHCal->get_tower_at_channel(towerIndex);
    if(towerIHCal && towerIHCal->get_isGood())
    {
      float energy = towerIHCal->get_energy();
      m_data.ihcal_energy += energy;
    }

    auto* towerOHCal = towersOHCal->get_tower_at_channel(towerIndex);
    if(towerOHCal && towerOHCal->get_isGood())
    {
      float energy = towerOHCal->get_energy();
      m_data.ohcal_energy += energy;
    }
  }

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
int JetValidationv3::process_EventPlane(PHCompositeNode *topNode)
{
  if (!m_do_flow)
  {
    return Fun4AllReturnCodes::EVENT_OK;
  }

  // get event plane map
  EventplaneinfoMap *epmap = findNode::getClass<EventplaneinfoMap>(topNode, "EventplaneinfoMap");
  if (!epmap || epmap->empty())
  {
    std::cout << "Aborting Run: Event Plane Map null or empty" << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  Eventplaneinfo *epd_S = epmap->get(EventplaneinfoMap::sEPDS);
  Eventplaneinfo *epd_N = epmap->get(EventplaneinfoMap::sEPDN);
  Eventplaneinfo *epd_NS = epmap->get(EventplaneinfoMap::sEPDNS);

  // ensure the ptrs are valid
  if (!epd_S || !epd_N || !epd_NS)
  {
    std::cout << "Aborting Run: Event Plane map pointers invalid" << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  std::pair<double, double> Q_S_2_raw = epd_S->get_qvector_raw(2);
  std::pair<double, double> Q_N_2_raw = epd_N->get_qvector_raw(2);
  std::pair<double, double> Q_NS_2_raw = epd_NS->get_qvector_raw(2);

  std::pair<double, double> Q_S_2 = epd_S->get_qvector(2);
  std::pair<double, double> Q_N_2 = epd_N->get_qvector(2);
  std::pair<double, double> Q_NS_2 = epd_NS->get_qvector(2);

  double _2psi2_raw_S = 2*epd_S->GetPsi(Q_S_2_raw.first, Q_S_2_raw.second, 2);
  double _2psi2_raw_N = 2*epd_N->GetPsi(Q_N_2_raw.first, Q_N_2_raw.second, 2);
  double _2psi2_raw_NS = 2*epd_NS->GetPsi(Q_NS_2_raw.first, Q_NS_2_raw.second, 2);

  double _2psi2_S = 2*epd_S->GetPsi(Q_S_2.first, Q_S_2.second, 2);
  double _2psi2_N = 2*epd_N->GetPsi(Q_N_2.first, Q_N_2.second, 2);
  double _2psi2_NS = 2*epd_NS->GetPsi(Q_NS_2.first, Q_NS_2.second, 2);

  m_data.psi2_raw_S = _2psi2_raw_S;
  m_data.psi2_raw_N = _2psi2_raw_N;
  m_data.psi2_raw_NS = _2psi2_raw_NS;

  m_data.psi2_S = _2psi2_S;
  m_data.psi2_N = _2psi2_N;
  m_data.psi2_NS = _2psi2_NS;

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
  int ret = process_Calo(topNode);
  if (ret)
  {
    return ret;
  }

  ret = process_UE(topNode);
  if (ret)
  {
    return ret;
  }

  ret = process_EventPlane(topNode);
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
  // Calo
  m_data.emcal_energy = 0;
  m_data.ihcal_energy = 0;
  m_data.ohcal_energy = 0;

  // UE
  m_data.seeds_iter = 0;
  m_data.seeds_mult = 0;

  m_data.calo_v2_iter = 9999;
  m_data.calo_v2_mult = 9999;

  m_data.is_flow_failure_iter = false;
  m_data.is_flow_failure_mult = false;

  // sEPD
  m_data.psi2_raw_S = 0;
  m_data.psi2_raw_N = 0;
  m_data.psi2_raw_NS = 0;

  m_data.psi2_S = 0;
  m_data.psi2_N = 0;
  m_data.psi2_NS = 0;

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
