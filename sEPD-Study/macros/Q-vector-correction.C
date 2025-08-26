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
  {
  }

  void run()
  {
    setup_chain();
    run_event_loop();
    save_results();
  }

 private:
  struct EventData
  {
    int event_id{0};
    double event_zvertex{0.0};
    double event_centrality{0.0};
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

  // --- Private Helper Methods ---
  void setup_chain();
  void run_event_loop();
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
  while (std::getline(file_list, root_file_path))
  {
    if (!root_file_path.empty())
    {
      m_chain->Add(root_file_path.c_str());

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
  m_chain->SetBranchStatus("sepd_charge", true);
  m_chain->SetBranchStatus("sepd_phi", true);
  m_chain->SetBranchStatus("sepd_eta", true);
  // m_chain->SetBranchStatus("mbd_charge", true);
  // m_chain->SetBranchStatus("mbd_phi", true);
  // m_chain->SetBranchStatus("mbd_eta", true);

  m_chain->SetBranchAddress("event_id", &m_event_data.event_id);
  m_chain->SetBranchAddress("event_centrality", &m_event_data.event_centrality);
  m_chain->SetBranchAddress("sepd_charge", &m_event_data.sepd_charge);
  m_chain->SetBranchAddress("sepd_phi", &m_event_data.sepd_phi);
  m_chain->SetBranchAddress("sepd_eta", &m_event_data.sepd_eta);
  // m_chain->SetBranchAddress("mbd_charge", &m_event_data.mbd_charge);
  // m_chain->SetBranchAddress("mbd_phi", &m_event_data.mbd_phi);
  // m_chain->SetBranchAddress("mbd_eta", &m_event_data.mbd_eta);
}

void QvectorAnalysis::run_event_loop()
{
  long long n_entries = m_chain->GetEntries();
  if (m_events_to_process)
  {
    n_entries = std::min(m_events_to_process, n_entries);
  }

  std::map<std::string, int> ctr;
  ctr["prints_sepd_const"] = 5;

  for (long long i = 0; i < n_entries; ++i)
  {
    m_chain->GetEntry(i);
    if (i % 1000 == 0)
    {
      std::cout << std::format("Processing entry {}/{}: {:.2f} %", i, n_entries, static_cast<double>(i) * 100. / static_cast<double>(n_entries)) << std::endl;
    }

    if (m_event_data.sepd_charge && m_event_data.sepd_phi && m_event_data.sepd_eta)
    {
      size_t n_towers_sepd = m_event_data.sepd_charge->size();

      ctr["prints_sepd"] = 0;
      for (size_t j = 0; j < n_towers_sepd; ++j)
      {
        double charge = m_event_data.sepd_charge->at(j);
        double phi = m_event_data.sepd_phi->at(j);
        double eta = m_event_data.sepd_eta->at(j);

        if (m_verbosity && ++ctr["prints_sepd"] < ctr["prints_sepd_const"])
        {
          std::cout << std::format("id: {:3d}, Z = {:5.2f}, Cent: {:2d}, Charge: {:5.2f}, Phi: {:3.2f}, Eta: {:3.2f}", m_event_data.event_id, m_event_data.event_zvertex, static_cast<int>(m_event_data.event_centrality), charge, phi, eta) << std::endl;
        }
      }
    }
  }
  std::cout << "Event loop finished." << std::endl;
}

void QvectorAnalysis::save_results()
{
  std::filesystem::create_directories(m_output_dir);
  // std::string output_filename = output_dir + "/analysis_output.root";
  // auto output_file = std::make_unique<TFile>(output_filename.c_str(), "RECREATE");

  // h_centrality->Write();
  // h_jet_pt->Write();

  // output_file->Close();
  // std::cout << std::format("Results saved to: {}", m_output_dir) << std::endl;
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
