// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#pragma once

// -- sPHENIX
#include <eventplaneinfo/Eventplaneinfo.h>
#include <fun4all/SubsysReco.h>

// -- c++
#include <cstdint>  // Required for std::uint8_t, std::uint16_t, etc.
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <vector>
#include <filesystem>

// -- ROOT
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TProfile2D.h>
#include <TMath.h>
#include <TTree.h>
#include <TFile.h>

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

  void set_q_vec_corr(std::string_view file)
  {
    m_q_vec_corr_fname = file;
    if (std::filesystem::exists(file))
    {
      m_do_q_vec_corr = true;
      m_do_q_vec_corr2 = true;
    }
  }

 private:
  int process_event_check(PHCompositeNode *topNode);
  int process_MBD(PHCompositeNode *topNode);
  int process_sEPD(PHCompositeNode *topNode);
  int process_EventPlane(PHCompositeNode *topNode);
  int process_EventPlane(Eventplaneinfo *epd_S, Eventplaneinfo *epd_N, int order = 2);
  int process_centrality(PHCompositeNode *topNode);

  int m_event{0};

  std::string m_outfile_name{"test.root"};
  std::string m_outtree_name{"tree.root"};
  bool m_condor_mode{false};
  bool m_do_ep{true};
  std::string m_q_vec_corr_fname;
  bool m_do_q_vec_corr{false};
  bool m_do_q_vec_corr2{false};

  struct HistConfig
  {
    unsigned int m_bins_zvtx{200};
    double m_zvtx_low{-50};
    double m_zvtx_high{50};

    unsigned int m_bins_cent{100};
    double m_cent_low{-0.5};
    double m_cent_high{99.5};

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

    unsigned int m_bins_sepd_Q{100};
    double m_sepd_Q_low{-1};
    double m_sepd_Q_high{1};

    unsigned int m_bins_sepd_total_charge{200};
    double m_sepd_total_charge_low{0};
    double m_sepd_total_charge_high{4e4};

    unsigned int m_bins_mbd_total_charge{200};
    double m_mbd_total_charge_low{0};
    double m_mbd_total_charge_high{5e3};

    unsigned int m_bins_psi{126};
    double m_psi_low{-M_PI};
    double m_psi_high{M_PI};
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
    ZVTX10_MB
  };

  enum class MinBiasType : std::uint8_t
  {
    BKG_HIGH,
    SIDE_HIT_LOW,
    ZDC_LOW,
    MBD_HIGH
  };

  std::vector<std::string> m_eventType{"All", "|z| < 10 cm", "|z| < 10 cm & MB"};
  std::vector<std::string> m_MinBias_Type{"MBD Background", "Hits < 2", "ZDC < 60 GeV", "MBD > 2100"};

  // Event Vars
  double m_zvtx{9999};
  double m_cent{9999};
  double m_mbd_total_charge{9999};

  // Cuts
  struct EventCuts
  {
    double m_zvtx_max{10};
    double m_sepd_charge_threshold{0.2};
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
    double m_sepd_Q_min{9999};
    double m_sepd_Q_max{0};
    double m_sepd_total_charge_south_min{9999};
    double m_sepd_total_charge_south_max{0};
    double m_sepd_total_charge_north_min{9999};
    double m_sepd_total_charge_north_max{0};
    double m_psi_min{9999};
    double m_psi_max{0};
  };

  LoggingInfo m_logging;

  std::map<std::string, std::unique_ptr<TH1>> m_hists;
  std::map<std::string, int> m_ctr;

  struct EventData
  {
    int event_id{0};
    double event_zvertex{9999};
    double event_centrality{9999};
    std::vector<double> sepd_charge;
    std::vector<double> sepd_phi;
    std::vector<double> sepd_eta;
    std::vector<double> mbd_charge;
    std::vector<double> mbd_phi;
    std::vector<double> mbd_eta;
  };

  EventData m_data;

  std::unique_ptr<TFile> m_output;
  TTree* m_tree{nullptr};
};
