#ifndef __SIMPLETRACKINGANALYSIS_H__
#define __SIMPLETRACKINGANALYSIS_H__


// --- need to check all these includes...
#include <fun4all/SubsysReco.h>
#include <vector>

class PHCompositeNode;
//class PHG4HoughTransform;
class TH1D;
class TH2D;
//class TProfile;
class TProfile2D;
class RawTower;
class RawCluster;
class RawTowerContainer;
class RawClusterContainer;

class SimpleTrackingAnalysis: public SubsysReco
{

 public:

  SimpleTrackingAnalysis(const std::string &name="SimpleTrackingAnalysis");

  int Init(PHCompositeNode*);
  int process_event(PHCompositeNode*);
  int End(PHCompositeNode*);

  void set_nlayers(unsigned int x) {nlayers = x;}

  void set_verbosity(int x) {verbosity = x;}
  //void set_docalocuts(bool x) {docalocuts = x;}



 private:

  // all values taken from sPHENIX pCDR
  // note the somewhat mysterious overlap
  // between the outer edge of the EMC
  // and the inner edge of the inner HCal
  // that shouldn't be a problem here, though
  const double emc_radius_inner =  90.0;
  const double hci_radius_inner = 115.7;
  const double hco_radius_inner = 182.0;
  const double emc_radius_outer = 116.1;
  const double hci_radius_outer = 137.0;
  const double hco_radius_outer = 268.5;

  // PHG4HoughTransform _hough; ERROR

  double magneticfield;

  int verbosity;
  //bool docalocuts;

  // event counter
  unsigned long long nevents;
  unsigned long long nerrors;
  unsigned long long nwarnings;

  // number of layers
  unsigned int nlayers;

  // output histograms ---------------------------------------------------------

  TH2D* _recopt_quality;                      // quality distributions
  TH2D* _recopt_quality_tracks_all;
  TH2D* _recopt_quality_tracks_recoWithin4Percent;
  TH2D* _truept_quality_particles_recoWithin4Percent;

  TH2D* _truept_dca; // dca resolution
  TH2D* _truept_dptoverpt; // momentum resolution
  TH1D* _truept_particles_leavingAllHits; // pattern reco eff baseline
  TH1D* _truept_particles_recoWithExactHits; // pattern reco eff by nhits
  TH1D* _truept_particles_recoWithin1Hit;
  TH1D* _truept_particles_recoWithin2Hits;
  TH1D* _truept_particles_recoWithin3Percent; // parttern reco eff by momentum match
  TH1D* _truept_particles_recoWithin4Percent;
  TH1D* _truept_particles_recoWithin5Percent;



  TH1D* _recopt_tracks_all;                   // purity baseline (non-embedded particles)
  TH1D* _recopt_tracks_recoWithExactHits;     // purity by nhit match
  TH1D* _recopt_tracks_recoWithin1Hit;
  TH1D* _recopt_tracks_recoWithin2Hits;
  TH1D* _recopt_tracks_recoWithin3Percent;    // purity by momentum match
  TH1D* _recopt_tracks_recoWithin4Percent;
  TH1D* _recopt_tracks_recoWithin5Percent;


  // purity histograms for calo cuts
  TH2D* th2d_recopt_tracks_withcalocuts_all; // purity baseline (non-embedded particles)
  TH2D* th2d_recopt_tracks_withcalocuts_recoWithExactHits; // purity by nhit match
  TH2D* th2d_recopt_tracks_withcalocuts_recoWithin1Hit;
  TH2D* th2d_recopt_tracks_withcalocuts_recoWithin2Hits;
  TH2D* th2d_recopt_tracks_withcalocuts_recoWithin3Percent; // purity by momentum match
  TH2D* th2d_recopt_tracks_withcalocuts_recoWithin4Percent;
  TH2D* th2d_recopt_tracks_withcalocuts_recoWithin5Percent;
  TH2D* th2d_recopt_tracks_withcalocuts_recoWithin1Sigma; // purity by momentum match
  TH2D* th2d_recopt_tracks_withcalocuts_recoWithin2Sigma;
  TH2D* th2d_recopt_tracks_withcalocuts_recoWithin3Sigma;

  // efficiency histograms for calo cuts
  TH2D* th2d_truept_particles_withcalocuts_leavingAllHits;
  TH2D* th2d_truept_particles_withcalocuts_recoWithExactHits;
  TH2D* th2d_truept_particles_withcalocuts_recoWithin1Hit;
  TH2D* th2d_truept_particles_withcalocuts_recoWithin2Hits;
  TH2D* th2d_truept_particles_withcalocuts_recoWithin3Percent;
  TH2D* th2d_truept_particles_withcalocuts_recoWithin4Percent;
  TH2D* th2d_truept_particles_withcalocuts_recoWithin5Percent;
  TH2D* th2d_truept_particles_withcalocuts_recoWithin1Sigma;
  TH2D* th2d_truept_particles_withcalocuts_recoWithin2Sigma;
  TH2D* th2d_truept_particles_withcalocuts_recoWithin3Sigma;

  // --- new additional (eventual replacement?) histograms for purity study
  TH2D* th2d_reco_calo_nhits8;
  TH2D* th2d_reco_calo_nhits7;
  TH2D* th2d_reco_calo_nhits6;
  TH2D* th2d_reco_calo_nhits5;
  TH2D* th2d_reco_calo_nhits4;
  TH2D* th2d_reco_calo_nhits3;
  TH2D* th2d_reco_calo_nhits2;
  TH2D* th2d_reco_calo_nhits1;

  TH2D* th2d_reco_calo_pt1sigma;
  TH2D* th2d_reco_calo_pt2sigma;
  TH2D* th2d_reco_calo_pt3sigma;
  TH2D* th2d_reco_calo_pt4sigma;
  TH2D* th2d_reco_calo_pt5sigma;
  TH2D* th2d_reco_calo_pt6sigma;



  // --- new additional (eventual replacement?) histograms for purity study
  TH2D* th2d_true_calo_nhits8;
  TH2D* th2d_true_calo_nhits7;
  TH2D* th2d_true_calo_nhits6;
  TH2D* th2d_true_calo_nhits5;
  TH2D* th2d_true_calo_nhits4;
  TH2D* th2d_true_calo_nhits3;
  TH2D* th2d_true_calo_nhits2;
  TH2D* th2d_true_calo_nhits1;

  TH2D* th2d_true_calo_pt1sigma;
  TH2D* th2d_true_calo_pt2sigma;
  TH2D* th2d_true_calo_pt3sigma;
  TH2D* th2d_true_calo_pt4sigma;
  TH2D* th2d_true_calo_pt5sigma;
  TH2D* th2d_true_calo_pt6sigma;


  // vertex info hisograms
  TH1D* _dx_vertex;
  TH1D* _dy_vertex;
  TH1D* _dz_vertex;

  TH1D* hmult;
  TH1D* hmult_vertex;



};

#endif // __SIMPLETRACKINGANALYSIS_H__
