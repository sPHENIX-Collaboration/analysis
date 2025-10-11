// -- utils
#include "myUtils.C"

// ====================================================================
// sPHENIX Includes
// ====================================================================
#include <calobase/TowerInfoDefs.h>
// CDB
#include <cdbobjects/CDBTTree.h>
#include <CDBUtils.C>

// ====================================================================
// Standard C++ Includes
// ====================================================================
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <numbers>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>

// ====================================================================
// ROOT Includes
// ====================================================================
#include <TChain.h>
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TProfile.h>
#include <TProfile2D.h>
#include <TROOT.h>
#include <TTree.h>

// ====================================================================
// The Analysis Class
// ====================================================================
class JetAnalysis
{
 public:
  // The constructor takes the configuration
  JetAnalysis(std::string input_list, std::string input_Q_calib, unsigned int runnumber, long long events, std::string output_dir)
    : m_chain(std::make_unique<TChain>("T"))
    , m_input_list(std::move(input_list))
    , m_input_Q_calib(std::move(input_Q_calib))
    , m_runnumber(runnumber)
    , m_events_to_process(events)
    , m_output_dir(std::move(output_dir))
  {
  }

  void run()
  {
    setup_chain();
    read_Q_calib();
    process_dead_channels();
    init_hists();
    process_events();
    finalize();
    validate_results();
    save_results();
  }

 private:
  enum class Subdetector
  {
    S,
    N
  };

  enum class QComponent
  {
    X,
    Y
  };

  struct QVec
  {
    double x{0.0};
    double y{0.0};
  };

  struct CorrectionData
  {
    // Averages of Qx, Qy, Qx^2, Qy^2, Qxy
    QVec avg_Q{};
    double avg_Q_xx{0.0};
    double avg_Q_yy{0.0};
    double avg_Q_xy{0.0};

    // Correction matrix
    std::array<std::array<double, 2>, 2> X_matrix{};
  };

  static constexpr size_t m_bins_cent = 8;
  static constexpr double m_cent_low = -0.5;
  static constexpr double m_cent_high = 79.5;

  // Holds all correction data
  // key: [Cent][Harmonic][Subdetector]
  // Harmonics {2,3,4} -> 3 elements
  // Subdetectors {S,N} -> 2 elements
  std::array<std::array<std::array<CorrectionData, 2>, 3>, m_bins_cent> m_correction_data;

  // Store harmonic orders and subdetectors for easy iteration
  static constexpr std::array<int, 3> m_harmonics = {2, 3, 4};
  static constexpr std::array<Subdetector, 2> m_subdetectors = {Subdetector::S, Subdetector::N};

  struct QAHists
  {
    TH2* h2Dummy{nullptr};
    TH2* h2EMCalBadTowersDeadv2{nullptr};
    TH3* h3JetPhiEtaPt{nullptr};
    TH3* h3JetPhiEtaPtv2{nullptr};
    TH2* h2Event{nullptr};
    TH2* h2Jet{nullptr};
    TH1* hCentrality{nullptr};
    std::array<TH3*, 3> hPsi{nullptr};
  };

  QAHists m_QA_hists;

  struct EventData
  {
    int event_id{0};
    double event_zvertex{0.0};
    double event_centrality{0.0};

    std::vector<double>* jet_pt{nullptr};
    std::vector<double>* jet_phi{nullptr};
    std::vector<double>* jet_eta{nullptr};

    std::vector<int>* sepd_channel{nullptr};
    std::vector<double>* sepd_charge{nullptr};
    std::vector<double>* sepd_phi{nullptr};

    // Array for harmonics [2, 3, 4] -> indices [0, 1, 2]
    // Array for subdetectors [S, N] -> indices [0, 1]
    std::array<std::array<QVec, 2>, 3> q_vectors;
  };

  // --- Member Variables ---
  EventData m_event_data;
  std::unique_ptr<TChain> m_chain;

  // Configuration stored as members
  std::string m_input_list;
  std::string m_input_Q_calib;
  unsigned int m_runnumber;
  long long m_events_to_process;
  std::string m_output_dir;
  std::string m_dbtag{"newcdbtag"};
  int m_bins_sample{25};

  // sEPD Bad Channels
  std::unordered_set<int> m_bad_channels;

  // Hists
  std::map<std::string, std::unique_ptr<TH1>> m_hists1D;
  std::map<std::string, std::unique_ptr<TH2>> m_hists2D;
  std::map<std::string, std::unique_ptr<TH3>> m_hists3D;
  std::map<std::string, std::unique_ptr<TProfile>> m_profiles;
  std::map<std::string, std::unique_ptr<TProfile2D>> m_profiles2D;

  // --- Private Helper Methods ---
  void setup_chain();
  void read_Q_calib();
  void process_dead_channels();
  void init_hists();

  void compute_SP_resolution(int sample);
  void compute_SP(int sample, const std::vector<std::pair<double, double>> &jet_phi_eta);
  std::vector<std::pair<double, double>> process_jets();
  void correct_QVecs();
  bool compute_QVecs();
  bool process_QVecs();
  void process_events();

  void finalize();

  void validate_results();
  void save_results() const;
};

// ====================================================================
// Implementation of the Class Methods
// ====================================================================
void JetAnalysis::setup_chain()
{
  std::cout << "Processing... setup_chain" << std::endl;

  std::ifstream file_list(m_input_list);
  if (!file_list.is_open())
  {
    throw std::runtime_error(std::format("Error: Could not open the input file list: {}", m_input_list));
  }
  std::string root_file_path;

  long long prev_events = 0;
  while (std::getline(file_list, root_file_path))
  {
    if (!root_file_path.empty())
    {
      m_chain->Add(root_file_path.c_str());

      long long curr_event = m_chain->GetEntries();
      long long events = curr_event - prev_events;
      prev_events = curr_event;

      std::cout << std::format("Reading: {}, Events: {}, Total Events: {}", root_file_path, events, curr_event) << std::endl;
      // Stop reading when enough events have been read
      if (m_events_to_process && m_chain->GetEntries() > m_events_to_process)
      {
        break;
      }
    }
  }
  std::cout << std::format("Successfully chained files. Total entries: {}", m_chain->GetEntries()) << std::endl;

  // Setup branches
  m_chain->SetBranchStatus("*", false);
  m_chain->SetBranchStatus("event_id", true);
  m_chain->SetBranchStatus("event_centrality", true);
  m_chain->SetBranchStatus("jet_phi", true);
  m_chain->SetBranchStatus("jet_eta", true);
  m_chain->SetBranchStatus("jet_pt", true);
  m_chain->SetBranchStatus("sepd_channel", true);
  m_chain->SetBranchStatus("sepd_charge", true);
  m_chain->SetBranchStatus("sepd_phi", true);

  m_chain->SetBranchAddress("event_id", &m_event_data.event_id);
  m_chain->SetBranchAddress("event_centrality", &m_event_data.event_centrality);
  m_chain->SetBranchAddress("jet_pt", &m_event_data.jet_pt);
  m_chain->SetBranchAddress("jet_phi", &m_event_data.jet_phi);
  m_chain->SetBranchAddress("jet_eta", &m_event_data.jet_eta);
  m_chain->SetBranchAddress("sepd_channel", &m_event_data.sepd_channel);
  m_chain->SetBranchAddress("sepd_charge", &m_event_data.sepd_charge);
  m_chain->SetBranchAddress("sepd_phi", &m_event_data.sepd_phi);

  std::cout << "Finished... setup_chain" << std::endl;
}

void JetAnalysis::read_Q_calib()
{
  TH1::AddDirectory(kFALSE);

  auto file = std::unique_ptr<TFile>(TFile::Open(m_input_Q_calib.c_str()));

  // Check if the file was opened successfully.
  if (!file || file->IsZombie())
  {
    throw std::runtime_error(std::format("Could not open file '{}'", m_input_Q_calib));
  }

  auto* h_sEPD_Bad_Channels = dynamic_cast<TH1*>(file->Get("h_sEPD_Bad_Channels"));

  if (!h_sEPD_Bad_Channels)
  {
    throw std::runtime_error(std::format("Could not find histogram 'h_sEPD_Bad_Channels' in file '{}'", m_input_Q_calib));
  }

  // Load Bad Channels
  for (int channel = 0; channel < h_sEPD_Bad_Channels->GetNbinsX(); ++channel)
  {
    // sEPD Channel Status
    // 0: good
    // non-zero: bad
    if (h_sEPD_Bad_Channels->GetBinContent(channel + 1))
    {
      m_bad_channels.insert(channel);
    }
  }

  for (size_t n_idx = 0; n_idx < m_harmonics.size(); ++n_idx)
  {
    int n = m_harmonics[n_idx];

    std::string S_x_avg_name = std::format("h_sEPD_Q_S_x_{}_avg", n);
    std::string S_y_avg_name = std::format("h_sEPD_Q_S_y_{}_avg", n);
    std::string N_x_avg_name = std::format("h_sEPD_Q_N_x_{}_avg", n);
    std::string N_y_avg_name = std::format("h_sEPD_Q_N_y_{}_avg", n);

    auto* h_sEPD_Q_S_x_avg = dynamic_cast<TProfile*>(file->Get(S_x_avg_name.c_str()));
    auto* h_sEPD_Q_S_y_avg = dynamic_cast<TProfile*>(file->Get(S_y_avg_name.c_str()));
    auto* h_sEPD_Q_N_x_avg = dynamic_cast<TProfile*>(file->Get(N_x_avg_name.c_str()));
    auto* h_sEPD_Q_N_y_avg = dynamic_cast<TProfile*>(file->Get(N_y_avg_name.c_str()));

    if (!h_sEPD_Q_S_x_avg)
    {
      throw std::runtime_error(std::format("Could not find histogram '{}' in file '{}'", S_x_avg_name, m_input_Q_calib));
    }
    if (!h_sEPD_Q_S_y_avg)
    {
      throw std::runtime_error(std::format("Could not find histogram '{}' in file '{}'", S_y_avg_name, m_input_Q_calib));
    }
    if (!h_sEPD_Q_N_x_avg)
    {
      throw std::runtime_error(std::format("Could not find histogram '{}' in file '{}'", N_x_avg_name, m_input_Q_calib));
    }
    if (!h_sEPD_Q_N_y_avg)
    {
      throw std::runtime_error(std::format("Could not find histogram '{}' in file '{}'", N_y_avg_name, m_input_Q_calib));
    }

    std::string S_xx_avg_name = std::format("h_sEPD_Q_S_xx_{}_avg", n);
    std::string S_yy_avg_name = std::format("h_sEPD_Q_S_yy_{}_avg", n);
    std::string S_xy_avg_name = std::format("h_sEPD_Q_S_xy_{}_avg", n);
    std::string N_xx_avg_name = std::format("h_sEPD_Q_N_xx_{}_avg", n);
    std::string N_yy_avg_name = std::format("h_sEPD_Q_N_yy_{}_avg", n);
    std::string N_xy_avg_name = std::format("h_sEPD_Q_N_xy_{}_avg", n);

    auto* h_sEPD_Q_S_xx_avg = dynamic_cast<TProfile*>(file->Get(S_xx_avg_name.c_str()));
    auto* h_sEPD_Q_S_yy_avg = dynamic_cast<TProfile*>(file->Get(S_yy_avg_name.c_str()));
    auto* h_sEPD_Q_S_xy_avg = dynamic_cast<TProfile*>(file->Get(S_xy_avg_name.c_str()));
    auto* h_sEPD_Q_N_xx_avg = dynamic_cast<TProfile*>(file->Get(N_xx_avg_name.c_str()));
    auto* h_sEPD_Q_N_yy_avg = dynamic_cast<TProfile*>(file->Get(N_yy_avg_name.c_str()));
    auto* h_sEPD_Q_N_xy_avg = dynamic_cast<TProfile*>(file->Get(N_xy_avg_name.c_str()));

    if (!h_sEPD_Q_S_xx_avg)
    {
      throw std::runtime_error(std::format("Could not find histogram '{}' in file '{}'", S_xx_avg_name, m_input_Q_calib));
    }
    if (!h_sEPD_Q_S_yy_avg)
    {
      throw std::runtime_error(std::format("Could not find histogram '{}' in file '{}'", S_yy_avg_name, m_input_Q_calib));
    }
    if (!h_sEPD_Q_S_xy_avg)
    {
      throw std::runtime_error(std::format("Could not find histogram '{}' in file '{}'", S_xy_avg_name, m_input_Q_calib));
    }
    if (!h_sEPD_Q_N_xx_avg)
    {
      throw std::runtime_error(std::format("Could not find histogram '{}' in file '{}'", N_xx_avg_name, m_input_Q_calib));
    }
    if (!h_sEPD_Q_N_yy_avg)
    {
      throw std::runtime_error(std::format("Could not find histogram '{}' in file '{}'", N_yy_avg_name, m_input_Q_calib));
    }
    if (!h_sEPD_Q_N_xy_avg)
    {
      throw std::runtime_error(std::format("Could not find histogram '{}' in file '{}'", N_xy_avg_name, m_input_Q_calib));
    }

    for (size_t cent_bin = 0; cent_bin < m_bins_cent; ++cent_bin)
    {
      int bin = static_cast<int>(cent_bin)+1;

      double Q_S_x_avg = h_sEPD_Q_S_x_avg->GetBinContent(bin);
      double Q_S_y_avg = h_sEPD_Q_S_y_avg->GetBinContent(bin);
      double Q_N_x_avg = h_sEPD_Q_N_x_avg->GetBinContent(bin);
      double Q_N_y_avg = h_sEPD_Q_N_y_avg->GetBinContent(bin);

      double Q_S_xx_avg = h_sEPD_Q_S_xx_avg->GetBinContent(bin);
      double Q_S_yy_avg = h_sEPD_Q_S_yy_avg->GetBinContent(bin);
      double Q_S_xy_avg = h_sEPD_Q_S_xy_avg->GetBinContent(bin);
      double Q_N_xx_avg = h_sEPD_Q_N_xx_avg->GetBinContent(bin);
      double Q_N_yy_avg = h_sEPD_Q_N_yy_avg->GetBinContent(bin);
      double Q_N_xy_avg = h_sEPD_Q_N_xy_avg->GetBinContent(bin);

      // Recentering Params
      m_correction_data[cent_bin][n_idx][0].avg_Q = {Q_S_x_avg, Q_S_y_avg};
      m_correction_data[cent_bin][n_idx][1].avg_Q = {Q_N_x_avg, Q_N_y_avg};

      // Flattening Params
      m_correction_data[cent_bin][n_idx][0].avg_Q_xx = Q_S_xx_avg;
      m_correction_data[cent_bin][n_idx][0].avg_Q_yy = Q_S_yy_avg;
      m_correction_data[cent_bin][n_idx][0].avg_Q_xy = Q_S_xy_avg;

      m_correction_data[cent_bin][n_idx][1].avg_Q_xx = Q_N_xx_avg;
      m_correction_data[cent_bin][n_idx][1].avg_Q_yy = Q_N_yy_avg;
      m_correction_data[cent_bin][n_idx][1].avg_Q_xy = Q_N_xy_avg;

      // ----
      // South
      // ----

      double D_term_S_arg = (Q_S_xx_avg * Q_S_yy_avg) - (Q_S_xy_avg * Q_S_xy_avg);
      if (D_term_S_arg <= 0)
      {
        throw std::runtime_error(std::format(
            "Invalid calibration data: D-term sqrt argument is non-positive ({}) "
            "for harmonic n={}, centrality bin {}, subdetector S. "
            "Check calibration file statistics.",
            D_term_S_arg, n, cent_bin));
      }
      double D_term_S = std::sqrt(D_term_S_arg);

      double N_term_S = D_term_S * (Q_S_xx_avg + Q_S_yy_avg + (2 * D_term_S));
      if (N_term_S <= 0)
      {
        throw std::runtime_error(std::format(
            "Invalid calibration data: N-term denominator sqrt argument is non-positive ({}) "
            "for harmonic n={}, centrality bin {}, subdetector S. "
            "Check calibration file statistics.",
            N_term_S, n, cent_bin));
      }

      auto& X_S_matrix = m_correction_data[cent_bin][n_idx][0].X_matrix;
      X_S_matrix[0][0] = (1. / std::sqrt(N_term_S)) * (Q_S_yy_avg + D_term_S);
      X_S_matrix[0][1] = (-1. / std::sqrt(N_term_S)) * Q_S_xy_avg;
      X_S_matrix[1][0] = X_S_matrix[0][1];
      X_S_matrix[1][1] = (1. / std::sqrt(N_term_S)) * (Q_S_xx_avg + D_term_S);

      // ----
      // North
      // ----

      double D_term_N_arg = (Q_N_xx_avg * Q_N_yy_avg) - (Q_N_xy_avg * Q_N_xy_avg);
      if (D_term_N_arg <= 0)
      {
        throw std::runtime_error(std::format(
            "Invalid calibration data: D-term sqrt argument is non-positive ({}) "
            "for harmonic n={}, centrality bin {}, subdetector N. "
            "Check calibration file statistics.",
            D_term_N_arg, n, cent_bin));
      }
      double D_term_N = std::sqrt(D_term_N_arg);

      double N_term_N = D_term_N * (Q_N_xx_avg + Q_N_yy_avg + (2 * D_term_N));
      if (N_term_N <= 0)
      {
        throw std::runtime_error(std::format(
            "Invalid calibration data: N-term denominator sqrt argument is non-positive ({}) "
            "for harmonic n={}, centrality bin {}, subdetector N. "
            "Check calibration file statistics.",
            N_term_N, n, cent_bin));
      }

      auto& X_N_matrix = m_correction_data[cent_bin][n_idx][1].X_matrix;
      X_N_matrix[0][0] = (1. / std::sqrt(N_term_N)) * (Q_N_yy_avg + D_term_N);
      X_N_matrix[0][1] = (-1. / std::sqrt(N_term_N)) * Q_N_xy_avg;
      X_N_matrix[1][0] = X_N_matrix[0][1];
      X_N_matrix[1][1] = (1. / std::sqrt(N_term_N)) * (Q_N_xx_avg + D_term_N);
    }
  }
}

void JetAnalysis::process_dead_channels()
{
  std::cout << "Processing... dead_channels" << std::endl;
  setGlobalTag(m_dbtag);

  std::string emcal_hotmap = getCalibration("CEMC_BadTowerMap", m_runnumber);

  std::cout << std::format("EMCal Bad Tower Map: {}\n", emcal_hotmap);

  std::unique_ptr<CDBTTree> cdbttree_hotMap = std::make_unique<CDBTTree>(emcal_hotmap);

  const std::string fieldname = "status";

  std::string title = std::format("EMCal Dead: Run {} ; Tower Index #phi; Tower Index #eta", m_runnumber);
  m_hists2D["h2EMCalBadTowersDead"] = std::make_unique<TH2F>("h2EMCalBadTowersDead", title.c_str(), myUtils::m_nphi, 0, myUtils::m_nphi, myUtils::m_neta, 0, myUtils::m_neta);

  auto* h2Dead = m_hists2D["h2EMCalBadTowersDead"].get();

  // bad tower map hist
  unsigned int nTowers = static_cast<unsigned int>(myUtils::m_nphi * myUtils::m_neta);

  // status counters
  std::map<std::string, int> ctr;

  for (unsigned int channel = 0; channel < nTowers; ++channel)
  {
    unsigned int key = TowerInfoDefs::encode_emcal(channel);
    unsigned int phi = TowerInfoDefs::getCaloTowerPhiBin(key);
    unsigned int eta = TowerInfoDefs::getCaloTowerEtaBin(key);

    int hotMap_val = cdbttree_hotMap->GetIntValue(static_cast<int>(key), fieldname);
    if (hotMap_val == 1)
    {
      h2Dead->Fill(phi, eta);
      ++ctr["dead"];
    }
  }

  std::cout << std::format("Dead Towers: {}\n", ctr["dead"]);

  m_hists2D["h2EMCalBadTowersDeadv2"] = std::unique_ptr<TH2>(static_cast<TH2*>(h2Dead->Clone("h2EMCalBadTowersDeadv2")));

  auto* h2Deadv2 = m_hists2D["h2EMCalBadTowersDeadv2"].get();
  h2Deadv2->Rebin2D(myUtils::m_ntowIBSide, myUtils::m_ntowIBSide);

  // half of the IB
  int ib_dead_tower_threshold = 32;

  for (int iphi = 1; iphi <= h2Deadv2->GetNbinsX(); ++iphi)
  {
    for (int ieta = 1; ieta <= h2Deadv2->GetNbinsY(); ++ieta)
    {
      int nDead = static_cast<int>(h2Deadv2->GetBinContent(iphi, ieta));

      if (nDead >= ib_dead_tower_threshold)
      {
        h2Deadv2->SetBinContent(iphi, ieta, 1);
      }
      else
      {
        h2Deadv2->SetBinContent(iphi, ieta, 0);
      }
    }
  }

  for (int iphi = 1; iphi <= h2Deadv2->GetNbinsX(); ++iphi)
  {
    for (int ieta = 1; ieta <= h2Deadv2->GetNbinsY(); ++ieta)
    {
      int status = static_cast<int>(h2Deadv2->GetBinContent(iphi, ieta));

      if (status == 1)
      {
        for (int dphi = -1; dphi <= 1; ++dphi)
        {
          for (int deta = -1; deta <= 1; ++deta)
          {
            if (dphi == 0 && deta == 0)
            {
              continue;
            }

            h2Deadv2->SetBinContent(iphi + dphi, ieta + deta, 2);
          }
        }
      }
    }
  }

  std::cout << "Finished... dead_channels" << std::endl;
}

void JetAnalysis::init_hists()
{
  int bins_phi = 64;
  double phi_low = 0;
  double phi_high = 2 * std::numbers::pi;

  int bins_eta = 24;
  double eta_low = -1.152;
  double eta_high = 1.152;

  int bins_pt = 50;
  double pt_low = 0;
  double pt_high = 50;

  double sample_low = -0.5;
  double sample_high = m_bins_sample-0.5;

  int bins_SP = 400;
  double SP_low = -1;
  double SP_high = 1;

  int bins_psi = 126;
  double psi_low = -std::numbers::pi;
  double psi_high = std::numbers::pi;

  m_hists3D["h3JetPhiEtaPt"] = std::make_unique<TH3F>("h3JetPhiEtaPt", "Jet: |z| < 10 cm and MB; #phi; #eta; p_{T} [GeV]"
                                                      , bins_phi, phi_low, phi_high
                                                      , bins_eta, eta_low, eta_high
                                                      , bins_pt, pt_low, pt_high);
  m_hists3D["h3JetPhiEtaPtv2"] = std::unique_ptr<TH3>(static_cast<TH3*>(m_hists3D["h3JetPhiEtaPt"]->Clone("h3JetPhiEtaPtv2")));

  m_hists2D["h2Event"] = std::make_unique<TH2F>("h2Event", "Events: |z| < 10 and MB; Centrality [%]; Sample"
                                                , m_bins_cent, m_cent_low, m_cent_high, m_bins_sample, sample_low, sample_high);
  m_hists2D["h2Jet"] = std::make_unique<TH2F>("h2Jet", "Jets; Centrality [%]; Sample"
                                              , m_bins_cent, m_cent_low, m_cent_high, m_bins_sample, sample_low, sample_high);

  m_hists1D["hCentrality"] = std::make_unique<TH1F>("hCentrality", "Centrality: |z| < 10 cm and MB; Centrality [%]; Events"
                                                    , m_bins_cent, m_cent_low, m_cent_high);

  for (auto n : m_harmonics)
  {
    // TH3 for Scalar Product
    std::string name = std::format("h3SP_re_{}", n);
    std::string title = std::format("Scalar Product (Order {}); Centrality [%]; Sample; SP", n);

    m_hists3D[name] = std::make_unique<TH3F>(name.c_str(), title.c_str(), m_bins_cent, m_cent_low, m_cent_high, m_bins_sample, sample_low, sample_high, bins_SP, SP_low, SP_high);

    std::string name_im = std::format("h3SP_im_{}", n);
    m_hists3D[name_im] = std::unique_ptr<TH3>(static_cast<TH3*>(m_hists3D[name]->Clone(name_im.c_str())));

    // TProfile2D for Scalar Product
    std::string name_re_prof = std::format("h2SP_re_prof_{}", n);
    std::string name_im_prof = std::format("h2SP_im_prof_{}", n);

    title = std::format("Scalar Product (Order {}); Centrality [%]; Sample", n);
    m_profiles2D[name_re_prof] = std::make_unique<TProfile2D>(name_re_prof.c_str(), title.c_str(), m_bins_cent, m_cent_low, m_cent_high, m_bins_sample, sample_low, sample_high);
    m_profiles2D[name_im_prof] = std::unique_ptr<TProfile2D>(static_cast<TProfile2D*>(m_profiles2D[name_re_prof]->Clone(name_im_prof.c_str())));

    // TH3 for Detector Resolution
    std::string name_res = std::format("h3SP_res_{}", n);
    title = std::format("sEPD (Order {0}): Q^{{S}}Q^{{N*}}; Centrality [%]; Sample; Re(Q^{{S}}_{{{0}}} Q^{{N*}}_{{{0}}}GT)", n);
    m_hists3D[name_res] = std::unique_ptr<TH3>(static_cast<TH3*>(m_hists3D[name]->Clone(name_res.c_str())));
    m_hists3D[name_res]->SetTitle(title.c_str());

    // TProfile2D for Detector Resolution
    std::string name_res_prof = std::format("h2SP_res_prof_{}", n);
    title = std::format("sEPD: Re(#LT Q^{{S}}_{{{0}}} Q^{{N*}}_{{{0}}}#GT); Centrality [%]; Sample", n);
    m_profiles2D[name_res_prof] = std::make_unique<TProfile2D>(name_res_prof.c_str(), title.c_str(), m_bins_cent, m_cent_low, m_cent_high, m_bins_sample, sample_low, sample_high);

    std::string name_vn_re = std::format("h2Vn_re_{}", n);
    std::string name_vn_im = std::format("h2Vn_im_{}", n);

    title = std::format("Jet v_{{{0}}}; Centrality [%]; v_{{{0}}}", n);

    m_hists2D[name_vn_re] = std::make_unique<TH2F>(name_vn_re.c_str(), title.c_str(), m_bins_cent, m_cent_low, m_cent_high, bins_SP, SP_low, SP_high);
    m_hists2D[name_vn_im] = std::unique_ptr<TH2>(static_cast<TH2*>(m_hists2D[name_vn_re]->Clone(name_vn_im.c_str())));


    std::string psi_corr2_hist_name = std::format("h3_sEPD_Psi_{}_corr2", n);
    m_hists3D[psi_corr2_hist_name] = std::make_unique<TH3F>(psi_corr2_hist_name.c_str(),
                                                            std::format("sEPD #Psi (Order {0}): |z| < 10 cm and MB; {0}#Psi^{{S}}_{{{0}}}; {0}#Psi^{{N}}_{{{0}}}; Centrality [%]", n).c_str(),
                                                            bins_psi, psi_low, psi_high, bins_psi, psi_low, psi_high, m_bins_cent, m_cent_low, m_cent_high);
  }

  m_hists2D["h2Dummy"] = std::unique_ptr<TH2>(static_cast<TH2*>(m_hists3D["h3JetPhiEtaPt"]->Project3D("yx")->Clone("h2Dummy")));
  m_hists2D["h2Dummy"]->Rebin2D(2, 2);

  m_QA_hists.h2Dummy = m_hists2D["h2Dummy"].get();
  m_QA_hists.h2EMCalBadTowersDeadv2 = m_hists2D["h2EMCalBadTowersDeadv2"].get();
  m_QA_hists.h3JetPhiEtaPt = m_hists3D["h3JetPhiEtaPt"].get();
  m_QA_hists.h3JetPhiEtaPtv2 = m_hists3D["h3JetPhiEtaPtv2"].get();
  m_QA_hists.h2Event = m_hists2D["h2Event"].get();
  m_QA_hists.h2Jet = m_hists2D["h2Jet"].get();
  m_QA_hists.hCentrality = m_hists1D["hCentrality"].get();

  for (size_t n_idx = 0; n_idx < m_harmonics.size(); ++n_idx)
  {
    int n = m_harmonics[n_idx];
    std::string psi_corr2_hist_name = std::format("h3_sEPD_Psi_{}_corr2", n);
    m_QA_hists.hPsi[n_idx] = m_hists3D[psi_corr2_hist_name].get();
  }
}

void JetAnalysis::correct_QVecs()
{
  double cent = m_event_data.event_centrality;
  size_t cent_bin = static_cast<size_t>(m_hists1D["hCentrality"]->FindBin(cent) - 1);

  for (size_t n_idx = 0; n_idx < m_harmonics.size(); ++n_idx)
  {
    double Q_S_x_avg = m_correction_data[cent_bin][n_idx][0].avg_Q.x;
    double Q_S_y_avg = m_correction_data[cent_bin][n_idx][0].avg_Q.y;
    double Q_N_x_avg = m_correction_data[cent_bin][n_idx][1].avg_Q.x;
    double Q_N_y_avg = m_correction_data[cent_bin][n_idx][1].avg_Q.y;

    QVec q_S = m_event_data.q_vectors[n_idx][0];
    QVec q_N = m_event_data.q_vectors[n_idx][1];

    // Apply Recentering
    QVec q_S_corr = {q_S.x - Q_S_x_avg, q_S.y - Q_S_y_avg};
    QVec q_N_corr = {q_N.x - Q_N_x_avg, q_N.y - Q_N_y_avg};

    const auto& X_S = m_correction_data[cent_bin][n_idx][0].X_matrix;
    const auto& X_N = m_correction_data[cent_bin][n_idx][1].X_matrix;

    // Apply Flattening
    double Q_S_x_corr2 = X_S[0][0] * q_S_corr.x + X_S[0][1] * q_S_corr.y;
    double Q_S_y_corr2 = X_S[1][0] * q_S_corr.x + X_S[1][1] * q_S_corr.y;
    double Q_N_x_corr2 = X_N[0][0] * q_N_corr.x + X_N[0][1] * q_N_corr.y;
    double Q_N_y_corr2 = X_N[1][0] * q_N_corr.x + X_N[1][1] * q_N_corr.y;

    m_event_data.q_vectors[n_idx][0] = {Q_S_x_corr2, Q_S_y_corr2};
    m_event_data.q_vectors[n_idx][1] = {Q_N_x_corr2, Q_N_y_corr2};

    double psi_S = std::atan2(Q_S_y_corr2, Q_S_x_corr2);
    double psi_N = std::atan2(Q_N_y_corr2, Q_N_x_corr2);

    m_QA_hists.hPsi[n_idx]->Fill(psi_S, psi_N, cent);
  }
}

bool JetAnalysis::compute_QVecs()
{
  size_t nChannels = m_event_data.sepd_channel->size();

  double sepd_total_charge_south = 0;
  double sepd_total_charge_north = 0;

  // Loop over all sEPD Channels
  for (size_t idx = 0; idx < nChannels; ++idx)
  {
    int channel = m_event_data.sepd_channel->at(idx);
    double charge = m_event_data.sepd_charge->at(idx);
    double phi = m_event_data.sepd_phi->at(idx);

    // Skip Bad Channels
    if (m_bad_channels.contains(channel))
    {
      continue;
    }

    unsigned int key = TowerInfoDefs::encode_epd(static_cast<unsigned int>(channel));
    unsigned int arm = TowerInfoDefs::get_epd_arm(key);

    // arm = 0: South
    // arm = 1: North
    double& sepd_total_charge = (arm == 0) ? sepd_total_charge_south : sepd_total_charge_north;

    // Compute total charge for the respective sEPD arm
    sepd_total_charge += charge;

    // Compute Raw Q vectors for each harmonic and respective arm
    for (size_t n_idx = 0; n_idx < m_harmonics.size(); ++n_idx)
    {
      int n = m_harmonics[n_idx];
      m_event_data.q_vectors[n_idx][arm].x += charge * std::cos(n * phi);
      m_event_data.q_vectors[n_idx][arm].y += charge * std::sin(n * phi);
    }
  }

  // Skip Events with Zero sEPD Total Charge in either arm
  if (sepd_total_charge_south == 0 || sepd_total_charge_north == 0)
  {
    std::cout << std::format("Warning! Q-Vec Zero: Event {}\n", m_event_data.event_id);
    return false;
  }

  // Normalize the Q-vectors by total charge
  for (size_t n_idx = 0; n_idx < m_harmonics.size(); ++n_idx)
  {
    for (auto det : m_subdetectors)
    {
      size_t arm = static_cast<size_t>(det);
      double sepd_total_charge = (arm == 0) ? sepd_total_charge_south : sepd_total_charge_north;
      m_event_data.q_vectors[n_idx][arm].x /= sepd_total_charge;
      m_event_data.q_vectors[n_idx][arm].y /= sepd_total_charge;
    }
  }

  return true;
}

void JetAnalysis::compute_SP_resolution(int sample)
{
  double cent = m_event_data.event_centrality;
  m_QA_hists.h2Event->Fill(cent, sample);

  // Compute Scalar Product for each harmonic
  for (size_t n_idx = 0; n_idx < m_harmonics.size(); ++n_idx)
  {
    int n = m_harmonics[n_idx];
    std::string name = std::format("h3SP_res_{}", n);
    std::string name_prof = std::format("h2SP_res_prof_{}", n);
    QVec sEPD_Q_S = m_event_data.q_vectors[n_idx][0];
    QVec sEPD_Q_N = m_event_data.q_vectors[n_idx][1];

    double SP_res = sEPD_Q_S.x * sEPD_Q_N.x + sEPD_Q_S.y * sEPD_Q_N.y;

    m_hists3D[name]->Fill(cent, sample, SP_res);
    m_profiles2D[name_prof]->Fill(cent, sample, SP_res);
  }
}

void JetAnalysis::compute_SP(int sample, const std::vector<std::pair<double, double>>& jet_phi_eta)
{
  size_t nJets = jet_phi_eta.size();
  double cent = m_event_data.event_centrality;

  m_QA_hists.h2Jet->Fill(cent, sample, static_cast<int>(nJets));

  // Loop over all jets
  for (size_t idx = 0; idx < nJets; ++idx)
  {
    double phi = jet_phi_eta[idx].first;
    double eta = jet_phi_eta[idx].second;

    size_t arm = (eta < 0) ? static_cast<size_t>(Subdetector::N) : static_cast<size_t>(Subdetector::S);

    // Compute Scalar Product for each harmonic
    for (size_t n_idx = 0; n_idx < m_harmonics.size(); ++n_idx)
    {
      int n = m_harmonics[n_idx];
      QVec jet_Q = {std::cos(n * phi), std::sin(n * phi)};
      QVec sEPD_Q = m_event_data.q_vectors[n_idx][arm];

      double SP_re = jet_Q.x * sEPD_Q.x + jet_Q.y * sEPD_Q.y;
      double SP_im = jet_Q.y * sEPD_Q.x - jet_Q.x * sEPD_Q.y;

      std::string name_re = std::format("h3SP_re_{}", n);
      std::string name_im = std::format("h3SP_im_{}", n);

      std::string name_re_prof = std::format("h2SP_re_prof_{}", n);
      std::string name_im_prof = std::format("h2SP_im_prof_{}", n);

      m_hists3D[name_re]->Fill(cent, sample, SP_re);
      m_hists3D[name_im]->Fill(cent, sample, SP_im);

      m_profiles2D[name_re_prof]->Fill(cent, sample, SP_re);
      m_profiles2D[name_im_prof]->Fill(cent, sample, SP_im);
    }
  }
}

std::vector<std::pair<double, double>> JetAnalysis::process_jets()
{
  size_t nJets = m_event_data.jet_phi->size();

  std::vector<std::pair<double, double>> jet_phi_eta;
  jet_phi_eta.reserve(nJets);

  // Loop over all jets
  for (size_t idx = 0; idx < nJets; ++idx)
  {
    double pt = m_event_data.jet_pt->at(idx);
    double phi = m_event_data.jet_phi->at(idx);
    double eta = m_event_data.jet_eta->at(idx);

    // map [-pi,pi] -> [0,2pi]
    if (phi < 0)
    {
      phi += 2.0 * std::numbers::pi;
    }

    int bin_phi = m_QA_hists.h2Dummy->GetXaxis()->FindBin(phi);
    int bin_eta = m_QA_hists.h2Dummy->GetYaxis()->FindBin(eta);
    int dead_status = static_cast<int>(m_QA_hists.h2EMCalBadTowersDeadv2->GetBinContent(bin_phi, bin_eta));

    m_QA_hists.h3JetPhiEtaPt->Fill(phi, eta, pt);

    if (dead_status == 0)
    {
      m_QA_hists.h3JetPhiEtaPtv2->Fill(phi, eta, pt);
      jet_phi_eta.push_back(std::make_pair(phi, eta));
    }
  }

  return jet_phi_eta;
}

bool JetAnalysis::process_QVecs()
{
  bool isGood = compute_QVecs();
  correct_QVecs();

  return isGood;
}

void JetAnalysis::process_events()
{
  std::cout << "Processing... process_events" << std::endl;
  long long n_entries = m_chain->GetEntries();
  if (m_events_to_process)
  {
    n_entries = std::min(m_events_to_process, n_entries);
  }

  // Event Loop
  for (long long event = 0; event < n_entries; ++event)
  {
    // Load Event Data from TChain
    m_chain->GetEntry(event);

    if (event % 5000 == 0)
    {
      std::cout << std::format("Processing {}/{}: {:.2f} %", event, n_entries, static_cast<double>(event) * 100. / static_cast<double>(n_entries)) << std::endl;
    }

    double cent = m_event_data.event_centrality;
    int cent_bin = m_QA_hists.hCentrality->FindBin(cent);

    // Skip events in underflow or overflow bins.
    if (cent_bin <= 0 || cent_bin > static_cast<int>(m_bins_cent))
    {
      std::cout << std::format("Warning: Weird Centrality: {}, Skipping Event\n", cent);
      continue;
    }

    // Reset Q-Vectors for the new event
    for (auto& q_vec_harmonic : m_event_data.q_vectors)
    {
      for (auto& q_vec : q_vec_harmonic)
      {
        q_vec.x = 0.0;
        q_vec.y = 0.0;
      }
    }

    // compute and correct the sEPD Q vectors
    bool isGood = process_QVecs();
    if (!isGood)
    {
      continue;
    }

    int events_cent = static_cast<int>(m_QA_hists.hCentrality->GetBinContent(cent_bin));
    int sample = events_cent % m_bins_sample;
    m_QA_hists.hCentrality->Fill(cent);

    std::vector<std::pair<double, double>> jet_phi_eta = process_jets();

    // Scalar Product Method
    compute_SP(sample, jet_phi_eta);

    // Scalar Product Resolution
    compute_SP_resolution(sample);
  }

  int jets = static_cast<int>(m_QA_hists.h3JetPhiEtaPt->GetEntries());
  int jets_good = static_cast<int>(m_QA_hists.h3JetPhiEtaPtv2->GetEntries());

  std::cout << std::format("Jets: {}, Post Masking: {}, {:.2f} %\n", jets, jets_good, jets_good * 100. / jets);

  std::cout << "Finished... process_events" << std::endl;
}

void JetAnalysis::finalize()
{
  for (auto n : m_harmonics)
  {
    std::string name_re = std::format("h2SP_re_prof_{}", n);
    std::string name_im = std::format("h2SP_im_prof_{}", n);
    std::string name_res = std::format("h2SP_res_prof_{}", n);

    std::string name_vn_re = std::format("h2Vn_re_{}", n);
    std::string name_vn_im = std::format("h2Vn_im_{}", n);

    auto* prof_re = m_profiles2D[name_re].get();
    auto* prof_im = m_profiles2D[name_im].get();
    auto* prof_res = m_profiles2D[name_res].get();

    auto* h2Vn_re = m_hists2D[name_vn_re].get();
    auto* h2Vn_im = m_hists2D[name_vn_im].get();

    for (int cent_bin = 0; cent_bin < static_cast<int>(m_bins_cent); ++cent_bin)
    {
      double cent = h2Vn_re->GetXaxis()->GetBinCenter(cent_bin + 1);

      for (int sample_bin = 0; sample_bin < m_bins_sample; ++sample_bin)
      {
        double SP_re = prof_re->GetBinContent(cent_bin + 1, sample_bin + 1);
        double SP_im = prof_im->GetBinContent(cent_bin + 1, sample_bin + 1);
        double SP_res = prof_res->GetBinContent(cent_bin + 1, sample_bin + 1);

        int nJets = static_cast<int>(m_QA_hists.h2Jet->GetBinContent(cent_bin + 1, sample_bin + 1));

        if (SP_res > 0)
        {
          double vn_re = SP_re / std::sqrt(SP_res);
          double vn_im = SP_im / std::sqrt(SP_res);

          h2Vn_re->Fill(cent, vn_re, nJets);
          h2Vn_im->Fill(cent, vn_im, nJets);
        }
      }
    }
  }
}

void JetAnalysis::validate_results()
{
  std::cout << std::format("{:#<{}}\n", "", 40);
  std::cout << std::format("Validating Results: Compute CV\n");
  std::cout << std::format("{:#<{}}\n", "", 40);

  for (int n : m_harmonics)
  {
    std::string psi_corr2_hist_name = std::format("h3_sEPD_Psi_{}_corr2", n);

    auto* h_psi_corr2 = m_hists3D[psi_corr2_hist_name].get();
    int bins_phi = h_psi_corr2->GetNbinsX();

    // South, North
    for (auto det : m_subdetectors)
    {
      std::string side = (det == Subdetector::S) ? "x" : "y";
      std::string det_name = (det == Subdetector::S) ? "South" : "North";
      std::string cv_name = std::format("h_sEPD_CV_{}_{}", det_name, n);
      std::string cv_title = std::format("sEPD {}: Order {}; Centrality [%]; #sigma/#mu", det_name, n);

      m_hists1D[cv_name] = std::make_unique<TH1F>(cv_name.c_str(), cv_title.c_str(), m_bins_cent, m_cent_low, m_cent_high);

      for (int cent_bin = 1; cent_bin <= static_cast<int>(m_bins_cent); ++cent_bin)
      {
        h_psi_corr2->GetZaxis()->SetRange(cent_bin, cent_bin);
        auto* h_psi = h_psi_corr2->Project3D(side.c_str());

        int cent_events = static_cast<int>(m_hists1D["hCentrality"]->GetBinContent(cent_bin));
        int psi_events = static_cast<int>(h_psi->Integral());

        if (cent_events != psi_events)
        {
          std::cout << std::format("ERROR: Cent Events: {}, Psi Events: {}\n", cent_events, psi_events);
        }

        double mean = 0;
        double M2 = 0;

        for (int phi_bin = 1; phi_bin <= bins_phi; ++phi_bin)
        {
          double val = h_psi->GetBinContent(phi_bin);
          double delta = val - mean;
          mean += delta / phi_bin;
          double delta2 = val - mean;
          M2 += delta * delta2;
        }

        double sigma = std::sqrt(M2 / bins_phi);
        double cv = (mean) ? sigma / mean : 0;
        // Derived from the Delta Method
        double cv_error = cv * std::sqrt(1. / bins_phi + cv * cv / (2 * bins_phi));

        m_hists1D[cv_name]->SetBinContent(cent_bin, cv);
        m_hists1D[cv_name]->SetBinError(cent_bin, cv_error);

        std::cout << std::format("n: {}, det: {}, cent: {}, mean: {:6.2f}, sigma: {:5.2f}, cv: {:.2f}, cv_err: {:.4f}\n",
                                 n, side, cent_bin, mean, sigma, cv, cv_error);
      }
    }

    // set the range back to full after calculations
    h_psi_corr2->GetZaxis()->SetRange(1, m_bins_cent);
  }
}

void JetAnalysis::save_results() const
{
  std::filesystem::create_directories(m_output_dir);
  std::string output_filename = m_output_dir + "/Jet-Ana-test.root";
  auto output_file = std::make_unique<TFile>(output_filename.c_str(), "RECREATE");

  for (const auto& [name, hist] : m_hists1D)
  {
    std::cout << std::format("Saving: {}\n", name);
    hist->Write();
  }
  for (const auto& [name, hist] : m_hists2D)
  {
    std::cout << std::format("Saving: {}\n", name);
    hist->Write();
  }
  for (const auto& [name, hist] : m_hists3D)
  {
    std::cout << std::format("Saving: {}\n", name);
    hist->Write();
  }
  for (const auto& [name, hist] : m_profiles)
  {
    std::cout << std::format("Saving: {}\n", name);
    hist->Write();
  }
  for (const auto& [name, hist] : m_profiles2D)
  {
    std::cout << std::format("Saving: {}\n", name);
    hist->Write();
  }

  // Save Projections for Convenience
  auto project_and_write = [&](const std::string& hist_name, const std::string& projection)
  {
    auto* hist = m_hists3D.at(hist_name).get();
    hist->Project3D(projection.c_str())->Write();
  };

  project_and_write("h3JetPhiEtaPt", "z");
  project_and_write("h3JetPhiEtaPt", "yx");
  project_and_write("h3JetPhiEtaPtv2", "z");
  project_and_write("h3JetPhiEtaPtv2", "yx");

  output_file->Close();

  std::cout << std::format("Results saved to: {}", output_filename) << std::endl;
}

// ====================================================================
// Main function remains clean and simple
// ====================================================================
int main(int argc, const char* const argv[])
{
  gROOT->SetBatch(true);

  if (argc < 4 || argc > 6)
  {
    std::cout << "Usage: " << argv[0] << " <input_list_file> <input_SEPD_Q_vec_calib> <runnumber> [events] [output_directory]" << std::endl;
    return 1;
  }

  const std::string input_list = argv[1];
  const std::string input_Q_calib = argv[2];
  unsigned int runnumber = static_cast<unsigned int>(std::atoi(argv[3]));
  long long events = (argc >= 5) ? std::atoll(argv[4]) : 0;
  std::string output_dir = (argc >= 6) ? argv[5] : ".";

  try
  {
    JetAnalysis analysis(input_list, input_Q_calib, runnumber, events, output_dir);
    analysis.run();
  }
  catch (const std::exception& e)
  {
    std::cout << std::format("An exception occurred: {}", e.what()) << std::endl;
    return 1;
  }

  std::cout << "Analysis complete." << std::endl;
  return 0;
}
