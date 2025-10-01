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
class JetAnalysis
{
 public:
  // The constructor takes the configuration
  JetAnalysis(std::string input_list, unsigned int runnumber, long long events, std::string output_dir)
    : m_chain(std::make_unique<TChain>("T"))
    , m_input_list(std::move(input_list))
    , m_runnumber(runnumber)
    , m_events_to_process(events)
    , m_output_dir(std::move(output_dir))
  {
  }

  void run()
  {
    setup_chain();
    process_dead_channels();
    save_results();
    // init_hists();
    // cache_events();
    // run_event_loop(Pass::ApplyRecentering);
    // run_event_loop(Pass::ApplyFlattening);
    // validate_results();
  }

 private:

  struct EventData
  {
    int event_id{0};
    double event_zvertex{0.0};
    double event_centrality{0.0};

    std::vector<double>* jet_pt{nullptr};
    std::vector<double>* jet_phi{nullptr};
    std::vector<double>* jet_eta{nullptr};
  };

  // --- Member Variables ---
  EventData m_event_data;
  std::unique_ptr<TChain> m_chain;

  // Configuration stored as members
  std::string m_input_list;
  unsigned int m_runnumber;
  long long m_events_to_process;
  std::string m_output_dir;
  std::string m_dbtag{"newcdbtag"};

  // Hists
  std::map<std::string, std::unique_ptr<TH1>> m_hists1D;
  std::map<std::string, std::unique_ptr<TH2>> m_hists2D;
  std::map<std::string, std::unique_ptr<TH3>> m_hists3D;
  std::map<std::string, std::unique_ptr<TProfile>> m_profiles;

  // --- Private Helper Methods ---
  void setup_chain();
  void process_dead_channels();
  // void init_hists();
  // bool compute_cached_event(CachedEvent &event);
  // void cache_events();
  // void run_event_loop(Pass pass);
  // void validate_results();
  void save_results() const;
  // static void process_averages(double cent, QVec q_S, QVec q_N, const AverageHists& h);
  // void process_recentering(double cent, size_t n_idx, QVec q_S, QVec q_N, const RecenterHists& h);
  // void process_flattening(double cent, size_t n_idx, QVec q_S, QVec q_N, const FlatteningHists& h);

  // void compute_averages(size_t cent_bin, int n);
  // void compute_recentering(size_t cent_bin, int n);
  // void print_flattening(size_t cent_bin, int n) const;

  // std::map<int, AverageHists> prepare_average_hists();
  // std::map<int, RecenterHists> prepare_recenter_hists();
  // std::map<int, FlatteningHists> prepare_flattening_hists();

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
  // m_chain->SetBranchStatus("jet_pt", true);

  m_chain->SetBranchAddress("event_id", &m_event_data.event_id);
  m_chain->SetBranchAddress("event_centrality", &m_event_data.event_centrality);
  m_chain->SetBranchAddress("jet_phi", &m_event_data.jet_phi);
  m_chain->SetBranchAddress("jet_eta", &m_event_data.jet_eta);

  std::cout << "Finished... setup_chain" << std::endl;
  // m_chain->SetBranchAddress("jet_pt", &m_event_data.jet_pt);
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

  auto h2Dead = m_hists2D["h2EMCalBadTowersDead"].get();

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

  auto h2Deadv2 = m_hists2D["h2EMCalBadTowersDeadv2"].get();
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

void JetAnalysis::save_results() const
{
  std::filesystem::create_directories(m_output_dir);
  std::string output_filename = m_output_dir + "/test.root";
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

  if (argc < 3 || argc > 5)
  {
    std::cout << "Usage: " << argv[0] << " <input_list_file> <runnumber> [events] [output_directory]" << std::endl;
    return 1;
  }

  const std::string input_list = argv[1];
  unsigned int runnumber = static_cast<unsigned int>(std::atoi(argv[2]));
  long long events = (argc >= 4) ? std::atoll(argv[3]) : 0;
  std::string output_dir = (argc >= 5) ? argv[4] : ".";

  try
  {
    JetAnalysis analysis(input_list, runnumber, events, output_dir);
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
