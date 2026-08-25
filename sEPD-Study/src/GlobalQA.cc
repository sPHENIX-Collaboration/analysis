#include "GlobalQA.h"

// -- Fun4All
#include <ffaobjects/EventHeader.h>
#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/Fun4AllServer.h>
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

// -- Event Plane
#include <eventplaneinfo/EventplaneinfoMap.h>
#include <eventplaneinfo/Eventplaneinfo.h>

// -- Calo
#include <calobase/TowerInfo.h>
#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfoDefs.h>

// -- MBD / MinBias
#include <pdbcalbase/PdbParameterMap.h>
#include <phparameter/PHParameters.h>

#include <TTree.h>

#include <treefiller/TreeFiller.h>

#include <format>
#include <iostream>

GlobalQA::GlobalQA(const std::string &name)
  : SubsysReco(name)
{
}

int GlobalQA::Init([[maybe_unused]] PHCompositeNode *topNode)
{
  TTree *tree = TreeFiller::getTree();
  if (tree)
  {
    // sEPD - Event Plane
    if (m_do_ep)
    {
      tree->Branch("psi2_raw_S", &m_data.psi2_raw_S);
      tree->Branch("psi2_raw_N", &m_data.psi2_raw_N);
      tree->Branch("psi2_raw_NS", &m_data.psi2_raw_NS);

      tree->Branch("psi2_S", &m_data.psi2_S);
      tree->Branch("psi2_N", &m_data.psi2_N);
      tree->Branch("psi2_NS", &m_data.psi2_NS);
    }

    if (m_do_sepd)
    {
      // sEPD - QA
      tree->Branch("sepd_charge_south", &m_data.sepd_charge_south);
      tree->Branch("sepd_charge_north", &m_data.sepd_charge_north);
    }

    if (m_do_mbd)
    {
      // MBD - QA
      tree->Branch("mbd_charge_south", &m_data.mbd_charge_south);
      tree->Branch("mbd_charge_north", &m_data.mbd_charge_north);
    }
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int GlobalQA::process_event(PHCompositeNode *topNode)
{
  int ret = Fun4AllReturnCodes::EVENT_OK;

  if (m_do_ep)
  {
    ret = process_event_plane(topNode);
  }

  if (ret != Fun4AllReturnCodes::EVENT_OK)
  {
    return ret;
  }

  if (m_do_sepd)
  {
    ret = process_sepd(topNode);
  }

  if (ret != Fun4AllReturnCodes::EVENT_OK)
  {
    return ret;
  }

  if (m_do_mbd)
  {
    ret = process_mbd(topNode);
  }

  return ret;
}

//____________________________________________________________________________..
int GlobalQA::process_event_plane(PHCompositeNode *topNode)
{
  EventHeader *eventInfo = findNode::getClass<EventHeader>(topNode, "EventHeader");
  int event_id = eventInfo ? eventInfo->get_EvtSequence() : -1;

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

  if (Verbosity() > 0)
  {
    std::cout << "GlobalQA::process_event_plane - [Event " << event_id << "] "
              << "psi2_raw (S/N/NS): " << m_data.psi2_raw_S << " / " << m_data.psi2_raw_N << " / " << m_data.psi2_raw_NS
              << " | psi2 (S/N/NS): " << m_data.psi2_S << " / " << m_data.psi2_N << " / " << m_data.psi2_NS
              << std::endl;
  }
  if (Verbosity() > 1)
  {
    std::cout << "    Q2_raw: S=(" << Q_S_2_raw.first << ", " << Q_S_2_raw.second << ")"
              << " N=(" << Q_N_2_raw.first << ", " << Q_N_2_raw.second << ")"
              << " NS=(" << Q_NS_2_raw.first << ", " << Q_NS_2_raw.second << ")" << std::endl
              << "    Q2_calib: S=(" << Q_S_2.first << ", " << Q_S_2.second << ")"
              << " N=(" << Q_N_2.first << ", " << Q_N_2.second << ")"
              << " NS=(" << Q_NS_2.first << ", " << Q_NS_2.second << ")" << std::endl;
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int GlobalQA::process_sepd(PHCompositeNode *topNode)
{
  EventHeader *eventInfo = findNode::getClass<EventHeader>(topNode, "EventHeader");
  int event_id = eventInfo ? eventInfo->get_EvtSequence() : -1;

  TowerInfoContainer *towerinfosEPD = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_SEPD");
  if (!towerinfosEPD)
  {
    return Fun4AllReturnCodes::ABORTRUN;
  }

  // sepd
  unsigned int nchannels_epd = towerinfosEPD->size();

  double sepd_total_charge_south = 0;
  double sepd_total_charge_north = 0;
  unsigned int nhits_south = 0;
  unsigned int nhits_north = 0;

  for (unsigned int channel = 0; channel < nchannels_epd; ++channel)
  {
    unsigned int key = TowerInfoDefs::encode_epd(channel);

    TowerInfo *tower = towerinfosEPD->get_tower_at_channel(channel);
    if (!tower)
    {
      continue;
    }

    double charge = tower->get_energy();

    unsigned int arm = TowerInfoDefs::get_epd_arm(key);

    // skip charge below minimum threshold
    if (charge < m_sepd_channel_threshold)
    {
      continue;
    }

    if (arm == 0)
    {
      sepd_total_charge_south += charge;
      ++nhits_south;
    }
    else
    {
      sepd_total_charge_north += charge;
      ++nhits_north;
    }
  }

  m_data.sepd_charge_south = sepd_total_charge_south;
  m_data.sepd_charge_north = sepd_total_charge_north;

  if (Verbosity() > 0)
  {
    std::cout << "GlobalQA::process_sepd - [Event " << event_id << "] "
              << "Charge S: " << m_data.sepd_charge_south << " (hits: " << nhits_south << ")"
              << " | Charge N: " << m_data.sepd_charge_north << " (hits: " << nhits_north << ")"
              << " | Total: " << (m_data.sepd_charge_south + m_data.sepd_charge_north)
              << " (threshold: " << m_sepd_channel_threshold << ")"
              << std::endl;
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int GlobalQA::process_mbd(PHCompositeNode *topNode)
{
  EventHeader *eventInfo = findNode::getClass<EventHeader>(topNode, "EventHeader");
  int event_id = eventInfo ? eventInfo->get_EvtSequence() : -1;

  PdbParameterMap *pdb = findNode::getClass<PdbParameterMap>(topNode, "MinBiasParams");
  if (!pdb)
  {
    return Fun4AllReturnCodes::ABORTRUN;
  }

  PHParameters pdb_params("MinBiasParams");
  pdb_params.FillFrom(pdb);

  double mbd_total_charge_south = pdb_params.get_double_param("minbias_mbd_total_charge_south");
  double mbd_total_charge_north = pdb_params.get_double_param("minbias_mbd_total_charge_north");

  m_data.mbd_charge_south = mbd_total_charge_south;
  m_data.mbd_charge_north = mbd_total_charge_north;

  if (Verbosity() > 0)
  {
    std::cout << "GlobalQA::process_mbd - [Event " << event_id << "] "
              << "Charge S: " << m_data.mbd_charge_south
              << " | Charge N: " << m_data.mbd_charge_north
              << " | Total: " << (m_data.mbd_charge_south + m_data.mbd_charge_north)
              << std::endl;
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

int GlobalQA::ResetEvent([[maybe_unused]] PHCompositeNode *topNode)
{
  // sEPD - Event Plane
  m_data.psi2_raw_S = 0;
  m_data.psi2_raw_N = 0;
  m_data.psi2_raw_NS = 0;

  m_data.psi2_S = 0;
  m_data.psi2_N = 0;
  m_data.psi2_NS = 0;

  // sEPD - QA
  m_data.sepd_charge_south = 0;
  m_data.sepd_charge_north = 0;

  // MBD - QA
  m_data.mbd_charge_south = 0;
  m_data.mbd_charge_north = 0;

  return Fun4AllReturnCodes::EVENT_OK;
}

int GlobalQA::End([[maybe_unused]] PHCompositeNode *topNode)
{
  std::cout << "GlobalQA::End" << std::endl;

  return Fun4AllReturnCodes::EVENT_OK;
}
