// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef SINGLEPHOTONCLUSTERANA_H
#define SINGLEPHOTONCLUSTERANA_H

#include <fun4all/SubsysReco.h>

#include <string>
#include <vector>

class PHCompositeNode;
class TTree;
class PHG4Particle;
class CaloEvalStack;
class TFile;
class Fun4AllHistoManager;

class singlePhotonClusterAna : public SubsysReco
{
 public:

  singlePhotonClusterAna(const std::string &name, const std::string &outName);

  ~singlePhotonClusterAna() override;

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
  TTree *conversion;
  
  //CaloEvalStack *caloevalstack;
  
  //stuff for towers and clusters
  std::vector<float> m_eta_center;
  std::vector<float> m_phi_center;
  std::vector<float> m_tower_energy;
  std::vector<float> m_cluster_eta;
  std::vector<float> m_cluster_phi;
  std::vector<float> m_cluster_e;
  std::vector<float> m_cluster_ecore;
  std::vector<float> m_cluster_chi2;
  std::vector<float> m_cluster_prob;
  std::vector<float> m_cluster_nTowers;
  
  //stuff for truth photons
  std::vector<float> m_photon_E;
  std::vector<float> m_photon_eta;
  std::vector<float> m_photon_phi;

  //stuff for conversions
  std::vector<float> m_electron_E;
  std::vector<float> m_electron_eta;
  std::vector<float> m_electron_phi;
  std::vector<float> m_positron_E;
  std::vector<float> m_positron_eta;
  std::vector<float> m_positron_phi;
  std::vector<float> m_vtx_x;
  std::vector<float> m_vtx_y;
  std::vector<float> m_vtx_z;
  std::vector<float> m_vtx_t;
  std::vector<float> m_vtx_r;
  std::vector<bool> m_isConversionEvent;
  
  TFile *out;
  Fun4AllHistoManager *hm = nullptr;
  std::string Outfile;

  
};

#endif // SINGLEPHOTONCLUSTERANA_H
