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
class JetAnalysisv3
{
 public:
  // The constructor takes the configuration
  JetAnalysisv3(std::string input_file, long long events,
                std::string output_dir)
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

  void set_jet_pt_min(double jet_pt_min) { m_jet_pt_min = jet_pt_min; }

  void set_verbosity(int verbosity) { m_verbosity = verbosity; }

 private:
  static constexpr size_t m_bins_cent = 60;
  static constexpr double m_cent_low = -0.5;
  static constexpr double m_cent_high = 59.5;

  float m_calo_v2_max{0.48F};

  struct AnalysisHists
  {
    TH1 *hEvent{nullptr};
    TH1 *hCentrality{nullptr};
    TH1 *hCentralityCaloFail{nullptr};

    TH2 *h2CaloECentrality_default{nullptr};
    TH2 *h2CaloECentrality{nullptr};

    TH1 *hCaloECentrality_min{nullptr};
    TH1 *hCaloECentrality_max{nullptr};

    TH1 *hCaloV2Fail_iter{nullptr};
    TH1 *hCaloV2Fail_mult{nullptr};

    struct JetHistSet
    {
      TH1 *hJetPt{nullptr};
      TH1 *hJetPt_raw{nullptr};

      // Positive Energy
      TH1 *hJetPtv2{nullptr};
      TH1 *hJetPtv2_raw{nullptr};

      // Positive Energy + |v2| < 0.48
      TH1 *hJetPtv3{nullptr};
      TH1 *hJetPtv3_raw{nullptr};

      TH2 *h2JetPtv2{nullptr};
    };

    JetHistSet iter_r02;
    JetHistSet mult_r02;

    JetHistSet iter_r03;
    JetHistSet mult_r03;

    TH2 *h2CaloV2_mult_iter{nullptr};
    TH2 *h2CaloV2_mult_Centrality{nullptr};
    TH2 *h2CaloV2_iter_Centrality{nullptr};

    TH2 *h2Seeds_iter_mult{nullptr};
    TH2 *h2Seeds_iter{nullptr};
    TH2 *h2Seeds_mult{nullptr};

    TH2 *h2Psi2_S_raw{nullptr};
    TH2 *h2Psi2_N_raw{nullptr};
    TH2 *h2Psi2_NS_raw{nullptr};

    TH2 *h2Psi2_S{nullptr};
    TH2 *h2Psi2_N{nullptr};
    TH2 *h2Psi2_NS{nullptr};
  };

  AnalysisHists m_hists;

  struct JetData
  {
    std::vector<double> *pt{nullptr};        // NOLINT(misc-non-private-member-variables-in-classes)
    std::vector<double> *pt_calib{nullptr};  // NOLINT(misc-non-private-member-variables-in-classes)
    std::vector<double> *e{nullptr};         // NOLINT(misc-non-private-member-variables-in-classes)
    std::vector<double> *phi{nullptr};       // NOLINT(misc-non-private-member-variables-in-classes)
    std::vector<double> *eta{nullptr};       // NOLINT(misc-non-private-member-variables-in-classes)
    double max_pt{0};                        // NOLINT(misc-non-private-member-variables-in-classes)
  };

  struct EventData
  {
    friend class JetAnalysisv3;

   private:
    int event{0};
    [[maybe_unused]] double zvtx{0.0};
    double centrality{0.0};
    double emcal_energy{0};
    double ihcal_energy{0};
    double ohcal_energy{0};

    double psi2_raw_S{0};
    double psi2_raw_N{0};
    double psi2_raw_NS{0};

    double psi2_S{0};
    double psi2_N{0};
    double psi2_NS{0};

    // UE
    // key: iter -> UE Iterative Method
    //      mult -> UE Multiplicity Method (New)
    float calo_v2_iter{0};
    float calo_v2_mult{0};

    int seeds_iter{0};
    int seeds_mult{0};

    bool is_flow_failure_iter{false};
    bool is_flow_failure_mult{false};

    // jets
    JetData iter_r02;
    JetData iter_r03;
    JetData mult_r02;
    JetData mult_r03;

    // Event Checks
    bool pass_calo_cent{false};
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
  double m_jet_eta_max_r02{0.9};
  double m_jet_eta_max_r03{0.8};

  std::map<std::string, int> m_ctr;

  enum class EventType : std::uint8_t
  {
    ZVTX10_MB,
    CALOCENT,
    FLOW_FAILURE_ITER,
    FLOW_FAILURE_MULT
  };

  std::vector<std::string> m_eventType{"|z| < 10 cm and MB", "Good Calo-Cent", "Flow Fail Iter", "Flow Fail Mult"};

  // Hists
  std::map<std::string, std::unique_ptr<TH1>> m_hists1D;
  std::map<std::string, std::unique_ptr<TH2>> m_hists2D;
  std::map<std::string, std::unique_ptr<TProfile>> m_profiles;
  std::map<std::string, std::unique_ptr<TProfile2D>> m_profiles2D;

  // --- Private Helper Methods ---
  void setup_chain();

  void load_calo_centrality_cuts();

  void init_hists();

  void process_jets();
  bool check_CaloMBD() const;
  void process_event_check();
  void process_events();
  void print_event_info(long long event_idx) const;

  void save_results() const;
};

// ====================================================================
// Implementation of the Class Methods
// ====================================================================
void JetAnalysisv3::setup_chain()
{
  std::cout << "Processing... setup_chain" << std::endl;

  m_chain = myUtils::setupTChain(m_input_file, "T");

  if (m_chain == nullptr)
  {
    throw std::runtime_error(
        std::format("Error in TChain Setup from file: {}", m_input_file));
  }

  // Setup branches
  m_chain->SetBranchStatus("*", false);

  // List of Branches of Interest
  // Common branches between data and sim
  std::unordered_set<std::string> branchNames = {"event", "centrality",
                                                 "emcal_energy", "ihcal_energy", "ohcal_energy",
                                                 "psi2_raw_S", "psi2_raw_N", "psi2_raw_NS",
                                                 "psi2_S", "psi2_N", "psi2_NS",
                                                 "calo_v2_iter", "calo_v2_mult",
                                                 "seeds_iter", "seeds_mult",
                                                 "is_flow_failure_iter", "is_flow_failure_mult",
                                                 "max_pt_iter_r02", "max_pt_mult_r02",
                                                 "max_pt_iter_r03", "max_pt_mult_r03",
                                                 "pt_iter_r02", "pt_calib_iter_r02", "e_iter_r02", "phi_iter_r02", "eta_iter_r02",
                                                 "pt_mult_r02", "pt_calib_mult_r02", "e_mult_r02", "phi_mult_r02", "eta_mult_r02",
                                                 "pt_iter_r03", "pt_calib_iter_r03", "e_iter_r03", "phi_iter_r03", "eta_iter_r03",
                                                 "pt_mult_r03", "pt_calib_mult_r03", "e_mult_r03", "phi_mult_r03", "eta_mult_r03"};

  // Check Branch Status
  for (const auto &branchName : branchNames)
  {
    TBranch *branch = m_chain->GetBranch(branchName.c_str());
    if (branch)
    {
      // Branch exists: Enable it
      m_chain->SetBranchStatus(branchName.c_str(), true);
    }
    else
    {
      throw std::runtime_error(std::format(
          "Could not find Branch '{}' in file '{}'", branchName, m_input_file));
    }
  }

  // Set branches to variables
  m_chain->SetBranchAddress("event", &m_event_data.event);
  m_chain->SetBranchAddress("centrality", &m_event_data.centrality);
  m_chain->SetBranchAddress("emcal_energy", &m_event_data.emcal_energy);
  m_chain->SetBranchAddress("ihcal_energy", &m_event_data.ihcal_energy);
  m_chain->SetBranchAddress("ohcal_energy", &m_event_data.ohcal_energy);

  m_chain->SetBranchAddress("psi2_raw_S", &m_event_data.psi2_raw_S);
  m_chain->SetBranchAddress("psi2_raw_N", &m_event_data.psi2_raw_N);
  m_chain->SetBranchAddress("psi2_raw_NS", &m_event_data.psi2_raw_NS);

  m_chain->SetBranchAddress("psi2_S", &m_event_data.psi2_S);
  m_chain->SetBranchAddress("psi2_N", &m_event_data.psi2_N);
  m_chain->SetBranchAddress("psi2_NS", &m_event_data.psi2_NS);

  m_chain->SetBranchAddress("seeds_iter", &m_event_data.seeds_iter);
  m_chain->SetBranchAddress("seeds_mult", &m_event_data.seeds_mult);

  m_chain->SetBranchAddress("calo_v2_iter", &m_event_data.calo_v2_iter);
  m_chain->SetBranchAddress("calo_v2_mult", &m_event_data.calo_v2_mult);

  m_chain->SetBranchAddress("is_flow_failure_iter", &m_event_data.is_flow_failure_iter);
  m_chain->SetBranchAddress("is_flow_failure_mult", &m_event_data.is_flow_failure_mult);

  m_chain->SetBranchAddress("max_pt_iter_r02", &m_event_data.iter_r02.max_pt);
  m_chain->SetBranchAddress("max_pt_iter_r03", &m_event_data.iter_r03.max_pt);

  m_chain->SetBranchAddress("max_pt_mult_r02", &m_event_data.mult_r02.max_pt);
  m_chain->SetBranchAddress("max_pt_mult_r03", &m_event_data.mult_r03.max_pt);

  m_chain->SetBranchAddress("pt_iter_r02", &m_event_data.iter_r02.pt);
  m_chain->SetBranchAddress("pt_calib_iter_r02", &m_event_data.iter_r02.pt_calib);
  m_chain->SetBranchAddress("e_iter_r02", &m_event_data.iter_r02.e);
  m_chain->SetBranchAddress("phi_iter_r02", &m_event_data.iter_r02.phi);
  m_chain->SetBranchAddress("eta_iter_r02", &m_event_data.iter_r02.eta);

  m_chain->SetBranchAddress("pt_mult_r02", &m_event_data.mult_r02.pt);
  m_chain->SetBranchAddress("pt_calib_mult_r02", &m_event_data.mult_r02.pt_calib);
  m_chain->SetBranchAddress("e_mult_r02", &m_event_data.mult_r02.e);
  m_chain->SetBranchAddress("phi_mult_r02", &m_event_data.mult_r02.phi);
  m_chain->SetBranchAddress("eta_mult_r02", &m_event_data.mult_r02.eta);

  m_chain->SetBranchAddress("pt_iter_r03", &m_event_data.iter_r03.pt);
  m_chain->SetBranchAddress("pt_calib_iter_r03", &m_event_data.iter_r03.pt_calib);
  m_chain->SetBranchAddress("e_iter_r03", &m_event_data.iter_r03.e);
  m_chain->SetBranchAddress("phi_iter_r03", &m_event_data.iter_r03.phi);
  m_chain->SetBranchAddress("eta_iter_r03", &m_event_data.iter_r03.eta);

  m_chain->SetBranchAddress("pt_mult_r03", &m_event_data.mult_r03.pt);
  m_chain->SetBranchAddress("pt_calib_mult_r03", &m_event_data.mult_r03.pt_calib);
  m_chain->SetBranchAddress("e_mult_r03", &m_event_data.mult_r03.e);
  m_chain->SetBranchAddress("phi_mult_r03", &m_event_data.mult_r03.phi);
  m_chain->SetBranchAddress("eta_mult_r03", &m_event_data.mult_r03.eta);

  std::cout << "Finished... setup_chain" << std::endl;
}

void JetAnalysisv3::load_calo_centrality_cuts()
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

void JetAnalysisv3::init_hists()
{
  int bins_pt = 500;
  double pt_low = 0;
  double pt_high = 500;

  int bins_Calo_E{540};
  double Calo_E_low{-2e2};
  double Calo_E_high{2.5e3};

  int bins_psi = 126;
  double psi_low = -std::numbers::pi;
  double psi_high = std::numbers::pi;

  int bins_seeds_iter = 30;
  double seeds_iter_low = 0;
  double seeds_iter_high = 30;

  int bins_seeds_mult = 3;
  double seeds_mult_low = 0;
  double seeds_mult_high = 3;

  int bins_v2{100};
  double v2_low{-1};
  double v2_high{1};

  unsigned int bins_event = static_cast<unsigned int>(m_eventType.size());

  auto clone_hist = [](auto &&map, std::string_view src, std::string_view dest)
  {
    using PtrType = typename std::decay_t<decltype(map)>::mapped_type; // e.g., std::unique_ptr<TH1>
    using ElementType = typename PtrType::element_type;  // e.g., TH1

    map[std::string(dest)] = PtrType(static_cast<ElementType *>(map[std::string(src)]->Clone(dest.data())));
  };

  m_hists1D["hEvent"] = std::make_unique<TH1F>("hEvent", "Event Type; Type; Events", bins_event, 0, bins_event);

  m_hists1D["hCentralityCaloFail"] = std::make_unique<TH1F>("hCentralityCaloFail", "Centrality Calo Failure; Centrality [%]; Events", m_bins_cent, m_cent_low, m_cent_high);

  m_hists1D["hCentrality"] = std::make_unique<TH1F>("hCentrality", "|z| < 10 cm & MB; Centrality [%]; Events", m_bins_cent, m_cent_low, m_cent_high);

  m_hists1D["hCaloV2Fail_iter"] = std::make_unique<TH1F>("hCaloV2Fail_iter", "; Centrality [%]; Events", m_bins_cent, m_cent_low, m_cent_high);
  clone_hist(m_hists1D, "hCaloV2Fail_iter", "hCaloV2Fail_mult");

  for (const std::string r : {"r02", "r03"})
  {
    for (const std::string ue : {"iter", "mult"})
    {
      std::string base = std::format("hJetPt_{}_{}", r, ue);
      m_hists1D[base] = std::make_unique<TH1F>(base.c_str(), "; p_{T} [GeV]; Jets / 1 GeV", bins_pt, pt_low, pt_high);
      clone_hist(m_hists1D, base, std::format("hJetPt_raw_{}_{}", r, ue));

      for (const std::string v : {"v2", "v3"})
      {
        for (const std::string calib : {"", "_raw"})
        {
          clone_hist(m_hists1D, base, std::format("hJetPt{}{}_{}_{}", v, calib, r, ue));
        }
      }

      std::string h2name = std::format("h2JetPtv2_{}_{}", r, ue);
      m_hists2D[h2name] = std::make_unique<TH2F>(h2name.c_str(), "; Calo v_{2}; Jet p_{T} [GeV]", bins_v2, v2_low, v2_high, bins_pt, pt_low, pt_high);
    }
  }

  m_hists2D["h2CaloECentrality_default"] = std::make_unique<TH2F>("h2CaloECentrality_default", "|z| < 10 cm and MB; Total Calorimeter Energy [GeV]; Centrality [%]", bins_Calo_E, Calo_E_low, Calo_E_high, m_bins_cent, m_cent_low, m_cent_high);
  clone_hist(m_hists2D, "h2CaloECentrality_default", "h2CaloECentrality");

  m_hists2D["h2CaloV2_mult_iter"] = std::make_unique<TH2F>("h2CaloV2_mult_iter", "; Calo v_{2} Iterative; Calo v_{2} Multiplicity", bins_v2, v2_low, v2_high, bins_v2, v2_low, v2_high);
  m_hists2D["h2CaloV2_iter_Centrality"] = std::make_unique<TH2F>("h2CaloV2_iter_Centrality", "; Centrality [%]; Calo v_{2}", m_bins_cent, m_cent_low, m_cent_high, bins_v2, v2_low, v2_high);
  clone_hist(m_hists2D, "h2CaloV2_iter_Centrality", "h2CaloV2_mult_Centrality");

  m_hists2D["h2Seeds_iter_mult"] = std::make_unique<TH2F>("h2Seeds_iter_mult", "Seeds; Multiplicity; Iterative", bins_seeds_mult, seeds_mult_low, seeds_mult_high, bins_seeds_iter, seeds_iter_low, seeds_iter_high);
  m_hists2D["h2Seeds_iter"] = std::make_unique<TH2F>("h2Seeds_iter", "Iterative Seeds; Centrality [%]; Seeds Iterative", m_bins_cent, m_cent_low, m_cent_high, bins_seeds_iter, seeds_iter_low, seeds_iter_high);
  m_hists2D["h2Seeds_mult"] = std::make_unique<TH2F>("h2Seeds_mult", "Multiplicity Seeds; Centrality [%]; Seeds Multiplicity", m_bins_cent, m_cent_low, m_cent_high, bins_seeds_mult, seeds_mult_low, seeds_mult_high);

  m_hists2D["h2Psi2_S_raw"] = std::make_unique<TH2F>("h2Psi2_S_raw", "; 2 #Psi_{2}; Centrality [%]", bins_psi, psi_low, psi_high, m_bins_cent, m_cent_low, m_cent_high);

  clone_hist(m_hists2D, "h2Psi2_S_raw", "h2Psi2_N_raw");
  clone_hist(m_hists2D, "h2Psi2_S_raw", "h2Psi2_NS_raw");

  clone_hist(m_hists2D, "h2Psi2_S_raw", "h2Psi2_S");
  clone_hist(m_hists2D, "h2Psi2_S_raw", "h2Psi2_N");
  clone_hist(m_hists2D, "h2Psi2_S_raw", "h2Psi2_NS");

  for (unsigned int i = 0; i < m_eventType.size(); ++i)
  {
    m_hists1D["hEvent"]->GetXaxis()->SetBinLabel(static_cast<int>(i + 1), m_eventType[i].c_str());
  }

  m_hists.hEvent = m_hists1D["hEvent"].get();
  m_hists.hCentrality = m_hists1D["hCentrality"].get();
  m_hists.hCentralityCaloFail = m_hists1D["hCentralityCaloFail"].get();

  m_hists.h2CaloECentrality_default = m_hists2D["h2CaloECentrality_default"].get();
  m_hists.h2CaloECentrality = m_hists2D["h2CaloECentrality"].get();

  m_hists.hCaloV2Fail_iter = m_hists1D["hCaloV2Fail_iter"].get();
  m_hists.hCaloV2Fail_mult = m_hists1D["hCaloV2Fail_mult"].get();

  auto bind_jet_set = [&](AnalysisHists::JetHistSet &set, std::string_view r, std::string_view ue)
  {
    set.hJetPt = m_hists1D[std::format("hJetPt_{}_{}", r, ue)].get();
    set.hJetPt_raw = m_hists1D[std::format("hJetPt_raw_{}_{}", r, ue)].get();
    set.hJetPtv2 = m_hists1D[std::format("hJetPtv2_{}_{}", r, ue)].get();
    set.hJetPtv2_raw = m_hists1D[std::format("hJetPtv2_raw_{}_{}", r, ue)].get();
    set.hJetPtv3 = m_hists1D[std::format("hJetPtv3_{}_{}", r, ue)].get();
    set.hJetPtv3_raw = m_hists1D[std::format("hJetPtv3_raw_{}_{}", r, ue)].get();
    set.h2JetPtv2 = m_hists2D[std::format("h2JetPtv2_{}_{}", r, ue)].get();
  };

  bind_jet_set(m_hists.iter_r02, "r02", "iter");
  bind_jet_set(m_hists.mult_r02, "r02", "mult");
  bind_jet_set(m_hists.iter_r03, "r03", "iter");
  bind_jet_set(m_hists.mult_r03, "r03", "mult");

  m_hists.h2CaloV2_mult_iter = m_hists2D["h2CaloV2_mult_iter"].get();
  m_hists.h2CaloV2_mult_Centrality = m_hists2D["h2CaloV2_mult_Centrality"].get();
  m_hists.h2CaloV2_iter_Centrality = m_hists2D["h2CaloV2_iter_Centrality"].get();

  m_hists.h2Seeds_iter_mult = m_hists2D["h2Seeds_iter_mult"].get();
  m_hists.h2Seeds_iter = m_hists2D["h2Seeds_iter"].get();
  m_hists.h2Seeds_mult = m_hists2D["h2Seeds_mult"].get();

  m_hists.h2Psi2_S_raw = m_hists2D["h2Psi2_S_raw"].get();
  m_hists.h2Psi2_N_raw = m_hists2D["h2Psi2_N_raw"].get();
  m_hists.h2Psi2_NS_raw = m_hists2D["h2Psi2_NS_raw"].get();

  m_hists.h2Psi2_S = m_hists2D["h2Psi2_S"].get();
  m_hists.h2Psi2_N = m_hists2D["h2Psi2_N"].get();
  m_hists.h2Psi2_NS = m_hists2D["h2Psi2_NS"].get();

  // Enable Sumw2
  auto enable = [](auto &...maps)
  {
    auto loop = [](auto &map)
    {
      for (auto &[key, hist] : map)
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

void JetAnalysisv3::process_jets()
{
  if (!m_event_data.pass_calo_cent)
  {
    return;
  }

  auto fill_jet_hists = [this](const JetData &jet_data, float calo_v2, const AnalysisHists::JetHistSet &h, double eta_max)
  {
    if (!jet_data.pt || !jet_data.pt_calib || !jet_data.e || !jet_data.eta)
    {
      return;
    }
    for (size_t idx = 0; idx < jet_data.pt->size(); ++idx)
    {
      double energy = jet_data.e->at(idx);
      double pt = jet_data.pt_calib->at(idx);
      double pt_raw = jet_data.pt->at(idx);
      double eta = jet_data.eta->at(idx);

      if (pt < m_jet_pt_min || std::abs(eta) >= eta_max)
      {
        continue;
      }

      h.hJetPt->Fill(pt);
      h.hJetPt_raw->Fill(pt_raw);

      if (energy > 0)
      {
        h.hJetPtv2->Fill(pt);
        h.hJetPtv2_raw->Fill(pt_raw);
        h.h2JetPtv2->Fill(calo_v2, pt);

        if (std::abs(calo_v2) < m_calo_v2_max)
        {
          h.hJetPtv3->Fill(pt);
          h.hJetPtv3_raw->Fill(pt_raw);
        }
      }
    }
  };

  // Process r02 branches for iter and mult (eta_max = 0.9)
  if (!m_event_data.is_flow_failure_iter)
  {
    fill_jet_hists(m_event_data.iter_r02, m_event_data.calo_v2_iter, m_hists.iter_r02, m_jet_eta_max_r02);
  }
  if (!m_event_data.is_flow_failure_mult)
  {
    fill_jet_hists(m_event_data.mult_r02, m_event_data.calo_v2_mult, m_hists.mult_r02, m_jet_eta_max_r02);
  }

  // Process r03 branches for iter and mult (eta_max = 0.8)
  if (!m_event_data.is_flow_failure_iter)
  {
    fill_jet_hists(m_event_data.iter_r03, m_event_data.calo_v2_iter, m_hists.iter_r03, m_jet_eta_max_r03);
  }
  if (!m_event_data.is_flow_failure_mult)
  {
    fill_jet_hists(m_event_data.mult_r03, m_event_data.calo_v2_mult, m_hists.mult_r03, m_jet_eta_max_r03);
  }
}

bool JetAnalysisv3::check_CaloMBD() const
{
  double total_EMCal = m_event_data.emcal_energy;
  double total_IHCal = m_event_data.ihcal_energy;
  double total_OHCal = m_event_data.ohcal_energy;
  double total_energy = total_EMCal + total_IHCal + total_OHCal;

  double cent = m_event_data.centrality;

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

void JetAnalysisv3::process_event_check()
{
  auto &ed = m_event_data;

  double cent = ed.centrality;

  ed.pass_calo_cent = check_CaloMBD();

  m_hists.hEvent->Fill(static_cast<std::uint8_t>(EventType::ZVTX10_MB));

  if (ed.pass_calo_cent)
  {
    m_hists.hEvent->Fill(static_cast<std::uint8_t>(EventType::CALOCENT));

    if (ed.is_flow_failure_iter)
    {
      m_hists.hEvent->Fill(static_cast<std::uint8_t>(EventType::FLOW_FAILURE_ITER));
      m_hists.hCaloV2Fail_iter->Fill(cent);
      ++m_ctr["events_flow_failure_iter"];
    }

    if (ed.is_flow_failure_mult)
    {
      m_hists.hEvent->Fill(static_cast<std::uint8_t>(EventType::FLOW_FAILURE_MULT));
      m_hists.hCaloV2Fail_mult->Fill(cent);
      ++m_ctr["events_flow_failure_mult"];
    }

    if (!ed.is_flow_failure_iter && !ed.is_flow_failure_mult)
    {
      m_hists.h2CaloV2_mult_iter->Fill(ed.calo_v2_iter, ed.calo_v2_mult);
      m_hists.h2Seeds_iter_mult->Fill(ed.seeds_mult, ed.seeds_iter);
    }

    if (!ed.is_flow_failure_mult)
    {
      m_hists.h2CaloV2_mult_Centrality->Fill(cent, ed.calo_v2_mult);
      m_hists.h2Seeds_mult->Fill(cent, ed.seeds_mult);
    }

    if (!ed.is_flow_failure_iter)
    {
      m_hists.h2CaloV2_iter_Centrality->Fill(cent, ed.calo_v2_iter);
      m_hists.h2Seeds_iter->Fill(cent, ed.seeds_iter);
    }

    m_hists.h2Psi2_S_raw->Fill(ed.psi2_raw_S, cent);
    m_hists.h2Psi2_N_raw->Fill(ed.psi2_raw_N, cent);
    m_hists.h2Psi2_NS_raw->Fill(ed.psi2_raw_NS, cent);

    m_hists.h2Psi2_S->Fill(ed.psi2_S, cent);
    m_hists.h2Psi2_N->Fill(ed.psi2_N, cent);
    m_hists.h2Psi2_NS->Fill(ed.psi2_NS, cent);
  }
  else
  {
    m_hists.hCentralityCaloFail->Fill(cent);
    ++m_ctr["events_bad_calo_mbd"];
  }
}

void JetAnalysisv3::process_events()
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

    double cent = m_event_data.centrality;
    m_hists.hCentrality->Fill(cent);

    // Event Selections
    process_event_check();

    // Jets
    process_jets();

    if (m_verbosity > 0)
    {
      print_event_info(event);
    }
  }

  std::cout << std::format("{:#<20}\n", "");
  std::cout << "Events Bad" << std::endl;
  for (const auto &[name, events] : m_ctr)
  {
    if (name.starts_with("events_bad") || name.starts_with("events_flow"))
    {
      std::cout << std::format("{}: {}\n", name, events);
    }
  }
  std::cout << std::format("{:#<20}\n", "");

  std::cout << "Finished... process_events" << std::endl;
}

void JetAnalysisv3::print_event_info(long long event_idx) const
{
  std::cout << std::format("\n{:=^70}\n", std::format(" Event {} (Entry ID: {}) ", event_idx, m_event_data.event));
  std::cout << std::format(" Centrality: {:.2f} %\n", m_event_data.centrality);
  std::cout << std::format(" Calo Energy [GeV] - EMCal: {:.2f}, IHCal: {:.2f}, OHCal: {:.2f} (Pass Cut: {})\n",
                           m_event_data.emcal_energy, m_event_data.ihcal_energy, m_event_data.ohcal_energy,
                           m_event_data.pass_calo_cent ? "Yes" : "No");
  std::cout << std::format(" Calo v2 - Iter: {:.4f} (Fail: {}), Mult: {:.4f} (Fail: {})\n",
                           m_event_data.calo_v2_iter, m_event_data.is_flow_failure_iter ? "Yes" : "No",
                           m_event_data.calo_v2_mult, m_event_data.is_flow_failure_mult ? "Yes" : "No");
  std::cout << std::format(" Seeds - Iter: {}, Mult: {}\n", m_event_data.seeds_iter, m_event_data.seeds_mult);
  std::cout << std::format(" Psi2 Raw  - S: {:.4f}, N: {:.4f}, NS: {:.4f}\n", m_event_data.psi2_raw_S, m_event_data.psi2_raw_N, m_event_data.psi2_raw_NS);
  std::cout << std::format(" Psi2 Corr - S: {:.4f}, N: {:.4f}, NS: {:.4f}\n", m_event_data.psi2_S, m_event_data.psi2_N, m_event_data.psi2_NS);

  auto print_jet_collection = [this](std::string_view label, const JetData &jd)
  {
    size_t njets = jd.pt ? jd.pt->size() : 0;
    std::cout << std::format("  [{}] N_jets: {}, max_pt: {:.2f} GeV\n", label, njets, jd.max_pt);

    if (m_verbosity >= 2 && njets > 0)
    {
      std::cout << std::format("    {:>4} | {:>10} | {:>10} | {:>10} | {:>10} | {:>10}\n",
                               "Idx", "pT_raw", "pT_calib", "Energy", "Eta", "Phi");
      std::cout << std::format("    {:-^62}\n", "");
      for (size_t i = 0; i < njets; ++i)
      {
        double pt_raw = jd.pt->at(i);
        double pt_cal = jd.pt_calib ? jd.pt_calib->at(i) : 0.0;
        double e = jd.e ? jd.e->at(i) : 0.0;
        double eta = jd.eta ? jd.eta->at(i) : 0.0;
        double phi = jd.phi ? jd.phi->at(i) : 0.0;
        std::cout << std::format("    {:>4} | {:>10.2f} | {:>10.2f} | {:>10.2f} | {:>10.3f} | {:>10.3f}\n",
                                 i, pt_raw, pt_cal, e, eta, phi);
      }
    }
  };

  std::cout << " -- Jet Collections --\n";
  print_jet_collection("r02_iter", m_event_data.iter_r02);
  print_jet_collection("r02_mult", m_event_data.mult_r02);
  print_jet_collection("r03_iter", m_event_data.iter_r03);
  print_jet_collection("r03_mult", m_event_data.mult_r03);
  std::cout << std::format("{:=^70}\n", "");
}

void JetAnalysisv3::save_results() const
{
  std::filesystem::create_directories(m_output_dir);

  std::filesystem::path input_path(m_input_file);
  std::string output_stem = input_path.stem().string();
  std::string output_filename =
      std::format("{}/Jet-Ana_{}.root", m_output_dir, output_stem);

  auto output_file =
      std::make_unique<TFile>(output_filename.c_str(), "RECREATE");

  auto save_all = [](auto &...collections)
  {
    auto save = [](const auto &container)
    {
      for (const auto &[name, hist] : container)
      {
        if (hist)
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

  std::cout << std::format("Results saved to: {}", output_filename)
            << std::endl;
}

// ====================================================================
// Main function remains clean and simple
// ====================================================================
int main(int argc, const char *const argv[])
{
  gROOT->SetBatch(true);
  TH1::AddDirectory(false);

  if (argc < 2 || argc > 6)
  {
    std::cout << "Usage: " << argv[0] << " input_file [events] [jet_pt_min] [output_directory] [verbosity]" << std::endl;
    return 1;
  }

  int ctr = 1;
  const std::string input_file = argv[ctr++];
  long long events = (argc >= ctr + 1) ? std::atoll(argv[ctr++]) : 0;
  double jet_pt_min = (argc >= ctr + 1) ? std::stod(argv[ctr++]) : 10;
  std::string output_dir = (argc >= ctr + 1) ? argv[ctr++] : ".";
  int verbosity = (argc >= ctr + 1) ? std::atoi(argv[ctr++]) : 0;

  std::cout << std::format("{:#<20}\n", "");
  std::cout << std::format("Run Params\n");
  std::cout << std::format("Input: {}\n", input_file);
  std::cout << std::format("Events: {}\n", events);
  std::cout << std::format("Jet pT min: {} [GeV]\n", jet_pt_min);
  std::cout << std::format("Output Dir: {}\n", output_dir);
  std::cout << std::format("Verbosity: {}\n", verbosity);
  std::cout << std::format("{:#<20}\n", "");

  try
  {
    JetAnalysisv3 analysis(input_file, events, output_dir);
    analysis.set_jet_pt_min(jet_pt_min);
    analysis.set_verbosity(verbosity);
    analysis.run();
  }
  catch (const std::exception &e)
  {
    std::cout << std::format("An exception occurred: {}", e.what())
              << std::endl;
    return 1;
  }

  std::cout << "Analysis complete." << std::endl;
  return 0;
}
