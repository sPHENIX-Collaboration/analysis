// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef NEUTRALMESONTSSA_H
#define NEUTRALMESONTSSA_H

#include <fun4all/SubsysReco.h>

#include <string>
#include <vector>
#include <utility>

class PHCompositeNode;
class RunHeader;
class Gl1Packet;
class RawClusterContainer;
class PHG4TruthInfoContainer;
class GlobalVertex;
class PHG4VtxPoint;
class TFile;
class TH1;
class TH2;
class BinnedHistSet;

class Diphoton
{
  public:
    float mass, E, eta, phi, pT, xF;
    /* Diphoton() {} */
    /* ~Diphoton() {} */
};

class PhiHists
{
  public:
    BinnedHistSet *phi_pT, *phi_pT_blue_up, *phi_pT_blue_down, *phi_pT_yellow_up, *phi_pT_yellow_down;
    BinnedHistSet *phi_xF, *phi_xF_blue_up, *phi_xF_blue_down, *phi_xF_yellow_up, *phi_xF_yellow_down;
    /* PhiHists() {} */
    /* ~PhiHists() {} */
};

class neutralMesonTSSA : public SubsysReco
{
 public:

  neutralMesonTSSA(const std::string &name = "neutralMesonTSSA", std::string histname = "neutralMesonTSSA_hists.root", bool isMC = false);

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
  bool InRange(float mass, std::pair<float, float> range);

  // In Init()
  void MakePhiHists(std::string which); // which = pi0, eta, pi0bkgr, etabkgr
  void MakeAllHists();
  void MakeVectors();
  // In InitRun()
  void GetRunNum();
  int GetSpinInfo(); // spin patterns, beam polarization and crossing shift for this run
  /* void CountLumi(); */
  // In process_event()
  void GetTrigger();
  void GetBunchNum();
  void GetSpins(); // blue and yellow spins for this event
  void FindGoodClusters();
  void FindDiphotons();
  void FillPhiHists(std::string which, int index); // which = pi0, eta, pi0bkgr, etabkgr
  void FillAllPhiHists();
  // In ResetEvent()
  void ClearVectors();
  // In End()
  void DeleteStuff();
  // Other info
  void PrintTrigger();

 private:
  bool isMonteCarlo;
  std::string outfilename;
  TFile* outfile = nullptr;

  // data containers
  RawClusterContainer* m_clusterContainer = nullptr;
  PHG4TruthInfoContainer* m_truthInfo = nullptr;
  GlobalVertex* gVtx = nullptr;
  PHG4VtxPoint* mcVtx = nullptr;
  RunHeader* runHeader = nullptr;
  Gl1Packet* gl1Packet = nullptr;

  // spin info
  static const int NBUNCHES = 120;
  int runNum = 0;
  int spinPatternBlue[NBUNCHES] = {0};
  int spinPatternYellow[NBUNCHES] = {0};
  int bunchNum = 0;
  int crossingShift = 0;
  int sphenixBunch = 0;
  int bspin = 0;
  int yspin = 0;
  long long gl1pScalers[NBUNCHES] = {0};
  float lumiUpBlue = 0;
  float lumiDownBlue = 0;
  /* float relLumiBlue; */
  float polBlue = 0;
  float lumiUpYellow = 0;
  float lumiDownYellow = 0;
  /* float relLumiYellow; */
  float polYellow = 0;

  // cluster distributions
  TH1* h_clusterE = nullptr;
  TH1* h_clusterEta = nullptr;
  TH1* h_clusterPhi = nullptr;
  TH2* h_clusterEta_Phi = nullptr;
  TH1* h_clusterpT = nullptr;
  TH1* h_clusterxF = nullptr;
  TH1* h_clusterChi2 = nullptr;
  TH1* h_clusterChi2zoomed = nullptr;

  // diphoton distributions
  TH1* h_diphotonMass = nullptr;
  BinnedHistSet* bhs_diphotonMass_pT = nullptr;
  BinnedHistSet* bhs_diphotonMass_xF = nullptr;
  TH1* h_diphotonpT = nullptr;
  TH1* h_diphotonxF = nullptr;

  // clusters and cuts
  float min_clusterE = 1.0;
  float max_clusterE = 8.0;
  float max_clusterChi2 = 4.0;
  std::vector<float>* goodclusters_E = nullptr;
  std::vector<float>* goodclusters_Eta = nullptr;
  std::vector<float>* goodclusters_Phi = nullptr;
  std::vector<float>* goodclusters_Ecore = nullptr;
  std::vector<float>* goodclusters_Chi2 = nullptr;

  // diphotons and cuts
  float min_diphotonPt = 1.0;
  float max_asym = 0.6;
  float min_deltaR = 0.0;
  float max_deltaR = 999.9;
  float pi0MassMean = 0.142;
  float pi0MassSigma = 0.0168;
  std::pair<float, float> pi0MassRange{0.117, 0.167};
  std::pair<float, float> pi0BkgrLowRange{0.047, 0.097};
  std::pair<float, float> pi0BkgrHighRange{0.177, 0.227};
  float min_pi0Mass;
  float max_pi0Mass;
  float etaMassMean = 0.564;
  float etaMassSigma = 0.020;
  std::pair<float, float> etaMassRange{0.494, 0.634};
  std::pair<float, float> etaBkgrLowRange{0.300, 0.400};
  std::pair<float, float> etaBkgrHighRange{0.700, 0.800};
  float min_etaMass;
  float max_etaMass;
  std::vector<Diphoton>* pi0s = nullptr;
  std::vector<Diphoton>* etas = nullptr;
  std::vector<Diphoton>* pi0Bkgr = nullptr;
  std::vector<Diphoton>* etaBkgr = nullptr;

  // phi histograms for asymmetries
  const float PI = 3.141592;
  int nBins_pT = 6;
  float bhs_max_pT = 8.0;
  int nBins_xF = 6;
  float bhs_max_xF = 0.15;
  int nHistBins_phi = 16;
  PhiHists* pi0Hists= nullptr;
  PhiHists* etaHists= nullptr;
  PhiHists* pi0BkgrHists= nullptr;
  PhiHists* etaBkgrHists= nullptr;

  // counters for events
  long int n_events_total = 0;
  bool mbdNtrigger = false;
  bool mbdStrigger = false;
  bool mbdtrigger = false;
  long int mbdcoinc_withoutNandS = 0;
  long int n_events_mbdtrigger = 0;
  long int n_events_mbdtrigger_vtx1 = 0;
  long int n_events_mbdtrigger_vtx2 = 0;
  long int n_events_mbdtrigger_vtx3 = 0;
  bool mbdvertex = false;
  bool globalvertex = false;
  long int n_events_mbdvtx_first1k = 0;
  long int n_events_globalvtx_first1k = 0;
  long int first_mbdvtx = 0;
  long int first_globalvtx = 0;
  long int n_events_mbdvtx_with_mbdtrig = 0;
  long int n_events_mbdvtx_without_mbdtrig = 0;
  long int n_events_globalvtx_with_mbdtrig = 0;
  long int n_events_globalvtx_without_mbdtrig = 0;
  long int n_events_globalvtx_with_mbdvtx = 0;
  long int n_events_globalvtx_without_mbdvtx = 0;
  long int n_events_with_mbdvertex = 0;
  long int n_events_with_globalvertex = 0;
  long int n_events_with_vtx10 = 0;
  long int n_events_with_vtx30 = 0;
  long int n_events_with_vtx50 = 0;
  long int n_events_with_good_vertex = 0;
  long int n_events_positiveCaloE = 0;

};

#endif // NEUTRALMESONTSSA_H
