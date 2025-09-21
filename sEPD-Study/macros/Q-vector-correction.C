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
#include <vector>

// ====================================================================
// ROOT Includes
// ====================================================================
#include <TChain.h>
#include <TFile.h>
#include <TH1.h>
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
  QvectorAnalysis(std::string input_list, long long events, int verbosity, std::string output_dir)
    : m_chain(std::make_unique<TChain>("T"))
    , m_input_list(std::move(input_list))
    , m_events_to_process(events)
    , m_verbosity(verbosity)
    , m_output_dir(std::move(output_dir))
  {
  }

  void run()
  {
    setup_chain();
    init_hists();
    run_event_loop(Pass::CalculateAverages);
    run_event_loop(Pass::ApplyRecentering);
    run_event_loop(Pass::ApplyFlattening);
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
    std::vector<std::vector<double>> X_matrix{2, std::vector<double>(2, 0.0)};
  };

  // Holds all correction data
  // key: [Cent][Harmonic][Subdetector]
  std::map<int, std::map<int, std::map<Subdetector, CorrectionData>>> m_correction_data;

  // Store harmonic orders and subdetectors for easy iteration
  const std::vector<int> m_harmonics = {2, 3, 4};
  const std::vector<Subdetector> m_subdetectors = {Subdetector::S, Subdetector::N};
  const std::vector<QComponent> m_components = {QComponent::X, QComponent::Y};

  enum class Pass
  {
    CalculateAverages,
    ApplyRecentering,
    ApplyFlattening
  };

  struct EventData
  {
    int event_id{0};
    double event_zvertex{0.0};
    double event_centrality{0.0};

    std::map<std::string, double> q_vals;
    std::vector<double>* sepd_charge{nullptr};
    std::vector<double>* sepd_phi{nullptr};
    std::vector<double>* sepd_eta{nullptr};
    // std::vector<double>* mbd_charge{nullptr};
    // std::vector<double>* mbd_phi{nullptr};
    // std::vector<double>* mbd_eta{nullptr};
  };

  // --- Member Variables ---
  EventData m_event_data;
  std::unique_ptr<TChain> m_chain;

  // Configuration stored as members
  std::string m_input_list;
  long long m_events_to_process;
  int m_verbosity;
  std::string m_output_dir;
  int m_cent_bins{10};

  // Hists
  std::map<std::string, std::unique_ptr<TH1>> m_hists;

  // --- Private Helper Methods ---
  void setup_chain();
  void init_hists();
  void run_event_loop(Pass pass);
  void save_results() const;
};

// ====================================================================
// Implementation of the Class Methods
// ====================================================================
void QvectorAnalysis::setup_chain()
{
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
  // m_chain->SetBranchStatus("sepd_charge", true);
  // m_chain->SetBranchStatus("sepd_phi", true);
  // m_chain->SetBranchStatus("sepd_eta", true);
  // m_chain->SetBranchStatus("mbd_charge", true);
  // m_chain->SetBranchStatus("mbd_phi", true);
  // m_chain->SetBranchStatus("mbd_eta", true);

  m_chain->SetBranchAddress("event_id", &m_event_data.event_id);
  m_chain->SetBranchAddress("event_centrality", &m_event_data.event_centrality);
  // m_chain->SetBranchAddress("sepd_charge", &m_event_data.sepd_charge);
  // m_chain->SetBranchAddress("sepd_phi", &m_event_data.sepd_phi);
  // m_chain->SetBranchAddress("sepd_eta", &m_event_data.sepd_eta);
  // m_chain->SetBranchAddress("mbd_charge", &m_event_data.mbd_charge);
  // m_chain->SetBranchAddress("mbd_phi", &m_event_data.mbd_phi);
  // m_chain->SetBranchAddress("mbd_eta", &m_event_data.mbd_eta);

  for (int n : m_harmonics)
  {
    for (auto det : m_subdetectors)
    {
      std::string det_str = (det == Subdetector::S) ? "S" : "N";

      for (auto comp : m_components)
      {
        std::string comp_str = (comp == QComponent::X) ? "x" : "y";
        std::string branch_name = std::format("sEPD_Q_{}_{}_{}", det_str, comp_str, n);

        m_chain->SetBranchStatus(branch_name.c_str(), true);
        int status = m_chain->SetBranchAddress(branch_name.c_str(), &m_event_data.q_vals[branch_name]);
        if (status != TTree::kMatch)
        {
          throw std::runtime_error(std::format("Failed to find branch '{}'.", branch_name));
        }
      }
    }
  }
}

void QvectorAnalysis::init_hists()
{
  unsigned int bins_Q = 100;
  double Q_low = -1;
  double Q_high = 1;

  double cent_low = 0;
  double cent_high = 100;

  unsigned int bins_psi = 126;
  double psi_low = -M_PI;
  double psi_high = M_PI;

  m_hists["h_Dummy_Cent"] = std::make_unique<TH1F>("h_Dummy_Cent", "", m_cent_bins, cent_low, cent_high);

  // n = 2, 3, 4, etc.
  for (int n : m_harmonics)
  {
    std::string psi_hist_name = std::format("h3_sEPD_Psi_{}", n);
    std::string psi_corr_hist_name = std::format("h3_sEPD_Psi_{}_corr", n);
    std::string psi_corr2_hist_name = std::format("h3_sEPD_Psi_{}_corr2", n);

    m_hists[psi_hist_name] = std::make_unique<TH3F>(psi_hist_name.c_str(),
                                                    std::format("sEPD #Psi (Order {0}): |z| < 10 cm and MB; 2#Psi^{{S}}_{0}; 2#Psi^{{N}}_{0}; Centrality [%]", n).c_str(),
                                                    bins_psi, psi_low, psi_high, bins_psi, psi_low, psi_high, m_cent_bins, cent_low, cent_high);
    m_hists[psi_corr_hist_name] = std::make_unique<TH3F>(psi_corr_hist_name.c_str(),
                                                         std::format("sEPD #Psi (Order {0}): |z| < 10 cm and MB; 2#Psi^{{S}}_{0}; 2#Psi^{{N}}_{0}; Centrality [%]", n).c_str(),
                                                         bins_psi, psi_low, psi_high, bins_psi, psi_low, psi_high, m_cent_bins, cent_low, cent_high);
    m_hists[psi_corr2_hist_name] = std::make_unique<TH3F>(psi_corr2_hist_name.c_str(),
                                                          std::format("sEPD #Psi (Order {0}): |z| < 10 cm and MB; 2#Psi^{{S}}_{0}; 2#Psi^{{N}}_{0}; Centrality [%]", n).c_str(),
                                                          bins_psi, psi_low, psi_high, bins_psi, psi_low, psi_high, m_cent_bins, cent_low, cent_high);

    // South, North
    for (auto det : m_subdetectors)
    {
      std::string det_str = (det == Subdetector::S) ? "S" : "N";
      std::string det_name = (det == Subdetector::S) ? "South" : "North";

      std::string q_hist_name = std::format("h3_sEPD_Q_{}_{}", det_str, n);
      m_hists[q_hist_name] = std::make_unique<TH3F>(q_hist_name.c_str(),
                                                    std::format("sEPD {} Q (Order {}): |z| < 10 cm and MB; Q_{{x}}; Q_{{y}}; Centrality [%]", det_name, n).c_str(),
                                                    bins_Q, Q_low, Q_high, bins_Q, Q_low, Q_high, m_cent_bins, cent_low, cent_high);

      std::string q_avg_sq_cross_name = std::format("h_sEPD_Q_{}_xy_{}_avg", det_str, n);
      std::string q_avg_sq_cross_corr_name = std::format("h_sEPD_Q_{}_xy_{}_corr_avg", det_str, n);

      m_hists[q_avg_sq_cross_name] = std::make_unique<TProfile>(q_avg_sq_cross_name.c_str(),
                                                                std::format("sEPD {0}; Centrality [%]; <Q_{{{1},x}} Q_{{{1},y}}>", det_name, n).c_str(),
                                                                m_cent_bins, cent_low, cent_high);

      m_hists[q_avg_sq_cross_corr_name] = std::make_unique<TProfile>(q_avg_sq_cross_corr_name.c_str(),
                                                                     std::format("sEPD {0}; Centrality [%]; <Q_{{{1},x}} Q_{{{1},y}}>", det_name, n).c_str(),
                                                                     m_cent_bins, cent_low, cent_high);

      // X, Y
      for (auto comp : m_components)
      {
        std::string comp_str = (comp == QComponent::X) ? "x" : "y";
        std::string q_avg_name = std::format("h_sEPD_Q_{}_{}_{}_avg", det_str, comp_str, n);
        std::string q_avg_corr_name = std::format("h_sEPD_Q_{}_{}_{}_corr_avg", det_str, comp_str, n);
        std::string q_avg_sq_name = std::format("h_sEPD_Q_{0}_{1}{1}_{2}_avg", det_str, comp_str, n);
        std::string q_avg_sq_corr_name = std::format("h_sEPD_Q_{0}_{1}{1}_{2}_corr_avg", det_str, comp_str, n);

        m_hists[q_avg_name] = std::make_unique<TProfile>(q_avg_name.c_str(),
                                                         std::format("sEPD {}; Centrality [%]; <Q_{{{},{}}}>", det_name, n, comp_str).c_str(),
                                                         m_cent_bins, cent_low, cent_high);

        m_hists[q_avg_corr_name] = std::make_unique<TProfile>(q_avg_corr_name.c_str(),
                                                              std::format("sEPD {}; Centrality [%]; <Q_{{{},{}}}>", det_name, n, comp_str).c_str(),
                                                              m_cent_bins, cent_low, cent_high);

        m_hists[q_avg_sq_name] = std::make_unique<TProfile>(q_avg_sq_name.c_str(),
                                                            std::format("sEPD {}; Centrality [%]; <Q_{{{},{}}}^{{2}}", det_name, n, comp_str).c_str(),
                                                            m_cent_bins, cent_low, cent_high);

        m_hists[q_avg_sq_corr_name] = std::make_unique<TProfile>(q_avg_sq_corr_name.c_str(),
                                                                 std::format("sEPD {}; Centrality [%]; <Q_{{{},{}}}^{{2}}", det_name, n, comp_str).c_str(),
                                                                 m_cent_bins, cent_low, cent_high);
      }
    }
  }
}

void QvectorAnalysis::run_event_loop(Pass pass)
{
  long long n_entries = m_chain->GetEntries();
  if (m_events_to_process)
  {
    n_entries = std::min(m_events_to_process, n_entries);
  }

  std::map<std::string, int> ctr;
  ctr["prints_sepd_const"] = 5;

  // Event Loop
  for (long long i = 0; i < n_entries; ++i)
  {
    // Load Event Data from TChain
    m_chain->GetEntry(i);
    if (i % 1000 == 0)
    {
      std::cout << std::format("Processing {}/{}: {:.2f} %", i, n_entries, static_cast<double>(i) * 100. / static_cast<double>(n_entries)) << std::endl;
    }

    double cent = m_event_data.event_centrality;

    // Identify Centrality Bin
    int cent_bin = m_hists["h_Dummy_Cent"]->FindBin(cent) - 1;
    if (pass == Pass::CalculateAverages)
    {
      m_hists["h_Dummy_Cent"]->Fill(cent);
    }

    // ensure centrality is valid
    if (cent_bin >= m_cent_bins)
    {
      std::cout << std::format("Weird Centrality: {}, Skipping Event: {}\n", cent, m_event_data.event_id);
      continue;
    }

    // Store Q-vectors for this event
    std::map<int, std::map<Subdetector, QVec>> event_q_vectors;
    for (int n : m_harmonics)
    {
      for (auto det : m_subdetectors)
      {
        std::string det_str = (det == Subdetector::S) ? "S" : "N";
        std::string branch_name_x = std::format("sEPD_Q_{}_x_{}", det_str, n);
        std::string branch_name_y = std::format("sEPD_Q_{}_y_{}", det_str, n);
        event_q_vectors[n][det] = {m_event_data.q_vals[branch_name_x], m_event_data.q_vals[branch_name_y]};
      }
    }

    // Now process for each harmonic
    for (int n : m_harmonics)
    {
      const auto& q_S = event_q_vectors[n][Subdetector::S];
      const auto& q_N = event_q_vectors[n][Subdetector::N];

      std::string S_x_avg_name = std::format("h_sEPD_Q_S_x_{}_avg", n);
      std::string S_y_avg_name = std::format("h_sEPD_Q_S_y_{}_avg", n);
      std::string N_x_avg_name = std::format("h_sEPD_Q_N_x_{}_avg", n);
      std::string N_y_avg_name = std::format("h_sEPD_Q_N_y_{}_avg", n);

      // --- First Pass: Calculate Averages ---
      if (pass == Pass::CalculateAverages)
      {
        std::string hist_Q_S_name = std::format("h3_sEPD_Q_S_{}", n);
        std::string hist_Q_N_name = std::format("h3_sEPD_Q_N_{}", n);
        std::string psi_Q_name = std::format("h3_sEPD_Psi_{}", n);

        double psi_S = std::atan2(q_S.y, q_S.x);
        double psi_N = std::atan2(q_N.y, q_N.x);

        m_hists[S_x_avg_name]->Fill(cent, q_S.x);
        m_hists[S_y_avg_name]->Fill(cent, q_S.y);
        m_hists[N_x_avg_name]->Fill(cent, q_N.x);
        m_hists[N_y_avg_name]->Fill(cent, q_N.y);

        dynamic_cast<TH3*>(m_hists[hist_Q_S_name].get())->Fill(q_S.x, q_S.y, cent);
        dynamic_cast<TH3*>(m_hists[hist_Q_N_name].get())->Fill(q_N.x, q_N.y, cent);
        dynamic_cast<TH3*>(m_hists[psi_Q_name].get())->Fill(psi_S, psi_N, cent);
      }

      // --- Second Pass: Apply 1st Order, Derive 2nd Order ---
      if (pass == Pass::ApplyRecentering)
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

        double Q_S_x_avg = m_correction_data[cent_bin][n][Subdetector::S].avg_Q.x;
        double Q_S_y_avg = m_correction_data[cent_bin][n][Subdetector::S].avg_Q.y;
        double Q_N_x_avg = m_correction_data[cent_bin][n][Subdetector::N].avg_Q.x;
        double Q_N_y_avg = m_correction_data[cent_bin][n][Subdetector::N].avg_Q.y;

        QVec q_S_corr = {q_S.x - Q_S_x_avg, q_S.y - Q_S_y_avg};
        QVec q_N_corr = {q_N.x - Q_N_x_avg, q_N.y - Q_N_y_avg};

        double psi_S_corr = std::atan2(q_S_corr.y, q_S_corr.x);
        double psi_N_corr = std::atan2(q_N_corr.y, q_N_corr.x);

        m_hists[S_x_corr_avg_name]->Fill(cent, q_S_corr.x);
        m_hists[S_y_corr_avg_name]->Fill(cent, q_S_corr.y);
        m_hists[N_x_corr_avg_name]->Fill(cent, q_N_corr.x);
        m_hists[N_y_corr_avg_name]->Fill(cent, q_N_corr.y);

        m_hists[S_xx_avg_name]->Fill(cent, q_S_corr.x * q_S_corr.x);
        m_hists[S_yy_avg_name]->Fill(cent, q_S_corr.y * q_S_corr.y);
        m_hists[S_xy_avg_name]->Fill(cent, q_S_corr.x * q_S_corr.y);
        m_hists[N_xx_avg_name]->Fill(cent, q_N_corr.x * q_N_corr.x);
        m_hists[N_yy_avg_name]->Fill(cent, q_N_corr.y * q_N_corr.y);
        m_hists[N_xy_avg_name]->Fill(cent, q_N_corr.x * q_N_corr.y);

        dynamic_cast<TH3*>(m_hists[psi_Q_corr_name].get())->Fill(psi_S_corr, psi_N_corr, cent);
      }

      // --- Third Pass: Apply 2nd Order, Validate ---
      if (pass == Pass::ApplyFlattening)
      {
        std::string S_xx_corr_avg_name = std::format("h_sEPD_Q_S_xx_{}_corr_avg", n);
        std::string S_yy_corr_avg_name = std::format("h_sEPD_Q_S_yy_{}_corr_avg", n);
        std::string S_xy_corr_avg_name = std::format("h_sEPD_Q_S_xy_{}_corr_avg", n);
        std::string N_xx_corr_avg_name = std::format("h_sEPD_Q_N_xx_{}_corr_avg", n);
        std::string N_yy_corr_avg_name = std::format("h_sEPD_Q_N_yy_{}_corr_avg", n);
        std::string N_xy_corr_avg_name = std::format("h_sEPD_Q_N_xy_{}_corr_avg", n);

        std::string psi_Q_corr2_name = std::format("h3_sEPD_Psi_{}_corr2", n);

        double Q_S_x_avg = m_correction_data[cent_bin][n][Subdetector::S].avg_Q.x;
        double Q_S_y_avg = m_correction_data[cent_bin][n][Subdetector::S].avg_Q.y;
        double Q_N_x_avg = m_correction_data[cent_bin][n][Subdetector::N].avg_Q.x;
        double Q_N_y_avg = m_correction_data[cent_bin][n][Subdetector::N].avg_Q.y;

        QVec q_S_corr = {q_S.x - Q_S_x_avg, q_S.y - Q_S_y_avg};
        QVec q_N_corr = {q_N.x - Q_N_x_avg, q_N.y - Q_N_y_avg};

        const auto& X_S = m_correction_data[cent_bin][n][Subdetector::S].X_matrix;
        const auto& X_N = m_correction_data[cent_bin][n][Subdetector::N].X_matrix;

        double Q_S_x_corr2 = X_S[0][0] * q_S_corr.x + X_S[0][1] * q_S_corr.y;
        double Q_S_y_corr2 = X_S[1][0] * q_S_corr.x + X_S[1][1] * q_S_corr.y;
        double Q_N_x_corr2 = X_N[0][0] * q_N_corr.x + X_N[0][1] * q_N_corr.y;
        double Q_N_y_corr2 = X_N[1][0] * q_N_corr.x + X_N[1][1] * q_N_corr.y;

        double psi_S = std::atan2(Q_S_y_corr2, Q_S_x_corr2);
        double psi_N = std::atan2(Q_N_y_corr2, Q_N_x_corr2);

        m_hists[S_xx_corr_avg_name]->Fill(cent, Q_S_x_corr2 * Q_S_x_corr2);
        m_hists[S_yy_corr_avg_name]->Fill(cent, Q_S_y_corr2 * Q_S_y_corr2);
        m_hists[S_xy_corr_avg_name]->Fill(cent, Q_S_x_corr2 * Q_S_y_corr2);
        m_hists[N_xx_corr_avg_name]->Fill(cent, Q_N_x_corr2 * Q_N_x_corr2);
        m_hists[N_yy_corr_avg_name]->Fill(cent, Q_N_y_corr2 * Q_N_y_corr2);
        m_hists[N_xy_corr_avg_name]->Fill(cent, Q_N_x_corr2 * Q_N_y_corr2);

        dynamic_cast<TH3*>(m_hists[psi_Q_corr2_name].get())->Fill(psi_S, psi_N, cent);
      }
    }
  }

  std::cout << std::format("{:#<20}\n", "");
  std::cout << "Event Stats" << std::endl;
  std::cout << std::format("Events with sEPD South Charge 0: {}\n", ctr["sepd_south_zero_charge"]);
  std::cout << std::format("Events with sEPD North Charge 0: {}\n", ctr["sepd_north_zero_charge"]);
  std::cout << std::format("{:#<20}\n", "");

  std::cout << std::format("Pass: {}\n", static_cast<uint8_t>(pass));
  for (int cent_bin = 0; cent_bin < m_cent_bins; ++cent_bin)
  {
    for (int n : m_harmonics)
    {
      if (pass == Pass::CalculateAverages)
      {
        std::string S_x_avg_name = std::format("h_sEPD_Q_S_x_{}_avg", n);
        std::string S_y_avg_name = std::format("h_sEPD_Q_S_y_{}_avg", n);
        std::string N_x_avg_name = std::format("h_sEPD_Q_N_x_{}_avg", n);
        std::string N_y_avg_name = std::format("h_sEPD_Q_N_y_{}_avg", n);

        double Q_S_x_avg = m_hists[S_x_avg_name]->GetBinContent(cent_bin + 1);
        double Q_S_y_avg = m_hists[S_y_avg_name]->GetBinContent(cent_bin + 1);
        double Q_N_x_avg = m_hists[N_x_avg_name]->GetBinContent(cent_bin + 1);
        double Q_N_y_avg = m_hists[N_y_avg_name]->GetBinContent(cent_bin + 1);

        m_correction_data[cent_bin][n][Subdetector::S].avg_Q = {Q_S_x_avg, Q_S_y_avg};
        m_correction_data[cent_bin][n][Subdetector::N].avg_Q = {Q_N_x_avg, Q_N_y_avg};

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

      if (pass == Pass::ApplyRecentering)
      {
        std::string S_x_corr_avg_name = std::format("h_sEPD_Q_S_x_{}_corr_avg", n);
        std::string S_y_corr_avg_name = std::format("h_sEPD_Q_S_y_{}_corr_avg", n);
        std::string N_x_corr_avg_name = std::format("h_sEPD_Q_N_x_{}_corr_avg", n);
        std::string N_y_corr_avg_name = std::format("h_sEPD_Q_N_y_{}_corr_avg", n);

        double Q_S_x_corr_avg = m_hists[S_x_corr_avg_name]->GetBinContent(cent_bin + 1);
        double Q_S_y_corr_avg = m_hists[S_y_corr_avg_name]->GetBinContent(cent_bin + 1);
        double Q_N_x_corr_avg = m_hists[N_x_corr_avg_name]->GetBinContent(cent_bin + 1);
        double Q_N_y_corr_avg = m_hists[N_y_corr_avg_name]->GetBinContent(cent_bin + 1);

        // -- Compute 2nd Order Correction --
        std::string S_xx_avg_name = std::format("h_sEPD_Q_S_xx_{}_avg", n);
        std::string S_yy_avg_name = std::format("h_sEPD_Q_S_yy_{}_avg", n);
        std::string S_xy_avg_name = std::format("h_sEPD_Q_S_xy_{}_avg", n);
        std::string N_xx_avg_name = std::format("h_sEPD_Q_N_xx_{}_avg", n);
        std::string N_yy_avg_name = std::format("h_sEPD_Q_N_yy_{}_avg", n);
        std::string N_xy_avg_name = std::format("h_sEPD_Q_N_xy_{}_avg", n);

        double Q_S_xx_avg = m_hists[S_xx_avg_name]->GetBinContent(cent_bin + 1);
        double Q_S_yy_avg = m_hists[S_yy_avg_name]->GetBinContent(cent_bin + 1);
        double Q_S_xy_avg = m_hists[S_xy_avg_name]->GetBinContent(cent_bin + 1);
        double Q_N_xx_avg = m_hists[N_xx_avg_name]->GetBinContent(cent_bin + 1);
        double Q_N_yy_avg = m_hists[N_yy_avg_name]->GetBinContent(cent_bin + 1);
        double Q_N_xy_avg = m_hists[N_xy_avg_name]->GetBinContent(cent_bin + 1);

        m_correction_data[cent_bin][n][Subdetector::S].avg_Q_xx = Q_S_xx_avg;
        m_correction_data[cent_bin][n][Subdetector::S].avg_Q_yy = Q_S_yy_avg;
        m_correction_data[cent_bin][n][Subdetector::S].avg_Q_xy = Q_S_xy_avg;
        m_correction_data[cent_bin][n][Subdetector::N].avg_Q_xx = Q_N_xx_avg;
        m_correction_data[cent_bin][n][Subdetector::N].avg_Q_yy = Q_N_yy_avg;
        m_correction_data[cent_bin][n][Subdetector::N].avg_Q_xy = Q_N_xy_avg;

        // Compute N and D terms
        double D_term_S = std::sqrt((Q_S_xx_avg * Q_S_yy_avg) - (Q_S_xy_avg * Q_S_xy_avg));
        double D_term_N = std::sqrt((Q_N_xx_avg * Q_N_yy_avg) - (Q_N_xy_avg * Q_N_xy_avg));
        double N_term_S = std::sqrt((Q_S_xx_avg * Q_S_yy_avg) - (Q_S_xy_avg * Q_S_xy_avg));
        double N_term_N = std::sqrt((Q_N_xx_avg * Q_N_yy_avg) - (Q_N_xy_avg * Q_N_xy_avg));

        auto& X_S_matrix = m_correction_data[cent_bin][n][Subdetector::S].X_matrix;
        auto& X_N_matrix = m_correction_data[cent_bin][n][Subdetector::N].X_matrix;

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
            "Q_S_xx_avg: {:13.10f}, "
            "Q_S_yy_avg: {:13.10f}, "
            "Q_S_xy_avg: {:13.10f}, "
            "Q_N_xx_avg: {:13.10f}, "
            "Q_N_yy_avg: {:13.10f}, "
            "Q_N_xy_avg: {:13.10f}\n",
            cent_bin,
            n,
            Q_S_x_corr_avg,
            Q_S_y_corr_avg,
            Q_N_x_corr_avg,
            Q_N_y_corr_avg,
            Q_S_xx_avg,
            Q_S_yy_avg,
            Q_S_xy_avg,
            Q_N_xx_avg,
            Q_N_yy_avg,
            Q_N_xy_avg);
      }

      if (pass == Pass::ApplyFlattening)
      {
        std::string S_xx_corr_avg_name = std::format("h_sEPD_Q_S_xx_{}_corr_avg", n);
        std::string S_yy_corr_avg_name = std::format("h_sEPD_Q_S_yy_{}_corr_avg", n);
        std::string S_xy_corr_avg_name = std::format("h_sEPD_Q_S_xy_{}_corr_avg", n);
        std::string N_xx_corr_avg_name = std::format("h_sEPD_Q_N_xx_{}_corr_avg", n);
        std::string N_yy_corr_avg_name = std::format("h_sEPD_Q_N_yy_{}_corr_avg", n);
        std::string N_xy_corr_avg_name = std::format("h_sEPD_Q_N_xy_{}_corr_avg", n);

        double Q_S_xx_corr_avg = m_hists[S_xx_corr_avg_name]->GetBinContent(cent_bin + 1);
        double Q_S_yy_corr_avg = m_hists[S_yy_corr_avg_name]->GetBinContent(cent_bin + 1);
        double Q_S_xy_corr_avg = m_hists[S_xy_corr_avg_name]->GetBinContent(cent_bin + 1);
        double Q_N_xx_corr_avg = m_hists[N_xx_corr_avg_name]->GetBinContent(cent_bin + 1);
        double Q_N_yy_corr_avg = m_hists[N_yy_corr_avg_name]->GetBinContent(cent_bin + 1);
        double Q_N_xy_corr_avg = m_hists[N_xy_corr_avg_name]->GetBinContent(cent_bin + 1);

        std::cout << std::format(
            "Centrality Bin: {}, "
            "Harmonic: {}, "
            "Q_S_xx_corr_avg / Q_S_yy_corr_avg: {:13.10f}, "
            "Q_N_xx_corr_avg / Q_N_yy_corr_avg: {:13.10f}, "
            "Q_S_xy_corr_avg: {:13.10f}, "
            "Q_N_xy_corr_avg: {:13.10f}\n",
            cent_bin,
            n,
            Q_S_xx_corr_avg / Q_S_yy_corr_avg,
            Q_N_xx_corr_avg / Q_N_yy_corr_avg,
            Q_S_xy_corr_avg,
            Q_N_xy_corr_avg);
      }
    }
  }

  std::cout << "Event loop finished." << std::endl;
}

void QvectorAnalysis::save_results() const
{
  std::filesystem::create_directories(m_output_dir);
  std::string output_filename = m_output_dir + "/test.root";
  auto output_file = std::make_unique<TFile>(output_filename.c_str(), "RECREATE");

  for (const auto& [name, hist] : m_hists)
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

  if (argc < 2 || argc > 5)
  {
    std::cout << "Usage: " << argv[0] << " <input_list_file> [events] [verbosity] [output_directory]" << std::endl;
    return 1;
  }

  const std::string input_list = argv[1];
  long long events = (argc >= 3) ? std::atoll(argv[2]) : 0;
  int verbosity = (argc >= 4) ? std::atoi(argv[3]) : 0;
  std::string output_dir = (argc >= 5) ? argv[4] : ".";

  try
  {
    QvectorAnalysis analysis(input_list, events, verbosity, output_dir);
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
