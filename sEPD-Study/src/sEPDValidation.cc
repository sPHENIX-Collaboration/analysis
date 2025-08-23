#include "sEPDValidation.h"
#include "JetUtils.h"

// -- c++
#include <iostream>
#include <sstream>
#include <format>

// -- event
#include <ffaobjects/EventHeader.h>

// -- Fun4All
#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/Fun4AllServer.h>

// -- Nodes
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

// -- flags
#include <phparameter/PHParameters.h>
#include <pdbcalbase/PdbParameterMap.h>

// -- Calo
#include <calobase/TowerInfo.h>
#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfoDefs.h>

// -- Vtx
#include <globalvertex/GlobalVertex.h>
#include <globalvertex/GlobalVertexMap.h>

// -- MB
#include <calotrigger/MinimumBiasInfo.h>
#include <calotrigger/MinimumBiasClassifier.h>
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

#include <TFile.h>
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
      {HistDef::Type::TProfile2D, "h2MBD_South_Charge", "MBD South Avg Charge: |z| < 10 cm and MB; #phi; #eta", {m_hist_config.m_bins_mbd_phi, m_hist_config.m_mbd_phi_low, m_hist_config.m_mbd_phi_high}, {m_hist_config.m_bins_mbd_eta, -m_hist_config.m_mbd_eta_high, m_hist_config.m_mbd_eta_low}},
      {HistDef::Type::TH2, "h2MBD_Total_Charge", "MBD Total Charge: |z| < 10 cm and MB; MBD Total Charge; Centrality [%]", {m_hist_config.m_bins_mbd_total_charge, m_hist_config.m_mbd_total_charge_low, m_hist_config.m_mbd_total_charge_high}, {m_hist_config.m_bins_cent, m_hist_config.m_cent_low, m_hist_config.m_cent_high}},
      {HistDef::Type::TH3, "h3SEPD_Total_Charge", "sEPD Total Charge: |z| < 10 cm and MB; South; North; Centrality [%]", {m_hist_config.m_bins_sepd_charge, m_hist_config.m_sepd_charge_low, m_hist_config.m_sepd_charge_high}, {m_hist_config.m_bins_sepd_charge, m_hist_config.m_sepd_charge_low, m_hist_config.m_sepd_charge_high}, {m_hist_config.m_bins_cent, m_hist_config.m_cent_low, m_hist_config.m_cent_high}},
      {HistDef::Type::TH3, "h3MBD_Total_Charge", "MBD Total Charge: |z| < 10 cm and MB; South; North; Centrality [%]", {m_hist_config.m_bins_mbd_charge, m_hist_config.m_mbd_charge_low, m_hist_config.m_mbd_charge_high}, {m_hist_config.m_bins_mbd_charge, m_hist_config.m_mbd_charge_low, m_hist_config.m_mbd_charge_high}, {m_hist_config.m_bins_cent, m_hist_config.m_cent_low, m_hist_config.m_cent_high}},
      {HistDef::Type::TH3, "h3SEPD_MBD_Total_Charge", "sEPD vs MBD Total Charge: |z| < 10 cm and MB; MBD Total Charge; sEPD Total Charge; Centrality [%]", {m_hist_config.m_bins_mbd_charge, m_hist_config.m_mbd_charge_low, m_hist_config.m_mbd_charge_high}, {m_hist_config.m_bins_sepd_total_charge, m_hist_config.m_sepd_total_charge_low, m_hist_config.m_sepd_total_charge_high}, {m_hist_config.m_bins_cent, m_hist_config.m_cent_low, m_hist_config.m_cent_high}},

      // Q-vector: 1st Order Correction
      {HistDef::Type::TH3, "h3SEPD_Q_S_2", "sEPD South Q (Order 2): |z| < 10 cm and MB; Q_{x}; Q_{y}; Centrality [%]", {m_hist_config.m_bins_sepd_Q, m_hist_config.m_sepd_Q_low, m_hist_config.m_sepd_Q_high}, {m_hist_config.m_bins_sepd_Q, m_hist_config.m_sepd_Q_low, m_hist_config.m_sepd_Q_high}, {m_hist_config.m_bins_cent, m_hist_config.m_cent_low, m_hist_config.m_cent_high}},
      {HistDef::Type::TH3, "h3SEPD_Q_N_2", "sEPD North Q (Order 2): |z| < 10 cm and MB; Q_{x}; Q_{y}; Centrality [%]", {m_hist_config.m_bins_sepd_Q, m_hist_config.m_sepd_Q_low, m_hist_config.m_sepd_Q_high}, {m_hist_config.m_bins_sepd_Q, m_hist_config.m_sepd_Q_low, m_hist_config.m_sepd_Q_high}, {m_hist_config.m_bins_cent, m_hist_config.m_cent_low, m_hist_config.m_cent_high}},
      {HistDef::Type::TH3, "h3SEPD_Q_S_3", "sEPD South Q (Order 3): |z| < 10 cm and MB; Q_{x}; Q_{y}; Centrality [%]", {m_hist_config.m_bins_sepd_Q, m_hist_config.m_sepd_Q_low, m_hist_config.m_sepd_Q_high}, {m_hist_config.m_bins_sepd_Q, m_hist_config.m_sepd_Q_low, m_hist_config.m_sepd_Q_high}, {m_hist_config.m_bins_cent, m_hist_config.m_cent_low, m_hist_config.m_cent_high}},
      {HistDef::Type::TH3, "h3SEPD_Q_N_3", "sEPD North Q (Order 3): |z| < 10 cm and MB; Q_{x}; Q_{y}; Centrality [%]", {m_hist_config.m_bins_sepd_Q, m_hist_config.m_sepd_Q_low, m_hist_config.m_sepd_Q_high}, {m_hist_config.m_bins_sepd_Q, m_hist_config.m_sepd_Q_low, m_hist_config.m_sepd_Q_high}, {m_hist_config.m_bins_cent, m_hist_config.m_cent_low, m_hist_config.m_cent_high}},

      {HistDef::Type::TH3, "h3SEPD_Psi_2", "sEPD #Psi (Order 2): |z| < 10 cm and MB; 2#Psi^{S}_{2}; 2#Psi^{N}_{2}; Centrality [%]", {m_hist_config.m_bins_psi, m_hist_config.m_psi_low, m_hist_config.m_psi_high}, {m_hist_config.m_bins_psi, m_hist_config.m_psi_low, m_hist_config.m_psi_high}, {m_hist_config.m_bins_cent, m_hist_config.m_cent_low, m_hist_config.m_cent_high}},
      {HistDef::Type::TH3, "h3SEPD_Psi_3", "sEPD #Psi (Order 3): |z| < 10 cm and MB; 3#Psi^{S}_{3}; 3#Psi^{N}_{3}; Centrality [%]", {m_hist_config.m_bins_psi, m_hist_config.m_psi_low, m_hist_config.m_psi_high}, {m_hist_config.m_bins_psi, m_hist_config.m_psi_low, m_hist_config.m_psi_high}, {m_hist_config.m_bins_cent, m_hist_config.m_cent_low, m_hist_config.m_cent_high}}
  };

  // Official
  if(m_do_ep)
  {
    histogram_definitions.emplace_back(HistDef{HistDef::Type::TH3, "h3SEPD_EventPlaneInfo_Psi_2", "sEPD #Psi (Order 2): |z| < 10 cm and MB; 2#Psi^{S}_{2}; 2#Psi^{N}_{2}; Centrality [%]", {m_hist_config.m_bins_psi, m_hist_config.m_psi_low, m_hist_config.m_psi_high}, {m_hist_config.m_bins_psi, m_hist_config.m_psi_low, m_hist_config.m_psi_high}, {m_hist_config.m_bins_cent, m_hist_config.m_cent_low, m_hist_config.m_cent_high}});
    histogram_definitions.emplace_back(HistDef{HistDef::Type::TH3, "h3SEPD_EventPlaneInfo_Psi_3", "sEPD #Psi (Order 3): |z| < 10 cm and MB; 3#Psi^{S}_{3}; 3#Psi^{N}_{3}; Centrality [%]", {m_hist_config.m_bins_psi, m_hist_config.m_psi_low, m_hist_config.m_psi_high}, {m_hist_config.m_bins_psi, m_hist_config.m_psi_low, m_hist_config.m_psi_high}, {m_hist_config.m_bins_cent, m_hist_config.m_cent_low, m_hist_config.m_cent_high}});
  }

  for (const auto &def : histogram_definitions)
  {
    create_histogram(def);
  }


  m_hists["h3SEPD_Q_S_2"]->Sumw2();
  m_hists["h3SEPD_Q_N_2"]->Sumw2();
  m_hists["h3SEPD_Q_S_3"]->Sumw2();
  m_hists["h3SEPD_Q_N_3"]->Sumw2();

  // Q-vector: 2nd Order Correction
  m_hists["h3SEPD_Q_S_xx_yy_2"] = std::make_unique<TH3F>("h3SEPD_Q_S_xx_yy_2", "sEPD South Q (Order 2): |z| < 10 cm and MB; Q_{xx}; Q_{yy}; Centrality [%]", m_hist_config.m_bins_sepd_Q, m_hist_config.m_sepd_Q_low, m_hist_config.m_sepd_Q_high, m_hist_config.m_bins_sepd_Q, m_hist_config.m_sepd_Q_low, m_hist_config.m_sepd_Q_high, m_hist_config.m_bins_cent, m_hist_config.m_cent_low, m_hist_config.m_cent_high);
  m_hists["h3SEPD_Q_N_xx_yy_2"] = std::make_unique<TH3F>("h3SEPD_Q_N_xx_yy_2", "sEPD North Q (Order 2): |z| < 10 cm and MB; Q_{xx}; Q_{yy}; Centrality [%]", m_hist_config.m_bins_sepd_Q, m_hist_config.m_sepd_Q_low, m_hist_config.m_sepd_Q_high, m_hist_config.m_bins_sepd_Q, m_hist_config.m_sepd_Q_low, m_hist_config.m_sepd_Q_high, m_hist_config.m_bins_cent, m_hist_config.m_cent_low, m_hist_config.m_cent_high);
  m_hists["h3SEPD_Q_xy_2"] = std::make_unique<TH3F>("h3SEPD_Q_xy_2", "sEPD Q xy (Order 2): |z| < 10 cm and MB; Q^{S}_{xy}; Q^{N}_{xy}; Centrality [%]", m_hist_config.m_bins_sepd_Q, m_hist_config.m_sepd_Q_low, m_hist_config.m_sepd_Q_high, m_hist_config.m_bins_sepd_Q, m_hist_config.m_sepd_Q_low, m_hist_config.m_sepd_Q_high, m_hist_config.m_bins_cent, m_hist_config.m_cent_low, m_hist_config.m_cent_high);

  m_hists["h3SEPD_Q_S_xx_yy_3"] = std::make_unique<TH3F>("h3SEPD_Q_S_xx_yy_3", "sEPD South Q (Order 3): |z| < 10 cm and MB; Q_{xx}; Q_{yy}; Centrality [%]", m_hist_config.m_bins_sepd_Q, m_hist_config.m_sepd_Q_low, m_hist_config.m_sepd_Q_high, m_hist_config.m_bins_sepd_Q, m_hist_config.m_sepd_Q_low, m_hist_config.m_sepd_Q_high, m_hist_config.m_bins_cent, m_hist_config.m_cent_low, m_hist_config.m_cent_high);
  m_hists["h3SEPD_Q_N_xx_yy_3"] = std::make_unique<TH3F>("h3SEPD_Q_N_xx_yy_3", "sEPD North Q (Order 3): |z| < 10 cm and MB; Q_{xx}; Q_{yy}; Centrality [%]", m_hist_config.m_bins_sepd_Q, m_hist_config.m_sepd_Q_low, m_hist_config.m_sepd_Q_high, m_hist_config.m_bins_sepd_Q, m_hist_config.m_sepd_Q_low, m_hist_config.m_sepd_Q_high, m_hist_config.m_bins_cent, m_hist_config.m_cent_low, m_hist_config.m_cent_high);
  m_hists["h3SEPD_Q_xy_3"] = std::make_unique<TH3F>("h3SEPD_Q_xy_3", "sEPD Q xy (Order 3): |z| < 10 cm and MB; Q^{S}_{xy}; Q^{N}_{xy}; Centrality [%]", m_hist_config.m_bins_sepd_Q, m_hist_config.m_sepd_Q_low, m_hist_config.m_sepd_Q_high, m_hist_config.m_bins_sepd_Q, m_hist_config.m_sepd_Q_low, m_hist_config.m_sepd_Q_high, m_hist_config.m_bins_cent, m_hist_config.m_cent_low, m_hist_config.m_cent_high);

  m_hists["h3SEPD_Q_S_xx_yy_2"]->Sumw2();
  m_hists["h3SEPD_Q_N_xx_yy_2"]->Sumw2();
  m_hists["h3SEPD_Q_xy_2"]->Sumw2();

  m_hists["h3SEPD_Q_S_xx_yy_3"]->Sumw2();
  m_hists["h3SEPD_Q_N_xx_yy_3"]->Sumw2();
  m_hists["h3SEPD_Q_xy_3"]->Sumw2();

  for (unsigned int i = 0; i < m_eventType.size(); ++i)
  {
    m_hists["hEvent"]->GetXaxis()->SetBinLabel(i + 1, m_eventType[i].c_str());
  }

  for (unsigned int i = 0; i < m_MinBias_Type.size(); ++i)
  {
    m_hists["hEventMinBias"]->GetXaxis()->SetBinLabel(i + 1, m_MinBias_Type[i].c_str());
  }

  // Q vector Correction Hists
  if (m_do_q_vec_corr)
  {
    std::unique_ptr<TFile> tfile(TFile::Open(m_q_vec_corr_fname.c_str()));
    if (!tfile || tfile->IsZombie())
    {
      std::cout << std::format("Error: Could not open ROOT file: {}", m_q_vec_corr_fname) << std::endl;
      m_do_q_vec_corr = false;
      return Fun4AllReturnCodes::EVENT_OK;
    }

    if (tfile->Get("h3SEPD_Q_N_2") == nullptr ||
        tfile->Get("h3SEPD_Q_S_2") == nullptr ||
        tfile->Get("h3SEPD_Q_N_3") == nullptr ||
        tfile->Get("h3SEPD_Q_S_3") == nullptr)
    {
      std::cout << std::format("Error: Could not get some / all of h3SEPD_Q hists from {}", m_q_vec_corr_fname) << std::endl;
      m_do_q_vec_corr = false;
      return Fun4AllReturnCodes::EVENT_OK;
    }

    if (tfile->Get("h3SEPD_Q_S_xx_yy_2") == nullptr ||
        tfile->Get("h3SEPD_Q_N_xx_yy_2") == nullptr ||
        tfile->Get("h3SEPD_Q_xy_2") == nullptr ||
        tfile->Get("h3SEPD_Q_S_xx_yy_3") == nullptr ||
        tfile->Get("h3SEPD_Q_N_xx_yy_3") == nullptr ||
        tfile->Get("h3SEPD_Q_xy_3") == nullptr
       )
    {
      std::cout << std::format("Warning: Could not get some / all of h3SEPD_Q hists for 2nd order correction from {}", m_q_vec_corr_fname) << std::endl;
      m_do_q_vec_corr2 = false;
    }

    // keep hists from deleting when tfile is closed
    TH1::AddDirectory(kFALSE);

    // Read Q vector calib
    m_hists["h3SEPD_Q_N_2_calib"] = std::unique_ptr<TH3>(dynamic_cast<TH3*>(tfile->Get("h3SEPD_Q_N_2")->Clone("h3SEPD_Q_N_2_calib")));
    m_hists["h3SEPD_Q_S_2_calib"] = std::unique_ptr<TH3>(dynamic_cast<TH3*>(tfile->Get("h3SEPD_Q_S_2")->Clone("h3SEPD_Q_S_2_calib")));
    m_hists["h3SEPD_Q_N_3_calib"] = std::unique_ptr<TH3>(dynamic_cast<TH3*>(tfile->Get("h3SEPD_Q_N_3")->Clone("h3SEPD_Q_N_3_calib")));
    m_hists["h3SEPD_Q_S_3_calib"] = std::unique_ptr<TH3>(dynamic_cast<TH3*>(tfile->Get("h3SEPD_Q_S_3")->Clone("h3SEPD_Q_S_3_calib")));

    // Init Q vector corr hists
    m_hists["h3SEPD_Q_S_corr_2"] = std::make_unique<TH3F>("h3SEPD_Q_S_corr_2", "sEPD South Q (Order 2): |z| < 10 cm and MB; Q_{x}; Q_{y}; Centrality [%]", m_hist_config.m_bins_sepd_Q, m_hist_config.m_sepd_Q_low, m_hist_config.m_sepd_Q_high, m_hist_config.m_bins_sepd_Q, m_hist_config.m_sepd_Q_low, m_hist_config.m_sepd_Q_high, m_hist_config.m_bins_cent, m_hist_config.m_cent_low, m_hist_config.m_cent_high);
    m_hists["h3SEPD_Q_N_corr_2"] = std::make_unique<TH3F>("h3SEPD_Q_N_corr_2", "sEPD North Q (Order 2): |z| < 10 cm and MB; Q_{x}; Q_{y}; Centrality [%]", m_hist_config.m_bins_sepd_Q, m_hist_config.m_sepd_Q_low, m_hist_config.m_sepd_Q_high, m_hist_config.m_bins_sepd_Q, m_hist_config.m_sepd_Q_low, m_hist_config.m_sepd_Q_high, m_hist_config.m_bins_cent, m_hist_config.m_cent_low, m_hist_config.m_cent_high);

    m_hists["h3SEPD_Q_S_corr_3"] = std::make_unique<TH3F>("h3SEPD_Q_S_corr_3", "sEPD South Q (Order 3): |z| < 10 cm and MB; Q_{x}; Q_{y}; Centrality [%]", m_hist_config.m_bins_sepd_Q, m_hist_config.m_sepd_Q_low, m_hist_config.m_sepd_Q_high, m_hist_config.m_bins_sepd_Q, m_hist_config.m_sepd_Q_low, m_hist_config.m_sepd_Q_high, m_hist_config.m_bins_cent, m_hist_config.m_cent_low, m_hist_config.m_cent_high);
    m_hists["h3SEPD_Q_N_corr_3"] = std::make_unique<TH3F>("h3SEPD_Q_N_corr_3", "sEPD North Q (Order 3): |z| < 10 cm and MB; Q_{x}; Q_{y}; Centrality [%]", m_hist_config.m_bins_sepd_Q, m_hist_config.m_sepd_Q_low, m_hist_config.m_sepd_Q_high, m_hist_config.m_bins_sepd_Q, m_hist_config.m_sepd_Q_low, m_hist_config.m_sepd_Q_high, m_hist_config.m_bins_cent, m_hist_config.m_cent_low, m_hist_config.m_cent_high);

    m_hists["h3SEPD_Q_S_corr_2"]->Sumw2();
    m_hists["h3SEPD_Q_N_corr_2"]->Sumw2();
    m_hists["h3SEPD_Q_S_corr_3"]->Sumw2();
    m_hists["h3SEPD_Q_N_corr_3"]->Sumw2();

    // local
    m_hists["h3SEPD_Psi_corr_2"] = std::make_unique<TH3F>("h3SEPD_Psi_corr_2", "sEPD #Psi (Order 2): |z| < 10 cm and MB; 2#Psi^{S}_{2}; 2#Psi^{N}_{2}; Centrality [%]", m_hist_config.m_bins_psi, m_hist_config.m_psi_low, m_hist_config.m_psi_high, m_hist_config.m_bins_psi, m_hist_config.m_psi_low, m_hist_config.m_psi_high, m_hist_config.m_bins_cent, m_hist_config.m_cent_low, m_hist_config.m_cent_high);

    m_hists["h3SEPD_Psi_corr_3"] = std::make_unique<TH3F>("h3SEPD_Psi_corr_3", "sEPD #Psi (Order 3): |z| < 10 cm and MB; 3#Psi^{S}_{3}; 3#Psi^{N}_{3}; Centrality [%]", m_hist_config.m_bins_psi, m_hist_config.m_psi_low, m_hist_config.m_psi_high, m_hist_config.m_bins_psi, m_hist_config.m_psi_low, m_hist_config.m_psi_high, m_hist_config.m_bins_cent, m_hist_config.m_cent_low, m_hist_config.m_cent_high);

    if(m_do_q_vec_corr2)
    {
      m_hists["h3SEPD_Q_S_xx_yy_2_calib"] = std::unique_ptr<TH3>(dynamic_cast<TH3*>(tfile->Get("h3SEPD_Q_S_xx_yy_2")->Clone("h3SEPD_Q_S_xx_yy_2_calib")));
      m_hists["h3SEPD_Q_N_xx_yy_2_calib"] = std::unique_ptr<TH3>(dynamic_cast<TH3*>(tfile->Get("h3SEPD_Q_N_xx_yy_2")->Clone("h3SEPD_Q_N_xx_yy_2_calib")));
      m_hists["h3SEPD_Q_xy_2_calib"] = std::unique_ptr<TH3>(dynamic_cast<TH3*>(tfile->Get("h3SEPD_Q_xy_2")->Clone("h3SEPD_Q_xy_2_calib")));

      m_hists["h3SEPD_Q_S_xx_yy_3_calib"] = std::unique_ptr<TH3>(dynamic_cast<TH3*>(tfile->Get("h3SEPD_Q_S_xx_yy_3")->Clone("h3SEPD_Q_S_xx_yy_3_calib")));
      m_hists["h3SEPD_Q_N_xx_yy_3_calib"] = std::unique_ptr<TH3>(dynamic_cast<TH3*>(tfile->Get("h3SEPD_Q_N_xx_yy_3")->Clone("h3SEPD_Q_N_xx_yy_3_calib")));
      m_hists["h3SEPD_Q_xy_3_calib"] = std::unique_ptr<TH3>(dynamic_cast<TH3*>(tfile->Get("h3SEPD_Q_xy_3")->Clone("h3SEPD_Q_xy_3_calib")));

      // local
      m_hists["h3SEPD_Psi_corr2_2"] = std::make_unique<TH3F>("h3SEPD_Psi_corr2_2", "sEPD #Psi (Order 2): |z| < 10 cm and MB; 2#Psi^{S}_{2}; 2#Psi^{N}_{2}; Centrality [%]", m_hist_config.m_bins_psi, m_hist_config.m_psi_low, m_hist_config.m_psi_high, m_hist_config.m_bins_psi, m_hist_config.m_psi_low, m_hist_config.m_psi_high, m_hist_config.m_bins_cent, m_hist_config.m_cent_low, m_hist_config.m_cent_high);

      m_hists["h3SEPD_Psi_corr2_3"] = std::make_unique<TH3F>("h3SEPD_Psi_corr2_3", "sEPD #Psi (Order 3): |z| < 10 cm and MB; 3#Psi^{S}_{3}; 3#Psi^{N}_{3}; Centrality [%]", m_hist_config.m_bins_psi, m_hist_config.m_psi_low, m_hist_config.m_psi_high, m_hist_config.m_bins_psi, m_hist_config.m_psi_low, m_hist_config.m_psi_high, m_hist_config.m_bins_cent, m_hist_config.m_cent_low, m_hist_config.m_cent_high);
    }
  }

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

  PdbParameterMap* pdb = getNode<PdbParameterMap>(topNode, "MinBiasParams");
  if (!pdb) {
    return Fun4AllReturnCodes::ABORTRUN;
  }

  PHParameters pdb_params("MinBiasParams");
  pdb_params.FillFrom(pdb);

  bool minbias_bkg_high     = pdb_params.get_int_param("minbias_background_cut_fail");
  bool minbias_side_hit_low = pdb_params.get_int_param("minbias_two_hit_min_fail");
  bool minbias_zdc_low      = pdb_params.get_int_param("minbias_zdc_energy_min_fail");
  bool minbias_mbd_high     = pdb_params.get_int_param("minbias_mbd_total_energy_max_fail");

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

  m_cent = centInfo->get_centile(CentralityInfo::PROP::mbd_NS)*100;

  m_hists["hCentrality"]->Fill(m_cent);

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
  double vertex_scale           = pdb_params.get_double_param("minbias_vertex_scale");
  double centrality_scale       = pdb_params.get_double_param("minbias_centrality_scale");

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

    if(mbd_arm == 0)
    {
      dynamic_cast<TProfile2D *>(m_hists["h2MBD_South_Charge"].get())->Fill(mbd_ch_phi, mbd_ch_eta, charge);
    }
    if(mbd_arm == 1)
    {
      dynamic_cast<TProfile2D *>(m_hists["h2MBD_North_Charge"].get())->Fill(mbd_ch_phi, mbd_ch_eta, charge);
    }

    JetUtils::update_min_max(mbd_ch_r, m_logging.m_mbd_ch_r_min, m_logging.m_mbd_ch_r_max);
    JetUtils::update_min_max(mbd_ch_z, m_logging.m_mbd_ch_z_min, m_logging.m_mbd_ch_z_max);
    JetUtils::update_min_max(mbd_ch_phi, m_logging.m_mbd_ch_phi_min, m_logging.m_mbd_ch_phi_max);
    JetUtils::update_min_max(mbd_ch_eta, m_logging.m_mbd_ch_eta_min, m_logging.m_mbd_ch_eta_max);
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

  // Setup Q-vectors
  double Q_S_x_2 = 0;
  double Q_S_y_2 = 0;
  double Q_N_x_2 = 0;
  double Q_N_y_2 = 0;

  double Q_S_x_3 = 0;
  double Q_S_y_3 = 0;
  double Q_N_x_3 = 0;
  double Q_N_y_3 = 0;

  // Q-vector correction
  double Q_S_x_2_avg = 0;
  double Q_S_y_2_avg = 0;
  double Q_N_x_2_avg = 0;
  double Q_N_y_2_avg = 0;

  double Q_S_x_3_avg = 0;
  double Q_S_y_3_avg = 0;
  double Q_N_x_3_avg = 0;
  double Q_N_y_3_avg = 0;

  double Q_S_xx_2_avg = 0;
  double Q_S_yy_2_avg = 0;
  double Q_S_xy_2_avg = 0;

  double Q_N_xx_2_avg = 0;
  double Q_N_yy_2_avg = 0;
  double Q_N_xy_2_avg = 0;

  double Q_S_xx_3_avg = 0;
  double Q_S_yy_3_avg = 0;
  double Q_S_xy_3_avg = 0;

  double Q_N_xx_3_avg = 0;
  double Q_N_yy_3_avg = 0;
  double Q_N_xy_3_avg = 0;

  if(m_do_q_vec_corr)
  {
    int cent_low = (static_cast<int>(m_cent) / 10) * 10;
    int cent_high = cent_low + 9;

    int cent_bin_low = m_hists["h3SEPD_Q_S_2_calib"]->GetZaxis()->FindBin(cent_low);
    int cent_bin_high = m_hists["h3SEPD_Q_S_2_calib"]->GetZaxis()->FindBin(cent_high);

    m_hists["h3SEPD_Q_S_2_calib"]->GetZaxis()->SetRange(cent_bin_low, cent_bin_high);
    m_hists["h3SEPD_Q_N_2_calib"]->GetZaxis()->SetRange(cent_bin_low, cent_bin_high);
    m_hists["h3SEPD_Q_S_3_calib"]->GetZaxis()->SetRange(cent_bin_low, cent_bin_high);
    m_hists["h3SEPD_Q_N_3_calib"]->GetZaxis()->SetRange(cent_bin_low, cent_bin_high);

    Q_S_x_2_avg =  m_hists["h3SEPD_Q_S_2_calib"]->GetMean(1);
    Q_S_y_2_avg =  m_hists["h3SEPD_Q_S_2_calib"]->GetMean(2);
    Q_N_x_2_avg =  m_hists["h3SEPD_Q_N_2_calib"]->GetMean(1);
    Q_N_y_2_avg =  m_hists["h3SEPD_Q_N_2_calib"]->GetMean(2);

    Q_S_x_3_avg =  m_hists["h3SEPD_Q_S_3_calib"]->GetMean(1);
    Q_S_y_3_avg =  m_hists["h3SEPD_Q_S_3_calib"]->GetMean(2);
    Q_N_x_3_avg =  m_hists["h3SEPD_Q_N_3_calib"]->GetMean(1);
    Q_N_y_3_avg =  m_hists["h3SEPD_Q_N_3_calib"]->GetMean(2);

    if (m_do_q_vec_corr2)
    {
      m_hists["h3SEPD_Q_S_xx_yy_2_calib"]->GetZaxis()->SetRange(cent_bin_low, cent_bin_high);
      m_hists["h3SEPD_Q_N_xx_yy_2_calib"]->GetZaxis()->SetRange(cent_bin_low, cent_bin_high);
      m_hists["h3SEPD_Q_xy_2_calib"]->GetZaxis()->SetRange(cent_bin_low, cent_bin_high);

      m_hists["h3SEPD_Q_S_xx_yy_3_calib"]->GetZaxis()->SetRange(cent_bin_low, cent_bin_high);
      m_hists["h3SEPD_Q_N_xx_yy_3_calib"]->GetZaxis()->SetRange(cent_bin_low, cent_bin_high);
      m_hists["h3SEPD_Q_xy_3_calib"]->GetZaxis()->SetRange(cent_bin_low, cent_bin_high);

      Q_S_xx_2_avg = m_hists["h3SEPD_Q_S_xx_yy_2_calib"]->GetMean(1);
      Q_S_yy_2_avg = m_hists["h3SEPD_Q_S_xx_yy_2_calib"]->GetMean(2);
      Q_N_xx_2_avg = m_hists["h3SEPD_Q_N_xx_yy_2_calib"]->GetMean(1);
      Q_N_yy_2_avg = m_hists["h3SEPD_Q_N_xx_yy_2_calib"]->GetMean(2);

      Q_S_xy_2_avg = m_hists["h3SEPD_Q_xy_2_calib"]->GetMean(1);
      Q_N_xy_2_avg = m_hists["h3SEPD_Q_xy_2_calib"]->GetMean(2);

      Q_S_xx_3_avg = m_hists["h3SEPD_Q_S_xx_yy_3_calib"]->GetMean(1);
      Q_S_yy_3_avg = m_hists["h3SEPD_Q_S_xx_yy_3_calib"]->GetMean(2);
      Q_N_xx_3_avg = m_hists["h3SEPD_Q_N_xx_yy_3_calib"]->GetMean(1);
      Q_N_yy_3_avg = m_hists["h3SEPD_Q_N_xx_yy_3_calib"]->GetMean(2);

      Q_S_xy_3_avg = m_hists["h3SEPD_Q_xy_3_calib"]->GetMean(1);
      Q_N_xy_3_avg = m_hists["h3SEPD_Q_xy_3_calib"]->GetMean(2);
    }

    if (Verbosity() >= Fun4AllBase::VERBOSITY_SOME)
    {
      std::cout << std::format("Cent: {:.2f}, low: {:2d}, high: {:3d}, Q_S_2 avg: ({:.4f}, {:.4f}), Q_N_2 avg: ({:.4f}, {:.4f})", m_cent, cent_bin_low, cent_bin_high, Q_S_x_2_avg, Q_S_y_2_avg, Q_N_x_2_avg, Q_N_y_2_avg) << std::endl;
    }
  }

  for (unsigned int channel = 0; channel < nchannels_epd; ++channel)
  {
    unsigned int key = TowerInfoDefs::encode_epd(channel);

    TowerInfo *tower = towerinfosEPD->get_tower_at_channel(channel);

    double charge = tower->get_energy();
    bool isZS     = tower->get_isZS();
    double sepd_ch_z = epdgeom->get_z(key);
    double sepd_ch_r = epdgeom->get_r(key);
    ROOT::Math::XYZTVector vec(sepd_ch_r, 0, sepd_ch_z, 0);
    double eta = vec.Eta();
    double phi = epdgeom->get_phi(key);

    // exclude ZS
    if(isZS)
    {
      ++m_ctr["sepd_tower_zs"];
      continue;
    }

    JetUtils::update_min_max(charge, m_logging.m_sepd_charge_min, m_logging.m_sepd_charge_max);

    // expecting Nmips
    if(charge < m_cuts.m_sepd_charge_threshold)
    {
      ++m_ctr["sepd_tower_charge_below_threshold"];
      continue;
    }

    JetUtils::update_min_max(sepd_ch_z, m_logging.m_sepd_z_min, m_logging.m_sepd_z_max);
    JetUtils::update_min_max(sepd_ch_r, m_logging.m_sepd_r_min, m_logging.m_sepd_r_max);
    JetUtils::update_min_max(phi, m_logging.m_sepd_phi_min, m_logging.m_sepd_phi_max);
    JetUtils::update_min_max(eta, m_logging.m_sepd_eta_min, m_logging.m_sepd_eta_max);

    double q_x_2 = charge*std::cos(2*phi);
    double q_y_2 = charge*std::sin(2*phi);

    double q_x_3 = charge*std::cos(3*phi);
    double q_y_3 = charge*std::sin(3*phi);

    // sepd charge sums
    unsigned int arm = TowerInfoDefs::get_epd_arm(key);
    // South
    if (arm == 0)
    {
      sepd_total_charge_south += charge;

      Q_S_x_2 += q_x_2;
      Q_S_y_2 += q_y_2;

      Q_S_x_3 += q_x_3;
      Q_S_y_3 += q_y_3;
    }
    // North
    else if (arm == 1)
    {
      sepd_total_charge_north += charge;

      Q_N_x_2 += q_x_2;
      Q_N_y_2 += q_y_2;

      Q_N_x_3 += q_x_3;
      Q_N_y_3 += q_y_3;
    }
    else {
      ++m_ctr["sepd_tower_unknown_arm"];
    }
  }

  double sepd_total_charge = sepd_total_charge_south + sepd_total_charge_north;

  dynamic_cast<TH2 *>(m_hists["h2SEPD_Charge"].get())->Fill(sepd_total_charge, m_cent);
  dynamic_cast<TH3 *>(m_hists["h3SEPD_MBD_Total_Charge"].get())->Fill(m_mbd_total_charge, sepd_total_charge, m_cent);

  // ensure both total charges are nonzero
  if(sepd_total_charge_south == 0 || sepd_total_charge_north == 0)
  {
    ++m_ctr["process_sEPD_total_charge_zero"];
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  Q_S_x_2 /= sepd_total_charge_south;
  Q_S_y_2 /= sepd_total_charge_south;

  Q_S_x_3 /= sepd_total_charge_south;
  Q_S_y_3 /= sepd_total_charge_south;

  Q_N_x_2 /= sepd_total_charge_north;
  Q_N_y_2 /= sepd_total_charge_north;

  Q_N_x_3 /= sepd_total_charge_north;
  Q_N_y_3 /= sepd_total_charge_north;

  // Compute Q vector first order correction
  double Q_S_x_corr_2 = Q_S_x_2 - Q_S_x_2_avg;
  double Q_S_y_corr_2 = Q_S_y_2 - Q_S_y_2_avg;

  double Q_S_x_corr_3 = Q_S_x_3 - Q_S_x_3_avg;
  double Q_S_y_corr_3 = Q_S_y_3 - Q_S_y_3_avg;

  double Q_N_x_corr_2 = Q_N_x_2 - Q_N_x_2_avg;
  double Q_N_y_corr_2 = Q_N_y_2 - Q_N_y_2_avg;

  double Q_N_x_corr_3 = Q_N_x_3 - Q_N_x_3_avg;
  double Q_N_y_corr_3 = Q_N_y_3 - Q_N_y_3_avg;

  // Compute Q vector second order correction factors
  double Q_S_xx_2 = Q_S_x_corr_2 * Q_S_x_corr_2;
  double Q_S_yy_2 = Q_S_y_corr_2 * Q_S_y_corr_2;
  double Q_S_xy_2 = Q_S_x_corr_2 * Q_S_y_corr_2;

  double Q_N_xx_2 = Q_N_x_corr_2 * Q_N_x_corr_2;
  double Q_N_yy_2 = Q_N_y_corr_2 * Q_N_y_corr_2;
  double Q_N_xy_2 = Q_N_x_corr_2 * Q_N_y_corr_2;

  double Q_S_xx_3 = Q_S_x_corr_3 * Q_S_x_corr_3;
  double Q_S_yy_3 = Q_S_y_corr_3 * Q_S_y_corr_3;
  double Q_S_xy_3 = Q_S_x_corr_3 * Q_S_y_corr_3;

  double Q_N_xx_3 = Q_N_x_corr_3 * Q_N_x_corr_3;
  double Q_N_yy_3 = Q_N_y_corr_3 * Q_N_y_corr_3;
  double Q_N_xy_3 = Q_N_x_corr_3 * Q_N_y_corr_3;

  double D_S_2 = std::sqrt(Q_S_xx_2_avg * Q_S_yy_2_avg - Q_S_xy_2_avg * Q_S_xy_2_avg);
  double D_N_2 = std::sqrt(Q_N_xx_2_avg * Q_N_yy_2_avg - Q_N_xy_2_avg * Q_N_xy_2_avg);

  double N_S_2 = D_S_2 * (Q_S_xx_2_avg + Q_S_yy_2_avg + 2 * D_S_2);
  double N_N_2 = D_N_2 * (Q_S_xx_2_avg + Q_S_yy_2_avg + 2 * D_N_2);

  std::vector<std::vector<double>> X_S_2(2, std::vector<double>(2, 0));
  std::vector<std::vector<double>> X_N_2(2, std::vector<double>(2, 0));

  // compute second order correction matrix elements
  if(N_S_2 != 0 && N_N_2 != 0)
  {
    X_S_2[0][0] = 1/std::sqrt(N_S_2) * (Q_S_yy_2_avg + D_S_2);
    X_S_2[0][1] = -1/std::sqrt(N_S_2) * (Q_S_xy_2_avg);
    X_S_2[1][0] = X_S_2[0][1];
    X_S_2[1][1] = 1/std::sqrt(N_S_2) * (Q_S_xx_2_avg + D_S_2);

    X_N_2[0][0] = 1/std::sqrt(N_N_2) * (Q_N_yy_2_avg + D_N_2);
    X_N_2[0][1] = -1/std::sqrt(N_N_2) * (Q_N_xy_2_avg);
    X_N_2[1][0] = X_N_2[0][1];
    X_N_2[1][1] = 1/std::sqrt(N_N_2) * (Q_N_xx_2_avg + D_N_2);
  }

  double D_S_3 = std::sqrt(Q_S_xx_3_avg * Q_S_yy_3_avg - Q_S_xy_3_avg * Q_S_xy_3_avg);
  double D_N_3 = std::sqrt(Q_N_xx_3_avg * Q_N_yy_3_avg - Q_N_xy_3_avg * Q_N_xy_3_avg);

  double N_S_3 = D_S_3 * (Q_S_xx_3_avg + Q_S_yy_3_avg + 2 * D_S_3);
  double N_N_3 = D_N_3 * (Q_S_xx_3_avg + Q_S_yy_3_avg + 2 * D_N_3);

  std::vector<std::vector<double>> X_S_3(2, std::vector<double>(2, 0));
  std::vector<std::vector<double>> X_N_3(2, std::vector<double>(2, 0));

  // compute second order correction matrix elements
  if(N_S_3 != 0 && N_N_3 != 0)
  {
    X_S_3[0][0] = 1/std::sqrt(N_S_3) * (Q_S_yy_3_avg + D_S_3);
    X_S_3[0][1] = -1/std::sqrt(N_S_3) * (Q_S_xy_3_avg);
    X_S_3[1][0] = X_S_3[0][1];
    X_S_3[1][1] = 1/std::sqrt(N_S_3) * (Q_S_xx_3_avg + D_S_3);

    X_N_3[0][0] = 1/std::sqrt(N_N_3) * (Q_N_yy_3_avg + D_N_3);
    X_N_3[0][1] = -1/std::sqrt(N_N_3) * (Q_N_xy_3_avg);
    X_N_3[1][0] = X_N_3[0][1];
    X_N_3[1][1] = 1/std::sqrt(N_N_3) * (Q_N_xx_3_avg + D_N_3);
  }

  // Compute Q vector second order correction
  double Q_S_x_corr2_2 = X_S_2[0][0] * Q_S_x_corr_2 + X_S_2[0][1] * Q_S_y_corr_2;
  double Q_S_y_corr2_2 = X_S_2[1][0] * Q_S_x_corr_2 + X_S_2[1][1] * Q_S_y_corr_2;
  double Q_N_x_corr2_2 = X_N_2[0][0] * Q_N_x_corr_2 + X_N_2[0][1] * Q_N_y_corr_2;
  double Q_N_y_corr2_2 = X_N_2[1][0] * Q_N_x_corr_2 + X_N_2[1][1] * Q_N_y_corr_2;

  double Q_S_x_corr2_3 = X_S_3[0][0] * Q_S_x_corr_3 + X_S_3[0][1] * Q_S_y_corr_3;
  double Q_S_y_corr2_3 = X_S_3[1][0] * Q_S_x_corr_3 + X_S_3[1][1] * Q_S_y_corr_3;
  double Q_N_x_corr2_3 = X_N_3[0][0] * Q_N_x_corr_3 + X_N_3[0][1] * Q_N_y_corr_3;
  double Q_N_y_corr2_3 = X_N_3[1][0] * Q_N_x_corr_3 + X_N_3[1][1] * Q_N_y_corr_3;

  m_logging.m_sepd_Q_min = std::min(m_logging.m_sepd_Q_min, std::min(Q_S_x_2,
                                        std::min(Q_S_y_2,
                                        std::min(Q_N_x_2,
                                        std::min(Q_N_y_2,
                                        std::min(Q_S_x_3,
                                        std::min(Q_S_y_3,
                                        std::min(Q_N_x_3, Q_N_y_3))))))));

  m_logging.m_sepd_Q_max = std::max(m_logging.m_sepd_Q_max, std::max(Q_S_x_2,
                                        std::max(Q_S_y_2,
                                        std::max(Q_N_x_2,
                                        std::max(Q_N_y_2,
                                        std::max(Q_S_x_3,
                                        std::max(Q_S_y_3,
                                        std::max(Q_N_x_3, Q_N_y_3))))))));

  dynamic_cast<TH3 *>(m_hists["h3SEPD_Q_S_2"].get())->Fill(Q_S_x_2, Q_S_y_2, m_cent);
  dynamic_cast<TH3 *>(m_hists["h3SEPD_Q_N_2"].get())->Fill(Q_N_x_2, Q_N_y_2, m_cent);
  dynamic_cast<TH3 *>(m_hists["h3SEPD_Q_S_3"].get())->Fill(Q_S_x_3, Q_S_y_3, m_cent);
  dynamic_cast<TH3 *>(m_hists["h3SEPD_Q_N_3"].get())->Fill(Q_N_x_3, Q_N_y_3, m_cent);

  double psi_S_2 = std::atan2(Q_S_y_2, Q_S_x_2);
  double psi_N_2 = std::atan2(Q_N_y_2, Q_N_x_2);

  double psi_S_3 = std::atan2(Q_S_y_3, Q_S_x_3);
  double psi_N_3 = std::atan2(Q_N_y_3, Q_N_x_3);

  dynamic_cast<TH3 *>(m_hists["h3SEPD_Psi_2"].get())->Fill(psi_S_2, psi_N_2, m_cent);
  dynamic_cast<TH3 *>(m_hists["h3SEPD_Psi_3"].get())->Fill(psi_S_3, psi_N_3, m_cent);

  // Q vector Corrected
  if(m_do_q_vec_corr)
  {
    // 1st order correction
    dynamic_cast<TH3 *>(m_hists["h3SEPD_Q_S_corr_2"].get())->Fill(Q_S_x_corr_2, Q_S_y_corr_2, m_cent);
    dynamic_cast<TH3 *>(m_hists["h3SEPD_Q_N_corr_2"].get())->Fill(Q_N_x_corr_2, Q_N_y_corr_2, m_cent);
    dynamic_cast<TH3 *>(m_hists["h3SEPD_Q_S_corr_3"].get())->Fill(Q_S_x_corr_3, Q_S_y_corr_3, m_cent);
    dynamic_cast<TH3 *>(m_hists["h3SEPD_Q_N_corr_3"].get())->Fill(Q_N_x_corr_3, Q_N_y_corr_3, m_cent);

    double psi_S_corr_2 = std::atan2(Q_S_y_corr_2, Q_S_x_corr_2);
    double psi_N_corr_2 = std::atan2(Q_N_y_corr_2, Q_N_x_corr_2);

    double psi_S_corr_3 = std::atan2(Q_S_y_corr_3, Q_S_x_corr_3);
    double psi_N_corr_3 = std::atan2(Q_N_y_corr_3, Q_N_x_corr_3);

    dynamic_cast<TH3 *>(m_hists["h3SEPD_Psi_corr_2"].get())->Fill(psi_S_corr_2, psi_N_corr_2, m_cent);
    dynamic_cast<TH3 *>(m_hists["h3SEPD_Psi_corr_3"].get())->Fill(psi_S_corr_3, psi_N_corr_3, m_cent);

    // 2nd order correction
    dynamic_cast<TH3 *>(m_hists["h3SEPD_Q_S_xx_yy_2"].get())->Fill(Q_S_xx_2, Q_S_yy_2, m_cent);
    dynamic_cast<TH3 *>(m_hists["h3SEPD_Q_N_xx_yy_2"].get())->Fill(Q_N_xx_2, Q_N_yy_2, m_cent);
    dynamic_cast<TH3 *>(m_hists["h3SEPD_Q_xy_2"].get())->Fill(Q_S_xy_2, Q_N_xy_2, m_cent);

    dynamic_cast<TH3 *>(m_hists["h3SEPD_Q_S_xx_yy_3"].get())->Fill(Q_S_xx_3, Q_S_yy_3, m_cent);
    dynamic_cast<TH3 *>(m_hists["h3SEPD_Q_N_xx_yy_3"].get())->Fill(Q_N_xx_3, Q_N_yy_3, m_cent);
    dynamic_cast<TH3 *>(m_hists["h3SEPD_Q_xy_3"].get())->Fill(Q_S_xy_3, Q_N_xy_3, m_cent);

    if (m_do_q_vec_corr2)
    {
      double psi_S_corr2_2 = std::atan2(Q_S_y_corr2_2, Q_S_x_corr2_2);
      double psi_N_corr2_2 = std::atan2(Q_N_y_corr2_2, Q_N_x_corr2_2);

      double psi_S_corr2_3 = std::atan2(Q_S_y_corr2_3, Q_S_x_corr2_3);
      double psi_N_corr2_3 = std::atan2(Q_N_y_corr2_3, Q_N_x_corr2_3);

      dynamic_cast<TH3 *>(m_hists["h3SEPD_Psi_corr2_2"].get())->Fill(psi_S_corr2_2, psi_N_corr2_2, m_cent);
      dynamic_cast<TH3 *>(m_hists["h3SEPD_Psi_corr2_3"].get())->Fill(psi_S_corr2_3, psi_N_corr2_3, m_cent);
    }
  }

  dynamic_cast<TH3 *>(m_hists["h3SEPD_Total_Charge"].get())->Fill(sepd_total_charge_south, sepd_total_charge_north, m_cent);

  JetUtils::update_min_max(sepd_total_charge_south, m_logging.m_sepd_total_charge_south_min, m_logging.m_sepd_total_charge_south_max);
  JetUtils::update_min_max(sepd_total_charge_north, m_logging.m_sepd_total_charge_north_min, m_logging.m_sepd_total_charge_north_max);

  return Fun4AllReturnCodes::EVENT_OK;
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
int sEPDValidation::process_event(PHCompositeNode *topNode)
{
  EventHeader *eventInfo = getNode<EventHeader>(topNode, "EventHeader");
  if (!eventInfo)
  {
    return Fun4AllReturnCodes::ABORTRUN;
  }

  int m_globalEvent = eventInfo->get_EvtSequence();
  // int m_run         = eventInfo->get_RunNumber();

  if (m_event % 20 == 0)
  {
    std::cout << "Progress: " << m_event << ", Global: " << m_globalEvent << std::endl;
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

  if(m_do_ep)
  {
    ret = process_EventPlane(topNode);
    if (ret)
    {
      return ret;
    }
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int sEPDValidation::End([[maybe_unused]] PHCompositeNode *topNode)
{
  std::cout << "sEPDValidation::End" << std::endl;

  std::cout << "=====================" << std::endl;
  std::cout << "stats" << std::endl;
  std::cout << "Cent: Min: " << m_logging.m_cent_min << ", Max: " << m_logging.m_cent_max << std::endl;
  std::cout << "sEPD Charge: Min: " << m_logging.m_sepd_charge_min << ", Max: " << m_logging.m_sepd_charge_max << std::endl;
  std::cout << "sEPD Q: Min: " << m_logging.m_sepd_Q_min << ", Max: " << m_logging.m_sepd_Q_max << std::endl;
  std::cout << "sEPD Total Charge South: Min: " << m_logging.m_sepd_total_charge_south_min << ", Max: " << m_logging.m_sepd_total_charge_south_max << std::endl;
  std::cout << "sEPD Total Charge North: Min " << m_logging.m_sepd_total_charge_north_min << ", Max: " << m_logging.m_sepd_total_charge_north_max << std::endl;
  std::cout << "sEPD z: Min " << m_logging.m_sepd_z_min << ", Max: " << m_logging.m_sepd_z_max << std::endl;
  std::cout << "sEPD r: Min " << m_logging.m_sepd_r_min << ", Max: " << m_logging.m_sepd_r_max << std::endl;
  std::cout << "sEPD Phi: Min " << m_logging.m_sepd_phi_min << ", Max: " << m_logging.m_sepd_phi_max << std::endl;
  std::cout << "sEPD Eta: Min " << m_logging.m_sepd_eta_min << ", Max: " << m_logging.m_sepd_eta_max << std::endl;
  std::cout << "=====================" << std::endl;
  std::cout << "sEPD" << std::endl;
  std::cout << "Avg towers with unknown arm: " << m_ctr["sepd_tower_unknown_arm"] / m_hists["hEvent"]->GetBinContent(static_cast<std::uint8_t>(EventType::ZVTX10_MB)+1) << std::endl;
  std::cout << "Avg towers ZS: " << m_ctr["sepd_tower_zs"] / m_hists["hEvent"]->GetBinContent(static_cast<std::uint8_t>(EventType::ZVTX10_MB)+1) << std::endl;
  std::cout << "Avg towers with charge below threshold: " << m_ctr["sepd_tower_charge_below_threshold"] / m_hists["hEvent"]->GetBinContent(static_cast<std::uint8_t>(EventType::ZVTX10_MB)+1) << std::endl;
  std::cout << "=====================" << std::endl;
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
  std::cout << "=====================" << std::endl;
  std::cout << "Abort Events Types" << std::endl;
  std::cout << std::format("process event, isAuAuMinBias Fail: {}", m_ctr["process_eventCheck_isAuAuMinBias_fail"]) << std::endl;
  std::cout << std::format("process event, |z| >= {} cm: {}", m_cuts.m_zvtx_max, m_ctr["process_eventCheck_zvtx_large"]) << std::endl;
  std::cout << "process sEPD, total charge zero: " << m_ctr["process_sEPD_total_charge_zero"] << std::endl;
  if(m_do_ep)
  {
  std::cout << std::format("process Event Plane, Q vec mag zero: {}", m_ctr["process_EventPlane_Q_mag_zero"]) << std::endl;
  std::cout << std::format("process Event Plane, ep map empty: {}", m_ctr["process_EventPlane_epmap_empty"]) << std::endl;
  std::cout << std::format("process Event Plane, epd invalid: {}", m_ctr["process_EventPlane_epd_invalid"]) << std::endl;
    std::cout << "process Event Plane, Psi: Min " << m_logging.m_psi_min << ", Max: " << m_logging.m_psi_max << std::endl;
    std::cout << "process Event Plane, Q mag zero: " << m_ctr["process_EventPlane_Q_mag_zero"] << std::endl;
    std::cout << "process Event Plane, epmap empty: " << m_ctr["process_EventPlane_epmap_empty"] << std::endl;
    std::cout << "process Event Plane, epd invalid: " << m_ctr["process_EventPlane_epd_invalid"] << std::endl;
  }
  std::cout << "=====================" << std::endl;
  std::cout << "Events" << std::endl;
  for (unsigned int i = 0; i < m_eventType.size(); ++i)
  {
    std::cout << m_eventType[i] << ": " << m_hists["hEvent"]->GetBinContent(i + 1) << std::endl;
  }
  std::cout << "=====================" << std::endl;

  TFile output(m_outfile_name.c_str(), "recreate");
  output.cd();

  for (const auto &[name, hist] : m_hists)
  {
    if(!name.ends_with("calib"))
    {
      hist->Write();
    }
  }

  auto project_and_write = [&](const std::string& hist_name, const std::string& projection) {
      auto* hist = m_hists[hist_name].get();
      if (auto* h3 = dynamic_cast<TH3*>(hist)) {
          h3->Project3D(projection.c_str())->Write();
      } else if (auto* h2 = dynamic_cast<TH2*>(hist)) {
          h2->ProjectionX()->Write();
      }
  };

  // local
  if(!m_condor_mode)
  {
    project_and_write("h2SEPD_Charge", "x");
    project_and_write("h2MBD_Total_Charge", "x");
    project_and_write("h3SEPD_Total_Charge", "yx");
    project_and_write("h3MBD_Total_Charge", "yx");

    project_and_write("h3SEPD_Q_S_2", "yx");
    project_and_write("h3SEPD_Q_N_2", "yx");

    project_and_write("h3SEPD_Q_S_3", "yx");
    project_and_write("h3SEPD_Q_N_3", "yx");

    project_and_write("h3SEPD_Psi_2", "yx");
    project_and_write("h3SEPD_Psi_3", "yx");

    // Q Vector Correction
    if(m_do_q_vec_corr)
    {
      project_and_write("h3SEPD_Q_S_corr_2", "yx");
      project_and_write("h3SEPD_Q_N_corr_2", "yx");

      project_and_write("h3SEPD_Q_S_corr_3", "yx");
      project_and_write("h3SEPD_Q_N_corr_3", "yx");

      project_and_write("h3SEPD_Psi_corr_2", "yx");
      project_and_write("h3SEPD_Psi_corr_3", "yx");
    }

    // Official
    if(m_do_ep)
    {
      project_and_write("h3SEPD_EventPlaneInfo_Psi_2", "yx");
      project_and_write("h3SEPD_EventPlaneInfo_Psi_3", "yx");
    }
  }
  output.Close();

  return Fun4AllReturnCodes::EVENT_OK;
}
