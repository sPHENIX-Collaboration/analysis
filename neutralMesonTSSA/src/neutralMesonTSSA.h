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
class TTree;
class TH1;
class TH2;
class BinnedHistSet;

class Diphoton
{
  public:
    float mass, E, eta, phi, pT, xF, vtxz;
    int clus1index, clus2index;
    /* Diphoton() {} */
    /* ~Diphoton() {} */
};

class PhiHists
{
  public:
    BinnedHistSet *phi_pT, *phi_pT_blue_up, *phi_pT_blue_down, *phi_pT_yellow_up, *phi_pT_yellow_down;
    BinnedHistSet *phi_xF, *phi_xF_blue_up, *phi_xF_blue_down, *phi_xF_yellow_up, *phi_xF_yellow_down;
    BinnedHistSet *phi_eta, *phi_eta_blue_up, *phi_eta_blue_down, *phi_eta_yellow_up, *phi_eta_yellow_down;
    BinnedHistSet *phi_vtxz, *phi_vtxz_blue_up, *phi_vtxz_blue_down, *phi_vtxz_yellow_up, *phi_vtxz_yellow_down;
    /* PhiHists() {} */
    /* ~PhiHists() {} */
};

class neutralMesonTSSA : public SubsysReco
{
 public:

  neutralMesonTSSA(const std::string &name = "neutralMesonTSSA", std::string histname = "neutralMesonTSSA_hists.root", std::string treename = "neutralMesonTSSA_trees.root", bool isMC = false);

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
  /* void FillClusterTree(); */
  void FindDiphotons();
  /* void FillDiphotonTree(); */
  /* void FillPhiHists(std::string which, int index); // which = pi0, eta, pi0bkgr, etabkgr */
  /* void FillAllPhiHists(); */
  // In ResetEvent()
  void ClearVectors();
  // In End()
  void DeleteStuff();
  // Other info
  void PrintTrigger();

 private:
  bool isMonteCarlo;
  std::string outfilename_hists;
  std::string outfilename_trees;
  TFile* outfile_hists = nullptr;
  TFile* outfile_trees = nullptr;
  TTree* tree_event = nullptr;
  TTree* tree_clusters = nullptr;
  TTree* tree_diphotons = nullptr;

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
  float crossingAngle = -999.9;
  float crossingAngleIntended = -999.9;

  // trigger & event-level info
  bool mbdNtrigger = false;
  bool mbdStrigger = false;
  bool mbdtrigger_live = false;
  bool photontrigger_live = false;
  bool mbdtrigger_scaled = false;
  bool photontrigger_scaled = false;
  bool mbdvertex = false;
  bool globalvertex = false;
  TH1* h_nEvents = nullptr;
  float vtxz = -9999999.9;
  TH1* h_vtxz = nullptr;
  TH1* h_crossingAngle = nullptr;

  // tower-level info
  TH2* h_towerEta_Phi_500MeV = nullptr;
  TH2* h_towerEta_Phi_800MeV = nullptr;

  // cluster distributions
  TH1* h_nAllClusters = nullptr;
  TH1* h_nGoodClusters = nullptr;
  TH1* h_clusterE = nullptr;
  TH1* h_clusterEta = nullptr;
  TH2* h_clusterVtxz_Eta = nullptr;
  TH1* h_clusterPhi = nullptr;
  TH2* h_clusterEta_Phi = nullptr;
  TH1* h_clusterpT = nullptr;
  TH1* h_clusterxF = nullptr;
  TH2* h_clusterpT_xF = nullptr;
  TH1* h_clusterChi2 = nullptr;
  TH1* h_clusterChi2zoomed = nullptr;
  TH1* h_mesonClusterChi2 = nullptr;
  TH1* h_goodClusterE = nullptr;
  TH1* h_goodClusterEta = nullptr;
  TH2* h_goodClusterVtxz_Eta = nullptr;
  TH1* h_goodClusterPhi = nullptr;
  TH2* h_goodClusterEta_Phi = nullptr;
  TH1* h_goodClusterpT = nullptr;
  TH1* h_goodClusterxF = nullptr;
  TH2* h_goodClusterpT_xF = nullptr;

  // diphoton distributions
  TH1* h_nDiphotons = nullptr;
  TH1* h_nRecoPi0s = nullptr;
  TH1* h_nRecoEtas = nullptr;
  TH1* h_diphotonE = nullptr;
  TH1* h_diphotonMass = nullptr;
  TH1* h_diphotonEta = nullptr;
  TH2* h_diphotonVtxz_Eta = nullptr;
  TH1* h_diphotonPhi = nullptr;
  TH2* h_diphotonEta_Phi = nullptr;
  TH1* h_diphotonpT = nullptr;
  TH1* h_diphotonxF = nullptr;
  TH2* h_diphotonpT_xF = nullptr;
  TH1* h_diphotonAsym = nullptr;
  TH1* h_diphotonDeltaR = nullptr;
  TH2* h_diphotonAsym_DeltaR = nullptr;
  BinnedHistSet* bhs_diphotonMass_pT = nullptr;
  BinnedHistSet* bhs_diphotonMass_xF = nullptr;

  // clusters and cuts
  float min_clusterE = 0.5;
  float max_clusterE = 30.0;
  float max_clusterChi2 = 1000.0;
  int nAllClusters = 0;
  int nGoodClusters = 0;
  std::vector<float>* goodclusters_E = nullptr;
  std::vector<float>* goodclusters_Ecore = nullptr;
  std::vector<float>* goodclusters_Eta = nullptr;
  std::vector<float>* goodclusters_Phi = nullptr;
  std::vector<float>* goodclusters_pT = nullptr;
  std::vector<float>* goodclusters_xF = nullptr;
  std::vector<float>* goodclusters_Chi2 = nullptr;

  // diphotons and cuts
  float min_diphotonPt = 1.0;
  float max_asym = 1.0;
  float min_deltaR = 0.0;
  float max_deltaR = 999.9;
  std::vector<float>* diphoton_E = nullptr;
  std::vector<float>* diphoton_M = nullptr;
  std::vector<float>* diphoton_Eta = nullptr;
  std::vector<float>* diphoton_Phi = nullptr;
  std::vector<float>* diphoton_pT = nullptr;
  std::vector<float>* diphoton_xF = nullptr;
  std::vector<int>* diphoton_clus1index = nullptr;
  std::vector<int>* diphoton_clus2index = nullptr;
  std::vector<float>* diphoton_deltaR = nullptr;
  std::vector<float>* diphoton_asym = nullptr;

  std::pair<float, float> pi0MassRange{0.117, 0.167};
  std::pair<float, float> pi0BkgrLowRange{0.047, 0.097};
  std::pair<float, float> pi0BkgrHighRange{0.177, 0.227};
  std::pair<float, float> etaMassRange{0.494, 0.634};
  std::pair<float, float> etaBkgrLowRange{0.300, 0.400};
  std::pair<float, float> etaBkgrHighRange{0.700, 0.800};
  std::vector<Diphoton>* pi0s = nullptr;
  std::vector<Diphoton>* etas = nullptr;
  std::vector<Diphoton>* pi0Bkgr = nullptr;
  std::vector<Diphoton>* etaBkgr = nullptr;

  // phi histograms for asymmetries
  const float PI = 3.141592;
  int nBins_pT = 8;
  float bhs_max_pT = 12.0;
  int nBins_xF = 8;
  float bhs_max_xF = 0.15;
  int nHistBins_phi = 16;
  PhiHists* pi0Hists = nullptr;
  PhiHists* etaHists = nullptr;
  PhiHists* pi0BkgrHists = nullptr;
  PhiHists* etaBkgrHists = nullptr;
  PhiHists* pi0Hists_lowEta = nullptr;
  PhiHists* etaHists_lowEta = nullptr;
  PhiHists* pi0BkgrHists_lowEta = nullptr;
  PhiHists* etaBkgrHists_lowEta = nullptr;
  PhiHists* pi0Hists_highEta = nullptr;
  PhiHists* etaHists_highEta = nullptr;
  PhiHists* pi0BkgrHists_highEta = nullptr;
  PhiHists* etaBkgrHists_highEta = nullptr;

  // counters for events
  long int n_events_total = 0;
  long int mbdcoinc_withoutNandS = 0;
  long int n_events_mbdtrigger = 0;
  long int n_events_mbdtrigger_vtx1 = 0;
  long int n_events_mbdtrigger_vtx2 = 0;
  long int n_events_mbdtrigger_vtx3 = 0;
  long int n_events_photontrigger = 0;
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
