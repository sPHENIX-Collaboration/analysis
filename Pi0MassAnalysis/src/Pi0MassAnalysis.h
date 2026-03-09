// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef PI0MASSANALYSIS_H
#define PI0MASSANALYSIS_H

#include <fun4all/SubsysReco.h>

#include <string>
#include "TFile.h"
#include "TH1F.h"
#include "TTree.h"

class PHCompositeNode;

class Pi0MassAnalysis : public SubsysReco
{
 public:
  Pi0MassAnalysis(const std::string &name = "Pi0MassAnalysis");

  ~Pi0MassAnalysis() override;

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

  void SetMinClusterEnergy(float e = 0.15) { minClusterEnergy = e; }
  void SetPhotonClusterProbability(float p = 0.1) { photonClusterProbability = p; }

 private:
  std::string _foutname;

  TFile *_f;
  TTree *_tree;

  TH1F *pi0MassHist;
  TH1F *pi0MassHistEtaDep[24];

  int counter;

  float minClusterEnergy;
  float photonClusterProbability;

  int nclus;

  float cluster_energy[10000];
  float cluster_eta[10000];
  float cluster_phi[10000];
  float cluster_prob[10000];
  float cluster_chi2[10000];

  int npi0;

  float pi0cand_pt[100000];
  float pi0cand_eta[100000];
  float pi0cand_phi[100000];
  float pi0cand_mass[100000];
};

#endif  // PI0MASSANALYSIS_H
