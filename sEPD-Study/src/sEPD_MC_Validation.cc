#include "sEPD_MC_Validation.h"

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

// -- MBD
#include <mbd/MbdGeom.h>
#include <mbd/MbdPmtContainer.h>
#include <mbd/MbdPmtHit.h>

// -- sEPD
#include <epd/EpdGeom.h>

// -- ROOT
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TProfile2D.h>
#include <TTree.h>
#include <TVector2.h>

//____________________________________________________________________________..
sEPD_MC_Validation::sEPD_MC_Validation(const std::string &name)
  : SubsysReco(name)
{
}

//____________________________________________________________________________..
int sEPD_MC_Validation::Init([[maybe_unused]] PHCompositeNode *topNode)
{
  Fun4AllServer *se = Fun4AllServer::instance();
  se->Print("NODETREE");

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

  h2SEPD_Psi2_raw_S = new TH2F("h2SEPD_Psi2_raw_S", "|z| < 10 cm and MB; Centrality [%]; 2#Psi_{2}", m_hist_config.m_bins_cent, m_hist_config.m_cent_low, m_hist_config.m_cent_high, m_hist_config.m_bins_psi, m_hist_config.m_psi_low, m_hist_config.m_psi_high);
  se->registerHisto(h2SEPD_Psi2_raw_S);

  h2SEPD_Psi2_raw_N = new TH2F("h2SEPD_Psi2_raw_N", "|z| < 10 cm and MB; Centrality [%]; 2#Psi_{2}", m_hist_config.m_bins_cent, m_hist_config.m_cent_low, m_hist_config.m_cent_high, m_hist_config.m_bins_psi, m_hist_config.m_psi_low, m_hist_config.m_psi_high);
  se->registerHisto(h2SEPD_Psi2_raw_N);

  h2SEPD_Psi2_S = new TH2F("h2SEPD_Psi2_S", "|z| < 10 cm and MB; Centrality [%]; 2#Psi_{2}", m_hist_config.m_bins_cent, m_hist_config.m_cent_low, m_hist_config.m_cent_high, m_hist_config.m_bins_psi, m_hist_config.m_psi_low, m_hist_config.m_psi_high);
  se->registerHisto(h2SEPD_Psi2_S);

  h2SEPD_Psi2_N = new TH2F("h2SEPD_Psi2_N", "|z| < 10 cm and MB; Centrality [%]; 2#Psi_{2}", m_hist_config.m_bins_cent, m_hist_config.m_cent_low, m_hist_config.m_cent_high, m_hist_config.m_bins_psi, m_hist_config.m_psi_low, m_hist_config.m_psi_high);
  se->registerHisto(h2SEPD_Psi2_N);

  h2Truth_Psi2 = new TH2F("h2Truth_Psi2", "|z| < 10 cm and MB; Centrality [%]; 2#Psi_{2}",
                          m_hist_config.m_bins_cent, m_hist_config.m_cent_low, m_hist_config.m_cent_high,
                          m_hist_config.m_bins_psi, m_hist_config.m_psi_low, m_hist_config.m_psi_high);
  se->registerHisto(h2Truth_Psi2);

  h2Delta_TruthPsi2_RecoPsi2_South = new TH2F("h2Delta_TruthPsi2_RecoPsi2_South",
                                              "|z| < 10 cm and MB; Centrality [%]; 2(#Psi_{2,truth} - #Psi_{2,S})",
                                              m_hist_config.m_bins_cent, m_hist_config.m_cent_low, m_hist_config.m_cent_high,
                                              m_hist_config.m_bins_psi, m_hist_config.m_psi_low, m_hist_config.m_psi_high);
  se->registerHisto(h2Delta_TruthPsi2_RecoPsi2_South);

  h2Delta_TruthPsi2_RecoPsi2_North = new TH2F("h2Delta_TruthPsi2_RecoPsi2_North",
                                              "|z| < 10 cm and MB; Centrality [%]; 2(#Psi_{2,truth} - #Psi_{2,N})",
                                              m_hist_config.m_bins_cent, m_hist_config.m_cent_low, m_hist_config.m_cent_high,
                                              m_hist_config.m_bins_psi, m_hist_config.m_psi_low, m_hist_config.m_psi_high);
  se->registerHisto(h2Delta_TruthPsi2_RecoPsi2_North);

  h2Delta_RecoPsi2_North_South = new TH2F("h2Delta_RecoPsi2_North_South",
                                          "|z| < 10 cm and MB; Centrality [%]; 2(#Psi_{2,N} - #Psi_{2,S})",
                                          m_hist_config.m_bins_cent, m_hist_config.m_cent_low, m_hist_config.m_cent_high,
                                          m_hist_config.m_bins_psi, m_hist_config.m_psi_low, m_hist_config.m_psi_high);
  se->registerHisto(h2Delta_RecoPsi2_North_South);

  // ---------------------------------

  for (unsigned int i = 0; i < m_eventType.size(); ++i)
  {
    hEvent->GetXaxis()->SetBinLabel(i + 1, m_eventType[i].c_str());
  }

  for (unsigned int i = 0; i < m_MinBias_Type.size(); ++i)
  {
    hEventMinBias->GetXaxis()->SetBinLabel(i + 1, m_MinBias_Type[i].c_str());
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int sEPD_MC_Validation::process_event_check(PHCompositeNode *topNode)
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

  if (std::abs(zvtx) < m_cuts.m_zvtx_max_v2)
  {
    hEvent->Fill(static_cast<std::uint8_t>(EventType::ZVTX50));
    if (std::abs(zvtx) < m_cuts.m_zvtx_max)
    {
      hEvent->Fill(static_cast<std::uint8_t>(EventType::ZVTX10));
    }
  }

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

  if (std::abs(zvtx) < m_cuts.m_zvtx_max)
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

  // skip event if not minimum bias
  if (!m_mb_info->isAuAuMinimumBias())
  {
    ++m_ctr["process_eventCheck_isAuAuMinBias_fail"];
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  hVtxZ_MB->Fill(zvtx);

  // skip event if zvtx is too large
  if (std::abs(zvtx) >= m_cuts.m_zvtx_max)
  {
    ++m_ctr["process_eventCheck_zvtx_large"];
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  hEvent->Fill(static_cast<std::uint8_t>(EventType::MB));

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int sEPD_MC_Validation::process_centrality(PHCompositeNode *topNode)
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
int sEPD_MC_Validation::process_EventPlane(PHCompositeNode *topNode)
{
  // get event plane map
  EventplaneinfoMap *epmap = findNode::getClass<EventplaneinfoMap>(topNode, "EventplaneinfoMap");
  if (!epmap)
  {
    return Fun4AllReturnCodes::ABORTRUN;
  }
  if (epmap->empty())
  {
    ++m_ctr["process_EventPlane_epmap_empty"];
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  Eventplaneinfo *epd_S = epmap->get(EventplaneinfoMap::sEPDS);
  Eventplaneinfo *epd_N = epmap->get(EventplaneinfoMap::sEPDN);

  // ensure the ptrs are valid
  if (!epd_S || !epd_N)
  {
    ++m_ctr["process_EventPlane_epd_invalid"];
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  std::pair<double, double> Q_S_2_raw = epd_S->get_qvector_raw(2);
  std::pair<double, double> Q_N_2_raw = epd_N->get_qvector_raw(2);

  std::pair<double, double> Q_S_2 = epd_S->get_qvector(2);
  std::pair<double, double> Q_N_2 = epd_N->get_qvector(2);

  // Ensure both Q Vectors North and South are valid and non-zero
  if((Q_S_2.first == 0 && Q_S_2.second == 0) || (Q_N_2.first == 0 && Q_N_2.second == 0))
  {
    ++m_ctr["process_EventPlane_QVec_invalid"];
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  double _2psi2_raw_S = 2*epd_S->GetPsi(Q_S_2_raw.first, Q_S_2_raw.second, 2);
  double _2psi2_raw_N = 2*epd_N->GetPsi(Q_N_2_raw.first, Q_N_2_raw.second, 2);

  double _2psi2_S = 2*epd_S->GetPsi(Q_S_2.first, Q_S_2.second, 2);
  double _2psi2_N = 2*epd_N->GetPsi(Q_N_2.first, Q_N_2.second, 2);

  double cent = m_data.centrality;

  h2SEPD_Psi2_raw_S->Fill(cent, _2psi2_raw_S);
  h2SEPD_Psi2_raw_N->Fill(cent, _2psi2_raw_N);

  h2SEPD_Psi2_S->Fill(cent, _2psi2_S);
  h2SEPD_Psi2_N->Fill(cent, _2psi2_N);

  EventHeader *eventInfo = findNode::getClass<EventHeader>(topNode, "EventHeader");
  if (eventInfo)
  {
    double truth_psi2 = eventInfo->get_FlowPsiN(2);

    double _2psi2_truth = 2.0 * truth_psi2;

    h2Truth_Psi2->Fill(cent, _2psi2_truth);

    // Since we multiplied the n=2 angles by 2, they naturally span [-pi, pi].
    double delta_S = TVector2::Phi_mpi_pi(_2psi2_truth - _2psi2_S);
    double delta_N = TVector2::Phi_mpi_pi(_2psi2_truth - _2psi2_N);
    double delta_NS = TVector2::Phi_mpi_pi(_2psi2_N - _2psi2_S);

    h2Delta_TruthPsi2_RecoPsi2_South->Fill(cent, delta_S);
    h2Delta_TruthPsi2_RecoPsi2_North->Fill(cent, delta_N);
    h2Delta_RecoPsi2_North_South->Fill(cent, delta_NS);
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int sEPD_MC_Validation::process_event(PHCompositeNode *topNode)
{
  EventHeader *eventInfo = findNode::getClass<EventHeader>(topNode, "EventHeader");
  if (!eventInfo)
  {
    return Fun4AllReturnCodes::ABORTRUN;
  }

  m_data.event = eventInfo->get_EvtSequence();

  if (m_event % m_progress_print == 0)
  {
    std::cout << "Progress: " << m_event << ", Global: " << m_data.event << std::endl;
  }
  ++m_event;

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

  ret = process_EventPlane(topNode);
  if (ret)
  {
    return ret;
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int sEPD_MC_Validation::ResetEvent([[maybe_unused]] PHCompositeNode *topNode)
{
  ++m_ctr["event_reset"];

  // Event
  m_data.event = 0;
  m_data.zvtx = 9999;
  m_data.centrality = 9999;

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int sEPD_MC_Validation::End([[maybe_unused]] PHCompositeNode *topNode)
{
  std::cout << "sEPD_MC_Validation::End" << std::endl;

  std::cout << std::format("{:#<20}\n", "");
  std::cout << "Events" << std::endl;
  for (unsigned int i = 0; i < m_eventType.size(); ++i)
  {
    std::cout << m_eventType[i] << ": " << hEvent->GetBinContent(i + 1) << std::endl;
  }
  std::cout << std::format("{:#<20}\n", "");

  std::cout << "Events MB Fail" << std::endl;
  for (unsigned int i = 0; i < m_MinBias_Type.size(); ++i)
  {
    std::cout << m_MinBias_Type[i] << ": " << hEventMinBias->GetBinContent(i + 1) << std::endl;
  }
  std::cout << std::format("{:#<20}\n", "");

  std::cout << "Event Counters" << std::endl;
  for (const auto &[name, events] : m_ctr)
  {
    std::cout << std::format("{}: {}\n", name, events);
  }
  std::cout << std::format("{:#<20}\n", "");

  return Fun4AllReturnCodes::EVENT_OK;
}
