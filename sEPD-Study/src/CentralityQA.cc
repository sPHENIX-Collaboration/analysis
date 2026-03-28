#include "CentralityQA.h"

// -- Fun4All
#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/Fun4AllServer.h>
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

// -- event
#include <ffaobjects/EventHeader.h>

// -- MB
#include <calotrigger/MinimumBiasInfo.h>
#include <centrality/CentralityInfo.h>

// -- Vtx
#include <globalvertex/GlobalVertex.h>
#include <globalvertex/GlobalVertexMap.h>

#include <TH1F.h>
#include <TH2F.h>

#include <format>
#include <iostream>

CentralityQA::CentralityQA(const std::string &name)
  : SubsysReco(name)
{
}

int CentralityQA::Init([[maybe_unused]] PHCompositeNode *topNode)
{
  Fun4AllServer *se = Fun4AllServer::instance();

  // Event Counter Histogram
  hEvent = new TH1F("hEvent", "Event Selection; Type; Events", m_eventType.size(), 0, m_eventType.size());
  for (unsigned int i = 0; i < m_eventType.size(); ++i)
  {
    hEvent->GetXaxis()->SetBinLabel(i + 1, m_eventType[i].c_str());
  }
  se->registerHisto(hEvent);

  // Centrality Histograms
  hCentrality = new TH1F("hCentrality", "|z| < 10 cm and MB; Centrality [%]; Events", m_bins_cent, m_cent_low, m_cent_high);
  hCentralityZ50 = new TH1F("hCentralityZ50", "|z| < 50 cm and MB; Centrality [%]; Events", m_bins_cent, m_cent_low, m_cent_high);
  hCentralityZOuter = new TH1F("hCentralityZOuter", "10 cm < |z| < 50 cm and MB; Centrality [%]; Events", m_bins_cent, m_cent_low, m_cent_high);

  se->registerHisto(hCentrality);
  se->registerHisto(hCentralityZ50);
  se->registerHisto(hCentralityZOuter);

  // Vertex Histograms
  unsigned int bins_zvtx{200};
  double zvtx_low{-50};
  double zvtx_high{50};

  hZVertex = new TH1F("hZVertex", "Min Bias; Z [cm]; Events", bins_zvtx, zvtx_low, zvtx_high);
  h2ZVertexCentrality = new TH2F("h2ZVertexCentrality", "Min Bias; Z [cm]; Centrality [%]", bins_zvtx, zvtx_low, zvtx_high, m_bins_cent, m_cent_low, m_cent_high);

  se->registerHisto(hZVertex);
  se->registerHisto(h2ZVertexCentrality);

  return Fun4AllReturnCodes::EVENT_OK;
}

int CentralityQA::process_event(PHCompositeNode *topNode)
{
  EventHeader *eventInfo = findNode::getClass<EventHeader>(topNode, "EventHeader");
  if (!eventInfo)
  {
    return Fun4AllReturnCodes::ABORTRUN;
  }

  m_globalEvent = eventInfo->get_EvtSequence();

  if ((Verbosity() && m_ctr["events_total"] % 1000 == 0) || Verbosity() > 1)
  {
    std::cout << std::format("Progress: {}, Global: {}", m_ctr["events_total"], m_globalEvent) << std::endl;
  }
  ++m_ctr["events_total"];

  int ret = process_event_check(topNode);
  if (ret != Fun4AllReturnCodes::EVENT_OK)
  {
    return ret;
  }

  ret = process_centrality(topNode);
  if (ret != Fun4AllReturnCodes::EVENT_OK)
  {
    return ret;
  }

  ++m_ctr["events_good"];
  return Fun4AllReturnCodes::EVENT_OK;
}

int CentralityQA::process_event_check(PHCompositeNode *topNode)
{
  hEvent->Fill(static_cast<int>(EventType::ALL));

  // 1. Get Minimum Bias Info
  MinimumBiasInfo *mb_info = findNode::getClass<MinimumBiasInfo>(topNode, "MinimumBiasInfo");
  if (!mb_info)
  {
    return Fun4AllReturnCodes::ABORTRUN;
  }

  // 2. Get Vertex Info
  GlobalVertexMap *vertexmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");
  if (!vertexmap)
  {
    return Fun4AllReturnCodes::ABORTRUN;
  }
  if (vertexmap->empty())
  {
    ++m_ctr["events_no_vertex"];
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  GlobalVertex *vtx = vertexmap->begin()->second;
  m_zvtx = vtx->get_z();

  hEvent->Fill(static_cast<int>(EventType::ZVTX));

  // Vertex Cuts Logic
  if (std::abs(m_zvtx) < m_zvtx_max_v2)
  {
    hEvent->Fill(static_cast<int>(EventType::ZVTX50));
    if (std::abs(m_zvtx) < m_zvtx_max)
    {
      hEvent->Fill(static_cast<int>(EventType::ZVTX10));
      m_pass_Zvtx = true;
    }
  }

  // Minimum Bias Filter
  if (!mb_info->isAuAuMinimumBias())
  {
    return Fun4AllReturnCodes::EVENT_OK;
  }
  m_pass_MB = true;

  // Fill QA Plots
  hZVertex->Fill(m_zvtx);

  if (m_pass_Zvtx)
  {
    hEvent->Fill(static_cast<int>(EventType::ZVTX10_MB));
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

int CentralityQA::process_centrality(PHCompositeNode *topNode)
{
  if (!m_pass_MB)
  {
    return Fun4AllReturnCodes::EVENT_OK;
  }

  CentralityInfo *centInfo = findNode::getClass<CentralityInfo>(topNode, "CentralityInfo");
  if (!centInfo)
  {
    return Fun4AllReturnCodes::ABORTRUN;
  }

  m_cent = centInfo->get_centile(CentralityInfo::PROP::mbd_NS) * 100;

  if (!std::isfinite(m_cent) || m_cent < 0 || m_cent >= m_cent_high)
  {
    std::cout << std::format("[WARNING] Event {}: Invalid centrality centile ({:.2f}). "
                             "Expected finite value in range [0, {}).",
                             m_globalEvent, m_cent, m_cent_high) << std::endl;

    ++m_ctr["events_centrality_bad"];
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  h2ZVertexCentrality->Fill(m_zvtx, m_cent);

  if (std::abs(m_zvtx) < m_zvtx_max_v2)
  {
    hCentralityZ50->Fill(m_cent);
    if (!m_pass_Zvtx)
    {
      hCentralityZOuter->Fill(m_cent);
    }
  }

  if (m_pass_Zvtx)
  {
    if (m_cent < m_cent_max_threshold_ana)
    {
      hEvent->Fill(static_cast<int>(EventType::ZVTX10_MB_CENT));
    }

    hCentrality->Fill(m_cent);
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

int CentralityQA::ResetEvent([[maybe_unused]] PHCompositeNode *topNode)
{
  m_zvtx = -9999;
  m_pass_Zvtx = false;
  m_pass_MB = false;
  return Fun4AllReturnCodes::EVENT_OK;
}

int CentralityQA::End([[maybe_unused]] PHCompositeNode *topNode)
{
  std::cout << "--- CentralityQA Stats ---" << std::endl;
  for (const auto &[name, counts] : m_ctr)
  {
    std::cout << std::format("{}: {}", name, counts) << std::endl;
  }
  return Fun4AllReturnCodes::EVENT_OK;
}
