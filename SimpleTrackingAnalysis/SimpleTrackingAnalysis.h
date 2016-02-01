#ifndef __SVTXSIMPERFORMANCECHECKRECO_H__
#define __SVTXSIMPERFORMANCECHECKRECO_H__

#include <fun4all/SubsysReco.h>
#include <string>
#include <TH1D.h>
#include <TH2D.h>

class PHCompositeNode;

class SimpleTrackingAnalysis: public SubsysReco
{

 public:

  SimpleTrackingAnalysis(const std::string &name="SimpleTrackingAnalysis");

  int Init(PHCompositeNode *topNode);
  int process_event(PHCompositeNode *topNode);
  int End(PHCompositeNode *topNode);

  void set_nlayers(unsigned int x) {nlayers = x;}

 private:

  // event counter
  unsigned long long nevents;

  // number of layers
  unsigned int nlayers;

  // output histograms ---------------------------------------------------------

  TH2D* _truept_dptoverpt;                    // momentum resolution

  TH2D* _truept_dca;                          // dca resolution

  TH1D* _truept_particles_leaving7Hits;       // pattern reco eff baseline

  TH1D* _truept_particles_recoWithExactHits;  // pattern reco eff by nhits
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

  TH2D* _recopt_quality;                     // quality distributions

  TH1D* _dx_vertex;                          // vertex resolution
  TH1D* _dy_vertex;
  TH1D* _dz_vertex;

  TH2D* _truept_quality_particles_recoWithin4Percent;

  TH2D* _recopt_quality_tracks_all;
  TH2D* _recopt_quality_tracks_recoWithin4Percent;

};

#endif // __SVTXSIMPERFORMANCECHECKRECO_H__
