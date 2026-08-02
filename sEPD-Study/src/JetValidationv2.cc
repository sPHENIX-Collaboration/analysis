#include "JetValidationv2.h"

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

// -- jet
#include <jetbase/JetContainer.h>

// -- jetbackground
#include <jetbackground/TowerBackground.h>

// Trigger
#include <calotrigger/TriggerAnalyzer.h>

// -- ROOT
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TProfile2D.h>
#include <TTree.h>

//____________________________________________________________________________..
JetValidationv2::JetValidationv2(const std::string &name)
  : SubsysReco(name)
{
}

//____________________________________________________________________________..
int JetValidationv2::Init([[maybe_unused]] PHCompositeNode *topNode)
{
  Fun4AllServer *se = Fun4AllServer::instance();
  se->Print("NODETREE");

  m_triggerAnalyzer = std::make_unique<TriggerAnalyzer>();

  hEvent = new TH1F("hEvent", "Event Type; Type; Events", static_cast<unsigned int>(m_eventType.size()), 0, static_cast<double>(m_eventType.size()));
  se->registerHisto(hEvent);

  hEventMinBias = new TH1F("hEventMinBias", "Event Type; Type; Events", static_cast<unsigned int>(m_MinBias_Type.size()), 0, static_cast<double>(m_MinBias_Type.size()));
  se->registerHisto(hEventMinBias);

  hVtxZ = new TH1F("hVtxZ", "Z Vertex; z [cm]; Events", m_hist_config.m_bins_zvtx, m_hist_config.m_zvtx_low, m_hist_config.m_zvtx_high);
  se->registerHisto(hVtxZ);

  hVtxZ_MB = new TH1F("hVtxZ_MB", "Z Vertex; z [cm]; Events", m_hist_config.m_bins_zvtx, m_hist_config.m_zvtx_low, m_hist_config.m_zvtx_high);
  se->registerHisto(hVtxZ_MB);

  hCentrality = new TH1F("hCentrality", "|z| < 10 cm and MB; Centrality [%]; Events", m_hist_config.m_bins_cent, m_hist_config.m_cent_low, m_hist_config.m_cent_high);
  se->registerHisto(hCentrality);

  for (unsigned int i = 0; i < m_eventType.size(); ++i)
  {
    hEvent->GetXaxis()->SetBinLabel(i + 1, m_eventType[i].c_str());
  }

  for (unsigned int i = 0; i < m_MinBias_Type.size(); ++i)
  {
    hEventMinBias->GetXaxis()->SetBinLabel(i + 1, m_MinBias_Type[i].c_str());
  }

  m_output = std::make_unique<TFile>(m_outtree_name.c_str(), "recreate");
  m_output->cd();

  // TTree
  m_tree = new TTree("T", "T");
  m_tree->SetDirectory(m_output.get());
  m_tree->Branch("event", &m_data.event);
  m_tree->Branch("zvtx", &m_data.zvtx);
  m_tree->Branch("centrality", &m_data.centrality);
  m_tree->Branch("emcal_energy", &m_data.emcal_energy);
  m_tree->Branch("ihcal_energy", &m_data.ihcal_energy);
  m_tree->Branch("ohcal_energy", &m_data.ohcal_energy);

  m_tree->Branch("max_pt_r02", &m_data.max_pt_r02);

  m_tree->Branch("pt_r02", &m_data.pt_r02);
  m_tree->Branch("pt_calib_r02", &m_data.pt_calib_r02);
  m_tree->Branch("e_r02", &m_data.e_r02);
  m_tree->Branch("phi_r02", &m_data.phi_r02);
  m_tree->Branch("eta_r02", &m_data.eta_r02);

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int JetValidationv2::process_event_check(PHCompositeNode *topNode)
{
  hEvent->Fill(static_cast<std::uint8_t>(EventType::ALL));

  // zvertex
  double zvtx = -9999;
  GlobalVertexMap *vertexmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");

  if (!vertexmap)
  {
    return Fun4AllReturnCodes::ABORTRUN;
  }

  if (!vertexmap->empty())
  {
    GlobalVertex *vtx = vertexmap->begin()->second;
    m_data.zvtx = vtx->get_z();
    zvtx = m_data.zvtx;

    hEvent->Fill(static_cast<std::uint8_t>(EventType::ZVTX));
  }

  hVtxZ->Fill(zvtx);

  bool pass_zvtx10 = std::abs(zvtx) < m_cuts.m_zvtx_max;

  if (std::abs(zvtx) < m_cuts.m_zvtx_max_v2)
  {
    hEvent->Fill(static_cast<std::uint8_t>(EventType::ZVTX50));
    if (pass_zvtx10)
    {
      hEvent->Fill(static_cast<std::uint8_t>(EventType::ZVTX10));
    }
  }

  // MBD Trigger
  m_triggerAnalyzer->decodeTriggers(topNode);

  bool didTrig14Fire = m_triggerAnalyzer->didTriggerFire(m_trig_14);
  bool didTrig12Fire = m_triggerAnalyzer->didTriggerFire(m_trig_12);

  bool mbd_trigger_fire = didTrig12Fire || didTrig14Fire;

  if (pass_zvtx10 && mbd_trigger_fire)
  {
    hEvent->Fill(static_cast<std::uint8_t>(EventType::MB_TRIG));
  }

  // Minimum Bias Classifier
  MinimumBiasInfo *m_mb_info = findNode::getClass<MinimumBiasInfo>(topNode, "MinimumBiasInfo");
  if (!m_mb_info)
  {
    return Fun4AllReturnCodes::ABORTRUN;
  }

  // Minimum Bias Check
  PdbParameterMap *pdb = findNode::getClass<PdbParameterMap>(topNode, "MinBiasParams");
  if (!pdb)
  {
    return Fun4AllReturnCodes::ABORTRUN;
  }

  PHParameters pdb_params("MinBiasParams");
  pdb_params.FillFrom(pdb);

  bool minbias_bkg_high = pdb_params.get_int_param("minbias_background_cut_fail");
  bool minbias_side_hit_low = pdb_params.get_int_param("minbias_two_hit_min_fail");
  bool minbias_zdc_low = pdb_params.get_int_param("minbias_zdc_energy_min_fail");
  bool minbias_mbd_high = pdb_params.get_int_param("minbias_mbd_total_energy_max_fail");

  if (pass_zvtx10 && mbd_trigger_fire)
  {
    if (minbias_bkg_high)
    {
      hEventMinBias->Fill(static_cast<std::uint8_t>(MinBiasType::BKG_HIGH));
    }
    if (minbias_side_hit_low)
    {
      hEventMinBias->Fill(static_cast<std::uint8_t>(MinBiasType::SIDE_HIT_LOW));
    }
    if (minbias_zdc_low)
    {
      hEventMinBias->Fill(static_cast<std::uint8_t>(MinBiasType::ZDC_LOW));
    }
    if (minbias_mbd_high)
    {
      hEventMinBias->Fill(static_cast<std::uint8_t>(MinBiasType::MBD_HIGH));
    }
  }

  // skip event if not fire MBD Trigger
  if (!mbd_trigger_fire)
  {
    ++m_ctr["process_eventCheck_mbd_trigger_fail"];
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  // skip event if not minimum bias
  if (!m_mb_info->isAuAuMinimumBias())
  {
    ++m_ctr["process_eventCheck_isAuAuMinBias_fail"];
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  hVtxZ_MB->Fill(zvtx);

  // skip event if zvtx is too large
  if (!pass_zvtx10)
  {
    ++m_ctr["process_eventCheck_zvtx_large"];
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  hEvent->Fill(static_cast<std::uint8_t>(EventType::MB));

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int JetValidationv2::process_centrality(PHCompositeNode *topNode)
{
  CentralityInfo *centInfo = findNode::getClass<CentralityInfo>(topNode, "CentralityInfo");
  if (!centInfo)
  {
    return Fun4AllReturnCodes::ABORTRUN;
  }

  m_data.centrality = centInfo->get_centile(CentralityInfo::PROP::mbd_NS) * 100;
  double cent = m_data.centrality;

  // skip event if centrality is too peripheral
  if (!std::isfinite(cent) || cent >= m_cuts.m_cent_max)
  {
    ++m_ctr["process_eventCheck_centrality_large"];
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  hCentrality->Fill(cent);

  hEvent->Fill(static_cast<std::uint8_t>(EventType::CENT));

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int JetValidationv2::process_Calo(PHCompositeNode *topNode)
{
  auto* towersCEMC  = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_CEMC_RETOWER");
  auto* towersIHCal = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALIN");
  auto* towersOHCal = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALOUT");

  if (!towersCEMC || !towersIHCal || !towersOHCal)
  {
    return Fun4AllReturnCodes::ABORTRUN;
  }

  size_t nTowersCEMC = towersCEMC->size();
  size_t nTowersIHCal = towersIHCal->size();
  size_t nTowersOHCal = towersOHCal->size();

  if(nTowersCEMC != nTowersIHCal || nTowersCEMC != nTowersOHCal)
  {
    std::cout << std::format("Calo Contains Missing Towers!, Event: {}\n", m_data.event);
    return Fun4AllReturnCodes::ABORTRUN;
  }

  for (unsigned int towerIndex = 0; towerIndex < towersCEMC->size(); ++towerIndex)
  {
    auto* towerCEMC = towersCEMC->get_tower_at_channel(towerIndex);
    if(towerCEMC->get_isGood())
    {
      float energy = towerCEMC->get_energy();
      m_data.emcal_energy += energy;
    }

    auto* towerIHCal = towersIHCal->get_tower_at_channel(towerIndex);
    if(towerIHCal->get_isGood())
    {
      float energy = towerIHCal->get_energy();
      m_data.ihcal_energy += energy;
    }

    auto* towerOHCal = towersOHCal->get_tower_at_channel(towerIndex);
    if(towerOHCal->get_isGood())
    {
      float energy = towerOHCal->get_energy();
      m_data.ohcal_energy += energy;
    }
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int JetValidationv2::process_jets(PHCompositeNode *topNode)
{
  JetContainer *jets_r02 = findNode::getClass<JetContainer>(topNode, m_recoJetName_r02);

  JetContainer *jets_r02_calib = findNode::getClass<JetContainer>(topNode, m_recoJetName_calib_r02);

  if (!jets_r02 || !jets_r02_calib)
  {
    return Fun4AllReturnCodes::ABORTRUN;
  }

  bool hasJet = false;

  // R = 0.2
  for (unsigned int i = 0; i < jets_r02->size(); ++i)
  {
    auto *jet = jets_r02->get_jet(i);
    auto *calib_jet = jets_r02_calib->get_jet(i);

    double pt = jet->get_pt();
    double pt_calib = calib_jet->get_pt();
    double energy = jet->get_e();
    double phi = jet->get_phi();
    double eta = jet->get_eta();

    // map [-pi,pi] -> [0,2pi]
    if (phi < 0)
    {
      phi += 2.0 * std::numbers::pi;
    }

    if (pt_calib >= m_jet_pt_min_cut && std::abs(eta) < m_jet_eta_max_cut_r02)
    {
      m_data.pt_r02.push_back(pt);
      m_data.pt_calib_r02.push_back(pt_calib);
      m_data.e_r02.push_back(energy);
      m_data.phi_r02.push_back(phi);
      m_data.eta_r02.push_back(eta);

      if (energy > 0)
      {
        m_data.max_pt_r02 = std::max(m_data.max_pt_r02, pt_calib);
        hasJet = true;
      }
    }
  }

  if (hasJet)
  {
    hEvent->Fill(static_cast<std::uint8_t>(EventType::JET));
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int JetValidationv2::process_event(PHCompositeNode *topNode)
{
  EventHeader *eventInfo = findNode::getClass<EventHeader>(topNode, "EventHeader");
  if (!eventInfo)
  {
    return Fun4AllReturnCodes::ABORTRUN;
  }

  int ret = process_event_check(topNode);
  if (ret)
  {
    return ret;
  }

  ret = process_centrality(topNode);
  if (ret)
  {
    return ret;
  }

  ret = process_Calo(topNode);
  if (ret)
  {
    return ret;
  }

  ret = process_jets(topNode);
  if (ret)
  {
    return ret;
  }

  // Fill the TTree
  m_tree->Fill();

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int JetValidationv2::ResetEvent([[maybe_unused]] PHCompositeNode *topNode)
{
  ++m_ctr["event_reset"];

  // Event
  m_data.event = 0;
  m_data.zvtx = 9999;
  m_data.centrality = 9999;

  // Calo
  m_data.emcal_energy = 0;
  m_data.ihcal_energy = 0;
  m_data.ohcal_energy = 0;

  // Jets
  m_data.max_pt_r02 = 0;

  m_data.pt_r02.clear();
  m_data.pt_calib_r02.clear();
  m_data.e_r02.clear();
  m_data.phi_r02.clear();
  m_data.eta_r02.clear();

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int JetValidationv2::End([[maybe_unused]] PHCompositeNode *topNode)
{
  std::cout << "JetValidationv2::End" << std::endl;

  std::cout << std::format("{:#<20}\n", "");
  std::cout << "stats" << std::endl;

  std::cout << std::format("{:#<20}\n", "");
  std::cout << "Abort Events Types" << std::endl;
  std::cout << std::format("process event, Reset Event Calls : {}", m_ctr["event_reset"]) << std::endl;
  std::cout << std::format("process event, MBD Trigger Fail: {}", m_ctr["process_eventCheck_mbd_trigger_fail"]) << std::endl;
  std::cout << std::format("process event, isAuAuMinBias Fail: {}", m_ctr["process_eventCheck_isAuAuMinBias_fail"]) << std::endl;
  std::cout << std::format("process event, |z| >= {} cm: {}", m_cuts.m_zvtx_max, m_ctr["process_eventCheck_zvtx_large"]) << std::endl;
  std::cout << std::format("process event, Centrality >= {}%: {}", m_cuts.m_cent_max, m_ctr["process_eventCheck_centrality_large"]) << std::endl;

  std::cout << std::format("{:#<20}\n", "");
  std::cout << "Events" << std::endl;
  for (unsigned int i = 0; i < m_eventType.size(); ++i)
  {
    std::cout << m_eventType[i] << ": " << hEvent->GetBinContent(i + 1) << std::endl;
  }
  std::cout << std::format("{:#<20}\n", "");

  // TTree
  m_output->cd();
  m_tree->Write();
  m_output->Close();

  return Fun4AllReturnCodes::EVENT_OK;
}
