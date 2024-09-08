#ifndef INTTANA_H__
#define INTTANA_H__

#include <fun4all/SubsysReco.h>
#include <string>

#include <cmath>
/// Cluster/Calorimeter includes

/// Fun4All includes
#include <fun4all/Fun4AllHistoManager.h>
#include <fun4all/Fun4AllReturnCodes.h>
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

// #include <trackbase/TrkrClusterv4.h>
// #include <trackbase/TrkrClusterContainerv3.h>
#include <trackbase/TrkrCluster.h>
#include <trackbase/TrkrClusterContainer.h>
#include <trackbase/TrkrHit.h>
#include <trackbase/TrkrHitSet.h>
#include <trackbase/TrkrHitSetContainer.h>
#include <trackbase/ActsGeometry.h>
#include <trackbase/InttDefs.h>

#include <trackbase/InttEventInfo.h>

#include <globalvertex/GlobalVertexv1.h>
#include <globalvertex/GlobalVertexMapv1.h>

// #include <globalvertex/MbdVertexv1.h>
// #include <globalvertex/MbdVertexMapv1.h>
#include <mbd/MbdOut.h>
#include <ffarawobjects/Gl1RawHit.h>
#include <ffarawobjects/Gl1Packetv2.h>

#include <ffarawobjects/InttRawHit.h>
#include <ffarawobjects/InttRawHitContainer.h>

// #include <inttxyvertexfinder/InttVertex.h>
// #include <inttxyvertexfinder/InttVertexMapv1.h>
// #include <inttxyvertexfinder/InttVertex3D.h>
// #include <inttxyvertexfinder/InttVertex3DMap.h>
#include <intt/InttVertexv1.h>
#include <intt/InttVertexMapv1.h>
// #include <inttxyvertexfinder/InttVertex3D.h>
// #include <inttxyvertexfinder/InttVertex3DMap.h>

#include <globalvertex/SvtxVertex.h>
#include <globalvertex/SvtxVertexMap.h>
#include <trackbase_historic/SvtxTrack.h>
#include <trackbase_historic/SvtxTrackMap.h>

#include <calobase/RawCluster.h>
#include <calobase/RawClusterContainer.h>

/// ROOT includes
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TNtuple.h>
#include <TTree.h>
#include <TVector3.h>

/// C++ includes
#include <cassert>
#include <cmath>
#include <sstream>
#include <string>

#include "InttEvent.h"
#include "InttRawData.h"

// truth
//  To get vertex
#include <globalvertex/GlobalVertexMap.h>
#include <globalvertex/GlobalVertex.h>

// TODO:
/// HEPMC truth includes
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <HepMC/GenEvent.h>
#include <HepMC/GenVertex.h>
#pragma GCC diagnostic pop

#include <phhepmc/PHHepMCGenEvent.h>
#include <phhepmc/PHHepMCGenEventMap.h>

#include <g4main/PHG4InEvent.h>
#include <g4main/PHG4VtxPoint.h> // for PHG4Particle
#include <g4main/PHG4Particle.h> // for PHG4Particle
// truth end

/// Class declarations for use in the analysis module
class PHCompositeNode;
class TFile;
class TH1;
class TH2;
class TNtuple;
class TTree;
class InttRawData;

struct ClustInfo
{
  int layer;
  Acts::Vector3 pos;
};

/// Definition of this analysis module class
class InttAna : public SubsysReco
{
 public:
  struct evtbco_info{
    int      evt_gl1 {-1};
    uint     evt_intt{ 0};
    int      evt_mbd {-1};
    uint64_t bco_gl1 {0};
    uint64_t bco_intt{0};
    uint64_t bco_mbd {0};

    void clear() {
      evt_gl1  = -1;
      evt_intt =  0;
      evt_mbd  = -1;
      bco_gl1  =  0;
      bco_intt =  0;
      bco_mbd  =  0;
    };

    void copy(const evtbco_info& info) {
      evt_gl1  = info.evt_gl1 ;
      evt_intt = info.evt_intt;
      evt_mbd  = info.evt_mbd ;
      bco_gl1  = info.bco_gl1 ;
      bco_intt = info.bco_intt;
      bco_mbd  = info.bco_mbd ;
    };
  };
  
 public:
  /// Constructor
  InttAna(const std::string &name = "InttAna",
          const std::string &fname = "AnaTutorial.root");

  // Destructor
  virtual ~InttAna();

  /// SubsysReco initialize processing method
  int Init(PHCompositeNode *) override;
 
  /// SubsysReco initialize processing method
  int InitRun(PHCompositeNode *) override;

  /// SubsysReco event processing method
  int process_event(PHCompositeNode *) override;

  /// Clean up internals after each event.
  int ResetEvent(PHCompositeNode *topNode) override;

  /// SubsysReco end processing method
  int End(PHCompositeNode *) override;

  //truth information
  // void getHEPMCTruth(PHCompositeNode *);
  // void getPHG4Particle(PHCompositeNode *);
  void getHEPMCTruth(PHCompositeNode *topNode);
  void getPHG4Particle(PHCompositeNode *topNode);

  void setInttRawData(InttRawData *rawModule) { _rawModule = rawModule; }
  void readRawHit(PHCompositeNode *);

  void setBeamCenter(float x=0, float y=0) {xbeam_=x; ybeam_=y;}

 private:
  InttRawData* _rawModule;

  std::string fname_;
  TFile* anafile_;
  TH1*   h_dca2d_zero; 
  TH2*   h2_dca2d_zero; 
  TH2*   h2_dca2d_len; 
  TH1*   h_zvtx;
  TH1*   h_eta;
  TH1*   h_phi;
  TH1*   h_theta;
  TNtuple*  h_ntp_clus; 
  TNtuple*  h_ntp_emcclus; 
  //TTree*  h_t_clus; 
  TNtuple*  h_ntp_cluspair; 
  TNtuple*  h_ntp_emccluspair{nullptr}; 
  TNtuple*  h_ntp_evt; 
  TTree*  h_t_evt_bco; 

  float xbeam_;
  float ybeam_;
  double xvtx_sim = -9999;
  double yvtx_sim = -9999;
  double zvtx_sim = -9999;

  evtbco_info m_evtbcoinfo;
  evtbco_info m_evtbcoinfo_prev;

  TH1* h_zvtxseed_;
  TTree *m_hepmctree;

  /// HEPMC Tree variables
  int m_evt=0;
  double m_xvtx = -9999;
  double m_yvtx = -9999;
  double m_zvtx = -9999;
  int m_partid1 = -9999;
  int m_partid2 = -9999;
  double m_x1 = -9999;
  double m_x2 = -9999;
  int m_mpi = -9999;
  int m_process_id = -9999;
  double m_truthenergy = -9999;
  double m_trutheta = -9999;
  double m_truththeta = -9999;
  double m_truthphi = -9999;
  int m_status = -9999;
  double m_truthpx = -9999;
  double m_truthpy = -9999;
  double m_truthpz = -9999;
  double m_truthpt = -9999;
  double m_truthp = -9999;
  int m_numparticlesinevent = -9999;
  int m_truthpid = -9999;
  double m_vertex = -9999;
  double vertex_[10][3]{ {-9999} };
  TVector3* intt_vertex_pos_;

  double mbdqn_ = 0;
  double mbdqs_ = 0;
  double mbdz_ = 0;
  uint64_t bco_ = 0;
  int evtseq_ = -1;
  double vtx_sim_[3]{-9999, -9999, -9999};
  int nclusmvtx_[3] = {0, 0, 0};
  int nclusadd_ = 0;
  int nclusadd2_ = 0;
  int nclus_inner_ = 0;
  int nclus_outer_ = 0;
  int nemc_ = -9999;
  int nemc1_ = -9999;
  double zvtx_ = -9999;
  static int evtCount;
  static int ievt;
  std::vector < ClustInfo > clusters_[2]; // inner=0; outer=1
  
  void InitHists();
  void InitTrees();
  void InitTuples();

  int GetNodes( PHCompositeNode *topNode );
  ActsGeometry *m_tGeometry;
  PHHepMCGenEventMap *hepmceventmap;
  PHG4InEvent *phg4inevent;
  TrkrClusterContainer *m_clusterMap;
  GlobalVertexMap *vertices;
  InttEventInfo *inttevthead;
  SvtxVertexMap *svtxvertexmap;
  MbdOut *mbdout;
  SvtxVertex *svtxvertex;
  //InttVertex3DMap *inttvertexmap;
  InttVertexMap *intt_vertex_map;
  SvtxTrackMap *svtxtrackmap;
  
  //Gl1RawHit *gl1raw_;
  Gl1Packet *gl1raw_;
  InttRawHit *inttraw_;
  //InttVertex3D *zvtxobj_;
  InttVertex *zvtxobj_;
  
  int process_event_gl1(PHCompositeNode *topNode );

  int process_event_mbd(PHCompositeNode *topNode );
  int process_event_global_vertex(PHCompositeNode *topNode );
  int process_event_svtx_vertex(PHCompositeNode *topNode );
  
  int process_event_intt_raw(PHCompositeNode *topNode );
  int process_event_intt_vertex(PHCompositeNode *topNode );
  int process_event_intt_cluster(PHCompositeNode *topNode );
  int process_event_intt_cluster_pair(PHCompositeNode *topNode );
  int process_event_mvtx(PHCompositeNode *topNode );
  int process_event_emcal(PHCompositeNode *topNode );
  int process_event_fill(PHCompositeNode *topNode );

};

#endif
