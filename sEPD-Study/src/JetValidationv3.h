// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#pragma once

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

class JetValidationv3 : public SubsysReco
{
 public:
  explicit JetValidationv3(const std::string &name = "JetValidationv3");

  int Init(PHCompositeNode *topNode) override;
  int process_event(PHCompositeNode *topNode) override;
  // Clean up internals after each event.
  int ResetEvent(PHCompositeNode *topNode) override;
  int End(PHCompositeNode *topNode) override;

  void set_do_unsub(bool b = true) { m_do_unsub = b; }
  void set_do_iter(bool b = true) { m_do_iter = b; }
  void set_do_mult(bool b = true) { m_do_mult = b; }

  bool get_do_unsub() const { return m_do_unsub; }
  bool get_do_iter() const { return m_do_iter; }
  bool get_do_mult() const { return m_do_mult; }

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

    void clear()
    {
      pt.clear();
      pt_calib.clear();
      e.clear();
      phi.clear();
      eta.clear();
      max_pt = 0;
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
  double m_jet_eta_max_cut_r02{0.9};  // 1.1-R
  double m_jet_eta_max_cut_r03{0.8};  // 1.1-R

  bool m_do_unsub{true};
  bool m_do_iter{true};
  bool m_do_mult{true};
};
