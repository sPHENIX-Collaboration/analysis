#include "TriggerQA.h"

// -- Fun4All
#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/Fun4AllServer.h>
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

// -- event
#include <ffaobjects/EventHeader.h>

// -- Vtx
#include <globalvertex/GlobalVertex.h>
#include <globalvertex/GlobalVertexMap.h>

// Trigger
#include <calotrigger/TriggerAnalyzer.h>

#include <TH1F.h>

#include <format>
#include <iostream>

TriggerQA::TriggerQA(const std::string &name)
: SubsysReco(name)
{
}

TriggerQA::~TriggerQA() = default;

int TriggerQA::Init([[maybe_unused]] PHCompositeNode *topNode)
{
  Fun4AllServer *se = Fun4AllServer::instance();

  m_triggerAnalyzer = std::make_unique<TriggerAnalyzer>();

  // Event Counter Histogram
  std::vector<std::string> eventType{"All", "Has Z", "|z| < 10 cm", "Trig 10", "Trig 12", "Trig 14", "|z| < 10 cm & Trig 10", "|z| < 10 cm & Trig 12", "|z| < 10 cm & Trig 14"};

  hEvent = new TH1F("hEvent", "; Type; Events", eventType.size(), 0, eventType.size());
  for (unsigned int i = 0; i < eventType.size(); ++i)
  {
    hEvent->GetXaxis()->SetBinLabel(i + 1, eventType[i].c_str());
  }
  se->registerHisto(hEvent);

  // Vertex Histograms
  unsigned int bins_zvtx{200};
  double zvtx_low{-50};
  double zvtx_high{50};

  hZVertex = new TH1F("hZVertex", "; Z [cm]; Events", bins_zvtx, zvtx_low, zvtx_high);
  hZVertex_Trig10 = dynamic_cast<TH1*>(hZVertex->Clone("hZVertex_Trig10"));
  hZVertex_Trig12 = dynamic_cast<TH1*>(hZVertex->Clone("hZVertex_Trig12"));
  hZVertex_Trig14 = dynamic_cast<TH1*>(hZVertex->Clone("hZVertex_Trig14"));

  se->registerHisto(hZVertex);
  se->registerHisto(hZVertex_Trig10);
  se->registerHisto(hZVertex_Trig12);
  se->registerHisto(hZVertex_Trig14);

  // Lumi Labels
  std::vector<std::string> lumiType{"|z| < 10 cm & MBD Trig", "|z| < 10 cm"};

  hLuminosity = new TH1F("hLuminosity", "; Type; Luminosity [nb^{-1}]", lumiType.size(), 0, lumiType.size());
  for (unsigned int i = 0; i < lumiType.size(); ++i)
  {
    hLuminosity->GetXaxis()->SetBinLabel(i + 1, lumiType[i].c_str());
  }
  se->registerHisto(hLuminosity);

  return Fun4AllReturnCodes::EVENT_OK;
}

int TriggerQA::process_event(PHCompositeNode *topNode)
{
  EventHeader *eventInfo = findNode::getClass<EventHeader>(topNode, "EventHeader");
  if (!eventInfo)
  {
    return Fun4AllReturnCodes::ABORTRUN;
  }

  int globalEvent = eventInfo->get_EvtSequence();

  if ((Verbosity() && m_total_events % m_progress_print == 0) || Verbosity() > 1)
  {
    std::cout << std::format("Progress: {}, Global: {}", m_total_events, globalEvent) << std::endl;
  }
  ++m_total_events;

  hEvent->Fill(static_cast<int>(EventType::ALL));

  m_triggerAnalyzer->decodeTriggers(topNode);

  bool didTrig10Fire = m_triggerAnalyzer->didTriggerFire(m_trig_10);
  bool didTrig12Fire = m_triggerAnalyzer->didTriggerFire(m_trig_12);
  bool didTrig14Fire = m_triggerAnalyzer->didTriggerFire(m_trig_14);

  if (didTrig10Fire)
  {
    hEvent->Fill(static_cast<int>(EventType::TRIG10));
  }

  if (didTrig12Fire)
  {
    hEvent->Fill(static_cast<int>(EventType::TRIG12));
  }

  if (didTrig14Fire)
  {
    hEvent->Fill(static_cast<int>(EventType::TRIG14));
  }

  GlobalVertexMap *vertexmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");
  if (!vertexmap)
  {
    return Fun4AllReturnCodes::ABORTRUN;
  }
  if (vertexmap->empty())
  {
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  GlobalVertex *vtx = vertexmap->begin()->second;
  double zvtx = vtx->get_z();

  hZVertex->Fill(zvtx);

  if (didTrig10Fire)
  {
    hZVertex_Trig10->Fill(zvtx);
  }

  if (didTrig12Fire)
  {
    hZVertex_Trig12->Fill(zvtx);
  }

  if (didTrig14Fire)
  {
    hZVertex_Trig14->Fill(zvtx);
  }

  hEvent->Fill(static_cast<int>(EventType::ZVTX));

  if (std::abs(zvtx) < m_zvtx_max)
  {
    hEvent->Fill(static_cast<int>(EventType::ZVTX10));

    if (didTrig10Fire)
    {
      hEvent->Fill(static_cast<int>(EventType::ZVTX10_TRIG10));

      if (Verbosity() > 1)
      {
        std::cout << std::format("Trigger {} Fired and |z| < 10 cm", m_trig_10) << std::endl;
      }
    }

    if (didTrig12Fire)
    {
      hEvent->Fill(static_cast<int>(EventType::ZVTX10_TRIG12));

      if (Verbosity() > 1)
      {
        std::cout << std::format("Trigger {} Fired and |z| < 10 cm", m_trig_12) << std::endl;
      }
    }

    if (didTrig14Fire)
    {
      hEvent->Fill(static_cast<int>(EventType::ZVTX10_TRIG14));

      if (Verbosity() > 1)
      {
        std::cout << std::format("Trigger {} Fired and |z| < 10 cm", m_trig_14) << std::endl;
      }
    }
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

int TriggerQA::ResetEvent([[maybe_unused]] PHCompositeNode *topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

int TriggerQA::End([[maybe_unused]] PHCompositeNode *topNode)
{
  uint64_t raw10 = m_triggerAnalyzer->getTriggerRawScalers(m_trig_10);
  uint64_t raw12 = m_triggerAnalyzer->getTriggerRawScalers(m_trig_12);
  uint64_t raw14 = m_triggerAnalyzer->getTriggerRawScalers(m_trig_14);

  int prescale_10 = m_triggerAnalyzer->getTriggerPrescale(m_trig_10);
  int prescale_12 = m_triggerAnalyzer->getTriggerPrescale(m_trig_12);
  int prescale_14 = m_triggerAnalyzer->getTriggerPrescale(m_trig_14);

  std::cout << std::format("Trigger: {}, Prescale: {}\n", m_trig_10, prescale_10);
  std::cout << std::format("Trigger: {}, Prescale: {}\n", m_trig_12, prescale_12);
  std::cout << std::format("Trigger: {}, Prescale: {}\n", m_trig_14, prescale_14);

  // default
  double lumi_trig = 0.0;
  double lumi_vtx = 0.0;

  // Protect against division by zero. We absolutely need raw12 for the numerator.
  if (raw12 > 0)
  {
    // Use raw14 as the wide baseline, but fall back to raw10 if raw14 is completely dead at the hardware level
    double raw_wide = (raw14 > 0) ? static_cast<double>(raw14) : static_cast<double>(raw10);

    if (raw_wide > 0)
    {
      double zfraction = static_cast<double>(raw12) / raw_wide;
      double sigma_mbd = 6.324;
      double sigma_narrow = sigma_mbd * zfraction;

      // 1st Choice: Standard Narrow Trigger
      if (prescale_12 > 0)
      {
        lumi_trig = hEvent->GetBinContent(static_cast<int>(EventType::ZVTX10_TRIG12) + 1) / sigma_narrow * 1e-9;
      }
      // 2nd Choice: Salvage with Wide Trigger
      else if (prescale_14 > 0)
      {
        lumi_trig = hEvent->GetBinContent(static_cast<int>(EventType::ZVTX10_TRIG14) + 1) / sigma_narrow * 1e-9;
      }
      // 3rd Choice: Salvage with Inclusive MB Trigger
      else if (prescale_10 > 0)
      {
        lumi_trig = hEvent->GetBinContent(static_cast<int>(EventType::ZVTX10_TRIG10) + 1) / sigma_narrow * 1e-9;
      }

      // VTX-only calculation (ignores trigger prescale statuses)
      lumi_vtx = hEvent->GetBinContent(static_cast<int>(EventType::ZVTX10) + 1) / sigma_narrow * 1e-9;
    }
  }

  hLuminosity->SetBinContent(1, lumi_trig);
  hLuminosity->SetBinContent(2, lumi_vtx);

  return Fun4AllReturnCodes::EVENT_OK;
}
