// -- utils
#include "myUtils.C"

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
#include <TH2.h>
#include <TROOT.h>
#include <TTree.h>

// ====================================================================
// The Analysis Class
// ====================================================================
class sEPDQA
{
 public:
  sEPDQA(std::string input_file, long long events, std::string output_dir)
    : m_input_file(std::move(input_file))
    , m_events_to_process(events)
    , m_output_dir(std::move(output_dir))
  {
  }

  void run()
  {
    setup_chain();
    init_hists();
    process_events();
    save_results();
  }

  void set_verbosity(int verbosity) { m_verbosity = verbosity; }
  int get_verbosity() const { return m_verbosity; }

 private:
  struct AnalysisHists
  {
    TH2 *h2sEPD_Centrality{nullptr};
    TH2 *h2sEPD_MBD{nullptr};
    TH2 *h2sEPD_CaloE{nullptr};
  };

  AnalysisHists m_hists;

  struct EventData
  {
    int event{0};
    double zvtx{0.0};
    double centrality{0.0};
    double emcal_energy{0.0};
    double ihcal_energy{0.0};
    double ohcal_energy{0.0};

    double sepd_charge_south{0.0};
    double sepd_charge_north{0.0};

    double mbd_charge_south{0.0};
    double mbd_charge_north{0.0};
  };

  EventData m_event_data;

  std::string m_input_file;
  long long m_events_to_process{0};
  std::string m_output_dir;

  int m_verbosity{0};

  std::unique_ptr<TChain> m_chain{nullptr};

  std::map<std::string, std::unique_ptr<TH2>> m_hists2D;

  void setup_chain();
  void init_hists();
  void process_events();
  void print_event_info(long long event_idx) const;
  void save_results() const;
};

void sEPDQA::setup_chain()
{
  std::cout << "Setting up TChain from: " << m_input_file << std::endl;

  m_chain = myUtils::setupTChain(m_input_file, "T");

  if (m_chain == nullptr)
  {
    throw std::runtime_error(std::format("Error in TChain Setup from file: {}", m_input_file));
  }

  // Setup branches
  m_chain->SetBranchStatus("*", false);

  std::unordered_set<std::string> branchNames = {
      "event", "centrality", "zvtx",
      "emcal_energy", "ihcal_energy", "ohcal_energy",
      "sepd_charge_south", "sepd_charge_north",
      "mbd_charge_south", "mbd_charge_north"};

  // Check Branch Status
  for (const auto &branchName : branchNames)
  {
    TBranch *branch = m_chain->GetBranch(branchName.c_str());
    if (branch)
    {
      m_chain->SetBranchStatus(branchName.c_str(), true);
    }
    else
    {
      throw std::runtime_error(std::format("Could not find Branch '{}' in file '{}'", branchName, m_input_file));
    }
  }

  // Set branch addresses
  m_chain->SetBranchAddress("event", &m_event_data.event);
  m_chain->SetBranchAddress("zvtx", &m_event_data.zvtx);
  m_chain->SetBranchAddress("centrality", &m_event_data.centrality);
  m_chain->SetBranchAddress("emcal_energy", &m_event_data.emcal_energy);
  m_chain->SetBranchAddress("ihcal_energy", &m_event_data.ihcal_energy);
  m_chain->SetBranchAddress("ohcal_energy", &m_event_data.ohcal_energy);

  m_chain->SetBranchAddress("sepd_charge_south", &m_event_data.sepd_charge_south);
  m_chain->SetBranchAddress("sepd_charge_north", &m_event_data.sepd_charge_north);

  m_chain->SetBranchAddress("mbd_charge_south", &m_event_data.mbd_charge_south);
  m_chain->SetBranchAddress("mbd_charge_north", &m_event_data.mbd_charge_north);

  std::cout << "Finished... setup_chain" << std::endl;
}

void sEPDQA::init_hists()
{
  unsigned int bins_sepd_total_charge{150};
  double sepd_total_charge_low{0};
  double sepd_total_charge_high{3e4};

  unsigned int bins_mbd_total_charge{120};
  double mbd_total_charge_low{0};
  double mbd_total_charge_high{3e3};

  unsigned int bins_Calo_E{540};
  double Calo_E_low{-2e2};
  double Calo_E_high{2.5e3};

  unsigned int bins_cent = 100;
  double cent_low = -0.5;
  double cent_high = 99.5;

  // Core requested 2D Histograms
  m_hists2D["h2sEPD_Centrality"] = std::make_unique<TH2F>("h2sEPD_Centrality", "; Centrality [%]; sEPD Total Charge", bins_cent, cent_low, cent_high, bins_sepd_total_charge, sepd_total_charge_low, sepd_total_charge_high);
  m_hists2D["h2sEPD_MBD"] = std::make_unique<TH2F>("h2sEPD_MBD", "; MBD Total Charge; sEPD Total Charge", bins_mbd_total_charge, mbd_total_charge_low, mbd_total_charge_high, bins_sepd_total_charge, sepd_total_charge_low, sepd_total_charge_high);
  m_hists2D["h2sEPD_CaloE"] = std::make_unique<TH2F>("h2sEPD_CaloE", "; Total Calorimeter Energy [GeV]; sEPD Total Charge", bins_Calo_E, Calo_E_low, Calo_E_high, bins_sepd_total_charge, sepd_total_charge_low, sepd_total_charge_high);

  // Bind pointers for performance
  m_hists.h2sEPD_Centrality = m_hists2D["h2sEPD_Centrality"].get();
  m_hists.h2sEPD_MBD = m_hists2D["h2sEPD_MBD"].get();
  m_hists.h2sEPD_CaloE = m_hists2D["h2sEPD_CaloE"].get();

  for (auto &[name, hist] : m_hists2D)
  {
    if (hist)
    {
      hist->Sumw2();
    }
  }
}

void sEPDQA::process_events()
{
  std::cout << "Processing... process_events" << std::endl;
  long long n_entries = m_chain->GetEntries();
  if (m_events_to_process)
  {
    n_entries = std::min(m_events_to_process, n_entries);
  }

  for (long long event = 0; event < n_entries; ++event)
  {
    m_chain->GetEntry(event);

    if (event % 100000 == 0)
    {
      std::cout << std::format("Processing {}/{}: {:.2f} %", event, n_entries, static_cast<double>(event) * 100. / static_cast<double>(n_entries)) << std::endl;
    }

    double sepd_total = m_event_data.sepd_charge_south + m_event_data.sepd_charge_north;
    double mbd_total = m_event_data.mbd_charge_south + m_event_data.mbd_charge_north;
    double total_calo_e = m_event_data.emcal_energy + m_event_data.ihcal_energy + m_event_data.ohcal_energy;
    double cent = m_event_data.centrality;

    // Fill Core 2D Histograms
    m_hists.h2sEPD_Centrality->Fill(cent, sepd_total);
    m_hists.h2sEPD_MBD->Fill(mbd_total, sepd_total);
    m_hists.h2sEPD_CaloE->Fill(total_calo_e, sepd_total);

    if (m_verbosity > 0)
    {
      print_event_info(event);
    }
  }

  std::cout << "Finished... process_events" << std::endl;
}

void sEPDQA::print_event_info(long long event_idx) const
{
  double sepd_total = m_event_data.sepd_charge_south + m_event_data.sepd_charge_north;
  double mbd_total = m_event_data.mbd_charge_south + m_event_data.mbd_charge_north;
  double total_calo_e = m_event_data.emcal_energy + m_event_data.ihcal_energy + m_event_data.ohcal_energy;

  std::cout << std::format("\n{:=^70}\n", std::format(" Event {} (Entry ID: {}) ", event_idx, m_event_data.event));
  std::cout << std::format(" Centrality: {:.2f} %, Zvtx: {:.2f} cm\n", m_event_data.centrality, m_event_data.zvtx);
  std::cout << std::format(" sEPD Charge  - Total: {:.2f}, South: {:.2f}, North: {:.2f}\n", sepd_total, m_event_data.sepd_charge_south, m_event_data.sepd_charge_north);
  std::cout << std::format(" MBD Charge   - Total: {:.2f}, South: {:.2f}, North: {:.2f}\n", mbd_total, m_event_data.mbd_charge_south, m_event_data.mbd_charge_north);
  std::cout << std::format(" Calo Energy  - Total: {:.2f} GeV, EMCal: {:.2f}, IHCal: {:.2f}, OHCal: {:.2f}\n", total_calo_e, m_event_data.emcal_energy, m_event_data.ihcal_energy, m_event_data.ohcal_energy);
  std::cout << std::format("{:=^70}\n", "");
}

void sEPDQA::save_results() const
{
  std::filesystem::create_directories(m_output_dir);

  std::filesystem::path input_path(m_input_file);
  std::string output_stem = input_path.stem().string();
  std::string output_filename = std::format("{}/sEPD-QA_{}.root", m_output_dir, output_stem);

  auto output_file = std::make_unique<TFile>(output_filename.c_str(), "RECREATE");

  for (const auto &[name, hist] : m_hists2D)
  {
    if (hist)
    {
      std::cout << std::format("Saving: {}, Estimated Size: {} Bytes\n", name, hist->Sizeof());
      hist->Write();
    }
  }

  output_file->Close();

  std::cout << std::format("Results saved to: {}", output_filename)
            << std::endl;
}

// Interactive ROOT macro function
void sEPD_QA(const std::string &input_file = "tree.root", long long events = 0, const std::string &output_dir = ".")
{
  sEPDQA analysis(input_file, events, output_dir);
  analysis.run();
}

// ====================================================================
// Main function for compiled binary
// ====================================================================
int main(int argc, const char *const argv[])
{
  gROOT->SetBatch(true);
  TH1::AddDirectory(false);

  if (argc < 2 || argc > 5)
  {
    std::cout << "Usage: " << argv[0] << " input_file [events] [output_directory] [verbosity]" << std::endl;
    return 1;
  }

  int ctr = 1;
  const std::string input_file = argv[ctr++];
  long long events = (argc >= ctr + 1) ? std::atoll(argv[ctr++]) : 0;
  std::string output_dir = (argc >= ctr + 1) ? argv[ctr++] : ".";
  int verbosity = (argc >= ctr + 1) ? std::atoi(argv[ctr++]) : 0;

  std::cout << std::format("{:#<20}\n", "");
  std::cout << std::format("Run Params\n");
  std::cout << std::format("Input: {}\n", input_file);
  std::cout << std::format("Events: {}\n", events);
  std::cout << std::format("Output Dir: {}\n", output_dir);
  std::cout << std::format("Verbosity: {}\n", verbosity);
  std::cout << std::format("{:#<20}\n", "");

  try
  {
    sEPDQA analysis(input_file, events, output_dir);
    analysis.set_verbosity(verbosity);
    analysis.run();
  }
  catch (const std::exception &e)
  {
    std::cout << std::format("An exception occurred: {}", e.what()) << std::endl;
    return 1;
  }

  std::cout << "Analysis complete." << std::endl;
  return 0;
}
