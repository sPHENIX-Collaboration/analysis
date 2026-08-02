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
#include <TProfile.h>
#include <TProfile2D.h>
#include <TROOT.h>
#include <TTree.h>

// ====================================================================
// The Analysis Class
// ====================================================================
class JetAnalysisv2
{
 public:
  // The constructor takes the configuration
  JetAnalysisv2(std::string input_file, long long events, std::string output_dir)
    : m_input_file(std::move(input_file))
    , m_events_to_process(events)
    , m_output_dir(std::move(output_dir))
  {
  }

  void run()
  {
    setup_chain();
    load_calo_centrality_cuts();
    init_hists();
    process_events();
    save_results();
  }

  void set_jet_pt_min(double jet_pt_min)
  {
    m_jet_pt_min = jet_pt_min;
  }

  void set_jet_eta_max(double jet_eta_max)
  {
    m_jet_eta_max = jet_eta_max;
  }

  void set_verbosity(int verbosity)
  {
    m_verbosity = verbosity;
  }

 private:
  static constexpr size_t m_bins_cent = 100;
  static constexpr double m_cent_low = -0.5;
  static constexpr double m_cent_high = 99.5;

  struct AnalysisHists
  {
    TH1* hEvent{nullptr};
    TH1* hCentralityCaloFail{nullptr};

    TH1* hJetPt{nullptr};
    TH1* hJetPt_raw{nullptr};
    TH2* h2JetPhiPt{nullptr};
    TH2* h2JetPhiEta{nullptr};

    TH1* hJetPtv2{nullptr};
    TH1* hJetPtv2_raw{nullptr};
    TH2* h2JetPhiPtv2{nullptr};  // Positive Jet Energy
    TH2* h2JetPhiEtav2{nullptr};

    TH2* h2EMCal_OHCal{nullptr};
    TH2* h2IHCal_OHCal{nullptr};
    TH2* h2CentralityJetPt{nullptr};
    TH2* h2CentralityJetEnergy{nullptr};

    TH2* h2JetPtEnergy{nullptr};
    TH2* h2CaloEJetPt{nullptr};
    TH2* h2CaloECentrality_default{nullptr};
    TH2* h2CaloECentrality{nullptr};
    TH1* hCentrality{nullptr};
    TH2* h2CentralityZVtx{nullptr};

    TH1* hCaloECentrality_min{nullptr};
    TH1* hCaloECentrality_max{nullptr};
  };

  AnalysisHists m_hists;

  struct EventData
  {
    friend class JetAnalysisv2;

   private:
    int event_id{0};
    [[maybe_unused]] double event_zvertex{0.0};
    double event_centrality{0.0};
    double event_EMCal_Energy{0};
    double event_IHCal_Energy{0};
    double event_OHCal_Energy{0};
    double max_jet_pt{0.0};

    // Event Checks
    bool pass_calo_mbd{false};

    std::vector<double>* jet_energy{nullptr};
    std::vector<double>* jet_pt{nullptr};
    std::vector<double>* jet_pt_calib{nullptr};
    std::vector<double>* jet_phi{nullptr};
    std::vector<double>* jet_eta{nullptr};
  };

  // --- Member Variables ---
  EventData m_event_data;
  std::unique_ptr<TChain> m_chain;

  // Configuration stored as members
  std::string m_input_file;
  long long m_events_to_process;
  std::string m_output_dir;
  int m_verbosity{0};

  // Jet Cuts
  double m_jet_pt_min{10}; /*GeV*/
  double m_jet_eta_max{0.9};

  std::map<std::string, int> m_ctr;

  enum class EventType : std::uint8_t
  {
    ZVTX10_MB,
    CALOCENT
  };

  std::vector<std::string> m_eventType{"|z| < 10 cm and MB", "Good Calo-Cent"};

  // Hists
  std::map<std::string, std::unique_ptr<TH1>> m_hists1D;
  std::map<std::string, std::unique_ptr<TH2>> m_hists2D;
  std::map<std::string, std::unique_ptr<TProfile>> m_profiles;
  std::map<std::string, std::unique_ptr<TProfile2D>> m_profiles2D;

  // --- Private Helper Methods ---
  void setup_chain();

  void load_calo_centrality_cuts();

  void init_hists();

  void process_jets() const;
  bool check_CaloMBD() const;
  void process_calo();
  void process_event_check();
  void process_events();

  void save_results() const;
};

// ====================================================================
// Implementation of the Class Methods
// ====================================================================
void JetAnalysisv2::setup_chain()
{
  std::cout << "Processing... setup_chain" << std::endl;

  m_chain = myUtils::setupTChain(m_input_file, "T");

  if (m_chain == nullptr)
  {
    throw std::runtime_error(std::format("Error in TChain Setup from file: {}", m_input_file));
  }

  // Setup branches
  m_chain->SetBranchStatus("*", false);

  // List of Branches of Interest
  // Common branches between data and sim
  std::unordered_set<std::string> branchNames = {"event", "centrality", "zvtx",
                                                 "emcal_energy", "ihcal_energy", "ohcal_energy",
                                                 "pt_r02", "pt_calib_r02", "e_r02", "phi_r02", "eta_r02", "max_pt_r02"};

  // Check Branch Status
  for(const auto& branchName : branchNames)
  {
    TBranch* branch = m_chain->GetBranch(branchName.c_str());
    if (branch)
    {
      // Branch exists: Enable it
      m_chain->SetBranchStatus(branchName.c_str(), true);
    }
    else
    {
      throw std::runtime_error(std::format("Could not find Branch '{}' in file '{}'", branchName, m_input_file));
    }
  }

  // Set branches to variables
  // Common branches between data and sim
  m_chain->SetBranchAddress("event", &m_event_data.event_id);
  m_chain->SetBranchAddress("centrality", &m_event_data.event_centrality);
  m_chain->SetBranchAddress("zvtx", &m_event_data.event_zvertex);

  m_chain->SetBranchAddress("emcal_energy", &m_event_data.event_EMCal_Energy);
  m_chain->SetBranchAddress("ihcal_energy", &m_event_data.event_IHCal_Energy);
  m_chain->SetBranchAddress("ohcal_energy", &m_event_data.event_OHCal_Energy);

  m_chain->SetBranchAddress("pt_r02", &m_event_data.jet_pt);
  m_chain->SetBranchAddress("pt_calib_r02", &m_event_data.jet_pt_calib);
  m_chain->SetBranchAddress("e_r02", &m_event_data.jet_energy);
  m_chain->SetBranchAddress("phi_r02", &m_event_data.jet_phi);
  m_chain->SetBranchAddress("eta_r02", &m_event_data.jet_eta);
  m_chain->SetBranchAddress("max_pt_r02", &m_event_data.max_jet_pt);

  std::cout << "Finished... setup_chain" << std::endl;
}

void JetAnalysisv2::load_calo_centrality_cuts()
{
  std::string filename = "/direct/sphenix+u/anarde/Documents/sPHENIX/analysis-sEPD-Study/sEPD-Study/files/calib/run2auau_all_weights.root";
  std::string hLow_name = "h1_sumE_cent_min";
  std::string hHigh_name = "h1_sumE_cent_max";

  auto file = std::unique_ptr<TFile>(TFile::Open(filename.c_str()));

  // Check if the file was opened successfully.
  if (!file || file->IsZombie())
  {
    throw std::runtime_error(std::format("Could not open file '{}'", filename));
  }

  m_hists.hCaloECentrality_min = file->Get<TH1>(hLow_name.c_str());
  m_hists.hCaloECentrality_max = file->Get<TH1>(hHigh_name.c_str());
}

void JetAnalysisv2::init_hists()
{
  int bins_phi = 64;
  double phi_low = 0;
  double phi_high = 2 * std::numbers::pi;

  int bins_eta = 24;
  double eta_low = -1.152;
  double eta_high = 1.152;

  int bins_zvtx = 40;
  double zvtx_low = -10;
  double zvtx_high = 10;

  int bins_pt = 500;
  double pt_low = 0;
  double pt_high = 500;

  int bins_energy = 1000;
  double energy_low = -500;
  double energy_high = 500;

  unsigned int bins_Calo_E{540};
  double Calo_E_low{-2e2};
  double Calo_E_high{2.5e3};

  unsigned int bins_ihcal_E{80};
  double ihcal_E_low{-1e2};
  double ihcal_E_high{3e2};

  unsigned int bins_ohcal_E{240};
  double ohcal_E_low{-2e2};
  double ohcal_E_high{1e3};

  unsigned int bins_event = static_cast<unsigned int>(m_eventType.size());

  m_hists1D["hEvent"] = std::make_unique<TH1F>("hEvent", "Event Type; Type; Events", bins_event, 0, bins_event);

  m_hists1D["hCentralityCaloFail"] = std::make_unique<TH1F>("hCentralityCaloFail", "Centrality Calo Failure; Centrality [%]; Events", m_bins_cent, m_cent_low, m_cent_high);

  m_hists1D["hJetPt"] = std::make_unique<TH1F>("hJetPt", "; p_{T} [GeV]; Jets / 1 GeV", bins_pt, pt_low, pt_high);
  m_hists1D["hJetPtv2"] = std::unique_ptr<TH1>(static_cast<TH1*>(m_hists1D["hJetPt"]->Clone("hJetPtv2")));

  m_hists1D["hJetPt_raw"] = std::unique_ptr<TH1>(static_cast<TH1*>(m_hists1D["hJetPt"]->Clone("hJetPt_raw")));
  m_hists1D["hJetPtv2_raw"] = std::unique_ptr<TH1>(static_cast<TH1*>(m_hists1D["hJetPt"]->Clone("hJetPtv2_raw")));

  m_hists2D["h2JetPhiPt"] = std::make_unique<TH2F>("h2JetPhiPt", "Jet: |z| < 10 cm and MB; #phi; p_{T} [GeV]", bins_phi, phi_low, phi_high, bins_pt, pt_low, pt_high);
  m_hists2D["h2JetPhiEta"] = std::make_unique<TH2F>("h2JetPhiEta", "Jet: |z| < 10 cm and MB; #phi; #eta", bins_phi, phi_low, phi_high, bins_eta, eta_low, eta_high);

  m_hists2D["h2JetPhiPtv2"] = std::unique_ptr<TH2>(static_cast<TH2*>(m_hists2D["h2JetPhiPt"]->Clone("h2JetPhiPtv2")));
  m_hists2D["h2JetPhiEtav2"] = std::unique_ptr<TH2>(static_cast<TH2*>(m_hists2D["h2JetPhiEta"]->Clone("h2JetPhiEtav2")));

  m_hists2D["h2CentralityJetPt"] = std::make_unique<TH2F>("h2CentralityJetPt", "Jets; Centrality [%]; Jet p_{T} [GeV]",
                                                          m_bins_cent, m_cent_low, m_cent_high,
                                                          bins_pt, pt_low, pt_high);

  m_hists2D["h2CentralityJetEnergy"] = std::make_unique<TH2F>("h2CentralityJetEnergy", "Jets; Centrality [%]; Jet Energy [GeV]",
                                                          m_bins_cent, m_cent_low, m_cent_high,
                                                          bins_energy, energy_low, energy_high);

  m_hists2D["h2EMCal_OHCal"] = std::make_unique<TH2F>("h2EMCal_OHCal", "|z| < 10 cm and MB; EMCal Total Energy [GeV]; OHCal Total Energy [GeV]",
                                                    bins_Calo_E, Calo_E_low, Calo_E_high,
                                                    bins_ohcal_E, ohcal_E_low, ohcal_E_high);

  m_hists2D["h2IHCal_OHCal"] = std::make_unique<TH2F>("h2IHCal_OHCal", "|z| < 10 cm and MB; IHCal Total Energy [GeV]; OHCal Total Energy [GeV]",
                                                    bins_ihcal_E, ihcal_E_low, ihcal_E_high,
                                                    bins_ohcal_E, ohcal_E_low, ohcal_E_high);

  m_hists2D["h2JetPtEnergy"] = std::make_unique<TH2F>("h2JetPtEnergy", "Jets; Jet p_{T} [GeV]; Jet Energy [GeV]",
                                                      bins_pt, pt_low, pt_high,
                                                      bins_energy, energy_low, energy_high);

  m_hists2D["h2CaloEJetPt"] = std::make_unique<TH2F>("h2CaloEJetPt", "Jets; Total Calorimeter Energy [GeV]; Jet p_{T} [GeV]",
                                                     bins_Calo_E, Calo_E_low, Calo_E_high,
                                                     bins_pt, pt_low, pt_high);

  m_hists1D["hCentrality"] = std::make_unique<TH1F>("hCentrality", "Centrality: |z| < 10 cm and MB; Centrality [%]; Events", m_bins_cent, m_cent_low, m_cent_high);
  m_hists2D["h2CentralityZVtx"] = std::make_unique<TH2F>("h2CentralityZVtx", "Min. Bias; z [cm]; Centrality [%]",
                                                          bins_zvtx, zvtx_low, zvtx_high,
                                                          m_bins_cent, m_cent_low, m_cent_high);

  m_hists2D["h2CaloECentrality_default"] = std::make_unique<TH2F>("h2CaloECentrality_default", "|z| < 10 cm and MB; Total Calorimeter Energy [GeV]; Centrality [%]", bins_Calo_E, Calo_E_low, Calo_E_high, m_bins_cent, m_cent_low, m_cent_high);

  m_hists2D["h2CaloECentrality"] = std::make_unique<TH2F>("h2CaloECentrality", "|z| < 10 cm and MB; Total Calorimeter Energy [GeV]; Centrality [%]", bins_Calo_E, Calo_E_low, Calo_E_high, m_bins_cent, m_cent_low, m_cent_high);

  for (unsigned int i = 0; i < m_eventType.size(); ++i)
  {
    m_hists1D["hEvent"]->GetXaxis()->SetBinLabel(static_cast<int>(i + 1), m_eventType[i].c_str());
  }

  m_hists.hEvent = m_hists1D["hEvent"].get();

  m_hists.hCentralityCaloFail = m_hists1D["hCentralityCaloFail"].get();

  m_hists.hJetPt_raw = m_hists1D["hJetPt_raw"].get();
  m_hists.hJetPtv2_raw = m_hists1D["hJetPtv2_raw"].get();

  m_hists.hJetPt = m_hists1D["hJetPt"].get();
  m_hists.hJetPtv2 = m_hists1D["hJetPtv2"].get();

  m_hists.h2JetPhiPt = m_hists2D["h2JetPhiPt"].get();
  m_hists.h2JetPhiEta = m_hists2D["h2JetPhiEta"].get();

  m_hists.h2JetPhiPtv2 = m_hists2D["h2JetPhiPtv2"].get();
  m_hists.h2JetPhiEtav2 = m_hists2D["h2JetPhiEtav2"].get();

  m_hists.h2CentralityJetPt = m_hists2D["h2CentralityJetPt"].get();
  m_hists.h2CentralityJetEnergy = m_hists2D["h2CentralityJetEnergy"].get();

  m_hists.h2CaloECentrality_default = m_hists2D["h2CaloECentrality_default"].get();
  m_hists.h2CaloECentrality = m_hists2D["h2CaloECentrality"].get();
  m_hists.hCentrality = m_hists1D["hCentrality"].get();
  m_hists.h2CentralityZVtx = m_hists2D["h2CentralityZVtx"].get();

  m_hists.h2JetPtEnergy = m_hists2D["h2JetPtEnergy"].get();
  m_hists.h2CaloEJetPt = m_hists2D["h2CaloEJetPt"].get();

  m_hists.h2EMCal_OHCal = m_hists2D["h2EMCal_OHCal"].get();
  m_hists.h2IHCal_OHCal = m_hists2D["h2IHCal_OHCal"].get();

  // Enable Sumw2
  auto enable = [](auto&... maps)
  {
    auto loop = [](auto& map)
    {
      for (auto& [key, hist] : map)
      {
        if (hist)
        {
          hist->Sumw2();
        }
      }
    };
    (loop(maps), ...);
  };

  enable(m_hists1D, m_hists2D, m_profiles, m_profiles2D);
}

void JetAnalysisv2::process_jets() const
{
  if (!m_event_data.pass_calo_mbd)
  {
    return;
  }

  double cent = m_event_data.event_centrality;

  double total_EMCal = m_event_data.event_EMCal_Energy;
  double total_IHCal = m_event_data.event_IHCal_Energy;
  double total_OHCal = m_event_data.event_OHCal_Energy;
  double total_energy = total_EMCal + total_IHCal + total_OHCal;

  // Loop over all jets
  for (size_t idx = 0; idx < m_event_data.jet_pt->size(); ++idx)
  {
    double energy = m_event_data.jet_energy->at(idx);
    double pt = m_event_data.jet_pt_calib->at(idx);
    double pt_raw = m_event_data.jet_pt->at(idx);
    double phi = m_event_data.jet_phi->at(idx);
    double eta = m_event_data.jet_eta->at(idx);

    if (pt < m_jet_pt_min || std::abs(eta) >= m_jet_eta_max)
    {
      continue;
    }

    // map [-pi,pi] -> [0,2pi]
    if (phi < 0)
    {
      phi += 2.0 * std::numbers::pi;
    }

    m_hists.hJetPt->Fill(pt);
    m_hists.hJetPt_raw->Fill(pt_raw);
    m_hists.h2JetPhiPt->Fill(phi, pt);
    m_hists.h2JetPhiEta->Fill(phi, eta);
    m_hists.h2JetPtEnergy->Fill(pt, energy);
    m_hists.h2CentralityJetEnergy->Fill(cent, energy);

    if (energy > 0)
    {
      m_hists.h2CaloEJetPt->Fill(total_energy, pt);
      m_hists.h2CentralityJetPt->Fill(cent, pt);

      m_hists.hJetPtv2->Fill(pt);
      m_hists.hJetPtv2_raw->Fill(pt_raw);
      m_hists.h2JetPhiPtv2->Fill(phi, pt);
      m_hists.h2JetPhiEtav2->Fill(phi, eta);
    }
  }
}

bool JetAnalysisv2::check_CaloMBD() const
{
  double total_EMCal = m_event_data.event_EMCal_Energy;
  double total_IHCal = m_event_data.event_IHCal_Energy;
  double total_OHCal = m_event_data.event_OHCal_Energy;
  double total_energy = total_EMCal + total_IHCal + total_OHCal;

  double cent = m_event_data.event_centrality;

  int bin = m_hists.hCaloECentrality_min->FindBin(cent);

  double CaloE_min = m_hists.hCaloECentrality_min->GetBinContent(bin);
  double CaloE_max = m_hists.hCaloECentrality_max->GetBinContent(bin);

  bool pass = total_energy > CaloE_min && total_energy < CaloE_max;

  m_hists.h2CaloECentrality_default->Fill(total_energy, cent);

  if (pass)
  {
    m_hists.h2CaloECentrality->Fill(total_energy, cent);
  }

  return pass;
}

void JetAnalysisv2::process_calo()
{
  double total_EMCal = m_event_data.event_EMCal_Energy;
  double total_IHCal = m_event_data.event_IHCal_Energy;
  double total_OHCal = m_event_data.event_OHCal_Energy;

  m_hists.h2EMCal_OHCal->Fill(total_EMCal, total_OHCal);
  m_hists.h2IHCal_OHCal->Fill(total_IHCal, total_OHCal);
}

void JetAnalysisv2::process_event_check()
{
  auto& ed = m_event_data;

  double cent = ed.event_centrality;

  ed.pass_calo_mbd = check_CaloMBD();

  m_hists.hEvent->Fill(static_cast<std::uint8_t>(EventType::ZVTX10_MB));

  if (ed.pass_calo_mbd)
  {
    m_hists.hEvent->Fill(static_cast<std::uint8_t>(EventType::CALOCENT));
  }

  if (!ed.pass_calo_mbd)
  {
    m_hists.hCentralityCaloFail->Fill(cent);
    ++m_ctr["events_bad_calo_mbd"];
  }
}

void JetAnalysisv2::process_events()
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

    if (event % 100000 == 0)
    {
      std::cout << std::format("Processing {}/{}: {:.2f} %", event, n_entries, static_cast<double>(event) * 100. / static_cast<double>(n_entries)) << std::endl;
    }

    double cent = m_event_data.event_centrality;
    double zvtx = m_event_data.event_zvertex;
    m_hists.hCentrality->Fill(cent);
    m_hists.h2CentralityZVtx->Fill(zvtx, cent);

    // Event Selections
    process_event_check();

    // Calo QA
    process_calo();

    // Jets
    process_jets();
  }

  int jets = static_cast<int>(m_hists.h2JetPhiEta->GetEntries());
  int jets_positive_energy = static_cast<int>(m_hists.h2JetPhiEtav2->GetEntries());


  std::cout << std::format("{:#<20}\n", "");
  std::cout << "Events Bad" << std::endl;
  for (const auto& [name, events] : m_ctr)
  {
    if (name.starts_with("events_bad"))
    {
      std::cout << std::format("{}: {}\n", name, events);
    }
  }
  std::cout << std::format("{:#<20}\n", "");

  std::cout << std::format("Jets: {}, Positive Energy: {}, {:.2f}%", jets
                                                                   , jets_positive_energy, jets_positive_energy * 100. / jets) << std::endl;

  std::cout << "Finished... process_events" << std::endl;
}

void JetAnalysisv2::save_results() const
{
  std::filesystem::create_directories(m_output_dir);

  std::filesystem::path input_path(m_input_file);
  std::string output_stem = input_path.stem().string();
  std::string output_filename = std::format("{}/Jet-Ana_{}.root", m_output_dir, output_stem);

  auto output_file = std::make_unique<TFile>(output_filename.c_str(), "RECREATE");

  auto save_all = [](auto&... collections)
  {
    auto save = [](const auto& container)
    {
      for (const auto& [name, hist] : container)
      {
        if (hist->GetEntries() != 0)
        {
          std::cout << std::format("Saving: {}, Estimated Size: {} Bytes\n", name, hist->Sizeof());
          hist->Write();
        }
      }
    };
    (save(collections), ...);  // Fold expression
  };

  save_all(m_hists1D, m_hists2D, m_profiles, m_profiles2D);

  output_file->Close();

  std::cout << std::format("Results saved to: {}", output_filename) << std::endl;
}

// ====================================================================
// Main function remains clean and simple
// ====================================================================
int main(int argc, const char* const argv[])
{
  gROOT->SetBatch(true);
  TH1::AddDirectory(false);

  if (argc < 2 || argc > 7)
  {
    std::cout << "Usage: " << argv[0] << " input_file [events] [jet_pt_min] [jet_eta_max] [output_directory] [verbosity]" << std::endl;
    return 1;
  }

  int ctr = 1;
  const std::string input_file = argv[ctr++];
  long long events = (argc >= ctr+1) ? std::atoll(argv[ctr++]) : 0;
  double jet_pt_min = (argc >= ctr+1) ? std::stod(argv[ctr++]) : 10;
  double jet_eta_max = (argc >= ctr+1) ? std::stod(argv[ctr++]) : 0.9;
  std::string output_dir = (argc >= ctr+1) ? argv[ctr++] : ".";
  int verbosity = (argc >= ctr+1) ? std::atoi(argv[ctr++]) : 0;

  std::cout << std::format("{:#<20}\n", "");
  std::cout << std::format("Run Params\n");
  std::cout << std::format("Input: {}\n", input_file);
  std::cout << std::format("Events: {}\n", events);
  std::cout << std::format("Jet pT min: {} [GeV]\n", jet_pt_min);
  std::cout << std::format("Jet eta max: {}\n", jet_eta_max);
  std::cout << std::format("Output Dir: {}\n", output_dir);
  std::cout << std::format("Verbosity: {}\n", verbosity);
  std::cout << std::format("{:#<20}\n", "");

  try
  {
    JetAnalysisv2 analysis(input_file, events, output_dir);
    analysis.set_jet_pt_min(jet_pt_min);
    analysis.set_jet_eta_max(jet_eta_max);
    analysis.set_verbosity(verbosity);
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
