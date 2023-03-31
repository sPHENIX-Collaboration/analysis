// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef PI0CLUSTERANA_H
#define PI0CLUSTERANA_H

#include <fun4all/SubsysReco.h>

#include <string>
#include <vector>

class PHCompositeNode;
class TTree;
class PHG4Particle;
class CaloEvalStack;
class TFile;
class Fun4AllHistoManager;

class pi0ClusterAna : public SubsysReco
{
 public:

  pi0ClusterAna(const std::string &name, const std::string &outName);

  ~pi0ClusterAna() override;

  /** Called during initialization.
      Typically this is where you can book histograms, and e.g.
      register them to Fun4AllServer (so they can be output to file
      using Fun4AllServer::dumpHistos() method).
   */
  int Init(PHCompositeNode *topNode) override;

  /** Called for first event when run number is known.
      Typically this is where you may want to fetch data from
      database, because you know the run number. A place
      to book histograms which have to know the run number.
   */
  int InitRun(PHCompositeNode *topNode) override;

  /** Called for each event.
      This is where you do the real work.
   */
  int process_event(PHCompositeNode *topNode) override;

  /// Clean up internals after each event.
  int ResetEvent(PHCompositeNode *topNode) override;

  /// Called at the end of each run.
  int EndRun(const int runnumber) override;

  /// Called at the end of all processing.
  int End(PHCompositeNode *topNode) override;

  /// Reset
  int Reset(PHCompositeNode * /*topNode*/) override;

  void Print(const std::string &what = "ALL") const override;

 private:

  float getEta(PHG4Particle *particle);
    
  TTree *clusters_Towers;
  TTree *truth_photon;
  TTree *truth_pi0;
  
  //CaloEvalStack *caloevalstack;
  
  //stuff for towers and clusters
  std::vector<float> m_eta_center;
  std::vector<float> m_phi_center;
  std::vector<float> m_tower_energy;
  std::vector<float> m_cluster_eta;
  std::vector<float> m_cluster_phi;
  std::vector<float> m_cluster_e;
  std::vector<float> m_cluster_chi2;
  std::vector<float> m_cluster_prob;
  std::vector<float> m_cluster_nTowers;
  
  //stuff for truth photons
  std::vector<float> m_asym;
  std::vector<float> m_deltaR;
  std::vector<float> m_lead_E;
  std::vector<float> m_sublead_E;
  std::vector<float> m_lead_phi;
  std::vector<float> m_lead_eta;
  std::vector<float> m_sublead_phi;
  std::vector<float> m_sublead_eta;

  //stuff for truth pi0's
  std::vector<float> m_pi0_E;
  std::vector<float> m_pi0_eta;
  std::vector<float> m_pi0_phi;
  
  TFile *out;
  Fun4AllHistoManager *hm = nullptr;
  std::string Outfile;

  
};

#endif // PI0CLUSTERANA_H
