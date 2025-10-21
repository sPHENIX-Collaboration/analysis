// ====================================================================
// sPHENIX Includes
// ====================================================================
#include <calobase/TowerInfoDefs.h>

// ====================================================================
// Standard C++ Includes
// ====================================================================
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
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
#include <TROOT.h>
#include <TTree.h>

// ====================================================================
// The Analysis Class
// ====================================================================
class QvectorAnalysis
{
 public:
  // The constructor takes the configuration
  QvectorAnalysis(std::string input_file, std::string input_hist, std::string input_Q_calib, int q_vec_ana, int pass, long long events, std::string output_dir)
    : m_chain(std::make_unique<TChain>("T"))
    , m_input_file(std::move(input_file))
    , m_input_hist(std::move(input_hist))
    , m_input_Q_calib(std::move(input_Q_calib))
    , m_pass(static_cast<Pass>(pass))
    , m_QVecAna(static_cast<QVecAna>(q_vec_ana))
    , m_events_to_process(events)
    , m_output_dir(std::move(output_dir))
  {
  }

  void run()
  {
    setup_chain();
    process_hot_channels();
    init_hists();
    process_events();
    save_results();
  }

  enum class QVecAna
  {
    DEFAULT, // All rbins
    HALF,    // ONLY RBINS 0 to 7
    HALF1    // ONLY RBINS 1 to 7
  };

  enum class Pass
  {
    ComputeRecentering,
    ApplyRecentering,
    ApplyFlattening
  };

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

  static constexpr size_t m_cent_bins = 8;
  double m_cent_low = -0.5;
  double m_cent_high = 79.5;

  // Holds all correction data
  // key: [Cent][Harmonic][Subdetector]
  // Harmonics {2,3,4} -> 3 elements
  // Subdetectors {S,N} -> 2 elements
  std::array<std::array<std::array<CorrectionData, 2>, 3>, m_cent_bins> m_correction_data;

  // Store harmonic orders and subdetectors for easy iteration
  static constexpr std::array<int, 3> m_harmonics = {2, 3, 4};
  static constexpr std::array<Subdetector, 2> m_subdetectors = {Subdetector::S, Subdetector::N};
  static constexpr std::array<QComponent, 2> m_components = {QComponent::X, QComponent::Y};

  struct EventData
  {
    int event_id{0};
    double event_zvertex{0.0};
    double event_centrality{0.0};

    std::array<std::array<QVec, 2>, 3> q_vectors;

    std::vector<int>* sepd_channel{nullptr};
    std::vector<double>* sepd_charge{nullptr};
    std::vector<double>* sepd_phi{nullptr};
  };

  // Helper to map harmonics to array indices
  static constexpr size_t harmonic_to_index(int n)
  {
    if (n == 2)
    {
      return 0;
    }
    if (n == 3)
    {
      return 1;
    }
    if (n == 4)
    {
      return 2;
    }
    throw std::out_of_range("Invalid harmonic");
  }

  struct AverageHists
  {
    TProfile* S_x_avg{nullptr};
    TProfile* S_y_avg{nullptr};
    TProfile* N_x_avg{nullptr};
    TProfile* N_y_avg{nullptr};

    TH3* Q_S{nullptr};
    TH3* Q_N{nullptr};

    TH3* Psi{nullptr};
  };

  struct RecenterHists
  {
    TProfile* S_x_corr_avg{nullptr};
    TProfile* S_y_corr_avg{nullptr};
    TProfile* N_x_corr_avg{nullptr};
    TProfile* N_y_corr_avg{nullptr};

    TProfile* S_xx_avg{nullptr};
    TProfile* S_yy_avg{nullptr};
    TProfile* S_xy_avg{nullptr};
    TProfile* N_xx_avg{nullptr};
    TProfile* N_yy_avg{nullptr};
    TProfile* N_xy_avg{nullptr};

    TH3* Psi_corr{nullptr};
  };

  struct FlatteningHists
  {
    TProfile* S_x_corr2_avg{nullptr};
    TProfile* S_y_corr2_avg{nullptr};
    TProfile* N_x_corr2_avg{nullptr};
    TProfile* N_y_corr2_avg{nullptr};

    TProfile* S_xx_corr_avg{nullptr};
    TProfile* S_yy_corr_avg{nullptr};
    TProfile* S_xy_corr_avg{nullptr};

    TProfile* N_xx_corr_avg{nullptr};
    TProfile* N_yy_corr_avg{nullptr};
    TProfile* N_xy_corr_avg{nullptr};

    TH3* Psi_corr2{nullptr};
  };

  // --- Member Variables ---
  EventData m_event_data;
  std::unique_ptr<TChain> m_chain;

  // Configuration stored as members
  std::string m_input_file;
  std::string m_input_hist;
  std::string m_input_Q_calib;
  Pass m_pass{0};
  QVecAna m_QVecAna{0};
  long long m_events_to_process;
  std::string m_output_dir;
  bool m_doBadChannelCheck{true};

  // Hists
  std::map<std::string, std::unique_ptr<TH1>> m_hists1D;
  std::map<std::string, std::unique_ptr<TH2>> m_hists2D;
  std::map<std::string, std::unique_ptr<TH3>> m_hists3D;
  std::map<std::string, std::unique_ptr<TProfile>> m_profiles;

  // sEPD Bad Channels
  std::unordered_set<int> m_bad_channels;
  double m_sEPD_min_avg_charge_threshold{1};
  double m_sEPD_sigma_threshold{3};
  std::unordered_set<int> m_rbins_skipped;

  // --- Private Helper Methods ---
  void setup_chain();
  void init_hists();
  void load_correction_data();
  void process_events();

  bool filter_sEPD(int rbin);
  bool process_sEPD();
  void run_event_loop();
  void save_results() const;
  static void process_averages(double cent, QVec q_S, QVec q_N, const AverageHists& h);
  void process_recentering(double cent, size_t n_idx, QVec q_S, QVec q_N, const RecenterHists& h);
  void process_flattening(double cent, size_t n_idx, QVec q_S, QVec q_N, const FlatteningHists& h);

  void compute_averages(size_t cent_bin, int n);
  void compute_recentering(size_t cent_bin, int n);
  void print_flattening(size_t cent_bin, int n) const;

  std::map<int, AverageHists> prepare_average_hists();
  std::map<int, RecenterHists> prepare_recenter_hists();
  std::map<int, FlatteningHists> prepare_flattening_hists();

  void process_hot_channels();
};

// ====================================================================
// Implementation of the Class Methods
// ====================================================================
void QvectorAnalysis::setup_chain()
{
  std::cout << "Processing... setup_chain" << std::endl;

  if (!std::filesystem::exists(m_input_file))
  {
    throw std::runtime_error(std::format("Input file does not exist: {}", m_input_file));
  }

  m_chain->Add(m_input_file.c_str());

  // If GetNtrees() is 0, the file might be corrupted, not a ROOT file, or is missing the TTree.
  if (m_chain->GetNtrees() == 0)
  {
    throw std::runtime_error(std::format("Could not find TTree 'T' in file: {}", m_input_file));
  }

  if (m_chain->GetEntries() == 0)
  {
    std::cout << "Warning: Input file contains 0 entries. Job will produce an empty output." << std::endl;
  }

  // Setup branches
  m_chain->SetBranchStatus("*", false);
  m_chain->SetBranchStatus("event_id", true);
  m_chain->SetBranchStatus("event_centrality", true);
  m_chain->SetBranchStatus("sepd_channel", true);
  m_chain->SetBranchStatus("sepd_charge", true);
  m_chain->SetBranchStatus("sepd_phi", true);

  m_chain->SetBranchAddress("event_id", &m_event_data.event_id);
  m_chain->SetBranchAddress("event_centrality", &m_event_data.event_centrality);
  m_chain->SetBranchAddress("sepd_channel", &m_event_data.sepd_channel);
  m_chain->SetBranchAddress("sepd_charge", &m_event_data.sepd_charge);
  m_chain->SetBranchAddress("sepd_phi", &m_event_data.sepd_phi);

  std::cout << "Finished... setup_chain" << std::endl;
}

void QvectorAnalysis::process_hot_channels()
{
  TH1::AddDirectory(kFALSE);
  auto file = std::unique_ptr<TFile>(TFile::Open(m_input_hist.c_str()));

  // Check if the file was opened successfully.
  if (!file || file->IsZombie())
  {
    throw std::runtime_error(std::format("Could not open file '{}'", m_input_hist));
  }

  std::string sepd_charge_hist = "hSEPD_Charge";

  auto* hist = file->Get(sepd_charge_hist.c_str());

  // Check if the hist is stored in the file
  if (hist == nullptr)
  {
    throw std::runtime_error(std::format("Cannot find hist: {}", sepd_charge_hist));
  }

  auto* hSEPD_Charge = dynamic_cast<TH1*>(hist);

  int sepd_channels = 744;
  int rbins = 16;
  int bins_charge = 40;

  m_hists2D["h2SEPD_South_Charge_rbin"] = std::make_unique<TH2F>("h2SEPD_South_Charge_rbin",
                                                                 "sEPD South; r_{bin}; Avg Charge",
                                                                 rbins, -0.5, rbins - 0.5,
                                                                 bins_charge, 0, bins_charge);

  m_hists2D["h2SEPD_North_Charge_rbin"] = std::make_unique<TH2F>("h2SEPD_North_Charge_rbin",
                                                                 "sEPD North; r_{bin}; Avg Charge",
                                                                 rbins, -0.5, rbins - 0.5,
                                                                 bins_charge, 0, bins_charge);

  m_hists2D["h2SEPD_South_Charge_rbinv2"] = std::make_unique<TH2F>("h2SEPD_South_Charge_rbinv2",
                                                                   "sEPD South; r_{bin}; Avg Charge",
                                                                   rbins, -0.5, rbins - 0.5,
                                                                   bins_charge, 0, bins_charge);

  m_hists2D["h2SEPD_North_Charge_rbinv2"] = std::make_unique<TH2F>("h2SEPD_North_Charge_rbinv2",
                                                                   "sEPD North; r_{bin}; Avg Charge",
                                                                   rbins, -0.5, rbins - 0.5,
                                                                   bins_charge, 0, bins_charge);

  m_profiles["h_sEPD_Bad_Channels"] = std::make_unique<TProfile>("h_sEPD_Bad_Channels", "sEPD Bad Channels; Channel; Status", sepd_channels, -0.5, sepd_channels-0.5);

  auto* h2S = m_hists2D["h2SEPD_South_Charge_rbin"].get();
  auto* h2N = m_hists2D["h2SEPD_North_Charge_rbin"].get();

  auto* h2Sv2 = m_hists2D["h2SEPD_South_Charge_rbinv2"].get();
  auto* h2Nv2 = m_hists2D["h2SEPD_North_Charge_rbinv2"].get();

  auto* hBad = m_profiles["h_sEPD_Bad_Channels"].get();

  for (int channel = 0; channel < sepd_channels; ++channel)
  {
    unsigned int key = TowerInfoDefs::encode_epd(static_cast<unsigned int>(channel));
    int rbin = static_cast<int>(TowerInfoDefs::get_epd_rbin(key));
    unsigned int arm = TowerInfoDefs::get_epd_arm(key);

    double avg_charge = hSEPD_Charge->GetBinContent(channel + 1);

    auto* h2 = (arm == 0) ? h2S : h2N;

    h2->Fill(rbin, avg_charge);
  }

  auto* hSpx = h2S->ProfileX("hSpx", 2, -1, "s");
  auto* hNpx = h2N->ProfileX("hNpx", 2, -1, "s");

  int ctr_dead = 0;
  int ctr_hot = 0;
  int ctr_cold = 0;

  for (int channel = 0; channel < sepd_channels; ++channel)
  {
    unsigned int key = TowerInfoDefs::encode_epd(static_cast<unsigned int>(channel));
    int rbin = static_cast<int>(TowerInfoDefs::get_epd_rbin(key));
    unsigned int arm = TowerInfoDefs::get_epd_arm(key);
    auto* h2 = (arm == 0) ? h2Sv2 : h2Nv2;

    auto* hprof = (arm == 0) ? hSpx : hNpx;

    double charge = hSEPD_Charge->GetBinContent(channel + 1);
    double mean_charge = hprof->GetBinContent(rbin + 1);
    double sigma = hprof->GetBinError(rbin + 1);

    if (charge < m_sEPD_min_avg_charge_threshold || std::fabs(charge - mean_charge) > m_sEPD_sigma_threshold * sigma)
    {
      m_bad_channels.insert(channel);

      // dead channel
      if(charge == 0)
      {
          hBad->Fill(channel, 1);
          std::cout << std::format("Dead Channel: {:3d}, arm: {}, rbin: {:2d}, Mean Charge: {:5.2f}, Charge: {:5.2f}\n", channel, arm, rbin, mean_charge, charge);
          ++ctr_dead;
      }
      // hot channel
      else if (charge - mean_charge > 3 * sigma)
      {
        hBad->Fill(channel, 2);
        std::cout << std::format("Hot Channel: {:3d}, arm: {}, rbin: {:2d}, Mean Charge: {:5.2f}, Sigma: {:5.2f}, Charge: {:5.2f}\n", channel, arm, rbin, mean_charge, sigma, charge);
        ++ctr_hot;
      }
      // cold channel
      else
      {
        hBad->Fill(channel, 3);
        std::cout << std::format("Cold Channel: {:3d}, arm: {}, rbin: {:2d}, Mean Charge: {:5.2f}, Charge: {:5.2f}\n", channel, arm, rbin, mean_charge, charge);
        ++ctr_cold;
      }
    }
    else
    {
      h2->Fill(rbin, charge);
    }
  }

  std::cout << std::format("Total Bad Channels: {}, Dead: {}, Hot: {}, Cold: {}\n", m_bad_channels.size(), ctr_dead, ctr_hot, ctr_cold);

  std::cout << "Finished processing Hot sEPD channels" << std::endl;
}

void QvectorAnalysis::init_hists()
{
  unsigned int bins_Q = 100;
  double Q_low = -1;
  double Q_high = 1;

  unsigned int bins_psi = 126;
  double psi_low = -M_PI;
  double psi_high = M_PI;

  m_hists1D["h_Cent"] = std::make_unique<TH1F>("h_Cent", "", m_cent_bins, m_cent_low, m_cent_high);

  // n = 2, 3, 4, etc.
  for (int n : m_harmonics)
  {
    std::string psi_hist_name = std::format("h3_sEPD_Psi_{}", n);
    std::string psi_corr_hist_name = std::format("h3_sEPD_Psi_{}_corr", n);
    std::string psi_corr2_hist_name = std::format("h3_sEPD_Psi_{}_corr2", n);

    if (m_pass == Pass::ComputeRecentering)
    {
      m_hists3D[psi_hist_name] = std::make_unique<TH3F>(psi_hist_name.c_str(),
                                                        std::format("sEPD #Psi (Order {0}): |z| < 10 cm and MB; {0}#Psi^{{S}}_{{{0}}}; {0}#Psi^{{N}}_{{{0}}}; Centrality [%]", n).c_str(),
                                                        bins_psi, psi_low, psi_high, bins_psi, psi_low, psi_high, m_cent_bins, m_cent_low, m_cent_high);
    }

    if (m_pass == Pass::ApplyRecentering)
    {
      m_hists3D[psi_corr_hist_name] = std::make_unique<TH3F>(psi_corr_hist_name.c_str(),
                                                             std::format("sEPD #Psi (Order {0}): |z| < 10 cm and MB; {0}#Psi^{{S}}_{{{0}}}; {0}#Psi^{{N}}_{{{0}}}; Centrality [%]", n).c_str(),
                                                             bins_psi, psi_low, psi_high, bins_psi, psi_low, psi_high, m_cent_bins, m_cent_low, m_cent_high);
    }

    if (m_pass == Pass::ApplyFlattening)
    {
      m_hists3D[psi_corr2_hist_name] = std::make_unique<TH3F>(psi_corr2_hist_name.c_str(),
                                                              std::format("sEPD #Psi (Order {0}): |z| < 10 cm and MB; {0}#Psi^{{S}}_{{{0}}}; {0}#Psi^{{N}}_{{{0}}}; Centrality [%]", n).c_str(),
                                                              bins_psi, psi_low, psi_high, bins_psi, psi_low, psi_high, m_cent_bins, m_cent_low, m_cent_high);
    }

    // South, North
    for (auto det : m_subdetectors)
    {
      std::string det_str = (det == Subdetector::S) ? "S" : "N";
      std::string det_name = (det == Subdetector::S) ? "South" : "North";

      if (m_pass == Pass::ComputeRecentering)
      {
        std::string q_hist_name = std::format("h3_sEPD_Q_{}_{}", det_str, n);
        m_hists3D[q_hist_name] = std::make_unique<TH3F>(q_hist_name.c_str(),
                                                        std::format("sEPD {} Q (Order {}): |z| < 10 cm and MB; Q_{{x}}; Q_{{y}}; Centrality [%]", det_name, n).c_str(),
                                                        bins_Q, Q_low, Q_high, bins_Q, Q_low, Q_high, m_cent_bins, m_cent_low, m_cent_high);
      }

      std::string q_avg_sq_cross_name = std::format("h_sEPD_Q_{}_xy_{}_avg", det_str, n);
      std::string q_avg_sq_cross_corr_name = std::format("h_sEPD_Q_{}_xy_{}_corr_avg", det_str, n);

      if (m_pass == Pass::ApplyRecentering)
      {
        m_profiles[q_avg_sq_cross_name] = std::make_unique<TProfile>(q_avg_sq_cross_name.c_str(),
                                                                     std::format("sEPD {0}; Centrality [%]; <Q_{{{1},x}} Q_{{{1},y}}>", det_name, n).c_str(),
                                                                     m_cent_bins, m_cent_low, m_cent_high);
      }

      if (m_pass == Pass::ApplyFlattening)
      {
        m_profiles[q_avg_sq_cross_corr_name] = std::make_unique<TProfile>(q_avg_sq_cross_corr_name.c_str(),
                                                                          std::format("sEPD {0}; Centrality [%]; <Q_{{{1},x}} Q_{{{1},y}}>", det_name, n).c_str(),
                                                                          m_cent_bins, m_cent_low, m_cent_high);
      }

      // X, Y
      for (auto comp : m_components)
      {
        std::string comp_str = (comp == QComponent::X) ? "x" : "y";
        std::string q_avg_name = std::format("h_sEPD_Q_{}_{}_{}_avg", det_str, comp_str, n);
        std::string q_avg_corr_name = std::format("h_sEPD_Q_{}_{}_{}_corr_avg", det_str, comp_str, n);
        std::string q_avg_corr2_name = std::format("h_sEPD_Q_{}_{}_{}_corr2_avg", det_str, comp_str, n);
        std::string q_avg_sq_name = std::format("h_sEPD_Q_{0}_{1}{1}_{2}_avg", det_str, comp_str, n);
        std::string q_avg_sq_corr_name = std::format("h_sEPD_Q_{0}_{1}{1}_{2}_corr_avg", det_str, comp_str, n);

        if (m_pass == Pass::ComputeRecentering)
        {
          std::string title = std::format("sEPD {}; Centrality [%]; <Q_{{{},{}}}>", det_name, n, comp_str);
          m_profiles[q_avg_name] = std::make_unique<TProfile>(q_avg_name.c_str(), title.c_str(), m_cent_bins, m_cent_low, m_cent_high);
        }

        if (m_pass == Pass::ApplyRecentering)
        {
          std::string title = std::format("sEPD {}; Centrality [%]; <Q_{{{},{}}}>", det_name, n, comp_str);
          m_profiles[q_avg_corr_name] = std::make_unique<TProfile>(q_avg_corr_name.c_str(), title.c_str(), m_cent_bins, m_cent_low, m_cent_high);

          title = std::format("sEPD {}; Centrality [%]; <Q_{{{},{}}}^{{2}}", det_name, n, comp_str);
          m_profiles[q_avg_sq_name] = std::make_unique<TProfile>(q_avg_sq_name.c_str(), title.c_str(), m_cent_bins, m_cent_low, m_cent_high);
        }

        if (m_pass == Pass::ApplyFlattening)
        {
          std::string title = std::format("sEPD {}; Centrality [%]; <Q_{{{},{}}}>", det_name, n, comp_str);
          m_profiles[q_avg_corr2_name] = std::make_unique<TProfile>(q_avg_corr2_name.c_str(), title.c_str(), m_cent_bins, m_cent_low, m_cent_high);

          title = std::format("sEPD {}; Centrality [%]; <Q_{{{},{}}}^{{2}}", det_name, n, comp_str);
          m_profiles[q_avg_sq_corr_name] = std::make_unique<TProfile>(q_avg_sq_corr_name.c_str(), title.c_str(), m_cent_bins, m_cent_low, m_cent_high);
        }
      }
    }
  }
}

void QvectorAnalysis::process_averages(double cent, QVec q_S, QVec q_N, const AverageHists& h)
{
  double psi_S = std::atan2(q_S.y, q_S.x);
  double psi_N = std::atan2(q_N.y, q_N.x);

  h.S_x_avg->Fill(cent, q_S.x);
  h.S_y_avg->Fill(cent, q_S.y);
  h.N_x_avg->Fill(cent, q_N.x);
  h.N_y_avg->Fill(cent, q_N.y);

  h.Q_S->Fill(q_S.x, q_S.y, cent);
  h.Q_N->Fill(q_N.x, q_N.y, cent);
  h.Psi->Fill(psi_S, psi_N, cent);
}

void QvectorAnalysis::process_recentering(double cent, size_t n_idx, QVec q_S, QVec q_N, const RecenterHists& h)
{
  size_t cent_bin = static_cast<size_t>(m_hists1D["h_Cent"]->FindBin(cent) - 1);

  double Q_S_x_avg = m_correction_data[cent_bin][n_idx][0].avg_Q.x;
  double Q_S_y_avg = m_correction_data[cent_bin][n_idx][0].avg_Q.y;
  double Q_N_x_avg = m_correction_data[cent_bin][n_idx][1].avg_Q.x;
  double Q_N_y_avg = m_correction_data[cent_bin][n_idx][1].avg_Q.y;

  QVec q_S_corr = {q_S.x - Q_S_x_avg, q_S.y - Q_S_y_avg};
  QVec q_N_corr = {q_N.x - Q_N_x_avg, q_N.y - Q_N_y_avg};

  double psi_S_corr = std::atan2(q_S_corr.y, q_S_corr.x);
  double psi_N_corr = std::atan2(q_N_corr.y, q_N_corr.x);

  h.S_x_corr_avg->Fill(cent, q_S_corr.x);
  h.S_y_corr_avg->Fill(cent, q_S_corr.y);
  h.N_x_corr_avg->Fill(cent, q_N_corr.x);
  h.N_y_corr_avg->Fill(cent, q_N_corr.y);

  h.S_xx_avg->Fill(cent, q_S_corr.x * q_S_corr.x);
  h.S_yy_avg->Fill(cent, q_S_corr.y * q_S_corr.y);
  h.S_xy_avg->Fill(cent, q_S_corr.x * q_S_corr.y);
  h.N_xx_avg->Fill(cent, q_N_corr.x * q_N_corr.x);
  h.N_yy_avg->Fill(cent, q_N_corr.y * q_N_corr.y);
  h.N_xy_avg->Fill(cent, q_N_corr.x * q_N_corr.y);

  h.Psi_corr->Fill(psi_S_corr, psi_N_corr, cent);
}

void QvectorAnalysis::process_flattening(double cent, size_t n_idx, QVec q_S, QVec q_N, const FlatteningHists& h)
{
  size_t cent_bin = static_cast<size_t>(m_hists1D["h_Cent"]->FindBin(cent) - 1);

  double Q_S_x_avg = m_correction_data[cent_bin][n_idx][0].avg_Q.x;
  double Q_S_y_avg = m_correction_data[cent_bin][n_idx][0].avg_Q.y;
  double Q_N_x_avg = m_correction_data[cent_bin][n_idx][1].avg_Q.x;
  double Q_N_y_avg = m_correction_data[cent_bin][n_idx][1].avg_Q.y;

  QVec q_S_corr = {q_S.x - Q_S_x_avg, q_S.y - Q_S_y_avg};
  QVec q_N_corr = {q_N.x - Q_N_x_avg, q_N.y - Q_N_y_avg};

  const auto& X_S = m_correction_data[cent_bin][n_idx][0].X_matrix;
  const auto& X_N = m_correction_data[cent_bin][n_idx][1].X_matrix;

  double Q_S_x_corr2 = X_S[0][0] * q_S_corr.x + X_S[0][1] * q_S_corr.y;
  double Q_S_y_corr2 = X_S[1][0] * q_S_corr.x + X_S[1][1] * q_S_corr.y;
  double Q_N_x_corr2 = X_N[0][0] * q_N_corr.x + X_N[0][1] * q_N_corr.y;
  double Q_N_y_corr2 = X_N[1][0] * q_N_corr.x + X_N[1][1] * q_N_corr.y;

  QVec q_S_corr2 = {Q_S_x_corr2, Q_S_y_corr2};
  QVec q_N_corr2 = {Q_N_x_corr2, Q_N_y_corr2};

  double psi_S = std::atan2(q_S_corr2.y, q_S_corr2.x);
  double psi_N = std::atan2(q_N_corr2.y, q_N_corr2.x);

  h.S_x_corr2_avg->Fill(cent, q_S_corr2.x);
  h.S_y_corr2_avg->Fill(cent, q_S_corr2.y);
  h.N_x_corr2_avg->Fill(cent, q_N_corr2.x);
  h.N_y_corr2_avg->Fill(cent, q_N_corr2.y);

  h.S_xx_corr_avg->Fill(cent, q_S_corr2.x * q_S_corr2.x);
  h.S_yy_corr_avg->Fill(cent, q_S_corr2.y * q_S_corr2.y);
  h.S_xy_corr_avg->Fill(cent, q_S_corr2.x * q_S_corr2.y);
  h.N_xx_corr_avg->Fill(cent, q_N_corr2.x * q_N_corr2.x);
  h.N_yy_corr_avg->Fill(cent, q_N_corr2.y * q_N_corr2.y);
  h.N_xy_corr_avg->Fill(cent, q_N_corr2.x * q_N_corr2.y);

  h.Psi_corr2->Fill(psi_S, psi_N, cent);
}

void QvectorAnalysis::compute_averages(size_t cent_bin, int n)
{
  size_t n_idx = harmonic_to_index(n);

  std::string S_x_avg_name = std::format("h_sEPD_Q_S_x_{}_avg", n);
  std::string S_y_avg_name = std::format("h_sEPD_Q_S_y_{}_avg", n);
  std::string N_x_avg_name = std::format("h_sEPD_Q_N_x_{}_avg", n);
  std::string N_y_avg_name = std::format("h_sEPD_Q_N_y_{}_avg", n);

  int bin = static_cast<int>(cent_bin + 1);

  double Q_S_x_avg = m_profiles[S_x_avg_name]->GetBinContent(bin);
  double Q_S_y_avg = m_profiles[S_y_avg_name]->GetBinContent(bin);
  double Q_N_x_avg = m_profiles[N_x_avg_name]->GetBinContent(bin);
  double Q_N_y_avg = m_profiles[N_y_avg_name]->GetBinContent(bin);

  m_correction_data[cent_bin][n_idx][0].avg_Q = {Q_S_x_avg, Q_S_y_avg};
  m_correction_data[cent_bin][n_idx][1].avg_Q = {Q_N_x_avg, Q_N_y_avg};

  std::cout << std::format(
      "Centrality Bin: {}, "
      "Harmonic: {}, "
      "Q_S_x_avg: {:13.10f}, "
      "Q_S_y_avg: {:13.10f}, "
      "Q_N_x_avg: {:13.10f}, "
      "Q_N_y_avg: {:13.10f}\n",
      cent_bin,
      n,
      Q_S_x_avg,
      Q_S_y_avg,
      Q_N_x_avg,
      Q_N_y_avg);
}

void QvectorAnalysis::compute_recentering(size_t cent_bin, int n)
{
  size_t n_idx = harmonic_to_index(n);

  std::string S_x_corr_avg_name = std::format("h_sEPD_Q_S_x_{}_corr_avg", n);
  std::string S_y_corr_avg_name = std::format("h_sEPD_Q_S_y_{}_corr_avg", n);
  std::string N_x_corr_avg_name = std::format("h_sEPD_Q_N_x_{}_corr_avg", n);
  std::string N_y_corr_avg_name = std::format("h_sEPD_Q_N_y_{}_corr_avg", n);

  int bin = static_cast<int>(cent_bin + 1);

  double Q_S_x_corr_avg = m_profiles[S_x_corr_avg_name]->GetBinContent(bin);
  double Q_S_y_corr_avg = m_profiles[S_y_corr_avg_name]->GetBinContent(bin);
  double Q_N_x_corr_avg = m_profiles[N_x_corr_avg_name]->GetBinContent(bin);
  double Q_N_y_corr_avg = m_profiles[N_y_corr_avg_name]->GetBinContent(bin);

  // -- Compute 2nd Order Correction --
  std::string S_xx_avg_name = std::format("h_sEPD_Q_S_xx_{}_avg", n);
  std::string S_yy_avg_name = std::format("h_sEPD_Q_S_yy_{}_avg", n);
  std::string S_xy_avg_name = std::format("h_sEPD_Q_S_xy_{}_avg", n);
  std::string N_xx_avg_name = std::format("h_sEPD_Q_N_xx_{}_avg", n);
  std::string N_yy_avg_name = std::format("h_sEPD_Q_N_yy_{}_avg", n);
  std::string N_xy_avg_name = std::format("h_sEPD_Q_N_xy_{}_avg", n);

  double Q_S_xx_avg = m_profiles[S_xx_avg_name]->GetBinContent(bin);
  double Q_S_yy_avg = m_profiles[S_yy_avg_name]->GetBinContent(bin);
  double Q_S_xy_avg = m_profiles[S_xy_avg_name]->GetBinContent(bin);
  double Q_N_xx_avg = m_profiles[N_xx_avg_name]->GetBinContent(bin);
  double Q_N_yy_avg = m_profiles[N_yy_avg_name]->GetBinContent(bin);
  double Q_N_xy_avg = m_profiles[N_xy_avg_name]->GetBinContent(bin);

  m_correction_data[cent_bin][n_idx][0].avg_Q_xx = Q_S_xx_avg;
  m_correction_data[cent_bin][n_idx][0].avg_Q_yy = Q_S_yy_avg;
  m_correction_data[cent_bin][n_idx][0].avg_Q_xy = Q_S_xy_avg;
  m_correction_data[cent_bin][n_idx][1].avg_Q_xx = Q_N_xx_avg;
  m_correction_data[cent_bin][n_idx][1].avg_Q_yy = Q_N_yy_avg;
  m_correction_data[cent_bin][n_idx][1].avg_Q_xy = Q_N_xy_avg;

  // Compute N and D terms
  double D_term_S_arg = (Q_S_xx_avg * Q_S_yy_avg) - (Q_S_xy_avg * Q_S_xy_avg);
  double D_term_N_arg = (Q_N_xx_avg * Q_N_yy_avg) - (Q_N_xy_avg * Q_N_xy_avg);

  if (D_term_S_arg <= 0 || D_term_N_arg <= 0)
  {
    throw std::runtime_error(std::format("Warning: D-term argument is non-positive ({}) in compute_recentering for n={}, cent={}.", D_term_S_arg, n, cent_bin));
  }

  double D_term_S = std::sqrt(D_term_S_arg);
  double D_term_N = std::sqrt(D_term_N_arg);

  double N_term_S = D_term_S * (Q_S_xx_avg + Q_S_yy_avg + (2 * D_term_S));
  double N_term_N = D_term_N * (Q_N_xx_avg + Q_N_yy_avg + (2 * D_term_N));

  auto& X_S_matrix = m_correction_data[cent_bin][n_idx][0].X_matrix;
  auto& X_N_matrix = m_correction_data[cent_bin][n_idx][1].X_matrix;

  X_S_matrix[0][0] = (1. / std::sqrt(N_term_S)) * (Q_S_yy_avg + D_term_S);
  X_S_matrix[0][1] = (-1. / std::sqrt(N_term_S)) * Q_S_xy_avg;
  X_S_matrix[1][0] = X_S_matrix[0][1];
  X_S_matrix[1][1] = (1. / std::sqrt(N_term_S)) * (Q_S_xx_avg + D_term_S);

  X_N_matrix[0][0] = (1. / std::sqrt(N_term_N)) * (Q_N_yy_avg + D_term_N);
  X_N_matrix[0][1] = (-1. / std::sqrt(N_term_N)) * Q_N_xy_avg;
  X_N_matrix[1][0] = X_N_matrix[0][1];
  X_N_matrix[1][1] = (1. / std::sqrt(N_term_N)) * (Q_N_xx_avg + D_term_N);

  std::cout << std::format(
      "Centrality Bin: {}, "
      "Harmonic: {}, "
      "Q_S_x_corr_avg: {:13.10f}, "
      "Q_S_y_corr_avg: {:13.10f}, "
      "Q_N_x_corr_avg: {:13.10f}, "
      "Q_N_y_corr_avg: {:13.10f}, "
      "Q_S_xx_avg / Q_S_yy_avg: {:13.10f}, "
      "Q_N_xx_avg / Q_N_yy_avg: {:13.10f}, "
      "Q_S_xy_avg: {:13.10f}, "
      "Q_N_xy_avg: {:13.10f}\n",
      cent_bin,
      n,
      Q_S_x_corr_avg,
      Q_S_y_corr_avg,
      Q_N_x_corr_avg,
      Q_N_y_corr_avg,
      Q_S_xx_avg / Q_S_yy_avg,
      Q_N_xx_avg / Q_N_yy_avg,
      Q_S_xy_avg,
      Q_N_xy_avg);
}

void QvectorAnalysis::print_flattening(size_t cent_bin, int n) const
{
  std::string S_x_corr2_avg_name = std::format("h_sEPD_Q_S_x_{}_corr2_avg", n);
  std::string S_y_corr2_avg_name = std::format("h_sEPD_Q_S_y_{}_corr2_avg", n);
  std::string N_x_corr2_avg_name = std::format("h_sEPD_Q_N_x_{}_corr2_avg", n);
  std::string N_y_corr2_avg_name = std::format("h_sEPD_Q_N_y_{}_corr2_avg", n);

  std::string S_xx_corr_avg_name = std::format("h_sEPD_Q_S_xx_{}_corr_avg", n);
  std::string S_yy_corr_avg_name = std::format("h_sEPD_Q_S_yy_{}_corr_avg", n);
  std::string S_xy_corr_avg_name = std::format("h_sEPD_Q_S_xy_{}_corr_avg", n);
  std::string N_xx_corr_avg_name = std::format("h_sEPD_Q_N_xx_{}_corr_avg", n);
  std::string N_yy_corr_avg_name = std::format("h_sEPD_Q_N_yy_{}_corr_avg", n);
  std::string N_xy_corr_avg_name = std::format("h_sEPD_Q_N_xy_{}_corr_avg", n);

  int bin = static_cast<int>(cent_bin + 1);

  double Q_S_x_corr2_avg = m_profiles.at(S_x_corr2_avg_name)->GetBinContent(bin);
  double Q_S_y_corr2_avg = m_profiles.at(S_y_corr2_avg_name)->GetBinContent(bin);
  double Q_N_x_corr2_avg = m_profiles.at(N_x_corr2_avg_name)->GetBinContent(bin);
  double Q_N_y_corr2_avg = m_profiles.at(N_y_corr2_avg_name)->GetBinContent(bin);

  double Q_S_xx_corr_avg = m_profiles.at(S_xx_corr_avg_name)->GetBinContent(bin);
  double Q_S_yy_corr_avg = m_profiles.at(S_yy_corr_avg_name)->GetBinContent(bin);
  double Q_S_xy_corr_avg = m_profiles.at(S_xy_corr_avg_name)->GetBinContent(bin);
  double Q_N_xx_corr_avg = m_profiles.at(N_xx_corr_avg_name)->GetBinContent(bin);
  double Q_N_yy_corr_avg = m_profiles.at(N_yy_corr_avg_name)->GetBinContent(bin);
  double Q_N_xy_corr_avg = m_profiles.at(N_xy_corr_avg_name)->GetBinContent(bin);

  std::cout << std::format(
      "Centrality Bin: {}, "
      "Harmonic: {}, "
      "Q_S_x_corr2_avg: {:13.10f}, "
      "Q_S_y_corr2_avg: {:13.10f}, "
      "Q_N_x_corr2_avg: {:13.10f}, "
      "Q_N_y_corr2_avg: {:13.10f}, "
      "Q_S_xx_corr_avg / Q_S_yy_corr_avg: {:13.10f}, "
      "Q_N_xx_corr_avg / Q_N_yy_corr_avg: {:13.10f}, "
      "Q_S_xy_corr_avg: {:13.10f}, "
      "Q_N_xy_corr_avg: {:13.10f}\n",
      cent_bin,
      n,
      Q_S_x_corr2_avg,
      Q_S_y_corr2_avg,
      Q_N_x_corr2_avg,
      Q_N_y_corr2_avg,
      Q_S_xx_corr_avg / Q_S_yy_corr_avg,
      Q_N_xx_corr_avg / Q_N_yy_corr_avg,
      Q_S_xy_corr_avg,
      Q_N_xy_corr_avg);
}

std::map<int, QvectorAnalysis::AverageHists> QvectorAnalysis::prepare_average_hists()
{
  std::map<int, AverageHists> hists_cache;
  for (int n : m_harmonics)
  {
    std::string S_x_avg_name = std::format("h_sEPD_Q_S_x_{}_avg", n);
    std::string S_y_avg_name = std::format("h_sEPD_Q_S_y_{}_avg", n);
    std::string N_x_avg_name = std::format("h_sEPD_Q_N_x_{}_avg", n);
    std::string N_y_avg_name = std::format("h_sEPD_Q_N_y_{}_avg", n);

    std::string hist_Q_S_name = std::format("h3_sEPD_Q_S_{}", n);
    std::string hist_Q_N_name = std::format("h3_sEPD_Q_N_{}", n);
    std::string psi_Q_name = std::format("h3_sEPD_Psi_{}", n);

    AverageHists h;

    h.S_x_avg = m_profiles.at(S_x_avg_name).get();
    h.S_y_avg = m_profiles.at(S_y_avg_name).get();
    h.N_x_avg = m_profiles.at(N_x_avg_name).get();
    h.N_y_avg = m_profiles.at(N_y_avg_name).get();

    h.Q_S = m_hists3D.at(hist_Q_S_name).get();
    h.Q_N = m_hists3D.at(hist_Q_N_name).get();

    h.Psi = m_hists3D.at(psi_Q_name).get();

    hists_cache[n] = h;
  }

  return hists_cache;
}

bool QvectorAnalysis::filter_sEPD(int rbin)
{
  return (m_QVecAna == QVecAna::DEFAULT) ||
         (m_QVecAna == QVecAna::HALF && rbin <= 7) ||
         (m_QVecAna == QVecAna::HALF1 && rbin <= 7 && rbin >= 1);
}

bool QvectorAnalysis::process_sEPD()
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
    if (m_doBadChannelCheck && m_bad_channels.contains(channel))
    {
      continue;
    }

    unsigned int key = TowerInfoDefs::encode_epd(static_cast<unsigned int>(channel));
    unsigned int arm = TowerInfoDefs::get_epd_arm(key);
    int rbin = static_cast<int>(TowerInfoDefs::get_epd_rbin(key));

    if (!filter_sEPD(rbin))
    {
      m_rbins_skipped.insert(rbin);
      continue;
    }

    // arm = 0: South
    // arm = 1: North
    double& sepd_total_charge = (arm == 0) ? sepd_total_charge_south : sepd_total_charge_north;

    // Compute total charge for the respective sEPD arm
    sepd_total_charge += charge;

    // Compute Raw Q vectors for each harmonic and respective arm
    for (int n : m_harmonics)
    {
      size_t n_idx = harmonic_to_index(n);
      m_event_data.q_vectors[n_idx][arm].x += charge * std::cos(n * phi);
      m_event_data.q_vectors[n_idx][arm].y += charge * std::sin(n * phi);
    }
  }

  // Skip Events with Zero sEPD Total Charge in either arm
  if (sepd_total_charge_south == 0 || sepd_total_charge_north == 0)
  {
    return false;
  }

  // Normalize the Q-vectors by total charge
  for (int n : m_harmonics)
  {
    size_t n_idx = harmonic_to_index(n);
    for (auto det : m_subdetectors)
    {
      size_t det_idx = (det == Subdetector::S) ? 0 : 1;
      double sepd_total_charge = (det_idx == 0) ? sepd_total_charge_south : sepd_total_charge_north;
      m_event_data.q_vectors[n_idx][det_idx].x /= sepd_total_charge;
      m_event_data.q_vectors[n_idx][det_idx].y /= sepd_total_charge;
    }
  }

  return true;
}

std::map<int, QvectorAnalysis::RecenterHists> QvectorAnalysis::prepare_recenter_hists()
{
  std::map<int, RecenterHists> hists_cache;
  for (int n : m_harmonics)
  {
    std::string S_x_corr_avg_name = std::format("h_sEPD_Q_S_x_{}_corr_avg", n);
    std::string S_y_corr_avg_name = std::format("h_sEPD_Q_S_y_{}_corr_avg", n);
    std::string N_x_corr_avg_name = std::format("h_sEPD_Q_N_x_{}_corr_avg", n);
    std::string N_y_corr_avg_name = std::format("h_sEPD_Q_N_y_{}_corr_avg", n);

    std::string S_xx_avg_name = std::format("h_sEPD_Q_S_xx_{}_avg", n);
    std::string S_yy_avg_name = std::format("h_sEPD_Q_S_yy_{}_avg", n);
    std::string S_xy_avg_name = std::format("h_sEPD_Q_S_xy_{}_avg", n);
    std::string N_xx_avg_name = std::format("h_sEPD_Q_N_xx_{}_avg", n);
    std::string N_yy_avg_name = std::format("h_sEPD_Q_N_yy_{}_avg", n);
    std::string N_xy_avg_name = std::format("h_sEPD_Q_N_xy_{}_avg", n);

    std::string psi_Q_corr_name = std::format("h3_sEPD_Psi_{}_corr", n);

    RecenterHists h;

    h.S_x_corr_avg = m_profiles.at(S_x_corr_avg_name).get();
    h.S_y_corr_avg = m_profiles.at(S_y_corr_avg_name).get();
    h.N_x_corr_avg = m_profiles.at(N_x_corr_avg_name).get();
    h.N_y_corr_avg = m_profiles.at(N_y_corr_avg_name).get();

    h.S_xx_avg = m_profiles.at(S_xx_avg_name).get();
    h.S_yy_avg = m_profiles.at(S_yy_avg_name).get();
    h.S_xy_avg = m_profiles.at(S_xy_avg_name).get();
    h.N_xx_avg = m_profiles.at(N_xx_avg_name).get();
    h.N_yy_avg = m_profiles.at(N_yy_avg_name).get();
    h.N_xy_avg = m_profiles.at(N_xy_avg_name).get();

    h.Psi_corr = m_hists3D.at(psi_Q_corr_name).get();

    hists_cache[n] = h;
  }

  return hists_cache;
}

std::map<int, QvectorAnalysis::FlatteningHists> QvectorAnalysis::prepare_flattening_hists()
{
  std::map<int, FlatteningHists> hists_cache;
  for (int n : m_harmonics)
  {
    std::string S_x_corr2_avg_name = std::format("h_sEPD_Q_S_x_{}_corr2_avg", n);
    std::string S_y_corr2_avg_name = std::format("h_sEPD_Q_S_y_{}_corr2_avg", n);
    std::string N_x_corr2_avg_name = std::format("h_sEPD_Q_N_x_{}_corr2_avg", n);
    std::string N_y_corr2_avg_name = std::format("h_sEPD_Q_N_y_{}_corr2_avg", n);

    std::string S_xx_corr_avg_name = std::format("h_sEPD_Q_S_xx_{}_corr_avg", n);
    std::string S_yy_corr_avg_name = std::format("h_sEPD_Q_S_yy_{}_corr_avg", n);
    std::string S_xy_corr_avg_name = std::format("h_sEPD_Q_S_xy_{}_corr_avg", n);
    std::string N_xx_corr_avg_name = std::format("h_sEPD_Q_N_xx_{}_corr_avg", n);
    std::string N_yy_corr_avg_name = std::format("h_sEPD_Q_N_yy_{}_corr_avg", n);
    std::string N_xy_corr_avg_name = std::format("h_sEPD_Q_N_xy_{}_corr_avg", n);

    std::string psi_Q_corr2_name = std::format("h3_sEPD_Psi_{}_corr2", n);

    FlatteningHists h;

    h.S_x_corr2_avg = m_profiles.at(S_x_corr2_avg_name).get();
    h.S_y_corr2_avg = m_profiles.at(S_y_corr2_avg_name).get();
    h.N_x_corr2_avg = m_profiles.at(N_x_corr2_avg_name).get();
    h.N_y_corr2_avg = m_profiles.at(N_y_corr2_avg_name).get();

    h.S_xx_corr_avg = m_profiles.at(S_xx_corr_avg_name).get();
    h.S_yy_corr_avg = m_profiles.at(S_yy_corr_avg_name).get();
    h.S_xy_corr_avg = m_profiles.at(S_xy_corr_avg_name).get();

    h.N_xx_corr_avg = m_profiles.at(N_xx_corr_avg_name).get();
    h.N_yy_corr_avg = m_profiles.at(N_yy_corr_avg_name).get();
    h.N_xy_corr_avg = m_profiles.at(N_xy_corr_avg_name).get();

    h.Psi_corr2 = m_hists3D.at(psi_Q_corr2_name).get();

    hists_cache[n] = h;
  }

  return hists_cache;
}

void QvectorAnalysis::run_event_loop()
{
  std::cout << std::format("Pass: {}\n", static_cast<uint8_t>(m_pass));

  long long n_entries = m_chain->GetEntries();
  if (m_events_to_process)
  {
    n_entries = std::min(m_events_to_process, n_entries);
  }

  std::map<int, AverageHists> average_hists;
  std::map<int, RecenterHists> recenter_hists;
  std::map<int, FlatteningHists> flattening_hists;

  if (m_pass == Pass::ComputeRecentering)
  {
    average_hists = prepare_average_hists();
  }
  else if (m_pass == Pass::ApplyRecentering)
  {
    recenter_hists = prepare_recenter_hists();
  }
  else if (m_pass == Pass::ApplyFlattening)
  {
    flattening_hists = prepare_flattening_hists();
  }

  std::map<std::string, int> ctr;
  // Event Loop
  for (long long i = 0; i < n_entries; ++i)
  {
    // Load Event Data from TChain
    m_chain->GetEntry(i);

    if (i % 10000 == 0)
    {
      std::cout << std::format("Processing {}/{}: {:.2f} %", i, n_entries, static_cast<double>(i) * 100. / static_cast<double>(n_entries)) << std::endl;
    }

    double cent = m_event_data.event_centrality;

    // Identify Centrality Bin
    size_t cent_bin = static_cast<size_t>(m_hists1D["h_Cent"]->FindBin(cent) - 1);

    // ensure centrality is valid
    if (cent_bin >= m_cent_bins)
    {
      std::cout << std::format("Weird Centrality: {}, Skipping Event: {}\n", cent, m_event_data.event_id);
      ++ctr["invalid_cent_bin"];
      continue;
    }

    bool isGood = process_sEPD();

    // Skip Events with Zero sEPD Total Charge in either arm
    if (!isGood)
    {
      ++ctr["zero_sepd_total_charge"];
      continue;
    }

    m_hists1D["h_Cent"]->Fill(cent);

    for (int n : m_harmonics)
    {
      size_t n_idx = harmonic_to_index(n);
      const auto& q_S = m_event_data.q_vectors[n_idx][0];  // 0 for South
      const auto& q_N = m_event_data.q_vectors[n_idx][1];  // 1 for North

      // --- First Pass: Derive 1st Order ---
      if (m_pass == Pass::ComputeRecentering)
      {
        process_averages(cent, q_S, q_N, average_hists.at(n));
      }

      // --- Second Pass: Apply 1st Order, Derive 2nd Order ---
      else if (m_pass == Pass::ApplyRecentering)
      {
        process_recentering(cent, n_idx, q_S, q_N, recenter_hists.at(n));
      }

      // --- Third Pass: Apply 2nd Order, Validate ---
      else if (m_pass == Pass::ApplyFlattening)
      {
        process_flattening(cent, n_idx, q_S, q_N, flattening_hists.at(n));
      }
    }
  }

  std::cout << std::format("QVecAna: {}, rbins skipped: {}\n", static_cast<int>(m_QVecAna), m_rbins_skipped.size());

  std::cout << "Skipped Event Types\n";
  for (const auto& [name, events] : ctr)
  {
    std::cout << std::format("{}: {}, {:.2f} %\n", name, events, events * 100. / static_cast<double>(n_entries));
  }

  // ---------------

  std::cout << std::format("{:#<20}\n", "");
  for (size_t cent_bin = 0; cent_bin < m_cent_bins; ++cent_bin)
  {
    for (int n : m_harmonics)
    {
      if (m_pass == Pass::ComputeRecentering)
      {
        compute_averages(cent_bin, n);
      }

      else if (m_pass == Pass::ApplyRecentering)
      {
        compute_recentering(cent_bin, n);
      }

      else if (m_pass == Pass::ApplyFlattening)
      {
        print_flattening(cent_bin, n);
      }
    }
  }

  std::cout << "Event loop finished." << std::endl;
}

void QvectorAnalysis::load_correction_data()
{
  TH1::AddDirectory(kFALSE);

  auto file = std::unique_ptr<TFile>(TFile::Open(m_input_Q_calib.c_str()));

  // Check if the file was opened successfully.
  if (!file || file->IsZombie())
  {
    throw std::runtime_error(std::format("Could not open file '{}'", m_input_Q_calib));
  }

  for (size_t n_idx = 0; n_idx < m_harmonics.size(); ++n_idx)
  {
    int n = m_harmonics[n_idx];

    std::string S_x_avg_name = std::format("h_sEPD_Q_S_x_{}_avg", n);
    std::string S_y_avg_name = std::format("h_sEPD_Q_S_y_{}_avg", n);
    std::string N_x_avg_name = std::format("h_sEPD_Q_N_x_{}_avg", n);
    std::string N_y_avg_name = std::format("h_sEPD_Q_N_y_{}_avg", n);

    std::string psi_hist_name = std::format("h3_sEPD_Psi_{}", n);

    auto* h_sEPD_Q_S_x_avg = dynamic_cast<TProfile*>(file->Get(S_x_avg_name.c_str()));
    auto* h_sEPD_Q_S_y_avg = dynamic_cast<TProfile*>(file->Get(S_y_avg_name.c_str()));
    auto* h_sEPD_Q_N_x_avg = dynamic_cast<TProfile*>(file->Get(N_x_avg_name.c_str()));
    auto* h_sEPD_Q_N_y_avg = dynamic_cast<TProfile*>(file->Get(N_y_avg_name.c_str()));

    auto* h3_sEPD_Psi = dynamic_cast<TH3*>(file->Get(psi_hist_name.c_str()));

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
    if (!h3_sEPD_Psi)
    {
      throw std::runtime_error(std::format("Could not find histogram '{}' in file '{}'", psi_hist_name, m_input_Q_calib));
    }

    m_profiles[S_x_avg_name] = std::unique_ptr<TProfile>(static_cast<TProfile*>(h_sEPD_Q_S_x_avg->Clone()));
    m_profiles[S_y_avg_name] = std::unique_ptr<TProfile>(static_cast<TProfile*>(h_sEPD_Q_S_y_avg->Clone()));
    m_profiles[N_x_avg_name] = std::unique_ptr<TProfile>(static_cast<TProfile*>(h_sEPD_Q_N_x_avg->Clone()));
    m_profiles[N_y_avg_name] = std::unique_ptr<TProfile>(static_cast<TProfile*>(h_sEPD_Q_N_y_avg->Clone()));

    m_hists3D[psi_hist_name] = std::unique_ptr<TH3>(static_cast<TH3*>(h3_sEPD_Psi->Clone()));

    std::string S_xx_avg_name = std::format("h_sEPD_Q_S_xx_{}_avg", n);
    std::string S_yy_avg_name = std::format("h_sEPD_Q_S_yy_{}_avg", n);
    std::string S_xy_avg_name = std::format("h_sEPD_Q_S_xy_{}_avg", n);
    std::string N_xx_avg_name = std::format("h_sEPD_Q_N_xx_{}_avg", n);
    std::string N_yy_avg_name = std::format("h_sEPD_Q_N_yy_{}_avg", n);
    std::string N_xy_avg_name = std::format("h_sEPD_Q_N_xy_{}_avg", n);

    std::string psi_corr_hist_name = std::format("h3_sEPD_Psi_{}_corr", n);

    auto* h_sEPD_Q_S_xx_avg = dynamic_cast<TProfile*>(file->Get(S_xx_avg_name.c_str()));
    auto* h_sEPD_Q_S_yy_avg = dynamic_cast<TProfile*>(file->Get(S_yy_avg_name.c_str()));
    auto* h_sEPD_Q_S_xy_avg = dynamic_cast<TProfile*>(file->Get(S_xy_avg_name.c_str()));
    auto* h_sEPD_Q_N_xx_avg = dynamic_cast<TProfile*>(file->Get(N_xx_avg_name.c_str()));
    auto* h_sEPD_Q_N_yy_avg = dynamic_cast<TProfile*>(file->Get(N_yy_avg_name.c_str()));
    auto* h_sEPD_Q_N_xy_avg = dynamic_cast<TProfile*>(file->Get(N_xy_avg_name.c_str()));

    auto* h3_sEPD_Psi_corr = dynamic_cast<TH3*>(file->Get(psi_corr_hist_name.c_str()));

    if(m_pass == Pass::ApplyFlattening)
    {
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

      m_profiles[S_xx_avg_name] = std::unique_ptr<TProfile>(static_cast<TProfile*>(h_sEPD_Q_S_xx_avg->Clone()));
      m_profiles[S_yy_avg_name] = std::unique_ptr<TProfile>(static_cast<TProfile*>(h_sEPD_Q_S_yy_avg->Clone()));
      m_profiles[S_xy_avg_name] = std::unique_ptr<TProfile>(static_cast<TProfile*>(h_sEPD_Q_S_xy_avg->Clone()));
      m_profiles[N_xx_avg_name] = std::unique_ptr<TProfile>(static_cast<TProfile*>(h_sEPD_Q_N_xx_avg->Clone()));
      m_profiles[N_yy_avg_name] = std::unique_ptr<TProfile>(static_cast<TProfile*>(h_sEPD_Q_N_yy_avg->Clone()));
      m_profiles[N_xy_avg_name] = std::unique_ptr<TProfile>(static_cast<TProfile*>(h_sEPD_Q_N_xy_avg->Clone()));

      m_hists3D[psi_corr_hist_name] = std::unique_ptr<TH3>(static_cast<TH3*>(h3_sEPD_Psi_corr->Clone()));
    }

    size_t south_idx = static_cast<size_t>(Subdetector::S);
    size_t north_idx = static_cast<size_t>(Subdetector::N);

    for (size_t cent_bin = 0; cent_bin < m_cent_bins; ++cent_bin)
    {
      int bin = static_cast<int>(cent_bin) + 1;

      double Q_S_x_avg = h_sEPD_Q_S_x_avg->GetBinContent(bin);
      double Q_S_y_avg = h_sEPD_Q_S_y_avg->GetBinContent(bin);
      double Q_N_x_avg = h_sEPD_Q_N_x_avg->GetBinContent(bin);
      double Q_N_y_avg = h_sEPD_Q_N_y_avg->GetBinContent(bin);

      // Recentering Params
      m_correction_data[cent_bin][n_idx][south_idx].avg_Q = {Q_S_x_avg, Q_S_y_avg};
      m_correction_data[cent_bin][n_idx][north_idx].avg_Q = {Q_N_x_avg, Q_N_y_avg};

      if (m_pass == Pass::ApplyFlattening)
      {
        double Q_S_xx_avg = h_sEPD_Q_S_xx_avg->GetBinContent(bin);
        double Q_S_yy_avg = h_sEPD_Q_S_yy_avg->GetBinContent(bin);
        double Q_S_xy_avg = h_sEPD_Q_S_xy_avg->GetBinContent(bin);
        double Q_N_xx_avg = h_sEPD_Q_N_xx_avg->GetBinContent(bin);
        double Q_N_yy_avg = h_sEPD_Q_N_yy_avg->GetBinContent(bin);
        double Q_N_xy_avg = h_sEPD_Q_N_xy_avg->GetBinContent(bin);

        // Flattening Params
        m_correction_data[cent_bin][n_idx][south_idx].avg_Q_xx = Q_S_xx_avg;
        m_correction_data[cent_bin][n_idx][south_idx].avg_Q_yy = Q_S_yy_avg;
        m_correction_data[cent_bin][n_idx][south_idx].avg_Q_xy = Q_S_xy_avg;

        m_correction_data[cent_bin][n_idx][north_idx].avg_Q_xx = Q_N_xx_avg;
        m_correction_data[cent_bin][n_idx][north_idx].avg_Q_yy = Q_N_yy_avg;
        m_correction_data[cent_bin][n_idx][north_idx].avg_Q_xy = Q_N_xy_avg;

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

        auto& X_S_matrix = m_correction_data[cent_bin][n_idx][south_idx].X_matrix;
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

        auto& X_N_matrix = m_correction_data[cent_bin][n_idx][north_idx].X_matrix;
        X_N_matrix[0][0] = (1. / std::sqrt(N_term_N)) * (Q_N_yy_avg + D_term_N);
        X_N_matrix[0][1] = (-1. / std::sqrt(N_term_N)) * Q_N_xy_avg;
        X_N_matrix[1][0] = X_N_matrix[0][1];
        X_N_matrix[1][1] = (1. / std::sqrt(N_term_N)) * (Q_N_xx_avg + D_term_N);
      }
    }
  }
}

void QvectorAnalysis::process_events()
{
  if (m_pass == Pass::ApplyRecentering || m_pass == Pass::ApplyFlattening)
  {
    load_correction_data();
  }

  run_event_loop();
}

void QvectorAnalysis::save_results() const
{
  std::filesystem::create_directories(m_output_dir);

  std::filesystem::path input_path(m_input_file);
  std::string output_stem = input_path.stem().string();
  std::string output_filename = std::format("{}/Q-vec-corr_Pass-{}_{}.root", m_output_dir, static_cast<int>(m_pass), output_stem);

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
  output_file->Close();

  std::cout << std::format("Results saved to: {}", output_filename) << std::endl;
}

// ====================================================================
// Main function remains clean and simple
// ====================================================================
int main(int argc, const char* const argv[])
{
  gROOT->SetBatch(true);

  if (argc < 4 || argc > 8)
  {
    std::cout << "Usage: " << argv[0] << " <input_file> <input_hist_file> <input_Q_calib> [QVecAna] [pass] [events] [output_directory]" << std::endl;
    return 1;
  }

  const std::string input_file = argv[1];
  const std::string input_hist = argv[2];
  const std::string input_Q_calib = argv[3];
  const std::string q_vec_ana_str = (argc >= 5) ? argv[4] : "DEFAULT"; // Default to the first pass
  const std::string pass_str = (argc >= 6) ? argv[5] : "ComputeRecentering"; // Default to the first pass
  long long events = (argc >= 7) ? std::atoll(argv[6]) : 0;
  std::string output_dir = (argc >= 8) ? argv[7] : ".";

  const std::map<std::string, QvectorAnalysis::QVecAna> q_vec_ana_map = {
      {"DEFAULT", QvectorAnalysis::QVecAna::DEFAULT},
      {"HALF", QvectorAnalysis::QVecAna::HALF},
      {"HALF1", QvectorAnalysis::QVecAna::HALF1}
  };

  QvectorAnalysis::QVecAna q_vec_ana = QvectorAnalysis::QVecAna::DEFAULT;
  if (q_vec_ana_map.contains(q_vec_ana_str))
  {
    q_vec_ana = q_vec_ana_map.at(q_vec_ana_str);
  }
  else
  {
    std::cout << std::format("Error: Invalid q_vec_ana specified: {}\n", q_vec_ana_str);
    std::cout << "Available q_vec_anaes are: DEFAULT, HALF, HALF1" << std::endl;
    return 1;
  }

  const std::map<std::string, QvectorAnalysis::Pass> pass_map = {
      {"ComputeRecentering", QvectorAnalysis::Pass::ComputeRecentering},
      {"ApplyRecentering", QvectorAnalysis::Pass::ApplyRecentering},
      {"ApplyFlattening", QvectorAnalysis::Pass::ApplyFlattening}
  };

  QvectorAnalysis::Pass pass = QvectorAnalysis::Pass::ComputeRecentering;
  if (pass_map.contains(pass_str))
  {
    pass = pass_map.at(pass_str);
  }
  else
  {
    std::cout << std::format("Error: Invalid pass specified: {}\n", pass_str);
    std::cout << "Available passes are: ComputeRecentering, ApplyRecentering, ApplyFlattening" << std::endl;
    return 1;
  }

  try
  {
    QvectorAnalysis analysis(input_file, input_hist, input_Q_calib, static_cast<int>(q_vec_ana), static_cast<int>(pass), events, output_dir);
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
