#include "JetValidation.h"

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

#include <TFile.h>
#include <TH2.h>
#include <TH3.h>

//____________________________________________________________________________..
JetValidation::JetValidation(const std::string &name)
  : SubsysReco(name)
  , m_event(0)
  , m_bins_zvtx(200) /*cm*/
  , m_zvtx_low(-50)  /*cm*/
  , m_zvtx_high(50)  /*cm*/
  , m_bins_cent(100)
  , m_cent_low(-0.5)
  , m_cent_high(99.5)
  , m_zvtx(9999)
  , m_cent(9999)
  , m_zvtx_max(10) /*cm*/
  , m_cent_min(9999)
  , m_cent_max(0)
{
}

//____________________________________________________________________________..
int JetValidation::Init([[maybe_unused]] PHCompositeNode *topNode)
{
  Fun4AllServer *se = Fun4AllServer::instance();
  se->Print("NODETREE");

  // init hists
  m_hists["hEvent"] = std::make_unique<TH1F>("hEvent", "Event Type; Type; Events", m_eventType.size(), 0, m_eventType.size());
  m_hists["hVtxZ"] = std::make_unique<TH1F>("hVtxZ", "Z Vertex; z [cm]; Events", m_bins_zvtx, m_zvtx_low, m_zvtx_high);
  m_hists["hVtxZ_MB"] = std::make_unique<TH1F>("hVtxZ_MB", "Z Vertex; z [cm]; Events", m_bins_zvtx, m_zvtx_low, m_zvtx_high);
  m_hists["hCentrality"] = std::make_unique<TH1F>("hCentrality", "Centrality; Centrality [%]; Events", m_bins_cent, m_cent_low, m_cent_high);

  for (unsigned int i = 0; i < m_eventType.size(); ++i)
  {
    m_hists["hEvent"]->GetXaxis()->SetBinLabel(i + 1, m_eventType[i].c_str());
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int JetValidation::process_event_check(PHCompositeNode *topNode)
{
  m_hists["hEvent"]->Fill(static_cast<std::uint8_t>(EventType::ALL));

  // zvertex
  m_zvtx = -9999;
  GlobalVertexMap *vertexmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");

  if (!vertexmap)
  {
    std::cout << "JetValidation::process_event - Error can not find global vertex node " << std::endl;
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
    std::cout << "JetValidation::process_event - Error can not find MinimumBiasInfo node " << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  // skip event if not minimum bias
  if (!m_mb_info->isAuAuMinimumBias())
  {
    ++m_ctr["abort_not_minbias"];
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  m_hists["hVtxZ_MB"]->Fill(m_zvtx);

  // skip event if zvtx is too large
  if (fabs(m_zvtx) >= m_zvtx_max)
  {
    ++m_ctr["abort_zvtx_high"];
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  m_hists["hEvent"]->Fill(static_cast<std::uint8_t>(EventType::ZVTX10_MB));

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int JetValidation::process_centrality(PHCompositeNode *topNode)
{
  CentralityInfo *centInfo = findNode::getClass<CentralityInfo>(topNode, "CentralityInfo");
  if (!centInfo)
  {
    std::cout << "JetValidation::process_event - Error can not find Centrality Info node " << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  m_cent = centInfo->get_centile(CentralityInfo::PROP::mbd_NS)*100;

  if(Verbosity()) {
    std::cout << "Cent: " << m_cent << std::endl;
  }

  m_hists["hCentrality"]->Fill(m_cent);

  m_cent_min = std::min(m_cent_min, m_cent);
  m_cent_max = std::max(m_cent_max, m_cent);

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int JetValidation::process_event(PHCompositeNode *topNode)
{
  EventHeader *eventInfo = findNode::getClass<EventHeader>(topNode, "EventHeader");
  if (!eventInfo)
  {
    std::cout << PHWHERE << "JetValidation::process_event - Fatal Error - EventHeader node is missing. " << std::endl;
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

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int JetValidation::End([[maybe_unused]] PHCompositeNode *topNode)
{
  std::cout << "JetValidation::End" << std::endl;

  std::cout << "=====================" << std::endl;
  std::cout << "stats" << std::endl;
  std::cout << "Cent: Min: " << m_cent_min << ", Max: " << m_cent_max << std::endl;
  std::cout << "=====================" << std::endl;
  std::cout << "Abort Events Types" << std::endl;
  std::cout << "No Min Bias: " << m_ctr["abort_not_minbias"] << std::endl;
  std::cout << "|z| > 10 cm: " << m_ctr["abort_zvtx_high"] << std::endl;
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

  output.Close();

  return Fun4AllReturnCodes::EVENT_OK;
}
