#include "sEPD_DataMC_Validation.h"

// -- c++
#include <format>
#include <iostream>
#include <algorithm>
#include <numbers>
#include <numeric>
#include <span>

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

// -- jetbackground
#include <jetbackground/TowerBackground.h>

#include <TFile.h>
#include <TTree.h>

//____________________________________________________________________________..
sEPD_DataMC_Validation::sEPD_DataMC_Validation(const std::string &name)
  : SubsysReco(name)
{
}

//____________________________________________________________________________..

void sEPD_DataMC_Validation::setup_tree()
{
  m_output = std::make_unique<TFile>(m_outtree_name.c_str(), "recreate");
  m_output->cd();

  // TTree
  m_tree = new TTree("T", "T");
  m_tree->SetDirectory(m_output.get());

  // Event Info
  m_tree->Branch("event", &m_data.event);
  m_tree->Branch("zvtx", &m_data.zvtx);
  m_tree->Branch("centrality", &m_data.centrality);
  m_tree->Branch("calo_v2", &m_data.calo_v2);
  m_tree->Branch("is_flow_failure", &m_data.is_flow_failure);
  m_tree->Branch("ue_emcal_avg", &m_data.ue_emcal_avg);
  m_tree->Branch("ue_ihcal_avg", &m_data.ue_ihcal_avg);
  m_tree->Branch("ue_ohcal_avg", &m_data.ue_ohcal_avg);

  // Calo
  m_tree->Branch("emcal_energy", &m_data.emcal_energy);
  m_tree->Branch("ihcal_energy", &m_data.ihcal_energy);
  m_tree->Branch("ohcal_energy", &m_data.ohcal_energy);

  // Q Vector Info
  // South Data
  m_tree->Branch("qsx_data", &m_data.qsx_data);
  m_tree->Branch("qsy_data", &m_data.qsy_data);
  // North Data
  m_tree->Branch("qnx_data", &m_data.qnx_data);
  m_tree->Branch("qny_data", &m_data.qny_data);

  // South Data+MC
  m_tree->Branch("qsx_data_mc", &m_data.qsx_data_mc);
  m_tree->Branch("qsy_data_mc", &m_data.qsy_data_mc);

  // North Data+MC
  m_tree->Branch("qnx_data_mc", &m_data.qnx_data_mc);
  m_tree->Branch("qny_data_mc", &m_data.qny_data_mc);

  // Combined North South
  m_tree->Branch("sepdpsi2_data", &m_data.sepdpsi2_data);
  m_tree->Branch("sepdpsi2_data_mc", &m_data.sepdpsi2_data_mc);

  // Jet
  // Reco (Data+MC)
  m_tree->Branch("pt_r02", &m_data.pt_r02);
  m_tree->Branch("e_r02", &m_data.e_r02);
  m_tree->Branch("phi_r02", &m_data.phi_r02);
  m_tree->Branch("eta_r02", &m_data.eta_r02);

  m_tree->Branch("max_pt_r02", &m_data.max_pt_r02);

  m_tree->Branch("pt_r03", &m_data.pt_r03);
  m_tree->Branch("e_r03", &m_data.e_r03);
  m_tree->Branch("phi_r03", &m_data.phi_r03);
  m_tree->Branch("eta_r03", &m_data.eta_r03);

  m_tree->Branch("max_pt_r03", &m_data.max_pt_r03);

  m_tree->Branch("nJets_r02", &m_data.nJets_r02);
  m_tree->Branch("nJets_truth_r02", &m_data.nJets_truth_r02);

  m_tree->Branch("nJets_r03", &m_data.nJets_r03);
  m_tree->Branch("nJets_truth_r03", &m_data.nJets_truth_r03);

  // Truth (MC)
  m_tree->Branch("truthPt_r02", &m_data.truthPt_r02);
  m_tree->Branch("truthE_r02", &m_data.truthE_r02);
  m_tree->Branch("truthPhi_r02", &m_data.truthPhi_r02);
  m_tree->Branch("truthEta_r02", &m_data.truthEta_r02);

  m_tree->Branch("max_truthPt_r02", &m_data.max_truthPt_r02);

  m_tree->Branch("truthPt_r03", &m_data.truthPt_r03);
  m_tree->Branch("truthE_r03", &m_data.truthE_r03);
  m_tree->Branch("truthPhi_r03", &m_data.truthPhi_r03);
  m_tree->Branch("truthEta_r03", &m_data.truthEta_r03);

  m_tree->Branch("max_truthPt_r03", &m_data.max_truthPt_r03);
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
  double jet_eta_low{-1.152};
  double jet_eta_high{1.152};

  unsigned int bins_jet_phi{64};
  double jet_phi_low{0};
  double jet_phi_high{2*std::numbers::pi};

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

  h2JetPhiEta = new TH2F("h2Jet_PhiEta", "|z| < 10 cm and MB; #phi; #eta",
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

  unsigned int bins_cent_full = 100;
  double cent_full_low = -0.5;
  double cent_full_high = 99.5;

  h2SEPD_Psi2_data_mc_NS_all = new TH2F("h2SEPD_Psi2_data_mc_NS_all", "; 2#Psi_{2}; Centrality [%]",
                                        bins_psi, psi_low, psi_high,
                                        bins_cent_full, cent_full_low, cent_full_high);

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

  se->registerHisto(h2SEPD_Psi2_data_mc_NS_all);

  unsigned int bins_QQ_avg{2048};
  double QQ_avg_low{-0.01};
  double QQ_avg_high{0.01};

  unsigned int bins_qQ_avg{8192};
  double qQ_avg_low{-0.15};
  double qQ_avg_high{0.15};

  hRefFlow_data = new TProfile("hRefFlow_data", "Reference Flow; Centrality [%]; Re(#LTQ^{S}_{2} Q^{N*}_{2}#GT)",
                                m_bins_cent, m_cent_low, m_cent_high);

  hRefFlow_data_min = new TH1F("hRefFlow_data_min", "Min; Centrality [%]; Re(#LTQ^{S}_{2} Q^{N*}_{2}#GT)",
                               m_bins_cent / 10, m_cent_low, m_cent_high);

  hRefFlow_data_max = new TH1F("hRefFlow_data_max", "Max; Centrality [%]; Re(#LTQ^{S}_{2} Q^{N*}_{2}#GT)",
                               m_bins_cent / 10, m_cent_low, m_cent_high);

  hRefFlow_data_mc = new TProfile("hRefFlow_data_mc", "Reference Flow; Centrality [%]; Re(#LTQ^{S}_{2} Q^{N*}_{2}#GT)",
                                m_bins_cent, m_cent_low, m_cent_high);

  h2RefFlow_data = new TH2F("h2RefFlow_data", "Reference Flow; Centrality [%]; Re(Q^{S}_{2} Q^{N*}_{2})",
                            m_bins_cent / 10, m_cent_low, m_cent_high,
                            bins_QQ_avg, QQ_avg_low, QQ_avg_high);

  h2RefFlow_data_mc = new TH2F("h2RefFlow_data_mc", "Reference Flow; Centrality [%]; Re(Q^{S}_{2} Q^{N*}_{2})",
                            m_bins_cent / 10, m_cent_low, m_cent_high,
                            bins_QQ_avg, QQ_avg_low, QQ_avg_high);

  hScalarProduct_data = new TProfile("hScalarProduct_data", "Scalar Product; Centrality [%]; Re(#LTq_{2} Q^{S|N*}_{2}#GT)",
                                      m_bins_cent, m_cent_low, m_cent_high);

  hScalarProduct_data_min = new TH1F("hScalarProduct_data_min", "Min; Centrality [%]; Re(#LTq_{2} Q^{S|N*}_{2}#GT)",
                                      m_bins_cent / 10, m_cent_low, m_cent_high);

  hScalarProduct_data_max = new TH1F("hScalarProduct_data_max", "Max; Centrality [%]; Re(#LTq_{2} Q^{S|N*}_{2}#GT)",
                                      m_bins_cent / 10, m_cent_low, m_cent_high);

  hScalarProduct_data_mc = new TProfile("hScalarProduct_data_mc", "Scalar Product; Centrality [%]; Re(#LTq_{2} Q^{S|N*}_{2}#GT)",
                                      m_bins_cent, m_cent_low, m_cent_high);

  hScalarProduct_data_mc_anti = new TProfile("hScalarProduct_data_mc_anti", "Scalar Product; Centrality [%]; Re(#LTq_{2} Q^{N|S*}_{2}#GT)",
                                      m_bins_cent, m_cent_low, m_cent_high);

  h2ScalarProduct_data = new TH2F("h2ScalarProduct_data", "Scalar Product; Centrality [%]; Re(q_{2} Q^{S|N*}_{2})",
                                  m_bins_cent / 10, m_cent_low, m_cent_high,
                                  bins_qQ_avg, qQ_avg_low, qQ_avg_high);

  h2ScalarProduct_data_mc = new TH2F("h2ScalarProduct_data_mc", "Scalar Product; Centrality [%]; Re(q_{2} Q^{S|N*}_{2})",
                                  m_bins_cent / 10, m_cent_low, m_cent_high,
                                  bins_qQ_avg, qQ_avg_low, qQ_avg_high);

  se->registerHisto(hRefFlow_data);
  se->registerHisto(hRefFlow_data_mc);
  se->registerHisto(hRefFlow_data_min);
  se->registerHisto(hRefFlow_data_max);
  se->registerHisto(h2RefFlow_data);
  se->registerHisto(h2RefFlow_data_mc);
  se->registerHisto(hScalarProduct_data);
  se->registerHisto(hScalarProduct_data_mc);
  se->registerHisto(hScalarProduct_data_min);
  se->registerHisto(hScalarProduct_data_max);
  se->registerHisto(h2ScalarProduct_data);
  se->registerHisto(h2ScalarProduct_data_mc);
  se->registerHisto(hScalarProduct_data_mc_anti);

  int n_pt_bins = 8;
  double pt_bin_width = 5.0;
  double pt_min = 10.0;

  // Initialize the vectors for the pT-binned histograms
  h2ScalarProduct_data_ptbin.resize(n_pt_bins, nullptr);
  h2ScalarProduct_data_mc_ptbin.resize(n_pt_bins, nullptr);

  for (int i = 0; i < n_pt_bins; ++i)
  {
    double pt_low = pt_min + i * pt_bin_width;
    double pt_high = pt_low + pt_bin_width;

    std::string hname_data = std::format("h2ScalarProduct_data_pt_{}_{}", (int)pt_low, (int)pt_high);
    std::string htitle_data = std::format("Scalar Product ({} < p_{{T}} < {} GeV); Centrality [%]; Re(q_{{2}} Q^{{S|N*}}_{{2}})", pt_low, pt_high);

    h2ScalarProduct_data_ptbin[i] = new TH2F(hname_data.c_str(), htitle_data.c_str(),
                                             m_bins_cent / 10, m_cent_low, m_cent_high,
                                             bins_qQ_avg, qQ_avg_low, qQ_avg_high);
    se->registerHisto(h2ScalarProduct_data_ptbin[i]);

    std::string hname_data_mc = std::format("h2ScalarProduct_data_mc_pt_{}_{}", (int)pt_low, (int)pt_high);
    std::string htitle_data_mc = std::format("Scalar Product Data+MC ({} < p_{{T}} < {} GeV); Centrality [%]; Re(q_{{2}} Q^{{S|N*}}_{{2}})", pt_low, pt_high);

    h2ScalarProduct_data_mc_ptbin[i] = new TH2F(hname_data_mc.c_str(), htitle_data_mc.c_str(),
                                                m_bins_cent / 10, m_cent_low, m_cent_high,
                                                bins_qQ_avg, qQ_avg_low, qQ_avg_high);
    se->registerHisto(h2ScalarProduct_data_mc_ptbin[i]);
  }

  // setup TTree
  setup_tree();

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
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
    m_data.zvtx = vtx->get_z();
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

  m_data.centrality = centInfo->get_centile(CentralityInfo::PROP::mbd_NS) * 100;

  hEvent->Fill(static_cast<int>(EventType::ALL));

  if(std::abs(m_data.zvtx) < m_zvtx_max_v2)
  {
    hEvent->Fill(static_cast<int>(EventType::ZVTX50));

    if (std::abs(m_data.zvtx) < m_zvtx_max)
    {
      hEvent->Fill(static_cast<int>(EventType::ZVTX10));
      m_pass_Zvtx = true;
    }
  }

  // skip event if not minimum bias
  if (!mb_info->isAuAuMinimumBias())
  {
    ++m_ctr["events_isAuAuMinBias_fail"];
    return Fun4AllReturnCodes::EVENT_OK;
  }

  m_pass_MB = true;

  hZVertex->Fill(m_data.zvtx);
  h2ZVertexTruthvsData->Fill(m_data.zvtx, truth_z);
  h2ZVertexCentrality->Fill(m_data.zvtx, m_data.centrality);

  if (std::abs(m_data.zvtx) < m_zvtx_max_v2)
  {
    hCentralityZ50->Fill(m_data.centrality);

    if (std::abs(m_data.zvtx) > m_zvtx_max)
    {
      hCentralityZOuter->Fill(m_data.centrality);
    }
  }

  // skip event if zvtx is too large
  if (std::abs(m_data.zvtx) > m_zvtx_max)
  {
    ++m_ctr["events_zvtx_fail"];
    return Fun4AllReturnCodes::EVENT_OK;
  }

  hEvent->Fill(static_cast<int>(EventType::ZVTX10_MB));

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int sEPD_DataMC_Validation::process_centrality([[maybe_unused]] PHCompositeNode *topNode)
{
  if (!m_pass_MB || !m_pass_Zvtx)
  {
    return Fun4AllReturnCodes::EVENT_OK;
  }

  if (!std::isfinite(m_data.centrality) || m_data.centrality < 0 || m_data.centrality >= m_cent_max_threshold)
  {
    ++m_ctr["events_centrality_bad"];
    return Fun4AllReturnCodes::EVENT_OK;
  }

  m_pass_Centrality = true;

  hEvent->Fill(static_cast<int>(EventType::ZVTX10_MB_CENT));
  hCentrality->Fill(m_data.centrality);

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int sEPD_DataMC_Validation::process_calo(PHCompositeNode *topNode)
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
int sEPD_DataMC_Validation::process_sEPD(PHCompositeNode *topNode)
{
  if (!m_pass_MB || !m_pass_Zvtx || !m_pass_Centrality)
  {
    return Fun4AllReturnCodes::EVENT_OK;
  }

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
    return Fun4AllReturnCodes::EVENT_OK;
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

  h2SEPD_totalcharge_centrality->Fill(sepd_totalcharge, m_data.centrality);

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

  m_data.qsx_data = m_Q_data_S_2.first;
  m_data.qsy_data = m_Q_data_S_2.second;

  m_data.qnx_data = m_Q_data_N_2.first;
  m_data.qny_data = m_Q_data_N_2.second;

  m_Q_data_mc_S_2 = epd_data_mc_S->get_qvector(2);
  m_Q_data_mc_N_2 = epd_data_mc_N->get_qvector(2);

  m_data.qsx_data_mc = m_Q_data_mc_S_2.first;
  m_data.qsy_data_mc = m_Q_data_mc_S_2.second;

  m_data.qnx_data_mc = m_Q_data_mc_N_2.first;
  m_data.qny_data_mc = m_Q_data_mc_N_2.second;

  std::pair<double, double> Q_raw_data_S_2 = epd_data_S->get_qvector_raw(2);
  std::pair<double, double> Q_raw_data_N_2 = epd_data_N->get_qvector_raw(2);

  std::pair<double, double> Q_raw_data_mc_S_2 = epd_data_mc_S->get_qvector_raw(2);
  std::pair<double, double> Q_raw_data_mc_N_2 = epd_data_mc_N->get_qvector_raw(2);
  std::pair<double, double> Q_raw_data_mc_NS_2 = epd_data_mc_NS->get_qvector_raw(2);

  double _2psi2_data_S = 2*epd_data_S->get_shifted_psi(2);
  double _2psi2_data_N = 2*epd_data_N->get_shifted_psi(2);
  double _2psi2_data_NS = 2*epd_data_NS->get_shifted_psi(2);

  double _2psi2_data_mc_S = 2*epd_data_mc_S->get_shifted_psi(2);
  double _2psi2_data_mc_N = 2*epd_data_mc_N->get_shifted_psi(2);
  double _2psi2_data_mc_NS = 2*epd_data_mc_NS->get_shifted_psi(2);

  m_data.sepdpsi2_data = _2psi2_data_NS;
  m_data.sepdpsi2_data_mc = _2psi2_data_mc_NS;

  if (std::isfinite(m_data.centrality))
  {
    h2SEPD_Psi2_data_mc_NS_all->Fill(_2psi2_data_mc_NS, m_data.centrality);
  }

  if (!m_pass_MB || !m_pass_Zvtx || !m_pass_Centrality)
  {
    return Fun4AllReturnCodes::EVENT_OK;
  }

  double ref_flow_data = m_Q_data_S_2.first * m_Q_data_N_2.first + m_Q_data_S_2.second * m_Q_data_N_2.second;
  double ref_flow_data_mc = m_Q_data_mc_S_2.first * m_Q_data_mc_N_2.first + m_Q_data_mc_S_2.second * m_Q_data_mc_N_2.second;

  hRefFlow_data->Fill(m_data.centrality, ref_flow_data);

  int bin_cent = hRefFlow_data_min->FindBin(m_data.centrality);
  if(ref_flow_data > hRefFlow_data_max->GetBinContent(bin_cent))
  {
    hRefFlow_data_max->SetBinContent(bin_cent, ref_flow_data);
  }
  if(ref_flow_data < hRefFlow_data_min->GetBinContent(bin_cent))
  {
    hRefFlow_data_min->SetBinContent(bin_cent, ref_flow_data);
  }

  hRefFlow_data_mc->Fill(m_data.centrality, ref_flow_data_mc);

  h2RefFlow_data->Fill(m_data.centrality, ref_flow_data);
  h2RefFlow_data_mc->Fill(m_data.centrality, ref_flow_data_mc);

  double _2psi2_raw_data_S = 2*epd_data_S->GetPsi(Q_raw_data_S_2.first, Q_raw_data_S_2.second, 2);
  double _2psi2_raw_data_N = 2*epd_data_N->GetPsi(Q_raw_data_N_2.first, Q_raw_data_N_2.second, 2);

  double _2psi2_raw_data_mc_S = 2*epd_data_mc_S->GetPsi(Q_raw_data_mc_S_2.first, Q_raw_data_mc_S_2.second, 2);
  double _2psi2_raw_data_mc_N = 2*epd_data_mc_N->GetPsi(Q_raw_data_mc_N_2.first, Q_raw_data_mc_N_2.second, 2);
  double _2psi2_raw_data_mc_NS = 2*epd_data_mc_N->GetPsi(Q_raw_data_mc_NS_2.first, Q_raw_data_mc_NS_2.second, 2);

  h2SEPD_Psi2_data_S->Fill(_2psi2_data_S, m_data.centrality);
  h2SEPD_Psi2_data_N->Fill(_2psi2_data_N, m_data.centrality);
  h2SEPD_Psi2_data_NS->Fill(_2psi2_data_NS, m_data.centrality);

  h2SEPD_Psi2_data_mc_S->Fill(_2psi2_data_mc_S, m_data.centrality);
  h2SEPD_Psi2_data_mc_N->Fill(_2psi2_data_mc_N, m_data.centrality);
  h2SEPD_Psi2_data_mc_NS->Fill(_2psi2_data_mc_NS, m_data.centrality);

  h2SEPD_Psi2_raw_data_S->Fill(_2psi2_raw_data_S, m_data.centrality);
  h2SEPD_Psi2_raw_data_N->Fill(_2psi2_raw_data_N, m_data.centrality);

  h2SEPD_Psi2_raw_data_mc_S->Fill(_2psi2_raw_data_mc_S, m_data.centrality);
  h2SEPD_Psi2_raw_data_mc_N->Fill(_2psi2_raw_data_mc_N, m_data.centrality);
  h2SEPD_Psi2_raw_data_mc_NS->Fill(_2psi2_raw_data_mc_NS, m_data.centrality);

  return Fun4AllReturnCodes::EVENT_OK;
}

void sEPD_DataMC_Validation::fill_jets(JetContainer* jets, double max_eta, int &nJets, double &max_pt, std::vector<double> &pt_vec, std::vector<double> &e_vec, std::vector<double> &phi_vec, std::vector<double> &eta_vec) const
{
  for (auto *jet : *jets)
  {
    double pt = jet->get_pt();
    double e = jet->get_e();
    double phi = jet->get_phi();
    double eta = jet->get_eta();

    if (pt >= m_jet_pt_min && std::abs(eta) < max_eta)
    {
      pt_vec.push_back(pt);
      e_vec.push_back(e);
      phi_vec.push_back(phi);
      eta_vec.push_back(eta);

      max_pt = std::max(max_pt, pt);
      ++nJets;
    }
  }
}

//____________________________________________________________________________..
std::vector<float> sEPD_DataMC_Validation::computeUEAverages(TowerBackground *towerBkg)
{
  const int DET_LAYERS = 3;
  std::vector<float> results;
  results.reserve(DET_LAYERS);

  for (int i = 0; i < DET_LAYERS; ++i)
  {
    // Accessing via reference is critical for efficiency
    const std::vector<float> &UE_det = towerBkg->get_UE(i);

    if (UE_det.empty())
    {
      if (Verbosity() > 2) std::cout << "Layer " << i << " is empty.\n";
      results.push_back(0.0f);
      continue;
    }

    // std::reduce for SIMD-friendly summation
    float sum = std::reduce(UE_det.begin(), UE_det.end(), 0.0f);
    float avg = sum / static_cast<float>(UE_det.size());
    results.push_back(avg);

    if (Verbosity() > 2)
    {
      std::cout << "--- Layer " << i << " ---\n";
      std::cout << "Values: ";
      for (float val : UE_det)
      {
        std::cout << val << " ";
      }
      std::cout << "\nTotal Sum: " << sum
                << " | Count: " << UE_det.size()
                << " | Average: " << avg << "\n\n";
    }
  }

  return results;
}

//____________________________________________________________________________..
int sEPD_DataMC_Validation::process_jets(PHCompositeNode *topNode)
{
  if (!m_pass_MB || !m_pass_Zvtx || !m_pass_Centrality)
  {
    return Fun4AllReturnCodes::EVENT_OK;
  }

  JetContainer* jets_truth_r02 = findNode::getClass<JetContainer>(topNode, m_jet_truth_r02);
  JetContainer* jets_reco_r02 = findNode::getClass<JetContainer>(topNode, m_jet_reco_r02);

  JetContainer* jets_truth_r03 = findNode::getClass<JetContainer>(topNode, m_jet_truth_r03);
  JetContainer* jets_reco_r03 = findNode::getClass<JetContainer>(topNode, m_jet_reco_r03);

  if (!jets_truth_r02 || !jets_reco_r02 || !jets_truth_r03 || !jets_reco_r03)
  {
    return Fun4AllReturnCodes::ABORTRUN;
  }

  fill_jets(jets_truth_r02, m_jet_eta_max_r02, m_data.nJets_truth_r02, m_data.max_truthPt_r02, m_data.truthPt_r02, m_data.truthE_r02, m_data.truthPhi_r02, m_data.truthEta_r02);
  fill_jets(jets_reco_r02, m_jet_eta_max_r02, m_data.nJets_r02, m_data.max_pt_r02, m_data.pt_r02, m_data.e_r02, m_data.phi_r02, m_data.eta_r02);
  fill_jets(jets_truth_r03, m_jet_eta_max_r03, m_data.nJets_truth_r03, m_data.max_truthPt_r03, m_data.truthPt_r03, m_data.truthE_r03, m_data.truthPhi_r03, m_data.truthEta_r03);
  fill_jets(jets_reco_r03, m_jet_eta_max_r03, m_data.nJets_r03, m_data.max_pt_r03, m_data.pt_r03, m_data.e_r03, m_data.phi_r03, m_data.eta_r03);

  auto* towerBkg1 = findNode::getClass<TowerBackground>(topNode, "TowerInfoBackground_Sub1");
  auto* towerBkg = findNode::getClass<TowerBackground>(topNode, "TowerInfoBackground_Sub2"); // Second Iteration

  m_data.is_flow_failure = towerBkg1->get_flow_failure_flag() || towerBkg->get_flow_failure_flag();
  m_data.calo_v2 = towerBkg->get_v2();

  std::vector<float> ue_avg = computeUEAverages(towerBkg);
  m_data.ue_emcal_avg = ue_avg[0];
  m_data.ue_ihcal_avg = ue_avg[1];
  m_data.ue_ohcal_avg = ue_avg[2];

  double Q_data_S_x = m_Q_data_S_2.first;
  double Q_data_S_y = m_Q_data_S_2.second;

  double Q_data_N_x = m_Q_data_N_2.first;
  double Q_data_N_y = m_Q_data_N_2.second;

  double Q_data_mc_S_x = m_Q_data_mc_S_2.first;
  double Q_data_mc_S_y = m_Q_data_mc_S_2.second;

  double Q_data_mc_N_x = m_Q_data_mc_N_2.first;
  double Q_data_mc_N_y = m_Q_data_mc_N_2.second;

  bool hasJet = false;

  int bin_cent = hScalarProduct_data_min->FindBin(m_data.centrality);

  for (auto *jet : *jets_truth_r02)
  {
    double pt = jet->get_pt();
    double energy = jet->get_e();
    double phi = jet->get_phi();
    double eta = jet->get_eta();

    // map [-pi,pi] -> [0,2pi]
    if (phi < 0)
    {
      phi += 2.0 * std::numbers::pi;
    }

    if (pt >= m_jet_pt_min && energy > 0)
    {
      h2JetEtaVtxZ->Fill(m_data.zvtx, eta);

      if (std::abs(eta) < m_jet_eta_max_r02)
      {
        hasJet = true;

        hJetPt->Fill(pt);
        h2JetPhiEta->Fill(phi, eta);
        h2JetPtCentrality->Fill(pt, m_data.centrality);

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

        hScalarProduct_data->Fill(m_data.centrality, scalar_product_data);
        hScalarProduct_data_mc->Fill(m_data.centrality, scalar_product_data_mc);

        hScalarProduct_data_mc_anti->Fill(m_data.centrality, scalar_product_data_mc_anti);

        if (scalar_product_data > hScalarProduct_data_max->GetBinContent(bin_cent))
        {
          hScalarProduct_data_max->SetBinContent(bin_cent, scalar_product_data);
        }
        if (scalar_product_data < hScalarProduct_data_min->GetBinContent(bin_cent))
        {
          hScalarProduct_data_min->SetBinContent(bin_cent, scalar_product_data);
        }

        h2ScalarProduct_data->Fill(m_data.centrality, scalar_product_data);
        h2ScalarProduct_data_mc->Fill(m_data.centrality, scalar_product_data_mc);

        if (pt >= 10.0 && pt < 50.0)
        {
          int pt_bin = static_cast<int>((pt - 10.0) / 5.0);

          // Safety check to ensure the index is within the vector bounds
          if (pt_bin >= 0 && pt_bin < 8)
          {
            h2ScalarProduct_data_ptbin[pt_bin]->Fill(m_data.centrality, scalar_product_data);
            h2ScalarProduct_data_mc_ptbin[pt_bin]->Fill(m_data.centrality, scalar_product_data_mc);
          }
        }
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

  m_data.event = eventInfo->get_EvtSequence();

  if ((Verbosity() && m_ctr["events_total"] % 1000 == 0) || Verbosity() > 1)
  {
    std::cout << std::format("Progress: {}, Global: {}\n", m_ctr["events_total"], m_data.event);
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

  ret = process_calo(topNode);
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

  if (m_pass_Zvtx && m_pass_MB && m_pass_Centrality)
  {
    // Fill the TTree
    m_tree->Fill();

    ++m_ctr["events_good"];
  }
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int sEPD_DataMC_Validation::ResetEvent([[maybe_unused]] PHCompositeNode *topNode)
{
  m_data.event = 0;
  m_data.zvtx = -9999;
  m_data.centrality = -9999;
  m_data.calo_v2 = -9999;
  m_data.is_flow_failure = false;

  m_data.ue_emcal_avg = 0;
  m_data.ue_ihcal_avg = 0;
  m_data.ue_ohcal_avg = 0;

  m_data.emcal_energy = 0;
  m_data.ihcal_energy = 0;
  m_data.ohcal_energy = 0;

  m_data.qsx_data = 0;
  m_data.qsy_data = 0;
  m_data.qnx_data = 0;
  m_data.qny_data = 0;

  m_data.qsx_data_mc = 0;
  m_data.qsy_data_mc = 0;
  m_data.qnx_data_mc = 0;
  m_data.qny_data_mc = 0;

  m_data.sepdpsi2_data = -9999;
  m_data.sepdpsi2_data_mc = -9999;

  m_data.max_pt_r02 = 0;
  m_data.max_pt_r03 = 0;

  m_data.max_truthPt_r02 = 0;
  m_data.max_truthPt_r03 = 0;

  m_data.nJets_r02 = 0;
  m_data.nJets_truth_r02 = 0;

  m_data.nJets_r03 = 0;
  m_data.nJets_truth_r03 = 0;

  m_data.pt_r02.clear();
  m_data.e_r02.clear();
  m_data.phi_r02.clear();
  m_data.eta_r02.clear();

  m_data.truthPt_r02.clear();
  m_data.truthE_r02.clear();
  m_data.truthPhi_r02.clear();
  m_data.truthEta_r02.clear();

  m_data.pt_r03.clear();
  m_data.e_r03.clear();
  m_data.phi_r03.clear();
  m_data.eta_r03.clear();

  m_data.truthPt_r03.clear();
  m_data.truthE_r03.clear();
  m_data.truthPhi_r03.clear();
  m_data.truthEta_r03.clear();

  m_pass_Zvtx = false;
  m_pass_MB = false;
  m_pass_Centrality = false;
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

  // TTree
  m_output->cd();
  m_tree->Write();
  m_output->Close();

  return Fun4AllReturnCodes::EVENT_OK;
}
