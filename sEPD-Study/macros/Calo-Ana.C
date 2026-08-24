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
#include <optional>
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
  void set_do_rcone(bool b = true) { m_do_rcone = b; }
  void set_do_jets(bool b = true) { m_do_jets = b; }

  bool get_do_iter() const { return m_do_iter; }
  bool get_do_mult() const { return m_do_mult; }
  bool get_do_rcone() const { return m_do_rcone; }
  bool get_do_jets() const { return m_do_jets; }

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

    // Subtracted Random Cone detailed towers
    // R = 0.2
    std::vector<int> *rcone_r02_emcal_tower_index{nullptr};
    std::vector<double> *rcone_r02_sub1_emcal_tower_energy{nullptr};

    std::vector<int> *rcone_r02_ihcal_tower_index{nullptr};
    std::vector<double> *rcone_r02_sub1_ihcal_tower_energy{nullptr};

    std::vector<int> *rcone_r02_ohcal_tower_index{nullptr};
    std::vector<double> *rcone_r02_sub1_ohcal_tower_energy{nullptr};

    // R = 0.3
    std::vector<int> *rcone_r03_emcal_tower_index{nullptr};
    std::vector<double> *rcone_r03_sub1_emcal_tower_energy{nullptr};

    std::vector<int> *rcone_r03_ihcal_tower_index{nullptr};
    std::vector<double> *rcone_r03_sub1_ihcal_tower_energy{nullptr};

    std::vector<int> *rcone_r03_ohcal_tower_index{nullptr};
    std::vector<double> *rcone_r03_sub1_ohcal_tower_energy{nullptr};

    // Iterative Subtracted Jet Constituent detailed towers
    // R = 0.2
    std::vector<std::vector<int>> *emcal_tower_index_iter_r02{nullptr};
    std::vector<std::vector<double>> *emcal_tower_energy_iter_r02{nullptr};

    std::vector<std::vector<int>> *ihcal_tower_index_iter_r02{nullptr};
    std::vector<std::vector<double>> *ihcal_tower_energy_iter_r02{nullptr};

    std::vector<std::vector<int>> *ohcal_tower_index_iter_r02{nullptr};
    std::vector<std::vector<double>> *ohcal_tower_energy_iter_r02{nullptr};

    // R = 0.3
    std::vector<std::vector<int>> *emcal_tower_index_iter_r03{nullptr};
    std::vector<std::vector<double>> *emcal_tower_energy_iter_r03{nullptr};

    std::vector<std::vector<int>> *ihcal_tower_index_iter_r03{nullptr};
    std::vector<std::vector<double>> *ihcal_tower_energy_iter_r03{nullptr};

    std::vector<std::vector<int>> *ohcal_tower_index_iter_r03{nullptr};
    std::vector<std::vector<double>> *ohcal_tower_energy_iter_r03{nullptr};
  };

  struct TowerDataGroup
  {
    std::string tag;
    std::string title_desc;
    const std::vector<int> *indices{nullptr};
    const std::vector<double> *energies{nullptr};
    std::optional<int> jet_idx{std::nullopt};
    bool is_base_emcal{false};
  };

  struct JetCaloTowerPointers
  {
    std::string r_tag;   // "r02" or "r03"
    std::string r_desc;  // "R=0.2" or "R=0.3"
    const std::vector<std::vector<int>> *emcal_idx{nullptr};
    const std::vector<std::vector<double>> *emcal_e{nullptr};
    const std::vector<std::vector<int>> *ihcal_idx{nullptr};
    const std::vector<std::vector<double>> *ihcal_e{nullptr};
    const std::vector<std::vector<int>> *ohcal_idx{nullptr};
    const std::vector<std::vector<double>> *ohcal_e{nullptr};
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
  bool m_do_rcone{true};
  bool m_do_jets{true};

  std::map<std::string, std::unique_ptr<TH2>> m_hists2D;

  // --- Private Helper Methods ---
  void setup_chain();
  void make_and_fill_hist(const TowerDataGroup &group, int run, int event);
  void process_jet_collection(const JetCaloTowerPointers &jet_ptrs, int run, int event);
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

  if (m_do_rcone)
  {
    branchNames.insert({
        "rcone_r02_emcal_tower_index", "rcone_r02_sub1_emcal_tower_energy",
        "rcone_r02_ihcal_tower_index", "rcone_r02_sub1_ihcal_tower_energy",
        "rcone_r02_ohcal_tower_index", "rcone_r02_sub1_ohcal_tower_energy",
        "rcone_r03_emcal_tower_index", "rcone_r03_sub1_emcal_tower_energy",
        "rcone_r03_ihcal_tower_index", "rcone_r03_sub1_ihcal_tower_energy",
        "rcone_r03_ohcal_tower_index", "rcone_r03_sub1_ohcal_tower_energy"
    });
  }

  if (m_do_jets && m_do_iter)
  {
    branchNames.insert({
        "emcal_tower_index_iter_r02", "emcal_tower_energy_iter_r02",
        "ihcal_tower_index_iter_r02", "ihcal_tower_energy_iter_r02",
        "ohcal_tower_index_iter_r02", "ohcal_tower_energy_iter_r02",
        "emcal_tower_index_iter_r03", "emcal_tower_energy_iter_r03",
        "ihcal_tower_index_iter_r03", "ihcal_tower_energy_iter_r03",
        "ohcal_tower_index_iter_r03", "ohcal_tower_energy_iter_r03"
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

  if (m_do_rcone)
  {
    m_chain->SetBranchAddress("rcone_r02_emcal_tower_index", &m_event_data.rcone_r02_emcal_tower_index);
    m_chain->SetBranchAddress("rcone_r02_sub1_emcal_tower_energy", &m_event_data.rcone_r02_sub1_emcal_tower_energy);
    m_chain->SetBranchAddress("rcone_r02_ihcal_tower_index", &m_event_data.rcone_r02_ihcal_tower_index);
    m_chain->SetBranchAddress("rcone_r02_sub1_ihcal_tower_energy", &m_event_data.rcone_r02_sub1_ihcal_tower_energy);
    m_chain->SetBranchAddress("rcone_r02_ohcal_tower_index", &m_event_data.rcone_r02_ohcal_tower_index);
    m_chain->SetBranchAddress("rcone_r02_sub1_ohcal_tower_energy", &m_event_data.rcone_r02_sub1_ohcal_tower_energy);

    m_chain->SetBranchAddress("rcone_r03_emcal_tower_index", &m_event_data.rcone_r03_emcal_tower_index);
    m_chain->SetBranchAddress("rcone_r03_sub1_emcal_tower_energy", &m_event_data.rcone_r03_sub1_emcal_tower_energy);
    m_chain->SetBranchAddress("rcone_r03_ihcal_tower_index", &m_event_data.rcone_r03_ihcal_tower_index);
    m_chain->SetBranchAddress("rcone_r03_sub1_ihcal_tower_energy", &m_event_data.rcone_r03_sub1_ihcal_tower_energy);
    m_chain->SetBranchAddress("rcone_r03_ohcal_tower_index", &m_event_data.rcone_r03_ohcal_tower_index);
    m_chain->SetBranchAddress("rcone_r03_sub1_ohcal_tower_energy", &m_event_data.rcone_r03_sub1_ohcal_tower_energy);
  }

  if (m_do_jets && m_do_iter)
  {
    m_chain->SetBranchAddress("emcal_tower_index_iter_r02", &m_event_data.emcal_tower_index_iter_r02);
    m_chain->SetBranchAddress("emcal_tower_energy_iter_r02", &m_event_data.emcal_tower_energy_iter_r02);
    m_chain->SetBranchAddress("ihcal_tower_index_iter_r02", &m_event_data.ihcal_tower_index_iter_r02);
    m_chain->SetBranchAddress("ihcal_tower_energy_iter_r02", &m_event_data.ihcal_tower_energy_iter_r02);
    m_chain->SetBranchAddress("ohcal_tower_index_iter_r02", &m_event_data.ohcal_tower_index_iter_r02);
    m_chain->SetBranchAddress("ohcal_tower_energy_iter_r02", &m_event_data.ohcal_tower_energy_iter_r02);

    m_chain->SetBranchAddress("emcal_tower_index_iter_r03", &m_event_data.emcal_tower_index_iter_r03);
    m_chain->SetBranchAddress("emcal_tower_energy_iter_r03", &m_event_data.emcal_tower_energy_iter_r03);
    m_chain->SetBranchAddress("ihcal_tower_index_iter_r03", &m_event_data.ihcal_tower_index_iter_r03);
    m_chain->SetBranchAddress("ihcal_tower_energy_iter_r03", &m_event_data.ihcal_tower_energy_iter_r03);
    m_chain->SetBranchAddress("ohcal_tower_index_iter_r03", &m_event_data.ohcal_tower_index_iter_r03);
    m_chain->SetBranchAddress("ohcal_tower_energy_iter_r03", &m_event_data.ohcal_tower_energy_iter_r03);
  }

  std::cout << "Finished... setup_chain" << std::endl;
}

void CaloAnalysis::make_and_fill_hist(const TowerDataGroup &group, int run, int event)
{
  if (!group.indices || !group.energies)
  {
    return;
  }

  int n_eta = group.is_base_emcal ? CaloGeometry::CEMC_ETA_BINS : CaloGeometry::HCAL_ETA_BINS;
  int n_phi = group.is_base_emcal ? CaloGeometry::CEMC_PHI_BINS : CaloGeometry::HCAL_PHI_BINS;

  std::string name;
  std::string title;

  if (group.jet_idx.has_value())
  {
    name = std::format("h2_{}_energy_run{}_event{}_jet{}", group.tag, run, event, *group.jet_idx);
    title = std::format("{} (Run {}, Event {}, Jet {}); Tower #eta bin; Tower #phi bin; Energy [GeV]",
                        group.title_desc, run, event, *group.jet_idx);
  }
  else
  {
    name = std::format("h2_{}_energy_run{}_event{}", group.tag, run, event);
    title = std::format("{} (Run {}, Event {}); Tower #eta bin; Tower #phi bin; Energy [GeV]",
                        group.title_desc, run, event);
  }

  auto h2 = std::make_unique<TH2F>(
      name.c_str(), title.c_str(),
      n_eta, -0.5, n_eta - 0.5,
      n_phi, -0.5, n_phi - 0.5);

  for (size_t i = 0; i < group.indices->size() && i < group.energies->size(); ++i)
  {
    unsigned int raw_idx = static_cast<unsigned int>((*group.indices)[i]);
    unsigned int key = group.is_base_emcal ? TowerInfoDefs::encode_emcal(raw_idx)
                                           : TowerInfoDefs::encode_hcal(raw_idx);
    unsigned int ieta = TowerInfoDefs::getCaloTowerEtaBin(key);
    unsigned int iphi = TowerInfoDefs::getCaloTowerPhiBin(key);
    h2->SetBinContent(static_cast<int>(ieta + 1), static_cast<int>(iphi + 1), (*group.energies)[i]);
  }

  m_hists2D[name] = std::move(h2);
}

void CaloAnalysis::process_jet_collection(const JetCaloTowerPointers &jet_ptrs, int run, int event)
{
  if (!jet_ptrs.emcal_idx)
  {
    return;
  }

  size_t n_jets = jet_ptrs.emcal_idx->size();
  for (size_t j = 0; j < n_jets; ++j)
  {
    auto get_vec = [](const auto *nested, size_t idx) -> const auto * {
      return (nested && idx < nested->size()) ? &(*nested)[idx] : nullptr;
    };

    int j_int = static_cast<int>(j);
    make_and_fill_hist({std::format("jet_iter_emcal_{}", jet_ptrs.r_tag),
                        std::format("Iter Sub1 Jet {} EMCal Energy", jet_ptrs.r_desc),
                        get_vec(jet_ptrs.emcal_idx, j), get_vec(jet_ptrs.emcal_e, j),
                        j_int, false}, run, event);

    make_and_fill_hist({std::format("jet_iter_ihcal_{}", jet_ptrs.r_tag),
                        std::format("Iter Sub1 Jet {} IHCal Energy", jet_ptrs.r_desc),
                        get_vec(jet_ptrs.ihcal_idx, j), get_vec(jet_ptrs.ihcal_e, j),
                        j_int, false}, run, event);

    make_and_fill_hist({std::format("jet_iter_ohcal_{}", jet_ptrs.r_tag),
                        std::format("Iter Sub1 Jet {} OHCal Energy", jet_ptrs.r_desc),
                        get_vec(jet_ptrs.ohcal_idx, j), get_vec(jet_ptrs.ohcal_e, j),
                        j_int, false}, run, event);
  }
}

void CaloAnalysis::process_events()
{
  long long total_entries = m_chain->GetEntries();
  long long entries_to_process = (m_events_to_process > 0 && m_events_to_process < total_entries)
                                     ? m_events_to_process
                                     : total_entries;

  std::cout << std::format("Processing {} / {} entries\n", entries_to_process, total_entries);

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

    std::vector<TowerDataGroup> active_groups = {
        {"emcal_base", "EMCal Base Energy", m_event_data.emcal_base_tower_index, m_event_data.emcal_base_tower_energy, std::nullopt, true},
        {"emcal_retower", "EMCal Retower Energy", m_event_data.emcal_retower_tower_index, m_event_data.emcal_retower_tower_energy, std::nullopt, false},
        {"ihcal", "IHCal Energy", m_event_data.ihcal_tower_index, m_event_data.ihcal_tower_energy, std::nullopt, false},
        {"ohcal", "OHCal Energy", m_event_data.ohcal_tower_index, m_event_data.ohcal_tower_energy, std::nullopt, false},
    };

    if (m_do_iter)
    {
      active_groups.push_back({"iter_emcal", "Iter Sub1 EMCal Energy", m_event_data.iter_emcal_tower_index, m_event_data.iter_emcal_tower_energy, std::nullopt, false});
      active_groups.push_back({"iter_ihcal", "Iter Sub1 IHCal Energy", m_event_data.iter_ihcal_tower_index, m_event_data.iter_ihcal_tower_energy, std::nullopt, false});
      active_groups.push_back({"iter_ohcal", "Iter Sub1 OHCal Energy", m_event_data.iter_ohcal_tower_index, m_event_data.iter_ohcal_tower_energy, std::nullopt, false});
    }

    if (m_do_mult)
    {
      active_groups.push_back({"mult_emcal", "Mult Sub1 EMCal Energy", m_event_data.mult_emcal_tower_index, m_event_data.mult_emcal_tower_energy, std::nullopt, false});
      active_groups.push_back({"mult_ihcal", "Mult Sub1 IHCal Energy", m_event_data.mult_ihcal_tower_index, m_event_data.mult_ihcal_tower_energy, std::nullopt, false});
      active_groups.push_back({"mult_ohcal", "Mult Sub1 OHCal Energy", m_event_data.mult_ohcal_tower_index, m_event_data.mult_ohcal_tower_energy, std::nullopt, false});
    }

    if (m_do_rcone)
    {
      // R = 0.2 subtracted random cone
      active_groups.push_back({"rcone_sub1_emcal_r02", "RCone R=0.2 Sub1 EMCal Energy", m_event_data.rcone_r02_emcal_tower_index, m_event_data.rcone_r02_sub1_emcal_tower_energy, std::nullopt, false});
      active_groups.push_back({"rcone_sub1_ihcal_r02", "RCone R=0.2 Sub1 IHCal Energy", m_event_data.rcone_r02_ihcal_tower_index, m_event_data.rcone_r02_sub1_ihcal_tower_energy, std::nullopt, false});
      active_groups.push_back({"rcone_sub1_ohcal_r02", "RCone R=0.2 Sub1 OHCal Energy", m_event_data.rcone_r02_ohcal_tower_index, m_event_data.rcone_r02_sub1_ohcal_tower_energy, std::nullopt, false});

      // R = 0.3 subtracted random cone
      active_groups.push_back({"rcone_sub1_emcal_r03", "RCone R=0.3 Sub1 EMCal Energy", m_event_data.rcone_r03_emcal_tower_index, m_event_data.rcone_r03_sub1_emcal_tower_energy, std::nullopt, false});
      active_groups.push_back({"rcone_sub1_ihcal_r03", "RCone R=0.3 Sub1 IHCal Energy", m_event_data.rcone_r03_ihcal_tower_index, m_event_data.rcone_r03_sub1_ihcal_tower_energy, std::nullopt, false});
      active_groups.push_back({"rcone_sub1_ohcal_r03", "RCone R=0.3 Sub1 OHCal Energy", m_event_data.rcone_r03_ohcal_tower_index, m_event_data.rcone_r03_sub1_ohcal_tower_energy, std::nullopt, false});
    }

    for (const auto &group : active_groups)
    {
      make_and_fill_hist(group, run, event);
    }

    if (m_do_jets && m_do_iter)
    {
      // R = 0.2 jets
      JetCaloTowerPointers r02_ptrs{
          "r02", "R=0.2",
          m_event_data.emcal_tower_index_iter_r02, m_event_data.emcal_tower_energy_iter_r02,
          m_event_data.ihcal_tower_index_iter_r02, m_event_data.ihcal_tower_energy_iter_r02,
          m_event_data.ohcal_tower_index_iter_r02, m_event_data.ohcal_tower_energy_iter_r02
      };
      process_jet_collection(r02_ptrs, run, event);

      // R = 0.3 jets
      JetCaloTowerPointers r03_ptrs{
          "r03", "R=0.3",
          m_event_data.emcal_tower_index_iter_r03, m_event_data.emcal_tower_energy_iter_r03,
          m_event_data.ihcal_tower_index_iter_r03, m_event_data.ihcal_tower_energy_iter_r03,
          m_event_data.ohcal_tower_index_iter_r03, m_event_data.ohcal_tower_energy_iter_r03
      };
      process_jet_collection(r03_ptrs, run, event);
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

  if (argc < 2 || argc > 9)
  {
    std::cout << "Usage: " << argv[0] << " input_file [events] [output_directory] [verbosity] [do_iter] [do_mult] [do_rcone] [do_jets]\n";
    return 1;
  }

  int ctr = 1;
  const std::string input_file = argv[ctr++];
  long long events = (argc >= ctr + 1) ? std::atoll(argv[ctr++]) : 0;
  std::string output_dir = (argc >= ctr + 1) ? argv[ctr++] : ".";
  int verbosity = (argc >= ctr + 1) ? std::atoi(argv[ctr++]) : 0;
  bool do_iter = (argc >= ctr + 1) ? (std::atoi(argv[ctr++]) != 0) : true;
  bool do_mult = (argc >= ctr + 1) ? (std::atoi(argv[ctr++]) != 0) : false;
  bool do_rcone = (argc >= ctr + 1) ? (std::atoi(argv[ctr++]) != 0) : true;
  bool do_jets = (argc >= ctr + 1) ? (std::atoi(argv[ctr++]) != 0) : true;

  std::cout << std::format("{:#<20}\n", "");
  std::cout << "Calo-Ana Parameters\n";
  std::cout << std::format("Input: {}\n", input_file);
  std::cout << std::format("Events: {}\n", events);
  std::cout << std::format("Output Dir: {}\n", output_dir);
  std::cout << std::format("Verbosity: {}\n", verbosity);
  std::cout << std::format("Do Iter: {}\n", do_iter);
  std::cout << std::format("Do Mult: {}\n", do_mult);
  std::cout << std::format("Do RCone: {}\n", do_rcone);
  std::cout << std::format("Do Jets: {}\n", do_jets);
  std::cout << std::format("{:#<20}\n", "");

  try
  {
    CaloAnalysis analysis(input_file, events, output_dir);
    analysis.set_verbosity(verbosity);
    analysis.set_do_iter(do_iter);
    analysis.set_do_mult(do_mult);
    analysis.set_do_rcone(do_rcone);
    analysis.set_do_jets(do_jets);
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
