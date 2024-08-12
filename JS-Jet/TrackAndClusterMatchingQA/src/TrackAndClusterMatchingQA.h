#ifndef TRACKANDCLUSTERMATCHINGQA_H__
#define TRACKANDCLUSTERMATCHINGQA_H__

#include <fun4all/SubsysReco.h>

#include <calobase/RawCluster.h>
#include <calobase/RawClusterContainer.h>
#include <calobase/RawClusterUtility.h>
#include <calobase/RawTower.h>
#include <calobase/RawTowerContainer.h>
#include <calobase/RawTowerGeom.h>
#include <calobase/RawTowerGeomContainer.h>
#include <calotrigger/CaloTriggerInfo.h>

#include <vector>

/// Class declarations for use in the analysis module
class Fun4AllHistoManager;
class PHCompositeNode;
class TFile;
class TH1;
class TH2;
class PHCompositeNode;
class RawClusterContainer;
class RawCluster;
class SvtxTrackMap;
class GlobalVertex;
class PHHepMCGenEventMap;
class JetRecoEval;
class SvtxTrackEval;
class PHG4TruthInfoContainer;
class PHHepMCGenEvent;
class SvtxTrack;
class PHG4Particle;

/// Definition of this analysis module class
class TrackAndClusterMatchingQA : public SubsysReco
{
 public:

  /// Constructor
  TrackAndClusterMatchingQA(const std::string &name = "TrackAndClusterMatchingQA",
              const std::string &fname = "TrackAndClusterMatchingQA.root");

  // Destructor
  virtual ~TrackAndClusterMatchingQA();

  /// SubsysReco initialize processing method
  int Init(PHCompositeNode *);

  /// SubsysReco event processing method
  int process_event(PHCompositeNode *);

  /// SubsysReco end processing method
  int End(PHCompositeNode *);

  void set_EM_clustersName(const std::string &name) { _em_clusters_name = name; }
  std::string get_EM_clustersName() { return _em_clusters_name; }

  void set_EM_retoweredTowersName(const std::string &name) { _em_retowered_towers_name = name; }
  std::string get_EM_retoweredTowersName() { return _em_retowered_towers_name; }

  void set_ApplyTowerSelection(bool b) { _apply_tower_selection = b; }
  bool get_ApplyTowerSelection() { return _apply_tower_selection; }

  void set_TrackMinPt(float f) { _track_min_pt = f; }
  float get_TrackMinPt() { return _track_min_pt; }

  void set_TrackQuality(float f) { _track_quality = f; }
  float get_TrackQuality() { return _track_quality; }

  void set_TrackMaxDCAxy(float f) { _track_max_dcaxy = f; }
  float get_TrackMaxDCAxy() { return _track_max_dcaxy; }

  void set_TrackMaxDCAz(float f) { _track_max_dcaz = f; }
  float get_TrackMaxDCAz() { return _track_max_dcaz; }

  void set_TrackMinTPCHits(int d) { _track_min_tpc_hits = d; }
  int get_TrackMinTPCHits() { return _track_min_tpc_hits; }

  void set_TrackMinSiliconHits(int d) { _track_min_silicon_hits = d; }
  int get_TrackMinSiliconHits() { return _track_min_silicon_hits; }

  void set_UseOriginWhenNoVertex(bool b) { _use_origin_when_no_vertex = b; }

  float GetDeltaR(float track_phi, float cluster_phi, float track_eta, float cluster_eta);

  bool IsAcceptableTrack(SvtxTrack *track, GlobalVertex *vtx);

 private:
  /// String to contain the outfile name containing the trees
  std::string _outfilename;

  SvtxTrackMap *_track_map;
  RawClusterContainer *_em_clusters;
  std::string _em_clusters_name = "";
  std::string _em_retowered_towers_name = "";
  bool _apply_tower_selection = true;
  bool _use_origin_when_no_vertex = false;

  //Track selection
  float _track_min_pt = 0.5;
  float _track_quality = 6.;
  float _track_max_dcaxy = 0.02;
  float _track_max_dcaz = 0.01;
  int _track_min_tpc_hits = 20;
  int _track_min_silicon_hits = 4;

  //Some track constants
  const unsigned int _nlayers_maps = 3;
  const unsigned int _nlayers_intt = 4;
  const unsigned int _nlayers_tpc = 48;

  /// Fun4All Histogram Manager tool
  TH2 *_h2trackPt_vs_clusterEt;
  TH1 *_h1EMCal_TowerEnergy;
  TH1 *_h1EMCal_TowerEnergy_Retowered;
  TH1 *_h1HCalIN_TowerEnergy;
  TH1 *_h1HCalOUT_TowerEnergy;
  TH1 *_h1EMCal_TowerEnergy_afterSelection;
  TH1 *_h1EMCal_TowerEnergy_Retowered_afterSelection;
  TH1 *_h1HCalIN_TowerEnergy_afterSelection;
  TH1 *_h1HCalOUT_TowerEnergy_afterSelection;
  TH2 *_h2TowerEnergy_EMCal_vs_HCalIN;
  TH2 *_h2TowerEnergy_EMCal_vs_HCalOUT;
  TH2 *_h2TowerEnergy_HCalIN_vs_HCalOUT;
  TH1 *_h1EMCal_RetowerEnergyFraction;

  TH1 *_h1Track_Quality;
  TH1 *_h1Track_DCAxy;
  TH1 *_h1Track_DCAz;
  TH1 *_h1Track_TPC_Hits;
  TH1 *_h1Track_Silicon_Hits;
  TH1 *_h1Track_Pt_beforeSelections;
  TH1 *_h1Track_Pt_afterSelections;

  TH1 *_h1Track_TPC_Hits_Selected;
  TH2 *_h2Track_TPC_Hits_vs_Phi;
  TH2 *_h2Track_TPC_Hits_vs_Eta;
  TH2 *_h2Track_TPC_Hits_vs_Pt;

  TH1 *_h1deta;
  TH1 *_h1dphi;
  TH1 *_h2phi_vs_deta;
  TH1 *_h1min_dR;



  /// TFile to hold the following TTrees and histograms
  TFile *_outfile;


  /**
   * Make variables for the relevant trees
   */


};

#endif
