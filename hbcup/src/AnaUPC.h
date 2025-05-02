#ifndef ANAUPC_H
#define ANAUPC_H

#include <Rtypes.h>
#include <fun4all/SubsysReco.h>

/// Class declarations for use in the analysis module
class PHCompositeNode;
class TFile;
class TTree;
class TH1;
class TH2;
class PHCompositeNode;
class RawClusterContainer;
class RawCluster;
class SvtxTrackMap;
class JetMap;
class GlobalVertex;
class JetEvalStack;
class JetRecoEval;
class SvtxTrackEval;
class PHG4TruthInfoContainer;
class PHHepMCGenEventMap;
class PHHepMCGenEvent;
class CaloTriggerInfo;
class JetTruthEval;
class SvtxEvalStack;
class EventHeader;

/// Definition of this analysis module class
class AnaUPC : public SubsysReco
{
 public:
  /// Constructor
  AnaUPC(const std::string &name = "AnaUPC",
              const std::string &outputfname = "AnaUPC.root");

  // Destructor
  virtual ~AnaUPC();

  /// SubsysReco initialize processing method
  int Init(PHCompositeNode *) override;

  /// SubsysReco event processing method
  int process_event(PHCompositeNode *) override;

  /// SubsysReco end processing method
  int End(PHCompositeNode *) override;

  /// Set the minimum jet pT to cut on
  //void setMinJetPt(float minjetpt) { m_minjetpt = minjetpt; }

  /// Set the minimum cluster pT to cut on
  //void setMinClusPt(float mincluspt) { m_mincluspt = mincluspt; }

  /// Set things to analyze
  void analyzeTracks(bool analyzeTracks) { m_analyzeTracks = analyzeTracks; }
  //void analyzeClusters(bool analyzeClusters) { m_analyzeClusters = analyzeClusters; }
  //void analyzeJets(bool analyzeJets) { m_analyzeJets = analyzeJets; }
  void analyzeTruth(bool analyzeTruth) { m_analyzeTruth = analyzeTruth; }

  int Reset(PHCompositeNode * /*topNode*/) override;

 private:
  /// String to contain the outfile name containing the trees
  std::string m_outfilename;

  /// A float for cutting on jet pt
  //float m_minjetpt;

  /// A float for cutting on cluster pt
  //float m_mincluspt;

  /// A boolean for running over tracks
  bool m_analyzeTracks;

  /// A boolean for running over clusters
  //bool m_analyzeClusters;

  /// A boolean for running over jets
  //bool m_analyzeJets;

  /// A boolean for collecting hepmc information
  bool m_analyzeTruth;

  /// TFile to hold the following TTrees and histograms
  TFile *m_outfile{nullptr};
  //TTree *m_clustertree;
  TTree *m_tracktree{nullptr};
  TTree *m_hepmctree{nullptr};
  TTree *m_truthtree{nullptr};
  TTree *m_globaltree{nullptr};

  TTree *m_pairtree{nullptr};
 
  TH1 *h_phi[2]{nullptr,nullptr};       // [0]=opp. sign, [1]=like sign
  TH2 *h2_eta_phi[2]{nullptr,nullptr};
  TH1 *h_mass[2]{nullptr,nullptr};
  TH1 *h_pt[2]{nullptr,nullptr};
  TH1 *h_y[2]{nullptr,nullptr};
  TH1 *h_eta[2]{nullptr,nullptr};

  TH1 *h_trig{nullptr};
  TH1 *h_ntracks{nullptr};
  TH2 *h2_ntrksvsb{nullptr};

  TH1 *h_b_mb{nullptr};
  TH1 *h_npart_mb{nullptr};
  TH1 *h_ncoll_mb{nullptr};
  TH1 *h_b{nullptr};
  TH1 *h_npart{nullptr};
  TH1 *h_ncoll{nullptr};

  const double E_MASS = 0.000510998950;  // electron mass [Gev]

  SvtxEvalStack *m_svtxEvalStack{nullptr};
  //JetEvalStack *m_jetEvalStack{nullptr};

  /// Methods for grabbing the data
  int GetNodes(PHCompositeNode *topNode);
  int getTracks(PHCompositeNode *topNode);
  //void getTruthJets(PHCompositeNode *topNode);
  //void getReconstructedJets(PHCompositeNode *topNode);
  //void getEMCalClusters(PHCompositeNode *topNode);
  void getHEPMCTruth();
  void getPHG4Truth();

  /// Data
  EventHeader *_evthdr{nullptr};
  SvtxTrackMap *_trackmap{nullptr};
  PHG4TruthInfoContainer *_truthinfo{nullptr};
  PHHepMCGenEventMap *_genevent_map{nullptr};

  void initializeVariables();
  void initializeTrees();

  /**
   * Make variables for the relevant trees
   */

  // Global Info
  Int_t   m_run{ 0 };
  Int_t   m_evt{ 0 };
  Int_t   m_npart_targ{ 0 };
  Int_t   m_npart_proj{ 0 };
  Int_t   m_npart{ 0 };
  Int_t   m_ncoll{ 0 };
  Int_t   m_ncoll_hard{ 0 };
  Float_t m_bimpact{ -1. };
  Int_t   m_ntracks{ 0 };       // total reconstructed ntracks
  Int_t   m_ntrk_sphenix{ 0 };  // monte carlo tracks in sphenix acceptance (>0.4 pt, |eta|<1.1)
  Int_t   m_ntrk_mc{ 0 };       // total monte carlo charged tracks

  /// HEPMC Tree variables
  int m_partid1;
  int m_partid2;
  double m_x1;
  double m_x2;
  int m_mpi;
  int m_process_id;
  double m_truthenergy;
  double m_trutheta;
  double m_truthphi;
  double m_truthpx;
  double m_truthpy;
  double m_truthpz;
  double m_truthpt;
  double m_truthp;
  int m_numparticlesinevent;
  int m_truthpid;
  int m_truthcharge;


  /// Track variables
  double m_tr_px;
  double m_tr_py;
  double m_tr_pz;
  double m_tr_p;
  double m_tr_pt;
  double m_tr_phi;
  double m_tr_eta;
  int m_charge;
  double m_chisq;
  int m_ndf;
  double m_dca;
  double m_tr_x;
  double m_tr_y;
  double m_tr_z;
  int m_truth_is_primary;
  double m_truthtrackpx;
  double m_truthtrackpy;
  double m_truthtrackpz;
  double m_truthtrackp;
  double m_truthtracke;
  double m_truthtrackpt;
  double m_truthtrackphi;
  double m_truthtracketa;
  int m_truthtrackpid;

  /// Pair variables
  Double_t m_pm{ 0. };    // pair mass
  Double_t m_ppt{ 0. };
  Double_t m_pphi{ 0. };
  Double_t m_py{ 0. };
  Double_t m_peta{ 0. };
  Double_t m_pdphi{ 0. };
  Double_t m_ppt1{ 0. };
  Double_t m_ppz1{ 0. };
  Double_t m_pphi1{ 0. };
  Double_t m_peta1{ 0. };
  Double_t m_ppt2{ 0. };
  Double_t m_ppz2{ 0. };
  Double_t m_pphi2{ 0. };
  Double_t m_peta2{ 0. };
  Short_t  m_pq1{ 0 };
  Short_t  m_pq2{ 0 };
 
  /// Reconstructed jet variables
  /*
  double m_recojetpt;
  int m_recojetid;
  double m_recojetpx;
  double m_recojetpy;
  double m_recojetpz;
  double m_recojetphi;
  double m_recojetp;
  double m_recojetenergy;
  double m_recojeteta;
  int m_truthjetid;
  double m_truthjetp;
  double m_truthjetphi;
  double m_truthjeteta;
  double m_truthjetpt;
  double m_truthjetenergy;
  double m_truthjetpx;
  double m_truthjetpy;
  double m_truthjetpz;
  double m_dR;
  */

  /// Cluster variables
  /*
  double m_clusenergy;
  double m_cluseta;
  double m_clustheta;
  double m_cluspt;
  double m_clusphi;
  double m_cluspx;
  double m_cluspy;
  double m_cluspz;
  double m_E_4x4;
  */
};

#endif  // ANAUPC_H
