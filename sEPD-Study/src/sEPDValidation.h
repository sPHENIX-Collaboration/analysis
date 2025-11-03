// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#pragma once

// -- sPHENIX
#include <eventplaneinfo/Eventplaneinfo.h>
#include <fun4all/SubsysReco.h>

// -- c++
#include <cstdint>  // Required for std::uint8_t, std::uint16_t, etc.
#include <filesystem>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

// -- ROOT
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TMath.h>
#include <TProfile2D.h>
#include <TTree.h>

class PHCompositeNode;

class sEPDValidation : public SubsysReco
{
 public:
  explicit sEPDValidation(const std::string &name = "sEPDValidation");

  int Init(PHCompositeNode *topNode) override;
  int process_event(PHCompositeNode *topNode) override;
  // Clean up internals after each event.
  int ResetEvent(PHCompositeNode *topNode) override;
  int End(PHCompositeNode *topNode) override;

  void set_filename(std::string_view file)
  {
    m_outfile_name = file;
  }

  void set_tree_filename(std::string_view file)
  {
    m_outtree_name = file;
  }

  void set_condor_mode(const bool condor_mode = true)
  {
    m_condor_mode = condor_mode;
  }

  void set_do_ep(const bool do_ep = true)
  {
    m_do_ep = do_ep;
  }

 private:
  int process_event_check(PHCompositeNode *topNode);
  int process_MBD(PHCompositeNode *topNode);
  int process_sEPD(PHCompositeNode *topNode);
  int process_EventPlane(PHCompositeNode *topNode);
  int process_EventPlane(Eventplaneinfo *epd_S, Eventplaneinfo *epd_N, int order = 2);
  int process_centrality(PHCompositeNode *topNode);
  int process_jets(PHCompositeNode *topNode);

  int m_event{0};

  std::string m_outfile_name{"test.root"};
  std::string m_outtree_name{"tree.root"};
  bool m_condor_mode{false};
  bool m_do_ep{true};

  struct HistConfig
  {
    unsigned int m_bins_zvtx{200};
    double m_zvtx_low{-50};
    double m_zvtx_high{50};

    unsigned int m_bins_cent{80};
    unsigned int m_bins_cent_reduced{8};
    double m_cent_low{-0.5};
    double m_cent_high{79.5};

    unsigned int m_bins_sepd_channels{744};

    unsigned int m_bins_sepd_channel_charge{300};
    double m_sepd_channel_charge_low{0};
    double m_sepd_channel_charge_high{3};

    unsigned int m_bins_sepd_channel_chargev2{210};
    double m_sepd_channel_chargev2_low{-10};
    double m_sepd_channel_chargev2_high{200};

    unsigned int m_bins_sepd_charge{200};
    double m_sepd_charge_low{0};
    double m_sepd_charge_high{2e4};

    unsigned int m_bins_mbd_charge{80};
    double m_mbd_charge_low{0};
    double m_mbd_charge_high{2e3};

    unsigned int m_bins_mbd_phi{60};
    double m_mbd_phi_low{-M_PI};
    double m_mbd_phi_high{M_PI};

    unsigned int m_bins_mbd_eta{7};
    double m_mbd_eta_low{3.6};
    double m_mbd_eta_high{4.3};

    unsigned int m_bins_sepd_total_charge{200};
    double m_sepd_total_charge_low{0};
    double m_sepd_total_charge_high{4e4};

    unsigned int m_bins_mbd_total_charge{200};
    double m_mbd_total_charge_low{0};
    double m_mbd_total_charge_high{5e3};

    unsigned int m_bins_psi{126};
    double m_psi_low{-M_PI};
    double m_psi_high{M_PI};

    unsigned int m_bins_sepd_phi{60};
    double m_sepd_phi_low{-M_PI};
    double m_sepd_phi_high{M_PI};

    unsigned int m_bins_sepd_eta{52};
    double m_sepd_eta_low{2};
    double m_sepd_eta_high{4.6};

    unsigned int m_bins_sepd_rbin{16};
    double m_sepd_rbin_low{-0.5};
    double m_sepd_rbin_high{15.5};

    unsigned int m_bins_jet_pt{100};
    double m_jet_pt_low{0};
    double m_jet_pt_high{100};

    unsigned int m_bins_jet_eta{24};
    double m_jet_eta_low{-1};
    double m_jet_eta_high{1};

    unsigned int m_bins_jet_phi{64};
    double m_jet_phi_low{-M_PI};
    double m_jet_phi_high{M_PI};

    unsigned int m_bins_jet_constituents{100};
    double m_jet_constituents_low{0};
    double m_jet_constituents_high{100};

    unsigned int m_bins_jet_nEvent{20};
    double m_jet_nEvent_low{0};
    double m_jet_nEvent_high{20};

    unsigned int m_bins_frcem{140};
    double m_frcem_low{-0.2};
    double m_frcem_high{1.2};
  };

  HistConfig m_hist_config;

  // A struct to hold all histogram configuration data
  struct HistDef
  {
    enum class Type
    {
      TH1,
      TH2,
      TH3,
      TProfile,
      TProfile2D
    };
    Type type;
    std::string name;
    std::string title;

    // Axis definitions (use for X, Y, Z as needed)
    struct Axis
    {
      unsigned int bins;
      double low;
      double high;
    };
    Axis x, y, z;
  };

  // Helper function to create histograms
  void create_histogram(const HistDef &def);

  enum class EventType : std::uint8_t
  {
    ALL,
    ZVTX10,
    ZVTX10_MB,
    ZVTX10_MB_JET
  };

  enum class MinBiasType : std::uint8_t
  {
    BKG_HIGH,
    SIDE_HIT_LOW,
    ZDC_LOW,
    MBD_HIGH
  };

  enum class JetBkgType : std::uint8_t
  {
    STREAK,
    LOEM,
    HIEM,
    IH
  };

  std::vector<std::string> m_eventType{"All", "|z| < 10 cm", "|z| < 10 cm & MB", "Has Jet"};
  std::vector<std::string> m_MinBias_Type{"MBD Background", "Hits < 2", "ZDC < 60 GeV", "MBD > 2100"};
  std::vector<std::string> m_JetBkg_Type{"Streak", "LoEM", "HiEM", "Ih"};

  // Event Vars
  double m_zvtx{9999};
  double m_cent{9999};
  double m_mbd_total_charge{9999};
  bool m_hasBeamBackground{false};
  bool m_failsLoEmJetCut{false};
  bool m_failsHiEmJetCut{false};
  bool m_failsIhJetCut{false};
  bool m_failsAnyJetCut{false};
  bool m_isDijet{false};
  double m_frcem{0};
  double m_frcoh{0};
  double m_maxJetET{0};
  double m_dPhi{0};

  // Cuts
  struct EventCuts
  {
    double m_zvtx_max{10};
    double m_sepd_charge_threshold{0.2};
    double m_cent_max{80};
  };

  EventCuts m_cuts;

  // Logging Info
  struct LoggingInfo
  {
    double m_cent_min{9999};
    double m_cent_max{0};
    double m_sepd_z_min{9999};
    double m_sepd_z_max{0};
    double m_sepd_r_min{9999};
    double m_sepd_r_max{0};
    double m_sepd_rbin_min{9999};
    double m_sepd_rbin_max{0};
    double m_sepd_phi_min{9999};
    double m_sepd_phi_max{0};
    double m_sepd_eta_min{9999};
    double m_sepd_eta_max{0};
    double m_sepd_charge_min{9999};
    double m_sepd_charge_max{0};
    double m_mbd_ch_z_min{9999};
    double m_mbd_ch_z_max{0};
    double m_mbd_ch_r_min{9999};
    double m_mbd_ch_r_max{0};
    double m_mbd_ch_phi_min{9999};
    double m_mbd_ch_phi_max{0};
    double m_mbd_ch_eta_min{9999};
    double m_mbd_ch_eta_max{0};
    double m_mbd_ch_charge_min{9999};
    double m_mbd_ch_charge_max{0};
    double m_mbd_charge_min{9999};
    double m_mbd_charge_max{0};
    double m_mbd_total_charge_min{9999};
    double m_mbd_total_charge_max{0};
    double m_vertex_scale_min{9999};
    double m_vertex_scale_max{0};
    double m_centrality_scale_min{9999};
    double m_centrality_scale_max{0};
    double m_sepd_total_charge_south_min{9999};
    double m_sepd_total_charge_south_max{0};
    double m_sepd_total_charge_north_min{9999};
    double m_sepd_total_charge_north_max{0};
    double m_psi_min{9999};
    double m_psi_max{0};
    double m_jet_pt_min{9999};
    double m_jet_pt_max{0};
    double m_jet_phi_min{9999};
    double m_jet_phi_max{0};
    double m_jet_eta_min{9999};
    double m_jet_eta_max{0};
    double m_jet_constituents_min{9999};
    double m_jet_constituents_max{0};
    int m_jet_nEvent_min{9999};
    int m_jet_nEvent_max{0};
    double m_frcem_min{9999};
    double m_frcem_max{0};
    double m_frcoh_min{9999};
    double m_frcoh_max{0};
    double m_maxJetET_min{9999};
    double m_maxJetET_max{0};
    double m_dPhi_min{9999};
    double m_dPhi_max{0};
  };

  LoggingInfo m_logging;

  std::map<std::string, std::unique_ptr<TH1>> m_hists;
  std::map<std::string, int> m_ctr;

  struct EventData
  {
    int event_id{0};
    double event_zvertex{9999};
    double event_centrality{9999};
    bool hasBkg{false};
    std::vector<int> sepd_channel;
    std::vector<double> sepd_charge;
    std::vector<double> sepd_phi;
    // std::vector<double> mbd_charge;
    // std::vector<double> mbd_phi;
    // std::vector<double> mbd_eta;
    std::vector<double> jet_pt;
    std::vector<double> jet_phi;
    std::vector<double> jet_eta;
  };

  EventData m_data;

  std::unique_ptr<TFile> m_output;
  TTree *m_tree{nullptr};

  std::string m_recoJetName{"AntiKt_Tower_r02_Sub1"};
  double m_jet_pt_min_cut{7};     // GeV
  double m_jet_eta_max_cut{0.9};  // 1.1-R
};
