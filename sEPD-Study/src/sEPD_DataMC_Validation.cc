#include "sEPD_DataMC_Validation.h"

// -- c++
#include <format>
#include <iostream>
#include <algorithm>
#include <numbers>

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

// -- Vtx - Truth
#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4VtxPoint.h>

// -- MB
#include <calotrigger/MinimumBiasClassifier.h>
#include <calotrigger/MinimumBiasInfo.h>
#include <centrality/CentralityInfo.h>

// -- Event Plane
#include <eventplaneinfo/EventplaneinfoMap.h>
#include <eventplaneinfo/Eventplaneinfo.h>

// -- jet
#include <jetbase/JetContainer.h>

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

  hEvent = new TH1F("hEvent", "Event Type; Type; Events", m_eventType.size(), 0, m_eventType.size());

  for (unsigned int i = 0; i < m_eventType.size(); ++i)
  {
    hEvent->GetXaxis()->SetBinLabel(i + 1, m_eventType[i].c_str());
  }

  se->registerHisto(hEvent);

  hCentrality = new TH1F("hCentrality", "|z| < 10 cm and MB; Centrality [%]; Events", m_bins_cent, m_cent_low, m_cent_high);

  hCentralityZ50 = new TH1F("hCentralityZ50", "|z| < 50 cm and MB; Centrality [%]; Events", m_bins_cent, m_cent_low, m_cent_high);

  hCentralityZOuter = new TH1F("hCentralityZOuter", "10 cm < |z| < 50 cm and MB; Centrality [%]; Events", m_bins_cent, m_cent_low, m_cent_high);

  se->registerHisto(hCentrality);
  se->registerHisto(hCentralityZ50);
  se->registerHisto(hCentralityZOuter);

  unsigned int bins_zvtx{200};
  double zvtx_low{-50};
  double zvtx_high{50};

  hZVertex = new TH1F("hZVertex", "Min Bias; Z [cm]; Events", bins_zvtx, zvtx_low, zvtx_high);
  se->registerHisto(hZVertex);

  h2ZVertexTruthvsData = new TH2F("h2ZVertexTruthvsData", "Min Bias; Data Z [cm]; Truth Z [cm]", bins_zvtx, zvtx_low, zvtx_high, bins_zvtx, zvtx_low, zvtx_high);
  se->registerHisto(h2ZVertexTruthvsData);

  h2ZVertexCentrality = new TH2F("h2ZVertexCentrality", "Min Bias; Z [cm]; Centrality [%]", bins_zvtx, zvtx_low, zvtx_high, m_bins_cent, m_cent_low, m_cent_high);
  se->registerHisto(h2ZVertexCentrality);

  unsigned int bins_sepd_totalcharge{100};
  double sepd_totalcharge_low{0};
  double sepd_totalcharge_high{2e4};

  h2SEPD_totalcharge_centrality = new TH2F("h2SEPD_totalcharge_centrality",
                                           "|z| < 10 cm and MB; sEPD Total Charge; Centrality [%]",
                                           bins_sepd_totalcharge, sepd_totalcharge_low, sepd_totalcharge_high,
                                           m_bins_cent, m_cent_low, m_cent_high);

  se->registerHisto(h2SEPD_totalcharge_centrality);

  unsigned int bins_charge{2500};
  double charge_low{-50};
  double charge_high{200};

  std::string sEPD_Charge_title = "|z| < 10 cm and MB; sEPD Charge; Towers";

  hSEPD_Charge_Data = new TH1F("hSEPD_Charge_Data", sEPD_Charge_title.c_str(), bins_charge, charge_low, charge_high);
  hSEPD_Charge_MC = new TH1F("hSEPD_Charge_MC", sEPD_Charge_title.c_str(), bins_charge, charge_low, charge_high);
  hSEPD_Charge_DataMC = new TH1F("hSEPD_Charge_DataMC", sEPD_Charge_title.c_str(), bins_charge, charge_low, charge_high);

  hSEPD_BadMasked_Charge_Data = new TH1F("hSEPD_BadMasked_Charge_Data", sEPD_Charge_title.c_str(), bins_charge, charge_low, charge_high);
  hSEPD_BadMasked_Charge_MC = new TH1F("hSEPD_BadMasked_Charge_MC", sEPD_Charge_title.c_str(), bins_charge, charge_low, charge_high);
  hSEPD_BadMasked_Charge_DataMC = new TH1F("hSEPD_BadMasked_Charge_DataMC", sEPD_Charge_title.c_str(), bins_charge, charge_low, charge_high);

  se->registerHisto(hSEPD_Charge_Data);
  se->registerHisto(hSEPD_Charge_MC);
  se->registerHisto(hSEPD_Charge_DataMC);

  se->registerHisto(hSEPD_BadMasked_Charge_Data);
  se->registerHisto(hSEPD_BadMasked_Charge_MC);
  se->registerHisto(hSEPD_BadMasked_Charge_DataMC);

  unsigned int bins_r{16};

  sEPD_Charge_title = "|z| < 10 cm and MB; Charge; r_{bin}";

  h2SEPD_South_Charge_Data = new TH2F("h2SEPD_South_Charge_Data", sEPD_Charge_title.c_str(), bins_charge, charge_low, charge_high, bins_r, 0, bins_r);
  h2SEPD_South_Charge_MC = new TH2F("h2SEPD_South_Charge_MC", sEPD_Charge_title.c_str(), bins_charge, charge_low, charge_high, bins_r, 0, bins_r);
  h2SEPD_South_Charge_DataMC = new TH2F("h2SEPD_South_Charge_DataMC", sEPD_Charge_title.c_str(), bins_charge, charge_low, charge_high, bins_r, 0, bins_r);

  h2SEPD_North_Charge_Data = new TH2F("h2SEPD_North_Charge_Data", sEPD_Charge_title.c_str(), bins_charge, charge_low, charge_high, bins_r, 0, bins_r);
  h2SEPD_North_Charge_MC = new TH2F("h2SEPD_North_Charge_MC", sEPD_Charge_title.c_str(), bins_charge, charge_low, charge_high, bins_r, 0, bins_r);
  h2SEPD_North_Charge_DataMC = new TH2F("h2SEPD_North_Charge_DataMC", sEPD_Charge_title.c_str(), bins_charge, charge_low, charge_high, bins_r, 0, bins_r);

  h2SEPD_BadMasked_South_Charge_Data = new TH2F("h2SEPD_BadMasked_South_Charge_Data", sEPD_Charge_title.c_str(), bins_charge, charge_low, charge_high, bins_r, 0, bins_r);
  h2SEPD_BadMasked_South_Charge_MC = new TH2F("h2SEPD_BadMasked_South_Charge_MC", sEPD_Charge_title.c_str(), bins_charge, charge_low, charge_high, bins_r, 0, bins_r);
  h2SEPD_BadMasked_South_Charge_DataMC = new TH2F("h2SEPD_BadMasked_South_Charge_DataMC", sEPD_Charge_title.c_str(), bins_charge, charge_low, charge_high, bins_r, 0, bins_r);

  h2SEPD_BadMasked_North_Charge_Data = new TH2F("h2SEPD_BadMasked_North_Charge_Data", sEPD_Charge_title.c_str(), bins_charge, charge_low, charge_high, bins_r, 0, bins_r);
  h2SEPD_BadMasked_North_Charge_MC = new TH2F("h2SEPD_BadMasked_North_Charge_MC", sEPD_Charge_title.c_str(), bins_charge, charge_low, charge_high, bins_r, 0, bins_r);
  h2SEPD_BadMasked_North_Charge_DataMC = new TH2F("h2SEPD_BadMasked_North_Charge_DataMC", sEPD_Charge_title.c_str(), bins_charge, charge_low, charge_high, bins_r, 0, bins_r);

  se->registerHisto(h2SEPD_South_Charge_Data);
  se->registerHisto(h2SEPD_South_Charge_MC);
  se->registerHisto(h2SEPD_South_Charge_DataMC);

  se->registerHisto(h2SEPD_North_Charge_Data);
  se->registerHisto(h2SEPD_North_Charge_MC);
  se->registerHisto(h2SEPD_North_Charge_DataMC);

  se->registerHisto(h2SEPD_BadMasked_South_Charge_Data);
  se->registerHisto(h2SEPD_BadMasked_South_Charge_MC);
  se->registerHisto(h2SEPD_BadMasked_South_Charge_DataMC);

  se->registerHisto(h2SEPD_BadMasked_North_Charge_Data);
  se->registerHisto(h2SEPD_BadMasked_North_Charge_MC);
  se->registerHisto(h2SEPD_BadMasked_North_Charge_DataMC);

  sEPD_Charge_title = "|z| < 10 cm and MB; Channel; Charge";

  pSEPD_Charge_Data = new TProfile("pSEPD_Charge_Data", sEPD_Charge_title.c_str(), m_sepd_channels, 0, m_sepd_channels);
  pSEPD_Charge_MC = new TProfile("pSEPD_Charge_MC", sEPD_Charge_title.c_str(), m_sepd_channels, 0, m_sepd_channels);
  pSEPD_Charge_DataMC = new TProfile("pSEPD_Charge_DataMC", sEPD_Charge_title.c_str(), m_sepd_channels, 0, m_sepd_channels);

  pSEPD_BadMasked_Charge_Data = new TProfile("pSEPD_BadMasked_Charge_Data", sEPD_Charge_title.c_str(), m_sepd_channels, 0, m_sepd_channels);
  pSEPD_BadMasked_Charge_MC = new TProfile("pSEPD_BadMasked_Charge_MC", sEPD_Charge_title.c_str(), m_sepd_channels, 0, m_sepd_channels);
  pSEPD_BadMasked_Charge_DataMC = new TProfile("pSEPD_BadMasked_Charge_DataMC", sEPD_Charge_title.c_str(), m_sepd_channels, 0, m_sepd_channels);

  se->registerHisto(pSEPD_Charge_Data);
  se->registerHisto(pSEPD_Charge_MC);
  se->registerHisto(pSEPD_Charge_DataMC);

  se->registerHisto(pSEPD_BadMasked_Charge_Data);
  se->registerHisto(pSEPD_BadMasked_Charge_MC);
  se->registerHisto(pSEPD_BadMasked_Charge_DataMC);

  unsigned int bins_jet_pt{100};
  double jet_pt_low{0};
  double jet_pt_high{100};

  unsigned int bins_jet_eta{24};
  double jet_eta_low{-1};
  double jet_eta_high{1};

  unsigned int bins_jet_phi{64};
  double jet_phi_low{-std::numbers::pi};
  double jet_phi_high{std::numbers::pi};

  unsigned int bins_zvtx2{40};
  double zvtx2_low{-10};
  double zvtx2_high{10};

  unsigned int bins_jet_etav2{160};
  double jet_etav2_low{-4};
  double jet_etav2_high{4};

  hJetPt = new TH1F("hJetPt", "|z| < 10 cm and MB; p_{T} [GeV]; Counts", bins_jet_pt, jet_pt_low, jet_pt_high);
  se->registerHisto(hJetPt);

  h2JetPtCentrality = new TH2F("h2JetPtCentrality", "|z| < 10 cm and MB; p_{T} [GeV]; Centrality [%]", bins_jet_pt, jet_pt_low, jet_pt_high, m_bins_cent/10, m_cent_low, m_cent_high);
  se->registerHisto(h2JetPtCentrality);

  h2JetPhiEta = new TH2F("h2JetPhiEta", "|z| < 10 cm and MB; #phi; #eta",
                         bins_jet_phi, jet_phi_low, jet_phi_high,
                         bins_jet_eta, jet_eta_low, jet_eta_high);

  se->registerHisto(h2JetPhiEta);

  h2JetEtaVtxZ = new TH2F("h2JetEtaVtxZ", "MB; z [cm]; #eta_{det}",
                          bins_zvtx2, zvtx2_low, zvtx2_high,
                          bins_jet_etav2, jet_etav2_low, jet_etav2_high);

  se->registerHisto(h2JetEtaVtxZ);

  unsigned int bins_psi{126};
  double psi_low{-std::numbers::pi};
  double psi_high{std::numbers::pi};

  // Raw
  h2SEPD_Psi2_raw_data_S = new TH2F("h2SEPD_Psi2_raw_data_S", "|z| < 10 cm and MB; 2#Psi_{2}; Centrality [%]",
                            bins_psi, psi_low, psi_high,
                            m_bins_cent, m_cent_low, m_cent_high);

  h2SEPD_Psi2_raw_data_N = new TH2F("h2SEPD_Psi2_raw_data_N", "|z| < 10 cm and MB; 2#Psi_{2}; Centrality [%]",
                            bins_psi, psi_low, psi_high,
                            m_bins_cent, m_cent_low, m_cent_high);

  h2SEPD_Psi2_raw_data_mc_S = new TH2F("h2SEPD_Psi2_raw_data_mc_S", "|z| < 10 cm and MB; 2#Psi_{2}; Centrality [%]",
                            bins_psi, psi_low, psi_high,
                            m_bins_cent, m_cent_low, m_cent_high);

  h2SEPD_Psi2_raw_data_mc_N = new TH2F("h2SEPD_Psi2_raw_data_mc_N", "|z| < 10 cm and MB; 2#Psi_{2}; Centrality [%]",
                            bins_psi, psi_low, psi_high,
                            m_bins_cent, m_cent_low, m_cent_high);

  h2SEPD_Psi2_raw_data_mc_NS = new TH2F("h2SEPD_Psi2_raw_data_mc_NS", "|z| < 10 cm and MB; 2#Psi_{2}; Centrality [%]",
                            bins_psi, psi_low, psi_high,
                            m_bins_cent, m_cent_low, m_cent_high);

  // Corrected
  h2SEPD_Psi2_data_S = new TH2F("h2SEPD_Psi2_data_S", "|z| < 10 cm and MB; 2#Psi_{2}; Centrality [%]",
                            bins_psi, psi_low, psi_high,
                            m_bins_cent, m_cent_low, m_cent_high);

  h2SEPD_Psi2_data_N = new TH2F("h2SEPD_Psi2_data_N", "|z| < 10 cm and MB; 2#Psi_{2}; Centrality [%]",
                            bins_psi, psi_low, psi_high,
                            m_bins_cent, m_cent_low, m_cent_high);

  h2SEPD_Psi2_data_NS = new TH2F("h2SEPD_Psi2_data_NS", "|z| < 10 cm and MB; 2#Psi_{2}; Centrality [%]",
                             bins_psi, psi_low, psi_high,
                             m_bins_cent, m_cent_low, m_cent_high);

  h2SEPD_Psi2_data_mc_S = new TH2F("h2SEPD_Psi2_data_mc_S", "|z| < 10 cm and MB; 2#Psi_{2}; Centrality [%]",
                            bins_psi, psi_low, psi_high,
                            m_bins_cent, m_cent_low, m_cent_high);

  h2SEPD_Psi2_data_mc_N = new TH2F("h2SEPD_Psi2_data_mc_N", "|z| < 10 cm and MB; 2#Psi_{2}; Centrality [%]",
                            bins_psi, psi_low, psi_high,
                            m_bins_cent, m_cent_low, m_cent_high);

  h2SEPD_Psi2_data_mc_NS = new TH2F("h2SEPD_Psi2_data_mc_NS", "|z| < 10 cm and MB; 2#Psi_{2}; Centrality [%]",
                             bins_psi, psi_low, psi_high,
                             m_bins_cent, m_cent_low, m_cent_high);

  se->registerHisto(h2SEPD_Psi2_raw_data_S);
  se->registerHisto(h2SEPD_Psi2_raw_data_N);

  se->registerHisto(h2SEPD_Psi2_raw_data_mc_S);
  se->registerHisto(h2SEPD_Psi2_raw_data_mc_N);
  se->registerHisto(h2SEPD_Psi2_raw_data_mc_NS);

  se->registerHisto(h2SEPD_Psi2_data_S);
  se->registerHisto(h2SEPD_Psi2_data_N);
  se->registerHisto(h2SEPD_Psi2_data_NS);

  se->registerHisto(h2SEPD_Psi2_data_mc_S);
  se->registerHisto(h2SEPD_Psi2_data_mc_N);
  se->registerHisto(h2SEPD_Psi2_data_mc_NS);

  unsigned int bins_QQ_avg{50};
  double QQ_avg_low{0};
  double QQ_avg_high{5e-4};

  unsigned int bins_qQ_avg{500};
  double qQ_avg_low{-1e-2};
  double qQ_avg_high{1e-2};

  hRefFlow_data = new TProfile("hRefFlow_data", "Reference Flow; Centrality [%]; Re(#LTQ^{S}_{2} Q^{N*}_{2}#GT)",
                                m_bins_cent, m_cent_low, m_cent_high);

  hRefFlow_data_mc = new TProfile("hRefFlow_data_mc", "Reference Flow; Centrality [%]; Re(#LTQ^{S}_{2} Q^{N*}_{2}#GT)",
                                m_bins_cent, m_cent_low, m_cent_high);

  h2RefFlow_data = new TH2F("h2RefFlow_data", "Reference Flow; Re(Q^{S}_{2} Q^{N*}_{2}); Centrality [%]",
                            bins_QQ_avg, QQ_avg_low, QQ_avg_high,
                            m_bins_cent / 10, m_cent_low, m_cent_high);

  h2RefFlow_data_mc = new TH2F("h2RefFlow_data_mc", "Reference Flow; Re(Q^{S}_{2} Q^{N*}_{2}); Centrality [%]",
                               bins_QQ_avg, QQ_avg_low, QQ_avg_high,
                               m_bins_cent / 10, m_cent_low, m_cent_high);

  hScalarProduct_data = new TProfile("hScalarProduct_data", "Scalar Product; Centrality [%]; Re(#LTq_{2} Q^{S|N*}_{2}#GT)",
                                      m_bins_cent, m_cent_low, m_cent_high);

  hScalarProduct_data_mc = new TProfile("hScalarProduct_data_mc", "Scalar Product; Centrality [%]; Re(#LTq_{2} Q^{S|N*}_{2}#GT)",
                                      m_bins_cent, m_cent_low, m_cent_high);

  hScalarProduct_data_mc_anti = new TProfile("hScalarProduct_data_mc_anti", "Scalar Product; Centrality [%]; Re(#LTq_{2} Q^{N|S*}_{2}#GT)",
                                      m_bins_cent, m_cent_low, m_cent_high);

  h2ScalarProduct_data = new TH2F("h2ScalarProduct_data", "Scalar Product; Re(q_{2} Q^{S|N*}_{2}); Centrality [%]",
                                  bins_qQ_avg, qQ_avg_low, qQ_avg_high,
                                  m_bins_cent / 10, m_cent_low, m_cent_high);

  h2ScalarProduct_data_mc = new TH2F("h2ScalarProduct_data_mc", "Scalar Product; Re(q_{2} Q^{S|N*}_{2}); Centrality [%]",
                                     bins_qQ_avg, qQ_avg_low, qQ_avg_high,
                                     m_bins_cent / 10, m_cent_low, m_cent_high);

  se->registerHisto(hRefFlow_data);
  se->registerHisto(hRefFlow_data_mc);
  se->registerHisto(h2RefFlow_data);
  se->registerHisto(h2RefFlow_data_mc);
  se->registerHisto(hScalarProduct_data);
  se->registerHisto(hScalarProduct_data_mc);
  se->registerHisto(h2ScalarProduct_data);
  se->registerHisto(h2ScalarProduct_data_mc);
  se->registerHisto(hScalarProduct_data_mc_anti);

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

  GlobalVertexMap *vertexmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");

  if (!vertexmap)
  {
    return Fun4AllReturnCodes::ABORTRUN;
  }

  if (!vertexmap->empty())
  {
    GlobalVertex *vtx = vertexmap->begin()->second;
    m_zvtx = vtx->get_z();
  }

  double truth_z = -9999;

  PHG4TruthInfoContainer *truthInfo = findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");

  if (truthInfo)
  {
    // Get the index of the primary vertex
    // This method identifies the vertex with the highest embedding ID (the "signal" event)
    int vtxID = truthInfo->GetPrimaryVertexIndex();

    // Retrieve the vertex point object using that ID
    PHG4VtxPoint *vtx = truthInfo->GetVtx(vtxID);

    if (vtx)
    {
      // Access the truth z-vertex
      truth_z = vtx->get_z();
    }
  }

  CentralityInfo *centInfo = findNode::getClass<CentralityInfo>(topNode, "CentralityInfo");
  if (!centInfo)
  {
    return Fun4AllReturnCodes::ABORTRUN;
  }

  m_cent = centInfo->get_centile(CentralityInfo::PROP::mbd_NS) * 100;

  hEvent->Fill(static_cast<int>(EventType::ALL));

  if (std::abs(m_zvtx) < m_zvtx_max)
  {
    hEvent->Fill(static_cast<int>(EventType::ZVTX10));
  }

  // skip event if not minimum bias
  if (!mb_info->isAuAuMinimumBias())
  {
    ++m_ctr["events_isAuAuMinBias_fail"];
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  hZVertex->Fill(m_zvtx);
  h2ZVertexTruthvsData->Fill(m_zvtx, truth_z);
  h2ZVertexCentrality->Fill(m_zvtx, m_cent);

  if (std::abs(m_zvtx) < m_zvtx_max_v2)
  {
    hCentralityZ50->Fill(m_cent);

    if (std::abs(m_zvtx) > m_zvtx_max)
    {
      hCentralityZOuter->Fill(m_cent);
    }
  }

  // skip event if zvtx is too large
  if (std::abs(m_zvtx) > m_zvtx_max)
  {
    ++m_ctr["events_zvtx_fail"];
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  hEvent->Fill(static_cast<int>(EventType::ZVTX10_MB));

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int sEPD_DataMC_Validation::process_centrality(PHCompositeNode *topNode)
{
  if (!std::isfinite(m_cent) || m_cent < 0 || m_cent >= m_cent_max_threshold)
  {
    ++m_ctr["events_centrality_bad"];
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  if (m_cent < m_cent_max_threshold_ana)
  {
    hEvent->Fill(static_cast<int>(EventType::ZVTX10_MB_CENT));
  }

  hCentrality->Fill(m_cent);

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

  if (nchannels_epd_data != nchannels_epd_mc || nchannels_epd_data != nchannels_epd_data_mc)
  {
    ++m_ctr["events_sepd_channel_fail"];
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  double sepd_totalcharge = 0;

  for (unsigned int channel = 0; channel < nchannels_epd_data; ++channel)
  {
    TowerInfo *tower_data = towerinfosEPD_data->get_tower_at_channel(channel);
    TowerInfo *tower_mc = towerinfosEPD_mc->get_tower_at_channel(channel);
    TowerInfo *tower_data_mc = towerinfosEPD_data_mc->get_tower_at_channel(channel);

    double charge_data = tower_data->get_energy();
    double charge_mc = tower_mc->get_energy();
    double charge_data_mc = tower_data_mc->get_energy();

    unsigned int key = TowerInfoDefs::encode_epd(channel);
    int rbin = TowerInfoDefs::get_epd_rbin(key);
    unsigned int arm = TowerInfoDefs::get_epd_arm(key);

    if (Verbosity() > 2 && channel < 100)
    {
      std::cout << std::format("Channel: {}, data: {:.6f}, MC: {:.6f}, data_MC: {:.6f}, Check: {:.6f}\n",
                               channel, charge_data, charge_mc, charge_data_mc, charge_data_mc - charge_mc - charge_data);
    }

    hSEPD_Charge_Data->Fill(charge_data);
    hSEPD_Charge_MC->Fill(charge_mc);
    hSEPD_Charge_DataMC->Fill(charge_data_mc);

    // South
    if (arm == 0)
    {
      h2SEPD_South_Charge_Data->Fill(charge_data, rbin);
      h2SEPD_South_Charge_MC->Fill(charge_mc, rbin);
      h2SEPD_South_Charge_DataMC->Fill(charge_data_mc, rbin);
    }
    // North
    else
    {
      h2SEPD_North_Charge_Data->Fill(charge_data, rbin);
      h2SEPD_North_Charge_MC->Fill(charge_mc, rbin);
      h2SEPD_North_Charge_DataMC->Fill(charge_data_mc, rbin);
    }

    pSEPD_Charge_Data->Fill(channel, charge_data);
    pSEPD_Charge_MC->Fill(channel, charge_mc);
    pSEPD_Charge_DataMC->Fill(channel, charge_data_mc);

    sepd_totalcharge += charge_data_mc;

    if (!tower_data_mc->get_isHot())
    {
      hSEPD_BadMasked_Charge_Data->Fill(charge_data);
      hSEPD_BadMasked_Charge_MC->Fill(charge_mc);
      hSEPD_BadMasked_Charge_DataMC->Fill(charge_data_mc);

      // South
      if (arm == 0)
      {
        h2SEPD_BadMasked_South_Charge_Data->Fill(charge_data, rbin);
        h2SEPD_BadMasked_South_Charge_MC->Fill(charge_mc, rbin);
        h2SEPD_BadMasked_South_Charge_DataMC->Fill(charge_data_mc, rbin);
      }
      // North
      else
      {
        h2SEPD_BadMasked_North_Charge_Data->Fill(charge_data, rbin);
        h2SEPD_BadMasked_North_Charge_MC->Fill(charge_mc, rbin);
        h2SEPD_BadMasked_North_Charge_DataMC->Fill(charge_data_mc, rbin);
      }

      pSEPD_BadMasked_Charge_Data->Fill(channel, charge_data);
      pSEPD_BadMasked_Charge_MC->Fill(channel, charge_mc);
      pSEPD_BadMasked_Charge_DataMC->Fill(channel, charge_data_mc);
    }
  }

  h2SEPD_totalcharge_centrality->Fill(sepd_totalcharge, m_cent);

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int sEPD_DataMC_Validation::process_EventPlane(PHCompositeNode *topNode)
{
  // get event plane map
  EventplaneinfoMap *epmap_data = findNode::getClass<EventplaneinfoMap>(topNode, "EventplaneinfoMap_data");
  EventplaneinfoMap *epmap_data_mc = findNode::getClass<EventplaneinfoMap>(topNode, "EventplaneinfoMap_data_mc");

  if (!epmap_data || !epmap_data_mc)
  {
    return Fun4AllReturnCodes::ABORTRUN;
  }
  if (epmap_data->empty() || epmap_data_mc->empty())
  {
    ++m_ctr["events_EventPlane_epmap_empty"];
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  Eventplaneinfo *epd_data_S = epmap_data->get(EventplaneinfoMap::sEPDS);
  Eventplaneinfo *epd_data_N = epmap_data->get(EventplaneinfoMap::sEPDN);
  Eventplaneinfo *epd_data_NS = epmap_data->get(EventplaneinfoMap::sEPDNS);

  Eventplaneinfo *epd_data_mc_S = epmap_data_mc->get(EventplaneinfoMap::sEPDS);
  Eventplaneinfo *epd_data_mc_N = epmap_data_mc->get(EventplaneinfoMap::sEPDN);
  Eventplaneinfo *epd_data_mc_NS = epmap_data_mc->get(EventplaneinfoMap::sEPDNS);

  // ensure the ptrs are valid
  if (!epd_data_S || !epd_data_N || !epd_data_NS || !epd_data_mc_S || !epd_data_mc_N || !epd_data_mc_NS)
  {
    ++m_ctr["events_EventPlane_epd_invalid"];
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  m_Q_data_S_2 = epd_data_S->get_qvector(2);
  m_Q_data_N_2 = epd_data_N->get_qvector(2);

  m_Q_data_mc_S_2 = epd_data_mc_S->get_qvector(2);
  m_Q_data_mc_N_2 = epd_data_mc_N->get_qvector(2);

  std::pair<double, double> Q_raw_data_S_2 = epd_data_S->get_qvector_raw(2);
  std::pair<double, double> Q_raw_data_N_2 = epd_data_N->get_qvector_raw(2);

  std::pair<double, double> Q_raw_data_mc_S_2 = epd_data_mc_S->get_qvector_raw(2);
  std::pair<double, double> Q_raw_data_mc_N_2 = epd_data_mc_N->get_qvector_raw(2);
  std::pair<double, double> Q_raw_data_mc_NS_2 = epd_data_mc_NS->get_qvector_raw(2);

  double ref_flow_data = m_Q_data_S_2.first * m_Q_data_N_2.first + m_Q_data_S_2.second * m_Q_data_N_2.second;
  double ref_flow_data_mc = m_Q_data_mc_S_2.first * m_Q_data_mc_N_2.first + m_Q_data_mc_S_2.second * m_Q_data_mc_N_2.second;

  hRefFlow_data->Fill(m_cent, ref_flow_data);
  hRefFlow_data_mc->Fill(m_cent, ref_flow_data_mc);

  h2RefFlow_data->Fill(ref_flow_data, m_cent);
  h2RefFlow_data_mc->Fill(ref_flow_data_mc, m_cent);

  double _2psi2_data_S = 2*epd_data_S->get_shifted_psi(2);
  double _2psi2_data_N = 2*epd_data_N->get_shifted_psi(2);
  double _2psi2_data_NS = 2*epd_data_NS->get_shifted_psi(2);

  double _2psi2_data_mc_S = 2*epd_data_mc_S->get_shifted_psi(2);
  double _2psi2_data_mc_N = 2*epd_data_mc_N->get_shifted_psi(2);
  double _2psi2_data_mc_NS = 2*epd_data_mc_NS->get_shifted_psi(2);

  double _2psi2_raw_data_S = 2*epd_data_S->GetPsi(Q_raw_data_S_2.first, Q_raw_data_S_2.second, 2);
  double _2psi2_raw_data_N = 2*epd_data_N->GetPsi(Q_raw_data_N_2.first, Q_raw_data_N_2.second, 2);

  double _2psi2_raw_data_mc_S = 2*epd_data_mc_S->GetPsi(Q_raw_data_mc_S_2.first, Q_raw_data_mc_S_2.second, 2);
  double _2psi2_raw_data_mc_N = 2*epd_data_mc_N->GetPsi(Q_raw_data_mc_N_2.first, Q_raw_data_mc_N_2.second, 2);
  double _2psi2_raw_data_mc_NS = 2*epd_data_mc_N->GetPsi(Q_raw_data_mc_NS_2.first, Q_raw_data_mc_NS_2.second, 2);

  h2SEPD_Psi2_data_S->Fill(_2psi2_data_S, m_cent);
  h2SEPD_Psi2_data_N->Fill(_2psi2_data_N, m_cent);
  h2SEPD_Psi2_data_NS->Fill(_2psi2_data_NS, m_cent);

  h2SEPD_Psi2_data_mc_S->Fill(_2psi2_data_mc_S, m_cent);
  h2SEPD_Psi2_data_mc_N->Fill(_2psi2_data_mc_N, m_cent);
  h2SEPD_Psi2_data_mc_NS->Fill(_2psi2_data_mc_NS, m_cent);

  h2SEPD_Psi2_raw_data_S->Fill(_2psi2_raw_data_S, m_cent);
  h2SEPD_Psi2_raw_data_N->Fill(_2psi2_raw_data_N, m_cent);

  h2SEPD_Psi2_raw_data_mc_S->Fill(_2psi2_raw_data_mc_S, m_cent);
  h2SEPD_Psi2_raw_data_mc_N->Fill(_2psi2_raw_data_mc_N, m_cent);
  h2SEPD_Psi2_raw_data_mc_NS->Fill(_2psi2_raw_data_mc_NS, m_cent);

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int sEPD_DataMC_Validation::process_jets(PHCompositeNode *topNode)
{
  JetContainer *jets = findNode::getClass<JetContainer>(topNode, m_recoJetName);
  if (!jets)
  {
    return Fun4AllReturnCodes::ABORTRUN;
  }

  double Q_data_S_x = m_Q_data_S_2.first;
  double Q_data_S_y = m_Q_data_S_2.second;

  double Q_data_N_x = m_Q_data_N_2.first;
  double Q_data_N_y = m_Q_data_N_2.second;

  double Q_data_mc_S_x = m_Q_data_mc_S_2.first;
  double Q_data_mc_S_y = m_Q_data_mc_S_2.second;

  double Q_data_mc_N_x = m_Q_data_mc_N_2.first;
  double Q_data_mc_N_y = m_Q_data_mc_N_2.second;

  bool hasJet = false;

  for (auto *jet : *jets)
  {
    double pt = jet->get_pt();
    // double energy = jet->get_e();
    double phi = jet->get_phi();
    double eta = jet->get_eta();

    if (pt >= m_jet_pt_min)
    {
      h2JetEtaVtxZ->Fill(m_zvtx, eta);

      if (std::abs(eta) < m_jet_eta_max)
      {
        hasJet = true;

        hJetPt->Fill(pt);
        h2JetPhiEta->Fill(phi, eta);
        h2JetPtCentrality->Fill(pt, m_cent);

        double Q_data_x = (eta > 0) ? Q_data_S_x : Q_data_N_x;
        double Q_data_y = (eta > 0) ? Q_data_S_y : Q_data_N_y;

        double Q_data_mc_x = (eta > 0) ? Q_data_mc_S_x : Q_data_mc_N_x;
        double Q_data_mc_y = (eta > 0) ? Q_data_mc_S_y : Q_data_mc_N_y;

        double Q_data_mc_anti_x = (eta > 0) ? Q_data_mc_N_x : Q_data_mc_S_x;
        double Q_data_mc_anti_y = (eta > 0) ? Q_data_mc_N_y : Q_data_mc_S_y;

        double jet_Q_x = std::cos(2 * phi);
        double jet_Q_y = std::sin(2 * phi);

        double scalar_product_data = jet_Q_x * Q_data_x + jet_Q_y * Q_data_y;
        double scalar_product_data_mc = jet_Q_x * Q_data_mc_x + jet_Q_y * Q_data_mc_y;
        double scalar_product_data_mc_anti = jet_Q_x * Q_data_mc_anti_x + jet_Q_y * Q_data_mc_anti_y;

        hScalarProduct_data->Fill(m_cent, scalar_product_data);
        hScalarProduct_data_mc->Fill(m_cent, scalar_product_data_mc);

        h2ScalarProduct_data->Fill(scalar_product_data, m_cent);
        h2ScalarProduct_data_mc->Fill(scalar_product_data_mc, m_cent);

        hScalarProduct_data_mc_anti->Fill(m_cent, scalar_product_data_mc_anti);
      }
    }
  }

  if (hasJet)
  {
    hEvent->Fill(static_cast<int>(EventType::ZVTX10_MB_CENT_JET));
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

  m_globalEvent = eventInfo->get_EvtSequence();

  if ((Verbosity() && m_ctr["events_total"] % 1000 == 0) || Verbosity() > 1)
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

  ret = process_sEPD(topNode);
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

  ++m_ctr["events_good"];
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int sEPD_DataMC_Validation::ResetEvent([[maybe_unused]] PHCompositeNode *topNode)
{
  m_zvtx = -9999;
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

  std::cout << std::format("{:#<20}\n", "");

  return Fun4AllReturnCodes::EVENT_OK;
}
