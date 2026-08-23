// -- utils
#include "myUtils.C"

// ====================================================================
// sPHENIX Includes
// ====================================================================
#include <calobase/TowerInfoDefs.h>
#include <sepdvalidation/geometry_constants.h>

// ====================================================================
// Standard C++ Includes
// ====================================================================
#include <filesystem>
#include <format>
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

// ====================================================================
// The Calo Analysis Class
// ====================================================================
class CaloAnalysis
{
 public:
  CaloAnalysis(std::string input_file, long long events, std::string output_dir)
    : m_input_file(std::move(input_file))
    , m_events_to_process(events)
    , m_output_dir(std::move(output_dir))
  {
  }

  void run()
  {
    setup_chain();
    process_events();
    save_results();
  }

  void set_do_iter(bool b = true) { m_do_iter = b; }
  void set_do_mult(bool b = true) { m_do_mult = b; }
  bool get_do_iter() const { return m_do_iter; }
  bool get_do_mult() const { return m_do_mult; }

  void set_verbosity(int verbosity) { m_verbosity = verbosity; }
  int get_verbosity() const { return m_verbosity; }

 private:
  struct EventData
  {
    int run{0};
    int event{0};
    double zvtx{0.0};
    double centrality{0.0};
    double emcal_energy{0.0};
    double ihcal_energy{0.0};
    double ohcal_energy{0.0};

    // Unsubtracted detailed towers
    std::vector<int> *emcal_base_tower_index{nullptr};
    std::vector<double> *emcal_base_tower_energy{nullptr};

    std::vector<int> *emcal_retower_tower_index{nullptr};
    std::vector<double> *emcal_retower_tower_energy{nullptr};

    std::vector<int> *ihcal_tower_index{nullptr};
    std::vector<double> *ihcal_tower_energy{nullptr};

    std::vector<int> *ohcal_tower_index{nullptr};
    std::vector<double> *ohcal_tower_energy{nullptr};

    // Iterative subtracted detailed towers
    std::vector<int> *iter_emcal_tower_index{nullptr};
    std::vector<double> *iter_emcal_tower_energy{nullptr};

    std::vector<int> *iter_ihcal_tower_index{nullptr};
    std::vector<double> *iter_ihcal_tower_energy{nullptr};

    std::vector<int> *iter_ohcal_tower_index{nullptr};
    std::vector<double> *iter_ohcal_tower_energy{nullptr};

    // Multiplicity subtracted detailed towers
    std::vector<int> *mult_emcal_tower_index{nullptr};
    std::vector<double> *mult_emcal_tower_energy{nullptr};

    std::vector<int> *mult_ihcal_tower_index{nullptr};
    std::vector<double> *mult_ihcal_tower_energy{nullptr};

    std::vector<int> *mult_ohcal_tower_index{nullptr};
    std::vector<double> *mult_ohcal_tower_energy{nullptr};
  };

  // --- Member Variables ---
  EventData m_event_data;
  std::unique_ptr<TChain> m_chain;

  std::string m_input_file;
  long long m_events_to_process;
  std::string m_output_dir;
  int m_verbosity{0};

  bool m_do_iter{true};
  bool m_do_mult{false};

  std::map<std::string, std::unique_ptr<TH2>> m_hists2D;

  // --- Private Helper Methods ---
  void setup_chain();
  void process_events();
  void save_results() const;
};

// ====================================================================
// Implementation of Class Methods
// ====================================================================
void CaloAnalysis::setup_chain()
{
  std::cout << "Processing... setup_chain" << std::endl;

  m_chain = myUtils::setupTChain(m_input_file, "T");

  if (!m_chain)
  {
    throw std::runtime_error(
        std::format("Error in TChain Setup from file: {}", m_input_file));
  }

  // Setup branches
  m_chain->SetBranchStatus("*", false);

  std::unordered_set<std::string> branchNames = {
      "run", "event", "zvtx", "centrality",
      "emcal_energy", "ihcal_energy", "ohcal_energy",
      "emcal_base_tower_index", "emcal_base_tower_energy",
      "emcal_retower_tower_index", "emcal_retower_tower_energy",
      "ihcal_tower_index", "ihcal_tower_energy",
      "ohcal_tower_index", "ohcal_tower_energy"
  };

  if (m_do_iter)
  {
    branchNames.insert({
        "iter_emcal_tower_index", "iter_emcal_tower_energy",
        "iter_ihcal_tower_index", "iter_ihcal_tower_energy",
        "iter_ohcal_tower_index", "iter_ohcal_tower_energy"
    });
  }

  if (m_do_mult)
  {
    branchNames.insert({
        "mult_emcal_tower_index", "mult_emcal_tower_energy",
        "mult_ihcal_tower_index", "mult_ihcal_tower_energy",
        "mult_ohcal_tower_index", "mult_ohcal_tower_energy"
    });
  }

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
      throw std::runtime_error(std::format(
          "Could not find Branch '{}' in file '{}'", branchName, m_input_file));
    }
  }

  // Set branch addresses
  m_chain->SetBranchAddress("run", &m_event_data.run);
  m_chain->SetBranchAddress("event", &m_event_data.event);
  m_chain->SetBranchAddress("zvtx", &m_event_data.zvtx);
  m_chain->SetBranchAddress("centrality", &m_event_data.centrality);
  m_chain->SetBranchAddress("emcal_energy", &m_event_data.emcal_energy);
  m_chain->SetBranchAddress("ihcal_energy", &m_event_data.ihcal_energy);
  m_chain->SetBranchAddress("ohcal_energy", &m_event_data.ohcal_energy);

  m_chain->SetBranchAddress("emcal_base_tower_index", &m_event_data.emcal_base_tower_index);
  m_chain->SetBranchAddress("emcal_base_tower_energy", &m_event_data.emcal_base_tower_energy);

  m_chain->SetBranchAddress("emcal_retower_tower_index", &m_event_data.emcal_retower_tower_index);
  m_chain->SetBranchAddress("emcal_retower_tower_energy", &m_event_data.emcal_retower_tower_energy);

  m_chain->SetBranchAddress("ihcal_tower_index", &m_event_data.ihcal_tower_index);
  m_chain->SetBranchAddress("ihcal_tower_energy", &m_event_data.ihcal_tower_energy);

  m_chain->SetBranchAddress("ohcal_tower_index", &m_event_data.ohcal_tower_index);
  m_chain->SetBranchAddress("ohcal_tower_energy", &m_event_data.ohcal_tower_energy);

  if (m_do_iter)
  {
    m_chain->SetBranchAddress("iter_emcal_tower_index", &m_event_data.iter_emcal_tower_index);
    m_chain->SetBranchAddress("iter_emcal_tower_energy", &m_event_data.iter_emcal_tower_energy);

    m_chain->SetBranchAddress("iter_ihcal_tower_index", &m_event_data.iter_ihcal_tower_index);
    m_chain->SetBranchAddress("iter_ihcal_tower_energy", &m_event_data.iter_ihcal_tower_energy);

    m_chain->SetBranchAddress("iter_ohcal_tower_index", &m_event_data.iter_ohcal_tower_index);
    m_chain->SetBranchAddress("iter_ohcal_tower_energy", &m_event_data.iter_ohcal_tower_energy);
  }

  if (m_do_mult)
  {
    m_chain->SetBranchAddress("mult_emcal_tower_index", &m_event_data.mult_emcal_tower_index);
    m_chain->SetBranchAddress("mult_emcal_tower_energy", &m_event_data.mult_emcal_tower_energy);

    m_chain->SetBranchAddress("mult_ihcal_tower_index", &m_event_data.mult_ihcal_tower_index);
    m_chain->SetBranchAddress("mult_ihcal_tower_energy", &m_event_data.mult_ihcal_tower_energy);

    m_chain->SetBranchAddress("mult_ohcal_tower_index", &m_event_data.mult_ohcal_tower_index);
    m_chain->SetBranchAddress("mult_ohcal_tower_energy", &m_event_data.mult_ohcal_tower_energy);
  }

  std::cout << "Finished... setup_chain" << std::endl;
}

void CaloAnalysis::process_events()
{
  long long total_entries = m_chain->GetEntries();
  long long entries_to_process = (m_events_to_process > 0 && m_events_to_process < total_entries)
                                     ? m_events_to_process
                                     : total_entries;

  std::cout << std::format("Processing {} / {} entries\n", entries_to_process, total_entries);

  auto fill_emcal_base_hist = [](TH2 *h2, const std::vector<int> *indices, const std::vector<double> *energies)
  {
    if (!h2 || !indices || !energies)
    {
      return;
    }
    for (size_t i = 0; i < indices->size() && i < energies->size(); ++i)
    {
      unsigned int key = TowerInfoDefs::encode_emcal(static_cast<unsigned int>((*indices)[i]));
      unsigned int ieta = TowerInfoDefs::getCaloTowerEtaBin(key);
      unsigned int iphi = TowerInfoDefs::getCaloTowerPhiBin(key);
      h2->SetBinContent(static_cast<int>(ieta + 1), static_cast<int>(iphi + 1), (*energies)[i]);
    }
  };

  auto fill_hcal_hist = [](TH2 *h2, const std::vector<int> *indices, const std::vector<double> *energies)
  {
    if (!h2 || !indices || !energies)
    {
      return;
    }
    for (size_t i = 0; i < indices->size() && i < energies->size(); ++i)
    {
      unsigned int key = TowerInfoDefs::encode_hcal(static_cast<unsigned int>((*indices)[i]));
      unsigned int ieta = TowerInfoDefs::getCaloTowerEtaBin(key);
      unsigned int iphi = TowerInfoDefs::getCaloTowerPhiBin(key);
      h2->SetBinContent(static_cast<int>(ieta + 1), static_cast<int>(iphi + 1), (*energies)[i]);
    }
  };

  for (long long entry = 0; entry < entries_to_process; ++entry)
  {
    m_chain->GetEntry(entry);

    if (entry % 100 == 0 || m_verbosity > 0)
    {
      std::cout << std::format("Entry {} / {} : Run {}, Event {}, Centrality {:.1f}%\n",
                               entry, entries_to_process, m_event_data.run, m_event_data.event, m_event_data.centrality);
    }

    int run = m_event_data.run;
    int event = m_event_data.event;

    // 1. EMCal Base (96 eta x 256 phi)
    std::string name_emcal_base = std::format("h2_emcal_base_energy_run{}_event{}", run, event);
    std::string title_emcal_base = std::format("EMCal Base Energy (Run {}, Event {}); Tower #eta bin; Tower #phi bin; Energy [GeV]", run, event);
    auto h2_emcal_base = std::make_unique<TH2F>(
        name_emcal_base.c_str(), title_emcal_base.c_str(),
        CaloGeometry::CEMC_ETA_BINS, -0.5, CaloGeometry::CEMC_ETA_BINS - 0.5,
        CaloGeometry::CEMC_PHI_BINS, -0.5, CaloGeometry::CEMC_PHI_BINS - 0.5);
    fill_emcal_base_hist(h2_emcal_base.get(), m_event_data.emcal_base_tower_index, m_event_data.emcal_base_tower_energy);
    m_hists2D[name_emcal_base] = std::move(h2_emcal_base);

    // 2. EMCal Retower (24 eta x 64 phi)
    std::string name_emcal_retower = std::format("h2_emcal_retower_energy_run{}_event{}", run, event);
    std::string title_emcal_retower = std::format("EMCal Retower Energy (Run {}, Event {}); Tower #eta bin; Tower #phi bin; Energy [GeV]", run, event);
    auto h2_emcal_retower = std::make_unique<TH2F>(
        name_emcal_retower.c_str(), title_emcal_retower.c_str(),
        CaloGeometry::HCAL_ETA_BINS, -0.5, CaloGeometry::HCAL_ETA_BINS - 0.5,
        CaloGeometry::HCAL_PHI_BINS, -0.5, CaloGeometry::HCAL_PHI_BINS - 0.5);
    fill_hcal_hist(h2_emcal_retower.get(), m_event_data.emcal_retower_tower_index, m_event_data.emcal_retower_tower_energy);
    m_hists2D[name_emcal_retower] = std::move(h2_emcal_retower);

    // 3. IHCal (24 eta x 64 phi)
    std::string name_ihcal = std::format("h2_ihcal_energy_run{}_event{}", run, event);
    std::string title_ihcal = std::format("IHCal Energy (Run {}, Event {}); Tower #eta bin; Tower #phi bin; Energy [GeV]", run, event);
    auto h2_ihcal = std::make_unique<TH2F>(
        name_ihcal.c_str(), title_ihcal.c_str(),
        CaloGeometry::HCAL_ETA_BINS, -0.5, CaloGeometry::HCAL_ETA_BINS - 0.5,
        CaloGeometry::HCAL_PHI_BINS, -0.5, CaloGeometry::HCAL_PHI_BINS - 0.5);
    fill_hcal_hist(h2_ihcal.get(), m_event_data.ihcal_tower_index, m_event_data.ihcal_tower_energy);
    m_hists2D[name_ihcal] = std::move(h2_ihcal);

    // 4. OHCal (24 eta x 64 phi)
    std::string name_ohcal = std::format("h2_ohcal_energy_run{}_event{}", run, event);
    std::string title_ohcal = std::format("OHCal Energy (Run {}, Event {}); Tower #eta bin; Tower #phi bin; Energy [GeV]", run, event);
    auto h2_ohcal = std::make_unique<TH2F>(
        name_ohcal.c_str(), title_ohcal.c_str(),
        CaloGeometry::HCAL_ETA_BINS, -0.5, CaloGeometry::HCAL_ETA_BINS - 0.5,
        CaloGeometry::HCAL_PHI_BINS, -0.5, CaloGeometry::HCAL_PHI_BINS - 0.5);
    fill_hcal_hist(h2_ohcal.get(), m_event_data.ohcal_tower_index, m_event_data.ohcal_tower_energy);
    m_hists2D[name_ohcal] = std::move(h2_ohcal);

    // 5. Iterative Subtracted Towers
    if (m_do_iter)
    {
      std::string name_iter_emcal = std::format("h2_iter_emcal_energy_run{}_event{}", run, event);
      std::string title_iter_emcal = std::format("Iter Sub1 EMCal Energy (Run {}, Event {}); Tower #eta bin; Tower #phi bin; Energy [GeV]", run, event);
      auto h2_iter_emcal = std::make_unique<TH2F>(
          name_iter_emcal.c_str(), title_iter_emcal.c_str(),
          CaloGeometry::HCAL_ETA_BINS, -0.5, CaloGeometry::HCAL_ETA_BINS - 0.5,
          CaloGeometry::HCAL_PHI_BINS, -0.5, CaloGeometry::HCAL_PHI_BINS - 0.5);
      fill_hcal_hist(h2_iter_emcal.get(), m_event_data.iter_emcal_tower_index, m_event_data.iter_emcal_tower_energy);
      m_hists2D[name_iter_emcal] = std::move(h2_iter_emcal);

      std::string name_iter_ihcal = std::format("h2_iter_ihcal_energy_run{}_event{}", run, event);
      std::string title_iter_ihcal = std::format("Iter Sub1 IHCal Energy (Run {}, Event {}); Tower #eta bin; Tower #phi bin; Energy [GeV]", run, event);
      auto h2_iter_ihcal = std::make_unique<TH2F>(
          name_iter_ihcal.c_str(), title_iter_ihcal.c_str(),
          CaloGeometry::HCAL_ETA_BINS, -0.5, CaloGeometry::HCAL_ETA_BINS - 0.5,
          CaloGeometry::HCAL_PHI_BINS, -0.5, CaloGeometry::HCAL_PHI_BINS - 0.5);
      fill_hcal_hist(h2_iter_ihcal.get(), m_event_data.iter_ihcal_tower_index, m_event_data.iter_ihcal_tower_energy);
      m_hists2D[name_iter_ihcal] = std::move(h2_iter_ihcal);

      std::string name_iter_ohcal = std::format("h2_iter_ohcal_energy_run{}_event{}", run, event);
      std::string title_iter_ohcal = std::format("Iter Sub1 OHCal Energy (Run {}, Event {}); Tower #eta bin; Tower #phi bin; Energy [GeV]", run, event);
      auto h2_iter_ohcal = std::make_unique<TH2F>(
          name_iter_ohcal.c_str(), title_iter_ohcal.c_str(),
          CaloGeometry::HCAL_ETA_BINS, -0.5, CaloGeometry::HCAL_ETA_BINS - 0.5,
          CaloGeometry::HCAL_PHI_BINS, -0.5, CaloGeometry::HCAL_PHI_BINS - 0.5);
      fill_hcal_hist(h2_iter_ohcal.get(), m_event_data.iter_ohcal_tower_index, m_event_data.iter_ohcal_tower_energy);
      m_hists2D[name_iter_ohcal] = std::move(h2_iter_ohcal);
    }

    // 6. Multiplicity Subtracted Towers
    if (m_do_mult)
    {
      std::string name_mult_emcal = std::format("h2_mult_emcal_energy_run{}_event{}", run, event);
      std::string title_mult_emcal = std::format("Mult Sub1 EMCal Energy (Run {}, Event {}); Tower #eta bin; Tower #phi bin; Energy [GeV]", run, event);
      auto h2_mult_emcal = std::make_unique<TH2F>(
          name_mult_emcal.c_str(), title_mult_emcal.c_str(),
          CaloGeometry::HCAL_ETA_BINS, -0.5, CaloGeometry::HCAL_ETA_BINS - 0.5,
          CaloGeometry::HCAL_PHI_BINS, -0.5, CaloGeometry::HCAL_PHI_BINS - 0.5);
      fill_hcal_hist(h2_mult_emcal.get(), m_event_data.mult_emcal_tower_index, m_event_data.mult_emcal_tower_energy);
      m_hists2D[name_mult_emcal] = std::move(h2_mult_emcal);

      std::string name_mult_ihcal = std::format("h2_mult_ihcal_energy_run{}_event{}", run, event);
      std::string title_mult_ihcal = std::format("Mult Sub1 IHCal Energy (Run {}, Event {}); Tower #eta bin; Tower #phi bin; Energy [GeV]", run, event);
      auto h2_mult_ihcal = std::make_unique<TH2F>(
          name_mult_ihcal.c_str(), title_mult_ihcal.c_str(),
          CaloGeometry::HCAL_ETA_BINS, -0.5, CaloGeometry::HCAL_ETA_BINS - 0.5,
          CaloGeometry::HCAL_PHI_BINS, -0.5, CaloGeometry::HCAL_PHI_BINS - 0.5);
      fill_hcal_hist(h2_mult_ihcal.get(), m_event_data.mult_ihcal_tower_index, m_event_data.mult_ihcal_tower_energy);
      m_hists2D[name_mult_ihcal] = std::move(h2_mult_ihcal);

      std::string name_mult_ohcal = std::format("h2_mult_ohcal_energy_run{}_event{}", run, event);
      std::string title_mult_ohcal = std::format("Mult Sub1 OHCal Energy (Run {}, Event {}); Tower #eta bin; Tower #phi bin; Energy [GeV]", run, event);
      auto h2_mult_ohcal = std::make_unique<TH2F>(
          name_mult_ohcal.c_str(), title_mult_ohcal.c_str(),
          CaloGeometry::HCAL_ETA_BINS, -0.5, CaloGeometry::HCAL_ETA_BINS - 0.5,
          CaloGeometry::HCAL_PHI_BINS, -0.5, CaloGeometry::HCAL_PHI_BINS - 0.5);
      fill_hcal_hist(h2_mult_ohcal.get(), m_event_data.mult_ohcal_tower_index, m_event_data.mult_ohcal_tower_energy);
      m_hists2D[name_mult_ohcal] = std::move(h2_mult_ohcal);
    }
  }
}

void CaloAnalysis::save_results() const
{
  std::filesystem::create_directories(m_output_dir);

  std::filesystem::path input_path(m_input_file);
  std::string output_stem = input_path.stem().string();
  std::string output_filename =
      std::format("{}/Calo-Ana_{}.root", m_output_dir, output_stem);

  auto output_file =
      std::make_unique<TFile>(output_filename.c_str(), "RECREATE");

  for (const auto &[name, hist] : m_hists2D)
  {
    if (hist)
    {
      if (m_verbosity > 0)
      {
        std::cout << std::format("Saving: {}, Estimated Size: {} Bytes\n", name, hist->Sizeof());
      }
      hist->Write();
    }
  }

  output_file->Close();

  std::cout << std::format("Results ({} histograms) saved to: {}\n", m_hists2D.size(), output_filename);
}

// ====================================================================
// Main function
// ====================================================================
int main(int argc, const char *const argv[])
{
  gROOT->SetBatch(true);
  TH1::AddDirectory(false);

  if (argc < 2 || argc > 7)
  {
    std::cout << "Usage: " << argv[0] << " input_file [events] [output_directory] [verbosity] [do_iter] [do_mult]\n";
    return 1;
  }

  int ctr = 1;
  const std::string input_file = argv[ctr++];
  long long events = (argc >= ctr + 1) ? std::atoll(argv[ctr++]) : 0;
  std::string output_dir = (argc >= ctr + 1) ? argv[ctr++] : ".";
  int verbosity = (argc >= ctr + 1) ? std::atoi(argv[ctr++]) : 0;
  bool do_iter = (argc >= ctr + 1) ? (std::atoi(argv[ctr++]) != 0) : true;
  bool do_mult = (argc >= ctr + 1) ? (std::atoi(argv[ctr++]) != 0) : false;

  std::cout << std::format("{:#<20}\n", "");
  std::cout << "Calo-Ana Parameters\n";
  std::cout << std::format("Input: {}\n", input_file);
  std::cout << std::format("Events: {}\n", events);
  std::cout << std::format("Output Dir: {}\n", output_dir);
  std::cout << std::format("Verbosity: {}\n", verbosity);
  std::cout << std::format("Do Iter: {}\n", do_iter);
  std::cout << std::format("Do Mult: {}\n", do_mult);
  std::cout << std::format("{:#<20}\n", "");

  try
  {
    CaloAnalysis analysis(input_file, events, output_dir);
    analysis.set_verbosity(verbosity);
    analysis.set_do_iter(do_iter);
    analysis.set_do_mult(do_mult);
    analysis.run();
  }
  catch (const std::exception &e)
  {
    std::cout << std::format("An exception occurred: {}\n", e.what());
    return 1;
  }

  std::cout << "Calo-Ana complete.\n";
  return 0;
}
