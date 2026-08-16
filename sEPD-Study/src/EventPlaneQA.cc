#include "EventPlaneQA.h"

// -- Fun4All
#include <ffaobjects/EventHeader.h>
#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/Fun4AllServer.h>
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

// -- Event Plane
#include <eventplaneinfo/EventplaneinfoMap.h>
#include <eventplaneinfo/Eventplaneinfo.h>

#include <TTree.h>

#include <treefiller/TreeFiller.h>

#include <format>
#include <iostream>

EventPlaneQA::EventPlaneQA(const std::string &name)
  : SubsysReco(name)
{
}

int EventPlaneQA::Init([[maybe_unused]] PHCompositeNode *topNode)
{
  TTree *tree = TreeFiller::getTree();
  if (tree)
  {
    tree->Branch("psi2_raw_S", &m_data.psi2_raw_S);
    tree->Branch("psi2_raw_N", &m_data.psi2_raw_N);
    tree->Branch("psi2_raw_NS", &m_data.psi2_raw_NS);

    tree->Branch("psi2_S", &m_data.psi2_S);
    tree->Branch("psi2_N", &m_data.psi2_N);
    tree->Branch("psi2_NS", &m_data.psi2_NS);
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int EventPlaneQA::process_event(PHCompositeNode *topNode)
{
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

int EventPlaneQA::ResetEvent([[maybe_unused]] PHCompositeNode *topNode)
{
  // sEPD
  m_data.psi2_raw_S = 0;
  m_data.psi2_raw_N = 0;
  m_data.psi2_raw_NS = 0;

  m_data.psi2_S = 0;
  m_data.psi2_N = 0;
  m_data.psi2_NS = 0;

  return Fun4AllReturnCodes::EVENT_OK;
}

int EventPlaneQA::End([[maybe_unused]] PHCompositeNode *topNode)
{
  std::cout << "EventPlaneQA::End" << std::endl;

  return Fun4AllReturnCodes::EVENT_OK;
}
