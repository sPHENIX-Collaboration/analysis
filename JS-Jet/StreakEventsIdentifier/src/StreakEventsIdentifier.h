#ifndef STREAK_EVENTS_IDENTIFIER_H
#define STREAK_EVENTS_IDENTIFIER_H


// sPHENIX core
#include <fun4all/SubsysReco.h>
#include <phool/PHCompositeNode.h>

// Cluster and tower types
#include <calobase/RawCluster.h>
#include <calobase/RawClusterContainer.h>
#include <calobase/RawTowerGeomContainer.h>
#include <calobase/TowerInfoContainer.h>

// Jet types
#include <jetbase/JetContainer.h>

// ROOT
#include <TH1F.h>
#include <TH2F.h>
#include <TFile.h>

#include <string>
#include <map>
#include <vector>
#include <utility>
#include <cmath>
#include <memory>

class StreakEventsIdentifier : public SubsysReco 
{
public:
  explicit StreakEventsIdentifier(const std::string& name = "StreakID",
                                          const std::string& outprefix = "streak_fromDST");
  virtual ~StreakEventsIdentifier() override;

  // SubsysReco
  int Init(PHCompositeNode* topNode) override;
  int InitRun(PHCompositeNode* topNode) override;
  int process_event(PHCompositeNode* topNode) override;
  int End(PHCompositeNode* topNode) override;

  // Config
  void SetOutputPrefix(const std::string& p) { m_outprefix = p; }
  void SetPngDir(const std::string& d) { m_png_dir = d; }
  void SetVertexCut(float cut) { m_vertex_cut = cut; }
  void SetEnergyThreshold(float etMin) { m_et_min = etMin; }
  void SetWetaCut(float wetaMin) { m_weta_min = wetaMin; }
  void SetTimingCuts(float absTimeNs, float rmsMinNs) { m_abs_time_cut_ns = absTimeNs; m_time_spread_min = rmsMinNs; }
  void SetTimeWeightEthresh(float e) { m_time_weight_Ethresh = e; }

  
  void SetJetContainer(const std::string& name) { m_jet_container_hint = name; }

    // to control timing requirement
  void SetRequireValidTiming(bool require) { m_require_valid_timing = require; }


  //Adding MBD timing cuts
  void SetExcludeStreaks(bool exclude) { m_exclude_streaks = exclude; }
  void SetMbdTimingCuts(float minNs, float maxNs) { 
    m_mbd_time_min = minNs; 
    m_mbd_time_max = maxNs; 
  }

  //trigger bits to use for event selection
  void set_using_trigger_bits(const std::vector<int>& trigger_bits) { 
    m_using_trigger_bits = trigger_bits; 
  }
  
  // Alternative: add single trigger bit
  void add_trigger_bit(int bit) { 
    m_using_trigger_bits.push_back(bit); 
  }

private:

std::pair<float, float> extractCaloTiming(TowerInfoContainer* towers, 
                                            float energy_threshold = 0.1) const;

  // Total calorimeter energies
  float m_totalEMCal_energy = 0.f;
  float m_totalIHCal_energy = 0.f;
  float m_totalOHCal_energy = 0.f;

  static inline float safe_div(float n, float d) { return (std::fabs(d) > 1e-9f) ? n/d : 0.f; }
  static inline float wrap_dphi(float a, float b) {
    float d = a - b; while (d > M_PI) d -= 2.f*M_PI; while (d <= -M_PI) d += 2.f*M_PI; return std::fabs(d);
  }

  
static void shift_tower_index(int& ieta, int& iphi, int neta, int nphi)
{
  if (ieta < 0) { ieta += neta; }
  if (ieta >= neta) { ieta -= neta; }

  if (iphi < 0) { iphi += nphi; }
  if (iphi >= nphi) { iphi -= nphi; }
}

// Trigger selection
  std::vector<int> m_using_trigger_bits;           
  
  // Trigger data storage
  bool m_scaledtrigger[64] = {false};
  bool m_livetrigger[64] = {false};
  int m_nscaledtrigger[64] = {0};
  int m_nlivetrigger[64] = {0};
  float m_trigger_prescale[64] = {-1.0};
  
  // Scaler tracking
  bool m_initilized = false;
  long long m_initscaler[64][3] = {{0}};
  long long m_currentscaler[64][3] = {{0}};
  long long m_currentscaler_raw[64] = {0};
  long long m_currentscaler_live[64] = {0};
  long long m_currentscaler_scaled[64] = {0};
  
  // Event tracking
  int m_eventnumber = 0;

  // Streaks + MBD timing exclusion
  bool m_exclude_streaks = false;
  float m_mbd_time_min = -10.0f;  // ns
  float m_mbd_time_max = 10.0f;   // ns
  int m_excluded_count = 0;

  // Node getters 
  RawClusterContainer* getCemcClusterContainer(PHCompositeNode* topNode) const;
  TowerInfoContainer*  getCemcCalibTowers(PHCompositeNode* topNode) const;   
  TowerInfoContainer*  getCemcRawTowers(PHCompositeNode* topNode) const;     
  RawTowerGeomContainer* getCemcGeom(PHCompositeNode* topNode) const;        
  JetContainer*        getJetContainer(PHCompositeNode* topNode) const;      

  
  // IHCal
  TowerInfoContainer*    getIhcalCalibTowers(PHCompositeNode* topNode) const;
  TowerInfoContainer*    getIhcalRawTowers  (PHCompositeNode* topNode) const;
  RawTowerGeomContainer* getIhcalGeom       (PHCompositeNode* topNode) const;

  // OHCal
  TowerInfoContainer*    getOhcalCalibTowers(PHCompositeNode* topNode) const;
  TowerInfoContainer*    getOhcalRawTowers  (PHCompositeNode* topNode) const;
  RawTowerGeomContainer* getOhcalGeom       (PHCompositeNode* topNode) const;


  float getVertexZ(PHCompositeNode* topNode) const;

  // Per-cluster calculations (7x7 around centroid)
  struct ClusterWindows {
  float E77[7][7]{};   // calibrated energy (thresholded)
  float T77[7][7]{};   // time (ns)
  int   Own77[7][7]{}; // ownership mask

  inline float E(int i, int j) const { return E77[i][j]; }
  inline float T(int i, int j) const { return T77[i][j]; }
  inline int   Own(int i, int j) const { return Own77[i][j]; }
  };


  bool fillClusterWindows(const RawCluster* cl,
                          TowerInfoContainer* emcCalib,
                          TowerInfoContainer* emcRaw,
                          ClusterWindows& win,
                          int& maxieta, int& maxiphi,
                          float& cog_eta, float& cog_phi) const;

  void computeWidths(const ClusterWindows& w, float cog_eta, float cog_phi,
                   float& weta, float& weta_cogx, 
                   float& wphi, float& wphi_cogx) const;

  void computeTimeMoments(const ClusterWindows& win,
                          float& t_avg, float& t_rms) const;

  // Book / save histos
  void bookHistos();
  void savePNGs() const;

private:
  std::string m_outprefix;
  std::string m_png_dir;  

    //Timing requirement
  bool m_require_valid_timing = true;  //  require timing
  bool has_valid_timing = true;
  
  // thresholds 
  float m_vertex_cut = 200.0f; // z-verte cut in cm
  float m_et_min = 10.f;       // GeV
  float m_weta_min = 0.5f;      // Weta cut
  float m_abs_time_cut_ns = 10.0f; //10
  float m_time_spread_min = 5.0f; //5
  float m_min_tower_E_for_shapes = 0.07f; // GeV
  float m_time_weight_Ethresh = 0.50f;    // GeV

  std::string m_jet_container_hint;

  // bookkeeping
  int   m_runnumber = 0;
  long  m_evt_processed = 0;
  long  m_streak_count = 0;
  std::vector<std::pair<int,int>> m_streakEvents; // (run, evt)
  std::map<int,int> m_run_total, m_run_streak;

  std::unique_ptr<TFile> m_out;

  // Histograms 
  TH2F *h_eta_phi_all = nullptr, *h_eta_phi_streak = nullptr;
  TH2F *h_phi_Et_all  = nullptr, *h_phi_Et_streak  = nullptr;
  TH1F *h_weta_all    = nullptr, *h_weta_streak    = nullptr;
  TH1F *h_weta_all_x    = nullptr, *h_weta_streak_x    = nullptr;
  TH1F *h_cluster_Et_all = nullptr, *h_cluster_Et_streak = nullptr;
  TH1F *h_asymmetry = nullptr, *h_jet_dphi = nullptr, *h_jet_asym = nullptr;
  TH2F *h_dphi_vs_pt_ratio = nullptr, *h_jet_pt_vs_phi = nullptr;
  TH1F *h_cluster_time_all = nullptr, *h_cluster_time_streak = nullptr;
  TH1F *h_time_spread_all = nullptr, *h_time_spread_streak = nullptr;

  TH1F* h_wphi_all{nullptr};
  TH1F* h_wphi_streak{nullptr};

  
};

#endif
// -----------------------------------------------------------
