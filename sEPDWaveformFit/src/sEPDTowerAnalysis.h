#ifndef SEPDTOWERANALYSIS_H
#define SEPDTOWERANALYSIS_H

#include <fun4all/SubsysReco.h>

#include <string>

class PHCompositeNode;
class TFile;
class TH1F;
class TH2F;
class TTree;
class TProfile;

class sEPDTowerAnalysis : public SubsysReco
{
 public:
  sEPDTowerAnalysis(const std::string &name = "sEPDTowerAnalysis");
  ~sEPDTowerAnalysis() override;

  int Init(PHCompositeNode *topNode) override;
  int InitRun(PHCompositeNode *topNode) override;
  int process_event(PHCompositeNode *topNode) override;
  int End(PHCompositeNode *topNode) override;

  void SetOutputFile(const std::string &filename) { m_outputFile = filename; }

 private:
  std::string m_outputFile;
  TFile *m_file;

  TH1F *h_energy_all;
  TH1F *h_energy_all_with_negative;
  TH1F *h_energy_tile;
  TH1F *h_energy_valid_time;
  TH1F *h_energy_nan_time;
  TH1F *h_time_all;
  TH1F *h_chi2_all;
  TH1F *h_pedestal_all;
  TH2F *h_energy_vs_time;
  TH2F *h_energy_vs_chi2;
  TProfile *h_energy_vs_chi2_profile;
  TH2F *h_chi2_vs_energy;
  TProfile *h_chi2_vs_energy_profile;

  TH1F *h_energy_fast;
  TH1F *h_time_fast;
  TH1F *h_chi2_fast;
  TH1F *h_pedestal_fast;
  TH2F *h_energy_vs_chi2_fast;

  TH2F *h_energy_template_vs_fast;
  TH2F *h_energy_template_vs_fast_time_cut1;
  TH2F *h_energy_template_vs_fast_time_cut2;
  TH2F *h_energy_template_vs_fast_fast_time_cut1;
  TH2F *h_energy_template_vs_fast_fast_time_cut2;
  TH1F *h_time_template;
  TH1F *h_time_fast_separate;
  TH2F *h_pedestal_template_vs_fast;
  TH2F *h_time_template_vs_fast;

  TH1F *h_event_summary;
  TH1F *h_energy_categories;

  TTree *m_tree;
  int m_event;
  int m_channel;
  int m_idx;

  float m_energy;
  float m_time;
  float m_chi2;
  float m_pedestal;
  bool m_valid_time;

  float m_energy_fast;
  float m_time_fast;
  float m_chi2_fast;
  float m_pedestal_fast;
  bool m_valid_time_fast;

  int m_total_channels;
  int m_channels_with_signal;
  int m_channels_valid_time;
  int m_channels_nan_time;
  int m_channels_zero_suppressed;
};

#endif
