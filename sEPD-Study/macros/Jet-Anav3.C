// -- utils
#include "myUtils.C"

// ====================================================================
// sPHENIX Includes
// ====================================================================
#include <calobase/TowerInfoDefs.h>
// CDB
#include <cdbobjects/CDBTTree.h>
#include <CDBUtils.C>

#include <sepdvalidation/JetUtils.h>

// ====================================================================
// Standard C++ Includes
// ====================================================================
#include <array>
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
  JetAnalysisv3(std::string input_file, long long events, std::string output_dir)
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

  void set_do_unsub(bool b = true) { m_do_unsub = b; }
  void set_do_iter(bool b = true) { m_do_iter = b; }
  void set_do_mult(bool b = true) { m_do_mult = b; }
  void set_do_rcone(bool b = true) { m_do_rcone = b; }

  bool get_do_unsub() const { return m_do_unsub; }
  bool get_do_iter() const { return m_do_iter; }
  bool get_do_mult() const { return m_do_mult; }
  bool get_do_rcone() const { return m_do_rcone; }

  void set_jet_pt_min(double jet_pt_min) { m_jet_pt_min = jet_pt_min; }
  double get_jet_pt_min() const { return m_jet_pt_min; }

  void set_verbosity(int verbosity) { m_verbosity = verbosity; }
  int get_verbosity() const { return m_verbosity; }

 private:
  static constexpr size_t m_bins_cent = 60;
  static constexpr double m_cent_low = -0.5;
  static constexpr double m_cent_high = 59.5;

  static constexpr std::array<double, 7> m_cent_bins = {0.0, 10.0, 20.0, 30.0, 40.0, 50.0, 60.0};
  static constexpr size_t m_num_cent_bins = m_cent_bins.size() - 1;

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

      // Positive Energy + Flow Failure
      TH1 *hJetPtFlowFail{nullptr};
      TH1 *hJetPtFlowFail_raw{nullptr};

      TH2 *h2JetPtv2{nullptr};
      TH2 *h2JetEta{nullptr};
      TH2 *h2JetEtav2{nullptr};
      TH2 *h2JetEtav3{nullptr};

      // Njets vs Leading Jet pT
      TH2 *h2Njets_LeadJetPt{nullptr};
      TH2 *h2Njets_LeadJetPtv2{nullptr};
      std::array<TH2 *, m_num_cent_bins> h2Njets_LeadJetPt_cent{};
      std::array<TH2 *, m_num_cent_bins> h2Njets_LeadJetPtv2_cent{};
    };

    JetHistSet iter_r02;
    JetHistSet mult_r02;

    JetHistSet iter_r03;
    JetHistSet mult_r03;

    JetHistSet unsub_r02;
    JetHistSet unsub_r03;

    struct RConeHistSet
    {
      TH1 *hEta{nullptr};
      TH1 *hEtaNorm{nullptr};
      TH2 *h2EtaZvtx{nullptr};
      TH1 *hPhi{nullptr};
      TH1 *hEnergy{nullptr};
      TH1 *hPt{nullptr};
      TH1 *hPtv2{nullptr};
      TProfile *pEtaPt{nullptr};
    };

    RConeHistSet rcone_r02;
    RConeHistSet rcone_r03;
    RConeHistSet rcone_iter_r02;
    RConeHistSet rcone_iter_r03;

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

  struct RConeData
  {
    double eta{0};
    double phi{0};
    double pt{0};
    double energy{0};
    double pt_sub1{0};
    double energy_sub1{0};
  };

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
    double zvtx{0.0};
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
    JetData unsub_r02;
    JetData unsub_r03;

    RConeData rcone_r02;
    RConeData rcone_r03;

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

  // Flags
  bool m_do_unsub{true};
  bool m_do_iter{true};
  bool m_do_mult{true};
  bool m_do_rcone{true};

  // Jet Cuts
  double m_jet_pt_min{10}; /*GeV*/
  double m_jet_eta_max_r02{0.9};
  double m_jet_eta_max_r03{0.8};

  std::map<std::string, int> m_ctr;

  enum class EventType : std::uint8_t
  {
    ZVTX10_MB,
    CALOCENT,
    FLOW_PASS_ITER,
    FLOW_PASS_MULT
  };

  std::vector<std::string> m_eventType{"|z| < 10 cm and MB", "Good Calo-Cent", "Flow Pass Iter", "Flow Pass Mult"};

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
  void process_rcones();
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
    throw std::runtime_error(std::format("Error in TChain Setup from file: {}", m_input_file));
  }

  // Setup branches
  m_chain->SetBranchStatus("*", false);

  // List of Branches of Interest
  // Common branches between data and sim
  std::unordered_set<std::string> branchNames = {"event", "centrality", "zvtx",
                                                 "emcal_energy", "ihcal_energy", "ohcal_energy",
                                                 "psi2_raw_S", "psi2_raw_N", "psi2_raw_NS",
                                                 "psi2_S", "psi2_N", "psi2_NS"};

  if (m_do_iter)
  {
    branchNames.insert({
        "calo_v2_iter", "seeds_iter", "is_flow_failure_iter",
        "max_pt_iter_r02", "max_pt_iter_r03",
        "pt_iter_r02", "pt_calib_iter_r02", "e_iter_r02", "phi_iter_r02", "eta_iter_r02",
        "pt_iter_r03", "pt_calib_iter_r03", "e_iter_r03", "phi_iter_r03", "eta_iter_r03"
    });
  }

  if (m_do_mult)
  {
    branchNames.insert({
        "calo_v2_mult", "seeds_mult", "is_flow_failure_mult",
        "max_pt_mult_r02", "max_pt_mult_r03",
        "pt_mult_r02", "pt_calib_mult_r02", "e_mult_r02", "phi_mult_r02", "eta_mult_r02",
        "pt_mult_r03", "pt_calib_mult_r03", "e_mult_r03", "phi_mult_r03", "eta_mult_r03"
    });
  }

  if (m_do_unsub)
  {
    branchNames.insert({
        "max_pt_unsub_r02", "max_pt_unsub_r03",
        "pt_unsub_r02", "pt_calib_unsub_r02", "e_unsub_r02", "phi_unsub_r02", "eta_unsub_r02",
        "pt_unsub_r03", "pt_calib_unsub_r03", "e_unsub_r03", "phi_unsub_r03", "eta_unsub_r03"
    });
  }

  if (m_do_rcone)
  {
    branchNames.insert({
        "rcone_r02_eta", "rcone_r02_phi", "rcone_r02_pt", "rcone_r02_energy", "rcone_r02_sub1_pt", "rcone_r02_sub1_energy",
        "rcone_r03_eta", "rcone_r03_phi", "rcone_r03_pt", "rcone_r03_energy", "rcone_r03_sub1_pt", "rcone_r03_sub1_energy"
    });
  }

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
      throw std::runtime_error(std::format("Could not find Branch '{}' in file '{}'", branchName, m_input_file));
    }
  }

  // Set branches to variables
  m_chain->SetBranchAddress("event", &m_event_data.event);
  m_chain->SetBranchAddress("zvtx", &m_event_data.zvtx);
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

  if (m_do_iter)
  {
    m_chain->SetBranchAddress("seeds_iter", &m_event_data.seeds_iter);
    m_chain->SetBranchAddress("calo_v2_iter", &m_event_data.calo_v2_iter);
    m_chain->SetBranchAddress("is_flow_failure_iter", &m_event_data.is_flow_failure_iter);

    m_chain->SetBranchAddress("max_pt_iter_r02", &m_event_data.iter_r02.max_pt);
    m_chain->SetBranchAddress("max_pt_iter_r03", &m_event_data.iter_r03.max_pt);

    m_chain->SetBranchAddress("pt_iter_r02", &m_event_data.iter_r02.pt);
    m_chain->SetBranchAddress("pt_calib_iter_r02", &m_event_data.iter_r02.pt_calib);
    m_chain->SetBranchAddress("e_iter_r02", &m_event_data.iter_r02.e);
    m_chain->SetBranchAddress("phi_iter_r02", &m_event_data.iter_r02.phi);
    m_chain->SetBranchAddress("eta_iter_r02", &m_event_data.iter_r02.eta);

    m_chain->SetBranchAddress("pt_iter_r03", &m_event_data.iter_r03.pt);
    m_chain->SetBranchAddress("pt_calib_iter_r03", &m_event_data.iter_r03.pt_calib);
    m_chain->SetBranchAddress("e_iter_r03", &m_event_data.iter_r03.e);
    m_chain->SetBranchAddress("phi_iter_r03", &m_event_data.iter_r03.phi);
    m_chain->SetBranchAddress("eta_iter_r03", &m_event_data.iter_r03.eta);
  }

  if (m_do_mult)
  {
    m_chain->SetBranchAddress("seeds_mult", &m_event_data.seeds_mult);
    m_chain->SetBranchAddress("calo_v2_mult", &m_event_data.calo_v2_mult);
    m_chain->SetBranchAddress("is_flow_failure_mult", &m_event_data.is_flow_failure_mult);

    m_chain->SetBranchAddress("max_pt_mult_r02", &m_event_data.mult_r02.max_pt);
    m_chain->SetBranchAddress("max_pt_mult_r03", &m_event_data.mult_r03.max_pt);

    m_chain->SetBranchAddress("pt_mult_r02", &m_event_data.mult_r02.pt);
    m_chain->SetBranchAddress("pt_calib_mult_r02", &m_event_data.mult_r02.pt_calib);
    m_chain->SetBranchAddress("e_mult_r02", &m_event_data.mult_r02.e);
    m_chain->SetBranchAddress("phi_mult_r02", &m_event_data.mult_r02.phi);
    m_chain->SetBranchAddress("eta_mult_r02", &m_event_data.mult_r02.eta);

    m_chain->SetBranchAddress("pt_mult_r03", &m_event_data.mult_r03.pt);
    m_chain->SetBranchAddress("pt_calib_mult_r03", &m_event_data.mult_r03.pt_calib);
    m_chain->SetBranchAddress("e_mult_r03", &m_event_data.mult_r03.e);
    m_chain->SetBranchAddress("phi_mult_r03", &m_event_data.mult_r03.phi);
    m_chain->SetBranchAddress("eta_mult_r03", &m_event_data.mult_r03.eta);
  }

  if (m_do_unsub)
  {
    m_chain->SetBranchAddress("max_pt_unsub_r02", &m_event_data.unsub_r02.max_pt);
    m_chain->SetBranchAddress("max_pt_unsub_r03", &m_event_data.unsub_r03.max_pt);

    m_chain->SetBranchAddress("pt_unsub_r02", &m_event_data.unsub_r02.pt);
    m_chain->SetBranchAddress("pt_calib_unsub_r02", &m_event_data.unsub_r02.pt_calib);
    m_chain->SetBranchAddress("e_unsub_r02", &m_event_data.unsub_r02.e);
    m_chain->SetBranchAddress("phi_unsub_r02", &m_event_data.unsub_r02.phi);
    m_chain->SetBranchAddress("eta_unsub_r02", &m_event_data.unsub_r02.eta);

    m_chain->SetBranchAddress("pt_unsub_r03", &m_event_data.unsub_r03.pt);
    m_chain->SetBranchAddress("pt_calib_unsub_r03", &m_event_data.unsub_r03.pt_calib);
    m_chain->SetBranchAddress("e_unsub_r03", &m_event_data.unsub_r03.e);
    m_chain->SetBranchAddress("phi_unsub_r03", &m_event_data.unsub_r03.phi);
    m_chain->SetBranchAddress("eta_unsub_r03", &m_event_data.unsub_r03.eta);
  }

  if (m_do_rcone)
  {
    m_chain->SetBranchAddress("rcone_r02_eta", &m_event_data.rcone_r02.eta);
    m_chain->SetBranchAddress("rcone_r02_phi", &m_event_data.rcone_r02.phi);
    m_chain->SetBranchAddress("rcone_r02_pt", &m_event_data.rcone_r02.pt);
    m_chain->SetBranchAddress("rcone_r02_energy", &m_event_data.rcone_r02.energy);
    m_chain->SetBranchAddress("rcone_r02_sub1_pt", &m_event_data.rcone_r02.pt_sub1);
    m_chain->SetBranchAddress("rcone_r02_sub1_energy", &m_event_data.rcone_r02.energy_sub1);

    m_chain->SetBranchAddress("rcone_r03_eta", &m_event_data.rcone_r03.eta);
    m_chain->SetBranchAddress("rcone_r03_phi", &m_event_data.rcone_r03.phi);
    m_chain->SetBranchAddress("rcone_r03_pt", &m_event_data.rcone_r03.pt);
    m_chain->SetBranchAddress("rcone_r03_energy", &m_event_data.rcone_r03.energy);
    m_chain->SetBranchAddress("rcone_r03_sub1_pt", &m_event_data.rcone_r03.pt_sub1);
    m_chain->SetBranchAddress("rcone_r03_sub1_energy", &m_event_data.rcone_r03.energy_sub1);
  }

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

  int bins_eta = 88;
  double eta_low = -1.1;
  double eta_high = 1.1;

  int bins_eta_norm = 100;
  double eta_norm_low = 0.0;
  double eta_norm_high = 1.0;

  int bins_zvtx = 100;
  double zvtx_low = -10.0;
  double zvtx_high = 10.0;

  int bins_phi = 64;
  double phi_low = 0;
  double phi_high = 2 * std::numbers::pi;

  int bins_rcone_e = 200;
  double rcone_e_low = -100;
  double rcone_e_high = 100;

  int bins_njets = 100;
  double njets_low = 0;
  double njets_high = 100;

  unsigned int bins_event = static_cast<unsigned int>(m_eventType.size());

  auto clone_hist = [](auto &&map, std::string_view src, std::string_view dest)
  {
    using PtrType = typename std::decay_t<decltype(map)>::mapped_type;  // e.g., std::unique_ptr<TH1>
    using ElementType = typename PtrType::element_type;                 // e.g., TH1

    map[std::string(dest)] = PtrType(static_cast<ElementType *>(map[std::string(src)]->Clone(dest.data())));
  };

  m_hists1D["hEvent"] = std::make_unique<TH1F>("hEvent", "Event Type; Type; Events", bins_event, 0, bins_event);

  m_hists1D["hCentralityCaloFail"] = std::make_unique<TH1F>("hCentralityCaloFail", "Centrality Calo Failure; Centrality [%]; Events", m_bins_cent, m_cent_low, m_cent_high);

  m_hists1D["hCentrality"] = std::make_unique<TH1F>("hCentrality", "|z| < 10 cm & MB; Centrality [%]; Events", m_bins_cent, m_cent_low, m_cent_high);

  if (m_do_iter)
  {
    m_hists1D["hCaloV2Fail_iter"] = std::make_unique<TH1F>("hCaloV2Fail_iter", "; Centrality [%]; Events", m_bins_cent, m_cent_low, m_cent_high);
  }
  if (m_do_mult)
  {
    m_hists1D["hCaloV2Fail_mult"] = std::make_unique<TH1F>("hCaloV2Fail_mult", "; Centrality [%]; Events", m_bins_cent, m_cent_low, m_cent_high);
  }

  int bins_pt_eta2D = 70;
  double pt_eta2D_low = 10;
  double pt_eta2D_high = 80;

  std::vector<std::string> ue_list;
  if (m_do_iter) ue_list.push_back("iter");
  if (m_do_mult) ue_list.push_back("mult");
  if (m_do_unsub) ue_list.push_back("unsub");

  for (const char *r : {"r02", "r03"})
  {
    for (const std::string &ue : ue_list)
    {
      std::string base = std::format("hJetPt_{}_{}", r, ue);
      m_hists1D[base] = std::make_unique<TH1F>(base.c_str(), "; p_{T} [GeV]; Jets / 1 GeV", bins_pt, pt_low, pt_high);
      clone_hist(m_hists1D, base, std::format("hJetPt_raw_{}_{}", r, ue));

      for (const char *v : {"v2", "v3"})
      {
        for (const char *calib : {"", "_raw"})
        {
          clone_hist(m_hists1D, base, std::format("hJetPt{}{}_{}_{}", v, calib, r, ue));
        }
      }

      if (ue != "unsub")
      {
        for (const char *calib : {"", "_raw"})
        {
          clone_hist(m_hists1D, base, std::format("hJetPtFlowFail{}_{}_{}", calib, r, ue));
        }
      }

      std::string h2name = std::format("h2JetPtv2_{}_{}", r, ue);
      m_hists2D[h2name] = std::make_unique<TH2F>(h2name.c_str(), "; Calo v_{2}; Jet p_{T} [GeV]", bins_v2, v2_low, v2_high, bins_pt, pt_low, pt_high);

      std::string h2eta_name = std::format("h2JetEta_{}_{}", r, ue);
      m_hists2D[h2eta_name] = std::make_unique<TH2F>(h2eta_name.c_str(), "; p_{T} [GeV]; #eta", bins_pt_eta2D, pt_eta2D_low, pt_eta2D_high, bins_eta, eta_low, eta_high);
      clone_hist(m_hists2D, h2eta_name, std::format("h2JetEtav2_{}_{}", r, ue));
      clone_hist(m_hists2D, h2eta_name, std::format("h2JetEtav3_{}_{}", r, ue));

      std::string h2njets_name = std::format("h2Njets_LeadJetPt_{}_{}", r, ue);
      m_hists2D[h2njets_name] = std::make_unique<TH2F>(h2njets_name.c_str(), "; Leading Jet p_{T} [GeV]; N_{jets}", bins_pt, pt_low, pt_high, bins_njets, njets_low, njets_high);

      std::string h2njets_v2_name = std::format("h2Njets_LeadJetPtv2_{}_{}", r, ue);
      clone_hist(m_hists2D, h2njets_name, h2njets_v2_name);

      for (size_t icent = 0; icent < m_num_cent_bins; ++icent)
      {
        int cent_low = static_cast<int>(m_cent_bins[icent]);
        int cent_high = static_cast<int>(m_cent_bins[icent + 1]);

        std::string h2njets_cent_name = std::format("h2Njets_LeadJetPt_{}_{}_cent_{}_{}", r, ue, cent_low, cent_high);
        std::string cent_title = std::format("{} - {}%; Leading Jet p_{{T}} [GeV]; N_{{jets}}", cent_low, cent_high);
        m_hists2D[h2njets_cent_name] = std::make_unique<TH2F>(h2njets_cent_name.c_str(), cent_title.c_str(), bins_pt, pt_low, pt_high, bins_njets, njets_low, njets_high);

        std::string h2njets_v2_cent_name = std::format("h2Njets_LeadJetPtv2_{}_{}_cent_{}_{}", r, ue, cent_low, cent_high);
        clone_hist(m_hists2D, h2njets_cent_name, h2njets_v2_cent_name);
      }
    }
  }

  if (m_do_rcone)
  {
    for (const char *r : {"r02", "r03"})
    {
      std::string base_eta = std::format("hRConeEta_{}", r);
      m_hists1D[base_eta] = std::make_unique<TH1F>(base_eta.c_str(), "; #eta; Cones", bins_eta, eta_low, eta_high);

      std::string base_etanorm = std::format("hRConeEtaNorm_{}", r);
      m_hists1D[base_etanorm] = std::make_unique<TH1F>(base_etanorm.c_str(), "; Normalized #eta; Cones", bins_eta_norm, eta_norm_low, eta_norm_high);

      std::string base_h2etazvtx = std::format("h2RConeEtaZvtx_{}", r);
      m_hists2D[base_h2etazvtx] = std::make_unique<TH2F>(base_h2etazvtx.c_str(), "; z_{vtx} [cm]; #eta", bins_zvtx, zvtx_low, zvtx_high, bins_eta, eta_low, eta_high);

      std::string base_phi = std::format("hRConePhi_{}", r);
      m_hists1D[base_phi] = std::make_unique<TH1F>(base_phi.c_str(), "; #phi; Cones", bins_phi, phi_low, phi_high);

      std::string base_e = std::format("hRConeEnergy_unsub_{}", r);
      m_hists1D[base_e] = std::make_unique<TH1F>(base_e.c_str(), "; E [GeV]; Cones / 1 GeV", bins_rcone_e, rcone_e_low, rcone_e_high);

      std::string base_pt = std::format("hRConePt_unsub_{}", r);
      m_hists1D[base_pt] = std::make_unique<TH1F>(base_pt.c_str(), "; p_{T} [GeV]; Cones / 1 GeV", bins_pt, pt_low, pt_high);
      clone_hist(m_hists1D, base_pt, std::format("hRConePtv2_unsub_{}", r));

      std::string base_e_iter = std::format("hRConeEnergy_iter_{}", r);
      m_hists1D[base_e_iter] = std::make_unique<TH1F>(base_e_iter.c_str(), "; E [GeV]; Cones / 1 GeV", bins_rcone_e, rcone_e_low, rcone_e_high);

      std::string base_pt_iter = std::format("hRConePt_iter_{}", r);
      m_hists1D[base_pt_iter] = std::make_unique<TH1F>(base_pt_iter.c_str(), "; p_{T} [GeV]; Cones / 1 GeV", bins_pt, pt_low, pt_high);
      clone_hist(m_hists1D, base_pt_iter, std::format("hRConePtv2_iter_{}", r));

      std::string base_pe_unsub = std::format("pRConeEtaPt_unsub_{}", r);
      m_profiles[base_pe_unsub] = std::make_unique<TProfile>(base_pe_unsub.c_str(), "; #eta; Average p_{T} [GeV]", bins_eta, eta_low, eta_high);

      std::string base_pe_iter = std::format("pRConeEtaPt_iter_{}", r);
      m_profiles[base_pe_iter] = std::make_unique<TProfile>(base_pe_iter.c_str(), "; #eta; Average p_{T} [GeV]", bins_eta, eta_low, eta_high);
    }
  }

  m_hists2D["h2CaloECentrality_default"] = std::make_unique<TH2F>("h2CaloECentrality_default", "|z| < 10 cm and MB; Total Calorimeter Energy [GeV]; Centrality [%]", bins_Calo_E, Calo_E_low, Calo_E_high, m_bins_cent, m_cent_low, m_cent_high);
  clone_hist(m_hists2D, "h2CaloECentrality_default", "h2CaloECentrality");

  if (m_do_iter && m_do_mult)
  {
    m_hists2D["h2CaloV2_mult_iter"] = std::make_unique<TH2F>("h2CaloV2_mult_iter", "; Calo v_{2} Iterative; Calo v_{2} Multiplicity", bins_v2, v2_low, v2_high, bins_v2, v2_low, v2_high);
    m_hists2D["h2Seeds_iter_mult"] = std::make_unique<TH2F>("h2Seeds_iter_mult", "Seeds; Multiplicity; Iterative", bins_seeds_mult, seeds_mult_low, seeds_mult_high, bins_seeds_iter, seeds_iter_low, seeds_iter_high);
  }

  if (m_do_iter)
  {
    m_hists2D["h2CaloV2_iter_Centrality"] = std::make_unique<TH2F>("h2CaloV2_iter_Centrality", "; Centrality [%]; Calo v_{2}", m_bins_cent, m_cent_low, m_cent_high, bins_v2, v2_low, v2_high);
    m_hists2D["h2Seeds_iter"] = std::make_unique<TH2F>("h2Seeds_iter", "Iterative Seeds; Centrality [%]; Seeds Iterative", m_bins_cent, m_cent_low, m_cent_high, bins_seeds_iter, seeds_iter_low, seeds_iter_high);
  }

  if (m_do_mult)
  {
    m_hists2D["h2CaloV2_mult_Centrality"] = std::make_unique<TH2F>("h2CaloV2_mult_Centrality", "; Centrality [%]; Calo v_{2}", m_bins_cent, m_cent_low, m_cent_high, bins_v2, v2_low, v2_high);
    m_hists2D["h2Seeds_mult"] = std::make_unique<TH2F>("h2Seeds_mult", "Multiplicity Seeds; Centrality [%]; Seeds Multiplicity", m_bins_cent, m_cent_low, m_cent_high, bins_seeds_mult, seeds_mult_low, seeds_mult_high);
  }

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

  if (m_do_iter)
  {
    m_hists.hCaloV2Fail_iter = m_hists1D["hCaloV2Fail_iter"].get();
    m_hists.h2CaloV2_iter_Centrality = m_hists2D["h2CaloV2_iter_Centrality"].get();
    m_hists.h2Seeds_iter = m_hists2D["h2Seeds_iter"].get();
  }

  if (m_do_mult)
  {
    m_hists.hCaloV2Fail_mult = m_hists1D["hCaloV2Fail_mult"].get();
    m_hists.h2CaloV2_mult_Centrality = m_hists2D["h2CaloV2_mult_Centrality"].get();
    m_hists.h2Seeds_mult = m_hists2D["h2Seeds_mult"].get();
  }

  if (m_do_iter && m_do_mult)
  {
    m_hists.h2CaloV2_mult_iter = m_hists2D["h2CaloV2_mult_iter"].get();
    m_hists.h2Seeds_iter_mult = m_hists2D["h2Seeds_iter_mult"].get();
  }

  auto bind_jet_set = [&](AnalysisHists::JetHistSet &set, std::string_view r, std::string_view ue)
  {
    set.hJetPt = m_hists1D[std::format("hJetPt_{}_{}", r, ue)].get();
    set.hJetPt_raw = m_hists1D[std::format("hJetPt_raw_{}_{}", r, ue)].get();
    set.hJetPtv2 = m_hists1D[std::format("hJetPtv2_{}_{}", r, ue)].get();
    set.hJetPtv2_raw = m_hists1D[std::format("hJetPtv2_raw_{}_{}", r, ue)].get();
    set.hJetPtv3 = m_hists1D[std::format("hJetPtv3_{}_{}", r, ue)].get();
    set.hJetPtv3_raw = m_hists1D[std::format("hJetPtv3_raw_{}_{}", r, ue)].get();
    if (ue != "unsub")
    {
      set.hJetPtFlowFail = m_hists1D[std::format("hJetPtFlowFail_{}_{}", r, ue)].get();
      set.hJetPtFlowFail_raw = m_hists1D[std::format("hJetPtFlowFail_raw_{}_{}", r, ue)].get();
    }
    set.h2JetPtv2 = m_hists2D[std::format("h2JetPtv2_{}_{}", r, ue)].get();
    set.h2JetEta = m_hists2D[std::format("h2JetEta_{}_{}", r, ue)].get();
    set.h2JetEtav2 = m_hists2D[std::format("h2JetEtav2_{}_{}", r, ue)].get();
    set.h2JetEtav3 = m_hists2D[std::format("h2JetEtav3_{}_{}", r, ue)].get();

    set.h2Njets_LeadJetPt = m_hists2D[std::format("h2Njets_LeadJetPt_{}_{}", r, ue)].get();
    set.h2Njets_LeadJetPtv2 = m_hists2D[std::format("h2Njets_LeadJetPtv2_{}_{}", r, ue)].get();
    for (size_t icent = 0; icent < m_num_cent_bins; ++icent)
    {
      int cent_low = static_cast<int>(m_cent_bins[icent]);
      int cent_high = static_cast<int>(m_cent_bins[icent + 1]);
      set.h2Njets_LeadJetPt_cent[icent] = m_hists2D[std::format("h2Njets_LeadJetPt_{}_{}_cent_{}_{}", r, ue, cent_low, cent_high)].get();
      set.h2Njets_LeadJetPtv2_cent[icent] = m_hists2D[std::format("h2Njets_LeadJetPtv2_{}_{}_cent_{}_{}", r, ue, cent_low, cent_high)].get();
    }
  };

  if (m_do_iter)
  {
    bind_jet_set(m_hists.iter_r02, "r02", "iter");
    bind_jet_set(m_hists.iter_r03, "r03", "iter");
  }
  if (m_do_mult)
  {
    bind_jet_set(m_hists.mult_r02, "r02", "mult");
    bind_jet_set(m_hists.mult_r03, "r03", "mult");
  }
  if (m_do_unsub)
  {
    bind_jet_set(m_hists.unsub_r02, "r02", "unsub");
    bind_jet_set(m_hists.unsub_r03, "r03", "unsub");
  }

  if (m_do_rcone)
  {
    auto bind_rcone_set = [&](AnalysisHists::RConeHistSet &set, std::string_view r, std::string_view ue)
    {
      if (ue == "unsub")
      {
        set.hEta = m_hists1D[std::format("hRConeEta_{}", r)].get();
        set.hEtaNorm = m_hists1D[std::format("hRConeEtaNorm_{}", r)].get();
        set.h2EtaZvtx = m_hists2D[std::format("h2RConeEtaZvtx_{}", r)].get();
        set.hPhi = m_hists1D[std::format("hRConePhi_{}", r)].get();
      }
      set.hEnergy = m_hists1D[std::format("hRConeEnergy_{}_{}", ue, r)].get();
      set.hPt = m_hists1D[std::format("hRConePt_{}_{}", ue, r)].get();
      set.hPtv2 = m_hists1D[std::format("hRConePtv2_{}_{}", ue, r)].get();
      set.pEtaPt = m_profiles[std::format("pRConeEtaPt_{}_{}", ue, r)].get();
    };

    bind_rcone_set(m_hists.rcone_r02, "r02", "unsub");
    bind_rcone_set(m_hists.rcone_r03, "r03", "unsub");
    bind_rcone_set(m_hists.rcone_iter_r02, "r02", "iter");
    bind_rcone_set(m_hists.rcone_iter_r03, "r03", "iter");
  }

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

    int njets = 0;
    int njets_v2 = 0;
    double max_pt_v2 = 0.0;

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

      ++njets;

      h.hJetPt->Fill(pt);
      h.hJetPt_raw->Fill(pt_raw);
      h.h2JetEta->Fill(pt, eta);

      if (energy > 0)
      {
        ++njets_v2;
        max_pt_v2 = std::max(max_pt_v2, pt);

        h.hJetPtv2->Fill(pt);
        h.hJetPtv2_raw->Fill(pt_raw);
        h.h2JetPtv2->Fill(calo_v2, pt);
        h.h2JetEtav2->Fill(pt, eta);

        if (std::abs(calo_v2) < m_calo_v2_max)
        {
          h.hJetPtv3->Fill(pt);
          h.hJetPtv3_raw->Fill(pt_raw);
          h.h2JetEtav3->Fill(pt, eta);
        }
      }
    }

    if (jet_data.max_pt > 0)
    {
      if (h.h2Njets_LeadJetPt)
      {
        h.h2Njets_LeadJetPt->Fill(jet_data.max_pt, static_cast<double>(njets));
      }
    }

    if (max_pt_v2 > 0)
    {
      if (h.h2Njets_LeadJetPtv2)
      {
        h.h2Njets_LeadJetPtv2->Fill(max_pt_v2, static_cast<double>(njets_v2));
      }
    }

    double cent = m_event_data.centrality;
    for (size_t icent = 0; icent < m_num_cent_bins; ++icent)
    {
      double cent_low = m_cent_bins[icent];
      double cent_high = m_cent_bins[icent + 1];
      if (cent >= cent_low && (cent < cent_high || (icent == m_num_cent_bins - 1 && cent <= cent_high)))
      {
        if (jet_data.max_pt > 0 && h.h2Njets_LeadJetPt_cent[icent])
        {
          h.h2Njets_LeadJetPt_cent[icent]->Fill(jet_data.max_pt, static_cast<double>(njets));
        }
        if (max_pt_v2 > 0 && h.h2Njets_LeadJetPtv2_cent[icent])
        {
          h.h2Njets_LeadJetPtv2_cent[icent]->Fill(max_pt_v2, static_cast<double>(njets_v2));
        }
        break;
      }
    }
  };

  auto fill_jet_flow_fail_hists = [this](const JetData &jet_data, const AnalysisHists::JetHistSet &h, double eta_max)
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

      if (energy > 0)
      {
        h.hJetPtFlowFail->Fill(pt);
        h.hJetPtFlowFail_raw->Fill(pt_raw);
      }
    }
  };

  if (m_do_iter)
  {
    // Process r02 branches for iter (eta_max = 0.9)
    if (!m_event_data.is_flow_failure_iter)
    {
      fill_jet_hists(m_event_data.iter_r02, m_event_data.calo_v2_iter, m_hists.iter_r02, m_jet_eta_max_r02);
    }
    else
    {
      fill_jet_flow_fail_hists(m_event_data.iter_r02, m_hists.iter_r02, m_jet_eta_max_r02);
    }

    // Process r03 branches for iter (eta_max = 0.8)
    if (!m_event_data.is_flow_failure_iter)
    {
      fill_jet_hists(m_event_data.iter_r03, m_event_data.calo_v2_iter, m_hists.iter_r03, m_jet_eta_max_r03);
    }
    else
    {
      fill_jet_flow_fail_hists(m_event_data.iter_r03, m_hists.iter_r03, m_jet_eta_max_r03);
    }
  }

  if (m_do_mult)
  {
    // Process r02 branches for mult (eta_max = 0.9)
    if (!m_event_data.is_flow_failure_mult)
    {
      fill_jet_hists(m_event_data.mult_r02, m_event_data.calo_v2_mult, m_hists.mult_r02, m_jet_eta_max_r02);
    }
    else
    {
      fill_jet_flow_fail_hists(m_event_data.mult_r02, m_hists.mult_r02, m_jet_eta_max_r02);
    }

    // Process r03 branches for mult (eta_max = 0.8)
    if (!m_event_data.is_flow_failure_mult)
    {
      fill_jet_hists(m_event_data.mult_r03, m_event_data.calo_v2_mult, m_hists.mult_r03, m_jet_eta_max_r03);
    }
    else
    {
      fill_jet_flow_fail_hists(m_event_data.mult_r03, m_hists.mult_r03, m_jet_eta_max_r03);
    }
  }

  if (m_do_unsub)
  {
    // Process unsubtracted jets
    fill_jet_hists(m_event_data.unsub_r02, 0, m_hists.unsub_r02, m_jet_eta_max_r02);
    fill_jet_hists(m_event_data.unsub_r03, 0, m_hists.unsub_r03, m_jet_eta_max_r03);
  }
}

void JetAnalysisv3::process_rcones()
{
  if (!m_event_data.pass_calo_cent)
  {
    return;
  }

  if (m_do_rcone)
  {
    auto fill_rcone = [&](const RConeData &rc, const AnalysisHists::RConeHistSet &h, bool use_sub1, double r_val)
    {
      double eta = rc.eta;
      double phi = rc.phi;
      double pt = use_sub1 ? rc.pt_sub1 : rc.pt;
      double energy = use_sub1 ? rc.energy_sub1 : rc.energy;

      if (h.hEta) h.hEta->Fill(eta);
      if (h.hEtaNorm)
      {
        auto [eta_min, eta_max] = JetUtils::get_valid_eta_range(m_event_data.zvtx, r_val);
        if (eta_max > eta_min)
        {
          double eta_norm = (eta - eta_min) / (eta_max - eta_min);
          h.hEtaNorm->Fill(eta_norm);
        }
      }
      if (h.h2EtaZvtx) h.h2EtaZvtx->Fill(m_event_data.zvtx, eta);
      if (h.hPhi) h.hPhi->Fill(phi);
      if (h.hEnergy) h.hEnergy->Fill(energy);
      if (h.hPt) h.hPt->Fill(pt);
      if (h.hPtv2 && energy > 0) h.hPtv2->Fill(pt);
      if (h.pEtaPt) h.pEtaPt->Fill(eta, pt);
    };

    fill_rcone(m_event_data.rcone_r02, m_hists.rcone_r02, false, 0.2);
    fill_rcone(m_event_data.rcone_r03, m_hists.rcone_r03, false, 0.3);

    if (!m_event_data.is_flow_failure_iter)
    {
      fill_rcone(m_event_data.rcone_r02, m_hists.rcone_iter_r02, true, 0.2);
      fill_rcone(m_event_data.rcone_r03, m_hists.rcone_iter_r03, true, 0.3);
    }
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

    if (m_do_iter)
    {
      if (ed.is_flow_failure_iter)
      {
        m_hists.hCaloV2Fail_iter->Fill(cent);
        ++m_ctr["events_flow_failure_iter"];
      }
      else
      {
        m_hists.hEvent->Fill(static_cast<std::uint8_t>(EventType::FLOW_PASS_ITER));
        m_hists.h2CaloV2_iter_Centrality->Fill(cent, ed.calo_v2_iter);
        m_hists.h2Seeds_iter->Fill(cent, ed.seeds_iter);
      }
    }

    if (m_do_mult)
    {
      if (ed.is_flow_failure_mult)
      {
        m_hists.hCaloV2Fail_mult->Fill(cent);
        ++m_ctr["events_flow_failure_mult"];
      }
      else
      {
        m_hists.hEvent->Fill(static_cast<std::uint8_t>(EventType::FLOW_PASS_MULT));
        m_hists.h2CaloV2_mult_Centrality->Fill(cent, ed.calo_v2_mult);
        m_hists.h2Seeds_mult->Fill(cent, ed.seeds_mult);
      }
    }

    if (m_do_iter && m_do_mult)
    {
      if (!ed.is_flow_failure_iter && !ed.is_flow_failure_mult)
      {
        m_hists.h2CaloV2_mult_iter->Fill(ed.calo_v2_iter, ed.calo_v2_mult);
        m_hists.h2Seeds_iter_mult->Fill(ed.seeds_mult, ed.seeds_iter);
      }
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
    process_rcones();

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
  if (m_do_iter)
  {
    std::cout << std::format(" Calo v2 Iter: {:.4f} (Fail: {}), Seeds Iter: {}\n",
                             m_event_data.calo_v2_iter, m_event_data.is_flow_failure_iter ? "Yes" : "No",
                             m_event_data.seeds_iter);
  }
  if (m_do_mult)
  {
    std::cout << std::format(" Calo v2 Mult: {:.4f} (Fail: {}), Seeds Mult: {}\n",
                             m_event_data.calo_v2_mult, m_event_data.is_flow_failure_mult ? "Yes" : "No",
                             m_event_data.seeds_mult);
  }
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
        std::cout << std::format("    {:>4} | {:>10.2f} | {:>10.2f} | {:>10.2f} | {:>10.3f} | {:>10.3f}\n", i, pt_raw, pt_cal, e, eta, phi);
      }
    }
  };

  std::cout << " -- Jet Collections --\n";
  if (m_do_iter)
  {
    print_jet_collection("r02_iter", m_event_data.iter_r02);
    print_jet_collection("r03_iter", m_event_data.iter_r03);
  }
  if (m_do_mult)
  {
    print_jet_collection("r02_mult", m_event_data.mult_r02);
    print_jet_collection("r03_mult", m_event_data.mult_r03);
  }
  if (m_do_unsub)
  {
    print_jet_collection("r02_unsub", m_event_data.unsub_r02);
    print_jet_collection("r03_unsub", m_event_data.unsub_r03);
  }
  std::cout << std::format("{:=^70}\n", "");
}

void JetAnalysisv3::save_results() const
{
  std::filesystem::create_directories(m_output_dir);

  std::filesystem::path input_path(m_input_file);
  std::string output_stem = input_path.stem().string();
  std::string output_filename = std::format("{}/Jet-Ana_{}.root", m_output_dir, output_stem);

  auto output_file = std::make_unique<TFile>(output_filename.c_str(), "RECREATE");

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

  std::cout << std::format("Results saved to: {}", output_filename) << std::endl;
}

// ====================================================================
// Main function remains clean and simple
// ====================================================================
int main(int argc, const char *const argv[])
{
  gROOT->SetBatch(true);
  TH1::AddDirectory(false);

  if (argc < 2 || argc > 10)
  {
    std::cout << "Usage: " << argv[0] << " input_file [events] [jet_pt_min] [output_directory] [verbosity] [do_iter] [do_mult] [do_unsub] [do_rcone]" << std::endl;
    return 1;
  }

  int ctr = 1;
  const std::string input_file = argv[ctr++];
  long long events = (argc >= ctr + 1) ? std::atoll(argv[ctr++]) : 0;
  double jet_pt_min = (argc >= ctr + 1) ? std::stod(argv[ctr++]) : 10;
  std::string output_dir = (argc >= ctr + 1) ? argv[ctr++] : ".";
  int verbosity = (argc >= ctr + 1) ? std::atoi(argv[ctr++]) : 0;
  bool do_iter = (argc >= ctr + 1) ? (std::atoi(argv[ctr++]) != 0) : true;
  bool do_mult = (argc >= ctr + 1) ? (std::atoi(argv[ctr++]) != 0) : true;
  bool do_unsub = (argc >= ctr + 1) ? (std::atoi(argv[ctr++]) != 0) : true;
  bool do_rcone = (argc >= ctr + 1) ? (std::atoi(argv[ctr++]) != 0) : true;

  std::cout << std::format("{:#<20}\n", "");
  std::cout << std::format("Run Params\n");
  std::cout << std::format("Input: {}\n", input_file);
  std::cout << std::format("Events: {}\n", events);
  std::cout << std::format("Jet pT min: {} [GeV]\n", jet_pt_min);
  std::cout << std::format("Output Dir: {}\n", output_dir);
  std::cout << std::format("Verbosity: {}\n", verbosity);
  std::cout << std::format("Do Iter: {}\n", do_iter);
  std::cout << std::format("Do Mult: {}\n", do_mult);
  std::cout << std::format("Do Unsub: {}\n", do_unsub);
  std::cout << std::format("Do RCone: {}\n", do_rcone);
  std::cout << std::format("{:#<20}\n", "");

  try
  {
    JetAnalysisv3 analysis(input_file, events, output_dir);
    analysis.set_jet_pt_min(jet_pt_min);
    analysis.set_verbosity(verbosity);
    analysis.set_do_iter(do_iter);
    analysis.set_do_mult(do_mult);
    analysis.set_do_unsub(do_unsub);
    analysis.set_do_rcone(do_rcone);
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
