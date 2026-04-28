#pragma once

// -- sPHENIX
#include <fun4all/SubsysReco.h>

// -- c++
#include <filesystem>
#include <map>
#include <string>
#include <vector>

// -- ROOT
#include <TH1.h>
#include <TProfile.h>
#include <TH2.h>

class PHCompositeNode;
class TFile;
class TTree;
class JetContainer;
class TowerBackground;

class sEPD_DataMC_Validation : public SubsysReco
{
 public:
  explicit sEPD_DataMC_Validation(const std::string &name = "sEPD_DataMC_Validation");

  int Init(PHCompositeNode *topNode) override;
  int process_event(PHCompositeNode *topNode) override;
  // Clean up internals after each event.
  int ResetEvent(PHCompositeNode *topNode) override;
  int End(PHCompositeNode *topNode) override;

  void set_cent_max_threshold(double cent_max_threshold)
  {
    m_cent_max_threshold = cent_max_threshold;
  }

  void set_jet_pt_min(double pt_min)
  {
    m_jet_pt_min = pt_min;
  }

  void set_tree_filename(const std::string &file)
  {
    m_outtree_name = file;
  }

 private:
  int process_event_check(PHCompositeNode *topNode);
  int process_sEPD(PHCompositeNode *topNode);
  int process_EventPlane(PHCompositeNode *topNode);
  int process_jets(PHCompositeNode *topNode);
  int process_centrality(PHCompositeNode *topNode);
  int process_calo(PHCompositeNode *topNode);

  void setup_tree();
  void fill_jets(JetContainer* jets, double max_eta, int &nJets, double &max_pt, std::vector<double> &pt_vec, std::vector<double> &e_vec, std::vector<double> &phi_vec, std::vector<double> &eta_vec) const;

  std::vector<float> computeUEAverages(TowerBackground *towerBkg);

  std::string m_outtree_name{"tree.root"};

  std::unique_ptr<TFile> m_output;
  TTree* m_tree{nullptr};

  struct EventData
  {
    int event{0};
    double zvtx{9999};
    double centrality{9999};
    float calo_v2{-9999};
    bool is_flow_failure{false};

    float ue_emcal_avg{0};
    float ue_ihcal_avg{0};
    float ue_ohcal_avg{0};

    // Calo
    double emcal_energy{0};
    double ihcal_energy{0};
    double ohcal_energy{0};

    // Q Vector Info
    double qsx_data{0};
    double qsy_data{0};
    double qnx_data{0};
    double qny_data{0};

    double qsx_data_mc{0};
    double qsy_data_mc{0};
    double qnx_data_mc{0};
    double qny_data_mc{0};

    double sepdpsi2_data{-9999};
    double sepdpsi2_data_mc{-9999};

    double max_pt_r02{0};
    double max_pt_r03{0};

    double max_truthPt_r02{0};
    double max_truthPt_r03{0};

    int nJets_r02{0};
    int nJets_truth_r02{0};

    int nJets_r03{0};
    int nJets_truth_r03{0};

    std::vector<double> pt_r02;
    std::vector<double> e_r02;
    std::vector<double> phi_r02;
    std::vector<double> eta_r02;

    std::vector<double> truthPt_r02;
    std::vector<double> truthE_r02;
    std::vector<double> truthPhi_r02;
    std::vector<double> truthEta_r02;

    std::vector<double> pt_r03;
    std::vector<double> e_r03;
    std::vector<double> phi_r03;
    std::vector<double> eta_r03;

    std::vector<double> truthPt_r03;
    std::vector<double> truthE_r03;
    std::vector<double> truthPhi_r03;
    std::vector<double> truthEta_r03;
  };

  EventData m_data;

  static constexpr int m_sepd_channels = 744;

  unsigned int m_bins_cent{80};
  double m_cent_low{-0.5};
  double m_cent_high{79.5};

  double m_cent_max_threshold{60};

  double m_jet_pt_min{10};   // GeV
  double m_jet_eta_max_r02{0.9}; // 1.1-R
  double m_jet_eta_max_r03{0.8}; // 1.1-R
  double m_zvtx_max{10};     // cm
  double m_zvtx_max_v2{50};  // cm

  // Event Vars
  std::pair<double, double> m_Q_data_S_2;
  std::pair<double, double> m_Q_data_N_2;
  std::pair<double, double> m_Q_data_mc_S_2;
  std::pair<double, double> m_Q_data_mc_N_2;
  std::map<std::string, int> m_ctr;

  std::string m_jet_truth_r02{"AntiKt_Truth_r02"};
  std::string m_jet_truth_r03{"AntiKt_Truth_r03"};

  std::string m_jet_reco_r02{"AntiKt_Tower_r02_Sub1"};
  std::string m_jet_reco_r03{"AntiKt_Tower_r03_Sub1"};

  enum class EventType : std::uint8_t
  {
    ALL,
    ZVTX50,
    ZVTX10,
    ZVTX10_MB,
    ZVTX10_MB_CENT,
    ZVTX10_MB_CENT_JET
  };

  std::vector<std::string> m_eventType{"All", "|z| < 50 cm", "|z| < 10 cm", "MB", "Cent", "Jet"};

  // Event Selection Flags
  bool m_pass_MB{false};
  bool m_pass_Zvtx{false};
  bool m_pass_Centrality{false};

  // Hists
  TH1* hZVertex{nullptr};
  TH2* h2ZVertexTruthvsData{nullptr};
  TH1* hCentrality{nullptr};
  TH1* hCentralityZ50{nullptr};
  TH1* hCentralityZOuter{nullptr};
  TH2* h2ZVertexCentrality{nullptr};
  TH1* hEvent{nullptr};

  TH2* h2SEPD_totalcharge_centrality{nullptr};

  TH1* hSEPD_Charge_Data{nullptr};
  TH1* hSEPD_Charge_MC{nullptr};
  TH1* hSEPD_Charge_DataMC{nullptr};

  TH1* hSEPD_BadMasked_Charge_Data{nullptr};
  TH1* hSEPD_BadMasked_Charge_MC{nullptr};
  TH1* hSEPD_BadMasked_Charge_DataMC{nullptr};

  TH2* h2SEPD_South_Charge_Data{nullptr};
  TH2* h2SEPD_South_Charge_MC{nullptr};
  TH2* h2SEPD_South_Charge_DataMC{nullptr};

  TH2* h2SEPD_North_Charge_Data{nullptr};
  TH2* h2SEPD_North_Charge_MC{nullptr};
  TH2* h2SEPD_North_Charge_DataMC{nullptr};

  TH2* h2SEPD_BadMasked_South_Charge_Data{nullptr};
  TH2* h2SEPD_BadMasked_South_Charge_MC{nullptr};
  TH2* h2SEPD_BadMasked_South_Charge_DataMC{nullptr};

  TH2* h2SEPD_BadMasked_North_Charge_Data{nullptr};
  TH2* h2SEPD_BadMasked_North_Charge_MC{nullptr};
  TH2* h2SEPD_BadMasked_North_Charge_DataMC{nullptr};

  TProfile* pSEPD_Charge_Data{nullptr};
  TProfile* pSEPD_Charge_MC{nullptr};
  TProfile* pSEPD_Charge_DataMC{nullptr};

  TProfile* pSEPD_BadMasked_Charge_Data{nullptr};
  TProfile* pSEPD_BadMasked_Charge_MC{nullptr};
  TProfile* pSEPD_BadMasked_Charge_DataMC{nullptr};

  TH1* hJetPt{nullptr};
  TH2* h2JetPtCentrality{nullptr};
  TH2* h2JetPhiEta{nullptr};
  TH2* h2JetEtaVtxZ{nullptr};

  TH2* h2SEPD_Psi2_raw_data_S{nullptr};
  TH2* h2SEPD_Psi2_raw_data_N{nullptr};

  TH2* h2SEPD_Psi2_raw_data_mc_S{nullptr};
  TH2* h2SEPD_Psi2_raw_data_mc_N{nullptr};
  TH2* h2SEPD_Psi2_raw_data_mc_NS{nullptr};

  TH2* h2SEPD_Psi2_data_S{nullptr};
  TH2* h2SEPD_Psi2_data_N{nullptr};
  TH2* h2SEPD_Psi2_data_NS{nullptr};

  TH2* h2SEPD_Psi2_data_mc_S{nullptr};
  TH2* h2SEPD_Psi2_data_mc_N{nullptr};
  TH2* h2SEPD_Psi2_data_mc_NS{nullptr};

  TH2* h2SEPD_Psi2_data_mc_NS_all{nullptr};

  TProfile* hScalarProduct_data{nullptr};
  TProfile* hScalarProduct_data_mc{nullptr};
  TProfile* hScalarProduct_data_mc_anti{nullptr};
  TProfile* hRefFlow_data{nullptr};
  TProfile* hRefFlow_data_mc{nullptr};

  TH1* hRefFlow_data_min{nullptr};
  TH1* hRefFlow_data_max{nullptr};

  TH1* hScalarProduct_data_min{nullptr};
  TH1* hScalarProduct_data_max{nullptr};

  TH2* h2ScalarProduct_data{nullptr};
  TH2* h2ScalarProduct_data_mc{nullptr};

  std::vector<TH2*> h2ScalarProduct_data_ptbin;
  std::vector<TH2*> h2ScalarProduct_data_mc_ptbin;

  TH2* h2RefFlow_data{nullptr};
  TH2* h2RefFlow_data_mc{nullptr};
};
