#include "sEPDValidation.h"
#include "JetUtils.h"
#include "MedianFinder.h"

// -- c++
#include <format>
#include <iostream>
#include <sstream>

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

// -- Trigger
#include <ffarawobjects/Gl1Packet.h>

// -- Event Plane
#include <eventplaneinfo/EventplaneinfoMap.h>

// -- MBD
#include <mbd/MbdGeom.h>
#include <mbd/MbdPmtContainer.h>
#include <mbd/MbdPmtHit.h>

// -- sEPD
#include <epd/EpdGeom.h>

// -- jet
#include <jetbase/JetContainer.h>

// -- jetbackground
#include <jetbackground/TowerBackground.h>

#include <Math/Vector4D.h>

//____________________________________________________________________________..
sEPDValidation::sEPDValidation(const std::string &name)
  : SubsysReco(name)
{
}

// Helper function to create histograms
void sEPDValidation::create_histogram(const HistDef &def)
{
  switch (def.type)
  {
  case HistDef::Type::TH1:
    m_hists[def.name] = std::make_unique<TH1F>(def.name.c_str(), def.title.c_str(), def.x.bins, def.x.low, def.x.high);
    break;
  case HistDef::Type::TH2:
    m_hists[def.name] = std::make_unique<TH2F>(def.name.c_str(), def.title.c_str(), def.x.bins, def.x.low, def.x.high, def.y.bins, def.y.low, def.y.high);
    break;
  case HistDef::Type::TH3:
    m_hists[def.name] = std::make_unique<TH3F>(def.name.c_str(), def.title.c_str(), def.x.bins, def.x.low, def.x.high, def.y.bins, def.y.low, def.y.high, def.z.bins, def.z.low, def.z.high);
    break;
  case HistDef::Type::TProfile:
    m_hists[def.name] = std::make_unique<TProfile>(def.name.c_str(), def.title.c_str(), def.x.bins, def.x.low, def.x.high);
    break;
  case HistDef::Type::TProfile2D:
    m_hists[def.name] = std::make_unique<TProfile2D>(def.name.c_str(), def.title.c_str(), def.x.bins, def.x.low, def.x.high, def.y.bins, def.y.low, def.y.high);
    break;
  }
}

//____________________________________________________________________________..
int sEPDValidation::Init([[maybe_unused]] PHCompositeNode *topNode)
{
  Fun4AllServer *se = Fun4AllServer::instance();
  se->Print("NODETREE");

  // Centralized configuration list
  std::vector<HistDef> histogram_definitions = {
      // Event
      {HistDef::Type::TH1, "hEvent", "Event Type; Type; Events", {static_cast<unsigned int>(m_eventType.size()), 0, static_cast<double>(m_eventType.size())}},
      {HistDef::Type::TH1, "hEventMinBias", "Event Type; Type; Events", {static_cast<unsigned int>(m_MinBias_Type.size()), 0, static_cast<double>(m_MinBias_Type.size())}},
      {HistDef::Type::TH1, "hVtxZ", "Z Vertex; z [cm]; Events", {m_hist_config.m_bins_zvtx, m_hist_config.m_zvtx_low, m_hist_config.m_zvtx_high}},
      {HistDef::Type::TH1, "hVtxZ_MB", "Z Vertex; z [cm]; Events", {m_hist_config.m_bins_zvtx, m_hist_config.m_zvtx_low, m_hist_config.m_zvtx_high}},
      {HistDef::Type::TH1, "hCentrality", "Centrality; Centrality [%]; Events", {m_hist_config.m_bins_cent, m_hist_config.m_cent_low, m_hist_config.m_cent_high}},

      // Charge
      {HistDef::Type::TH2, "h2SEPD_Charge", "sEPD Charge: |z| < 10 cm and MB; sEPD Total Charge; Centrality [%]", {m_hist_config.m_bins_sepd_total_charge, m_hist_config.m_sepd_total_charge_low, m_hist_config.m_sepd_total_charge_high}, {m_hist_config.m_bins_cent, m_hist_config.m_cent_low, m_hist_config.m_cent_high}},

      {HistDef::Type::TProfile2D, "h2MBD_North_Charge", "MBD North Avg Charge: |z| < 10 cm and MB; #phi; #eta", {m_hist_config.m_bins_mbd_phi, m_hist_config.m_mbd_phi_low, m_hist_config.m_mbd_phi_high}, {m_hist_config.m_bins_mbd_eta, m_hist_config.m_mbd_eta_low, m_hist_config.m_mbd_eta_high}},
      {HistDef::Type::TProfile2D, "h2MBD_South_Charge", "MBD South Avg Charge: |z| < 10 cm and MB; #phi; -#eta", {m_hist_config.m_bins_mbd_phi, m_hist_config.m_mbd_phi_low, m_hist_config.m_mbd_phi_high}, {m_hist_config.m_bins_mbd_eta, m_hist_config.m_mbd_eta_low, m_hist_config.m_mbd_eta_high}},

      {HistDef::Type::TH2, "h2SEPD_Channel_Charge", "sEPD Channel Charge: |z| < 10 cm and MB; Channel; Charge", {m_hist_config.m_bins_sepd_channels, 0, static_cast<double>(m_hist_config.m_bins_sepd_channels)}, {m_hist_config.m_bins_sepd_channel_charge, m_hist_config.m_sepd_channel_charge_low, m_hist_config.m_sepd_channel_charge_high}},
      {HistDef::Type::TH2, "h2SEPD_Channel_Chargev2", "sEPD Channel Charge: |z| < 10 cm and MB; Channel; Charge", {m_hist_config.m_bins_sepd_channels, 0, static_cast<double>(m_hist_config.m_bins_sepd_channels)}, {m_hist_config.m_bins_sepd_channel_chargev2, m_hist_config.m_sepd_channel_chargev2_low, m_hist_config.m_sepd_channel_chargev2_high}},

      {HistDef::Type::TProfile, "hSEPD_Charge", "sEPD Channel Avg Charge: |z| < 10 cm and MB; Channel; Avg Charge", {m_hist_config.m_bins_sepd_channels, 0, static_cast<double>(m_hist_config.m_bins_sepd_channels)}},

      {HistDef::Type::TProfile2D, "h2SEPD_North_Charge", "SEPD North Avg Charge: |z| < 10 cm and MB; #phi; #eta", {m_hist_config.m_bins_sepd_phi, m_hist_config.m_sepd_phi_low, m_hist_config.m_sepd_phi_high}, {m_hist_config.m_bins_sepd_eta, m_hist_config.m_sepd_eta_low, m_hist_config.m_sepd_eta_high}},
      {HistDef::Type::TProfile2D, "h2SEPD_South_Charge", "SEPD South Avg Charge: |z| < 10 cm and MB; #phi; -#eta", {m_hist_config.m_bins_sepd_phi, m_hist_config.m_sepd_phi_low, m_hist_config.m_sepd_phi_high}, {m_hist_config.m_bins_sepd_eta, m_hist_config.m_sepd_eta_low, m_hist_config.m_sepd_eta_high}},

      {HistDef::Type::TProfile2D, "h2SEPD_North_ZS", "SEPD North Frac ZS: |z| < 10 cm and MB; #phi; #eta", {m_hist_config.m_bins_sepd_phi, m_hist_config.m_sepd_phi_low, m_hist_config.m_sepd_phi_high}, {m_hist_config.m_bins_sepd_eta, m_hist_config.m_sepd_eta_low, m_hist_config.m_sepd_eta_high}},
      {HistDef::Type::TProfile2D, "h2SEPD_South_ZS", "SEPD South Frac ZS: |z| < 10 cm and MB; #phi; -#eta", {m_hist_config.m_bins_sepd_phi, m_hist_config.m_sepd_phi_low, m_hist_config.m_sepd_phi_high}, {m_hist_config.m_bins_sepd_eta, m_hist_config.m_sepd_eta_low, m_hist_config.m_sepd_eta_high}},

      {HistDef::Type::TProfile2D, "h2SEPD_North_BelowThresh", "SEPD North Frac Charge < 0.2: |z| < 10 cm and MB; #phi; #eta", {m_hist_config.m_bins_sepd_phi, m_hist_config.m_sepd_phi_low, m_hist_config.m_sepd_phi_high}, {m_hist_config.m_bins_sepd_eta, m_hist_config.m_sepd_eta_low, m_hist_config.m_sepd_eta_high}},
      {HistDef::Type::TProfile2D, "h2SEPD_South_BelowThresh", "SEPD South Frac Charge < 0.2: |z| < 10 cm and MB; #phi; -#eta", {m_hist_config.m_bins_sepd_phi, m_hist_config.m_sepd_phi_low, m_hist_config.m_sepd_phi_high}, {m_hist_config.m_bins_sepd_eta, m_hist_config.m_sepd_eta_low, m_hist_config.m_sepd_eta_high}},

      {HistDef::Type::TProfile, "hSEPD_North_Charge", "SEPD North Avg Charge: |z| < 10 cm and MB; r_{bin}; Avg Charge", {m_hist_config.m_bins_sepd_rbin, m_hist_config.m_sepd_rbin_low, m_hist_config.m_sepd_rbin_high}},
      {HistDef::Type::TProfile, "hSEPD_South_Charge", "SEPD South Avg Charge: |z| < 10 cm and MB; r_{bin}; Avg Charge", {m_hist_config.m_bins_sepd_rbin, m_hist_config.m_sepd_rbin_low, m_hist_config.m_sepd_rbin_high}},

      {HistDef::Type::TH2, "h2MBD_Total_Charge", "MBD Total Charge: |z| < 10 cm and MB; MBD Total Charge; Centrality [%]", {m_hist_config.m_bins_mbd_total_charge, m_hist_config.m_mbd_total_charge_low, m_hist_config.m_mbd_total_charge_high}, {m_hist_config.m_bins_cent, m_hist_config.m_cent_low, m_hist_config.m_cent_high}},

      {HistDef::Type::TH3, "h3SEPD_Total_Charge", "sEPD Total Charge: |z| < 10 cm and MB; South; North; Centrality [%]", {m_hist_config.m_bins_sepd_charge, m_hist_config.m_sepd_charge_low, m_hist_config.m_sepd_charge_high}, {m_hist_config.m_bins_sepd_charge, m_hist_config.m_sepd_charge_low, m_hist_config.m_sepd_charge_high}, {m_hist_config.m_bins_cent, m_hist_config.m_cent_low, m_hist_config.m_cent_high}},
      {HistDef::Type::TH3, "h3MBD_Total_Charge", "MBD Total Charge: |z| < 10 cm and MB; South; North; Centrality [%]", {m_hist_config.m_bins_mbd_charge, m_hist_config.m_mbd_charge_low, m_hist_config.m_mbd_charge_high}, {m_hist_config.m_bins_mbd_charge, m_hist_config.m_mbd_charge_low, m_hist_config.m_mbd_charge_high}, {m_hist_config.m_bins_cent, m_hist_config.m_cent_low, m_hist_config.m_cent_high}},

      {HistDef::Type::TH3, "h3SEPD_MBD_Total_Charge", "sEPD vs MBD Total Charge: |z| < 10 cm and MB; MBD Total Charge; sEPD Total Charge; Centrality [%]", {m_hist_config.m_bins_mbd_charge, m_hist_config.m_mbd_charge_low, m_hist_config.m_mbd_charge_high}, {m_hist_config.m_bins_sepd_total_charge, m_hist_config.m_sepd_total_charge_low, m_hist_config.m_sepd_total_charge_high}, {m_hist_config.m_bins_cent, m_hist_config.m_cent_low, m_hist_config.m_cent_high}},

      // Calo
      {HistDef::Type::TProfile2D, "h2EMCal_Energy", "EMCal: |z| < 10 cm and MB; Tower Index #phi; Tower Index #eta", {CaloGeometry::HCAL_PHI_BINS, -0.5, CaloGeometry::HCAL_PHI_BINS - 0.5}, {CaloGeometry::HCAL_ETA_BINS, -0.5, CaloGeometry::HCAL_ETA_BINS - 0.5}},
      {HistDef::Type::TProfile2D, "h2IHCal_Energy", "IHCal: |z| < 10 cm and MB; Tower Index #phi; Tower Index #eta", {CaloGeometry::HCAL_PHI_BINS, -0.5, CaloGeometry::HCAL_PHI_BINS - 0.5}, {CaloGeometry::HCAL_ETA_BINS, -0.5, CaloGeometry::HCAL_ETA_BINS - 0.5}},
      {HistDef::Type::TProfile2D, "h2OHCal_Energy", "OHCal: |z| < 10 cm and MB; Tower Index #phi; Tower Index #eta", {CaloGeometry::HCAL_PHI_BINS, -0.5, CaloGeometry::HCAL_PHI_BINS - 0.5}, {CaloGeometry::HCAL_ETA_BINS, -0.5, CaloGeometry::HCAL_ETA_BINS - 0.5}},

      {HistDef::Type::TH3, "h3EMCal_MBD_sEPD", "|z| < 10 cm and MB; EMCal Total Energy [GeV]; MBD Total Charge; sEPD Total Charge", {m_hist_config.m_bins_Calo_E, m_hist_config.m_Calo_E_low, m_hist_config.m_Calo_E_high}, {m_hist_config.m_bins_mbd_charge, m_hist_config.m_mbd_charge_low, m_hist_config.m_mbd_charge_high}, {m_hist_config.m_bins_sepd_total_charge, m_hist_config.m_sepd_total_charge_low, m_hist_config.m_sepd_total_charge_high}},
      {HistDef::Type::TH3, "h3IHCal_MBD_sEPD", "|z| < 10 cm and MB; IHCal Total Energy [GeV]; MBD Total Charge; sEPD Total Charge", {m_hist_config.m_bins_Calo_E, m_hist_config.m_Calo_E_low, m_hist_config.m_Calo_E_high}, {m_hist_config.m_bins_mbd_charge, m_hist_config.m_mbd_charge_low, m_hist_config.m_mbd_charge_high}, {m_hist_config.m_bins_sepd_total_charge, m_hist_config.m_sepd_total_charge_low, m_hist_config.m_sepd_total_charge_high}},
      {HistDef::Type::TH3, "h3OHCal_MBD_sEPD", "|z| < 10 cm and MB; OHCal Total Energy [GeV]; MBD Total Charge; sEPD Total Charge", {m_hist_config.m_bins_Calo_E, m_hist_config.m_Calo_E_low, m_hist_config.m_Calo_E_high}, {m_hist_config.m_bins_mbd_charge, m_hist_config.m_mbd_charge_low, m_hist_config.m_mbd_charge_high}, {m_hist_config.m_bins_sepd_total_charge, m_hist_config.m_sepd_total_charge_low, m_hist_config.m_sepd_total_charge_high}},
      {HistDef::Type::TH3, "h3EMCal_IHCal_OHCal", "|z| < 10 cm and MB; EMCal Total Energy [GeV]; IHCal Total Energy [GeV]; OHCal Total Energy [GeV];", {m_hist_config.m_bins_Calo_E, m_hist_config.m_Calo_E_low, m_hist_config.m_Calo_E_high}, {m_hist_config.m_bins_Calo_E, m_hist_config.m_Calo_E_low, m_hist_config.m_Calo_E_high}, {m_hist_config.m_bins_Calo_E, m_hist_config.m_Calo_E_low, m_hist_config.m_Calo_E_high}},

      // Jets
      {HistDef::Type::TH3, "h3Jet_pT_Constituents", "Jet: |z| < 10 cm and MB; p_{T} [GeV]; Constituents; Centrality [%]", {m_hist_config.m_bins_jet_pt, m_hist_config.m_jet_pt_low, m_hist_config.m_jet_pt_high}, {m_hist_config.m_bins_jet_constituents, m_hist_config.m_jet_constituents_low, m_hist_config.m_jet_constituents_high}, {m_hist_config.m_bins_cent_reduced, m_hist_config.m_cent_low, m_hist_config.m_cent_high}},
      {HistDef::Type::TH3, "h3Jet_pT_Phi", "Jets: |z| < 10 cm and MB; p_{T} [GeV]; #phi; Centrality [%]", {m_hist_config.m_bins_jet_pt, m_hist_config.m_jet_pt_low, m_hist_config.m_jet_pt_high}, {m_hist_config.m_bins_jet_phi, m_hist_config.m_jet_phi_low, m_hist_config.m_jet_phi_high}, {m_hist_config.m_bins_cent_reduced, m_hist_config.m_cent_low, m_hist_config.m_cent_high}},
      {HistDef::Type::TH3, "h3Jet_pT_Energy", "Jets: |z| < 10 cm and MB; p_{T} [GeV]; Energy [GeV]; Centrality [%]", {m_hist_config.m_bins_jet_ptv2, m_hist_config.m_jet_ptv2_low, m_hist_config.m_jet_ptv2_high}, {m_hist_config.m_bins_jet_energy, m_hist_config.m_jet_energy_low, m_hist_config.m_jet_energy_high}, {m_hist_config.m_bins_cent_reduced, m_hist_config.m_cent_low, m_hist_config.m_cent_high}},
      {HistDef::Type::TH3, "h3Jet_PhiEta", "Jet: |z| < 10 cm and MB; #phi; #eta; Centrality [%]", {m_hist_config.m_bins_jet_phi, m_hist_config.m_jet_phi_low, m_hist_config.m_jet_phi_high}, {m_hist_config.m_bins_jet_eta, m_hist_config.m_jet_eta_low, m_hist_config.m_jet_eta_high}, {m_hist_config.m_bins_cent_reduced, m_hist_config.m_cent_low, m_hist_config.m_cent_high}},
      {HistDef::Type::TProfile, "hJet_nEvent", "Jet: |z| < 10 cm and MB; Centrality [%]; Average Jet [Counts]", {m_hist_config.m_bins_cent_reduced, m_hist_config.m_cent_low, m_hist_config.m_cent_high}},

      // UE
      {HistDef::Type::TH3, "h3UE", "UE: |z| < 10 cm and MB; v_{2}; Towers; Strips", {m_hist_config.m_bins_v2, m_hist_config.m_v2_low, m_hist_config.m_v2_high}, {m_hist_config.m_bins_nTowerUE, 0, static_cast<double>(m_hist_config.m_bins_nTowerUE)}, {m_hist_config.m_bins_nStripsUE, 0, static_cast<double>(m_hist_config.m_bins_nStripsUE)}},
      {HistDef::Type::TH3, "h3UE_Jet", "UE: |z| < 10 cm and MB; Jet p_{T}^{max} [GeV]; v_{2}; Centrality [%]", {m_hist_config.m_bins_jet_ptv2, m_hist_config.m_jet_ptv2_low, m_hist_config.m_jet_ptv2_high}, {m_hist_config.m_bins_v2, m_hist_config.m_v2_low, m_hist_config.m_v2_high}, {m_hist_config.m_bins_cent_reduced, m_hist_config.m_cent_low, m_hist_config.m_cent_high}},
      {HistDef::Type::TH3, "h3UE_SumE", "UE: |z| < 10 cm and MB; Sum E [GeV]; v_{2}; Centrality [%]", {m_hist_config.m_bins_sum_E, m_hist_config.m_sum_E_low, m_hist_config.m_sum_E_high}, {m_hist_config.m_bins_v2, m_hist_config.m_v2_low, m_hist_config.m_v2_high}, {m_hist_config.m_bins_cent_reduced, m_hist_config.m_cent_low, m_hist_config.m_cent_high}},
  };

  // Official
  if (m_do_ep)
  {
    histogram_definitions.emplace_back(HistDef{HistDef::Type::TH3, "h3SEPD_EventPlaneInfo_Psi_2", "sEPD #Psi (Order 2): |z| < 10 cm and MB; 2#Psi^{S}_{2}; 2#Psi^{N}_{2}; Centrality [%]", {m_hist_config.m_bins_psi, m_hist_config.m_psi_low, m_hist_config.m_psi_high}, {m_hist_config.m_bins_psi, m_hist_config.m_psi_low, m_hist_config.m_psi_high}, {m_hist_config.m_bins_cent, m_hist_config.m_cent_low, m_hist_config.m_cent_high}});
    histogram_definitions.emplace_back(HistDef{HistDef::Type::TH3, "h3SEPD_EventPlaneInfo_Psi_3", "sEPD #Psi (Order 3): |z| < 10 cm and MB; 3#Psi^{S}_{3}; 3#Psi^{N}_{3}; Centrality [%]", {m_hist_config.m_bins_psi, m_hist_config.m_psi_low, m_hist_config.m_psi_high}, {m_hist_config.m_bins_psi, m_hist_config.m_psi_low, m_hist_config.m_psi_high}, {m_hist_config.m_bins_cent, m_hist_config.m_cent_low, m_hist_config.m_cent_high}});
  }

  for (const auto &def : histogram_definitions)
  {
    create_histogram(def);
  }

  for (unsigned int i = 0; i < m_eventType.size(); ++i)
  {
    m_hists["hEvent"]->GetXaxis()->SetBinLabel(i + 1, m_eventType[i].c_str());
  }

  for (unsigned int i = 0; i < m_MinBias_Type.size(); ++i)
  {
    m_hists["hEventMinBias"]->GetXaxis()->SetBinLabel(i + 1, m_MinBias_Type[i].c_str());
  }

  m_hists["h2SEPD_Channel_Charge"]->Sumw2();
  m_hists["h2SEPD_Channel_Chargev2"]->Sumw2();

  // Check if Calib Q Vecs are present
  PdbParameterMap *pdb = getNode<PdbParameterMap>(topNode, "CalibQVec");
  if (pdb)
  {
    m_calib_Q = true;
    std::cout << "Calibrated Q Vec Present" << std::endl;
  }

  m_output = std::make_unique<TFile>(m_outtree_name.c_str(), "recreate");
  m_output->cd();

  // TTree
  m_tree = new TTree("T", "T");
  m_tree->SetDirectory(m_output.get());
  m_tree->Branch("event_id", &m_data.event_id, "event_id/I");
  m_tree->Branch("event_zvertex", &m_data.event_zvertex, "event_zvertex/D");
  m_tree->Branch("event_centrality", &m_data.event_centrality, "event_centrality/D");
  m_tree->Branch("event_MBD_Charge_South", &m_data.event_MBD_Charge_South);
  m_tree->Branch("event_MBD_Charge_North", &m_data.event_MBD_Charge_North);
  m_tree->Branch("event_sEPD_Charge_South", &m_data.event_sEPD_Charge_South);
  m_tree->Branch("event_sEPD_Charge_North", &m_data.event_sEPD_Charge_North);
  m_tree->Branch("event_EMCal_Energy", &m_data.event_EMCal_Energy);
  m_tree->Branch("event_IHCal_Energy", &m_data.event_IHCal_Energy);
  m_tree->Branch("event_OHCal_Energy", &m_data.event_OHCal_Energy);
  m_tree->Branch("event_tower_median_Energy", &m_data.event_tower_median_Energy);
  m_tree->Branch("event_EMCal_tower_median_Energy", &m_data.event_EMCal_tower_median_Energy);
  m_tree->Branch("sepd_channel", &m_data.sepd_channel);
  m_tree->Branch("sepd_charge", &m_data.sepd_charge);
  m_tree->Branch("sepd_phi", &m_data.sepd_phi);
  if (m_calib_Q)
  {
    m_tree->Branch("Q_S_x_2_raw", &m_data.Q_S_x_2_raw);
    m_tree->Branch("Q_S_y_2_raw", &m_data.Q_S_y_2_raw);
    m_tree->Branch("Q_N_x_2_raw", &m_data.Q_N_x_2_raw);
    m_tree->Branch("Q_N_y_2_raw", &m_data.Q_N_y_2_raw);

    m_tree->Branch("Q_S_x_2_recentered", &m_data.Q_S_x_2_recentered);
    m_tree->Branch("Q_S_y_2_recentered", &m_data.Q_S_y_2_recentered);
    m_tree->Branch("Q_N_x_2_recentered", &m_data.Q_N_x_2_recentered);
    m_tree->Branch("Q_N_y_2_recentered", &m_data.Q_N_y_2_recentered);

    m_tree->Branch("Q_S_x_2", &m_data.Q_S_x_2);
    m_tree->Branch("Q_S_y_2", &m_data.Q_S_y_2);
    m_tree->Branch("Q_N_x_2", &m_data.Q_N_x_2);
    m_tree->Branch("Q_N_y_2", &m_data.Q_N_y_2);

    m_tree->Branch("UE_sum_E", &m_data.UE_sum_E);
    m_tree->Branch("calo_v2", &m_data.calo_v2);
    m_tree->Branch("calo_v2_it1", &m_data.calo_v2_it1);
    m_tree->Branch("nStripsCEMC", &m_data.nStripsCEMC);
    m_tree->Branch("nHIRecoSeedsSub", &m_data.nHIRecoSeedsSub);
    m_tree->Branch("nHIRecoSeedsSubIt1", &m_data.nHIRecoSeedsSubIt1);
  }
  // m_tree->Branch("mbd_charge", &m_data.mbd_charge);
  // m_tree->Branch("mbd_phi", &m_data.mbd_phi);
  // m_tree->Branch("mbd_eta", &m_data.mbd_eta);
  m_tree->Branch("max_jet_pt", &m_data.max_jet_pt);
  m_tree->Branch("jet_pt", &m_data.jet_pt);
  m_tree->Branch("jet_energy", &m_data.jet_energy);
  m_tree->Branch("jet_phi", &m_data.jet_phi);
  m_tree->Branch("jet_eta", &m_data.jet_eta);

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int sEPDValidation::process_event_check(PHCompositeNode *topNode)
{
  m_hists["hEvent"]->Fill(static_cast<std::uint8_t>(EventType::ALL));

  // zvertex
  m_zvtx = -9999;
  GlobalVertexMap *vertexmap = getNode<GlobalVertexMap>(topNode, "GlobalVertexMap");

  if (!vertexmap)
  {
    return Fun4AllReturnCodes::ABORTRUN;
  }

  if (!vertexmap->empty())
  {
    GlobalVertex *vtx = vertexmap->begin()->second;
    m_zvtx = vtx->get_z();
    m_data.event_zvertex = m_zvtx;
  }

  m_hists["hVtxZ"]->Fill(m_zvtx);

  if (fabs(m_zvtx) < m_cuts.m_zvtx_max)
  {
    m_hists["hEvent"]->Fill(static_cast<std::uint8_t>(EventType::ZVTX10));
  }

  MinimumBiasInfo *m_mb_info = getNode<MinimumBiasInfo>(topNode, "MinimumBiasInfo");
  if (!m_mb_info)
  {
    return Fun4AllReturnCodes::ABORTRUN;
  }

  // Minimum Bias Check
  PdbParameterMap *pdb = getNode<PdbParameterMap>(topNode, "MinBiasParams");
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

  if (fabs(m_zvtx) < m_cuts.m_zvtx_max)
  {
    if (minbias_bkg_high)
    {
      m_hists["hEventMinBias"]->Fill(static_cast<std::uint8_t>(MinBiasType::BKG_HIGH));
    }
    if (minbias_side_hit_low)
    {
      m_hists["hEventMinBias"]->Fill(static_cast<std::uint8_t>(MinBiasType::SIDE_HIT_LOW));
    }
    if (minbias_zdc_low)
    {
      m_hists["hEventMinBias"]->Fill(static_cast<std::uint8_t>(MinBiasType::ZDC_LOW));
    }
    if (minbias_mbd_high)
    {
      m_hists["hEventMinBias"]->Fill(static_cast<std::uint8_t>(MinBiasType::MBD_HIGH));
    }
  }

  // skip event if not minimum bias
  if (!m_mb_info->isAuAuMinimumBias())
  {
    ++m_ctr["process_eventCheck_isAuAuMinBias_fail"];
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  m_hists["hVtxZ_MB"]->Fill(m_zvtx);

  // skip event if zvtx is too large
  if (fabs(m_zvtx) >= m_cuts.m_zvtx_max)
  {
    ++m_ctr["process_eventCheck_zvtx_large"];
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  m_hists["hEvent"]->Fill(static_cast<std::uint8_t>(EventType::ZVTX10_MB));
  m_hists["hCentrality"]->Fill(m_cent);

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int sEPDValidation::process_centrality(PHCompositeNode *topNode)
{
  CentralityInfo *centInfo = getNode<CentralityInfo>(topNode, "CentralityInfo");
  if (!centInfo)
  {
    return Fun4AllReturnCodes::ABORTRUN;
  }

  m_cent = centInfo->get_centile(CentralityInfo::PROP::mbd_NS) * 100;
  m_data.event_centrality = m_cent;

  // skip event if centrality is too peripheral
  if (!std::isfinite(m_cent) || m_cent >= m_cuts.m_cent_max)
  {
    ++m_ctr["process_eventCheck_centrality_large"];
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  JetUtils::update_min_max(m_cent, m_logging.m_cent_min, m_logging.m_cent_max);

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int sEPDValidation::process_MBD(PHCompositeNode *topNode)
{
  MbdPmtContainer *mbd_container = getNode<MbdPmtContainer>(topNode, "MbdPmtContainer");
  if (!mbd_container)
  {
    return Fun4AllReturnCodes::ABORTRUN;
  }

  MbdGeom *mbdgeom = getNode<MbdGeom>(topNode, "MbdGeom");
  if (!mbdgeom)
  {
    return Fun4AllReturnCodes::ABORTRUN;
  }

  PdbParameterMap *pdb = getNode<PdbParameterMap>(topNode, "MinBiasParams");
  if (!pdb)
  {
    return Fun4AllReturnCodes::ABORTRUN;
  }

  PHParameters pdb_params("MinBiasParams");
  pdb_params.FillFrom(pdb);

  double mbd_total_charge_south = pdb_params.get_double_param("minbias_mbd_total_charge_south");
  double mbd_total_charge_north = pdb_params.get_double_param("minbias_mbd_total_charge_north");
  double vertex_scale = pdb_params.get_double_param("minbias_vertex_scale");
  double centrality_scale = pdb_params.get_double_param("minbias_centrality_scale");

  m_data.event_MBD_Charge_South = mbd_total_charge_south;
  m_data.event_MBD_Charge_North = mbd_total_charge_north;

  m_mbd_total_charge = mbd_total_charge_south + mbd_total_charge_north;

  MinimumBiasClassifier mb;

  int mbd_channels = mbd_container->get_npmt();

  for (int i = 0; i < mbd_channels; ++i)
  {
    MbdPmtHit *mbd_pmt = mbd_container->get_pmt(i);
    bool pass = mb.passesHitCut(mbd_pmt);
    if (!pass)
    {
      continue;
    }

    // geom
    double mbd_ch_r = mbdgeom->get_r(i);
    double mbd_ch_z = mbdgeom->get_z(i);
    ROOT::Math::XYZTVector vec(mbd_ch_r, 0, mbd_ch_z, 0);
    double mbd_ch_phi = mbdgeom->get_phi(i);
    double mbd_ch_eta = vec.Eta();
    int mbd_arm = mbdgeom->get_arm(i);

    double charge = mbd_pmt->get_q() * vertex_scale * centrality_scale;

    // m_data.mbd_charge.push_back(charge);
    // m_data.mbd_phi.push_back(mbd_ch_phi);
    // m_data.mbd_eta.push_back(mbd_ch_eta);

    if (mbd_arm == 0)
    {
      dynamic_cast<TProfile2D *>(m_hists["h2MBD_South_Charge"].get())->Fill(mbd_ch_phi, -mbd_ch_eta, charge);
    }
    if (mbd_arm == 1)
    {
      dynamic_cast<TProfile2D *>(m_hists["h2MBD_North_Charge"].get())->Fill(mbd_ch_phi, mbd_ch_eta, charge);
    }

    JetUtils::update_min_max(mbd_ch_r, m_logging.m_mbd_ch_r_min, m_logging.m_mbd_ch_r_max);
    JetUtils::update_min_max(mbd_ch_z, m_logging.m_mbd_ch_z_min, m_logging.m_mbd_ch_z_max);
    JetUtils::update_min_max(mbd_ch_phi, m_logging.m_mbd_ch_phi_min, m_logging.m_mbd_ch_phi_max);
    if (mbd_ch_eta > 0)
    {
      JetUtils::update_min_max(mbd_ch_eta, m_logging.m_mbd_ch_eta_min, m_logging.m_mbd_ch_eta_max);
    }
    JetUtils::update_min_max(charge, m_logging.m_mbd_ch_charge_min, m_logging.m_mbd_ch_charge_max);
  }

  dynamic_cast<TH2 *>(m_hists["h2MBD_Total_Charge"].get())->Fill(m_mbd_total_charge, m_cent);
  dynamic_cast<TH3 *>(m_hists["h3MBD_Total_Charge"].get())->Fill(mbd_total_charge_south, mbd_total_charge_north, m_cent);

  JetUtils::update_min_max(m_mbd_total_charge, m_logging.m_mbd_total_charge_min, m_logging.m_mbd_total_charge_max);
  JetUtils::update_min_max(mbd_total_charge_south, m_logging.m_mbd_charge_min, m_logging.m_mbd_charge_max);
  JetUtils::update_min_max(mbd_total_charge_north, m_logging.m_mbd_charge_min, m_logging.m_mbd_charge_max);
  JetUtils::update_min_max(vertex_scale, m_logging.m_vertex_scale_min, m_logging.m_vertex_scale_max);
  JetUtils::update_min_max(centrality_scale, m_logging.m_centrality_scale_min, m_logging.m_centrality_scale_max);

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int sEPDValidation::process_sEPD(PHCompositeNode *topNode)
{
  TowerInfoContainer *towerinfosEPD = getNode<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_SEPD");
  if (!towerinfosEPD)
  {
    return Fun4AllReturnCodes::ABORTRUN;
  }

  EpdGeom *epdgeom = getNode<EpdGeom>(topNode, "TOWERGEOM_EPD");
  if (!epdgeom)
  {
    return Fun4AllReturnCodes::ABORTRUN;
  }

  // sepd
  unsigned int nchannels_epd = towerinfosEPD->size();

  double sepd_total_charge_south = 0;
  double sepd_total_charge_north = 0;

  for (unsigned int channel = 0; channel < nchannels_epd; ++channel)
  {
    unsigned int key = TowerInfoDefs::encode_epd(channel);

    TowerInfo *tower = towerinfosEPD->get_tower_at_channel(channel);

    double charge = tower->get_energy();
    bool isZS = tower->get_isZS();
    double sepd_ch_z = epdgeom->get_z(key);
    double sepd_ch_r = epdgeom->get_r(key);
    ROOT::Math::XYZTVector vec(sepd_ch_r, 0, sepd_ch_z, 0);
    double eta = vec.Eta();
    double phi = epdgeom->get_phi(key);
    int rbin = TowerInfoDefs::get_epd_rbin(key);

    // Ensure phi is in [-pi, pi]
    if (phi > M_PI)
    {
      phi -= 2 * M_PI;
    }

    // exclude ZS
    if (isZS)
    {
      ++m_ctr["sepd_tower_zs"];
      if (eta < 0)
      {
        dynamic_cast<TProfile2D *>(m_hists["h2SEPD_South_ZS"].get())->Fill(phi, -eta, 1);
      }
      else
      {
        dynamic_cast<TProfile2D *>(m_hists["h2SEPD_North_ZS"].get())->Fill(phi, eta, 1);
      }
      continue;
    }

    if (eta < 0)
    {
      dynamic_cast<TProfile2D *>(m_hists["h2SEPD_South_ZS"].get())->Fill(phi, -eta, 0);
    }
    else
    {
      dynamic_cast<TProfile2D *>(m_hists["h2SEPD_North_ZS"].get())->Fill(phi, eta, 0);
    }

    JetUtils::update_min_max(charge, m_logging.m_sepd_charge_min, m_logging.m_sepd_charge_max);

    dynamic_cast<TH2*>(m_hists["h2SEPD_Channel_Charge"].get())->Fill(channel, charge);
    dynamic_cast<TH2*>(m_hists["h2SEPD_Channel_Chargev2"].get())->Fill(channel, charge);

    // expecting Nmips
    if (charge < m_cuts.m_sepd_charge_threshold)
    {
      ++m_ctr["sepd_tower_charge_below_threshold"];

      if (eta < 0)
      {
        dynamic_cast<TProfile2D *>(m_hists["h2SEPD_South_BelowThresh"].get())->Fill(phi, -eta, 1);
      }
      else
      {
        dynamic_cast<TProfile2D *>(m_hists["h2SEPD_North_BelowThresh"].get())->Fill(phi, eta, 1);
      }
      continue;
    }

    if (eta < 0)
    {
      dynamic_cast<TProfile2D *>(m_hists["h2SEPD_South_BelowThresh"].get())->Fill(phi, -eta, 0);
    }
    else
    {
      dynamic_cast<TProfile2D *>(m_hists["h2SEPD_North_BelowThresh"].get())->Fill(phi, eta, 0);
    }

    m_data.sepd_channel.push_back(channel);
    m_data.sepd_charge.push_back(charge);
    m_data.sepd_phi.push_back(phi);

    JetUtils::update_min_max(sepd_ch_z, m_logging.m_sepd_z_min, m_logging.m_sepd_z_max);
    JetUtils::update_min_max(sepd_ch_r, m_logging.m_sepd_r_min, m_logging.m_sepd_r_max);
    JetUtils::update_min_max(phi, m_logging.m_sepd_phi_min, m_logging.m_sepd_phi_max);
    JetUtils::update_min_max(rbin, m_logging.m_sepd_rbin_min, m_logging.m_sepd_rbin_max);
    if (eta > 0)
    {
      JetUtils::update_min_max(eta, m_logging.m_sepd_eta_min, m_logging.m_sepd_eta_max);
    }

    // sepd charge sums
    unsigned int arm = TowerInfoDefs::get_epd_arm(key);
    dynamic_cast<TProfile *>(m_hists["hSEPD_Charge"].get())->Fill(channel, charge);

    std::string detector = (arm == 0) ? "South" : "North";
    double& sepd_total_charge = (arm == 0) ? sepd_total_charge_south : sepd_total_charge_north;

    sepd_total_charge += charge;
    dynamic_cast<TProfile2D *>(m_hists[std::format("h2SEPD_{}_Charge", detector)].get())->Fill(phi, -eta, charge);
    dynamic_cast<TProfile *>(m_hists[std::format("hSEPD_{}_Charge", detector)].get())->Fill(rbin, charge);
  }

  m_data.event_sEPD_Charge_South = sepd_total_charge_south;
  m_data.event_sEPD_Charge_North = sepd_total_charge_north;

  double sepd_total_charge = sepd_total_charge_south + sepd_total_charge_north;

  dynamic_cast<TH2 *>(m_hists["h2SEPD_Charge"].get())->Fill(sepd_total_charge, m_cent);
  dynamic_cast<TH3 *>(m_hists["h3SEPD_MBD_Total_Charge"].get())->Fill(m_mbd_total_charge, sepd_total_charge, m_cent);

  // ensure both total charges are nonzero
  if (sepd_total_charge_south == 0 || sepd_total_charge_north == 0)
  {
    ++m_ctr["process_sEPD_total_charge_zero"];
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  dynamic_cast<TH3 *>(m_hists["h3SEPD_Total_Charge"].get())->Fill(sepd_total_charge_south, sepd_total_charge_north, m_cent);

  JetUtils::update_min_max(sepd_total_charge_south, m_logging.m_sepd_total_charge_south_min, m_logging.m_sepd_total_charge_south_max);
  JetUtils::update_min_max(sepd_total_charge_north, m_logging.m_sepd_total_charge_north_min, m_logging.m_sepd_total_charge_north_max);

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int sEPDValidation::process_Calo(PHCompositeNode *topNode)
{
  auto* towersCEMC  = getNode<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_CEMC_RETOWER");
  auto* towersIHCal = getNode<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALIN");
  auto* towersOHCal = getNode<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALOUT");

  if (!towersCEMC || !towersIHCal || !towersOHCal)
  {
    return Fun4AllReturnCodes::ABORTRUN;
  }

  size_t nTowersCEMC = towersCEMC->size();
  size_t nTowersIHCal = towersIHCal->size();
  size_t nTowersOHCal = towersOHCal->size();

  if(nTowersCEMC != nTowersIHCal || nTowersCEMC != nTowersOHCal)
  {
    std::cout << std::format("Calo Contains Missing Towers!, Event: {}\n", m_data.event_id);
    return Fun4AllReturnCodes::ABORTRUN;
  }

  auto* h2EMCal_Energy = dynamic_cast<TProfile2D *>(m_hists["h2EMCal_Energy"].get());
  auto* h2IHCal_Energy = dynamic_cast<TProfile2D *>(m_hists["h2IHCal_Energy"].get());
  auto* h2OHCal_Energy = dynamic_cast<TProfile2D *>(m_hists["h2OHCal_Energy"].get());

  MedianFinder mf;
  MedianFinder mf_EMCal;

  for (unsigned int towerIndex = 0; towerIndex < towersCEMC->size(); ++towerIndex)
  {
    unsigned int key = TowerInfoDefs::encode_hcal(towerIndex);
    unsigned int iphi = TowerInfoDefs::getCaloTowerPhiBin(key);
    unsigned int ieta = TowerInfoDefs::getCaloTowerEtaBin(key);

    auto* towerCEMC = towersCEMC->get_tower_at_channel(towerIndex);
    if(towerCEMC->get_isGood())
    {
      float energy = towerCEMC->get_energy();
      m_data.event_EMCal_Energy += energy;
      mf.addNum(energy);
      mf_EMCal.addNum(energy);
      h2EMCal_Energy->Fill(iphi, ieta, energy);
    }

    auto* towerIHCal = towersIHCal->get_tower_at_channel(towerIndex);
    if(towerIHCal->get_isGood())
    {
      float energy = towerIHCal->get_energy();
      m_data.event_IHCal_Energy += energy;
      mf.addNum(energy);
      h2IHCal_Energy->Fill(iphi, ieta, energy);
    }

    auto* towerOHCal = towersOHCal->get_tower_at_channel(towerIndex);
    if(towerOHCal->get_isGood())
    {
      float energy = towerOHCal->get_energy();
      m_data.event_OHCal_Energy += energy;
      mf.addNum(energy);
      h2OHCal_Energy->Fill(iphi, ieta, energy);
    }
  }

  double total_MBD = m_data.event_MBD_Charge_South + m_data.event_MBD_Charge_North;
  double total_sEPD = m_data.event_sEPD_Charge_South + m_data.event_sEPD_Charge_North;
  double total_EMCal = m_data.event_EMCal_Energy;
  double total_IHCal = m_data.event_IHCal_Energy;
  double total_OHCal = m_data.event_OHCal_Energy;

  // Get the median tower energy
  m_data.event_tower_median_Energy = mf.findMedian();
  m_data.event_EMCal_tower_median_Energy = mf_EMCal.findMedian();

  dynamic_cast<TH3 *>(m_hists["h3EMCal_MBD_sEPD"].get())->Fill(total_EMCal, total_MBD, total_sEPD);
  dynamic_cast<TH3 *>(m_hists["h3IHCal_MBD_sEPD"].get())->Fill(total_IHCal, total_MBD, total_sEPD);
  dynamic_cast<TH3 *>(m_hists["h3OHCal_MBD_sEPD"].get())->Fill(total_OHCal, total_MBD, total_sEPD);
  dynamic_cast<TH3 *>(m_hists["h3EMCal_IHCal_OHCal"].get())->Fill(total_EMCal, total_IHCal, total_OHCal);

  JetUtils::update_min_max(m_data.event_EMCal_Energy, m_logging.m_EMCal_Energy_min, m_logging.m_EMCal_Energy_max);
  JetUtils::update_min_max(m_data.event_IHCal_Energy, m_logging.m_IHCal_Energy_min, m_logging.m_IHCal_Energy_max);
  JetUtils::update_min_max(m_data.event_OHCal_Energy, m_logging.m_OHCal_Energy_min, m_logging.m_OHCal_Energy_max);

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
void sEPDValidation::process_CalibQVec(PHCompositeNode *topNode)
{
  PdbParameterMap *pdb = getNode<PdbParameterMap>(topNode, "CalibQVec");

  PHParameters pdb_params("CalibQVec");
  pdb_params.FillFrom(pdb);

  m_data.Q_S_x_2_raw = pdb_params.get_double_param("Q_S_x_2_raw");
  m_data.Q_S_y_2_raw = pdb_params.get_double_param("Q_S_y_2_raw");
  m_data.Q_N_x_2_raw = pdb_params.get_double_param("Q_N_x_2_raw");
  m_data.Q_N_y_2_raw = pdb_params.get_double_param("Q_N_y_2_raw");

  m_data.Q_S_x_2_recentered = pdb_params.get_double_param("Q_S_x_2_recentered");
  m_data.Q_S_y_2_recentered = pdb_params.get_double_param("Q_S_y_2_recentered");
  m_data.Q_N_x_2_recentered = pdb_params.get_double_param("Q_N_x_2_recentered");
  m_data.Q_N_y_2_recentered = pdb_params.get_double_param("Q_N_y_2_recentered");

  m_data.Q_S_x_2 = pdb_params.get_double_param("Q_S_x_2");
  m_data.Q_S_y_2 = pdb_params.get_double_param("Q_S_y_2");
  m_data.Q_N_x_2 = pdb_params.get_double_param("Q_N_x_2");
  m_data.Q_N_y_2 = pdb_params.get_double_param("Q_N_y_2");
}

//____________________________________________________________________________..
int sEPDValidation::process_EventPlane(Eventplaneinfo *epd_S, Eventplaneinfo *epd_N, int order)
{
  std::pair<double, double> epdsouthQn = epd_S->get_qvector(order);
  std::pair<double, double> epdnorthQn = epd_N->get_qvector(order);

  double Qx_south = epdsouthQn.first;
  double Qy_south = epdsouthQn.second;

  double Qx_north = epdnorthQn.first;
  double Qy_north = epdnorthQn.second;

  // Compute Q vector magnitude
  double Q_mag_south = std::sqrt((Qx_south * Qx_south) + (Qy_south * Qy_south));
  double Q_mag_north = std::sqrt((Qx_north * Qx_north) + (Qy_north * Qy_north));

  // ensure the magnitude of both Q vectors are non-zero
  if (Q_mag_south == 0 || Q_mag_north == 0)
  {
    ++m_ctr["process_EventPlane_Q_mag_zero"];
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  double psi_n_south = std::atan2(Qy_south, Qx_south);
  double psi_n_north = std::atan2(Qy_north, Qx_north);

  m_logging.m_psi_min = std::min(m_logging.m_psi_min, std::min(psi_n_south, psi_n_north));
  m_logging.m_psi_max = std::max(m_logging.m_psi_max, std::max(psi_n_south, psi_n_north));

  dynamic_cast<TH3 *>(m_hists["h3SEPD_EventPlaneInfo_Psi_" + std::to_string(order)].get())->Fill(psi_n_south, psi_n_north, m_cent);

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int sEPDValidation::process_EventPlane(PHCompositeNode *topNode)
{
  // get event plane map
  EventplaneinfoMap *epmap = getNode<EventplaneinfoMap>(topNode, "EventplaneinfoMap");
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

  // process order 2
  int ret = process_EventPlane(epd_S, epd_N, 2);
  if (ret)
  {
    return ret;
  }

  // process order 3
  ret = process_EventPlane(epd_S, epd_N, 3);
  if (ret)
  {
    return ret;
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int sEPDValidation::process_jets(PHCompositeNode *topNode)
{
  JetContainer *jets = findNode::getClass<JetContainer>(topNode, m_recoJetName);
  if (!jets)
  {
    return Fun4AllReturnCodes::ABORTRUN;
  }

  int n_jets = 0;
  bool hasJet = false;
  bool hasHighPt = false;

  for (auto* jet : *jets)
  {
    double pt = jet->get_pt();
    double energy = jet->get_e();
    double phi = jet->get_phi();
    double eta = jet->get_eta();

    Jet::TYPE_comp_vec comp_vec = jet->get_comp_vec();
    int constituents = comp_vec.size();

    if (pt >= m_jet_pt_min_cut && fabs(eta) < m_jet_eta_max_cut)
    {
      JetUtils::update_min_max(pt, m_logging.m_jet_pt_min, m_logging.m_jet_pt_max);
      JetUtils::update_min_max(energy, m_logging.m_jet_energy_min, m_logging.m_jet_energy_max);
      JetUtils::update_min_max(phi, m_logging.m_jet_phi_min, m_logging.m_jet_phi_max);
      JetUtils::update_min_max(eta, m_logging.m_jet_eta_min, m_logging.m_jet_eta_max);
      JetUtils::update_min_max(constituents, m_logging.m_jet_constituents_min, m_logging.m_jet_constituents_max);

      m_data.jet_pt.push_back(pt);
      m_data.jet_energy.push_back(energy);
      m_data.jet_phi.push_back(phi);
      m_data.jet_eta.push_back(eta);

      dynamic_cast<TH3 *>(m_hists["h3Jet_pT_Constituents"].get())->Fill(pt, constituents, m_cent);
      dynamic_cast<TH3 *>(m_hists["h3Jet_pT_Phi"].get())->Fill(pt, phi, m_cent);
      dynamic_cast<TH3 *>(m_hists["h3Jet_pT_Energy"].get())->Fill(pt, energy, m_cent);
      dynamic_cast<TH3 *>(m_hists["h3Jet_PhiEta"].get())->Fill(phi, eta, m_cent);

      if (pt >= m_cuts.m_jet_pt_threshold)
      {
        hasHighPt = true;
      }

      m_data.max_jet_pt = std::max(m_data.max_jet_pt, pt);
      hasJet = true;
      ++n_jets;
    }
  }

  if (hasJet)
  {
    m_hists["hEvent"]->Fill(static_cast<std::uint8_t>(EventType::ZVTX10_MB_JET));

    dynamic_cast<TProfile *>(m_hists["hJet_nEvent"].get())->Fill(m_cent, n_jets);

    JetUtils::update_min_max(n_jets, m_logging.m_jet_nEvent_min, m_logging.m_jet_nEvent_max);

    if (hasHighPt)
    {
      std::cout << std::format("High pT Jet. Event: {}, pT: {}\n", m_data.event_id, m_data.max_jet_pt);
    }
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int sEPDValidation::process_UE(PHCompositeNode *topNode)
{
  auto* towerBkg1 = getNode<TowerBackground>(topNode, "TowerInfoBackground_Sub1"); // First Iteration
  auto* towerBkg = getNode<TowerBackground>(topNode, "TowerInfoBackground_Sub2"); // Second Iteration

  if (!towerBkg1 || !towerBkg)
  {
    return Fun4AllReturnCodes::ABORTRUN;
  }

  float v2 = towerBkg->get_v2();
  float v2_it1 = towerBkg1->get_v2();
  float sum_E = towerBkg->get_sum_E();
  int nTowers = towerBkg->get_nTowersUsedForBkg();
  int nStrips = towerBkg->get_nStripsUsedForFlow();
  int nStripsCEMC = towerBkg->get_nStripsCEMCUsedForFlow();
  int nHIRecoSeedsSub = towerBkg->get_nHIRecoSeedsSub();
  int nHIRecoSeedsSubIt1 = towerBkg1->get_nHIRecoSeedsSub();

  m_data.calo_v2 = v2;
  m_data.calo_v2_it1 = v2_it1;
  m_data.UE_sum_E = sum_E;
  m_data.nStripsCEMC = nStripsCEMC;
  m_data.nHIRecoSeedsSub = nHIRecoSeedsSub;
  m_data.nHIRecoSeedsSubIt1 = nHIRecoSeedsSubIt1;

  dynamic_cast<TH3 *>(m_hists["h3UE"].get())->Fill(v2, nTowers, nStrips);
  dynamic_cast<TH3 *>(m_hists["h3UE_Jet"].get())->Fill(m_data.max_jet_pt, v2, m_data.event_centrality);
  dynamic_cast<TH3 *>(m_hists["h3UE_SumE"].get())->Fill(sum_E, v2, m_data.event_centrality);

  JetUtils::update_min_max(v2, m_logging.m_UE_calo_v2_min, m_logging.m_UE_calo_v2_max);
  JetUtils::update_min_max(v2_it1, m_logging.m_UE_calo_v2_min, m_logging.m_UE_calo_v2_max);
  JetUtils::update_min_max(sum_E, m_logging.m_UE_sum_E_min, m_logging.m_UE_sum_E_max);

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int sEPDValidation::process_event(PHCompositeNode *topNode)
{
  EventHeader *eventInfo = getNode<EventHeader>(topNode, "EventHeader");
  if (!eventInfo)
  {
    return Fun4AllReturnCodes::ABORTRUN;
  }

  int m_globalEvent = eventInfo->get_EvtSequence();
  m_data.event_id = m_globalEvent;
  // int m_run         = eventInfo->get_RunNumber();

  if (m_event % 20 == 0)
  {
    std::cout << "Progress: " << m_event << ", Global: " << m_globalEvent << std::endl;
  }
  ++m_event;

  int ret = process_centrality(topNode);
  if (ret)
  {
    return ret;
  }

  ret = process_event_check(topNode);
  if (ret)
  {
    return ret;
  }

  ret = process_jets(topNode);
  if (ret)
  {
    return ret;
  }

  ret = process_MBD(topNode);
  if (ret)
  {
    return ret;
  }

  ret = process_sEPD(topNode);
  if (ret)
  {
    return ret;
  }

  ret = process_Calo(topNode);
  if (ret)
  {
    return ret;
  }

  if (m_calib_Q)
  {
    process_CalibQVec(topNode);

    ret = process_UE(topNode);
    if (ret)
    {
      return ret;
    }
  }

  if (m_do_ep)
  {
    ret = process_EventPlane(topNode);
    if (ret)
    {
      return ret;
    }
  }

  // Fill the TTree
  m_tree->Fill();

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int sEPDValidation::ResetEvent([[maybe_unused]] PHCompositeNode *topNode)
{
  ++m_ctr["event_reset"];

  // Event
  m_data.event_id = -1;
  m_data.event_zvertex = 9999;
  m_data.event_centrality = 9999;
  m_data.event_MBD_Charge_South = 9999;
  m_data.event_MBD_Charge_North = 9999;
  m_data.event_sEPD_Charge_South = 9999;
  m_data.event_sEPD_Charge_North = 9999;
  m_data.max_jet_pt = -9999;

  // sEPD
  m_data.sepd_channel.clear();
  m_data.sepd_charge.clear();
  m_data.sepd_phi.clear();

  // Calo
  m_data.event_EMCal_Energy = 0;
  m_data.event_IHCal_Energy = 0;
  m_data.event_OHCal_Energy = 0;
  m_data.event_tower_median_Energy = -9999;
  m_data.event_EMCal_tower_median_Energy = -9999;

  // MBD
  // m_data.mbd_charge.clear();
  // m_data.mbd_phi.clear();
  // m_data.mbd_eta.clear();

  // Jets
  m_data.jet_pt.clear();
  m_data.jet_energy.clear();
  m_data.jet_phi.clear();
  m_data.jet_eta.clear();

  // Q Vec
  m_data.Q_S_x_2_raw = 0;
  m_data.Q_S_y_2_raw = 0;
  m_data.Q_N_x_2_raw = 0;
  m_data.Q_N_y_2_raw = 0;

  m_data.Q_S_x_2_recentered = 0;
  m_data.Q_S_y_2_recentered = 0;
  m_data.Q_N_x_2_recentered = 0;
  m_data.Q_N_y_2_recentered = 0;

  m_data.Q_S_x_2 = 0;
  m_data.Q_S_y_2 = 0;
  m_data.Q_N_x_2 = 0;
  m_data.Q_N_y_2 = 0;

  // UE
  m_data.calo_v2 = 9999;
  m_data.calo_v2_it1 = 9999;
  m_data.UE_sum_E = 9999;
  m_data.nStripsCEMC = 9999;
  m_data.nHIRecoSeedsSub = 0;
  m_data.nHIRecoSeedsSubIt1 = 0;

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int sEPDValidation::End([[maybe_unused]] PHCompositeNode *topNode)
{
  std::cout << "sEPDValidation::End" << std::endl;

  std::cout << std::format("{:#<20}\n", "");
  std::cout << "stats" << std::endl;
  std::cout << "Cent: Min: " << m_logging.m_cent_min << ", Max: " << m_logging.m_cent_max << std::endl;
  std::cout << "sEPD Charge: Min: " << m_logging.m_sepd_charge_min << ", Max: " << m_logging.m_sepd_charge_max << std::endl;
  std::cout << "sEPD Total Charge South: Min: " << m_logging.m_sepd_total_charge_south_min << ", Max: " << m_logging.m_sepd_total_charge_south_max << std::endl;
  std::cout << "sEPD Total Charge North: Min " << m_logging.m_sepd_total_charge_north_min << ", Max: " << m_logging.m_sepd_total_charge_north_max << std::endl;
  std::cout << "sEPD z: Min " << m_logging.m_sepd_z_min << ", Max: " << m_logging.m_sepd_z_max << std::endl;
  std::cout << "sEPD r: Min " << m_logging.m_sepd_r_min << ", Max: " << m_logging.m_sepd_r_max << std::endl;
  std::cout << "sEPD rbin: Min " << m_logging.m_sepd_rbin_min << ", Max: " << m_logging.m_sepd_rbin_max << std::endl;
  std::cout << "sEPD Phi: Min " << m_logging.m_sepd_phi_min << ", Max: " << m_logging.m_sepd_phi_max << std::endl;
  std::cout << "sEPD Eta: Min " << m_logging.m_sepd_eta_min << ", Max: " << m_logging.m_sepd_eta_max << std::endl;
  std::cout << std::format("{:#<20}\n", "");
  std::cout << "sEPD" << std::endl;
  std::cout << "Avg towers ZS: " << m_ctr["sepd_tower_zs"] / m_hists["hEvent"]->GetBinContent(static_cast<std::uint8_t>(EventType::ZVTX10_MB) + 1) << std::endl;
  std::cout << "Avg towers with charge below threshold: " << m_ctr["sepd_tower_charge_below_threshold"] / m_hists["hEvent"]->GetBinContent(static_cast<std::uint8_t>(EventType::ZVTX10_MB) + 1) << std::endl;
  std::cout << std::format("{:#<20}\n", "");
  std::cout << "MBD" << std::endl;
  std::cout << "Mbd Total Charge: Min: " << m_logging.m_mbd_total_charge_min << ", Max: " << m_logging.m_mbd_total_charge_max << std::endl;
  std::cout << "Mbd Total Charge (North / South): Min: " << m_logging.m_mbd_charge_min << ", Max: " << m_logging.m_mbd_charge_max << std::endl;
  std::cout << "Mbd Vertex Scale Factor: Min: " << m_logging.m_vertex_scale_min << ", Max: " << m_logging.m_vertex_scale_max << std::endl;
  std::cout << "Mbd Centrality Scale Factor: Min: " << m_logging.m_centrality_scale_min << ", Max: " << m_logging.m_centrality_scale_max << std::endl;
  std::cout << "Mbd Channel r: Min: " << m_logging.m_mbd_ch_r_min << ", Max: " << m_logging.m_mbd_ch_r_max << std::endl;
  std::cout << "Mbd Channel z: Min: " << m_logging.m_mbd_ch_z_min << ", Max: " << m_logging.m_mbd_ch_z_max << std::endl;
  std::cout << "Mbd Channel phi: Min: " << m_logging.m_mbd_ch_phi_min << ", Max: " << m_logging.m_mbd_ch_phi_max << std::endl;
  std::cout << "Mbd Channel eta: Min: " << m_logging.m_mbd_ch_eta_min << ", Max: " << m_logging.m_mbd_ch_eta_max << std::endl;
  std::cout << "Mbd Channel charge: Min: " << m_logging.m_mbd_ch_charge_min << ", Max: " << m_logging.m_mbd_ch_charge_max << std::endl;
  std::cout << std::format("{:#<20}\n", "");
  std::cout << "Calo" << std::endl;
  std::cout << std::format("EMCal: Min {:0.2f}, Max: {:0.2f}\n", m_logging.m_EMCal_Energy_min, m_logging.m_EMCal_Energy_max);
  std::cout << std::format("IHCal: Min {:0.2f}, Max: {:0.2f}\n", m_logging.m_IHCal_Energy_min, m_logging.m_IHCal_Energy_max);
  std::cout << std::format("OHCal: Min {:0.2f}, Max: {:0.2f}\n", m_logging.m_OHCal_Energy_min, m_logging.m_OHCal_Energy_max);
  std::cout << std::format("{:#<20}\n", "");
  std::cout << "Jets" << std::endl;
  std::cout << std::format("Jet pT: Min: {:0.2f}, Max: {:0.2f}\n", m_logging.m_jet_pt_min, m_logging.m_jet_pt_max);
  std::cout << std::format("Jet energy: Min {:0.2f}, Max: {:0.2f}\n", m_logging.m_jet_energy_min, m_logging.m_jet_energy_max);
  std::cout << std::format("Jet phi: Min: {:0.2f}, Max: {:0.2f}\n", m_logging.m_jet_phi_min, m_logging.m_jet_phi_max);
  std::cout << std::format("Jet eta: Min: {:0.2f}, Max: {:0.2f}\n", m_logging.m_jet_eta_min, m_logging.m_jet_eta_max);
  std::cout << std::format("Jet constituents: Min {:0.2f}, Max: {:0.2f}\n", m_logging.m_jet_constituents_min, m_logging.m_jet_constituents_max);
  std::cout << std::format("Jets per Event: Min {}, Max: {}\n", m_logging.m_jet_nEvent_min, m_logging.m_jet_nEvent_max);
  std::cout << std::format("{:#<20}\n", "");
  std::cout << "Abort Events Types" << std::endl;
  std::cout << std::format("process event, Reset Event Calls : {}", m_ctr["event_reset"]) << std::endl;
  std::cout << std::format("process event, isAuAuMinBias Fail: {}", m_ctr["process_eventCheck_isAuAuMinBias_fail"]) << std::endl;
  std::cout << std::format("process event, |z| >= {} cm: {}", m_cuts.m_zvtx_max, m_ctr["process_eventCheck_zvtx_large"]) << std::endl;
  std::cout << std::format("process event, Centrality >= {}%: {}", m_cuts.m_cent_max, m_ctr["process_eventCheck_centrality_large"]) << std::endl;
  std::cout << "process sEPD, total charge zero: " << m_ctr["process_sEPD_total_charge_zero"] << std::endl;
  if (m_do_ep)
  {
    std::cout << std::format("process Event Plane, Q vec mag zero: {}", m_ctr["process_EventPlane_Q_mag_zero"]) << std::endl;
    std::cout << std::format("process Event Plane, ep map empty: {}", m_ctr["process_EventPlane_epmap_empty"]) << std::endl;
    std::cout << std::format("process Event Plane, epd invalid: {}", m_ctr["process_EventPlane_epd_invalid"]) << std::endl;
    std::cout << "process Event Plane, Psi: Min " << m_logging.m_psi_min << ", Max: " << m_logging.m_psi_max << std::endl;
    std::cout << "process Event Plane, Q mag zero: " << m_ctr["process_EventPlane_Q_mag_zero"] << std::endl;
    std::cout << "process Event Plane, epmap empty: " << m_ctr["process_EventPlane_epmap_empty"] << std::endl;
    std::cout << "process Event Plane, epd invalid: " << m_ctr["process_EventPlane_epd_invalid"] << std::endl;
  }

  // UE
  if (m_calib_Q)
  {
    std::cout << std::format("{:#<20}\n", "");
    std::cout << "UE" << std::endl;
    std::cout << std::format("Calo V2: Min {:0.2f}, Max: {:0.2f}\n", m_logging.m_UE_calo_v2_min, m_logging.m_UE_calo_v2_max);
    std::cout << std::format("Sum E: Min {:0.2f}, Max: {:0.2f}\n", m_logging.m_UE_sum_E_min, m_logging.m_UE_sum_E_max);
  }

  std::cout << std::format("{:#<20}\n", "");
  std::cout << "Events" << std::endl;
  for (unsigned int i = 0; i < m_eventType.size(); ++i)
  {
    std::cout << m_eventType[i] << ": " << m_hists["hEvent"]->GetBinContent(i + 1) << std::endl;
  }
  std::cout << std::format("{:#<20}\n", "");

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

  auto project_and_write = [&](const std::string &hist_name, const std::string &projection)
  {
    auto *hist = m_hists[hist_name].get();
    if (auto *h3 = dynamic_cast<TH3 *>(hist))
    {
      std::cout << std::format("Saving: {}, Projection: {}\n", hist_name, projection);
      h3->Project3D(projection.c_str())->Write();
    }
    else if (auto *h2 = dynamic_cast<TH2 *>(hist))
    {
      std::cout << std::format("Saving: {}, Projection: {}\n", hist_name, projection);
      h2->ProjectionX()->Write();
    }
  };

  // local
  if (!m_condor_mode)
  {
    project_and_write("h2SEPD_Charge", "x");
    project_and_write("h2MBD_Total_Charge", "x");
    project_and_write("h3SEPD_Total_Charge", "yx");
    project_and_write("h3MBD_Total_Charge", "yx");

    // Jets
    project_and_write("h3Jet_pT_Constituents", "x");
    project_and_write("h3Jet_pT_Constituents", "yx");
    project_and_write("h3Jet_pT_Phi", "yx");
    project_and_write("h3Jet_pT_Energy", "yx");
    project_and_write("h3Jet_PhiEta", "yx");

    // Calo
    project_and_write("h3EMCal_MBD_sEPD", "yx");
    project_and_write("h3EMCal_MBD_sEPD", "zx");
    project_and_write("h3IHCal_MBD_sEPD", "yx");
    project_and_write("h3IHCal_MBD_sEPD", "zx");
    project_and_write("h3OHCal_MBD_sEPD", "yx");
    project_and_write("h3OHCal_MBD_sEPD", "zx");
    project_and_write("h3EMCal_IHCal_OHCal", "yx");
    project_and_write("h3EMCal_IHCal_OHCal", "zx");

    // UE
    project_and_write("h3UE", "x");
    project_and_write("h3UE", "yx");
    project_and_write("h3UE", "zx");

    project_and_write("h3UE_Jet", "xy");
    project_and_write("h3UE_Jet", "yz");
    project_and_write("h3UE_Jet", "xz");

    project_and_write("h3UE_SumE", "yx");
    project_and_write("h3UE_SumE", "xz");

    // Official
    if (m_do_ep)
    {
      project_and_write("h3SEPD_EventPlaneInfo_Psi_2", "yx");
      project_and_write("h3SEPD_EventPlaneInfo_Psi_3", "yx");
    }
  }
  output.Close();

  // TTree
  m_output->cd();
  m_tree->Write();
  m_output->Close();

  return Fun4AllReturnCodes::EVENT_OK;
}
