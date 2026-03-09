// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef CALOTREEGEN_H
#define CALOTREEGEN_H

#include <string>
#include <vector>
#include <limits>

#include <TFile.h>
#include <TTree.h> 

#include <fun4all/SubsysReco.h>
#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfoContainerv1.h>
#include <calobase/TowerInfoContainerv2.h>
#include <calobase/TowerInfoContainerv3.h>
#include <calobase/TowerInfoContainerv4.h>
#include <jetbase/Jetv1.h>
#include <jetbase/Jetv2.h>

class PHCompositeNode;

class caloTreeGen : public SubsysReco
{
 public:
  caloTreeGen(const std::string &name = "caloTreeGen", const std::string &outfilename = "output.root");
  ~caloTreeGen() override = default;
  int Init(PHCompositeNode *topNode) override;
  int process_event(PHCompositeNode *topNode) override;
  int ResetEvent(PHCompositeNode *topNode) override;
  int End(PHCompositeNode *topNode) override;

  // ********** Setters ********** //
  void SetVerbosity(int verbo) {verbosity = verbo;}
  void DoRecoJet_radius(std::vector<float> radius_vector) {doRecoJet_radius.clear(); doRecoJet_radius = radius_vector;}

  // ********** Functions ********** //
  void Initialize_z_vertex();
  void Initialize_jet(std::string jet_prefix , float radius);
  void Initialize_trigger_vector();
  void Fill_z_vertex(PHCompositeNode *topNode);
  void Fill_recojet(PHCompositeNode *topNode, std::string jet_prefix, std::string node_prefix, float radius);
  void Fill_trigger_vector(PHCompositeNode *topNode);
  void Reset_jet();

 private:
  // ********** General variables ********** //
  TFile *file{nullptr};
  TTree *tree{nullptr};
  std::string foutname{"output.root"};
  int verbosity{0};
  int ievent{0};
  // Operation flags.
  std::vector<float> doRecoJet_radius{0.4};

  // ********** Constants ********** //
  // Tower information.
  static const int n_trigger_vec = 64;

  // ********** Tree variables ********** //
  // Vertex information.
  float z_vertex{std::numeric_limits<float>::quiet_NaN()};
  // Jet general information.
  std::map<std::string, std::vector<float>> jet_e_map={};
  std::map<std::string, std::vector<float>> jet_et_map={};
  std::map<std::string, std::vector<float>> jet_pt_map={};
  std::map<std::string, std::vector<float>> jet_eta_map={};
  std::map<std::string, std::vector<float>> jet_phi_map={};
  // Jet specific information.
  std::map<std::string, std::vector<float>> jet_calo_e_map={};
  // Trigger vector information.
  uint64_t gl1_trigger_scalar_live[n_trigger_vec];
  uint64_t gl1_trigger_scalar_scaled[n_trigger_vec];
  int gl1_trigger_vector_live[n_trigger_vec];
  int gl1_trigger_vector_scaled[n_trigger_vec];
};

#endif