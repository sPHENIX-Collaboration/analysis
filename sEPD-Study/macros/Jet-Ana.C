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
    init_hists();
    process_events();
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

  // Store harmonic orders and subdetectors for easy iteration
  static constexpr std::array<int, 3> m_harmonics = {2, 3, 4};
  static constexpr std::array<Subdetector, 2> m_subdetectors = {Subdetector::S, Subdetector::N};

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

  // --- Member Variables ---
  EventData m_event_data;
  std::unique_ptr<TChain> m_chain;

  // Configuration stored as members
  std::string m_input_list;
  unsigned int m_runnumber;
  long long m_events_to_process;
  std::string m_output_dir;
  std::string m_dbtag{"newcdbtag"};

  // sEPD Bad Channels
  std::unordered_set<int> m_bad_channels;

  // Hists
  std::map<std::string, std::unique_ptr<TH1>> m_hists1D;
  std::map<std::string, std::unique_ptr<TH2>> m_hists2D;
  std::map<std::string, std::unique_ptr<TH3>> m_hists3D;
  std::map<std::string, std::unique_ptr<TProfile>> m_profiles;

  // --- Private Helper Methods ---
  void setup_chain();
  void process_dead_channels();
  void init_hists();

  void correct_QVecs();
  void compute_QVecs();
  void process_QVecs();
  void process_events();

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

  int bins_pt = 60;
  double pt_low = 0;
  double pt_high = 60;

  m_hists3D["h3JetPhiEtaPt"] = std::make_unique<TH3F>("h3JetPhiEtaPt", "Jet: |z| < 10 cm and MB; #phi; #eta; p_{T} [GeV]", bins_phi, phi_low, phi_high, bins_eta, eta_low, eta_high, bins_pt, pt_low, pt_high);
  m_hists3D["h3JetPhiEtaPtv2"] = std::unique_ptr<TH3>(static_cast<TH3*>(m_hists3D["h3JetPhiEtaPt"]->Clone("h3JetPhiEtaPtv2")));

  m_hists2D["h2Dummy"] = std::unique_ptr<TH2>(static_cast<TH2*>(m_hists3D["h3JetPhiEtaPt"]->Project3D("yx")->Clone("h2Dummy")));
  m_hists2D["h2Dummy"]->Rebin2D(2, 2);
}

void JetAnalysis::correct_QVecs()
{
}

void JetAnalysis::compute_QVecs()
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
    return;
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
}

void JetAnalysis::process_QVecs()
{
  compute_QVecs();
  correct_QVecs();
}

void JetAnalysis::process_events()
{
  std::cout << "Processing... process_events" << std::endl;
  long long n_entries = m_chain->GetEntries();
  if (m_events_to_process)
  {
    n_entries = std::min(m_events_to_process, n_entries);
  }

  auto* h2Dummy = m_hists2D["h2Dummy"].get();
  auto* h2EMCalBadTowersDeadv2 = m_hists2D["h2EMCalBadTowersDeadv2"].get();
  auto* h3JetPhiEtaPt = m_hists3D["h3JetPhiEtaPt"].get();
  auto* h3JetPhiEtaPtv2 = m_hists3D["h3JetPhiEtaPtv2"].get();

  // Event Loop
  for (long long i = 0; i < n_entries; ++i)
  {
    // Load Event Data from TChain
    m_chain->GetEntry(i);

    if (i % 5000 == 0)
    {
      std::cout << std::format("Processing {}/{}: {:.2f} %", i, n_entries, static_cast<double>(i) * 100. / static_cast<double>(n_entries)) << std::endl;
    }

    size_t nJets = m_event_data.jet_phi->size();

    // compute and correct the sEPD Q vectors
    process_QVecs();

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

      int bin_phi = h2Dummy->GetXaxis()->FindBin(phi);
      int bin_eta = h2Dummy->GetYaxis()->FindBin(eta);
      int dead_status = static_cast<int>(h2EMCalBadTowersDeadv2->GetBinContent(bin_phi, bin_eta));

      h3JetPhiEtaPt->Fill(phi, eta, pt);

      if (dead_status == 0)
      {
        h3JetPhiEtaPtv2->Fill(phi, eta, pt);
      }
    }
  }

  int jets = static_cast<int>(h3JetPhiEtaPt->GetEntries());
  int jets_good = static_cast<int>(h3JetPhiEtaPtv2->GetEntries());

  std::cout << std::format("Jets: {}, Post Masking: {}, {:.2f} %\n", jets, jets_good, jets_good * 100. / jets);

  std::cout << "Finished... process_events" << std::endl;
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
