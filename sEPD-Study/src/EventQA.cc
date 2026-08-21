#include "EventQA.h"

#include <treefiller/TreeFiller.h>

#include <format>
#include <iostream>

#include <ffaobjects/EventHeader.h>
#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/Fun4AllServer.h>
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

#include <pdbcalbase/PdbParameterMap.h>
#include <phparameter/PHParameters.h>

#include <globalvertex/GlobalVertex.h>
#include <globalvertex/GlobalVertexMap.h>

#include <calotrigger/MinimumBiasClassifier.h>
#include <calotrigger/MinimumBiasInfo.h>
#include <centrality/CentralityInfo.h>

#include <calotrigger/TriggerAnalyzer.h>

#include <TH1.h>
#include <TTree.h>

EventQA::EventQA(const std::string &name)
  : SubsysReco(name)
{
}

int EventQA::Init([[maybe_unused]] PHCompositeNode *topNode)
{
  Fun4AllServer *se = Fun4AllServer::instance();

  m_triggerAnalyzer = std::make_unique<TriggerAnalyzer>();

  if (m_do_hist)
  {
    hEvent = new TH1F("hEvent", "Event Type; Type; Events", static_cast<unsigned int>(m_eventType.size()), 0, static_cast<double>(m_eventType.size()));
    se->registerHisto(hEvent);

    hEventMinBias = new TH1F("hEventMinBias", "Event Type; Type; Events", static_cast<unsigned int>(m_MinBias_Type.size()), 0, static_cast<double>(m_MinBias_Type.size()));
    se->registerHisto(hEventMinBias);

    hVtxZ = new TH1F("hVtxZ", "Z Vertex; z [cm]; Events", m_hist_config.m_bins_zvtx, m_hist_config.m_zvtx_low, m_hist_config.m_zvtx_high);
    se->registerHisto(hVtxZ);

    hVtxZ_MB = new TH1F("hVtxZ_MB", "Z Vertex; z [cm]; Events", m_hist_config.m_bins_zvtx, m_hist_config.m_zvtx_low, m_hist_config.m_zvtx_high);
    se->registerHisto(hVtxZ_MB);

    hCentrality = new TH1F("hCentrality", "|z| < 10 cm and MB; Centrality [%]; Events", m_hist_config.m_bins_cent, m_hist_config.m_cent_low, m_hist_config.m_cent_high);
    se->registerHisto(hCentrality);

    for (unsigned int i = 0; i < m_eventType.size(); ++i)
    {
      hEvent->GetXaxis()->SetBinLabel(i + 1, m_eventType[i].c_str());
    }

    for (unsigned int i = 0; i < m_MinBias_Type.size(); ++i)
    {
      hEventMinBias->GetXaxis()->SetBinLabel(i + 1, m_MinBias_Type[i].c_str());
    }
  }

  TTree *tree = TreeFiller::getTree();
  if (tree)
  {
    tree->Branch("event", &m_data.event);
    tree->Branch("zvtx", &m_data.zvtx);
    tree->Branch("centrality", &m_data.centrality);
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

int EventQA::process_event_check(PHCompositeNode *topNode)
{
  if (m_do_hist)
  {
    hEvent->Fill(static_cast<std::uint8_t>(EventType::ALL));
  }

  EventHeader *eventInfo = findNode::getClass<EventHeader>(topNode, "EventHeader");
  if (!eventInfo)
  {
    std::cout << "Aborting Run: EventHeader null" << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  m_data.event = eventInfo->get_EvtSequence();

  // zvertex
  double zvtx = -9999;
  GlobalVertexMap *vertexmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");

  if (!vertexmap)
  {
    std::cout << "Aborting Run: GlobalVertexMap null" << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  if (!vertexmap->empty())
  {
    GlobalVertex *vtx = vertexmap->begin()->second;
    if (vtx)
    {
      m_data.zvtx = vtx->get_z();
      zvtx = m_data.zvtx;

      if (m_do_hist)
      {
        hEvent->Fill(static_cast<std::uint8_t>(EventType::ZVTX));
      }
    }
  }

  if (m_do_hist)
  {
    hVtxZ->Fill(zvtx);
  }

  bool pass_zvtx10 = std::abs(zvtx) < m_cuts.m_zvtx_max;

  if (std::abs(zvtx) < m_cuts.m_zvtx_max_v2)
  {
    if (m_do_hist)
    {
      hEvent->Fill(static_cast<std::uint8_t>(EventType::ZVTX50));
      if (pass_zvtx10)
      {
        hEvent->Fill(static_cast<std::uint8_t>(EventType::ZVTX10));
      }
    }
  }

  // MBD Trigger
  m_triggerAnalyzer->decodeTriggers(topNode);

  bool didTrig14Fire = m_triggerAnalyzer->didTriggerFire(m_trig_14);
  bool didTrig12Fire = m_triggerAnalyzer->didTriggerFire(m_trig_12);

  bool mbd_trigger_fire = didTrig12Fire || didTrig14Fire;

  if (pass_zvtx10 && mbd_trigger_fire)
  {
    if (m_do_hist)
    {
      hEvent->Fill(static_cast<std::uint8_t>(EventType::MB_TRIG));
    }
  }

  // Minimum Bias Classifier
  MinimumBiasInfo *m_mb_info = findNode::getClass<MinimumBiasInfo>(topNode, "MinimumBiasInfo");
  if (!m_mb_info)
  {
    std::cout << "Aborting Run: MinimumBiasInfo null" << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  // Minimum Bias Check
  PdbParameterMap *pdb = findNode::getClass<PdbParameterMap>(topNode, "MinBiasParams");
  if (!pdb)
  {
    std::cout << "Aborting Run: PdbParameterMap null" << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  PHParameters pdb_params("MinBiasParams");
  pdb_params.FillFrom(pdb);

  bool minbias_bkg_high = pdb_params.get_int_param("minbias_background_cut_fail");
  bool minbias_side_hit_low = pdb_params.get_int_param("minbias_two_hit_min_fail");
  bool minbias_zdc_low = pdb_params.get_int_param("minbias_zdc_energy_min_fail");
  bool minbias_mbd_high = pdb_params.get_int_param("minbias_mbd_total_energy_max_fail");

  if (m_do_hist && pass_zvtx10 && mbd_trigger_fire)
  {
    if (minbias_bkg_high)
    {
      hEventMinBias->Fill(static_cast<std::uint8_t>(MinBiasType::BKG_HIGH));
    }
    if (minbias_side_hit_low)
    {
      hEventMinBias->Fill(static_cast<std::uint8_t>(MinBiasType::SIDE_HIT_LOW));
    }
    if (minbias_zdc_low)
    {
      hEventMinBias->Fill(static_cast<std::uint8_t>(MinBiasType::ZDC_LOW));
    }
    if (minbias_mbd_high)
    {
      hEventMinBias->Fill(static_cast<std::uint8_t>(MinBiasType::MBD_HIGH));
    }
  }

  // skip event if not fire MBD Trigger
  if (!mbd_trigger_fire)
  {
    ++m_ctr["process_eventCheck_mbd_trigger_fail"];
    return (m_doAbort) ? Fun4AllReturnCodes::ABORTEVENT : Fun4AllReturnCodes::EVENT_OK;
  }

  // skip event if not minimum bias
  if (!m_mb_info->isAuAuMinimumBias())
  {
    ++m_ctr["process_eventCheck_isAuAuMinBias_fail"];
    return (m_doAbort) ? Fun4AllReturnCodes::ABORTEVENT : Fun4AllReturnCodes::EVENT_OK;
  }

  if (m_do_hist)
  {
    hVtxZ_MB->Fill(zvtx);
  }

  // skip event if zvtx is too large
  if (!pass_zvtx10)
  {
    ++m_ctr["process_eventCheck_zvtx_large"];
    return (m_doAbort) ? Fun4AllReturnCodes::ABORTEVENT : Fun4AllReturnCodes::EVENT_OK;
  }

  if (m_do_hist)
  {
    hEvent->Fill(static_cast<std::uint8_t>(EventType::MB));
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

int EventQA::process_centrality(PHCompositeNode *topNode)
{
  CentralityInfo *centInfo = findNode::getClass<CentralityInfo>(topNode, "CentralityInfo");
  if (!centInfo)
  {
    std::cout << "Aborting Run: CentralityInfo null" << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  m_data.centrality = centInfo->get_centile(CentralityInfo::PROP::mbd_NS) * 100;
  double cent = m_data.centrality;

  if (m_do_hist)
  {
    hCentrality->Fill(cent);
  }

  // skip event if centrality is too peripheral
  if (!std::isfinite(cent) || cent >= m_cuts.m_cent_max)
  {
    ++m_ctr["process_eventCheck_centrality_large"];
    return (m_doAbort) ? Fun4AllReturnCodes::ABORTEVENT : Fun4AllReturnCodes::EVENT_OK;
  }

  if (m_do_hist)
  {
    hEvent->Fill(static_cast<std::uint8_t>(EventType::CENT));
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

int EventQA::process_event(PHCompositeNode *topNode)
{
  int ret = process_event_check(topNode);
  if (ret && m_doAbort)
  {
    return ret;
  }

  ret = process_centrality(topNode);
  if (ret && m_doAbort)
  {
    return ret;
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

int EventQA::ResetEvent([[maybe_unused]] PHCompositeNode *topNode)
{
  ++m_ctr["event_reset"];

  // Event
  m_data.event = 0;
  m_data.zvtx = 9999;
  m_data.centrality = 9999;

  return Fun4AllReturnCodes::EVENT_OK;
}

int EventQA::End([[maybe_unused]] PHCompositeNode *topNode)
{
  std::cout << "EventQA::End" << std::endl;

  std::cout << std::format("{:#<20}\n", "");
  std::cout << "stats" << std::endl;

  std::cout << std::format("{:#<20}\n", "");
  std::cout << "Abort Events Types" << std::endl;
  std::cout << std::format("process event, Reset Event Calls : {}", m_ctr["event_reset"]) << std::endl;
  std::cout << std::format("process event, MBD Trigger Fail: {}", m_ctr["process_eventCheck_mbd_trigger_fail"]) << std::endl;
  std::cout << std::format("process event, isAuAuMinBias Fail: {}", m_ctr["process_eventCheck_isAuAuMinBias_fail"]) << std::endl;
  std::cout << std::format("process event, |z| >= {} cm: {}", m_cuts.m_zvtx_max, m_ctr["process_eventCheck_zvtx_large"]) << std::endl;
  std::cout << std::format("process event, Centrality >= {}%: {}", m_cuts.m_cent_max, m_ctr["process_eventCheck_centrality_large"]) << std::endl;

  if (m_do_hist && hEvent)
  {
    std::cout << std::format("{:#<20}\n", "");
    std::cout << "Events" << std::endl;
    for (unsigned int i = 0; i < m_eventType.size(); ++i)
    {
      std::cout << m_eventType[i] << ": " << hEvent->GetBinContent(i + 1) << std::endl;
    }
    std::cout << std::format("{:#<20}\n", "");
  }

  return Fun4AllReturnCodes::EVENT_OK;
}
