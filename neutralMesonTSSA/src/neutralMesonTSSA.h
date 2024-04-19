// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef NEUTRALMESONTSSA_H
#define NEUTRALMESONTSSA_H

#include <fun4all/SubsysReco.h>

#include <string>
#include <vector>

class PHCompositeNode;
class RawClusterContainer;
class PHG4TruthInfoContainer;
class GlobalVertex;
class PHG4VtxPoint;
class TFile;
class TH1F;

class neutralMesonTSSA : public SubsysReco
{
 public:

  neutralMesonTSSA(const std::string &name = "neutralMesonTSSA", bool isMC = false);

  ~neutralMesonTSSA() override;

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

  float get_min_clusterE();
  void set_min_clusterE(float Emin);
  float get_max_clusterChi2();
  void set_max_clusterChi2(float Chi2max);

  void FindGoodClusters();
  void FindMesons();
  void CountMesons();

 private:
  bool isMonteCarlo;
  RawClusterContainer* m_clusterContainer;
  PHG4TruthInfoContainer* m_truthInfo;
  GlobalVertex* gVtx;
  PHG4VtxPoint* mcVtx;
  TFile* histfile;
  TH1F* h_diphotonMass;
  TH1F* h_pi0Mass;
  TH1F* h_etaMass;

  float min_clusterE;
  float max_clusterChi2;
  std::vector<float> goodclusters_E;
  std::vector<float> goodclusters_Eta;
  std::vector<float> goodclusters_Phi;
  std::vector<float> goodclusters_Ecore;
  std::vector<float> goodclusters_Chi2;
  float min_mesonPt;
  float pi0MassMean;
  float pi0MassSigma;
  float min_pi0Mass;
  float max_pi0Mass;
  float etaMassMean;
  float etaMassSigma;
  float min_etaMass;
  float max_etaMass;
  std::vector<float> pi0_E;
  std::vector<float> pi0_M;
  std::vector<float> pi0_Eta;
  std::vector<float> pi0_Phi;
  std::vector<float> pi0_Pt;
  std::vector<float> pi0_xF;
  std::vector<float> eta_E;
  std::vector<float> eta_M;
  std::vector<float> eta_Eta;
  std::vector<float> eta_Phi;
  std::vector<float> eta_Pt;
  std::vector<float> eta_xF;
  // counters for events
  long int n_events_total;
  long int n_events_with_vertex;
  long int n_events_with_good_vertex;
  long int n_events_positiveCaloE;
  // counters for asymmetries
  long int N_pi0_up_left;
  long int N_pi0_up_right;
  long int N_pi0_down_left;
  long int N_pi0_down_right;
  long int N_eta_up_left;
  long int N_eta_up_right;
  long int N_eta_down_left;
  long int N_eta_down_right;
};

#endif // NEUTRALMESONTSSA_H
