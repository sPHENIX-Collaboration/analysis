// Tell emacs that this is a C++ source
//  -*- C++ -*-.
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

class TriggerAnalyzer;
class PHCompositeNode;
class TFile;
class TTree;
class TH1;
class TH2;
class TProfile;
class TProfile2D;

class JetValidationv2 : public SubsysReco
{
 public:
  explicit JetValidationv2(const std::string &name = "JetValidationv2");

  int Init(PHCompositeNode *topNode) override;
  int process_event(PHCompositeNode *topNode) override;
  // Clean up internals after each event.
  int ResetEvent(PHCompositeNode *topNode) override;
  int End(PHCompositeNode *topNode) override;

  void set_tree_filename(std::string_view file)
  {
    m_outtree_name = file;
  }

 private:
  int process_event_check(PHCompositeNode *topNode);
  int process_Calo(PHCompositeNode *topNode);
  int process_centrality(PHCompositeNode *topNode);
  int process_jets(PHCompositeNode *topNode);

  int m_event{0};

  std::string m_outtree_name{"tree.root"};

  struct HistConfig
  {
    unsigned int m_bins_zvtx{200};
    double m_zvtx_low{-50};
    double m_zvtx_high{50};

    unsigned int m_bins_cent{100};
    double m_cent_low{-0.5};
    double m_cent_high{99.5};
  };

  HistConfig m_hist_config;

  enum class EventType : std::uint8_t
  {
    ALL,
    ZVTX,
    ZVTX50,
    ZVTX10,
    MB_TRIG,
    MB,
    CENT,
    JET
  };

  enum class MinBiasType : std::uint8_t
  {
    BKG_HIGH,
    SIDE_HIT_LOW,
    ZDC_LOW,
    MBD_HIGH
  };

  std::vector<std::string> m_eventType{"All", "Has Z", "|z| < 50 cm", "|z| < 10 cm", "MB Trig", "MB", "Cent", "Has Jet"};
  std::vector<std::string> m_MinBias_Type{"MBD Background", "Hits < 2", "ZDC < 60 GeV", "MBD > 2100"};

  std::unique_ptr<TriggerAnalyzer> m_triggerAnalyzer;

  const int m_trig_12 = 12; // MBD N&S >= 2, vtx < 10 cm
  const int m_trig_14 = 14; // MBD N&S >= 2, vtx < 150 cm

  // Cuts
  struct EventCuts
  {
    double m_zvtx_max{10}; // cm
    double m_zvtx_max_v2{50}; // cm
    double m_cent_max{100};
  };

  EventCuts m_cuts;

  std::map<std::string, int> m_ctr;

  struct EventData
  {
    int event{0};
    double zvtx{9999};
    double centrality{9999};
    double emcal_energy{0};
    double ihcal_energy{0};
    double ohcal_energy{0};

    // jets
    std::vector<double> pt_r02;
    std::vector<double> pt_calib_r02;
    std::vector<double> e_r02;
    std::vector<double> phi_r02;
    std::vector<double> eta_r02;

    double max_pt_r02{0};
  };

  EventData m_data;

  TH1* hEvent{nullptr};
  TH1* hEventMinBias{nullptr};
  TH1* hVtxZ{nullptr};
  TH1* hVtxZ_MB{nullptr};
  TH1* hCentrality{nullptr};

  std::unique_ptr<TFile> m_output;
  TTree *m_tree{nullptr};

  std::string m_recoJetName_r02{"AntiKt_Tower_r02"};

  std::string m_recoJetName_calib_r02{"AntiKt_Tower_r02_calib"};

  double m_jet_pt_min_cut{10};     // GeV
  double m_jet_eta_max_cut_r02{0.9};  // 1.1-R
};
