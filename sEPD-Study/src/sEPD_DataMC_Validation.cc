#include "sEPD_DataMC_Validation.h"

// -- c++
#include <format>
#include <iostream>
#include <algorithm>

// -- event
#include <ffaobjects/EventHeader.h>

// -- Fun4All
#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/Fun4AllServer.h>

// -- Nodes
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

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

//____________________________________________________________________________..
sEPD_DataMC_Validation::sEPD_DataMC_Validation(const std::string &name)
  : SubsysReco(name)
{
}

//____________________________________________________________________________..
int sEPD_DataMC_Validation::Init([[maybe_unused]] PHCompositeNode *topNode)
{
  Fun4AllServer *se = Fun4AllServer::instance();
  se->Print("NODETREE");

  m_hists["hCentrality"] = std::make_unique<TH1F>("hCentrality", "Min Bias; Centrality [%]; Events", m_bins_cent, m_cent_low, m_cent_high);

  m_hists["hSEPD_Charge_Data"] = std::make_unique<TH1F>("hSEPD_Charge_Data", "Min Bias [Data]; sEPD Charge; Towers", m_bins_charge, m_charge_low, m_charge_high);
  m_hists["hSEPD_Charge_MC"] = std::make_unique<TH1F>("hSEPD_Charge_MC", "Min Bias [MC]; sEPD Charge; Towers", m_bins_charge, m_charge_low, m_charge_high);

  std::string title_data = "Min Bias [Data]; sEPD Charge; Events";
  std::string title_mc = "Min Bias [MC]; sEPD Charge; Events";

  for (int channel = 0; channel < m_sepd_channels; ++channel)
  {
    std::string name_data = std::format("hSEPD_data_channel_{}", channel);
    std::string name_mc = std::format("hSEPD_mc_channel_{}", channel);

    m_hists[name_data] = std::make_unique<TH1F>(name_data.c_str(), title_data.c_str(), m_bins_charge, m_charge_low, m_charge_high);
    m_hists[name_mc] = std::make_unique<TH1F>(name_mc.c_str(), title_mc.c_str(), m_bins_charge, m_charge_low, m_charge_high);
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

// //____________________________________________________________________________..
int sEPD_DataMC_Validation::process_event_check(PHCompositeNode *topNode)
{
  MinimumBiasInfo *mb_info = findNode::getClass<MinimumBiasInfo>(topNode, "MinimumBiasInfo");
  if (!mb_info)
  {
    return Fun4AllReturnCodes::ABORTRUN;
  }
  // skip event if not minimum bias
  if (!mb_info->isAuAuMinimumBias())
  {
    ++m_ctr["events_isAuAuMinBias_fail"];
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int sEPD_DataMC_Validation::process_centrality(PHCompositeNode *topNode)
{
  CentralityInfo *centInfo = findNode::getClass<CentralityInfo>(topNode, "CentralityInfo");
  if (!centInfo)
  {
    return Fun4AllReturnCodes::ABORTRUN;
  }

  m_cent = centInfo->get_centile(CentralityInfo::PROP::mbd_NS) * 100;

  if (!std::isfinite(m_cent))
  {
    ++m_ctr["events_centrality_nan"];
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  m_hists["hCentrality"]->Fill(m_cent);

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int sEPD_DataMC_Validation::process_sEPD(PHCompositeNode *topNode)
{
  TowerInfoContainer *towerinfosEPD_data = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_SEPD_data");
  if (!towerinfosEPD_data)
  {
    return Fun4AllReturnCodes::ABORTRUN;
  }

  TowerInfoContainer *towerinfosEPD_mc = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_EPD");
  if (!towerinfosEPD_mc)
  {
    return Fun4AllReturnCodes::ABORTRUN;
  }

  TowerInfoContainer *towerinfosEPD_data_mc = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_COMBINED_SEPD");
  if (!towerinfosEPD_data_mc)
  {
    return Fun4AllReturnCodes::ABORTRUN;
  }

  unsigned int nchannels_epd_data = towerinfosEPD_data->size();
  unsigned int nchannels_epd_mc = towerinfosEPD_mc->size();
  unsigned int nchannels_epd_data_mc = towerinfosEPD_data_mc->size();

  if(nchannels_epd_data != nchannels_epd_mc || nchannels_epd_data != nchannels_epd_data_mc)
  {
     ++m_ctr["events_sepd_channel_fail"];
     return Fun4AllReturnCodes::ABORTEVENT;
  }

  for (unsigned int channel = 0; channel < nchannels_epd_data; ++channel)
  {
    TowerInfo *tower_data = towerinfosEPD_data->get_tower_at_channel(channel);
    TowerInfo *tower_mc = towerinfosEPD_mc->get_tower_at_channel(channel);
    TowerInfo *tower_data_mc = towerinfosEPD_data_mc->get_tower_at_channel(channel);

    double charge_data = tower_data->get_energy();
    double charge_mc = tower_mc->get_energy();
    double charge_data_mc = tower_data_mc->get_energy();

    std::string name_data = std::format("hSEPD_data_channel_{}", channel);
    std::string name_mc = std::format("hSEPD_mc_channel_{}", channel);

    m_hists[name_data]->Fill(charge_data);
    m_hists[name_mc]->Fill(charge_mc);

    m_hists["hSEPD_Charge_Data"]->Fill(charge_data);
    m_hists["hSEPD_Charge_MC"]->Fill(charge_mc);

    m_ctr["sepd_data_charge_max"] = std::max(static_cast<double>(m_ctr["sepd_data_charge_max"]), charge_data);
    m_ctr["sepd_data_charge_min"] = std::min(static_cast<double>(m_ctr["sepd_data_charge_min"]), charge_data);

    m_ctr["sepd_mc_charge_max"] = std::max(static_cast<double>(m_ctr["sepd_mc_charge_max"]), charge_mc);
    m_ctr["sepd_mc_charge_min"] = std::min(static_cast<double>(m_ctr["sepd_mc_charge_min"]), charge_mc);

    m_ctr["sepd_data_mc_charge_max"] = std::max(static_cast<double>(m_ctr["sepd_data_mc_charge_max"]), charge_data_mc);
    m_ctr["sepd_data_mc_charge_min"] = std::min(static_cast<double>(m_ctr["sepd_data_mc_charge_min"]), charge_data_mc);
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int sEPD_DataMC_Validation::process_event(PHCompositeNode *topNode)
{
  EventHeader *eventInfo = findNode::getClass<EventHeader>(topNode, "EventHeader");
  if (!eventInfo)
  {
    return Fun4AllReturnCodes::ABORTRUN;
  }

  int m_globalEvent = eventInfo->get_EvtSequence();

  if (m_ctr["events_total"] % 1000 == 0)
  {
    std::cout << std::format("Progress: {}, Global: {}\n", m_ctr["events_total"], m_globalEvent);
  }
  ++m_ctr["events_total"];


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

  if(m_cent >= m_cent_min_threshold)
  {
    ret = process_sEPD(topNode);
    if (ret)
    {
      return ret;
    }
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int sEPD_DataMC_Validation::ResetEvent([[maybe_unused]] PHCompositeNode *topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int sEPD_DataMC_Validation::End([[maybe_unused]] PHCompositeNode *topNode)
{
  std::cout << "sEPD_DataMC_Validation::End" << std::endl;

  std::cout << std::format("{:#<20}\n", "");
  std::cout << "stats" << std::endl;

  for (const auto &[name, counts] : m_ctr)
  {
    std::cout << std::format("{}: {}\n", name, counts);
  }

  TFile output(m_outfile_name.c_str(), "recreate");
  output.cd();

  for (const auto &[name, hist] : m_hists)
  {
    if (!name.ends_with("calib"))
    {
      std::cout << std::format("Saving: {}\n", name);
      hist->Write();
    }
  }

  output.Close();

  return Fun4AllReturnCodes::EVENT_OK;
}
