#pragma once

#include "geometry_constants.h"

// -- sPHENIX
#include <fun4all/SubsysReco.h>

// -- c++
#include <cstdint>  // Required for std::uint8_t, std::uint16_t, etc.
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <vector>
#include <numbers>

class PHCompositeNode;
class TFile;
class TTree;
class TH1;
class TH2;
class TProfile;
class TProfile2D;

class sEPD_MC_Validation : public SubsysReco
{
 public:
  explicit sEPD_MC_Validation(const std::string &name = "sEPD_MC_Validation");

  int Init(PHCompositeNode *topNode) override;
  int process_event(PHCompositeNode *topNode) override;
  // Clean up internals after each event.
  int ResetEvent(PHCompositeNode *topNode) override;
  int End(PHCompositeNode *topNode) override;

  void set_print_interval(int events)
  {
    m_progress_print = events;
  }

 private:
  int process_event_check(PHCompositeNode *topNode);
  int process_centrality(PHCompositeNode *topNode);
  int process_EventPlane(PHCompositeNode *topNode);

  int m_progress_print{1000};
  int m_event{0};

  std::map<std::string, int> m_ctr;

  struct HistConfig
  {
    unsigned int m_bins_zvtx{200};
    double m_zvtx_low{-50};
    double m_zvtx_high{50};

    unsigned int m_bins_cent{80};
    double m_cent_low{-0.5};
    double m_cent_high{79.5};

    unsigned int m_bins_psi{126};
    double m_psi_low{-std::numbers::pi};
    double m_psi_high{std::numbers::pi};
  };

  HistConfig m_hist_config;

  // Cuts
  struct EventCuts
  {
    double m_zvtx_max{10};
    double m_zvtx_max_v2{50};
    double m_cent_max{80};
  };

  EventCuts m_cuts;

  struct EventData
  {
    int event{0};
    double zvtx{9999};
    double centrality{9999};
  };

  EventData m_data;

  enum class EventType : std::uint8_t
  {
    ALL,
    ZVTX,
    ZVTX50,
    ZVTX10,
    MB,
    CENT,
  };

  enum class MinBiasType : std::uint8_t
  {
    BKG_HIGH,
    SIDE_HIT_LOW,
    ZDC_LOW,
    MBD_HIGH
  };

  std::vector<std::string> m_eventType{"All", "Has Z", "|z| < 50 cm", "|z| < 10 cm", "MB", "Cent"};
  std::vector<std::string> m_MinBias_Type{"MBD Background", "Hits < 2", "ZDC < 60 GeV", "MBD > 2100"};

  TH1* hEvent{nullptr};
  TH1* hEventMinBias{nullptr};
  TH1* hVtxZ{nullptr};
  TH1* hVtxZ_MB{nullptr};
  TH1* hCentrality{nullptr};

  TH2* h2SEPD_Psi2_raw_S{nullptr};
  TH2* h2SEPD_Psi2_raw_N{nullptr};

  TH2* h2SEPD_Psi2_S{nullptr};
  TH2* h2SEPD_Psi2_N{nullptr};

  TH2* h2Truth_Psi2{nullptr};

  TH2* h2Delta_TruthPsi2_RecoPsi2_South{nullptr};
  TH2* h2Delta_TruthPsi2_RecoPsi2_North{nullptr};

  TH2* h2Delta_RecoPsi2_North_South{nullptr};
};
