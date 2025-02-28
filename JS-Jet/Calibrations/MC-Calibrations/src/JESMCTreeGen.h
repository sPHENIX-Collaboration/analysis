// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef JESMCTREEGEN_H
#define JESMCTREEGEN_H

#include <string>
#include <vector>
#include <limits>

#include <TFile.h>
#include <TTree.h> 

#include <fun4all/SubsysReco.h>

class PHCompositeNode;

class JESMCTreeGen : public SubsysReco
{
 public:
  JESMCTreeGen(const std::string &name = "JESMCTreeGen", const std::string &outfilename = "output.root");
  ~JESMCTreeGen() override = default;
  int Init(PHCompositeNode *topNode) override;
  int process_event(PHCompositeNode *topNode) override;
  int ResetEvent(PHCompositeNode *topNode) override;
  int End(PHCompositeNode *topNode) override;

  // ********** Setters ********** //
  void SetVerbosity(int verbo) {verbosity = verbo;}
  void DoZVertexReadout(bool operate) {doZVertexReadout = operate;}
  void DoTowerInfoReadout(bool operate) {doTowerInfoReadout = operate;}
    void DoTowerInfoReadout_RetowerEMCal(bool operate) {doTowerInfoReadout_RetowerEMCal = operate;}
  void DoClusterReadout(bool operate) {doClusterReadout = operate;}
    void DoClusterIsoReadout(bool operate) {doClusterIsoReadout = operate;}
    void DoClusterCNNReadout(bool operate) {doClusterCNNReadout = operate;}
  void DoRecoJetReadout(bool operate) {doRecoJetReadout = operate;}
    void DoRecoJet_radius(std::vector<float> radius_vector) {doRecoJet_radius.clear(); doRecoJet_radius = radius_vector;}
    void DoSeedJetRawReadout(bool operate) {doSeedJetRawReadout = operate;}
    void DoSeedJetSubReadout(bool operate) {doSeedJetSubReadout = operate;}
  void DoTruthJetReadout(bool operate) {doTruthJetReadout = operate;}
    void DoTruthJet_radius(std::vector<float> radius_vector) {doTruthJet_radius.clear(); doTruthJet_radius = radius_vector;}
  void DoTruthParticleReadout(bool operate) {doTruthParticleReadout = operate;}

  // ********** Functions ********** //
  void Initialize_z_vertex();
  void Initialize_calo_tower();
  void Initialize_calo_tower_RetowerEMCal();
  void Initialize_cluster();
  void Initialize_clusterIso();
  void Initialize_clusterCNN();
  void Initialize_jet(std::string jet_prefix , float radius);
  void Initialize_truthparticle();
  void Fill_z_vertex(PHCompositeNode *topNode);
  void Fill_calo_tower(PHCompositeNode *topNode, std::string calorimeter);
  void Fill_emcal_retower(PHCompositeNode *topNode);
  void Fill_cluster(PHCompositeNode *topNode);
  void Fill_recojet(PHCompositeNode *topNode, std::string jet_prefix, std::string node_prefix, float radius);
  void Fill_truthjet(PHCompositeNode *topNode, float radius);
  void Fill_truthparticle(PHCompositeNode *topNode);
  void Reset_cluster();
  void Reset_jet();
  void Reset_truthparticle();
 private:

  // ********** General variables ********** //
  TFile *file{nullptr};
  TTree *tree{nullptr};
  std::string foutname{"output.root"};
  int verbosity{0};
  int ievent{0};
  // Operation flags.
  bool doZVertexReadout{true};
  bool doTowerInfoReadout{false};
    bool doTowerInfoReadout_RetowerEMCal{false};
  bool doClusterReadout{false};
    bool doClusterIsoReadout{false};
    bool doClusterCNNReadout{false};
  bool doRecoJetReadout{true};
    std::vector<float> doRecoJet_radius{0.4};
    bool doSeedJetRawReadout{false};
    bool doSeedJetSubReadout{false};
  bool doTruthJetReadout{true};
    std::vector<float> doTruthJet_radius{0.4};
  bool doTruthParticleReadout{false};

  // ********** Constants ********** //
  // Tower information.
  static const int n_emcal_tower = 24576;
  static const int n_emcal_tower_etabin = 96;
  static const int n_emcal_tower_phibin = 256;
  static const int n_hcal_tower = 1536;
  static const int n_hcal_tower_etabin = 24;
  static const int n_hcal_tower_phibin = 64;

  // ********** Tree variables ********** //
  // Vertex information.
  float z_vertex{std::numeric_limits<float>::quiet_NaN()};
  // Calo tower information.
  float emcal_tower_e[n_emcal_tower_etabin][n_emcal_tower_phibin]{}, emcal_tower_time[n_emcal_tower_etabin][n_emcal_tower_phibin]{};
  float ihcal_tower_e[n_hcal_tower_etabin][n_hcal_tower_phibin]{}, ihcal_tower_time[n_hcal_tower_etabin][n_hcal_tower_phibin]{};
  float ohcal_tower_e[n_hcal_tower_etabin][n_hcal_tower_phibin]{}, ohcal_tower_time[n_hcal_tower_etabin][n_hcal_tower_phibin]{};
  float emcal_retower_e[n_hcal_tower_etabin][n_hcal_tower_phibin]{};
  int emcal_tower_status[n_emcal_tower_etabin][n_emcal_tower_phibin]{};
  int ihcal_tower_status[n_hcal_tower_etabin][n_hcal_tower_phibin]{};
  int ohcal_tower_status[n_hcal_tower_etabin][n_hcal_tower_phibin]{};
  // Cluster information.
  std::vector<float> cluster_e={}, cluster_pt={}, cluster_eta={}, cluster_phi={};
  std::vector<float> cluster_e_core={}, cluster_pt_core={}, cluster_eta_core={}, cluster_phi_core={};
  std::vector<float> cluster_probability={};
  std::vector<float> cluster_iso={};
  std::vector<float> cluster_e_cnn={}, cluster_probability_cnn={};
  // Jet general information.
  std::map<std::string, std::vector<float>> jet_e_map={};
  std::map<std::string, std::vector<float>> jet_et_map={};
  std::map<std::string, std::vector<float>> jet_pt_map={};
  std::map<std::string, std::vector<float>> jet_eta_map={};
  std::map<std::string, std::vector<float>> jet_phi_map={};
  // Jet tower information.
  std::map<std::string, std::vector<int>> jet_tower_ncomponent_map={};
  std::map<std::string, std::vector<float>> jet_tower_e_map={};
  std::map<std::string, std::vector<int>> jet_tower_ieta_map={};
  std::map<std::string, std::vector<int>> jet_tower_iphi_map={};
  // Jet particle information.
  std::map<std::string, std::vector<float>> jet_truthparticle_e_map={};
  std::map<std::string, std::vector<float>> jet_truthparticle_pt_map={};
  std::map<std::string, std::vector<float>> jet_truthparticle_eta_map={};
  std::map<std::string, std::vector<float>> jet_truthparticle_phi_map={};
  std::map<std::string, std::vector<int>> jet_truthparticle_pid_map={};
  // Truth particle information.
  std::vector<float> truthparticle_e={}, truthparticle_pt={}, truthparticle_eta={}, truthparticle_phi={};
  std::vector<int> truthparticle_pid={};
};

#endif