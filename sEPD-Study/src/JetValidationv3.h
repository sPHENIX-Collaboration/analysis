// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#pragma once

#include "JetUtils.h"
#include "geometry_constants.h"

// -- sPHENIX
#include <fun4all/SubsysReco.h>

// -- c++
#include <array>
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

class JetValidationv3 : public SubsysReco
{
 public:
  explicit JetValidationv3(const std::string &name = "JetValidationv3");

  int Init(PHCompositeNode *topNode) override;
  int InitRun(PHCompositeNode *topNode) override;
  int process_event(PHCompositeNode *topNode) override;
  // Clean up internals after each event.
  int ResetEvent(PHCompositeNode *topNode) override;
  int End(PHCompositeNode *topNode) override;

  void set_do_unsub(bool b = true) { m_do_unsub = b; }
  void set_do_iter(bool b = true) { m_do_iter = b; }
  void set_do_mult(bool b = true) { m_do_mult = b; }
  void set_do_detailed(bool b = true) { m_do_detailed = b; }
  void set_jet_pt_min(double pt_min) { m_jet_pt_min_cut = pt_min; }

  bool get_do_unsub() const { return m_do_unsub; }
  bool get_do_iter() const { return m_do_iter; }
  bool get_do_mult() const { return m_do_mult; }
  bool get_do_detailed() const { return m_do_detailed; }
  double get_jet_pt_min() const { return m_jet_pt_min_cut; }

 private:
  int process_UE(PHCompositeNode *topNode);
  int process_jets(PHCompositeNode *topNode);

  struct JetData
  {
    std::vector<double> pt;        // NOLINT(misc-non-private-member-variables-in-classes)
    std::vector<double> pt_calib;  // NOLINT(misc-non-private-member-variables-in-classes)
    std::vector<double> e;         // NOLINT(misc-non-private-member-variables-in-classes)
    std::vector<double> phi;       // NOLINT(misc-non-private-member-variables-in-classes)
    std::vector<double> eta;       // NOLINT(misc-non-private-member-variables-in-classes)
    double max_pt{0};              // NOLINT(misc-non-private-member-variables-in-classes)

    // Detailed tower constituent info (nested vectors: 1 vector per jet in the event)
    std::vector<std::vector<int>> emcal_tower_index;        // NOLINT(misc-non-private-member-variables-in-classes)
    std::vector<std::vector<double>> emcal_tower_energy;   // NOLINT(misc-non-private-member-variables-in-classes)
    std::vector<std::vector<double>> emcal_tower_pt;       // NOLINT(misc-non-private-member-variables-in-classes)

    std::vector<std::vector<int>> ihcal_tower_index;        // NOLINT(misc-non-private-member-variables-in-classes)
    std::vector<std::vector<double>> ihcal_tower_energy;   // NOLINT(misc-non-private-member-variables-in-classes)
    std::vector<std::vector<double>> ihcal_tower_pt;       // NOLINT(misc-non-private-member-variables-in-classes)

    std::vector<std::vector<int>> ohcal_tower_index;        // NOLINT(misc-non-private-member-variables-in-classes)
    std::vector<std::vector<double>> ohcal_tower_energy;   // NOLINT(misc-non-private-member-variables-in-classes)
    std::vector<std::vector<double>> ohcal_tower_pt;       // NOLINT(misc-non-private-member-variables-in-classes)

    void clear()
    {
      pt.clear();
      pt_calib.clear();
      e.clear();
      phi.clear();
      eta.clear();
      max_pt = 0;

      emcal_tower_index.clear();
      emcal_tower_energy.clear();
      emcal_tower_pt.clear();

      ihcal_tower_index.clear();
      ihcal_tower_energy.clear();
      ihcal_tower_pt.clear();

      ohcal_tower_index.clear();
      ohcal_tower_energy.clear();
      ohcal_tower_pt.clear();
    }
  };

  struct EventData
  {
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
  };

  EventData m_data;

  std::string m_recoJetName_iter_r02{"AntiKt_Tower_r02_Sub1"};
  std::string m_recoJetName_mult_r02{"AntiKt_Tower_r02_MultSub1"};
  std::string m_recoJetName_iter_calib_r02{"AntiKt_Tower_r02_Sub1_calib"};
  std::string m_recoJetName_mult_calib_r02{"AntiKt_Tower_r02_MultSub1_calib"};

  std::string m_recoJetName_iter_r03{"AntiKt_Tower_r03_Sub1"};
  std::string m_recoJetName_mult_r03{"AntiKt_Tower_r03_MultSub1"};
  std::string m_recoJetName_iter_calib_r03{"AntiKt_Tower_r03_Sub1_calib"};
  std::string m_recoJetName_mult_calib_r03{"AntiKt_Tower_r03_MultSub1_calib"};

  std::string m_recoJetName_unsub_r02{"AntiKt_TowerInfo_r02"};
  std::string m_recoJetName_unsub_calib_r02{"AntiKt_TowerInfo_r02_calib"};
  std::string m_recoJetName_unsub_r03{"AntiKt_TowerInfo_r03"};
  std::string m_recoJetName_unsub_calib_r03{"AntiKt_TowerInfo_r03_calib"};

  double m_jet_pt_min_cut{10};     // GeV

  bool m_do_unsub{true};
  bool m_do_iter{true};
  bool m_do_mult{true};
  bool m_do_detailed{false};

  static constexpr size_t N_HCAL_TOWERS = CaloGeometry::HCAL_ETA_BINS * CaloGeometry::HCAL_PHI_BINS;

  struct TowerGeomInfo
  {
    double eta = 0.0;
    double phi = 0.0;
    double z0 = 0.0;
    bool is_valid = false;
  };

  using TowerGeomArray = std::array<TowerGeomInfo, N_HCAL_TOWERS>;

  double m_r_cemc = JetUtils::radius_EM;
  double m_r_hcalin = JetUtils::radius_IH;
  double m_r_hcalout = JetUtils::radius_OH;

  TowerGeomArray m_geom_cemc{};
  TowerGeomArray m_geom_hcalin{};
  TowerGeomArray m_geom_hcalout{};
};
