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
#include <TProfile.h>
#include <TH3.h>
#include <TROOT.h>

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
    , m_D_S_2(m_cent_bins)
    , m_D_N_2(m_cent_bins)
    , m_N_S_2(m_cent_bins)
    , m_N_N_2(m_cent_bins)
    , m_D_S_3(m_cent_bins)
    , m_D_N_3(m_cent_bins)
    , m_N_S_3(m_cent_bins)
    , m_N_N_3(m_cent_bins)
    , m_D_S_4(m_cent_bins)
    , m_D_N_4(m_cent_bins)
    , m_N_S_4(m_cent_bins)
    , m_N_N_4(m_cent_bins)
    , m_X_S_2(m_cent_bins, std::vector<std::vector<double>>(2, std::vector<double>(2, 0)))
    , m_X_N_2(m_cent_bins, std::vector<std::vector<double>>(2, std::vector<double>(2, 0)))
    , m_X_S_3(m_cent_bins, std::vector<std::vector<double>>(2, std::vector<double>(2, 0)))
    , m_X_N_3(m_cent_bins, std::vector<std::vector<double>>(2, std::vector<double>(2, 0)))
    , m_X_S_4(m_cent_bins, std::vector<std::vector<double>>(2, std::vector<double>(2, 0)))
    , m_X_N_4(m_cent_bins, std::vector<std::vector<double>>(2, std::vector<double>(2, 0)))
  {
  }

  void run()
  {
    setup_chain();
    init_hists();
    run_event_loop(0);
    run_event_loop(1);
    run_event_loop(2);
    save_results();
  }

 private:
  struct EventData
  {
    int event_id{0};
    double event_zvertex{0.0};
    double event_centrality{0.0};
    double sEPD_Q_S_x_2{0.0};
    double sEPD_Q_S_y_2{0.0};
    double sEPD_Q_N_x_2{0.0};
    double sEPD_Q_N_y_2{0.0};
    double sEPD_Q_S_x_3{0.0};
    double sEPD_Q_S_y_3{0.0};
    double sEPD_Q_N_x_3{0.0};
    double sEPD_Q_N_y_3{0.0};
    double sEPD_Q_S_x_4{0.0};
    double sEPD_Q_S_y_4{0.0};
    double sEPD_Q_N_x_4{0.0};
    double sEPD_Q_N_y_4{0.0};
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

  size_t m_cent_bins{10};

  // Q vector members
  // Second Order Correction
  std::vector<double> m_D_S_2;
  std::vector<double> m_D_N_2;

  std::vector<double> m_N_S_2;
  std::vector<double> m_N_N_2;

  std::vector<double> m_D_S_3;
  std::vector<double> m_D_N_3;

  std::vector<double> m_N_S_3;
  std::vector<double> m_N_N_3;

  std::vector<double> m_D_S_4;
  std::vector<double> m_D_N_4;

  std::vector<double> m_N_S_4;
  std::vector<double> m_N_N_4;

  std::vector<std::vector<std::vector<double>>> m_X_S_2;
  std::vector<std::vector<std::vector<double>>> m_X_N_2;

  std::vector<std::vector<std::vector<double>>> m_X_S_3;
  std::vector<std::vector<std::vector<double>>> m_X_N_3;

  std::vector<std::vector<std::vector<double>>> m_X_S_4;
  std::vector<std::vector<std::vector<double>>> m_X_N_4;

  // Hists
  std::map<std::string, std::unique_ptr<TH1>> m_hists;

  // --- Private Helper Methods ---
  void setup_chain();
  void init_hists();
  void run_event_loop(int order);
  void save_results();
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
  m_chain->SetBranchStatus("sEPD_Q_S_x_2", true);
  m_chain->SetBranchStatus("sEPD_Q_S_y_2", true);
  m_chain->SetBranchStatus("sEPD_Q_N_x_2", true);
  m_chain->SetBranchStatus("sEPD_Q_N_y_2", true);
  m_chain->SetBranchStatus("sEPD_Q_S_x_3", true);
  m_chain->SetBranchStatus("sEPD_Q_S_y_3", true);
  m_chain->SetBranchStatus("sEPD_Q_N_x_3", true);
  m_chain->SetBranchStatus("sEPD_Q_N_y_3", true);
  m_chain->SetBranchStatus("sEPD_Q_S_x_4", true);
  m_chain->SetBranchStatus("sEPD_Q_S_y_4", true);
  m_chain->SetBranchStatus("sEPD_Q_N_x_4", true);
  m_chain->SetBranchStatus("sEPD_Q_N_y_4", true);
  // m_chain->SetBranchStatus("sepd_charge", true);
  // m_chain->SetBranchStatus("sepd_phi", true);
  // m_chain->SetBranchStatus("sepd_eta", true);
  // m_chain->SetBranchStatus("mbd_charge", true);
  // m_chain->SetBranchStatus("mbd_phi", true);
  // m_chain->SetBranchStatus("mbd_eta", true);

  m_chain->SetBranchAddress("event_id", &m_event_data.event_id);
  m_chain->SetBranchAddress("event_centrality", &m_event_data.event_centrality);
  m_chain->SetBranchAddress("sEPD_Q_S_x_2", &m_event_data.sEPD_Q_S_x_2);
  m_chain->SetBranchAddress("sEPD_Q_S_y_2", &m_event_data.sEPD_Q_S_y_2);
  m_chain->SetBranchAddress("sEPD_Q_N_x_2", &m_event_data.sEPD_Q_N_x_2);
  m_chain->SetBranchAddress("sEPD_Q_N_y_2", &m_event_data.sEPD_Q_N_y_2);
  m_chain->SetBranchAddress("sEPD_Q_S_x_3", &m_event_data.sEPD_Q_S_x_3);
  m_chain->SetBranchAddress("sEPD_Q_S_y_3", &m_event_data.sEPD_Q_S_y_3);
  m_chain->SetBranchAddress("sEPD_Q_N_x_3", &m_event_data.sEPD_Q_N_x_3);
  m_chain->SetBranchAddress("sEPD_Q_N_y_3", &m_event_data.sEPD_Q_N_y_3);
  m_chain->SetBranchAddress("sEPD_Q_S_x_4", &m_event_data.sEPD_Q_S_x_4);
  m_chain->SetBranchAddress("sEPD_Q_S_y_4", &m_event_data.sEPD_Q_S_y_4);
  m_chain->SetBranchAddress("sEPD_Q_N_x_4", &m_event_data.sEPD_Q_N_x_4);
  m_chain->SetBranchAddress("sEPD_Q_N_y_4", &m_event_data.sEPD_Q_N_y_4);
  // m_chain->SetBranchAddress("sepd_charge", &m_event_data.sepd_charge);
  // m_chain->SetBranchAddress("sepd_phi", &m_event_data.sepd_phi);
  // m_chain->SetBranchAddress("sepd_eta", &m_event_data.sepd_eta);
  // m_chain->SetBranchAddress("mbd_charge", &m_event_data.mbd_charge);
  // m_chain->SetBranchAddress("mbd_phi", &m_event_data.mbd_phi);
  // m_chain->SetBranchAddress("mbd_eta", &m_event_data.mbd_eta);
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

  m_hists["h3_sEPD_Q_S_2"] = std::make_unique<TH3F>("h3_sEPD_Q_S_2", "sEPD South Q (Order 2): |z| < 10 cm and MB; Q_{x}; Q_{y}; Centrality [%]", bins_Q, Q_low, Q_high, bins_Q, Q_low, Q_high, m_cent_bins, cent_low, cent_high);
  m_hists["h3_sEPD_Q_N_2"] = std::make_unique<TH3F>("h3_sEPD_Q_N_2", "sEPD North Q (Order 2): |z| < 10 cm and MB; Q_{x}; Q_{y}; Centrality [%]", bins_Q, Q_low, Q_high, bins_Q, Q_low, Q_high, m_cent_bins, cent_low, cent_high);
  m_hists["h3_sEPD_Q_S_3"] = std::make_unique<TH3F>("h3_sEPD_Q_S_3", "sEPD South Q (Order 3): |z| < 10 cm and MB; Q_{x}; Q_{y}; Centrality [%]", bins_Q, Q_low, Q_high, bins_Q, Q_low, Q_high, m_cent_bins, cent_low, cent_high);
  m_hists["h3_sEPD_Q_N_3"] = std::make_unique<TH3F>("h3_sEPD_Q_N_3", "sEPD North Q (Order 3): |z| < 10 cm and MB; Q_{x}; Q_{y}; Centrality [%]", bins_Q, Q_low, Q_high, bins_Q, Q_low, Q_high, m_cent_bins, cent_low, cent_high);
  m_hists["h3_sEPD_Q_S_4"] = std::make_unique<TH3F>("h3_sEPD_Q_S_4", "sEPD South Q (Order 4): |z| < 10 cm and MB; Q_{x}; Q_{y}; Centrality [%]", bins_Q, Q_low, Q_high, bins_Q, Q_low, Q_high, m_cent_bins, cent_low, cent_high);
  m_hists["h3_sEPD_Q_N_4"] = std::make_unique<TH3F>("h3_sEPD_Q_N_4", "sEPD North Q (Order 4): |z| < 10 cm and MB; Q_{x}; Q_{y}; Centrality [%]", bins_Q, Q_low, Q_high, bins_Q, Q_low, Q_high, m_cent_bins, cent_low, cent_high);

  m_hists["h3_sEPD_Psi_2"] = std::make_unique<TH3F>("h3_sEPD_Psi_2", "sEPD #Psi (Order 2): |z| < 10 cm and MB; 2#Psi^{S}_{2}; 2#Psi^{N}_{2}; Centrality [%]", bins_psi, psi_low, psi_high, bins_psi, psi_low, psi_high, m_cent_bins, cent_low, cent_high);
  m_hists["h3_sEPD_Psi_3"] = std::make_unique<TH3F>("h3_sEPD_Psi_3", "sEPD #Psi (Order 3): |z| < 10 cm and MB; 3#Psi^{S}_{3}; 3#Psi^{N}_{3}; Centrality [%]", bins_psi, psi_low, psi_high, bins_psi, psi_low, psi_high, m_cent_bins, cent_low, cent_high);
  m_hists["h3_sEPD_Psi_4"] = std::make_unique<TH3F>("h3_sEPD_Psi_4", "sEPD #Psi (Order 4): |z| < 10 cm and MB; 4#Psi^{S}_{4}; 4#Psi^{N}_{4}; Centrality [%]", bins_psi, psi_low, psi_high, bins_psi, psi_low, psi_high, m_cent_bins, cent_low, cent_high);

  m_hists["h3_sEPD_Psi_2_corr"] = std::make_unique<TH3F>("h3_sEPD_Psi_2_corr", "sEPD #Psi (Order 2): |z| < 10 cm and MB; 2#Psi^{S}_{2}; 2#Psi^{N}_{2}; Centrality [%]", bins_psi, psi_low, psi_high, bins_psi, psi_low, psi_high, m_cent_bins, cent_low, cent_high);
  m_hists["h3_sEPD_Psi_3_corr"] = std::make_unique<TH3F>("h3_sEPD_Psi_3_corr", "sEPD #Psi (Order 3): |z| < 10 cm and MB; 3#Psi^{S}_{3}; 3#Psi^{N}_{3}; Centrality [%]", bins_psi, psi_low, psi_high, bins_psi, psi_low, psi_high, m_cent_bins, cent_low, cent_high);
  m_hists["h3_sEPD_Psi_4_corr"] = std::make_unique<TH3F>("h3_sEPD_Psi_4_corr", "sEPD #Psi (Order 4): |z| < 10 cm and MB; 4#Psi^{S}_{4}; 4#Psi^{N}_{4}; Centrality [%]", bins_psi, psi_low, psi_high, bins_psi, psi_low, psi_high, m_cent_bins, cent_low, cent_high);

  m_hists["h3_sEPD_Psi_2_corr2"] = std::make_unique<TH3F>("h3_sEPD_Psi_2_corr2", "sEPD #Psi (Order 2): |z| < 10 cm and MB; 2#Psi^{S}_{2}; 2#Psi^{N}_{2}; Centrality [%]", bins_psi, psi_low, psi_high, bins_psi, psi_low, psi_high, m_cent_bins, cent_low, cent_high);
  m_hists["h3_sEPD_Psi_3_corr2"] = std::make_unique<TH3F>("h3_sEPD_Psi_3_corr2", "sEPD #Psi (Order 3): |z| < 10 cm and MB; 3#Psi^{S}_{3}; 3#Psi^{N}_{3}; Centrality [%]", bins_psi, psi_low, psi_high, bins_psi, psi_low, psi_high, m_cent_bins, cent_low, cent_high);
  m_hists["h3_sEPD_Psi_4_corr2"] = std::make_unique<TH3F>("h3_sEPD_Psi_4_corr2", "sEPD #Psi (Order 4): |z| < 10 cm and MB; 4#Psi^{S}_{4}; 4#Psi^{N}_{4}; Centrality [%]", bins_psi, psi_low, psi_high, bins_psi, psi_low, psi_high, m_cent_bins, cent_low, cent_high);

  m_hists["h_sEPD_Q_S_x_2_avg"] = std::make_unique<TProfile>("h_sEPD_Q_S_x_2_avg", "sEPD South; Centrality [%]; <Q_{2,x}>", m_cent_bins, cent_low, cent_high);
  m_hists["h_sEPD_Q_S_y_2_avg"] = std::make_unique<TProfile>("h_sEPD_Q_S_y_2_avg", "sEPD South; Centrality [%]; <Q_{2,y}>", m_cent_bins, cent_low, cent_high);
  m_hists["h_sEPD_Q_N_x_2_avg"] = std::make_unique<TProfile>("h_sEPD_Q_N_x_2_avg", "sEPD North; Centrality [%]; <Q_{2,x}>", m_cent_bins, cent_low, cent_high);
  m_hists["h_sEPD_Q_N_y_2_avg"] = std::make_unique<TProfile>("h_sEPD_Q_N_y_2_avg", "sEPD North; Centrality [%]; <Q_{2,y}>", m_cent_bins, cent_low, cent_high);
  m_hists["h_sEPD_Q_S_x_3_avg"] = std::make_unique<TProfile>("h_sEPD_Q_S_x_3_avg", "sEPD South; Centrality [%]; <Q_{3,x}>", m_cent_bins, cent_low, cent_high);
  m_hists["h_sEPD_Q_S_y_3_avg"] = std::make_unique<TProfile>("h_sEPD_Q_S_y_3_avg", "sEPD South; Centrality [%]; <Q_{3,y}>", m_cent_bins, cent_low, cent_high);
  m_hists["h_sEPD_Q_N_x_3_avg"] = std::make_unique<TProfile>("h_sEPD_Q_N_x_3_avg", "sEPD North; Centrality [%]; <Q_{3,x}>", m_cent_bins, cent_low, cent_high);
  m_hists["h_sEPD_Q_N_y_3_avg"] = std::make_unique<TProfile>("h_sEPD_Q_N_y_3_avg", "sEPD North; Centrality [%]; <Q_{3,y}>", m_cent_bins, cent_low, cent_high);
  m_hists["h_sEPD_Q_S_x_4_avg"] = std::make_unique<TProfile>("h_sEPD_Q_S_x_4_avg", "sEPD South; Centrality [%]; <Q_{4,x}>", m_cent_bins, cent_low, cent_high);
  m_hists["h_sEPD_Q_S_y_4_avg"] = std::make_unique<TProfile>("h_sEPD_Q_S_y_4_avg", "sEPD South; Centrality [%]; <Q_{4,y}>", m_cent_bins, cent_low, cent_high);
  m_hists["h_sEPD_Q_N_x_4_avg"] = std::make_unique<TProfile>("h_sEPD_Q_N_x_4_avg", "sEPD North; Centrality [%]; <Q_{4,x}>", m_cent_bins, cent_low, cent_high);
  m_hists["h_sEPD_Q_N_y_4_avg"] = std::make_unique<TProfile>("h_sEPD_Q_N_y_4_avg", "sEPD North; Centrality [%]; <Q_{4,y}>", m_cent_bins, cent_low, cent_high);

  m_hists["h_sEPD_Q_S_x_2_corr_avg"] = std::make_unique<TProfile>("h_sEPD_Q_S_x_2_corr_avg", "sEPD South; Centrality [%]; <Q_{2,x}>", m_cent_bins, cent_low, cent_high);
  m_hists["h_sEPD_Q_S_y_2_corr_avg"] = std::make_unique<TProfile>("h_sEPD_Q_S_y_2_corr_avg", "sEPD South; Centrality [%]; <Q_{2,y}>", m_cent_bins, cent_low, cent_high);
  m_hists["h_sEPD_Q_N_x_2_corr_avg"] = std::make_unique<TProfile>("h_sEPD_Q_N_x_2_corr_avg", "sEPD North; Centrality [%]; <Q_{2,x}>", m_cent_bins, cent_low, cent_high);
  m_hists["h_sEPD_Q_N_y_2_corr_avg"] = std::make_unique<TProfile>("h_sEPD_Q_N_y_2_corr_avg", "sEPD North; Centrality [%]; <Q_{2,y}>", m_cent_bins, cent_low, cent_high);
  m_hists["h_sEPD_Q_S_x_3_corr_avg"] = std::make_unique<TProfile>("h_sEPD_Q_S_x_3_corr_avg", "sEPD South; Centrality [%]; <Q_{3,x}>", m_cent_bins, cent_low, cent_high);
  m_hists["h_sEPD_Q_S_y_3_corr_avg"] = std::make_unique<TProfile>("h_sEPD_Q_S_y_3_corr_avg", "sEPD South; Centrality [%]; <Q_{3,y}>", m_cent_bins, cent_low, cent_high);
  m_hists["h_sEPD_Q_N_x_3_corr_avg"] = std::make_unique<TProfile>("h_sEPD_Q_N_x_3_corr_avg", "sEPD North; Centrality [%]; <Q_{3,x}>", m_cent_bins, cent_low, cent_high);
  m_hists["h_sEPD_Q_N_y_3_corr_avg"] = std::make_unique<TProfile>("h_sEPD_Q_N_y_3_corr_avg", "sEPD North; Centrality [%]; <Q_{3,y}>", m_cent_bins, cent_low, cent_high);
  m_hists["h_sEPD_Q_S_x_4_corr_avg"] = std::make_unique<TProfile>("h_sEPD_Q_S_x_4_corr_avg", "sEPD South; Centrality [%]; <Q_{4,x}>", m_cent_bins, cent_low, cent_high);
  m_hists["h_sEPD_Q_S_y_4_corr_avg"] = std::make_unique<TProfile>("h_sEPD_Q_S_y_4_corr_avg", "sEPD South; Centrality [%]; <Q_{4,y}>", m_cent_bins, cent_low, cent_high);
  m_hists["h_sEPD_Q_N_x_4_corr_avg"] = std::make_unique<TProfile>("h_sEPD_Q_N_x_4_corr_avg", "sEPD North; Centrality [%]; <Q_{4,x}>", m_cent_bins, cent_low, cent_high);
  m_hists["h_sEPD_Q_N_y_4_corr_avg"] = std::make_unique<TProfile>("h_sEPD_Q_N_y_4_corr_avg", "sEPD North; Centrality [%]; <Q_{4,y}>", m_cent_bins, cent_low, cent_high);

  m_hists["h_sEPD_Q_S_xx_2_avg"] = std::make_unique<TProfile>("h_sEPD_Q_S_xx_2_avg", "sEPD South; Centrality [%]; <Q_{2,x}^{2}>", m_cent_bins, cent_low, cent_high);
  m_hists["h_sEPD_Q_S_yy_2_avg"] = std::make_unique<TProfile>("h_sEPD_Q_S_yy_2_avg", "sEPD South; Centrality [%]; <Q_{2,y}^{2}>", m_cent_bins, cent_low, cent_high);
  m_hists["h_sEPD_Q_S_xy_2_avg"] = std::make_unique<TProfile>("h_sEPD_Q_S_xy_2_avg", "sEPD South; Centrality [%]; <Q_{2,x} Q_{2,y}>", m_cent_bins, cent_low, cent_high);
  m_hists["h_sEPD_Q_N_xx_2_avg"] = std::make_unique<TProfile>("h_sEPD_Q_N_xx_2_avg", "sEPD North; Centrality [%]; <Q_{2,x}^{2}>", m_cent_bins, cent_low, cent_high);
  m_hists["h_sEPD_Q_N_yy_2_avg"] = std::make_unique<TProfile>("h_sEPD_Q_N_yy_2_avg", "sEPD North; Centrality [%]; <Q_{2,y}^{2}>", m_cent_bins, cent_low, cent_high);
  m_hists["h_sEPD_Q_N_xy_2_avg"] = std::make_unique<TProfile>("h_sEPD_Q_N_xy_2_avg", "sEPD North; Centrality [%]; <Q_{2,x} Q_{2,y}>", m_cent_bins, cent_low, cent_high);

  m_hists["h_sEPD_Q_S_xx_3_avg"] = std::make_unique<TProfile>("h_sEPD_Q_S_xx_3_avg", "sEPD South; Centrality [%]; <Q_{3,x}^{2}>", m_cent_bins, cent_low, cent_high);
  m_hists["h_sEPD_Q_S_yy_3_avg"] = std::make_unique<TProfile>("h_sEPD_Q_S_yy_3_avg", "sEPD South; Centrality [%]; <Q_{3,y}^{2}>", m_cent_bins, cent_low, cent_high);
  m_hists["h_sEPD_Q_S_xy_3_avg"] = std::make_unique<TProfile>("h_sEPD_Q_S_xy_3_avg", "sEPD South; Centrality [%]; <Q_{3,x} Q_{3,y}>", m_cent_bins, cent_low, cent_high);
  m_hists["h_sEPD_Q_N_xx_3_avg"] = std::make_unique<TProfile>("h_sEPD_Q_N_xx_3_avg", "sEPD North; Centrality [%]; <Q_{3,x}^{2}>", m_cent_bins, cent_low, cent_high);
  m_hists["h_sEPD_Q_N_yy_3_avg"] = std::make_unique<TProfile>("h_sEPD_Q_N_yy_3_avg", "sEPD North; Centrality [%]; <Q_{3,y}^{2}>", m_cent_bins, cent_low, cent_high);
  m_hists["h_sEPD_Q_N_xy_3_avg"] = std::make_unique<TProfile>("h_sEPD_Q_N_xy_3_avg", "sEPD North; Centrality [%]; <Q_{3,x} Q_{3,y}>", m_cent_bins, cent_low, cent_high);

  m_hists["h_sEPD_Q_S_xx_4_avg"] = std::make_unique<TProfile>("h_sEPD_Q_S_xx_4_avg", "sEPD South; Centrality [%]; <Q_{4,x}^{2}>", m_cent_bins, cent_low, cent_high);
  m_hists["h_sEPD_Q_S_yy_4_avg"] = std::make_unique<TProfile>("h_sEPD_Q_S_yy_4_avg", "sEPD South; Centrality [%]; <Q_{4,y}^{2}>", m_cent_bins, cent_low, cent_high);
  m_hists["h_sEPD_Q_S_xy_4_avg"] = std::make_unique<TProfile>("h_sEPD_Q_S_xy_4_avg", "sEPD South; Centrality [%]; <Q_{4,x} Q_{4,y}>", m_cent_bins, cent_low, cent_high);
  m_hists["h_sEPD_Q_N_xx_4_avg"] = std::make_unique<TProfile>("h_sEPD_Q_N_xx_4_avg", "sEPD North; Centrality [%]; <Q_{4,x}^{2}>", m_cent_bins, cent_low, cent_high);
  m_hists["h_sEPD_Q_N_yy_4_avg"] = std::make_unique<TProfile>("h_sEPD_Q_N_yy_4_avg", "sEPD North; Centrality [%]; <Q_{4,y}^{2}>", m_cent_bins, cent_low, cent_high);
  m_hists["h_sEPD_Q_N_xy_4_avg"] = std::make_unique<TProfile>("h_sEPD_Q_N_xy_4_avg", "sEPD North; Centrality [%]; <Q_{4,x} Q_{4,y}>", m_cent_bins, cent_low, cent_high);

  m_hists["h_sEPD_Q_S_xx_2_corr_avg"] = std::make_unique<TProfile>("h_sEPD_Q_S_xx_2_corr_avg", "sEPD South; Centrality [%]; <Q_{2,x}^{2}>", m_cent_bins, cent_low, cent_high);
  m_hists["h_sEPD_Q_S_yy_2_corr_avg"] = std::make_unique<TProfile>("h_sEPD_Q_S_yy_2_corr_avg", "sEPD South; Centrality [%]; <Q_{2,y}^{2}>", m_cent_bins, cent_low, cent_high);
  m_hists["h_sEPD_Q_S_xy_2_corr_avg"] = std::make_unique<TProfile>("h_sEPD_Q_S_xy_2_corr_avg", "sEPD South; Centrality [%]; <Q_{2,x} Q_{2,y}>", m_cent_bins, cent_low, cent_high);
  m_hists["h_sEPD_Q_N_xx_2_corr_avg"] = std::make_unique<TProfile>("h_sEPD_Q_N_xx_2_corr_avg", "sEPD North; Centrality [%]; <Q_{2,x}^{2}>", m_cent_bins, cent_low, cent_high);
  m_hists["h_sEPD_Q_N_yy_2_corr_avg"] = std::make_unique<TProfile>("h_sEPD_Q_N_yy_2_corr_avg", "sEPD North; Centrality [%]; <Q_{2,y}^{2}>", m_cent_bins, cent_low, cent_high);
  m_hists["h_sEPD_Q_N_xy_2_corr_avg"] = std::make_unique<TProfile>("h_sEPD_Q_N_xy_2_corr_avg", "sEPD North; Centrality [%]; <Q_{2,x} Q_{2,y}>", m_cent_bins, cent_low, cent_high);

  m_hists["h_sEPD_Q_S_xx_3_corr_avg"] = std::make_unique<TProfile>("h_sEPD_Q_S_xx_3_corr_avg", "sEPD South; Centrality [%]; <Q_{3,x}^{2}>", m_cent_bins, cent_low, cent_high);
  m_hists["h_sEPD_Q_S_yy_3_corr_avg"] = std::make_unique<TProfile>("h_sEPD_Q_S_yy_3_corr_avg", "sEPD South; Centrality [%]; <Q_{3,y}^{2}>", m_cent_bins, cent_low, cent_high);
  m_hists["h_sEPD_Q_S_xy_3_corr_avg"] = std::make_unique<TProfile>("h_sEPD_Q_S_xy_3_corr_avg", "sEPD South; Centrality [%]; <Q_{3,x} Q_{3,y}>", m_cent_bins, cent_low, cent_high);
  m_hists["h_sEPD_Q_N_xx_3_corr_avg"] = std::make_unique<TProfile>("h_sEPD_Q_N_xx_3_corr_avg", "sEPD North; Centrality [%]; <Q_{3,x}^{2}>", m_cent_bins, cent_low, cent_high);
  m_hists["h_sEPD_Q_N_yy_3_corr_avg"] = std::make_unique<TProfile>("h_sEPD_Q_N_yy_3_corr_avg", "sEPD North; Centrality [%]; <Q_{3,y}^{2}>", m_cent_bins, cent_low, cent_high);
  m_hists["h_sEPD_Q_N_xy_3_corr_avg"] = std::make_unique<TProfile>("h_sEPD_Q_N_xy_3_corr_avg", "sEPD North; Centrality [%]; <Q_{3,x} Q_{3,y}>", m_cent_bins, cent_low, cent_high);

  m_hists["h_sEPD_Q_S_xx_4_corr_avg"] = std::make_unique<TProfile>("h_sEPD_Q_S_xx_4_corr_avg", "sEPD South; Centrality [%]; <Q_{4,x}^{2}>", m_cent_bins, cent_low, cent_high);
  m_hists["h_sEPD_Q_S_yy_4_corr_avg"] = std::make_unique<TProfile>("h_sEPD_Q_S_yy_4_corr_avg", "sEPD South; Centrality [%]; <Q_{4,y}^{2}>", m_cent_bins, cent_low, cent_high);
  m_hists["h_sEPD_Q_S_xy_4_corr_avg"] = std::make_unique<TProfile>("h_sEPD_Q_S_xy_4_corr_avg", "sEPD South; Centrality [%]; <Q_{4,x} Q_{4,y}>", m_cent_bins, cent_low, cent_high);
  m_hists["h_sEPD_Q_N_xx_4_corr_avg"] = std::make_unique<TProfile>("h_sEPD_Q_N_xx_4_corr_avg", "sEPD North; Centrality [%]; <Q_{4,x}^{2}>", m_cent_bins, cent_low, cent_high);
  m_hists["h_sEPD_Q_N_yy_4_corr_avg"] = std::make_unique<TProfile>("h_sEPD_Q_N_yy_4_corr_avg", "sEPD North; Centrality [%]; <Q_{4,y}^{2}>", m_cent_bins, cent_low, cent_high);
  m_hists["h_sEPD_Q_N_xy_4_corr_avg"] = std::make_unique<TProfile>("h_sEPD_Q_N_xy_4_corr_avg", "sEPD North; Centrality [%]; <Q_{4,x} Q_{4,y}>", m_cent_bins, cent_low, cent_high);
}

void QvectorAnalysis::run_event_loop(int order)
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
    size_t cent_bin = static_cast<size_t>(m_hists["h_Dummy_Cent"]->FindBin(cent) - 1);
    if (order == 0)
    {
      m_hists["h_Dummy_Cent"]->Fill(cent);
    }

    // ensure centrality is valid
    if (cent_bin >= m_cent_bins)
    {
      std::cout << std::format("Weird Centrality: {}, Skipping Event: {}\n", cent, m_event_data.event_id);
      continue;
    }

    double Q_S_x_2 = m_event_data.sEPD_Q_S_x_2;
    double Q_S_y_2 = m_event_data.sEPD_Q_S_y_2;
    double Q_N_x_2 = m_event_data.sEPD_Q_N_x_2;
    double Q_N_y_2 = m_event_data.sEPD_Q_N_y_2;

    double Q_S_x_3 = m_event_data.sEPD_Q_S_x_3;
    double Q_S_y_3 = m_event_data.sEPD_Q_S_y_3;
    double Q_N_x_3 = m_event_data.sEPD_Q_N_x_3;
    double Q_N_y_3 = m_event_data.sEPD_Q_N_y_3;

    double Q_S_x_4 = m_event_data.sEPD_Q_S_x_4;
    double Q_S_y_4 = m_event_data.sEPD_Q_S_y_4;
    double Q_N_x_4 = m_event_data.sEPD_Q_N_x_4;
    double Q_N_y_4 = m_event_data.sEPD_Q_N_y_4;

    // 1st Order Derive
    if (order == 0)
    {
      m_hists["h_sEPD_Q_S_x_2_avg"]->Fill(cent, Q_S_x_2);
      m_hists["h_sEPD_Q_S_y_2_avg"]->Fill(cent, Q_S_y_2);
      m_hists["h_sEPD_Q_N_x_2_avg"]->Fill(cent, Q_N_x_2);
      m_hists["h_sEPD_Q_N_y_2_avg"]->Fill(cent, Q_N_y_2);

      m_hists["h_sEPD_Q_S_x_3_avg"]->Fill(cent, Q_S_x_3);
      m_hists["h_sEPD_Q_S_y_3_avg"]->Fill(cent, Q_S_y_3);
      m_hists["h_sEPD_Q_N_x_3_avg"]->Fill(cent, Q_N_x_3);
      m_hists["h_sEPD_Q_N_y_3_avg"]->Fill(cent, Q_N_y_3);

      m_hists["h_sEPD_Q_S_x_4_avg"]->Fill(cent, Q_S_x_4);
      m_hists["h_sEPD_Q_S_y_4_avg"]->Fill(cent, Q_S_y_4);
      m_hists["h_sEPD_Q_N_x_4_avg"]->Fill(cent, Q_N_x_4);
      m_hists["h_sEPD_Q_N_y_4_avg"]->Fill(cent, Q_N_y_4);

      dynamic_cast<TH3*>(m_hists["h3_sEPD_Q_S_2"].get())->Fill(Q_S_x_2, Q_S_y_2, cent);
      dynamic_cast<TH3*>(m_hists["h3_sEPD_Q_N_2"].get())->Fill(Q_N_x_2, Q_N_y_2, cent);
      dynamic_cast<TH3*>(m_hists["h3_sEPD_Q_S_3"].get())->Fill(Q_S_x_3, Q_S_y_3, cent);
      dynamic_cast<TH3*>(m_hists["h3_sEPD_Q_N_3"].get())->Fill(Q_N_x_3, Q_N_y_3, cent);
      dynamic_cast<TH3*>(m_hists["h3_sEPD_Q_S_4"].get())->Fill(Q_S_x_4, Q_S_y_4, cent);
      dynamic_cast<TH3*>(m_hists["h3_sEPD_Q_N_4"].get())->Fill(Q_N_x_4, Q_N_y_4, cent);

      // Compute Psi
      double psi_S_2 = std::atan2(Q_S_y_2, Q_S_x_2);
      double psi_N_2 = std::atan2(Q_N_y_2, Q_N_x_2);

      double psi_S_3 = std::atan2(Q_S_y_3, Q_S_x_3);
      double psi_N_3 = std::atan2(Q_N_y_3, Q_N_x_3);

      double psi_S_4 = std::atan2(Q_S_y_4, Q_S_x_4);
      double psi_N_4 = std::atan2(Q_N_y_4, Q_N_x_4);

      dynamic_cast<TH3*>(m_hists["h3_sEPD_Psi_2"].get())->Fill(psi_S_2, psi_N_2, cent);
      dynamic_cast<TH3*>(m_hists["h3_sEPD_Psi_3"].get())->Fill(psi_S_3, psi_N_3, cent);
      dynamic_cast<TH3*>(m_hists["h3_sEPD_Psi_4"].get())->Fill(psi_S_4, psi_N_4, cent);
    }

    if (order == 1)
    {
      double Q_S_x_2_avg = m_hists["h_sEPD_Q_S_x_2_avg"]->GetBinContent(static_cast<int>(cent_bin)+1);
      double Q_S_y_2_avg = m_hists["h_sEPD_Q_S_y_2_avg"]->GetBinContent(static_cast<int>(cent_bin)+1);
      double Q_N_x_2_avg = m_hists["h_sEPD_Q_N_x_2_avg"]->GetBinContent(static_cast<int>(cent_bin)+1);
      double Q_N_y_2_avg = m_hists["h_sEPD_Q_N_y_2_avg"]->GetBinContent(static_cast<int>(cent_bin)+1);

      double Q_S_x_3_avg = m_hists["h_sEPD_Q_S_x_3_avg"]->GetBinContent(static_cast<int>(cent_bin)+1);
      double Q_S_y_3_avg = m_hists["h_sEPD_Q_S_y_3_avg"]->GetBinContent(static_cast<int>(cent_bin)+1);
      double Q_N_x_3_avg = m_hists["h_sEPD_Q_N_x_3_avg"]->GetBinContent(static_cast<int>(cent_bin)+1);
      double Q_N_y_3_avg = m_hists["h_sEPD_Q_N_y_3_avg"]->GetBinContent(static_cast<int>(cent_bin)+1);

      double Q_S_x_4_avg = m_hists["h_sEPD_Q_S_x_4_avg"]->GetBinContent(static_cast<int>(cent_bin)+1);
      double Q_S_y_4_avg = m_hists["h_sEPD_Q_S_y_4_avg"]->GetBinContent(static_cast<int>(cent_bin)+1);
      double Q_N_x_4_avg = m_hists["h_sEPD_Q_N_x_4_avg"]->GetBinContent(static_cast<int>(cent_bin)+1);
      double Q_N_y_4_avg = m_hists["h_sEPD_Q_N_y_4_avg"]->GetBinContent(static_cast<int>(cent_bin)+1);

      // 1st Order Apply
      double Q_S_x_2_corr = Q_S_x_2 - Q_S_x_2_avg;
      double Q_S_y_2_corr = Q_S_y_2 - Q_S_y_2_avg;
      double Q_N_x_2_corr = Q_N_x_2 - Q_N_x_2_avg;
      double Q_N_y_2_corr = Q_N_y_2 - Q_N_y_2_avg;
      double Q_S_x_3_corr = Q_S_x_3 - Q_S_x_3_avg;
      double Q_S_y_3_corr = Q_S_y_3 - Q_S_y_3_avg;
      double Q_N_x_3_corr = Q_N_x_3 - Q_N_x_3_avg;
      double Q_N_y_3_corr = Q_N_y_3 - Q_N_y_3_avg;
      double Q_S_x_4_corr = Q_S_x_4 - Q_S_x_4_avg;
      double Q_S_y_4_corr = Q_S_y_4 - Q_S_y_4_avg;
      double Q_N_x_4_corr = Q_N_x_4 - Q_N_x_4_avg;
      double Q_N_y_4_corr = Q_N_y_4 - Q_N_y_4_avg;

      // 1st Order Validate
      m_hists["h_sEPD_Q_S_x_2_corr_avg"]->Fill(cent, Q_S_x_2_corr);
      m_hists["h_sEPD_Q_S_y_2_corr_avg"]->Fill(cent, Q_S_y_2_corr);
      m_hists["h_sEPD_Q_N_x_2_corr_avg"]->Fill(cent, Q_N_x_2_corr);
      m_hists["h_sEPD_Q_N_y_2_corr_avg"]->Fill(cent, Q_N_y_2_corr);
      m_hists["h_sEPD_Q_S_x_3_corr_avg"]->Fill(cent, Q_S_x_3_corr);
      m_hists["h_sEPD_Q_S_y_3_corr_avg"]->Fill(cent, Q_S_y_3_corr);
      m_hists["h_sEPD_Q_N_x_3_corr_avg"]->Fill(cent, Q_N_x_3_corr);
      m_hists["h_sEPD_Q_N_y_3_corr_avg"]->Fill(cent, Q_N_y_3_corr);
      m_hists["h_sEPD_Q_S_x_4_corr_avg"]->Fill(cent, Q_S_x_4_corr);
      m_hists["h_sEPD_Q_S_y_4_corr_avg"]->Fill(cent, Q_S_y_4_corr);
      m_hists["h_sEPD_Q_N_x_4_corr_avg"]->Fill(cent, Q_N_x_4_corr);
      m_hists["h_sEPD_Q_N_y_4_corr_avg"]->Fill(cent, Q_N_y_4_corr);

      // 2nd Order Derive
      m_hists["h_sEPD_Q_S_xx_2_avg"]->Fill(cent, Q_S_x_2_corr * Q_S_x_2_corr);
      m_hists["h_sEPD_Q_S_yy_2_avg"]->Fill(cent, Q_S_y_2_corr * Q_S_y_2_corr);
      m_hists["h_sEPD_Q_S_xy_2_avg"]->Fill(cent, Q_S_x_2_corr * Q_S_y_2_corr);

      m_hists["h_sEPD_Q_N_xx_2_avg"]->Fill(cent, Q_N_x_2_corr * Q_N_x_2_corr);
      m_hists["h_sEPD_Q_N_yy_2_avg"]->Fill(cent, Q_N_y_2_corr * Q_N_y_2_corr);
      m_hists["h_sEPD_Q_N_xy_2_avg"]->Fill(cent, Q_N_x_2_corr * Q_N_y_2_corr);

      m_hists["h_sEPD_Q_S_xx_3_avg"]->Fill(cent, Q_S_x_3_corr * Q_S_x_3_corr);
      m_hists["h_sEPD_Q_S_yy_3_avg"]->Fill(cent, Q_S_y_3_corr * Q_S_y_3_corr);
      m_hists["h_sEPD_Q_S_xy_3_avg"]->Fill(cent, Q_S_x_3_corr * Q_S_y_3_corr);

      m_hists["h_sEPD_Q_N_xx_3_avg"]->Fill(cent, Q_N_x_3_corr * Q_N_x_3_corr);
      m_hists["h_sEPD_Q_N_yy_3_avg"]->Fill(cent, Q_N_y_3_corr * Q_N_y_3_corr);
      m_hists["h_sEPD_Q_N_xy_3_avg"]->Fill(cent, Q_N_x_3_corr * Q_N_y_3_corr);

      m_hists["h_sEPD_Q_S_xx_4_avg"]->Fill(cent, Q_S_x_4_corr * Q_S_x_4_corr);
      m_hists["h_sEPD_Q_S_yy_4_avg"]->Fill(cent, Q_S_y_4_corr * Q_S_y_4_corr);
      m_hists["h_sEPD_Q_S_xy_4_avg"]->Fill(cent, Q_S_x_4_corr * Q_S_y_4_corr);

      m_hists["h_sEPD_Q_N_xx_4_avg"]->Fill(cent, Q_N_x_4_corr * Q_N_x_4_corr);
      m_hists["h_sEPD_Q_N_yy_4_avg"]->Fill(cent, Q_N_y_4_corr * Q_N_y_4_corr);
      m_hists["h_sEPD_Q_N_xy_4_avg"]->Fill(cent, Q_N_x_4_corr * Q_N_y_4_corr);

      // Compute Psi
      double psi_S_2 = std::atan2(Q_S_y_2_corr, Q_S_x_2_corr);
      double psi_N_2 = std::atan2(Q_N_y_2_corr, Q_N_x_2_corr);

      double psi_S_3 = std::atan2(Q_S_y_3_corr, Q_S_x_3_corr);
      double psi_N_3 = std::atan2(Q_N_y_3_corr, Q_N_x_3_corr);

      double psi_S_4 = std::atan2(Q_S_y_4_corr, Q_S_x_4_corr);
      double psi_N_4 = std::atan2(Q_N_y_4_corr, Q_N_x_4_corr);

      dynamic_cast<TH3*>(m_hists["h3_sEPD_Psi_2_corr"].get())->Fill(psi_S_2, psi_N_2, cent);
      dynamic_cast<TH3*>(m_hists["h3_sEPD_Psi_3_corr"].get())->Fill(psi_S_3, psi_N_3, cent);
      dynamic_cast<TH3*>(m_hists["h3_sEPD_Psi_4_corr"].get())->Fill(psi_S_4, psi_N_4, cent);
    }

    if (order == 2)
    {
      double Q_S_x_2_avg = m_hists["h_sEPD_Q_S_x_2_avg"]->GetBinContent(static_cast<int>(cent_bin)+1);
      double Q_S_y_2_avg = m_hists["h_sEPD_Q_S_y_2_avg"]->GetBinContent(static_cast<int>(cent_bin)+1);
      double Q_N_x_2_avg = m_hists["h_sEPD_Q_N_x_2_avg"]->GetBinContent(static_cast<int>(cent_bin)+1);
      double Q_N_y_2_avg = m_hists["h_sEPD_Q_N_y_2_avg"]->GetBinContent(static_cast<int>(cent_bin)+1);

      double Q_S_x_3_avg = m_hists["h_sEPD_Q_S_x_3_avg"]->GetBinContent(static_cast<int>(cent_bin)+1);
      double Q_S_y_3_avg = m_hists["h_sEPD_Q_S_y_3_avg"]->GetBinContent(static_cast<int>(cent_bin)+1);
      double Q_N_x_3_avg = m_hists["h_sEPD_Q_N_x_3_avg"]->GetBinContent(static_cast<int>(cent_bin)+1);
      double Q_N_y_3_avg = m_hists["h_sEPD_Q_N_y_3_avg"]->GetBinContent(static_cast<int>(cent_bin)+1);

      double Q_S_x_4_avg = m_hists["h_sEPD_Q_S_x_4_avg"]->GetBinContent(static_cast<int>(cent_bin)+1);
      double Q_S_y_4_avg = m_hists["h_sEPD_Q_S_y_4_avg"]->GetBinContent(static_cast<int>(cent_bin)+1);
      double Q_N_x_4_avg = m_hists["h_sEPD_Q_N_x_4_avg"]->GetBinContent(static_cast<int>(cent_bin)+1);
      double Q_N_y_4_avg = m_hists["h_sEPD_Q_N_y_4_avg"]->GetBinContent(static_cast<int>(cent_bin)+1);

      // 1st Order Apply
      double Q_S_x_2_corr = Q_S_x_2 - Q_S_x_2_avg;
      double Q_S_y_2_corr = Q_S_y_2 - Q_S_y_2_avg;
      double Q_N_x_2_corr = Q_N_x_2 - Q_N_x_2_avg;
      double Q_N_y_2_corr = Q_N_y_2 - Q_N_y_2_avg;

      double Q_S_x_3_corr = Q_S_x_3 - Q_S_x_3_avg;
      double Q_S_y_3_corr = Q_S_y_3 - Q_S_y_3_avg;
      double Q_N_x_3_corr = Q_N_x_3 - Q_N_x_3_avg;
      double Q_N_y_3_corr = Q_N_y_3 - Q_N_y_3_avg;

      double Q_S_x_4_corr = Q_S_x_4 - Q_S_x_4_avg;
      double Q_S_y_4_corr = Q_S_y_4 - Q_S_y_4_avg;
      double Q_N_x_4_corr = Q_N_x_4 - Q_N_x_4_avg;
      double Q_N_y_4_corr = Q_N_y_4 - Q_N_y_4_avg;

      // 2nd Order Apply
      double Q_S_x_2_corr2 = m_X_S_2[cent_bin][0][0] * Q_S_x_2_corr + m_X_S_2[cent_bin][0][1] * Q_S_y_2_corr;
      double Q_S_y_2_corr2 = m_X_S_2[cent_bin][1][0] * Q_S_x_2_corr + m_X_S_2[cent_bin][1][1] * Q_S_y_2_corr;
      double Q_N_x_2_corr2 = m_X_N_2[cent_bin][0][0] * Q_N_x_2_corr + m_X_N_2[cent_bin][0][1] * Q_N_y_2_corr;
      double Q_N_y_2_corr2 = m_X_N_2[cent_bin][1][0] * Q_N_x_2_corr + m_X_N_2[cent_bin][1][1] * Q_N_y_2_corr;

      double Q_S_x_3_corr2 = m_X_S_3[cent_bin][0][0] * Q_S_x_3_corr + m_X_S_3[cent_bin][0][1] * Q_S_y_3_corr;
      double Q_S_y_3_corr2 = m_X_S_3[cent_bin][1][0] * Q_S_x_3_corr + m_X_S_3[cent_bin][1][1] * Q_S_y_3_corr;
      double Q_N_x_3_corr2 = m_X_N_3[cent_bin][0][0] * Q_N_x_3_corr + m_X_N_3[cent_bin][0][1] * Q_N_y_3_corr;
      double Q_N_y_3_corr2 = m_X_N_3[cent_bin][1][0] * Q_N_x_3_corr + m_X_N_3[cent_bin][1][1] * Q_N_y_3_corr;

      double Q_S_x_4_corr2 = m_X_S_4[cent_bin][0][0] * Q_S_x_4_corr + m_X_S_4[cent_bin][0][1] * Q_S_y_4_corr;
      double Q_S_y_4_corr2 = m_X_S_4[cent_bin][1][0] * Q_S_x_4_corr + m_X_S_4[cent_bin][1][1] * Q_S_y_4_corr;
      double Q_N_x_4_corr2 = m_X_N_4[cent_bin][0][0] * Q_N_x_4_corr + m_X_N_4[cent_bin][0][1] * Q_N_y_4_corr;
      double Q_N_y_4_corr2 = m_X_N_4[cent_bin][1][0] * Q_N_x_4_corr + m_X_N_4[cent_bin][1][1] * Q_N_y_4_corr;

      // 2nd Order Validate
      m_hists["h_sEPD_Q_S_xx_2_corr_avg"]->Fill(cent, Q_S_x_2_corr2 * Q_S_x_2_corr2);
      m_hists["h_sEPD_Q_S_yy_2_corr_avg"]->Fill(cent, Q_S_y_2_corr2 * Q_S_y_2_corr2);
      m_hists["h_sEPD_Q_S_xy_2_corr_avg"]->Fill(cent, Q_S_x_2_corr2 * Q_S_y_2_corr2);

      m_hists["h_sEPD_Q_N_xx_2_corr_avg"]->Fill(cent, Q_N_x_2_corr2 * Q_N_x_2_corr2);
      m_hists["h_sEPD_Q_N_yy_2_corr_avg"]->Fill(cent, Q_N_y_2_corr2 * Q_N_y_2_corr2);
      m_hists["h_sEPD_Q_N_xy_2_corr_avg"]->Fill(cent, Q_N_x_2_corr2 * Q_N_y_2_corr2);

      m_hists["h_sEPD_Q_S_xx_3_corr_avg"]->Fill(cent, Q_S_x_3_corr2 * Q_S_x_3_corr2);
      m_hists["h_sEPD_Q_S_yy_3_corr_avg"]->Fill(cent, Q_S_y_3_corr2 * Q_S_y_3_corr2);
      m_hists["h_sEPD_Q_S_xy_3_corr_avg"]->Fill(cent, Q_S_x_3_corr2 * Q_S_y_3_corr2);

      m_hists["h_sEPD_Q_N_xx_3_corr_avg"]->Fill(cent, Q_N_x_3_corr2 * Q_N_x_3_corr2);
      m_hists["h_sEPD_Q_N_yy_3_corr_avg"]->Fill(cent, Q_N_y_3_corr2 * Q_N_y_3_corr2);
      m_hists["h_sEPD_Q_N_xy_3_corr_avg"]->Fill(cent, Q_N_x_3_corr2 * Q_N_y_3_corr2);

      m_hists["h_sEPD_Q_S_xx_4_corr_avg"]->Fill(cent, Q_S_x_4_corr2 * Q_S_x_4_corr2);
      m_hists["h_sEPD_Q_S_yy_4_corr_avg"]->Fill(cent, Q_S_y_4_corr2 * Q_S_y_4_corr2);
      m_hists["h_sEPD_Q_S_xy_4_corr_avg"]->Fill(cent, Q_S_x_4_corr2 * Q_S_y_4_corr2);

      m_hists["h_sEPD_Q_N_xx_4_corr_avg"]->Fill(cent, Q_N_x_4_corr2 * Q_N_x_4_corr2);
      m_hists["h_sEPD_Q_N_yy_4_corr_avg"]->Fill(cent, Q_N_y_4_corr2 * Q_N_y_4_corr2);
      m_hists["h_sEPD_Q_N_xy_4_corr_avg"]->Fill(cent, Q_N_x_4_corr2 * Q_N_y_4_corr2);

      // Compute Psi
      double psi_S_2 = std::atan2(Q_S_y_2_corr2, Q_S_x_2_corr2);
      double psi_N_2 = std::atan2(Q_N_y_2_corr2, Q_N_x_2_corr2);

      double psi_S_3 = std::atan2(Q_S_y_3_corr2, Q_S_x_3_corr2);
      double psi_N_3 = std::atan2(Q_N_y_3_corr2, Q_N_x_3_corr2);

      double psi_S_4 = std::atan2(Q_S_y_4_corr2, Q_S_x_4_corr2);
      double psi_N_4 = std::atan2(Q_N_y_4_corr2, Q_N_x_4_corr2);

      dynamic_cast<TH3*>(m_hists["h3_sEPD_Psi_2_corr2"].get())->Fill(psi_S_2, psi_N_2, cent);
      dynamic_cast<TH3*>(m_hists["h3_sEPD_Psi_3_corr2"].get())->Fill(psi_S_3, psi_N_3, cent);
      dynamic_cast<TH3*>(m_hists["h3_sEPD_Psi_4_corr2"].get())->Fill(psi_S_4, psi_N_4, cent);
    }
  }

  std::cout << std::format("{:#<20}\n", "");
  std::cout << "Event Stats" << std::endl;
  std::cout << std::format("Events with sEPD South Charge 0: {}\n", ctr["sepd_south_zero_charge"]);
  std::cout << std::format("Events with sEPD North Charge 0: {}\n", ctr["sepd_north_zero_charge"]);
  std::cout << std::format("{:#<20}\n", "");
  if (order == 0)
  {
    std::cout << std::format("Q Vector First Order Correction Averages\n");
    for (int cent_bin = 0; cent_bin < static_cast<int>(m_cent_bins); ++cent_bin)
    {
      double Q_S_x_2_avg = m_hists["h_sEPD_Q_S_x_2_avg"]->GetBinContent(cent_bin + 1);
      double Q_S_y_2_avg = m_hists["h_sEPD_Q_S_y_2_avg"]->GetBinContent(cent_bin + 1);
      double Q_N_x_2_avg = m_hists["h_sEPD_Q_N_x_2_avg"]->GetBinContent(cent_bin + 1);
      double Q_N_y_2_avg = m_hists["h_sEPD_Q_N_y_2_avg"]->GetBinContent(cent_bin + 1);

      double Q_S_x_3_avg = m_hists["h_sEPD_Q_S_x_3_avg"]->GetBinContent(cent_bin + 1);
      double Q_S_y_3_avg = m_hists["h_sEPD_Q_S_y_3_avg"]->GetBinContent(cent_bin + 1);
      double Q_N_x_3_avg = m_hists["h_sEPD_Q_N_x_3_avg"]->GetBinContent(cent_bin + 1);
      double Q_N_y_3_avg = m_hists["h_sEPD_Q_N_y_3_avg"]->GetBinContent(cent_bin + 1);

      double Q_S_x_4_avg = m_hists["h_sEPD_Q_S_x_4_avg"]->GetBinContent(cent_bin + 1);
      double Q_S_y_4_avg = m_hists["h_sEPD_Q_S_y_4_avg"]->GetBinContent(cent_bin + 1);
      double Q_N_x_4_avg = m_hists["h_sEPD_Q_N_x_4_avg"]->GetBinContent(cent_bin + 1);
      double Q_N_y_4_avg = m_hists["h_sEPD_Q_N_y_4_avg"]->GetBinContent(cent_bin + 1);

      std::cout << std::format(
          "cent_bin: {:2}, "
          "Q_S_x_2_avg: {:7.4f}, "
          "Q_S_y_2_avg: {:7.4f}, "
          "Q_N_x_2_avg: {:7.4f}, "
          "Q_N_y_2_avg: {:7.4f}, "
          "Q_S_x_3_avg: {:7.4f}, "
          "Q_S_y_3_avg: {:7.4f}, "
          "Q_N_x_3_avg: {:7.4f}, "
          "Q_N_y_3_avg: {:7.4f}, "
          "Q_S_x_4_avg: {:7.4f}, "
          "Q_S_y_4_avg: {:7.4f}, "
          "Q_N_x_4_avg: {:7.4f}, "
          "Q_N_y_4_avg: {:7.4f}\n",
          cent_bin,
          Q_S_x_2_avg,
          Q_S_y_2_avg,
          Q_N_x_2_avg,
          Q_N_y_2_avg,
          Q_S_x_3_avg,
          Q_S_y_3_avg,
          Q_N_x_3_avg,
          Q_N_y_3_avg,
          Q_S_x_4_avg,
          Q_S_y_4_avg,
          Q_N_x_4_avg,
          Q_N_y_4_avg);
    }
  }

  if (order == 1)
  {
    std::cout << std::format("Q Vector First Order Correction Validation\n");
    for (int cent_bin = 0; cent_bin < static_cast<int>(m_cent_bins); ++cent_bin)
    {
      // Order 1 Validate
      double Q_S_x_2_corr_avg = m_hists["h_sEPD_Q_S_x_2_corr_avg"]->GetBinContent(cent_bin + 1);
      double Q_S_y_2_corr_avg = m_hists["h_sEPD_Q_S_y_2_corr_avg"]->GetBinContent(cent_bin + 1);
      double Q_N_x_2_corr_avg = m_hists["h_sEPD_Q_N_x_2_corr_avg"]->GetBinContent(cent_bin + 1);
      double Q_N_y_2_corr_avg = m_hists["h_sEPD_Q_N_y_2_corr_avg"]->GetBinContent(cent_bin + 1);

      double Q_S_x_3_corr_avg = m_hists["h_sEPD_Q_S_x_3_corr_avg"]->GetBinContent(cent_bin + 1);
      double Q_S_y_3_corr_avg = m_hists["h_sEPD_Q_S_y_3_corr_avg"]->GetBinContent(cent_bin + 1);
      double Q_N_x_3_corr_avg = m_hists["h_sEPD_Q_N_x_3_corr_avg"]->GetBinContent(cent_bin + 1);
      double Q_N_y_3_corr_avg = m_hists["h_sEPD_Q_N_y_3_corr_avg"]->GetBinContent(cent_bin + 1);

      double Q_S_x_4_corr_avg = m_hists["h_sEPD_Q_S_x_4_corr_avg"]->GetBinContent(cent_bin + 1);
      double Q_S_y_4_corr_avg = m_hists["h_sEPD_Q_S_y_4_corr_avg"]->GetBinContent(cent_bin + 1);
      double Q_N_x_4_corr_avg = m_hists["h_sEPD_Q_N_x_4_corr_avg"]->GetBinContent(cent_bin + 1);
      double Q_N_y_4_corr_avg = m_hists["h_sEPD_Q_N_y_4_corr_avg"]->GetBinContent(cent_bin + 1);

      std::cout << std::format(
          "cent_bin: {:2}, "
          "Q_S_x_2_corr_avg: {:7.4f}, "
          "Q_S_y_2_corr_avg: {:7.4f}, "
          "Q_N_x_2_corr_avg: {:7.4f}, "
          "Q_N_y_2_corr_avg: {:7.4f}, "
          "Q_S_x_3_corr_avg: {:7.4f}, "
          "Q_S_y_3_corr_avg: {:7.4f}, "
          "Q_N_x_3_corr_avg: {:7.4f}, "
          "Q_N_y_3_corr_avg: {:7.4f}, "
          "Q_S_x_4_corr_avg: {:7.4f}, "
          "Q_S_y_4_corr_avg: {:7.4f}, "
          "Q_N_x_4_corr_avg: {:7.4f}, "
          "Q_N_y_4_corr_avg: {:7.4f}\n",
          cent_bin,
          Q_S_x_2_corr_avg,
          Q_S_y_2_corr_avg,
          Q_N_x_2_corr_avg,
          Q_N_y_2_corr_avg,
          Q_S_x_3_corr_avg,
          Q_S_y_3_corr_avg,
          Q_N_x_3_corr_avg,
          Q_N_y_3_corr_avg,
          Q_S_x_4_corr_avg,
          Q_S_y_4_corr_avg,
          Q_N_x_4_corr_avg,
          Q_N_y_4_corr_avg);
    }

    std::cout << std::format("{:#<20}\n", "");
    std::cout << std::format("Q Vector Second Order Correction Averages\n");
    for (size_t cent_bin = 0; cent_bin < m_cent_bins; ++cent_bin)
    {
      double Q_S_xx_2_avg = m_hists["h_sEPD_Q_S_xx_2_avg"]->GetBinContent(static_cast<int>(cent_bin) + 1);
      double Q_S_yy_2_avg = m_hists["h_sEPD_Q_S_yy_2_avg"]->GetBinContent(static_cast<int>(cent_bin) + 1);
      double Q_S_xy_2_avg = m_hists["h_sEPD_Q_S_xy_2_avg"]->GetBinContent(static_cast<int>(cent_bin) + 1);

      double Q_N_xx_2_avg = m_hists["h_sEPD_Q_N_xx_2_avg"]->GetBinContent(static_cast<int>(cent_bin) + 1);
      double Q_N_yy_2_avg = m_hists["h_sEPD_Q_N_yy_2_avg"]->GetBinContent(static_cast<int>(cent_bin) + 1);
      double Q_N_xy_2_avg = m_hists["h_sEPD_Q_N_xy_2_avg"]->GetBinContent(static_cast<int>(cent_bin) + 1);

      double Q_S_xx_3_avg = m_hists["h_sEPD_Q_S_xx_3_avg"]->GetBinContent(static_cast<int>(cent_bin) + 1);
      double Q_S_yy_3_avg = m_hists["h_sEPD_Q_S_yy_3_avg"]->GetBinContent(static_cast<int>(cent_bin) + 1);
      double Q_S_xy_3_avg = m_hists["h_sEPD_Q_S_xy_3_avg"]->GetBinContent(static_cast<int>(cent_bin) + 1);

      double Q_N_xx_3_avg = m_hists["h_sEPD_Q_N_xx_3_avg"]->GetBinContent(static_cast<int>(cent_bin) + 1);
      double Q_N_yy_3_avg = m_hists["h_sEPD_Q_N_yy_3_avg"]->GetBinContent(static_cast<int>(cent_bin) + 1);
      double Q_N_xy_3_avg = m_hists["h_sEPD_Q_N_xy_3_avg"]->GetBinContent(static_cast<int>(cent_bin) + 1);

      double Q_S_xx_4_avg = m_hists["h_sEPD_Q_S_xx_4_avg"]->GetBinContent(static_cast<int>(cent_bin) + 1);
      double Q_S_yy_4_avg = m_hists["h_sEPD_Q_S_yy_4_avg"]->GetBinContent(static_cast<int>(cent_bin) + 1);
      double Q_S_xy_4_avg = m_hists["h_sEPD_Q_S_xy_4_avg"]->GetBinContent(static_cast<int>(cent_bin) + 1);

      double Q_N_xx_4_avg = m_hists["h_sEPD_Q_N_xx_4_avg"]->GetBinContent(static_cast<int>(cent_bin) + 1);
      double Q_N_yy_4_avg = m_hists["h_sEPD_Q_N_yy_4_avg"]->GetBinContent(static_cast<int>(cent_bin) + 1);
      double Q_N_xy_4_avg = m_hists["h_sEPD_Q_N_xy_4_avg"]->GetBinContent(static_cast<int>(cent_bin) + 1);

      // Compute N and D terms
      m_D_S_2[cent_bin] = std::sqrt((Q_S_xx_2_avg * Q_S_yy_2_avg) - (Q_S_xy_2_avg * Q_S_xy_2_avg));
      m_D_N_2[cent_bin] = std::sqrt((Q_N_xx_2_avg * Q_N_yy_2_avg) - (Q_N_xy_2_avg * Q_N_xy_2_avg));

      m_D_S_3[cent_bin] = std::sqrt((Q_S_xx_3_avg * Q_S_yy_3_avg) - (Q_S_xy_3_avg * Q_S_xy_3_avg));
      m_D_N_3[cent_bin] = std::sqrt((Q_N_xx_3_avg * Q_N_yy_3_avg) - (Q_N_xy_3_avg * Q_N_xy_3_avg));

      m_D_S_4[cent_bin] = std::sqrt((Q_S_xx_4_avg * Q_S_yy_4_avg) - (Q_S_xy_4_avg * Q_S_xy_4_avg));
      m_D_N_4[cent_bin] = std::sqrt((Q_N_xx_4_avg * Q_N_yy_4_avg) - (Q_N_xy_4_avg * Q_N_xy_4_avg));

      m_N_S_2[cent_bin] = m_D_S_2[cent_bin] * (Q_S_xx_2_avg + Q_S_yy_2_avg + (2 * m_D_S_2[cent_bin]));
      m_N_N_2[cent_bin] = m_D_N_2[cent_bin] * (Q_N_xx_2_avg + Q_N_yy_2_avg + (2 * m_D_N_2[cent_bin]));

      m_N_S_3[cent_bin] = m_D_S_3[cent_bin] * (Q_S_xx_3_avg + Q_S_yy_3_avg + (2 * m_D_S_3[cent_bin]));
      m_N_N_3[cent_bin] = m_D_N_3[cent_bin] * (Q_N_xx_3_avg + Q_N_yy_3_avg + (2 * m_D_N_3[cent_bin]));

      m_N_S_4[cent_bin] = m_D_S_4[cent_bin] * (Q_S_xx_4_avg + Q_S_yy_4_avg + (2 * m_D_S_4[cent_bin]));
      m_N_N_4[cent_bin] = m_D_N_4[cent_bin] * (Q_N_xx_4_avg + Q_N_yy_4_avg + (2 * m_D_N_4[cent_bin]));

      // Compute matrix elements
      m_X_S_2[cent_bin][0][0] = (1. / std::sqrt(m_N_S_2[cent_bin])) * (Q_S_yy_2_avg + m_D_S_2[cent_bin]);
      m_X_S_2[cent_bin][0][1] = (-1. / std::sqrt(m_N_S_2[cent_bin])) * Q_S_xy_2_avg;
      m_X_S_2[cent_bin][1][0] = m_X_S_2[cent_bin][0][1];
      m_X_S_2[cent_bin][1][1] = (1. / std::sqrt(m_N_S_2[cent_bin])) * (Q_S_xx_2_avg + m_D_S_2[cent_bin]);

      m_X_N_2[cent_bin][0][0] = (1. / std::sqrt(m_N_N_2[cent_bin])) * (Q_N_yy_2_avg + m_D_N_2[cent_bin]);
      m_X_N_2[cent_bin][0][1] = (-1. / std::sqrt(m_N_N_2[cent_bin])) * Q_N_xy_2_avg;
      m_X_N_2[cent_bin][1][0] = m_X_N_2[cent_bin][0][1];
      m_X_N_2[cent_bin][1][1] = (1. / std::sqrt(m_N_N_2[cent_bin])) * (Q_N_xx_2_avg + m_D_N_2[cent_bin]);

      m_X_S_3[cent_bin][0][0] = (1. / std::sqrt(m_N_S_3[cent_bin])) * (Q_S_yy_3_avg + m_D_S_3[cent_bin]);
      m_X_S_3[cent_bin][0][1] = (-1. / std::sqrt(m_N_S_3[cent_bin])) * Q_S_xy_3_avg;
      m_X_S_3[cent_bin][1][0] = m_X_S_3[cent_bin][0][1];
      m_X_S_3[cent_bin][1][1] = (1. / std::sqrt(m_N_S_3[cent_bin])) * (Q_S_xx_3_avg + m_D_S_3[cent_bin]);

      m_X_N_3[cent_bin][0][0] = (1. / std::sqrt(m_N_N_3[cent_bin])) * (Q_N_yy_3_avg + m_D_N_3[cent_bin]);
      m_X_N_3[cent_bin][0][1] = (-1. / std::sqrt(m_N_N_3[cent_bin])) * Q_N_xy_3_avg;
      m_X_N_3[cent_bin][1][0] = m_X_N_3[cent_bin][0][1];
      m_X_N_3[cent_bin][1][1] = (1. / std::sqrt(m_N_N_3[cent_bin])) * (Q_N_xx_3_avg + m_D_N_3[cent_bin]);

      m_X_S_4[cent_bin][0][0] = (1. / std::sqrt(m_N_S_4[cent_bin])) * (Q_S_yy_4_avg + m_D_S_4[cent_bin]);
      m_X_S_4[cent_bin][0][1] = (-1. / std::sqrt(m_N_S_4[cent_bin])) * Q_S_xy_4_avg;
      m_X_S_4[cent_bin][1][0] = m_X_S_4[cent_bin][0][1];
      m_X_S_4[cent_bin][1][1] = (1. / std::sqrt(m_N_S_4[cent_bin])) * (Q_S_xx_4_avg + m_D_S_4[cent_bin]);

      m_X_N_4[cent_bin][0][0] = (1. / std::sqrt(m_N_N_4[cent_bin])) * (Q_N_yy_4_avg + m_D_N_4[cent_bin]);
      m_X_N_4[cent_bin][0][1] = (-1. / std::sqrt(m_N_N_4[cent_bin])) * Q_N_xy_4_avg;
      m_X_N_4[cent_bin][1][0] = m_X_N_4[cent_bin][0][1];
      m_X_N_4[cent_bin][1][1] = (1. / std::sqrt(m_N_N_4[cent_bin])) * (Q_N_xx_4_avg + m_D_N_4[cent_bin]);

      std::cout << std::format(
          "cent_bin: {:2}, "
          "Q_S_xx_2_avg: {:7.4f}, "
          "Q_S_yy_2_avg: {:7.4f}, "
          "Q_S_xy_2_avg: {:7.4f}, "
          "Q_N_xx_2_avg: {:7.4f}, "
          "Q_N_yy_2_avg: {:7.4f}, "
          "Q_N_xy_2_avg: {:7.4f}, "
          "Q_S_xx_3_avg: {:7.4f}, "
          "Q_S_yy_3_avg: {:7.4f}, "
          "Q_S_xy_3_avg: {:7.4f}, "
          "Q_N_xx_3_avg: {:7.4f}, "
          "Q_N_yy_3_avg: {:7.4f}, "
          "Q_N_xy_3_avg: {:7.4f}, "
          "Q_S_xx_4_avg: {:7.4f}, "
          "Q_S_yy_4_avg: {:7.4f}, "
          "Q_S_xy_4_avg: {:7.4f}, "
          "Q_N_xx_4_avg: {:7.4f}, "
          "Q_N_yy_4_avg: {:7.4f}, "
          "Q_N_xy_4_avg: {:7.4f}\n",
          cent_bin,
          Q_S_xx_2_avg,
          Q_S_yy_2_avg,
          Q_S_xy_2_avg,
          Q_N_xx_2_avg,
          Q_N_yy_2_avg,
          Q_N_xy_2_avg,
          Q_S_xx_3_avg,
          Q_S_yy_3_avg,
          Q_S_xy_3_avg,
          Q_N_xx_3_avg,
          Q_N_yy_3_avg,
          Q_N_xy_3_avg,
          Q_S_xx_4_avg,
          Q_S_yy_4_avg,
          Q_S_xy_4_avg,
          Q_N_xx_4_avg,
          Q_N_yy_4_avg,
          Q_N_xy_4_avg);
    }
  }

  if (order == 2)
  {
    std::cout << std::format("{:#<20}\n", "");
    std::cout << std::format("Q Vector Second Order Correction Validation\n");
    for (int cent_bin = 0; cent_bin < static_cast<int>(m_cent_bins); ++cent_bin)
    {
      double Q_S_xx_2_corr_avg = m_hists["h_sEPD_Q_S_xx_2_corr_avg"]->GetBinContent(cent_bin + 1);
      double Q_S_yy_2_corr_avg = m_hists["h_sEPD_Q_S_yy_2_corr_avg"]->GetBinContent(cent_bin + 1);
      double Q_S_xy_2_corr_avg = m_hists["h_sEPD_Q_S_xy_2_corr_avg"]->GetBinContent(cent_bin + 1);

      double Q_N_xx_2_corr_avg = m_hists["h_sEPD_Q_N_xx_2_corr_avg"]->GetBinContent(cent_bin + 1);
      double Q_N_yy_2_corr_avg = m_hists["h_sEPD_Q_N_yy_2_corr_avg"]->GetBinContent(cent_bin + 1);
      double Q_N_xy_2_corr_avg = m_hists["h_sEPD_Q_N_xy_2_corr_avg"]->GetBinContent(cent_bin + 1);

      double Q_S_xx_3_corr_avg = m_hists["h_sEPD_Q_S_xx_3_corr_avg"]->GetBinContent(cent_bin + 1);
      double Q_S_yy_3_corr_avg = m_hists["h_sEPD_Q_S_yy_3_corr_avg"]->GetBinContent(cent_bin + 1);
      double Q_S_xy_3_corr_avg = m_hists["h_sEPD_Q_S_xy_3_corr_avg"]->GetBinContent(cent_bin + 1);

      double Q_N_xx_3_corr_avg = m_hists["h_sEPD_Q_N_xx_3_corr_avg"]->GetBinContent(cent_bin + 1);
      double Q_N_yy_3_corr_avg = m_hists["h_sEPD_Q_N_yy_3_corr_avg"]->GetBinContent(cent_bin + 1);
      double Q_N_xy_3_corr_avg = m_hists["h_sEPD_Q_N_xy_3_corr_avg"]->GetBinContent(cent_bin + 1);

      double Q_S_xx_4_corr_avg = m_hists["h_sEPD_Q_S_xx_4_corr_avg"]->GetBinContent(cent_bin + 1);
      double Q_S_yy_4_corr_avg = m_hists["h_sEPD_Q_S_yy_4_corr_avg"]->GetBinContent(cent_bin + 1);
      double Q_S_xy_4_corr_avg = m_hists["h_sEPD_Q_S_xy_4_corr_avg"]->GetBinContent(cent_bin + 1);

      double Q_N_xx_4_corr_avg = m_hists["h_sEPD_Q_N_xx_4_corr_avg"]->GetBinContent(cent_bin + 1);
      double Q_N_yy_4_corr_avg = m_hists["h_sEPD_Q_N_yy_4_corr_avg"]->GetBinContent(cent_bin + 1);
      double Q_N_xy_4_corr_avg = m_hists["h_sEPD_Q_N_xy_4_corr_avg"]->GetBinContent(cent_bin + 1);

      std::cout << std::format(
          "cent_bin: {:2}, "
          "Q_S_xx_2_corr_avg: {:7.4f}, "
          "Q_S_yy_2_corr_avg: {:7.4f}, "
          "Q_N_xx_2_corr_avg: {:7.4f}, "
          "Q_N_yy_2_corr_avg: {:7.4f}, "
          "Q_S_xx_3_corr_avg: {:7.4f}, "
          "Q_S_yy_3_corr_avg: {:7.4f}, "
          "Q_N_xx_3_corr_avg: {:7.4f}, "
          "Q_N_yy_3_corr_avg: {:7.4f}, "
          "Q_S_xx_4_corr_avg: {:7.4f}, "
          "Q_S_yy_4_corr_avg: {:7.4f}, "
          "Q_N_xx_4_corr_avg: {:7.4f}, "
          "Q_N_yy_4_corr_avg: {:7.4f}, "
          "Q_S_xy_2_corr_avg: {:7.4f}, "
          "Q_N_xy_2_corr_avg: {:7.4f}, "
          "Q_S_xy_3_corr_avg: {:7.4f}, "
          "Q_N_xy_3_corr_avg: {:7.4f}, "
          "Q_S_xy_4_corr_avg: {:7.4f}, "
          "Q_N_xy_4_corr_avg: {:7.4f}\n",
          cent_bin,
          Q_S_xx_2_corr_avg,
          Q_S_yy_2_corr_avg,
          Q_N_xx_2_corr_avg,
          Q_N_yy_2_corr_avg,
          Q_S_xx_3_corr_avg,
          Q_S_yy_3_corr_avg,
          Q_N_xx_3_corr_avg,
          Q_N_yy_3_corr_avg,
          Q_S_xx_4_corr_avg,
          Q_S_yy_4_corr_avg,
          Q_N_xx_4_corr_avg,
          Q_N_yy_4_corr_avg,
          Q_S_xy_2_corr_avg,
          Q_N_xy_2_corr_avg,
          Q_S_xy_3_corr_avg,
          Q_N_xy_3_corr_avg,
          Q_S_xy_4_corr_avg,
          Q_N_xy_4_corr_avg);
    }
  }

  std::cout << "Event loop finished." << std::endl;
}

void QvectorAnalysis::save_results()
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
