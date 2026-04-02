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

// Trigger
#include <calotrigger/TriggerAnalyzer.h>

#include <TH1F.h>
#include <TH2F.h>

#include <algorithm>
#include <iterator>
#include <format>
#include <iostream>

CentralityQA::CentralityQA(const std::string &name)
: SubsysReco(name),
  hZVertexTrig(m_triggernames.size()),
  hCentralityTrig(m_triggernames.size()),
  hCentralityZ50Trig(m_triggernames.size()),
  hCentralityZOuterTrig(m_triggernames.size()),
  h2ZVertexCentralityTrig(m_triggernames.size())
{
}

int CentralityQA::Init([[maybe_unused]] PHCompositeNode *topNode)
{
  Fun4AllServer *se = Fun4AllServer::instance();

  triggeranalyzer = new TriggerAnalyzer();

  // Event Counter Histogram
  std::vector<std::string> eventType{"All", "Has Z", "|z| < 50 cm", "|z| < 10 cm", "MB", "Cent"};

  hEvent = new TH1F("hEvent", "Event Selection; Type; Events", eventType.size(), 0, eventType.size());
  for (unsigned int i = 0; i < eventType.size(); ++i)
  {
    hEvent->GetXaxis()->SetBinLabel(i + 1, eventType[i].c_str());
  }
  se->registerHisto(hEvent);

  // Event Trigger Counter Histogram
  std::vector<std::string> eventTypeTrigger{"|z| < 10 cm"};
  std::ranges::copy(m_triggernames, std::back_inserter(eventTypeTrigger));

  hEventTrigger = new TH1F("hEventTrigger", "Event Selection; Type; Events", eventTypeTrigger.size(), 0, eventTypeTrigger.size());
  for (unsigned int i = 0; i < eventTypeTrigger.size(); ++i)
  {
    hEventTrigger->GetXaxis()->SetBinLabel(i + 1, eventTypeTrigger[i].c_str());
  }
  se->registerHisto(hEventTrigger);

  // Centrality Histograms
  hCentrality = new TH1F("hCentrality", "|z| < 10 cm and MB; Centrality [%]; Events", m_bins_cent, m_cent_low, m_cent_high);
  hCentralityZ50 = new TH1F("hCentralityZ50", "|z| < 50 cm and MB; Centrality [%]; Events", m_bins_cent, m_cent_low, m_cent_high);
  hCentralityZOuter = new TH1F("hCentralityZOuter", "10 cm < |z| < 50 cm and MB; Centrality [%]; Events", m_bins_cent, m_cent_low, m_cent_high);

  se->registerHisto(hCentrality);
  se->registerHisto(hCentralityZ50);
  se->registerHisto(hCentralityZOuter);

  for (int i = 0, triggerIdx = 10; const auto &trig : m_triggernames)
  {
    std::string title_centrality = std::format("|z| < 10 cm and {}; Centrality [%]; Events", trig);
    std::string title_centralityZ50 = std::format("|z| < 50 cm and {}; Centrality [%]; Events", trig);
    std::string title_centralityZOuter = std::format("10 cm < |z| < 50 cm and {}; Centrality [%]; Events", trig);

    std::string name_centrality = std::format("hCentrality_Trig{}", triggerIdx);
    std::string name_centralityZ50 = std::format("hCentralityZ50_Trig{}", triggerIdx);
    std::string name_centralityZOuter = std::format("hCentralityZOuter_Trig{}", triggerIdx);

    hCentralityTrig[i] = new TH1F(name_centrality.c_str(), title_centrality.c_str(), m_bins_cent, m_cent_low, m_cent_high);
    hCentralityZ50Trig[i] = new TH1F(name_centralityZ50.c_str(), title_centralityZ50.c_str(), m_bins_cent, m_cent_low, m_cent_high);
    hCentralityZOuterTrig[i] = new TH1F(name_centralityZOuter.c_str(), title_centralityZOuter.c_str(), m_bins_cent, m_cent_low, m_cent_high);

    se->registerHisto(hCentralityTrig[i]);
    se->registerHisto(hCentralityZ50Trig[i]);
    se->registerHisto(hCentralityZOuterTrig[i]);

    ++i;
    triggerIdx += 2;
  }

  // Vertex Histograms
  unsigned int bins_zvtx{200};
  double zvtx_low{-50};
  double zvtx_high{50};

  hZVertex = new TH1F("hZVertex", "Min Bias; Z [cm]; Events", bins_zvtx, zvtx_low, zvtx_high);
  h2ZVertexCentrality = new TH2F("h2ZVertexCentrality", "Min Bias; Z [cm]; Centrality [%]", bins_zvtx, zvtx_low, zvtx_high, m_bins_cent, m_cent_low, m_cent_high);

  se->registerHisto(hZVertex);
  se->registerHisto(h2ZVertexCentrality);

  for (int i = 0, triggerIdx = 10; const auto &trig : m_triggernames)
  {
    std::string title_h1 = std::format("{}; Z [cm]; Events", trig);
    std::string title_h2 = std::format("{}; Z [cm]; Centrality [%]", trig);

    std::string name_h1 = std::format("hZVertex_Trig{}", triggerIdx);
    std::string name_h2 = std::format("h2ZVertexCentrality_Trig{}", triggerIdx);

    hZVertexTrig[i] = new TH1F(name_h1.c_str(), title_h1.c_str(), bins_zvtx, zvtx_low, zvtx_high);
    h2ZVertexCentralityTrig[i] = new TH2F(name_h2.c_str(), title_h2.c_str(), bins_zvtx, zvtx_low, zvtx_high, m_bins_cent, m_cent_low, m_cent_high);

    se->registerHisto(hZVertexTrig[i]);
    se->registerHisto(h2ZVertexCentralityTrig[i]);

    ++i;
    triggerIdx += 2;
  }

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
      hEventTrigger->Fill(0);
      m_pass_Zvtx = true;
    }
  }

  triggeranalyzer->decodeTriggers(topNode);

  if (Verbosity() > 2)
  {
    std::cout << std::format("Z Vtx: {:.2f} cm", m_zvtx) << std::endl;
    triggeranalyzer->Print();
  }

  if (m_pass_Zvtx)
  {
    for (int i = static_cast<int>(EventType::TRIG10); const auto &trig : m_triggernames)
    {
      if (triggeranalyzer->didTriggerFire(trig))
      {
        hEventTrigger->Fill(i);

        if (Verbosity() > 2)
        {
          std::cout << std::format("Trigger {} Fired", trig) << std::endl;
        }
      }
      ++i;
    }
  }

  for (int i = 0; const auto &trig : m_triggernames)
  {
    if (triggeranalyzer->didTriggerFire(trig))
    {
      hZVertexTrig[i]->Fill(m_zvtx);
    }
    ++i;
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

  if(m_pass_MB)
  {
    h2ZVertexCentrality->Fill(m_zvtx, m_cent);

    if (std::abs(m_zvtx) < m_zvtx_max_v2)
    {
      hCentralityZ50->Fill(m_cent);

      if (m_pass_Zvtx)
      {
        if (m_cent < m_cent_max_threshold_ana)
        {
          hEvent->Fill(static_cast<int>(EventType::ZVTX10_MB_CENT));
        }

        hCentrality->Fill(m_cent);
      }
      else
      {
        hCentralityZOuter->Fill(m_cent);
      }
    }
  }

  for (int i = 0; const auto &trig : m_triggernames)
  {
    if (triggeranalyzer->didTriggerFire(trig))
    {
      h2ZVertexCentralityTrig[i]->Fill(m_zvtx, m_cent);

      if (std::abs(m_zvtx) < m_zvtx_max_v2)
      {
        hCentralityZ50Trig[i]->Fill(m_cent);

        if (m_pass_Zvtx)
        {
          hCentralityTrig[i]->Fill(m_cent);
        }
        else
        {
          hCentralityZOuterTrig[i]->Fill(m_cent);
        }
      }
    }
    ++i;
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
