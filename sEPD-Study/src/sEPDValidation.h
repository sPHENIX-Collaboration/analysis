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
#include <vector>
#include <filesystem>

// -- ROOT
#include <TH1.h>
#include <TMath.h>

class PHCompositeNode;

class sEPDValidation : public SubsysReco
{
 public:
  explicit sEPDValidation(const std::string &name = "sEPDValidation");

  int Init(PHCompositeNode *topNode) override;
  int process_event(PHCompositeNode *topNode) override;
  int End(PHCompositeNode *topNode) override;

  void set_filename(const std::string &file)
  {
    m_outfile_name = file;
  }

  void set_condor_mode(const bool condor_mode = true)
  {
    m_condor_mode = condor_mode;
  }

  void set_do_ep(const bool do_ep = true)
  {
    m_do_ep = do_ep;
  }

  void set_q_vec_corr(const std::string &file)
  {
    m_q_vec_corr_fname = file;
    if (std::filesystem::exists(file))
    {
      m_do_q_vec_corr = true;
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
  bool m_condor_mode{false};
  bool m_do_ep{true};
  std::string m_q_vec_corr_fname;
  bool m_do_q_vec_corr{false};

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
  double m_zvtx_max{10};
  double m_sepd_charge_threshold{0.2};

  // Logging Info
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

  std::map<std::string, std::unique_ptr<TH1>> m_hists;
  std::map<std::string, int> m_ctr;
};
