#include "NoiseValidation.h"

// -- Fun4All
#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/Fun4AllServer.h>

// -- Nodes
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

// -- Calo
#include <calobase/TowerInfo.h>
#include <calobase/TowerInfoContainer.h>

// Trigger
#include <calotrigger/TriggerAnalyzer.h>

// -- ROOT
#include <TH1F.h>
#include <TH2F.h>

// -- c++
#include <iostream>
#include <format>
#include <algorithm>

NoiseValidation::NoiseValidation(const std::string &name)
  : SubsysReco(name)
{
}

NoiseValidation::~NoiseValidation() = default;

int NoiseValidation::Init([[maybe_unused]] PHCompositeNode *topNode)
{
  Fun4AllServer *se = Fun4AllServer::instance();

  m_triggerAnalyzer = std::make_unique<TriggerAnalyzer>();

  // Event count histogram
  hEvent = new TH1F("hEvent", "Event Counts; Event Type; Counts", 2, -0.5, 1.5);
  hEvent->GetXaxis()->SetBinLabel(1, "All Events");
  hEvent->GetXaxis()->SetBinLabel(2, "Clock Trigger");
  se->registerHisto(hEvent);

  // Channel axis bounds
  const double hcal_ch_min = -0.5;
  const double hcal_ch_max = m_nchannels_hcal - 0.5;

  const double sepd_ch_min = -0.5;
  const double sepd_ch_max = m_nchannels_sepd - 0.5;

  h2_ihcal_noise = new TH2F("h2_ihcal_noise", "IHCal Noise; Channel; ADC", m_nchannels_hcal, hcal_ch_min, hcal_ch_max, m_adc_bins, m_adc_min, m_adc_max);
  h2_ihcal_noise->Sumw2();
  se->registerHisto(h2_ihcal_noise);

  h2_ohcal_noise = new TH2F("h2_ohcal_noise", "OHCal Noise; Channel; ADC", m_nchannels_hcal, hcal_ch_min, hcal_ch_max, m_adc_bins, m_adc_min, m_adc_max);
  h2_ohcal_noise->Sumw2();
  se->registerHisto(h2_ohcal_noise);

  h2_sepd_noise = new TH2F("h2_sepd_noise", "sEPD Noise; Channel; ADC", m_nchannels_sepd, sepd_ch_min, sepd_ch_max, m_adc_bins, m_adc_min, m_adc_max);
  h2_sepd_noise->Sumw2();
  se->registerHisto(h2_sepd_noise);

  return Fun4AllReturnCodes::EVENT_OK;
}

int NoiseValidation::process_event(PHCompositeNode *topNode)
{
  hEvent->Fill(0);
  ++m_total_events;

  m_triggerAnalyzer->decodeTriggers(topNode);

  // check if trigger bit fired (clock trigger)
  if (!m_triggerAnalyzer->didTriggerFire(m_trig_bit))
  {
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  hEvent->Fill(1);
  ++m_clock_trig_events;

  auto* towersIHCal = findNode::getClass<TowerInfoContainer>(topNode, "TOWERS_HCALIN");
  auto* towersOHCal = findNode::getClass<TowerInfoContainer>(topNode, "TOWERS_HCALOUT");
  auto* towersSEPD  = findNode::getClass<TowerInfoContainer>(topNode, "TOWERS_SEPD");

  if (towersIHCal)
  {
    for (unsigned int channel = 0; channel < towersIHCal->size(); ++channel)
    {
      auto* tower = towersIHCal->get_tower_at_channel(channel);
      if (!tower->get_isGood())
      {
        continue;
      }
      double adc = tower->get_energy();
      h2_ihcal_noise->Fill(channel, adc);

      m_ihcal_min_adc = std::min(m_ihcal_min_adc, adc);
      m_ihcal_max_adc = std::max(m_ihcal_max_adc, adc);
    }
  }

  if (towersOHCal)
  {
    for (unsigned int channel = 0; channel < towersOHCal->size(); ++channel)
    {
      auto* tower = towersOHCal->get_tower_at_channel(channel);
      if (!tower->get_isGood())
      {
        continue;
      }
      double adc = tower->get_energy();
      h2_ohcal_noise->Fill(channel, adc);

      m_ohcal_min_adc = std::min(m_ohcal_min_adc, adc);
      m_ohcal_max_adc = std::max(m_ohcal_max_adc, adc);
    }
  }

  if (towersSEPD)
  {
    for (unsigned int channel = 0; channel < towersSEPD->size(); ++channel)
    {
      auto* tower = towersSEPD->get_tower_at_channel(channel);
      if (!tower->get_isGood())
      {
        continue;
      }
      double adc = tower->get_energy();
      h2_sepd_noise->Fill(channel, adc);

      m_sepd_min_adc = std::min(m_sepd_min_adc, adc);
      m_sepd_max_adc = std::max(m_sepd_max_adc, adc);
    }
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

int NoiseValidation::ResetEvent([[maybe_unused]] PHCompositeNode *topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

int NoiseValidation::End([[maybe_unused]] PHCompositeNode *topNode)
{
  std::cout << "------------------------------------------\n";
  std::cout << "Noise Validation Results:\n";
  std::cout << std::format("Total Events: {}\n", m_total_events);
  std::cout << std::format("Clock Trigger Events: {}\n", m_clock_trig_events);
  std::cout << "ADC Ranges:\n";
  std::cout << std::format("IHCal min ADC: {}, max ADC: {}\n", m_ihcal_min_adc, m_ihcal_max_adc);
  std::cout << std::format("OHCal min ADC: {}, max ADC: {}\n", m_ohcal_min_adc, m_ohcal_max_adc);
  std::cout << std::format("sEPD  min ADC: {}, max ADC: {}\n", m_sepd_min_adc, m_sepd_max_adc);
  std::cout << "------------------------------------------\n";

  return Fun4AllReturnCodes::EVENT_OK;
}
