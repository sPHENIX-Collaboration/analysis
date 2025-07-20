#include "sEPDValidation.h"
#include "JetUtils.h"

// -- c++
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

// -- Calo
#include <calobase/TowerInfo.h>
#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfoDefs.h>

// -- Vtx
#include <globalvertex/GlobalVertex.h>
#include <globalvertex/GlobalVertexMap.h>

// -- MB
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

#include <TFile.h>
#include <TH2.h>
#include <TH3.h>

//____________________________________________________________________________..
sEPDValidation::sEPDValidation(const std::string &name)
  : SubsysReco(name)
  , m_event(0)
  , m_bins_zvtx(200) /*cm*/
  , m_zvtx_low(-50)  /*cm*/
  , m_zvtx_high(50)  /*cm*/
  , m_bins_cent(100)
  , m_cent_low(-0.5)
  , m_cent_high(99.5)
  , m_bins_sepd_charge(200)
  , m_sepd_charge_low(0)
  , m_sepd_charge_high(200)
  , m_bins_sepd_Q(100)
  , m_sepd_Q_low(-1)
  , m_sepd_Q_high(1)
  , m_bins_sepd_total_charge(200)
  , m_sepd_total_charge_low(0)
  , m_sepd_total_charge_high(2e4)
  , m_bins_psi(126)
  , m_psi_low(-3.15)
  , m_psi_high(3.15)
  , m_bins_Q(100)
  , m_Q_low(0)
  , m_Q_high(200)
  , m_zvtx(9999)
  , m_cent(9999)
  , m_zvtx_max(10) /*cm*/
  , m_sepd_charge_threshold(0.2)
  , m_cent_min(9999)
  , m_cent_max(0)
  , m_sepd_charge_min(9999)
  , m_sepd_charge_max(0)
  , m_sepd_Q_min(9999)
  , m_sepd_Q_max(0)
  , m_sepd_total_charge_south_min(9999)
  , m_sepd_total_charge_south_max(0)
  , m_sepd_total_charge_north_min(9999)
  , m_sepd_total_charge_north_max(0)
  , m_sepd_phi_min(9999)
  , m_sepd_phi_max(0)
  , m_psi_min(9999)
  , m_psi_max(0)
{
}

//____________________________________________________________________________..
int sEPDValidation::Init([[maybe_unused]] PHCompositeNode *topNode)
{
  Fun4AllServer *se = Fun4AllServer::instance();
  se->Print("NODETREE");

  // init hists
  m_hists["hEvent"] = std::make_unique<TH1F>("hEvent", "Event Type; Type; Events", m_eventType.size(), 0, m_eventType.size());
  m_hists["hVtxZ"] = std::make_unique<TH1F>("hVtxZ", "Z Vertex; z [cm]; Events", m_bins_zvtx, m_zvtx_low, m_zvtx_high);
  m_hists["hVtxZ_MB"] = std::make_unique<TH1F>("hVtxZ_MB", "Z Vertex; z [cm]; Events", m_bins_zvtx, m_zvtx_low, m_zvtx_high);
  m_hists["hCentrality"] = std::make_unique<TH1F>("hCentrality", "Centrality; Centrality [%]; Events", m_bins_cent, m_cent_low, m_cent_high);

  // Charge
  m_hists["h2SEPD_Charge"] = std::make_unique<TH2F>("h2SEPD_Charge", "sEPD Charge: |z| < 10 cm and MB; Tower Charge; Centrality [%]", m_bins_sepd_charge, m_sepd_charge_low, m_sepd_charge_high, m_bins_cent, m_cent_low, m_cent_high);
  m_hists["h3SEPD_Total_Charge"] = std::make_unique<TH3F>("h3SEPD_Total_Charge", "sEPD Total Charge: |z| < 10 cm and MB; South; North; Centrality [%]", m_bins_sepd_total_charge, m_sepd_total_charge_low, m_sepd_total_charge_high, m_bins_sepd_total_charge, m_sepd_total_charge_low, m_sepd_total_charge_high, m_bins_cent, m_cent_low, m_cent_high);

  // Q-vectors local
  m_hists["h3SEPD_Q_S_2"] = std::make_unique<TH3F>("h3SEPD_Q_S_2", "sEPD South Q (Order 2): |z| < 10 cm and MB; Q_{x}; Q_{y}; Centrality [%]", m_bins_sepd_Q, m_sepd_Q_low, m_sepd_Q_high, m_bins_sepd_Q, m_sepd_Q_low, m_sepd_Q_high, m_bins_cent, m_cent_low, m_cent_high);
  m_hists["h3SEPD_Q_N_2"] = std::make_unique<TH3F>("h3SEPD_Q_N_2", "sEPD North Q (Order 2): |z| < 10 cm and MB; Q_{x}; Q_{y}; Centrality [%]", m_bins_sepd_Q, m_sepd_Q_low, m_sepd_Q_high, m_bins_sepd_Q, m_sepd_Q_low, m_sepd_Q_high, m_bins_cent, m_cent_low, m_cent_high);

  m_hists["h3SEPD_Q_S_3"] = std::make_unique<TH3F>("h3SEPD_Q_S_3", "sEPD South Q (Order 3): |z| < 10 cm and MB; Q_{x}; Q_{y}; Centrality [%]", m_bins_sepd_Q, m_sepd_Q_low, m_sepd_Q_high, m_bins_sepd_Q, m_sepd_Q_low, m_sepd_Q_high, m_bins_cent, m_cent_low, m_cent_high);
  m_hists["h3SEPD_Q_N_3"] = std::make_unique<TH3F>("h3SEPD_Q_N_3", "sEPD North Q (Order 3): |z| < 10 cm and MB; Q_{x}; Q_{y}; Centrality [%]", m_bins_sepd_Q, m_sepd_Q_low, m_sepd_Q_high, m_bins_sepd_Q, m_sepd_Q_low, m_sepd_Q_high, m_bins_cent, m_cent_low, m_cent_high);

  m_hists["h3SEPD_Q_S_2"]->Sumw2();
  m_hists["h3SEPD_Q_N_2"]->Sumw2();
  m_hists["h3SEPD_Q_S_3"]->Sumw2();
  m_hists["h3SEPD_Q_N_3"]->Sumw2();

  // local
  m_hists["h3SEPD_Psi_2"] = std::make_unique<TH3F>("h3SEPD_Psi_2", "sEPD #Psi (Order 2): |z| < 10 cm and MB; 2#Psi^{S}_{2}; 2#Psi^{N}_{2}; Centrality [%]", m_bins_psi, m_psi_low, m_psi_high, m_bins_psi, m_psi_low, m_psi_high, m_bins_cent, m_cent_low, m_cent_high);

  m_hists["h3SEPD_Psi_3"] = std::make_unique<TH3F>("h3SEPD_Psi_3", "sEPD #Psi (Order 3): |z| < 10 cm and MB; 3#Psi^{S}_{3}; 3#Psi^{N}_{3}; Centrality [%]", m_bins_psi, m_psi_low, m_psi_high, m_bins_psi, m_psi_low, m_psi_high, m_bins_cent, m_cent_low, m_cent_high);

  // Official
  m_hists["h3SEPD_EventPlaneInfo_Psi_2"] = std::make_unique<TH3F>("h3SEPD_EventPlaneInfo_Psi_2", "sEPD #Psi (Order 2): |z| < 10 cm and MB; 2#Psi^{S}_{2}; 2#Psi^{N}_{2}; Centrality [%]", m_bins_psi, m_psi_low, m_psi_high, m_bins_psi, m_psi_low, m_psi_high, m_bins_cent, m_cent_low, m_cent_high);

  m_hists["h3SEPD_EventPlaneInfo_Psi_3"] = std::make_unique<TH3F>("h3SEPD_EventPlaneInfo_Psi_3", "sEPD #Psi (Order 3): |z| < 10 cm and MB; 3#Psi^{S}_{3}; 3#Psi^{N}_{3}; Centrality [%]", m_bins_psi, m_psi_low, m_psi_high, m_bins_psi, m_psi_low, m_psi_high, m_bins_cent, m_cent_low, m_cent_high);

  for (unsigned int i = 0; i < m_eventType.size(); ++i)
  {
    m_hists["hEvent"]->GetXaxis()->SetBinLabel(i + 1, m_eventType[i].c_str());
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int sEPDValidation::process_event_check(PHCompositeNode *topNode)
{
  m_hists["hEvent"]->Fill(static_cast<std::uint8_t>(EventType::ALL));

  // zvertex
  m_zvtx = -9999;
  GlobalVertexMap *vertexmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");

  if (!vertexmap)
  {
    std::cout << "sEPDValidation::process_event - Error can not find global vertex node " << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  if (!vertexmap->empty())
  {
    GlobalVertex *vtx = vertexmap->begin()->second;
    m_zvtx = vtx->get_z();
  }

  m_hists["hVtxZ"]->Fill(m_zvtx);

  if (fabs(m_zvtx) < m_zvtx_max)
  {
    m_hists["hEvent"]->Fill(static_cast<std::uint8_t>(EventType::ZVTX10));
  }

  MinimumBiasInfo *m_mb_info = findNode::getClass<MinimumBiasInfo>(topNode, "MinimumBiasInfo");
  if (!m_mb_info)
  {
    std::cout << "sEPDValidation::process_event - Error can not find MinimumBiasInfo node " << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  // skip event if not minimum bias
  if (!m_mb_info->isAuAuMinimumBias())
  {
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  m_hists["hVtxZ_MB"]->Fill(m_zvtx);

  // skip event if zvtx is too large
  if (fabs(m_zvtx) >= m_zvtx_max)
  {
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  m_hists["hEvent"]->Fill(static_cast<std::uint8_t>(EventType::ZVTX10_MB));

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int sEPDValidation::process_centrality(PHCompositeNode *topNode)
{
  CentralityInfo *centInfo = findNode::getClass<CentralityInfo>(topNode, "CentralityInfo");
  if (!centInfo)
  {
    std::cout << "sEPDValidation::process_event - Error can not find Centrality Info node " << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  m_cent = centInfo->get_centile(CentralityInfo::PROP::mbd_NS)*100;

  m_hists["hCentrality"]->Fill(m_cent);

  JetUtils::update_min_max(m_cent, m_cent_min, m_cent_max);

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int sEPDValidation::process_sEPD(PHCompositeNode *topNode)
{
  TowerInfoContainer *towerinfosEPD = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_SEPD");
  if (!towerinfosEPD)
  {
    std::cout << "sEPDValidation::process_event - Error can not find TOWERINFO_CALIB_SEPD node " << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  EpdGeom *epdgeom = findNode::getClass<EpdGeom>(topNode, "TOWERGEOM_EPD");
  if (!epdgeom)
  {
    std::cout << "sEPDValidation::process_event - Error can not find TOWERGEOM_EPD node " << std::endl;
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

  for (unsigned int channel = 0; channel < nchannels_epd; ++channel)
  {
    unsigned int key = TowerInfoDefs::encode_epd(channel);

    TowerInfo *tower = towerinfosEPD->get_tower_at_channel(channel);

    double charge = tower->get_energy();
    bool isZS     = tower->get_isZS();
    double phi    = epdgeom->get_phi(key);

    // exclude ZS
    if(isZS)
    {
      ++m_ctr["sepd_tower_zs"];
      continue;
    }


    dynamic_cast<TH2 *>(m_hists["h2SEPD_Charge"].get())->Fill(charge, m_cent);
    JetUtils::update_min_max(charge, m_sepd_charge_min, m_sepd_charge_max);

    // expecting Nmips
    if(charge < m_sepd_charge_threshold)
    {
      ++m_ctr["sepd_tower_charge_below_threshold"];
      continue;
    }

    JetUtils::update_min_max(phi, m_sepd_phi_min, m_sepd_phi_max);

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

  m_sepd_Q_min = std::min(m_sepd_Q_min, std::min(Q_S_x_2,
                                        std::min(Q_S_y_2,
                                        std::min(Q_N_x_2,
                                        std::min(Q_N_y_2,
                                        std::min(Q_S_x_3,
                                        std::min(Q_S_y_3,
                                        std::min(Q_N_x_3, Q_N_y_3))))))));

  m_sepd_Q_max = std::max(m_sepd_Q_max, std::max(Q_S_x_2,
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

  dynamic_cast<TH3 *>(m_hists["h3SEPD_Total_Charge"].get())->Fill(sepd_total_charge_south, sepd_total_charge_north, m_cent);

  JetUtils::update_min_max(sepd_total_charge_south, m_sepd_total_charge_south_min, m_sepd_total_charge_south_max);
  JetUtils::update_min_max(sepd_total_charge_north, m_sepd_total_charge_north_min, m_sepd_total_charge_north_max);

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

  m_psi_min = std::min(m_psi_min, std::min(psi_n_south, psi_n_north));
  m_psi_max = std::max(m_psi_max, std::max(psi_n_south, psi_n_north));

  dynamic_cast<TH3 *>(m_hists["h3SEPD_EventPlaneInfo_Psi_" + std::to_string(order)].get())->Fill(psi_n_south, psi_n_north, m_cent);

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int sEPDValidation::process_EventPlane(PHCompositeNode *topNode)
{
  // get event plane map
  EventplaneinfoMap *epmap = findNode::getClass<EventplaneinfoMap>(topNode, "EventplaneinfoMap");
  if (!epmap)
  {
    std::cout << "sEPDValidation::process_event - Error can not find EventplaneinfoMap node " << std::endl;
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
  EventHeader *eventInfo = findNode::getClass<EventHeader>(topNode, "EventHeader");
  if (!eventInfo)
  {
    std::cout << PHWHERE << "sEPDValidation::process_event - Fatal Error - EventHeader node is missing. " << std::endl;
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

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int sEPDValidation::End([[maybe_unused]] PHCompositeNode *topNode)
{
  std::cout << "sEPDValidation::End" << std::endl;

  std::cout << "=====================" << std::endl;
  std::cout << "stats" << std::endl;
  std::cout << "Cent: Min: " << m_cent_min << ", Max: " << m_cent_max << std::endl;
  std::cout << "sEPD Charge: Min: " << m_sepd_charge_min << ", Max: " << m_sepd_charge_max << std::endl;
  std::cout << "sEPD Q: Min: " << m_sepd_Q_min << ", Max: " << m_sepd_Q_max << std::endl;
  std::cout << "sEPD Total Charge South: Min: " << m_sepd_total_charge_south_min << ", Max: " << m_sepd_total_charge_south_max << std::endl;
  std::cout << "sEPD Total Charge North: Min " << m_sepd_total_charge_north_min << ", Max: " << m_sepd_total_charge_north_max << std::endl;
  std::cout << "sEPD Phi: Min " << m_sepd_phi_min << ", Max: " << m_sepd_phi_max << std::endl;
  std::cout << "Psi: Min " << m_psi_min << ", Max: " << m_psi_max << std::endl;
  std::cout << "=====================" << std::endl;
  std::cout << "sEPD" << std::endl;
  std::cout << "Avg towers with unknown arm: " << m_ctr["sepd_tower_unknown_arm"] / m_hists["hEvent"]->GetBinContent(static_cast<std::uint8_t>(EventType::ZVTX10_MB)+1) << std::endl;
  std::cout << "Avg towers ZS: " << m_ctr["sepd_tower_zs"] / m_hists["hEvent"]->GetBinContent(static_cast<std::uint8_t>(EventType::ZVTX10_MB)+1) << std::endl;
  std::cout << "Avg towers with charge below threshold: " << m_ctr["sepd_tower_charge_below_threshold"] / m_hists["hEvent"]->GetBinContent(static_cast<std::uint8_t>(EventType::ZVTX10_MB)+1) << std::endl;
  std::cout << "=====================" << std::endl;
  std::cout << "Abort Events Types" << std::endl;
  std::cout << "process sEPD, total charge zero: " << m_ctr["process_sEPD_total_charge_zero"] << std::endl;
  std::cout << "process Event Plane, Q mag zero: " << m_ctr["process_EventPlane_Q_mag_zero"] << std::endl;
  std::cout << "process Event Plane, epmap empty: " << m_ctr["process_EventPlane_epmap_empty"] << std::endl;
  std::cout << "process Event Plane, epd invalid: " << m_ctr["process_EventPlane_epd_invalid"] << std::endl;
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
    hist->Write();
  }

  // local
  dynamic_cast<TH2 *>(m_hists["h2SEPD_Charge"].get())->ProjectionX()->Write();
  dynamic_cast<TH3 *>(m_hists["h3SEPD_Total_Charge"].get())->Project3D("yx")->Write();

  dynamic_cast<TH3 *>(m_hists["h3SEPD_Q_S_2"].get())->Project3D("yx")->Write();
  dynamic_cast<TH3 *>(m_hists["h3SEPD_Q_N_2"].get())->Project3D("yx")->Write();

  dynamic_cast<TH3 *>(m_hists["h3SEPD_Q_S_3"].get())->Project3D("yx")->Write();
  dynamic_cast<TH3 *>(m_hists["h3SEPD_Q_N_3"].get())->Project3D("yx")->Write();

  dynamic_cast<TH3 *>(m_hists["h3SEPD_Psi_2"].get())->Project3D("yx")->Write();
  dynamic_cast<TH3 *>(m_hists["h3SEPD_Psi_3"].get())->Project3D("yx")->Write();

  // Official
  dynamic_cast<TH3 *>(m_hists["h3SEPD_EventPlaneInfo_Psi_2"].get())->Project3D("yx")->Write();
  dynamic_cast<TH3 *>(m_hists["h3SEPD_EventPlaneInfo_Psi_3"].get())->Project3D("yx")->Write();

  output.Close();

  return Fun4AllReturnCodes::EVENT_OK;
}
