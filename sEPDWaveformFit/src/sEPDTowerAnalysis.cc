#include "sEPDTowerAnalysis.h"

#include <calobase/TowerInfo.h>
#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfoDefs.h>

#include <fun4all/Fun4AllReturnCodes.h>
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

#include <TFile.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TTree.h>
#include <TLine.h>
#include <TProfile.h>

#include <iostream>
#include <cmath>

sEPDTowerAnalysis::sEPDTowerAnalysis(const std::string &name)
  : SubsysReco(name)
  , m_outputFile("sEPDTowerAnalysis.root")
  , m_file(nullptr)
  , h_energy_all(nullptr)
  , h_energy_all_with_negative(nullptr)
  , h_energy_tile(nullptr)
  , h_energy_valid_time(nullptr)
  , h_energy_nan_time(nullptr)
  , h_time_all(nullptr)
  , h_chi2_all(nullptr)
  , h_pedestal_all(nullptr)
  , h_energy_vs_time(nullptr)
  , h_energy_vs_chi2(nullptr)
  , h_energy_vs_chi2_profile(nullptr)
  , h_chi2_vs_energy(nullptr)
  , h_chi2_vs_energy_profile(nullptr)
  , h_energy_fast(nullptr)
  , h_time_fast(nullptr)
  , h_chi2_fast(nullptr)
  , h_pedestal_fast(nullptr)
  , h_energy_vs_chi2_fast(nullptr)
  , h_energy_template_vs_fast(nullptr)
  , h_energy_template_vs_fast_time_cut1(nullptr)
  , h_energy_template_vs_fast_time_cut2(nullptr)
  , h_energy_template_vs_fast_fast_time_cut1(nullptr)
  , h_energy_template_vs_fast_fast_time_cut2(nullptr)
  , h_time_template(nullptr)
  , h_time_fast_separate(nullptr)
  , h_pedestal_template_vs_fast(nullptr)
  , h_time_template_vs_fast(nullptr)
  , h_event_summary(nullptr)
  , h_energy_categories(nullptr)
  , m_tree(nullptr)
  , m_event(0)
  , m_channel(0)
  , m_idx(0)
  , m_energy(0)
  , m_time(0)
  , m_chi2(0)
  , m_pedestal(0)
  , m_valid_time(false)
  , m_energy_fast(0)
  , m_time_fast(0)
  , m_chi2_fast(0)
  , m_pedestal_fast(0)
  , m_valid_time_fast(false)
  , m_total_channels(0)
  , m_channels_with_signal(0)
  , m_channels_valid_time(0)
  , m_channels_nan_time(0)
  , m_channels_zero_suppressed(0)
{
}

sEPDTowerAnalysis::~sEPDTowerAnalysis()
{
  if (m_file)
  {
    m_file->Close();
    delete m_file;
  }
}

int sEPDTowerAnalysis::Init(PHCompositeNode *topNode)
{
  m_file = TFile::Open(m_outputFile.c_str(), "RECREATE");

  h_energy_all = new TH1F("h_energy_template", "TEMPLATE Tower Energies (>20 ADC);Energy [ADC];Entries", 480, 20, 500);
  h_energy_all_with_negative = new TH1F("h_energy_template_with_negative", "TEMPLATE Tower Energies (incl. negative);Energy [ADC];Entries", 600, -100, 500);
  h_energy_valid_time = new TH1F("h_energy_template_valid_time", "TEMPLATE Tower Energies (Valid Time);Energy [ADC];Entries", 500, 0, 500);
  h_energy_nan_time = new TH1F("h_energy_template_nan_time", "TEMPLATE Tower Energies (NaN Time);Energy [ADC];Entries", 500, 0, 500);
  h_energy_tile = new TH1F("h_energy_template_tile", "TEMPLATE Tile 23;ADC;Entries", 500, 0, 500);
  h_time_all = new TH1F("h_time_template", "TEMPLATE Tower Times;Time [samples];Entries", 200, -10, 20);
  h_chi2_all = new TH1F("h_chi2_template", "TEMPLATE Tower Chi2;Chi2;Entries", 200, 0, 10000);
  h_pedestal_all = new TH1F("h_pedestal_template", "TEMPLATE Tower Pedestals (WARNING: All NaN);Pedestal [ADC];Entries", 500, 0, 5000);
  h_energy_vs_time = new TH2F("h_energy_vs_time_template", "TEMPLATE Energy vs Time;Time [samples];Energy [ADC]", 100, -5, 15, 200, 0, 500);
  h_energy_vs_chi2 = new TH2F("h_energy_vs_chi2_template", "TEMPLATE Energy vs Chi2;Chi2;Energy [ADC]", 100, 0, 1000, 200, 0, 500);
  h_energy_vs_chi2_profile = new TProfile("h_energy_vs_chi2_profile_template", "TEMPLATE Average Energy vs Chi2;Chi2;Average Energy [ADC]", 100, 0, 1000, 0, 500);
  h_chi2_vs_energy = new TH2F("h_chi2_vs_energy_template", "TEMPLATE Chi2 vs Energy;Energy [ADC];Chi2", 200, 0, 500, 100, 0, 1000);
  h_chi2_vs_energy_profile = new TProfile("h_chi2_vs_energy_profile_template", "TEMPLATE Average Chi2 vs Energy;Energy [ADC];Average Chi2", 200, 0, 500, 0, 1000);

  h_energy_fast = new TH1F("h_energy_fast", "FAST Tower Energies (>20 ADC);Energy [ADC];Entries", 480, 20, 500);
  h_time_fast = new TH1F("h_time_fast", "FAST Tower Times;Time [samples];Entries", 200, -10, 20);
  h_chi2_fast = new TH1F("h_chi2_fast", "FAST Tower Chi2;Chi2;Entries", 200, 0, 10000);
  h_pedestal_fast = new TH1F("h_pedestal_fast", "FAST Tower Pedestals (WARNING: All NaN);Pedestal [ADC];Entries", 500, 0, 5000);
  h_energy_vs_chi2_fast = new TH2F("h_energy_vs_chi2_fast", "FAST Energy vs Chi2;Chi2;Energy [ADC]", 100, 0, 1000, 200, 0, 500);

  h_energy_template_vs_fast = new TH2F("h_energy_template_vs_fast", "Energy Correlation: TEMPLATE vs FAST;FAST Energy [ADC];TEMPLATE Energy [ADC]", 200, 0, 500, 200, 0, 500);
  h_energy_template_vs_fast_time_cut1 = new TH2F("h_energy_template_vs_fast_time_cut1", "Energy Correlation (-1 < template_time < 0): TEMPLATE vs FAST;FAST Energy [ADC];TEMPLATE Energy [ADC]", 200, 0, 500, 200, 0, 500);
  h_energy_template_vs_fast_time_cut2 = new TH2F("h_energy_template_vs_fast_time_cut2", "Energy Correlation (-2 < template_time < -1): TEMPLATE vs FAST;FAST Energy [ADC];TEMPLATE Energy [ADC]", 200, 0, 500, 200, 0, 500);
  h_energy_template_vs_fast_fast_time_cut1 = new TH2F("h_energy_template_vs_fast_fast_time_cut1", "Energy Correlation (-1 < fast_time < 0): TEMPLATE vs FAST;FAST Energy [ADC];TEMPLATE Energy [ADC]", 200, 0, 500, 200, 0, 500);
  h_energy_template_vs_fast_fast_time_cut2 = new TH2F("h_energy_template_vs_fast_fast_time_cut2", "Energy Correlation (-2 < fast_time < -1): TEMPLATE vs FAST;FAST Energy [ADC];TEMPLATE Energy [ADC]", 200, 0, 500, 200, 0, 500);
  h_time_template = new TH1F("h_time_template_comparison", "TEMPLATE Times (for comparison);Time [samples];Entries", 200, -10, 20);
  h_time_fast_separate = new TH1F("h_time_fast_comparison", "FAST Times (for comparison);Time [samples];Entries", 200, -10, 20);
  h_pedestal_template_vs_fast = new TH2F("h_pedestal_template_vs_fast", "Pedestal Correlation (WARNING: All NaN);FAST Pedestal [ADC];TEMPLATE Pedestal [ADC]", 250, 0, 5000, 250, 0, 5000);
  h_time_template_vs_fast = new TH2F("h_time_template_vs_fast", "Time Correlation: TEMPLATE vs FAST;FAST Time [samples];TEMPLATE Time [samples]", 100, -10, 20, 100, -10, 20);

  h_event_summary = new TH1F("h_event_summary", "Event Summary;Category;Count per Event", 6, 0, 6);
  h_event_summary->GetXaxis()->SetBinLabel(1, "Total Channels");
  h_event_summary->GetXaxis()->SetBinLabel(2, "Energy > 1 ADC");
  h_event_summary->GetXaxis()->SetBinLabel(3, "Energy > 5 ADC");
  h_event_summary->GetXaxis()->SetBinLabel(4, "Valid Time");
  h_event_summary->GetXaxis()->SetBinLabel(5, "NaN Time");
  h_event_summary->GetXaxis()->SetBinLabel(6, "Zero Suppressed");

  h_energy_categories = new TH1F("h_energy_categories", "Energy Categories;Energy Range;Channels", 11, 0, 11);
  h_energy_categories->GetXaxis()->SetBinLabel(1, "< 0");
  h_energy_categories->GetXaxis()->SetBinLabel(2, "0-0.5");
  h_energy_categories->GetXaxis()->SetBinLabel(3, "0.5-1");
  h_energy_categories->GetXaxis()->SetBinLabel(4, "1-2");
  h_energy_categories->GetXaxis()->SetBinLabel(5, "2-5");
  h_energy_categories->GetXaxis()->SetBinLabel(6, "5-10");
  h_energy_categories->GetXaxis()->SetBinLabel(7, "10-20");
  h_energy_categories->GetXaxis()->SetBinLabel(8, "20-50");
  h_energy_categories->GetXaxis()->SetBinLabel(9, "50-100");
  h_energy_categories->GetXaxis()->SetBinLabel(10, "100-200");
  h_energy_categories->GetXaxis()->SetBinLabel(11, "> 200");

  m_tree = new TTree("tower_tree", "sEPD Tower Data");
  m_tree->Branch("event", &m_event, "event/I");
  m_tree->Branch("channel", &m_channel, "channel/I");
  m_tree->Branch("idx", &m_idx, "idx/I");

  m_tree->Branch("energy_template", &m_energy, "energy_template/F");
  m_tree->Branch("time_template", &m_time, "time_template/F");
  m_tree->Branch("chi2_template", &m_chi2, "chi2_template/F");
  m_tree->Branch("pedestal_template", &m_pedestal, "pedestal_template/F");
  m_tree->Branch("valid_time_template", &m_valid_time, "valid_time_template/O");

  m_tree->Branch("energy_fast", &m_energy_fast, "energy_fast/F");
  m_tree->Branch("time_fast", &m_time_fast, "time_fast/F");
  m_tree->Branch("chi2_fast", &m_chi2_fast, "chi2_fast/F");
  m_tree->Branch("pedestal_fast", &m_pedestal_fast, "pedestal_fast/F");
  m_tree->Branch("valid_time_fast", &m_valid_time_fast, "valid_time_fast/O");

  std::cout << "sEPDTowerAnalysis initialized - output file: " << m_outputFile << std::endl;

  return Fun4AllReturnCodes::EVENT_OK;
}

int sEPDTowerAnalysis::InitRun(PHCompositeNode *topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

int sEPDTowerAnalysis::process_event(PHCompositeNode *topNode)
{
  m_event++;

  std::string template_container = "TOWERS_SEPD";
  std::string fast_container = "TOWERS_FAST_SEPD";

  TowerInfoContainer *towers_template = findNode::getClass<TowerInfoContainer>(topNode, template_container);
  TowerInfoContainer *towers_fast = findNode::getClass<TowerInfoContainer>(topNode, fast_container);

  if (!towers_template) {
    std::cout << "No TEMPLATE towers found (" << template_container << ")!" << std::endl;
    return Fun4AllReturnCodes::EVENT_OK;
  }

  if (!towers_fast) {
    std::cout << "No FAST towers found (" << fast_container << ")!" << std::endl;
    return Fun4AllReturnCodes::EVENT_OK;
  }

  if (m_event <= 5 || m_event % 1000 == 0) {
    std::cout << "Event " << m_event << ": TEMPLATE container " << template_container << " has " << towers_template->size() << " towers" << std::endl;
    std::cout << "Event " << m_event << ": FAST container " << fast_container << " has " << towers_fast->size() << " towers" << std::endl;
  }

  int event_total = 0;
  int event_signal_10 = 0;
  int event_signal_50 = 0;
  int event_valid_time = 0;
  int event_nan_time = 0;
  int event_zero_suppressed = 0;
  int pedestal_template_count = 0;
  int pedestal_fast_count = 0;

  unsigned int ntowers_template = towers_template->size();
  unsigned int ntowers_fast = towers_fast->size();

  if (ntowers_template != ntowers_fast) {
    std::cout << "WARNING: Container size mismatch! TEMPLATE=" << ntowers_template << ", FAST=" << ntowers_fast << std::endl;
    return Fun4AllReturnCodes::EVENT_OK;
  }

  for (unsigned int i = 0; i < ntowers_template; i++)
  {
    TowerInfo *tower_template = towers_template->get_tower_at_channel(i);
    TowerInfo *tower_fast = towers_fast->get_tower_at_channel(i);

    if (!tower_template || !tower_fast) continue;

    float energy_template = tower_template->get_energy();
    float time_template = tower_template->get_time();
    float chi2_template = tower_template->get_chi2();
    float pedestal_template = tower_template->get_pedestal();
    bool valid_time_template = !std::isnan(time_template);

    float energy_fast = tower_fast->get_energy();
    float time_fast = tower_fast->get_time();
    float chi2_fast = tower_fast->get_chi2();
    float pedestal_fast = tower_fast->get_pedestal();
    bool valid_time_fast = !std::isnan(time_fast);

    m_channel = i;
    m_idx = i;
    m_energy = energy_template;
    m_time = time_template;
    m_chi2 = chi2_template;
    m_pedestal = pedestal_template;
    m_valid_time = valid_time_template;
    m_energy_fast = energy_fast;
    m_time_fast = time_fast;
    m_chi2_fast = chi2_fast;
    m_pedestal_fast = pedestal_fast;
    m_valid_time_fast = valid_time_fast;
    m_tree->Fill();

    if (i == 23) {
      h_energy_tile->Fill(energy_template);
    }

    if (energy_template > 20) {
      h_energy_all->Fill(energy_template);
    }

    h_energy_all_with_negative->Fill(energy_template);

    if (valid_time_template)
    {
      h_energy_valid_time->Fill(energy_template);
      h_time_all->Fill(time_template);
      h_energy_vs_time->Fill(time_template, energy_template);
      h_time_template->Fill(time_template);
    }
    else
    {
      h_energy_nan_time->Fill(energy_template);
    }

    if (!std::isnan(chi2_template))
    {
      h_chi2_all->Fill(chi2_template);
      h_energy_vs_chi2->Fill(chi2_template, energy_template);
      h_energy_vs_chi2_profile->Fill(chi2_template, energy_template);
      h_chi2_vs_energy->Fill(energy_template, chi2_template);
      h_chi2_vs_energy_profile->Fill(energy_template, chi2_template);
    }

    if (m_event <= 5) {

      if (i < 5 || i == 100 || i == 200 || i == 400 || i == 700) {
        std::cout << "Event " << m_event << " Ch " << i << ": pedestal_template=" << pedestal_template
                  << ", pedestal_fast=" << pedestal_fast
                  << ", energy_template=" << energy_template
                  << ", energy_fast=" << energy_fast << std::endl;
      }
    }

    if (!std::isnan(pedestal_template))
    {
      h_pedestal_all->Fill(pedestal_template);
      pedestal_template_count++;
    }

    if (energy_fast > 20) {
      h_energy_fast->Fill(energy_fast);
    }

    if (valid_time_fast) {
      h_time_fast->Fill(time_fast);
      h_time_fast_separate->Fill(time_fast);
    }

    if (!std::isnan(chi2_fast)) {
      h_chi2_fast->Fill(chi2_fast);
      h_energy_vs_chi2_fast->Fill(chi2_fast, energy_fast);
    }

    if (!std::isnan(pedestal_fast)) {
      h_pedestal_fast->Fill(pedestal_fast);
      pedestal_fast_count++;
    }

    h_energy_template_vs_fast->Fill(energy_fast, energy_template);

    if (valid_time_template && valid_time_fast) {

      if (time_template > -1.0 && time_template < 0.0) {
        h_energy_template_vs_fast_time_cut1->Fill(energy_fast, energy_template);
      }
      if (time_template > -2.0 && time_template < -1.0) {
        h_energy_template_vs_fast_time_cut2->Fill(energy_fast, energy_template);
      }

      if (time_fast > -1.0 && time_fast < 0.0) {
        h_energy_template_vs_fast_fast_time_cut1->Fill(energy_fast, energy_template);
      }
      if (time_fast > -2.0 && time_fast < -1.0) {
        h_energy_template_vs_fast_fast_time_cut2->Fill(energy_fast, energy_template);
      }

      h_time_template_vs_fast->Fill(time_fast, time_template);
    }

    if (!std::isnan(pedestal_template) && !std::isnan(pedestal_fast)) {
      h_pedestal_template_vs_fast->Fill(pedestal_fast, pedestal_template);
    }

    if (energy_template < 0) h_energy_categories->Fill(0.0);
    else if (energy_template < 0.5) h_energy_categories->Fill(1.0);
    else if (energy_template < 1) h_energy_categories->Fill(2.0);
    else if (energy_template < 2) h_energy_categories->Fill(3.0);
    else if (energy_template < 5) h_energy_categories->Fill(4.0);
    else if (energy_template < 10) h_energy_categories->Fill(5.0);
    else if (energy_template < 20) h_energy_categories->Fill(6.0);
    else if (energy_template < 50) h_energy_categories->Fill(7.0);
    else if (energy_template < 100) h_energy_categories->Fill(8.0);
    else if (energy_template < 200) h_energy_categories->Fill(9.0);
    else h_energy_categories->Fill(10.0);

    event_total++;
    if (energy_template > 1) event_signal_10++;
    if (energy_template > 5) event_signal_50++;
    if (valid_time_template) event_valid_time++;
    else event_nan_time++;
    if (!valid_time_template && energy_template < 1) event_zero_suppressed++;

    m_total_channels++;
    if (energy_template > 1) m_channels_with_signal++;
    if (valid_time_template) m_channels_valid_time++;
    else m_channels_nan_time++;
    if (!valid_time_template && energy_template < 1) m_channels_zero_suppressed++;
  }

  h_event_summary->Fill(0.0, (double)event_total);
  h_event_summary->Fill(1.0, (double)event_signal_10);
  h_event_summary->Fill(2.0, (double)event_signal_50);
  h_event_summary->Fill(3.0, (double)event_valid_time);
  h_event_summary->Fill(4.0, (double)event_nan_time);
  h_event_summary->Fill(5.0, (double)event_zero_suppressed);

  if (m_event % 100 == 0)
  {
    std::cout << "Event " << m_event << " summary:" << std::endl;
    std::cout << "  Total channels: " << event_total << std::endl;
    std::cout << "  Energy > 1 ADC: " << event_signal_10 << " (" << 100.0*event_signal_10/event_total << "%)" << std::endl;
    std::cout << "  Energy > 5 ADC: " << event_signal_50 << " (" << 100.0*event_signal_50/event_total << "%)" << std::endl;
    std::cout << "  Valid time: " << event_valid_time << " (" << 100.0*event_valid_time/event_total << "%)" << std::endl;
    std::cout << "  NaN time: " << event_nan_time << " (" << 100.0*event_nan_time/event_total << "%)" << std::endl;
    std::cout << "  Zero suppressed: " << event_zero_suppressed << " (" << 100.0*event_zero_suppressed/event_total << "%)" << std::endl;
    std::cout << "  TEMPLATE pedestals filled: " << pedestal_template_count << " (" << 100.0*pedestal_template_count/event_total << "%)" << std::endl;
    std::cout << "  FAST pedestals filled: " << pedestal_fast_count << " (" << 100.0*pedestal_fast_count/event_total << "%)" << std::endl;
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

int sEPDTowerAnalysis::End(PHCompositeNode *topNode)
{
  std::cout << "\nTOTAL EVENTS PROCESSED: " << m_event << " events" << std::endl;
  std::cout << "  Total channels processed: " << m_total_channels << std::endl;
  std::cout << "  Channels with signal (>1 ADC): " << m_channels_with_signal << " (" << 100.0*m_channels_with_signal/m_total_channels << "%)" << std::endl;
  std::cout << "  Channels with valid time: " << m_channels_valid_time << " (" << 100.0*m_channels_valid_time/m_total_channels << "%)" << std::endl;
  std::cout << "  Channels with NaN time: " << m_channels_nan_time << " (" << 100.0*m_channels_nan_time/m_total_channels << "%)" << std::endl;
  std::cout << "  Zero suppressed channels: " << m_channels_zero_suppressed << " (" << 100.0*m_channels_zero_suppressed/m_total_channels << "%)" << std::endl;

  std::cout << "\n  TEMPLATE method: " << h_pedestal_all->GetEntries() << " valid pedestals found" << std::endl;
  std::cout << "  FAST method: " << h_pedestal_fast->GetEntries() << " valid pedestals found" << std::endl;

  if (h_pedestal_all->GetEntries() == 0 && h_pedestal_fast->GetEntries() == 0) {
    std::cout << "\n  WARNING: No valid pedestal values found in either method!" << std::endl;
  }

  if (m_file)
  {
    m_file->cd();

    h_energy_all->Write();
    h_energy_all_with_negative->Write();
    h_energy_tile->Write();
    h_energy_valid_time->Write();
    h_energy_nan_time->Write();
    h_time_all->Write();
    h_chi2_all->Write();
    h_pedestal_all->Write();
    h_energy_vs_time->Write();

    h_energy_vs_chi2->Write();
    h_energy_vs_chi2_profile->SetLineColor(kRed);
    h_energy_vs_chi2_profile->SetLineWidth(2);
    h_energy_vs_chi2_profile->Write();

    h_chi2_vs_energy->Write();
    h_chi2_vs_energy_profile->SetLineColor(kRed);
    h_chi2_vs_energy_profile->SetLineWidth(2);
    h_chi2_vs_energy_profile->Write();

    h_energy_fast->Write();
    h_time_fast->Write();
    h_chi2_fast->Write();
    h_pedestal_fast->Write();
    h_energy_vs_chi2_fast->Write();

    TLine *diagonal_line = new TLine(0, 0, 500, 500);
    diagonal_line->SetLineColor(kRed);
    diagonal_line->SetLineWidth(2);
    diagonal_line->SetLineStyle(2);
    h_energy_template_vs_fast->GetListOfFunctions()->Add(diagonal_line);
    h_energy_template_vs_fast->Write();

    TLine *diagonal_line_cut1 = new TLine(0, 0, 500, 500);
    diagonal_line_cut1->SetLineColor(kRed);
    diagonal_line_cut1->SetLineWidth(2);
    diagonal_line_cut1->SetLineStyle(2);
    h_energy_template_vs_fast_time_cut1->GetListOfFunctions()->Add(diagonal_line_cut1);
    h_energy_template_vs_fast_time_cut1->Write();

    TLine *diagonal_line_cut2 = new TLine(0, 0, 500, 500);
    diagonal_line_cut2->SetLineColor(kRed);
    diagonal_line_cut2->SetLineWidth(2);
    diagonal_line_cut2->SetLineStyle(2);
    h_energy_template_vs_fast_time_cut2->GetListOfFunctions()->Add(diagonal_line_cut2);
    h_energy_template_vs_fast_time_cut2->Write();

    TLine *diagonal_line_fast_cut1 = new TLine(0, 0, 500, 500);
    diagonal_line_fast_cut1->SetLineColor(kRed);
    diagonal_line_fast_cut1->SetLineWidth(2);
    diagonal_line_fast_cut1->SetLineStyle(2);
    h_energy_template_vs_fast_fast_time_cut1->GetListOfFunctions()->Add(diagonal_line_fast_cut1);
    h_energy_template_vs_fast_fast_time_cut1->Write();

    TLine *diagonal_line_fast_cut2 = new TLine(0, 0, 500, 500);
    diagonal_line_fast_cut2->SetLineColor(kRed);
    diagonal_line_fast_cut2->SetLineWidth(2);
    diagonal_line_fast_cut2->SetLineStyle(2);
    h_energy_template_vs_fast_fast_time_cut2->GetListOfFunctions()->Add(diagonal_line_fast_cut2);
    h_energy_template_vs_fast_fast_time_cut2->Write();

    h_time_template->Write();
    h_time_fast_separate->Write();
    h_pedestal_template_vs_fast->Write();
    h_time_template_vs_fast->Write();

    h_event_summary->Write();
    h_energy_categories->Write();

    m_tree->Write();

    m_file->Close();
    std::cout << "\nOutput written to: " << m_outputFile << std::endl;
  }

  return Fun4AllReturnCodes::EVENT_OK;
}
