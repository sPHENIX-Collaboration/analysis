#ifndef __SVTXSIMPERFORMANCECHECKRECO_H__
#define __SVTXSIMPERFORMANCECHECKRECO_H__

#include <fun4all/SubsysReco.h>
#include <string>
#include <TH1D.h>
#include <TH2D.h>

class PHCompositeNode;

class SvtxSimPerformanceCheckReco: public SubsysReco {

public: 

  SvtxSimPerformanceCheckReco(const std::string &name="SvtxSimPerformanceCheckReco");

  int Init(PHCompositeNode *topNode);
  int process_event(PHCompositeNode *topNode);
  int End(PHCompositeNode *topNode);

private:

  // event counter
  unsigned long long _event;

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

  TH2D* _eff_vs_purity_pt_0_1;
  TH2D* _eff_vs_purity_pt_1_2;
  TH2D* _eff_vs_purity_pt_2_3;
  TH2D* _eff_vs_purity_pt_3_4;
  TH2D* _eff_vs_purity_pt_4_5;
  TH2D* _eff_vs_purity_pt_5_6;
  TH2D* _eff_vs_purity_pt_6_7;
};

#endif // __SVTXSIMPERFORMANCECHECKRECO_H__
