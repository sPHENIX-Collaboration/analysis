// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef SILICONSEEDSANA_H
#define SILICONSEEDSANA_H

#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllHistoManager.h>
#include <fun4all/Fun4AllReturnCodes.h>

//track include
#include <trackbase/ActsGeometry.h>
#include <trackbase/TrkrDefs.h>
#include <trackbase/TrkrClusterContainer.h>
#include <trackbase/TrackFitUtils.h>

//track map include
#include <trackbase_historic/SvtxTrack.h>
#include <trackbase_historic/SvtxTrackMap.h>
#include <trackbase_historic/TrackAnalysisUtils.h>
#include <trackbase_historic/SvtxTrackState.h>
//#include <trackbase_historic/SvtxTrackState_v1.h>

// vertex include
#include <globalvertex/SvtxVertex.h>
#include <globalvertex/SvtxVertexMap.h>

//calo include
#include <calobase/RawCluster.h>
#include <calobase/RawClusterContainer.h>
#include <calobase/RawTower.h>
#include <calobase/RawTowerContainer.h>
#include <calobase/RawTowerGeom.h>
#include <calobase/RawTowerGeomContainer.h>
#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfoContainerv1.h>
#include <calobase/TowerInfoContainerv2.h>
#include <calobase/TowerInfoContainerv3.h>
#include <calobase/TowerInfo.h>

//truth include
#include <g4main/PHG4Hit.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4VtxPoint.h>
#include <g4eval/SvtxEvalStack.h>


#include <TLorentzVector.h>
#include <string>
#include <vector>
#include <memory>

class SvtxTrack;
class PHCompositeNode;
class TH1;
class TH2;
class TProfile2D;
class TFile;
class TTree;

class SiliconSeedsAna : public SubsysReco
{
public:
  SiliconSeedsAna(const std::string &name = "SiliconSeedsAna");

  ~SiliconSeedsAna() override = default;

  int InitRun(PHCompositeNode *topNode) override;
  int process_event(PHCompositeNode *topNode) override;
  int EndRun(const int runnumber) override;
  int End(PHCompositeNode *topNode) override;
  void setTrackMapName(const std::string &name) { m_trackMapName = name; }
  void setVertexMapName(const std::string &name) { m_vertexMapName = name; }
  void setOutputFileName(const std::string &name) { m_outputfilename = name; }
  void setStartEventNumber(int start)
  {
    evt = start;
    calo_evt = start;
  }
  void setClusterContainerName(const std::string &name) { m_clusterContainerName = name; }
  void setEMCalClusterContainerName(const std::string &name) { m_emcalClusName = name; }
  void setEMcalRadius(float radius) { _caloRadiusEMCal = radius; }
  void setEmcalLowEcut(float ecut)  { m_emcal_low_cut = ecut; }
  void setTopoCluster(bool topo)
  {
    if (topo)
    {
      setClusterContainerName("TOPOCLUSTER_EMCAL");
    }
  }


  
  void setMC(bool input, bool doEval=true) { isMC = input; _doEval = doEval;}
  void setVtxSkip(bool input) { b_skipvtx = input; }
  void setCaloSkip(bool input) { b_skipcalo = input; }


protected:
  //--std::tuple<float, float, float> getClusterPos(TrkrDefs::cluskey cluskey, TrkrClusterContainer *clusterContainer);
  std::string getHistoPrefix() const;

  void createTree();
  void createHistos();

  void fillTruthTree(PHCompositeNode *topNode);
  void processTrackMap(PHCompositeNode *topNode);
  void processSiCluster(PHCompositeNode *topNode);
  void processCaloClusters(PHCompositeNode *topNode);
  void processVertexMap(PHCompositeNode *topNode);

  // Utility functions for track vector management and EMCal state
  void clearTrackVectors();
  void fillEMCalState(SvtxTrackState* state, SvtxTrackState* ostate, SvtxTrackState* rvsState);
  void initTrackTreeBranches();
  void clearCaloVectors();
  void initCaloTreeBranches();
  SvtxTrackState* projectToEMCal(PHCompositeNode* topNode, SvtxTrack* trk);

  void getCaloPosition(RawCluster *calo, float &x, float &y, float &z);

  std::string m_clusterContainerName = "TRKR_CLUSTER";
  std::string m_actsgeometryName     = "ActsGeometry";
  std::string m_trackMapName         = "SvtxTrackMap";
  std::string m_vertexMapName        = "SvtxVertexMap";
  std::string m_emcalClusName        = "CLUSTER_CEMC";
  std::string m_outputfilename       = "output_histograms.root";

  // std::string m_emcal_node_name = "TOWERINFO_CALIB_CEMC";
  // std::string m_ihcal_node_name = "TOWERINFO_CALIB_HCALIN";
  // std::string m_ohcal_node_name = "TOWERINFO_CALIB_HCALOUT";
  // std::string m_ihcalClus_node_name = "CLUSTER_HCALIN";
  // std::string m_ohcalClus_node_name = "CLUSTER_HCALOUT";
  SvtxTrackMap  *trackmap      = nullptr;
  SvtxTrack     *track         = nullptr;
  TrackSeed     *si_seed       = nullptr;
  TrkrCluster   *trkrCluster   = nullptr;
  std::unique_ptr<SvtxEvalStack> _svtxEvalStack;
 
  // Truth info tree and vectors
  TFile *m_outfile = nullptr;

  TTree *truthTree = nullptr;
  std::vector<int>   truth_pid, truth_id;
  std::vector<float> truth_px, truth_py, truth_pz;
  std::vector<float> truth_x, truth_y, truth_z;
  std::vector<float> truth_pt, truth_eta, truth_phi, truth_e;
  std::vector<int>   truth_vtxid;
  std::vector<int>   truth_primary_id, truth_parent_id;
  std::vector<float> truth_vtx_x, truth_vtx_y, truth_vtx_z; // index is  vtxid, not trackid. vector size is Nvertex in this event(usually 1)

  TTree *trackTree = nullptr;
  int evt = 0;
  std::vector<unsigned int> track_id;
  std::vector<float> track_px, track_py, track_pz;
  std::vector<float> track_x, track_y, track_z;
  std::vector<float> track_pt,track_eta,track_phi;
  std::vector<float> track_chi2ndf;
  std::vector<float> track_dxy, track_dz; // xyz - vertex
  std::vector<short int> track_crossing;
  std::vector<int> track_charge;
  std::vector<int> track_nmaps;
  std::vector<int> track_nintt;
  std::vector<int> track_innerintt;
  std::vector<int> track_outerintt;
  std::vector<int> track_id_truth; // G4Particle ID (truth ID) for MC

  // track projection to EMC
  std::vector<float> track_px_emc, track_py_emc, track_pz_emc;
  std::vector<float> track_x_emc;
  std::vector<float> track_y_emc;
  std::vector<float> track_z_emc;
  std::vector<float> track_eta_emc;
  std::vector<float> track_phi_emc;
  std::vector<float> track_pt_emc;
  std::vector<float> track_x_oemc;
  std::vector<float> track_y_oemc;
  std::vector<float> track_z_oemc;
  std::vector<float> track_rv_x_emc;
  std::vector<float> track_rv_y_emc;
  std::vector<float> track_rv_z_emc;

  // --- Matched EMCal cluster info for each track ---
  std::vector<float> sicalo_pt;
  std::vector<float> sicalo_phi;
  std::vector<float> sicalo_eta;
  std::vector<float> sicalo_dphi;
  std::vector<float> sicalo_dz;
  std::vector<float> sicalo_emc_x;
  std::vector<float> sicalo_emc_y;
  std::vector<float> sicalo_emc_z;
  std::vector<float> sicalo_emc_r;
  std::vector<float> sicalo_emc_phi;
  std::vector<float> sicalo_emc_energy;
  std::vector<float> sicalo_emc_chi2;
  std::vector<float> sicalo_emc_prob;

  TTree *SiClusTree    = nullptr;
  TTree *SiClusAllTree = nullptr;
  std::vector<int>   SiClus_trackid;
  std::vector<int>   SiClus_layer;
  std::vector<float> SiClus_x;
  std::vector<float> SiClus_y;
  std::vector<float> SiClus_z;
  std::vector<int>   SiClus_t;

  TTree *caloTree = nullptr;
  int calo_evt = 0;
  std::vector<float> calo_x;
  std::vector<float> calo_y;
  std::vector<float> calo_z;
  std::vector<float> calo_r;
  std::vector<float> calo_phi;
  std::vector<float> calo_eta;
  std::vector<float> calo_energy;
  std::vector<float> calo_chi2;
  std::vector<float> calo_prob;

  TTree *evtTree = nullptr;
  //int trk_evt = 0;
  //int calo_evt = 0;
  int       evt_evtseq;
  long long evt_bco;
  uint64_t  evt_scaledtrig;
  int       evt_crossing;
  int       evt_nintt;
  int       evt_nintt50;
  int       evt_nmaps;
  int       evt_nemc;
  int       evt_nemc02;
  int       evt_nsiseed;
  int       evt_nsiseed0;
  float     evt_xvtx; // svtxvertex
  float     evt_yvtx; // svtxvertex
  float     evt_zvtx; // svtxvertex
  float     evt_xgvtx; // evtTree variable
  float     evt_ygvtx; // evtTree variable
  float     evt_zgvtx; // evtTree variable
  std::vector<int>   evt_gvtx_type;// all vertex in globalvertex
  std::vector<float> evt_gvtx_x;   // all vertex in globalvertex
  std::vector<float> evt_gvtx_y;   // all vertex in globalvertex
  std::vector<float> evt_gvtx_z;   // all vertex in globalvertex



  double m_emcal_low_cut = 0.18;
  float _caloRadiusEMCal    = 93.5;
  float _caloThicknessEMCal = 20.4997;

  bool isMC    = false;
  bool _doEval = false;

  bool b_skipvtx = false;
  bool b_skipcalo = false;
  TH1 *h_ntrack1d = nullptr;
  TH2 *h_ntrack = nullptr;
  TH1 *h_nmaps = nullptr;
  TH1 *h_nintt = nullptr;
  TH1 *h_nlayer = nullptr;
  TH2 *h_nmaps_nintt = nullptr;
  TProfile2D *h_avgnclus_eta_phi = nullptr;
  TH1 *h_trackcrossing = nullptr;
  TH1 *h_trackchi2ndf = nullptr;
  TH2 *h_dcaxyorigin_phi = nullptr;
  TH2 *h_dcaxyvtx_phi = nullptr;
  TH2 *h_dcazorigin_phi = nullptr;
  TH2 *h_dcazvtx_phi = nullptr;
  TH1 *h_ntrack_isfromvtx = nullptr;
  TH1 *h_trackpt_inclusive = nullptr;
  TH1 *h_trackpt_pos = nullptr;
  TH1 *h_trackpt_neg = nullptr;
  TH1 *h_ntrack_IsPosCharge = nullptr;

  TH1 *h_nvertex = nullptr;
  TH1 *h_vx = nullptr;
  TH1 *h_vy = nullptr;
  TH1 *h_vz = nullptr;
  TH2 *h_vx_vy = nullptr;
  TH1 *h_vt = nullptr;
  TH1 *h_vcrossing = nullptr;
  TH1 *h_vchi2dof = nullptr;
  TH1 *h_ntrackpervertex = nullptr;
};

#endif  // SILICONSEEDSANA_H

