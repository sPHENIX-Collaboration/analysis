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
#include <random>
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
#include <TProfile2D.h>
#include <TROOT.h>
#include <TTree.h>

// ====================================================================
// The Analysis Class
// ====================================================================
class JetAnalysis
{
 public:
  // The constructor takes the configuration
  JetAnalysis(std::string input_file, unsigned int runnumber, long long events, std::string output_dir)
    : m_input_file(std::move(input_file))
    , m_runnumber(runnumber)
    , m_events_to_process(events)
    , m_output_dir(std::move(output_dir))
  {
  }

  void run()
  {
    setup_chain();
    process_dead_channels();
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

 private:
  enum class Subdetector
  {
    S,
    N,
    NS
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

  struct JetInfo
  {
    double energy{0};
    double pt{0};
    double phi{0};
    double eta{0};
  };

  static constexpr size_t m_bins_cent = 8;
  static constexpr double m_cent_low = -0.5;
  static constexpr double m_cent_high = 79.5;

  // Store harmonic orders and subdetectors for easy iteration
  // static constexpr std::array<int, 3> m_harmonics = {2, 3, 4};
  static constexpr std::array<int, 1> m_harmonics = {2};
  static constexpr std::array<Subdetector, 2> m_subdetectors = {Subdetector::S, Subdetector::N};
  static constexpr std::array<QComponent, 2> m_components = {QComponent::X, QComponent::Y};

  // Min Jet pT [GeV]
  static constexpr std::array<int, 7> m_jet_pt_min_vec = {7, 10, 12, 14, 16, 18, 20};

  struct AnalysisHists
  {
    TH2* h2Dummy{nullptr};
    TH2* h2EMCalBadTowersDeadv2{nullptr};
    TH3* h3JetPhiEtaPt{nullptr};
    TH3* h3JetPhiEtaPtv2{nullptr};
    TH3* h3JetPhiEtaPtv3{nullptr}; // Positive Jet Energy
    TH3* h3JetPhiEtaPtv4{nullptr}; // Positive Jet Energy and Calo V2 Cut
    TH2* h2Event{nullptr};
    TH2* h2Jet{nullptr};
    TH2* h2MBD{nullptr};
    TH2* h2sEPD{nullptr};
    TH3* h3EMCal_MBD_sEPD{nullptr};
    TH3* h3IHCal_MBD_sEPD{nullptr};
    TH3* h3OHCal_MBD_sEPD{nullptr};
    TH3* h3EMCal_IHCal_OHCal{nullptr};
    TH3* h3JetPtCentralityCaloV2{nullptr};
    TH3* h3JetEnergyCentralityCaloV2{nullptr};
    TH3* h3SumECaloV2Centrality{nullptr};
    TH3* h3CaloESumETowMedE{nullptr};
    TH2* h2TowMedECaloV2{nullptr};
    TH2* h2CaloETowMedE_EMCal{nullptr};
    TH2* h2SumETowMedE_EMCal{nullptr};
    TH2* h2TowMedE_EMCal_CaloV2{nullptr};
    TH3* h3JetPtEnergySumE{nullptr};
    TH3* h3JetPtEnergyCaloE{nullptr};
    TH2* h2CaloECentrality{nullptr};
    TH2* h2SumECentrality{nullptr};
    TH1* hCentrality{nullptr};

    TH2* h2SeedsCaloE{nullptr};
    TH2* h2SeedsSumE{nullptr};
    TH2* h2SeedsCentrality{nullptr};

    TH2* h2SeedsIt1CaloE{nullptr};
    TH2* h2SeedsIt1SumE{nullptr};
    TH2* h2SeedsIt1Centrality{nullptr};

    TH2* h2SeedsIt1{nullptr};
    TH2* h2CaloV2It1{nullptr};

    TH2* h2SeedsIt1CaloV2{nullptr};
    TH2* h2SeedsCaloV2{nullptr};

    TH1* hCaloECentrality_min{nullptr};
    TH1* hCaloECentrality_max{nullptr};

    std::array<TH2*, m_harmonics.size()> hPsi_S_raw{nullptr};
    std::array<TH2*, m_harmonics.size()> hPsi_N_raw{nullptr};

    std::array<TH2*, m_harmonics.size()> hPsi_S_recentered{nullptr};
    std::array<TH2*, m_harmonics.size()> hPsi_N_recentered{nullptr};

    std::array<TH2*, m_harmonics.size()> hPsi_S{nullptr};
    std::array<TH2*, m_harmonics.size()> hPsi_N{nullptr};
    std::array<TH2*, m_harmonics.size()> hPsi_NS{nullptr};

    // Profiles
    std::array<TProfile2D*, m_harmonics.size()> p2SP_re{nullptr};
    std::array<TProfile2D*, m_harmonics.size()> p2SP_im{nullptr};
    std::array<TProfile2D*, m_harmonics.size()> p2SP_res{nullptr};
    std::array<TProfile*, m_harmonics.size()> p1SP_res{nullptr};
    std::array<TProfile*, m_harmonics.size()> p1SP_evt_res{nullptr};  // Event Plane Resolution Squared
    std::array<std::array<TProfile*, m_jet_pt_min_vec.size()>, 3> p1SP_re{{{}, {}, {}}};
    std::array<std::array<TProfile*, m_jet_pt_min_vec.size()>, 3> p1SP_rev2{{{}, {}, {}}}; // Jet Energy > 0
    std::array<std::array<TProfile*, m_jet_pt_min_vec.size()>, 3> p1SP_rev3{{{}, {}, {}}}; // Jet Energy > 0 and Calo V2 Cut
    std::array<std::array<TProfile*, m_jet_pt_min_vec.size()>, 3> p1SP_re_anti{{{}, {}, {}}};

    // Event Plane Method
    std::array<std::array<TProfile*, m_jet_pt_min_vec.size()>, 3> p1EP_re{{{}, {}, {}}};

    // Q Vector - Crosschecks
    std::array<TProfile*, m_harmonics.size()> S_x_raw_avg{nullptr};
    std::array<TProfile*, m_harmonics.size()> S_y_raw_avg{nullptr};
    std::array<TProfile*, m_harmonics.size()> N_x_raw_avg{nullptr};
    std::array<TProfile*, m_harmonics.size()> N_y_raw_avg{nullptr};

    std::array<TProfile*, m_harmonics.size()> S_x_corr_avg{nullptr};
    std::array<TProfile*, m_harmonics.size()> S_y_corr_avg{nullptr};
    std::array<TProfile*, m_harmonics.size()> N_x_corr_avg{nullptr};
    std::array<TProfile*, m_harmonics.size()> N_y_corr_avg{nullptr};

    std::array<TProfile*, m_harmonics.size()> S_xx_raw_avg{nullptr};
    std::array<TProfile*, m_harmonics.size()> S_yy_raw_avg{nullptr};
    std::array<TProfile*, m_harmonics.size()> S_xy_raw_avg{nullptr};

    std::array<TProfile*, m_harmonics.size()> N_xx_raw_avg{nullptr};
    std::array<TProfile*, m_harmonics.size()> N_yy_raw_avg{nullptr};
    std::array<TProfile*, m_harmonics.size()> N_xy_raw_avg{nullptr};

    std::array<TProfile*, m_harmonics.size()> S_xx_corr_avg{nullptr};
    std::array<TProfile*, m_harmonics.size()> S_yy_corr_avg{nullptr};
    std::array<TProfile*, m_harmonics.size()> S_xy_corr_avg{nullptr};

    std::array<TProfile*, m_harmonics.size()> N_xx_corr_avg{nullptr};
    std::array<TProfile*, m_harmonics.size()> N_yy_corr_avg{nullptr};
    std::array<TProfile*, m_harmonics.size()> N_xy_corr_avg{nullptr};
  };

  AnalysisHists m_hists;

  struct EventData
  {
    friend class JetAnalysis;

   private:
    int event_id{0};
    [[maybe_unused]] double event_zvertex{0.0};
    double event_centrality{0.0};
    double event_MBD_Charge_South{0.0};
    double event_MBD_Charge_North{0.0};
    double event_sEPD_Charge_South{0.0};
    double event_sEPD_Charge_North{0.0};
    double event_EMCal_Energy{0};
    double event_IHCal_Energy{0};
    double event_OHCal_Energy{0};
    double event_tower_median_Energy{0};
    double event_EMCal_tower_median_Energy{0};
    float calo_v2{0.0};
    float calo_v2_it1{0.0};
    float UE_sum_E{0.0};
    int nHIRecoSeedsSub{0};
    int nHIRecoSeedsSubIt1{0};
    bool has_good_calo_v2{false};
    double max_jet_pt{0.0};

    std::vector<double>* jet_energy{nullptr};
    std::vector<double>* jet_pt{nullptr};
    std::vector<double>* jet_phi{nullptr};
    std::vector<double>* jet_eta{nullptr};

    std::vector<int>* sepd_channel{nullptr};
    std::vector<double>* sepd_charge{nullptr};
    std::vector<double>* sepd_phi{nullptr};

    // Array for harmonics [2, 3, 4] -> indices [0, 1, 2]
    // Array for subdetectors [S, N] -> indices [0, 1]
    std::array<std::array<QVec, 2>, m_harmonics.size()> q_vectors_raw;
    std::array<std::array<QVec, 2>, m_harmonics.size()> q_vectors_recentered;
    std::array<std::array<QVec, 3>, m_harmonics.size()> q_vectors;

    double Q_S_x_raw;
    double Q_S_y_raw;
    double Q_N_x_raw;
    double Q_N_y_raw;

    double Q_S_x_recentered;
    double Q_S_y_recentered;
    double Q_N_x_recentered;
    double Q_N_y_recentered;

    double Q_S_x;
    double Q_S_y;
    double Q_N_x;
    double Q_N_y;

    double Q_NS_x;
    double Q_NS_y;
  };

  // --- Member Variables ---
  EventData m_event_data;
  std::unique_ptr<TChain> m_chain;

  // Configuration stored as members
  std::string m_input_file;
  unsigned int m_runnumber;
  long long m_events_to_process;
  std::string m_output_dir;
  std::string m_dbtag{"newcdbtag"};
  int m_bins_sample{25};

  // Calo V2 Analysis
  bool m_do_secondary_processing{true};

  // Jet Cuts
  double m_jet_pt_min{7}; /*GeV*/
  double m_jet_eta_max{0.9};

  // Calo V2 Cuts
  float m_calo_v2_max{0.48F};

  // Hists
  std::map<std::string, std::unique_ptr<TH1>> m_hists1D;
  std::map<std::string, std::unique_ptr<TH2>> m_hists2D;
  std::map<std::string, std::unique_ptr<TH3>> m_hists3D;
  std::map<std::string, std::unique_ptr<TProfile>> m_profiles;
  std::map<std::string, std::unique_ptr<TProfile2D>> m_profiles2D;

  // --- Private Helper Methods ---
  void setup_chain();

  void load_correction_data(TFile* file);
  void read_Q_calib();
  void load_calo_centrality_cuts();

  void process_dead_channels();

  void create_vn_histograms(int n);
  void init_hists();

  void compute_SP_resolution(int sample);
  void compute_SP(int sample);
  std::vector<JetInfo> process_jets() const;
  void process_QVecs();
  bool check_CaloMBD() const;
  void process_calo();
  void process_events();

  void save_results() const;
};

// ====================================================================
// Implementation of the Class Methods
// ====================================================================
void JetAnalysis::setup_chain()
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
  std::unordered_set<std::string> branchNames = {"event_id", "event_centrality",
                                                 "event_MBD_Charge_South", "event_MBD_Charge_North",
                                                 "event_sEPD_Charge_South", "event_sEPD_Charge_North",
                                                 "event_EMCal_Energy", "event_IHCal_Energy", "event_OHCal_Energy",
                                                 "event_tower_median_Energy", "event_EMCal_tower_median_Energy",
                                                 "nHIRecoSeedsSub", "nHIRecoSeedsSubIt1",
                                                 "jet_phi", "jet_eta", "jet_pt", "jet_energy", "max_jet_pt",
                                                 "sepd_channel", "sepd_charge", "sepd_phi"};

  std::unordered_set<std::string> branchNames_secondary = {"calo_v2", "calo_v2_it1", "UE_sum_E"
                                                         , "Q_S_x_2_raw", "Q_S_y_2_raw", "Q_N_x_2_raw", "Q_N_y_2_raw"
                                                         , "Q_S_x_2_recentered", "Q_S_y_2_recentered", "Q_N_x_2_recentered", "Q_N_y_2_recentered"
                                                         , "Q_S_x_2", "Q_S_y_2", "Q_N_x_2", "Q_N_y_2"
                                                         , "Q_NS_x_2", "Q_NS_y_2"};

  // Append the secondary branch names to the initial
  branchNames.insert(branchNames_secondary.begin(), branchNames_secondary.end());

  // Check Branch Status
  for(const auto& branchName : branchNames)
  {
    TBranch* branch = m_chain->GetBranch(branchName.c_str());
    if (branch)
    {
      // Branch exists: Enable it
      m_chain->SetBranchStatus(branchName.c_str(), true);
    }
    else if (branchNames_secondary.contains(branchName))
    {
      m_do_secondary_processing = false;
      std::cout << std::format("Missing Branch: {}, Disable Secondary Processing\n", branchName);
    }
    else
    {
      throw std::runtime_error(std::format("Could not find Branch '{}' in file '{}'", branchName, m_input_file));
    }
  }

  // Set branches to variables
  m_chain->SetBranchAddress("event_id", &m_event_data.event_id);
  m_chain->SetBranchAddress("event_centrality", &m_event_data.event_centrality);
  m_chain->SetBranchAddress("event_MBD_Charge_South", &m_event_data.event_MBD_Charge_South);
  m_chain->SetBranchAddress("event_MBD_Charge_North", &m_event_data.event_MBD_Charge_North);
  m_chain->SetBranchAddress("event_sEPD_Charge_South", &m_event_data.event_sEPD_Charge_South);
  m_chain->SetBranchAddress("event_sEPD_Charge_North", &m_event_data.event_sEPD_Charge_North);
  m_chain->SetBranchAddress("event_EMCal_Energy", &m_event_data.event_EMCal_Energy);
  m_chain->SetBranchAddress("event_IHCal_Energy", &m_event_data.event_IHCal_Energy);
  m_chain->SetBranchAddress("event_OHCal_Energy", &m_event_data.event_OHCal_Energy);
  m_chain->SetBranchAddress("event_tower_median_Energy", &m_event_data.event_tower_median_Energy);
  m_chain->SetBranchAddress("event_EMCal_tower_median_Energy", &m_event_data.event_EMCal_tower_median_Energy);
  m_chain->SetBranchAddress("max_jet_pt", &m_event_data.max_jet_pt);
  m_chain->SetBranchAddress("jet_pt", &m_event_data.jet_pt);
  m_chain->SetBranchAddress("jet_energy", &m_event_data.jet_energy);
  m_chain->SetBranchAddress("jet_phi", &m_event_data.jet_phi);
  m_chain->SetBranchAddress("jet_eta", &m_event_data.jet_eta);
  m_chain->SetBranchAddress("sepd_channel", &m_event_data.sepd_channel);
  m_chain->SetBranchAddress("sepd_charge", &m_event_data.sepd_charge);
  m_chain->SetBranchAddress("sepd_phi", &m_event_data.sepd_phi);

  m_chain->SetBranchAddress("nHIRecoSeedsSub", &m_event_data.nHIRecoSeedsSub);
  m_chain->SetBranchAddress("nHIRecoSeedsSubIt1", &m_event_data.nHIRecoSeedsSubIt1);

  if (m_do_secondary_processing)
  {
    m_chain->SetBranchAddress("calo_v2", &m_event_data.calo_v2);
    m_chain->SetBranchAddress("calo_v2_it1", &m_event_data.calo_v2_it1);
    m_chain->SetBranchAddress("UE_sum_E", &m_event_data.UE_sum_E);

    m_chain->SetBranchAddress("Q_S_x_2_raw", &m_event_data.Q_S_x_raw);
    m_chain->SetBranchAddress("Q_S_y_2_raw", &m_event_data.Q_S_y_raw);
    m_chain->SetBranchAddress("Q_N_x_2_raw", &m_event_data.Q_N_x_raw);
    m_chain->SetBranchAddress("Q_N_y_2_raw", &m_event_data.Q_N_y_raw);

    m_chain->SetBranchAddress("Q_S_x_2_recentered", &m_event_data.Q_S_x_recentered);
    m_chain->SetBranchAddress("Q_S_y_2_recentered", &m_event_data.Q_S_y_recentered);
    m_chain->SetBranchAddress("Q_N_x_2_recentered", &m_event_data.Q_N_x_recentered);
    m_chain->SetBranchAddress("Q_N_y_2_recentered", &m_event_data.Q_N_y_recentered);

    m_chain->SetBranchAddress("Q_S_x_2", &m_event_data.Q_S_x);
    m_chain->SetBranchAddress("Q_S_y_2", &m_event_data.Q_S_y);
    m_chain->SetBranchAddress("Q_N_x_2", &m_event_data.Q_N_x);
    m_chain->SetBranchAddress("Q_N_y_2", &m_event_data.Q_N_y);

    m_chain->SetBranchAddress("Q_NS_x_2", &m_event_data.Q_NS_x);
    m_chain->SetBranchAddress("Q_NS_y_2", &m_event_data.Q_NS_y);
  }

  std::cout << "Finished... setup_chain" << std::endl;
}

void JetAnalysis::load_calo_centrality_cuts()
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

  m_hists.hCaloECentrality_min = dynamic_cast<TH1*>(file->Get(hLow_name.c_str()));
  m_hists.hCaloECentrality_max = dynamic_cast<TH1*>(file->Get(hHigh_name.c_str()));
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

void JetAnalysis::create_vn_histograms(int n)
{
  double sample_low = -0.5;
  double sample_high = m_bins_sample - 0.5;

  int bins_SP = 400;
  double SP_low = -1;
  double SP_high = 1;

  int bins_psi = 126;
  double psi_low = -std::numbers::pi;
  double psi_high = std::numbers::pi;

  // TH3 for Scalar Product
  std::string name = std::format("h3SP_re_{}", n);
  std::string title = std::format("Scalar Product (Order {}); Centrality [%]; Sample; SP", n);

  m_hists3D[name] = std::make_unique<TH3F>(name.c_str(), title.c_str(), m_bins_cent, m_cent_low, m_cent_high, m_bins_sample, sample_low, sample_high, bins_SP, SP_low, SP_high);

  std::string name_im = std::format("h3SP_im_{}", n);
  m_hists3D[name_im] = std::unique_ptr<TH3>(static_cast<TH3*>(m_hists3D[name]->Clone(name_im.c_str())));

  // TProfile2D for Scalar Product
  std::string name_re_prof = std::format("h2SP_re_prof_{}", n);
  std::string name_im_prof = std::format("h2SP_im_prof_{}", n);

  title = std::format("Scalar Product (Order {}); Centrality [%]; Sample", n);
  m_profiles2D[name_re_prof] = std::make_unique<TProfile2D>(name_re_prof.c_str(), title.c_str(), m_bins_cent, m_cent_low, m_cent_high, m_bins_sample, sample_low, sample_high);
  m_profiles2D[name_im_prof] = std::unique_ptr<TProfile2D>(static_cast<TProfile2D*>(m_profiles2D[name_re_prof]->Clone(name_im_prof.c_str())));

  // TProfile for Scalar Product
  for (auto pt : m_jet_pt_min_vec)
  {
    name_re_prof = std::format("hSP_re_prof_{}_{}", n, pt);

    title = std::format("Scalar Product; Centrality [%]; Re(#LTq_{{{0}}} Q^{{S|N*}}_{{{0}}}#GT)", n);
    m_profiles[name_re_prof] = std::make_unique<TProfile>(name_re_prof.c_str(), title.c_str(), m_bins_cent, m_cent_low, m_cent_high);

    // with jet energy > 0
    name_re_prof = std::format("hSP_re_profv2_{}_{}", n, pt);

    title = std::format("Scalar Product; Centrality [%]; Re(#LTq_{{{0}}} Q^{{S|N*}}_{{{0}}}#GT)", n);
    m_profiles[name_re_prof] = std::make_unique<TProfile>(name_re_prof.c_str(), title.c_str(), m_bins_cent, m_cent_low, m_cent_high);

    if(m_do_secondary_processing)
    {
      // With jet energy > 0 and calo v2 cut
      name_re_prof = std::format("hSP_re_profv3_{}_{}", n, pt);

      title = std::format("Scalar Product; Centrality [%]; Re(#LTq_{{{0}}} Q^{{S|N*}}_{{{0}}}#GT)", n);
      m_profiles[name_re_prof] = std::make_unique<TProfile>(name_re_prof.c_str(), title.c_str(), m_bins_cent, m_cent_low, m_cent_high);
    }

    name_re_prof = std::format("hSP_re_anti_prof_{}_{}", n, pt);

    title = std::format("Scalar Product; Centrality [%]; Re(#LTq_{{{0}}} Q^{{N|S*}}_{{{0}}}#GT)", n);
    m_profiles[name_re_prof] = std::make_unique<TProfile>(name_re_prof.c_str(), title.c_str(), m_bins_cent, m_cent_low, m_cent_high);

    // Event Plane Method
    std::string name_EP_re = std::format("hEP_re_prof_{}_{}", n, pt);

    title = std::format("Scalar Product; Centrality [%]; Re(#LTq_{{{0}}} Q^{{N|S*}}_{{{0}}} / |Q^{{N|S}}_{{{0}}}|#GT)", n);
    m_profiles[name_EP_re] = std::make_unique<TProfile>(name_EP_re.c_str(), title.c_str(), m_bins_cent, m_cent_low, m_cent_high);
  }

  // TH3 for Reference Flow
  std::string name_res = std::format("h3SP_res_{}", n);
  title = std::format("sEPD (Order {0}): Q^{{S}}Q^{{N*}}; Centrality [%]; Sample; Re(Q^{{S}}_{{{0}}} Q^{{N*}}_{{{0}}}GT)", n);
  m_hists3D[name_res] = std::unique_ptr<TH3>(static_cast<TH3*>(m_hists3D[name]->Clone(name_res.c_str())));
  m_hists3D[name_res]->SetTitle(title.c_str());

  // TProfile2D for Reference Flow
  std::string name_res_prof = std::format("h2SP_res_prof_{}", n);
  title = std::format("sEPD: Re(#LT Q^{{S}}_{{{0}}} Q^{{N*}}_{{{0}}}#GT); Centrality [%]; Sample", n);
  m_profiles2D[name_res_prof] = std::make_unique<TProfile2D>(name_res_prof.c_str(), title.c_str(), m_bins_cent, m_cent_low, m_cent_high, m_bins_sample, sample_low, sample_high);

  // TProfile for Reference Flow
  name_res_prof = std::format("hSP_res_prof_{}", n);
  title = std::format("; Centrality [%]; Re(#LT Q^{{S}}_{{{0}}} Q^{{N*}}_{{{0}}}#GT)", n);
  m_profiles[name_res_prof] = std::make_unique<TProfile>(name_res_prof.c_str(), title.c_str(), m_bins_cent, m_cent_low, m_cent_high);

  std::string name_evt_res_prof = std::format("hSP_evt_res_prof_{}", n);
  title = std::format("; Centrality [%]; #LTRe(Q^{{S}}_{{{0}}} Q^{{N*}}_{{{0}}}) / (|Q^{{S}}_{{{0}}}||Q^{{N}}_{{{0}}}|)#GT", n);
  m_profiles[name_evt_res_prof] = std::make_unique<TProfile>(name_evt_res_prof.c_str(), title.c_str(), m_bins_cent, m_cent_low, m_cent_high);

  std::string psi_hist_S_title = std::format("sEPD #Psi (Order {0}): |z| < 10 cm and MB; {0}#Psi^{{S}}_{{{0}}}; Centrality [%]", n);
  std::string psi_hist_N_title = std::format("sEPD #Psi (Order {0}): |z| < 10 cm and MB; {0}#Psi^{{N}}_{{{0}}}; Centrality [%]", n);
  std::string psi_hist_NS_title = std::format("sEPD #Psi (Order {0}): |z| < 10 cm and MB; {0}#Psi^{{N,S}}_{{{0}}}; Centrality [%]", n);

  std::string psi_hist_name = std::format("h2_sEPD_Psi_S_{}_raw", n);
  m_hists2D[psi_hist_name] = std::make_unique<TH2F>(psi_hist_name.c_str(),
                                                    psi_hist_S_title.c_str(),
                                                    bins_psi, psi_low, psi_high,
                                                    m_bins_cent, m_cent_low, m_cent_high);

  psi_hist_name = std::format("h2_sEPD_Psi_N_{}_raw", n);
  m_hists2D[psi_hist_name] = std::make_unique<TH2F>(psi_hist_name.c_str(),
                                                    psi_hist_N_title.c_str(),
                                                    bins_psi, psi_low, psi_high,
                                                    m_bins_cent, m_cent_low, m_cent_high);

  psi_hist_name = std::format("h2_sEPD_Psi_S_{}_recentered", n);
  m_hists2D[psi_hist_name] = std::make_unique<TH2F>(psi_hist_name.c_str(),
                                                    psi_hist_S_title.c_str(),
                                                    bins_psi, psi_low, psi_high,
                                                    m_bins_cent, m_cent_low, m_cent_high);

  psi_hist_name = std::format("h2_sEPD_Psi_N_{}_recentered", n);
  m_hists2D[psi_hist_name] = std::make_unique<TH2F>(psi_hist_name.c_str(),
                                                    psi_hist_N_title.c_str(),
                                                    bins_psi, psi_low, psi_high,
                                                    m_bins_cent, m_cent_low, m_cent_high);

  psi_hist_name = std::format("h2_sEPD_Psi_S_{}_flat", n);
  m_hists2D[psi_hist_name] = std::make_unique<TH2F>(psi_hist_name.c_str(),
                                                    psi_hist_S_title.c_str(),
                                                    bins_psi, psi_low, psi_high,
                                                    m_bins_cent, m_cent_low, m_cent_high);

  psi_hist_name = std::format("h2_sEPD_Psi_N_{}_flat", n);
  m_hists2D[psi_hist_name] = std::make_unique<TH2F>(psi_hist_name.c_str(),
                                                    psi_hist_N_title.c_str(),
                                                    bins_psi, psi_low, psi_high,
                                                    m_bins_cent, m_cent_low, m_cent_high);

  psi_hist_name = std::format("h2_sEPD_Psi_NS_{}_flat", n);
  m_hists2D[psi_hist_name] = std::make_unique<TH2F>(psi_hist_name.c_str(),
                                                    psi_hist_NS_title.c_str(),
                                                    bins_psi, psi_low, psi_high,
                                                    m_bins_cent, m_cent_low, m_cent_high);

  // South, North
  for (auto det : m_subdetectors)
  {
    std::string det_str = (det == Subdetector::S) ? "S" : "N";
    std::string det_name = (det == Subdetector::S) ? "South" : "North";

    std::string q_avg_sq_cross_raw_name = std::format("h_sEPD_Q_{}_xy_{}_raw_avg", det_str, n);

    m_profiles[q_avg_sq_cross_raw_name] = std::make_unique<TProfile>(q_avg_sq_cross_raw_name.c_str(),
                                                                     std::format("sEPD {0}; Centrality [%]; <Q_{{{1},x}} Q_{{{1},y}}>", det_name, n).c_str(),
                                                                     m_bins_cent, m_cent_low, m_cent_high);

    std::string q_avg_sq_cross_corr_name = std::format("h_sEPD_Q_{}_xy_{}_corr_avg", det_str, n);

    m_profiles[q_avg_sq_cross_corr_name] = std::make_unique<TProfile>(q_avg_sq_cross_corr_name.c_str(),
                                                                      std::format("sEPD {0}; Centrality [%]; <Q_{{{1},x}} Q_{{{1},y}}>", det_name, n).c_str(),
                                                                      m_bins_cent, m_cent_low, m_cent_high);
    // X, Y
    for (auto comp : m_components)
    {
      std::string comp_str = (comp == QComponent::X) ? "x" : "y";
      std::string q_avg_raw_name = std::format("h_sEPD_Q_{}_{}_{}_raw_avg", det_str, comp_str, n);
      std::string q_avg_corr_name = std::format("h_sEPD_Q_{}_{}_{}_corr_avg", det_str, comp_str, n);
      std::string q_avg_sq_raw_name = std::format("h_sEPD_Q_{0}_{1}{1}_{2}_raw_avg", det_str, comp_str, n);
      std::string q_avg_sq_corr_name = std::format("h_sEPD_Q_{0}_{1}{1}_{2}_corr_avg", det_str, comp_str, n);

      m_profiles[q_avg_raw_name] = std::make_unique<TProfile>(q_avg_raw_name.c_str(),
                                                              std::format("sEPD {}; Centrality [%]; <Q_{{{},{}}}>", det_name, n, comp_str).c_str(),
                                                              m_bins_cent, m_cent_low, m_cent_high);

      m_profiles[q_avg_corr_name] = std::make_unique<TProfile>(q_avg_corr_name.c_str(),
                                                               std::format("sEPD {}; Centrality [%]; <Q_{{{},{}}}>", det_name, n, comp_str).c_str(),
                                                               m_bins_cent, m_cent_low, m_cent_high);

      m_profiles[q_avg_sq_raw_name] = std::make_unique<TProfile>(q_avg_sq_raw_name.c_str(),
                                                                 std::format("sEPD {}; Centrality [%]; <Q_{{{},{}}}^{{2}}>", det_name, n, comp_str).c_str(),
                                                                 m_bins_cent, m_cent_low, m_cent_high);

      m_profiles[q_avg_sq_corr_name] = std::make_unique<TProfile>(q_avg_sq_corr_name.c_str(),
                                                                  std::format("sEPD {}; Centrality [%]; <Q_{{{},{}}}^{{2}}>", det_name, n, comp_str).c_str(),
                                                                  m_bins_cent, m_cent_low, m_cent_high);
    }
  }
}

void JetAnalysis::init_hists()
{
  int bins_phi = 64;
  double phi_low = 0;
  double phi_high = 2 * std::numbers::pi;

  int bins_eta = 24;
  double eta_low = -1.152;
  double eta_high = 1.152;

  int bins_pt = 500;
  int bins_pt_reduced = 100;
  double pt_low = 0;
  double pt_high = 500;

  int bins_cent = 80;
  double cent_low = -0.5;
  double cent_high = 79.5;

  int bins_energy = 1000;
  int bins_energy_reduced = 200;
  double energy_low = -500;
  double energy_high = 500;

  unsigned int bins_v2{100};
  double v2_low{-1};
  double v2_high{1};

  unsigned int bins_nHIRecoSeeds{100};
  double nHIRecoSeeds_low{0};
  double nHIRecoSeeds_high{100};

  unsigned int bins_sum_E{540};
  double sum_E_low{-2e2};
  double sum_E_high{2.5e3};

  unsigned int bins_Calo_E{540};
  double Calo_E_low{-2e2};
  double Calo_E_high{2.5e3};

  unsigned int bins_sepd_total_charge{200};
  double sepd_total_charge_low{0};
  double sepd_total_charge_high{4e4};

  unsigned int bins_sepd_charge{100};
  double sepd_charge_low{0};
  double sepd_charge_high{2e4};

  unsigned int bins_mbd_total_charge{200};
  double mbd_total_charge_low{0};
  double mbd_total_charge_high{5e3};

  unsigned int bins_mbd_charge{100};
  double mbd_charge_low{0};
  double mbd_charge_high{2.5e3};

  unsigned int bins_tower_median_energy{310};
  double tower_median_energy_low{-10};
  double tower_median_energy_high{300}; // MeV

  unsigned int bins_EMCal_tower_median_energy{1100};
  double EMCal_tower_median_energy_low{-10};
  double EMCal_tower_median_energy_high{1000}; // MeV

  double sample_low = -0.5;
  double sample_high = m_bins_sample - 0.5;

  m_hists3D["h3JetPhiEtaPt"] = std::make_unique<TH3F>("h3JetPhiEtaPt", "Jet: |z| < 10 cm and MB; #phi; #eta; p_{T} [GeV]", bins_phi, phi_low, phi_high, bins_eta, eta_low, eta_high, bins_pt, pt_low, pt_high);
  m_hists3D["h3JetPhiEtaPtv2"] = std::unique_ptr<TH3>(static_cast<TH3*>(m_hists3D["h3JetPhiEtaPt"]->Clone("h3JetPhiEtaPtv2")));
  m_hists3D["h3JetPhiEtaPtv3"] = std::unique_ptr<TH3>(static_cast<TH3*>(m_hists3D["h3JetPhiEtaPt"]->Clone("h3JetPhiEtaPtv3")));

  m_hists2D["h2MBD"] = std::make_unique<TH2F>("h2MBD", "|z| < 10 and MB; MBD South Charge; MBD North Charge", bins_mbd_charge, mbd_charge_low, mbd_charge_high, bins_mbd_charge, mbd_charge_low, mbd_charge_high);
  m_hists2D["h2sEPD"] = std::make_unique<TH2F>("h2sEPD", "|z| < 10 and MB; sEPD South Charge; sEPD North Charge", bins_sepd_charge, sepd_charge_low, sepd_charge_high, bins_sepd_charge, sepd_charge_low, sepd_charge_high);

  m_hists3D["h3EMCal_MBD_sEPD"] = std::make_unique<TH3F>("h3EMCal_MBD_sEPD", "|z| < 10 cm and MB; EMCal Total Energy [GeV]; MBD Total Charge; sEPD Total Charge", bins_Calo_E, Calo_E_low, Calo_E_high, bins_mbd_total_charge, mbd_total_charge_low, mbd_total_charge_high, bins_sepd_total_charge, sepd_total_charge_low, sepd_total_charge_high);
  m_hists3D["h3IHCal_MBD_sEPD"] = std::make_unique<TH3F>("h3IHCal_MBD_sEPD", "|z| < 10 cm and MB; IHCal Total Energy [GeV]; MBD Total Charge; sEPD Total Charge", bins_Calo_E, Calo_E_low, Calo_E_high, bins_mbd_total_charge, mbd_total_charge_low, mbd_total_charge_high, bins_sepd_total_charge, sepd_total_charge_low, sepd_total_charge_high);
  m_hists3D["h3OHCal_MBD_sEPD"] = std::make_unique<TH3F>("h3OHCal_MBD_sEPD", "|z| < 10 cm and MB; OHCal Total Energy [GeV]; MBD Total Charge; sEPD Total Charge", bins_Calo_E, Calo_E_low, Calo_E_high, bins_mbd_total_charge, mbd_total_charge_low, mbd_total_charge_high, bins_sepd_total_charge, sepd_total_charge_low, sepd_total_charge_high);
  m_hists3D["h3EMCal_IHCal_OHCal"] = std::make_unique<TH3F>("h3EMCal_IHCal_OHCal", "|z| < 10 cm and MB; EMCal Total Energy [GeV]; IHCal Total Energy [GeV]; OHCal Total Energy [GeV]", bins_Calo_E, Calo_E_low, Calo_E_high, bins_Calo_E, Calo_E_low, Calo_E_high, bins_Calo_E, Calo_E_low, Calo_E_high);
  m_hists3D["h3JetPtEnergyCaloE"] = std::make_unique<TH3F>("h3JetPtEnergyCaloE", "Jets; Jet p_{T} [GeV]; Jet Energy [GeV]; Total Calorimeter Energy [GeV]", bins_pt_reduced, pt_low, pt_high, bins_energy_reduced, energy_low, energy_high, bins_Calo_E, Calo_E_low, Calo_E_high);

  if (m_do_secondary_processing)
  {
    m_hists3D["h3JetPhiEtaPtv4"] = std::unique_ptr<TH3>(static_cast<TH3*>(m_hists3D["h3JetPhiEtaPt"]->Clone("h3JetPhiEtaPtv4")));

    m_hists3D["h3JetPtCentralityCaloV2"] = std::make_unique<TH3F>("h3JetPtCentralityCaloV2", "Jets; Jet p_{T} [GeV]; Centrality [%]; v_{2}", bins_pt, pt_low, pt_high, m_bins_cent, m_cent_low, m_cent_high, bins_v2, v2_low, v2_high);
    m_hists3D["h3JetEnergyCentralityCaloV2"] = std::make_unique<TH3F>("h3JetEnergyCentralityCaloV2", "Jets; Jet Energy [GeV]; Centrality [%]; v_{2}", bins_energy, energy_low, energy_high, m_bins_cent, m_cent_low, m_cent_high, bins_v2, v2_low, v2_high);
    m_hists3D["h3SumECaloV2Centrality"] = std::make_unique<TH3F>("h3SumECaloV2Centrality", "; Sum E [GeV]; v_{2}; Centrality [%]", bins_sum_E, sum_E_low, sum_E_high, bins_v2, v2_low, v2_high, m_bins_cent, m_cent_low, m_cent_high);
    m_hists3D["h3CaloESumETowMedE"] = std::make_unique<TH3F>("h3CaloESumETowMedE", "|z| < 10 and MB; Total Calorimeter Energy [GeV]; Sum E [GeV]; Median Tower Energy [MeV]", bins_Calo_E, Calo_E_low, Calo_E_high, bins_sum_E, sum_E_low, sum_E_high, bins_tower_median_energy, tower_median_energy_low, tower_median_energy_high);
    m_hists2D["h2TowMedECaloV2"] = std::make_unique<TH2F>("h2TowMedECaloV2", "|z| < 10 and MB; Median Tower Energy [MeV]; v_{2}", bins_tower_median_energy, tower_median_energy_low, tower_median_energy_high, bins_v2, v2_low, v2_high);

    m_hists2D["h2CaloETowMedE_EMCal"] = std::make_unique<TH2F>("h2CaloETowMedE_EMCal", "|z| < 10 and MB; EMCal Median Tower Energy [MeV]; Total Calorimeter Energy [GeV]", bins_EMCal_tower_median_energy, EMCal_tower_median_energy_low, EMCal_tower_median_energy_high, bins_Calo_E, Calo_E_low, Calo_E_high);
    m_hists2D["h2SumETowMedE_EMCal"] = std::make_unique<TH2F>("h2SumETowMedE_EMCal", "|z| < 10 and MB; EMCal Median Tower Energy [MeV]; Sum E [GeV]", bins_EMCal_tower_median_energy, EMCal_tower_median_energy_low, EMCal_tower_median_energy_high, bins_sum_E, sum_E_low, sum_E_high);
    m_hists2D["h2TowMedE_EMCal_CaloV2"] = std::make_unique<TH2F>("h2TowMedE_EMCal_CaloV2", "|z| < 10 and MB; EMCal Median Tower Energy [MeV]; v_{2}", bins_EMCal_tower_median_energy, EMCal_tower_median_energy_low, EMCal_tower_median_energy_high, bins_v2, v2_low, v2_high);

    m_hists3D["h3JetPtEnergySumE"] = std::make_unique<TH3F>("h3JetPtEnergySumE", "Jets; Jet p_{T} [GeV]; Jet Energy [GeV]; Sum E [GeV]", bins_pt_reduced, pt_low, pt_high, bins_energy_reduced, energy_low, energy_high, bins_sum_E, sum_E_low, sum_E_high);

    m_hists2D["h2SumECentrality"] = std::make_unique<TH2F>("h2SumECentrality", "|z| < 10 cm and MB; Sum E [GeV]; Centrality [%]", bins_sum_E, sum_E_low, sum_E_high, bins_cent, cent_low, cent_high);


    m_hists2D["h2SeedsIt1SumE"] = std::make_unique<TH2F>("h2SeedsIt1SumE", "|z| < 10 cm and MB; Jet Seeds [Counts]; Sum E [GeV]", bins_nHIRecoSeeds, nHIRecoSeeds_low, nHIRecoSeeds_high, bins_sum_E, sum_E_low, sum_E_high);
    m_hists2D["h2SeedsSumE"] = std::make_unique<TH2F>("h2SeedsSumE", "|z| < 10 cm and MB; Jet Seeds [Counts]; Sum E [GeV]", bins_nHIRecoSeeds, nHIRecoSeeds_low, nHIRecoSeeds_high, bins_sum_E, sum_E_low, sum_E_high);

    m_hists2D["h2CaloV2It1"] = std::make_unique<TH2F>("h2CaloV2It1", "|z| < 10 cm and MB; v_{2} It1; v_{2}", bins_v2, v2_low, v2_high, bins_v2, v2_low, v2_high);

    m_hists2D["h2SeedsIt1CaloV2"] = std::make_unique<TH2F>("h2SeedsIt1CaloV2", "|z| < 10 cm and MB; Jet Seeds [Counts]; v_{2} It1", bins_nHIRecoSeeds, nHIRecoSeeds_low, nHIRecoSeeds_high, bins_v2, v2_low, v2_high);
    m_hists2D["h2SeedsCaloV2"] = std::make_unique<TH2F>("h2SeedsCaloV2", "|z| < 10 cm and MB; Jet Seeds [Counts]; v_{2}", bins_nHIRecoSeeds, nHIRecoSeeds_low, nHIRecoSeeds_high, bins_v2, v2_low, v2_high);
  }

  // -----------
  // Jet Seeds

  m_hists2D["h2SeedsIt1Centrality"] = std::make_unique<TH2F>("h2SeedsIt1Centrality", "|z| < 10 cm and MB; Jet Seeds [Counts]; Centrality [%]", bins_nHIRecoSeeds, nHIRecoSeeds_low, nHIRecoSeeds_high, bins_cent, cent_low, cent_high);
  m_hists2D["h2SeedsCentrality"] = std::make_unique<TH2F>("h2SeedsCentrality", "|z| < 10 cm and MB; Jet Seeds [Counts]; Centrality [%]", bins_nHIRecoSeeds, nHIRecoSeeds_low, nHIRecoSeeds_high, bins_cent, cent_low, cent_high);

  m_hists2D["h2SeedsIt1CaloE"] = std::make_unique<TH2F>("h2SeedsIt1CaloE", "|z| < 10 cm and MB; Jet Seeds [Counts]; Total Calorimeter Energy [GeV]", bins_nHIRecoSeeds, nHIRecoSeeds_low, nHIRecoSeeds_high, bins_Calo_E, Calo_E_low, Calo_E_high);
  m_hists2D["h2SeedsCaloE"] = std::make_unique<TH2F>("h2SeedsCaloE", "|z| < 10 cm and MB; Jet Seeds [Counts]; Total Calorimeter Energy [GeV]", bins_nHIRecoSeeds, nHIRecoSeeds_low, nHIRecoSeeds_high, bins_Calo_E, Calo_E_low, Calo_E_high);

  m_hists2D["h2SeedsIt1"] = std::make_unique<TH2F>("h2SeedsIt1", "|z| < 10 cm and MB; Jet Seeds It1 [Counts]; Jet Seeds [Counts]", bins_nHIRecoSeeds, nHIRecoSeeds_low, nHIRecoSeeds_high, bins_nHIRecoSeeds, nHIRecoSeeds_low, nHIRecoSeeds_high);

  // -----------

  m_hists2D["h2Event"] = std::make_unique<TH2F>("h2Event", "Events: |z| < 10 and MB; Centrality [%]; Sample", m_bins_cent, m_cent_low, m_cent_high, m_bins_sample, sample_low, sample_high);
  m_hists2D["h2Jet"] = std::make_unique<TH2F>("h2Jet", "Jets; Centrality [%]; Sample", m_bins_cent, m_cent_low, m_cent_high, m_bins_sample, sample_low, sample_high);

  m_hists1D["hCentrality"] = std::make_unique<TH1F>("hCentrality", "Centrality: |z| < 10 cm and MB; Centrality [%]; Events", m_bins_cent, m_cent_low, m_cent_high);

  m_hists2D["h2CaloECentrality"] = std::make_unique<TH2F>("h2CaloECentrality", "|z| < 10 cm and MB; Total Calorimeter Energy [GeV]; Centrality [%]", bins_Calo_E, Calo_E_low, Calo_E_high, bins_cent, cent_low, cent_high);

  for (auto n : m_harmonics)
  {
    create_vn_histograms(n);
  }

  m_hists2D["h2Dummy"] = std::unique_ptr<TH2>(static_cast<TH2*>(m_hists3D["h3JetPhiEtaPt"]->Project3D("yx")->Clone("h2Dummy")));
  m_hists2D["h2Dummy"]->Rebin2D(2, 2);

  m_hists.h2Dummy = m_hists2D["h2Dummy"].get();
  m_hists.h2EMCalBadTowersDeadv2 = m_hists2D["h2EMCalBadTowersDeadv2"].get();
  m_hists.h3JetPhiEtaPt = m_hists3D["h3JetPhiEtaPt"].get();
  m_hists.h3JetPhiEtaPtv2 = m_hists3D["h3JetPhiEtaPtv2"].get();
  m_hists.h3JetPhiEtaPtv3 = m_hists3D["h3JetPhiEtaPtv3"].get();
  m_hists.h2Event = m_hists2D["h2Event"].get();
  m_hists.h2Jet = m_hists2D["h2Jet"].get();
  m_hists.h2CaloECentrality = m_hists2D["h2CaloECentrality"].get();
  m_hists.hCentrality = m_hists1D["hCentrality"].get();
  m_hists.h3JetPtEnergyCaloE = m_hists3D["h3JetPtEnergyCaloE"].get();

  m_hists.h2MBD = m_hists2D["h2MBD"].get();
  m_hists.h2sEPD = m_hists2D["h2sEPD"].get();

  m_hists.h3EMCal_MBD_sEPD = m_hists3D["h3EMCal_MBD_sEPD"].get();
  m_hists.h3IHCal_MBD_sEPD = m_hists3D["h3IHCal_MBD_sEPD"].get();
  m_hists.h3OHCal_MBD_sEPD = m_hists3D["h3OHCal_MBD_sEPD"].get();
  m_hists.h3EMCal_IHCal_OHCal = m_hists3D["h3EMCal_IHCal_OHCal"].get();

  m_hists.h2SeedsIt1 = m_hists2D["h2SeedsIt1"].get();
  m_hists.h2SeedsIt1CaloE = m_hists2D["h2SeedsIt1CaloE"].get();
  m_hists.h2SeedsCaloE = m_hists2D["h2SeedsCaloE"].get();
  m_hists.h2SeedsIt1Centrality = m_hists2D["h2SeedsIt1Centrality"].get();
  m_hists.h2SeedsCentrality = m_hists2D["h2SeedsCentrality"].get();

  if (m_do_secondary_processing)
  {
    m_hists.h3JetPhiEtaPtv4 = m_hists3D["h3JetPhiEtaPtv4"].get();
    m_hists.h3JetPtCentralityCaloV2 = m_hists3D["h3JetPtCentralityCaloV2"].get();
    m_hists.h3JetEnergyCentralityCaloV2 = m_hists3D["h3JetEnergyCentralityCaloV2"].get();
    m_hists.h3SumECaloV2Centrality = m_hists3D["h3SumECaloV2Centrality"].get();
    m_hists.h3CaloESumETowMedE = m_hists3D["h3CaloESumETowMedE"].get();
    m_hists.h2TowMedECaloV2 = m_hists2D["h2TowMedECaloV2"].get();
    m_hists.h2CaloETowMedE_EMCal = m_hists2D["h2CaloETowMedE_EMCal"].get();
    m_hists.h2SumETowMedE_EMCal = m_hists2D["h2SumETowMedE_EMCal"].get();
    m_hists.h2TowMedE_EMCal_CaloV2 = m_hists2D["h2TowMedE_EMCal_CaloV2"].get();
    m_hists.h3JetPtEnergySumE = m_hists3D["h3JetPtEnergySumE"].get();
    m_hists.h2SumECentrality = m_hists2D["h2SumECentrality"].get();

    m_hists.h2SeedsSumE = m_hists2D["h2SeedsSumE"].get();
    m_hists.h2SeedsIt1SumE = m_hists2D["h2SeedsIt1SumE"].get();

    m_hists.h2CaloV2It1 = m_hists2D["h2CaloV2It1"].get();

    m_hists.h2SeedsIt1CaloV2 = m_hists2D["h2SeedsIt1CaloV2"].get();
    m_hists.h2SeedsCaloV2 = m_hists2D["h2SeedsCaloV2"].get();
  }

  for (size_t n_idx = 0; n_idx < m_harmonics.size(); ++n_idx)
  {
    int n = m_harmonics[n_idx];

    m_hists.hPsi_S_raw[n_idx] = m_hists2D[std::format("h2_sEPD_Psi_S_{}_raw", n)].get();
    m_hists.hPsi_N_raw[n_idx] = m_hists2D[std::format("h2_sEPD_Psi_N_{}_raw", n)].get();

    m_hists.hPsi_S_recentered[n_idx] = m_hists2D[std::format("h2_sEPD_Psi_S_{}_recentered", n)].get();
    m_hists.hPsi_N_recentered[n_idx] = m_hists2D[std::format("h2_sEPD_Psi_N_{}_recentered", n)].get();

    m_hists.hPsi_S[n_idx] = m_hists2D[std::format("h2_sEPD_Psi_S_{}_flat", n)].get();
    m_hists.hPsi_N[n_idx] = m_hists2D[std::format("h2_sEPD_Psi_N_{}_flat", n)].get();
    m_hists.hPsi_NS[n_idx] = m_hists2D[std::format("h2_sEPD_Psi_NS_{}_flat", n)].get();

    m_hists.h3SP_re[n_idx] = m_hists3D[std::format("h3SP_re_{}", n)].get();
    m_hists.h3SP_im[n_idx] = m_hists3D[std::format("h3SP_im_{}", n)].get();
    m_hists.h3SP_res[n_idx] = m_hists3D[std::format("h3SP_res_{}", n)].get();
    m_hists.p2SP_re[n_idx] = m_profiles2D[std::format("h2SP_re_prof_{}", n)].get();
    m_hists.p2SP_im[n_idx] = m_profiles2D[std::format("h2SP_im_prof_{}", n)].get();
    m_hists.p2SP_res[n_idx] = m_profiles2D[std::format("h2SP_res_prof_{}", n)].get();
    m_hists.p1SP_res[n_idx] = m_profiles[std::format("hSP_res_prof_{}", n)].get();
    m_hists.p1SP_evt_res[n_idx] = m_profiles[std::format("hSP_evt_res_prof_{}", n)].get();

    m_hists.S_x_raw_avg[n_idx] = m_profiles[std::format("h_sEPD_Q_S_x_{}_raw_avg", n)].get();
    m_hists.S_y_raw_avg[n_idx] = m_profiles[std::format("h_sEPD_Q_S_y_{}_raw_avg", n)].get();
    m_hists.N_x_raw_avg[n_idx] = m_profiles[std::format("h_sEPD_Q_N_x_{}_raw_avg", n)].get();
    m_hists.N_y_raw_avg[n_idx] = m_profiles[std::format("h_sEPD_Q_N_y_{}_raw_avg", n)].get();

    m_hists.S_x_corr_avg[n_idx] = m_profiles[std::format("h_sEPD_Q_S_x_{}_corr_avg", n)].get();
    m_hists.S_y_corr_avg[n_idx] = m_profiles[std::format("h_sEPD_Q_S_y_{}_corr_avg", n)].get();
    m_hists.N_x_corr_avg[n_idx] = m_profiles[std::format("h_sEPD_Q_N_x_{}_corr_avg", n)].get();
    m_hists.N_y_corr_avg[n_idx] = m_profiles[std::format("h_sEPD_Q_N_y_{}_corr_avg", n)].get();

    m_hists.S_xx_raw_avg[n_idx] = m_profiles[std::format("h_sEPD_Q_S_xx_{}_raw_avg", n)].get();
    m_hists.S_yy_raw_avg[n_idx] = m_profiles[std::format("h_sEPD_Q_S_yy_{}_raw_avg", n)].get();
    m_hists.S_xy_raw_avg[n_idx] = m_profiles[std::format("h_sEPD_Q_S_xy_{}_raw_avg", n)].get();

    m_hists.N_xx_raw_avg[n_idx] = m_profiles[std::format("h_sEPD_Q_N_xx_{}_raw_avg", n)].get();
    m_hists.N_yy_raw_avg[n_idx] = m_profiles[std::format("h_sEPD_Q_N_yy_{}_raw_avg", n)].get();
    m_hists.N_xy_raw_avg[n_idx] = m_profiles[std::format("h_sEPD_Q_N_xy_{}_raw_avg", n)].get();

    m_hists.S_xx_corr_avg[n_idx] = m_profiles[std::format("h_sEPD_Q_S_xx_{}_corr_avg", n)].get();
    m_hists.S_yy_corr_avg[n_idx] = m_profiles[std::format("h_sEPD_Q_S_yy_{}_corr_avg", n)].get();
    m_hists.S_xy_corr_avg[n_idx] = m_profiles[std::format("h_sEPD_Q_S_xy_{}_corr_avg", n)].get();

    m_hists.N_xx_corr_avg[n_idx] = m_profiles[std::format("h_sEPD_Q_N_xx_{}_corr_avg", n)].get();
    m_hists.N_yy_corr_avg[n_idx] = m_profiles[std::format("h_sEPD_Q_N_yy_{}_corr_avg", n)].get();
    m_hists.N_xy_corr_avg[n_idx] = m_profiles[std::format("h_sEPD_Q_N_xy_{}_corr_avg", n)].get();

    for (size_t idx_pt = 0; idx_pt < m_jet_pt_min_vec.size(); ++idx_pt)
    {
      int pt = m_jet_pt_min_vec[idx_pt];
      m_hists.p1SP_re[n_idx][idx_pt] = m_profiles[std::format("hSP_re_prof_{}_{}", n, pt)].get();
      m_hists.p1SP_rev2[n_idx][idx_pt] = m_profiles[std::format("hSP_re_profv2_{}_{}", n, pt)].get();
      if (m_do_secondary_processing)
      {
        m_hists.p1SP_rev3[n_idx][idx_pt] = m_profiles[std::format("hSP_re_profv3_{}_{}", n, pt)].get();
      }
      m_hists.p1SP_re_anti[n_idx][idx_pt] = m_profiles[std::format("hSP_re_anti_prof_{}_{}", n, pt)].get();

      // Event Plane Method
      m_hists.p1EP_re[n_idx][idx_pt] = m_profiles[std::format("hEP_re_prof_{}_{}", n, pt)].get();
    }
  }
}

void JetAnalysis::compute_SP_resolution(int sample)
{
  double cent = m_event_data.event_centrality;
  m_hists.h2Event->Fill(cent, sample);

  size_t south_idx = static_cast<size_t>(Subdetector::S);
  size_t north_idx = static_cast<size_t>(Subdetector::N);

  // Compute Scalar Product for each harmonic
  for (size_t n_idx = 0; n_idx < m_harmonics.size(); ++n_idx)
  {
    QVec sEPD_Q_S = m_event_data.q_vectors[n_idx][south_idx];
    QVec sEPD_Q_N = m_event_data.q_vectors[n_idx][north_idx];

    double SP_res = sEPD_Q_S.x * sEPD_Q_N.x + sEPD_Q_S.y * sEPD_Q_N.y;
    double norm_S = std::sqrt(sEPD_Q_S.x * sEPD_Q_S.x + sEPD_Q_S.y * sEPD_Q_S.y);
    double norm_N = std::sqrt(sEPD_Q_N.x * sEPD_Q_N.x + sEPD_Q_N.y * sEPD_Q_N.y);
    double SP_evt_res = SP_res / (norm_S * norm_N);

    m_hists.h3SP_res[n_idx]->Fill(cent, sample, SP_res);
    m_hists.p2SP_res[n_idx]->Fill(cent, sample, SP_res);
    m_hists.p1SP_res[n_idx]->Fill(cent, SP_res);
    m_hists.p1SP_evt_res[n_idx]->Fill(cent, SP_evt_res);
  }
}

void JetAnalysis::compute_SP(int sample)
{
  std::vector<JetInfo> jet_info = process_jets();

  size_t nJets = jet_info.size();
  double cent = m_event_data.event_centrality;

  m_hists.h2Jet->Fill(cent, sample, static_cast<int>(nJets));

  // Loop over all jets
  for (size_t idx = 0; idx < nJets; ++idx)
  {
    double phi = jet_info[idx].phi;
    double eta = jet_info[idx].eta;
    double pt = jet_info[idx].pt;
    double energy = jet_info[idx].energy;

    size_t arm = (eta < 0) ? static_cast<size_t>(Subdetector::N) : static_cast<size_t>(Subdetector::S);
    size_t arm_anti = (eta > 0) ? static_cast<size_t>(Subdetector::N) : static_cast<size_t>(Subdetector::S);

    // Compute Scalar Product for each harmonic
    for (size_t n_idx = 0; n_idx < m_harmonics.size(); ++n_idx)
    {
      int n = m_harmonics[n_idx];
      QVec jet_Q = {std::cos(n * phi), std::sin(n * phi)};
      QVec sEPD_Q = m_event_data.q_vectors[n_idx][arm];
      QVec sEPD_Q_anti = m_event_data.q_vectors[n_idx][arm_anti];

      double SP_re = jet_Q.x * sEPD_Q.x + jet_Q.y * sEPD_Q.y;
      double SP_re_anti = jet_Q.x * sEPD_Q_anti.x + jet_Q.y * sEPD_Q_anti.y;
      double SP_im = jet_Q.y * sEPD_Q.x - jet_Q.x * sEPD_Q.y;

      // Event Plane Method
      double sEPD_Q_norm = std::sqrt(sEPD_Q.x * sEPD_Q.x + sEPD_Q.y * sEPD_Q.y);
      double EP_re = SP_re / sEPD_Q_norm;

      m_hists.h3SP_re[n_idx]->Fill(cent, sample, SP_re);
      m_hists.h3SP_im[n_idx]->Fill(cent, sample, SP_im);

      m_hists.p2SP_re[n_idx]->Fill(cent, sample, SP_re);
      m_hists.p2SP_im[n_idx]->Fill(cent, sample, SP_im);

      // Loop over each jet pT min
      for (size_t idx_pt = 0; idx_pt < m_jet_pt_min_vec.size(); ++idx_pt)
      {
        int pt_min = m_jet_pt_min_vec[idx_pt];

        // Ensure the jet has minimum pT before proceeding
        if (pt < pt_min)
        {
          break;
        }

        m_hists.p1SP_re[n_idx][idx_pt]->Fill(cent, SP_re);
        m_hists.p1SP_re_anti[n_idx][idx_pt]->Fill(cent, SP_re_anti);

        // Event Plane Method
        m_hists.p1EP_re[n_idx][idx_pt]->Fill(cent, EP_re);

        // Ensure Positive Jet Energy
        if (energy > 0)
        {
          m_hists.p1SP_rev2[n_idx][idx_pt]->Fill(cent, SP_re);

          // Calo V2 Cut
          if (m_event_data.has_good_calo_v2)
          {
            m_hists.p1SP_rev3[n_idx][idx_pt]->Fill(cent, SP_re);
          }
        }
      }
    }
  }
}

std::vector<JetAnalysis::JetInfo> JetAnalysis::process_jets() const
{
  size_t nJets = m_event_data.jet_phi->size();

  std::vector<JetInfo> jet_info;
  jet_info.reserve(nJets);

  double cent = m_event_data.event_centrality;
  float calo_v2 = m_event_data.calo_v2;
  float sum_E = m_event_data.UE_sum_E;

  double total_EMCal = m_event_data.event_EMCal_Energy;
  double total_IHCal = m_event_data.event_IHCal_Energy;
  double total_OHCal = m_event_data.event_OHCal_Energy;
  double total_energy = total_EMCal + total_IHCal + total_OHCal;

  // Loop over all jets
  for (size_t idx = 0; idx < nJets; ++idx)
  {
    double energy = m_event_data.jet_energy->at(idx);
    double pt = m_event_data.jet_pt->at(idx);
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

    int bin_phi = m_hists.h2Dummy->GetXaxis()->FindBin(phi);
    int bin_eta = m_hists.h2Dummy->GetYaxis()->FindBin(eta);
    int dead_status = static_cast<int>(m_hists.h2EMCalBadTowersDeadv2->GetBinContent(bin_phi, bin_eta));

    m_hists.h3JetPhiEtaPt->Fill(phi, eta, pt);

    if (dead_status == 0)
    {
      m_hists.h3JetPhiEtaPtv2->Fill(phi, eta, pt);
      m_hists.h3JetPtEnergyCaloE->Fill(pt, energy, total_energy);

      jet_info.emplace_back(energy, pt, phi, eta);

      if(m_do_secondary_processing)
      {
        m_hists.h3JetPtCentralityCaloV2->Fill(pt, cent, calo_v2);
        m_hists.h3JetEnergyCentralityCaloV2->Fill(energy, cent, calo_v2);
        m_hists.h3JetPtEnergySumE->Fill(pt, energy, sum_E);
      }

      if (energy > 0)
      {
        m_hists.h3JetPhiEtaPtv3->Fill(phi, eta, pt);

        if (m_event_data.has_good_calo_v2)
        {
          m_hists.h3JetPhiEtaPtv4->Fill(phi, eta, pt);
        }
      }
      else
      {
        std::cout << std::format("Negative Energy Jet! Event: {}, Calo V2: {:.6f}, Jet Energy: {:.2f}, Jet Pt: {:.2f}\n",
                                 m_event_data.event_id, calo_v2, energy, pt);
      }
    }
  }

  return jet_info;
}

void JetAnalysis::process_QVecs()
{
  double cent = m_event_data.event_centrality;

  size_t south_idx = static_cast<size_t>(Subdetector::S);
  size_t north_idx = static_cast<size_t>(Subdetector::N);
  size_t ns_idx = static_cast<size_t>(Subdetector::NS);

  m_event_data.q_vectors_raw[0][0] = {m_event_data.Q_S_x_raw, m_event_data.Q_S_y_raw};
  m_event_data.q_vectors_raw[0][1] = {m_event_data.Q_N_x_raw, m_event_data.Q_N_y_raw};

  m_event_data.q_vectors_recentered[0][0] = {m_event_data.Q_S_x_recentered, m_event_data.Q_S_y_recentered};
  m_event_data.q_vectors_recentered[0][1] = {m_event_data.Q_N_x_recentered, m_event_data.Q_N_y_recentered};

  m_event_data.q_vectors[0][0] = {m_event_data.Q_S_x, m_event_data.Q_S_y};
  m_event_data.q_vectors[0][1] = {m_event_data.Q_N_x, m_event_data.Q_N_y};
  m_event_data.q_vectors[0][2] = {m_event_data.Q_NS_x, m_event_data.Q_NS_y};

  for (size_t n_idx = 0; n_idx < m_harmonics.size(); ++n_idx)
  {
    // Grab Q Vectors
    QVec q_S_raw = m_event_data.q_vectors_raw[n_idx][south_idx];
    QVec q_N_raw = m_event_data.q_vectors_raw[n_idx][north_idx];

    QVec q_S_recentered = m_event_data.q_vectors_recentered[n_idx][south_idx];
    QVec q_N_recentered = m_event_data.q_vectors_recentered[n_idx][north_idx];

    QVec q_S = m_event_data.q_vectors[n_idx][south_idx];
    QVec q_N = m_event_data.q_vectors[n_idx][north_idx];

    QVec q_NS = m_event_data.q_vectors[n_idx][ns_idx];

    // Compute Psi
    double psi_S_raw = std::atan2(q_S_raw.y, q_S_raw.x);
    double psi_N_raw = std::atan2(q_N_raw.y, q_N_raw.x);

    double psi_S_recentered = std::atan2(q_S_recentered.y, q_S_recentered.x);
    double psi_N_recentered = std::atan2(q_N_recentered.y, q_N_recentered.x);

    double psi_S = std::atan2(q_S.y, q_S.x);
    double psi_N = std::atan2(q_N.y, q_N.x);

    double psi_NS = std::atan2(q_NS.y, q_NS.x);

    m_hists.hPsi_S_raw[n_idx]->Fill(psi_S_raw, cent);
    m_hists.hPsi_N_raw[n_idx]->Fill(psi_N_raw, cent);

    m_hists.hPsi_S_recentered[n_idx]->Fill(psi_S_recentered, cent);
    m_hists.hPsi_N_recentered[n_idx]->Fill(psi_N_recentered, cent);

    m_hists.hPsi_S[n_idx]->Fill(psi_S, cent);
    m_hists.hPsi_N[n_idx]->Fill(psi_N, cent);

    m_hists.hPsi_NS[n_idx]->Fill(psi_NS, cent);

    m_hists.S_x_raw_avg[n_idx]->Fill(cent, q_S_raw.x);
    m_hists.S_y_raw_avg[n_idx]->Fill(cent, q_S_raw.y);
    m_hists.N_x_raw_avg[n_idx]->Fill(cent, q_N_raw.x);
    m_hists.N_y_raw_avg[n_idx]->Fill(cent, q_N_raw.y);

    m_hists.S_x_corr_avg[n_idx]->Fill(cent, q_S.x);
    m_hists.S_y_corr_avg[n_idx]->Fill(cent, q_S.y);
    m_hists.N_x_corr_avg[n_idx]->Fill(cent, q_N.x);
    m_hists.N_y_corr_avg[n_idx]->Fill(cent, q_N.y);

    m_hists.S_xx_raw_avg[n_idx]->Fill(cent, q_S_raw.x * q_S_raw.x);
    m_hists.S_yy_raw_avg[n_idx]->Fill(cent, q_S_raw.y * q_S_raw.y);
    m_hists.S_xy_raw_avg[n_idx]->Fill(cent, q_S_raw.x * q_S_raw.y);

    m_hists.N_xx_raw_avg[n_idx]->Fill(cent, q_N_raw.x * q_N_raw.x);
    m_hists.N_yy_raw_avg[n_idx]->Fill(cent, q_N_raw.y * q_N_raw.y);
    m_hists.N_xy_raw_avg[n_idx]->Fill(cent, q_N_raw.x * q_N_raw.y);

    m_hists.S_xx_corr_avg[n_idx]->Fill(cent, q_S.x * q_S.x);
    m_hists.S_yy_corr_avg[n_idx]->Fill(cent, q_S.y * q_S.y);
    m_hists.S_xy_corr_avg[n_idx]->Fill(cent, q_S.x * q_S.y);

    m_hists.N_xx_corr_avg[n_idx]->Fill(cent, q_N.x * q_N.x);
    m_hists.N_yy_corr_avg[n_idx]->Fill(cent, q_N.y * q_N.y);
    m_hists.N_xy_corr_avg[n_idx]->Fill(cent, q_N.x * q_N.y);
  }
}

bool JetAnalysis::check_CaloMBD() const
{
  double total_EMCal = m_event_data.event_EMCal_Energy;
  double total_IHCal = m_event_data.event_IHCal_Energy;
  double total_OHCal = m_event_data.event_OHCal_Energy;
  double total_energy = total_EMCal + total_IHCal + total_OHCal;

  double cent = m_event_data.event_centrality;

  int bin = m_hists.hCaloECentrality_min->FindBin(cent);

  double CaloE_min = m_hists.hCaloECentrality_min->GetBinContent(bin);
  double CaloE_max = m_hists.hCaloECentrality_max->GetBinContent(bin);

  return total_energy > CaloE_min && total_energy < CaloE_max;
}

void JetAnalysis::process_calo()
{
  double cent = m_event_data.event_centrality;

  double mbd_south = m_event_data.event_MBD_Charge_South;
  double mbd_north = m_event_data.event_MBD_Charge_North;
  
  double sepd_south = m_event_data.event_sEPD_Charge_South;
  double sepd_north = m_event_data.event_sEPD_Charge_North;

  double total_MBD = mbd_south + mbd_north;
  double total_sEPD = sepd_south + sepd_north;

  double total_EMCal = m_event_data.event_EMCal_Energy;
  double total_IHCal = m_event_data.event_IHCal_Energy;
  double total_OHCal = m_event_data.event_OHCal_Energy;
  double total_energy = total_EMCal + total_IHCal + total_OHCal;
  double tower_median_energy = m_event_data.event_tower_median_Energy * 1e3; // GeV -> MeV
  double tower_EMCal_median_energy = m_event_data.event_EMCal_tower_median_Energy * 1e3; // GeV -> MeV

  int seeds = m_event_data.nHIRecoSeedsSub;
  int seedsIt1 = m_event_data.nHIRecoSeedsSubIt1;

  m_hists.h2MBD->Fill(mbd_south, mbd_north);
  m_hists.h2sEPD->Fill(sepd_south, sepd_north);

  m_hists.h3EMCal_MBD_sEPD->Fill(total_EMCal, total_MBD, total_sEPD);
  m_hists.h3IHCal_MBD_sEPD->Fill(total_IHCal, total_MBD, total_sEPD);
  m_hists.h3OHCal_MBD_sEPD->Fill(total_OHCal, total_MBD, total_sEPD);
  m_hists.h3EMCal_IHCal_OHCal->Fill(total_EMCal, total_IHCal, total_OHCal);

  m_hists.h2CaloECentrality->Fill(total_energy, cent);

  m_hists.h2SeedsIt1CaloE->Fill(seedsIt1, total_energy);
  m_hists.h2SeedsCaloE->Fill(seeds, total_energy);

  m_hists.h2SeedsCentrality->Fill(seeds, cent);
  m_hists.h2SeedsIt1Centrality->Fill(seedsIt1, cent);
  m_hists.h2SeedsIt1->Fill(seedsIt1, seeds);

  if (m_do_secondary_processing)
  {
    float calo_v2 = m_event_data.calo_v2;
    float calo_v2_it1 = m_event_data.calo_v2_it1;
    float UE_sum_E = m_event_data.UE_sum_E;

    m_hists.h3SumECaloV2Centrality->Fill(UE_sum_E, calo_v2, cent);
    m_hists.h3CaloESumETowMedE->Fill(total_energy, UE_sum_E, tower_median_energy);
    m_hists.h2TowMedECaloV2->Fill(tower_median_energy, calo_v2);

    m_hists.h2CaloETowMedE_EMCal->Fill(tower_EMCal_median_energy, total_energy);
    m_hists.h2SumETowMedE_EMCal->Fill(tower_EMCal_median_energy, UE_sum_E);
    m_hists.h2TowMedE_EMCal_CaloV2->Fill(tower_EMCal_median_energy, calo_v2);

    m_hists.h2SumECentrality->Fill(UE_sum_E, cent);

    m_hists.h2SeedsSumE->Fill(seeds, UE_sum_E);

    m_hists.h2SeedsIt1SumE->Fill(seedsIt1, UE_sum_E);

    m_hists.h2CaloV2It1->Fill(calo_v2_it1, calo_v2);

    m_hists.h2SeedsIt1CaloV2->Fill(seedsIt1, calo_v2_it1);
    m_hists.h2SeedsCaloV2->Fill(seeds, calo_v2);

    m_event_data.has_good_calo_v2 = std::abs(calo_v2) < m_calo_v2_max && m_do_secondary_processing;

    if (std::abs(calo_v2) > 0.5F && cent < 50)
    {
      std::cout << std::format("High Calo V2! Event: {}, Centrality: {}, Calo V2: {:.6f}\n", m_event_data.event_id, cent, calo_v2);
    }
  }
}

void JetAnalysis::process_events()
{
  std::cout << "Processing... process_events" << std::endl;
  long long n_entries = m_chain->GetEntries();
  if (m_events_to_process)
  {
    n_entries = std::min(m_events_to_process, n_entries);
  }

  // Generate Sample Offset
  std::random_device rd;
  std::mt19937 generator(rd());
  std::uniform_int_distribution<> distribution(0, m_bins_sample - 1);
  int sample_offset = distribution(generator);
  std::cout << std::format("Sample Offset: {}\n", sample_offset);

  std::map<std::string, int> ctr;

  // Event Loop
  for (long long event = 0; event < n_entries; ++event)
  {
    // Load Event Data from TChain
    m_chain->GetEntry(event);

    if (event % 5000 == 0)
    {
      std::cout << std::format("Processing {}/{}: {:.2f} %", event, n_entries, static_cast<double>(event) * 100. / static_cast<double>(n_entries)) << std::endl;
    }

    double cent = m_event_data.event_centrality;
    int cent_bin = m_hists.hCentrality->FindBin(cent);

    // Skip events in underflow or overflow bins.
    if (cent_bin <= 0 || cent_bin > static_cast<int>(m_bins_cent))
    {
      std::cout << std::format("Warning: Weird Centrality: {}, Skipping Event\n", cent);
      ++ctr["events_skipped_cent"];
      continue;
    }

    // Ensure good correlation between Calo and MBD
    bool isGood = check_CaloMBD();
    if (!isGood)
    {
      ++ctr["events_skipped_calo_mbd"];
      continue;
    }

    // Q Vectors QA
    process_QVecs();

    int events_cent = static_cast<int>(m_hists.hCentrality->GetBinContent(cent_bin));
    int sample = (events_cent + sample_offset) % m_bins_sample;
    m_hists.hCentrality->Fill(cent);

    // Calo QA
    process_calo();

    // Scalar Product Method
    compute_SP(sample);

    // Scalar Product Resolution
    compute_SP_resolution(sample);
  }

  int jets = static_cast<int>(m_hists.h3JetPhiEtaPt->GetEntries());
  int jets_good_regions = static_cast<int>(m_hists.h3JetPhiEtaPtv2->GetEntries());
  int jets_positive_energy = static_cast<int>(m_hists.h3JetPhiEtaPtv3->GetEntries());
  int jets_good_caloV2 = (m_do_secondary_processing) ? static_cast<int>(m_hists.h3JetPhiEtaPtv4->GetEntries()) : 0;

  std::cout << std::format("Event Skipped: CaloMBD Mismatch: {}, Cent (Out of Bounds): {}\n", ctr["events_skipped_calo_mbd"],
                                                                                              ctr["events_skipped_cent"]);

  std::cout << std::format("Jets: {}, Post Masking: {}, {:.2f}, Positive Energy: {}, {:.2f}, Good Calo V2: {}, {:.2f}%\n", jets, jets_good_regions, jets_good_regions * 100. / jets
                                                                                                                               , jets_positive_energy, jets_positive_energy * 100. / jets
                                                                                                                               , jets_good_caloV2, jets_good_caloV2 * 100. / jets);

  std::cout << "Finished... process_events" << std::endl;
}

void JetAnalysis::save_results() const
{
  std::filesystem::create_directories(m_output_dir);

  std::filesystem::path input_path(m_input_file);
  std::string output_stem = input_path.stem().string();
  std::string output_filename = std::format("{}/Jet-Ana_{}.root", m_output_dir, output_stem);

  auto output_file = std::make_unique<TFile>(output_filename.c_str(), "RECREATE");

  for (const auto& [name, hist] : m_hists1D)
  {
    std::cout << std::format("Saving: {}, Estimated Size: {} Bytes\n", name, hist->Sizeof());
    hist->Write();
  }
  for (const auto& [name, hist] : m_hists2D)
  {
    std::cout << std::format("Saving: {}, Estimated Size: {} Bytes\n", name, hist->Sizeof());
    hist->Write();
  }
  for (const auto& [name, hist] : m_hists3D)
  {
    std::cout << std::format("Saving: {}, Estimated Size: {} Bytes\n", name, hist->Sizeof());
    hist->Write();
  }
  for (const auto& [name, hist] : m_profiles)
  {
    std::cout << std::format("Saving: {}, Estimated Size: {} Bytes\n", name, hist->Sizeof());
    hist->Write();
  }
  for (const auto& [name, hist] : m_profiles2D)
  {
    std::cout << std::format("Saving: {}, Estimated Size: {} Bytes\n", name, hist->Sizeof());
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
  project_and_write("h3JetPhiEtaPtv3", "z");
  project_and_write("h3JetPhiEtaPtv3", "yx");

  project_and_write("h3JetPtEnergyCaloE", "yx");
  project_and_write("h3JetPtEnergyCaloE", "xz");
  project_and_write("h3JetPtEnergyCaloE", "yz");

  if (m_do_secondary_processing)
  {
    project_and_write("h3JetPhiEtaPtv4", "z");
    project_and_write("h3JetPhiEtaPtv4", "yx");

    project_and_write("h3JetPtCentralityCaloV2", "xy");
    project_and_write("h3JetPtCentralityCaloV2", "xz");

    project_and_write("h3JetEnergyCentralityCaloV2", "xy");
    project_and_write("h3JetEnergyCentralityCaloV2", "xz");

    project_and_write("h3JetPtEnergySumE", "xz");
    project_and_write("h3JetPtEnergySumE", "yz");

    project_and_write("h3CaloESumETowMedE", "xz");
    project_and_write("h3CaloESumETowMedE", "yz");
    project_and_write("h3CaloESumETowMedE", "yx");
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
  TH1::AddDirectory(false);

  if (argc < 3 || argc > 7)
  {
    std::cout << "Usage: " << argv[0] << " input_file <runnumber> [events] [jet_pt_min] [jet_eta_max] [output_directory]" << std::endl;
    return 1;
  }

  const std::string input_file = argv[1];
  unsigned int runnumber = static_cast<unsigned int>(std::atoi(argv[2]));
  long long events = (argc >= 4) ? std::atoll(argv[3]) : 0;
  double jet_pt_min = (argc >= 5) ? std::stod(argv[4]) : 7;
  double jet_eta_max = (argc >= 6) ? std::stod(argv[5]) : 0.9;
  std::string output_dir = (argc >= 7) ? argv[6] : ".";

  std::cout << std::format("{:#<20}\n", "");
  std::cout << std::format("Run Params\n");
  std::cout << std::format("Input: {}\n", input_file);
  std::cout << std::format("Run Number: {}\n", runnumber);
  std::cout << std::format("Events: {}\n", events);
  std::cout << std::format("Jet pT min: {} [GeV]\n", jet_pt_min);
  std::cout << std::format("Jet eta max: {}\n", jet_eta_max);
  std::cout << std::format("{:#<20}\n", "");

  try
  {
    JetAnalysis analysis(input_file, runnumber, events, output_dir);
    analysis.set_jet_pt_min(jet_pt_min);
    analysis.set_jet_eta_max(jet_eta_max);
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
