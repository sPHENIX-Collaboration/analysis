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

 private:
  int process_event_check(PHCompositeNode *topNode);
  int process_sEPD(PHCompositeNode *topNode);
  int process_EventPlane(PHCompositeNode *topNode);
  int process_jets(PHCompositeNode *topNode);
  int process_centrality(PHCompositeNode *topNode);

  double GetPhysicsEta(double det_eta, double vtx_z);

  static constexpr int m_sepd_channels = 744;

  unsigned int m_bins_cent{80};
  double m_cent_low{-0.5};
  double m_cent_high{79.5};

  double m_cent_max_threshold{80};
  double m_cent_max_threshold_ana{60};

  double m_jet_pt_min{10};   // GeV
  double m_jet_eta_max{0.9}; // 1.1-R
  double m_zvtx_max{10};     // cm

  // Event Vars
  double m_cent{0};
  double m_zvtx{0};
  int m_globalEvent{0};
  std::pair<double, double> m_Q_data_S_2;
  std::pair<double, double> m_Q_data_N_2;
  std::pair<double, double> m_Q_data_mc_S_2;
  std::pair<double, double> m_Q_data_mc_N_2;
  std::map<std::string, int> m_ctr;

  std::string m_recoJetName{"AntiKt_Truth_r02"};

  enum class EventType : std::uint8_t
  {
    ALL,
    ZVTX10,
    ZVTX10_MB,
    ZVTX10_MB_CENT,
    ZVTX10_MB_CENT_JET
  };

  std::vector<std::string> m_eventType{"All", "|z| < 10 cm", "MB", "Cent", "Jet"};

  // Hists
  TH1* hZVertex{nullptr};
  TH1* hCentrality{nullptr};
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
  TH2* h2JetEtaPhysVtxZ{nullptr};
  TH2* h2JetEtaDiffVtxZ{nullptr};
  TH2* h2JetEtaSignFlip{nullptr};

  TH2* h2SEPD_Psi2_data_S{nullptr};
  TH2* h2SEPD_Psi2_data_N{nullptr};
  TH2* h2SEPD_Psi2_data_NS{nullptr};

  TH2* h2SEPD_Psi2_data_mc_S{nullptr};
  TH2* h2SEPD_Psi2_data_mc_N{nullptr};
  TH2* h2SEPD_Psi2_data_mc_NS{nullptr};

  TProfile* hScalarProduct_data{nullptr};
  TProfile* hScalarProduct_data_corr{nullptr};
  TProfile* hScalarProduct_data_mc{nullptr};
  TProfile* hScalarProduct_data_mc_corr{nullptr};
  TProfile* hRefFlow_data{nullptr};
  TProfile* hRefFlow_data_mc{nullptr};
};
