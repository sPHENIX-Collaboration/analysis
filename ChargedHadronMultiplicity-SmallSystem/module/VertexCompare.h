// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef VERTEXCOMPARE_H
#define VERTEXCOMPARE_H

#include <fun4all/SubsysReco.h>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <math.h>
#include <string>

#include <TFile.h>
#include <TTree.h>
#include <TVector3.h>
#include <TMath.h>
#include <TH1.h>
#include <TH2.h>
#include <Math/Util.h>
#include <Math/Vector4D.h>

class PHCompositeNode;

class TrkrClusterContainer;
class TrkrClusterHitAssoc;
class TrkrClusterCrossingAssoc;
class ActsGeometry;
class TrackSeedContainer;
class SvtxVertexMap;
class SvtxTrackMap;
class SvtxPHG4ParticleMap;
class PHG4TpcGeomContainer;
class Gl1Packet;

class CentralityInfo;

class MbdPmtHit;
class MbdOut;
class MbdPmtContainer;
class MinimumBiasInfo;

class PHG4TruthInfoContainer;
class PHG4Particle;
class SvtxEvalStack;
class SvtxTruthEval;
class SvtxClusterEval;
class SvtxHitEval;

using namespace ROOT::Math;

namespace VertexCompareVerbosity
{
    inline int fillSilconSeed = 0;
    inline int fillCluster = 0;
    inline int fillTruthParticle = 0;
}

class VertexCompare : public SubsysReco
{
  public:
    VertexCompare(const std::string &name = "VertexCompare");

    ~VertexCompare() override;

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

    void setOutputName(std::string name) { outFileName = name; };

    // set the flag for simulation
    void IsSimulation() { isSimulation = true; };

    // a setter for simulation for truth matching
    void doTruthMatching() { doTruthMatching_ = true; };
    void doTrackOutput() { writeTrackBranches_ = true; };
    void doTpcSeedOutput() { writeTpcSeedBranches_ = true; };
    void doTruthOnlyOutput() { truthOnlyOutput_ = true; IsSimulation(); };
    void SaveClustersOnTrack(bool saveClustersOnTrack) { _saveClustersOnTrack = saveClustersOnTrack; };

  private:
    void FillTrackTree();
    void FillSiliconSeedTree();
    void FillTpcSeedTree();
    void FillClusterTree();
    void FillTruthParticleTree();
    void Cleanup();

    // flag for simulation
    bool isSimulation = false;
    bool doTruthMatching_ = false;
    bool writeTrackBranches_ = false;
    bool writeTpcSeedBranches_ = false;
    bool truthOnlyOutput_ = false;
    bool _saveClustersOnTrack = false;

    TFile *outFile = nullptr;
    TTree *outTree = nullptr;
    std::string outFileName = "outputVTX.root";

    std::string clusterContainerName = "TRKR_CLUSTER";
    std::string clusterHitAssocName = "TRKR_CLUSTERHITASSOC";
    std::string geometryNodeName = "ActsGeometry";
    std::string svtxvertexmapName = "SvtxVertexMap";
    std::string svtxTrackMapName = "SvtxTrackMap";
    std::string seedContainerName = "SiliconTrackSeedContainer";
    std::string tpcSeedContainerName = "TpcTrackSeedContainer";
    std::string svtxPHG4ParticleMapName = "SvtxPHG4ParticleMap";
    std::string gl1NodeName = "GL1RAWHIT";
    std::string mbdOutNodeName = "MbdOut";

    TrkrClusterContainer *clustermap = nullptr;
    TrkrClusterHitAssoc *clusterhitassoc = nullptr;
    TrkrClusterCrossingAssoc *clustercrossingassoc = nullptr;
    ActsGeometry *geometry = nullptr;
    PHG4TpcGeomContainer *tpcgeom = nullptr;
    TrackSeedContainer *silseedmap = nullptr;
    TrackSeedContainer *tpcseedmap = nullptr;
    SvtxTrackMap *svtxTrackMap = nullptr;
    SvtxPHG4ParticleMap *svtxPHG4ParticleMap = nullptr;
    Gl1Packet *gl1PacketInfo = nullptr;
    MbdOut *m_mbdout = nullptr;
    MinimumBiasInfo *minimumbiasinfo = nullptr;
    CentralityInfo *m_CentInfo = nullptr;

    // for truth information
    PHG4TruthInfoContainer *m_truth_info = nullptr;
    SvtxEvalStack *svtx_evalstack = nullptr;
    SvtxTruthEval *truth_eval = nullptr;
    SvtxClusterEval *clustereval = nullptr;
    SvtxHitEval *hiteval = nullptr;

    // float mbdVertex{std::numeric_limits<float>::quiet_NaN()};
    std::vector<float> mbdVertex = std::vector<float>();
    std::vector<int> mbdVertexId = std::vector<int>();
    std::vector<short int> mbdVertexCrossing = std::vector<short int>();
    float MBD_charge_sum{std::numeric_limits<float>::quiet_NaN()};
    // float trackerVertexX{std::numeric_limits<float>::quiet_NaN()};
    // float trackerVertexY{std::numeric_limits<float>::quiet_NaN()};
    // float trackerVertexZ{std::numeric_limits<float>::quiet_NaN()};
    // float trackerVertexChisq{std::numeric_limits<float>::quiet_NaN()};
    // int trackerVertexNdof{std::numeric_limits<int>::quiet_NaN()};
    int nSvtxVertices{std::numeric_limits<int>::quiet_NaN()};
    int nSvtxVertices_validCrossing{std::numeric_limits<int>::quiet_NaN()};
    std::vector<int> trackerVertexId = std::vector<int>();
    std::vector<float> trackerVertexX = std::vector<float>();
    std::vector<float> trackerVertexY = std::vector<float>();
    std::vector<float> trackerVertexZ = std::vector<float>();
    std::vector<float> trackerVertexChisq = std::vector<float>();
    std::vector<int> trackerVertexNdof = std::vector<int>();
    std::vector<int> trackerVertexNTracks = std::vector<int>();
    std::vector<std::vector<int>> trackerVertexTrackIDs = std::vector<std::vector<int>>();
    std::vector<short int> trackerVertexCrossing = std::vector<short int>();

    UInt_t nTracks{std::numeric_limits<unsigned int>::quiet_NaN()};
    UInt_t n_MBDVertex{std::numeric_limits<unsigned int>::quiet_NaN()};
    UInt_t n_TRKVertex{std::numeric_limits<unsigned int>::quiet_NaN()};

    bool hasMBD{false};
    bool hasTRK{false};

    int counter = 1; // initialize to 1 corresponding to event number in Fun4All

    bool is_min_bias = false;
    std::vector<int> firedTriggers = std::vector<int>();
    uint64_t gl1bco = std::numeric_limits<uint64_t>::quiet_NaN();
    uint64_t gl1BunchCrossing = std::numeric_limits<uint64_t>::quiet_NaN();
    uint64_t bcotr = std::numeric_limits<uint64_t>::quiet_NaN();
    // centrality
    float centrality_mbd_;

    // truth vertex information
    int nTruthVertex = 0; // total number of primary truth vertices
    // coordinates for primary truth vertices with isEmbededVtx == 0 (dNdEtaINTT-style trigger vertex selection)
    std::vector<int> TruthVertex_isEmbeded = std::vector<int>();
    std::vector<float> TruthVertexX = std::vector<float>();
    std::vector<float> TruthVertexY = std::vector<float>();
    std::vector<float> TruthVertexZ = std::vector<float>();
    std::vector<float> TruthVertexT = std::vector<float>();
    std::vector<int> TruthVertex_crossing = std::vector<int>();

    // full reconstructed track information
    int nRecoTracks = 0;
    std::vector<float> track_deltapt = std::vector<float>();
    std::vector<float> track_deltaeta = std::vector<float>();
    std::vector<float> track_deltaphi = std::vector<float>();
    std::vector<int> track_nhits = std::vector<int>();
    std::vector<int> track_nmaps = std::vector<int>();
    std::vector<int> track_nintt = std::vector<int>();
    std::vector<int> track_ntpc = std::vector<int>();
    std::vector<int> track_nmms = std::vector<int>();
    std::vector<int> track_ntpc1 = std::vector<int>();
    std::vector<int> track_ntpc11 = std::vector<int>();
    std::vector<int> track_ntpc2 = std::vector<int>();
    std::vector<int> track_ntpc3 = std::vector<int>();
    std::vector<float> track_pidedx = std::vector<float>();
    std::vector<float> track_kdedx = std::vector<float>();
    std::vector<float> track_prdedx = std::vector<float>();
    std::vector<float> track_vx = std::vector<float>();
    std::vector<float> track_vy = std::vector<float>();
    std::vector<float> track_vz = std::vector<float>();
    std::vector<float> track_dca2d = std::vector<float>();
    std::vector<float> track_dca2dsigma = std::vector<float>();
    std::vector<float> track_dca3dxy = std::vector<float>();
    std::vector<float> track_dca3dxysigma = std::vector<float>();
    std::vector<float> track_dca3dz = std::vector<float>();
    std::vector<float> track_dca3dzsigma = std::vector<float>();
    std::vector<float> track_hlxpt = std::vector<float>();
    std::vector<float> track_hlxeta = std::vector<float>();
    std::vector<float> track_hlxphi = std::vector<float>();
    std::vector<float> track_hlxX0 = std::vector<float>();
    std::vector<float> track_hlxY0 = std::vector<float>();
    std::vector<float> track_hlxZ0 = std::vector<float>();
    std::vector<int> track_hlxcharge = std::vector<int>();
    std::vector<unsigned int> track_id = std::vector<unsigned int>();
    std::vector<float> track_x = std::vector<float>();
    std::vector<float> track_y = std::vector<float>();
    std::vector<float> track_z = std::vector<float>();
    std::vector<float> track_px = std::vector<float>();
    std::vector<float> track_py = std::vector<float>();
    std::vector<float> track_pz = std::vector<float>();
    std::vector<float> track_pt = std::vector<float>();
    std::vector<float> track_eta = std::vector<float>();
    std::vector<float> track_phi = std::vector<float>();
    std::vector<float> track_dedx = std::vector<float>();
    std::vector<int> track_charge = std::vector<int>();
    std::vector<int> track_crossing = std::vector<int>();
    std::vector<int> track_vertex_id = std::vector<int>();
    std::vector<float> track_chisq = std::vector<float>();
    std::vector<int> track_ndf = std::vector<int>();
    std::vector<float> track_quality = std::vector<float>();
    std::vector<int> track_silseed_id = std::vector<int>();
    std::vector<float> track_silseed_x = std::vector<float>();
    std::vector<float> track_silseed_y = std::vector<float>();
    std::vector<float> track_silseed_z = std::vector<float>();
    std::vector<float> track_silseed_pt = std::vector<float>();
    std::vector<float> track_silseed_eta = std::vector<float>();
    std::vector<float> track_silseed_phi = std::vector<float>();
    std::vector<int> track_silseed_crossing = std::vector<int>();
    std::vector<int> track_silseed_charge = std::vector<int>();
    std::vector<int> track_silseed_nMvtx = std::vector<int>();
    std::vector<int> track_silseed_nIntt = std::vector<int>();
    std::vector<std::vector<uint64_t>> track_silseed_clusterKeys = std::vector<std::vector<uint64_t>>();
    // branches for clusters on track
    std::vector<std::vector<unsigned int>> track_cluster_layer = std::vector<std::vector<unsigned int>>();
    std::vector<std::vector<float>> track_cluster_globalX = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> track_cluster_globalY = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> track_cluster_globalZ = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> track_cluster_phi = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> track_cluster_eta = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> track_cluster_r = std::vector<std::vector<float>>();

    // silicon seed information
    int nTotalSilSeeds = 0;
    int nSilSeedsValidCrossing = 0;
    std::vector<unsigned int> silseed_id = std::vector<unsigned int>();
    std::vector<int> silseed_assocVtxId = std::vector<int>();
    std::vector<float> silseed_x = std::vector<float>();
    std::vector<float> silseed_y = std::vector<float>();
    std::vector<float> silseed_z = std::vector<float>();
    std::vector<float> silseed_pt = std::vector<float>();
    std::vector<float> silseed_eta = std::vector<float>();
    std::vector<float> silseed_phi = std::vector<float>();
    std::vector<float> silseed_eta_vtx = std::vector<float>(); // with respect to the matched same-crossing vertex
    std::vector<float> silseed_phi_vtx = std::vector<float>(); // with respect to the matched same-crossing vertex
    std::vector<int> silseed_crossing = std::vector<int>();
    std::vector<int> silseed_charge = std::vector<int>();
    std::vector<int> silseed_nMvtx = std::vector<int>();
    std::vector<int> silseed_nIntt = std::vector<int>();
    std::vector<std::vector<uint64_t>> silseed_clusterKeys = std::vector<std::vector<uint64_t>>();
    std::vector<std::vector<unsigned int>> silseed_cluster_layer = std::vector<std::vector<unsigned int>>();
    std::vector<std::vector<float>> silseed_cluster_globalX = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> silseed_cluster_globalY = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> silseed_cluster_globalZ = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> silseed_cluster_phi = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> silseed_cluster_eta = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> silseed_cluster_r = std::vector<std::vector<float>>();
    std::vector<std::vector<int>> silseed_cluster_phiSize = std::vector<std::vector<int>>();
    std::vector<std::vector<int>> silseed_cluster_zSize = std::vector<std::vector<int>>();
    std::vector<std::vector<int>> silseed_cluster_strobeID = std::vector<std::vector<int>>();
    std::vector<std::vector<int>> silseed_cluster_timeBucketID = std::vector<std::vector<int>>();
    // seed cluster truth matching
    std::vector<int> silseed_ngmvtx = std::vector<int>();
    std::vector<int> silseed_ngintt = std::vector<int>();
    std::vector<std::vector<uint64_t>> silseed_cluster_gcluster_key = std::vector<std::vector<uint64_t>>();
    std::vector<std::vector<unsigned int>> silseed_cluster_gcluster_layer = std::vector<std::vector<unsigned int>>();
    std::vector<std::vector<float>> silseed_cluster_gcluster_X = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> silseed_cluster_gcluster_Y = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> silseed_cluster_gcluster_Z = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> silseed_cluster_gcluster_r = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> silseed_cluster_gcluster_phi = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> silseed_cluster_gcluster_eta = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> silseed_cluster_gcluster_edep = std::vector<std::vector<float>>();
    std::vector<std::vector<int>> silseed_cluster_gcluster_adc = std::vector<std::vector<int>>();
    std::vector<std::vector<float>> silseed_cluster_gcluster_phiSize = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> silseed_cluster_gcluster_zSize = std::vector<std::vector<float>>();
    bool hasSvtxPHG4ParticleMap = false;
    bool svtxPHG4ParticleMapProcessed = false;
    std::vector<int> silseed_f4a_nMatched = std::vector<int>();
    std::vector<std::vector<int>> silseed_f4a_truthTrackID = std::vector<std::vector<int>>();
    std::vector<std::vector<float>> silseed_f4a_truthWeight = std::vector<std::vector<float>>();
    std::vector<int> silseed_f4a_bestTrackID = std::vector<int>();
    std::vector<float> silseed_f4a_bestWeight = std::vector<float>();
    std::vector<int> silseed_f4a_bestG4P_PID = std::vector<int>();
    std::vector<float> silseed_f4a_bestG4P_E = std::vector<float>();
    std::vector<float> silseed_f4a_bestG4P_pT = std::vector<float>();
    std::vector<float> silseed_f4a_bestG4P_eta = std::vector<float>();
    std::vector<float> silseed_f4a_bestG4P_phi = std::vector<float>();
    std::vector<std::vector<int>> silseed_f4a_bestG4P_ancestor_trackID = std::vector<std::vector<int>>();
    std::vector<std::vector<int>> silseed_f4a_bestG4P_ancestor_PID = std::vector<std::vector<int>>();

    // tpc seed information
    int nTotalTpcSeeds = 0;
    std::vector<unsigned int> tpcseed_id = std::vector<unsigned int>();
    std::vector<float> tpcseed_x = std::vector<float>();
    std::vector<float> tpcseed_y = std::vector<float>();
    std::vector<float> tpcseed_z = std::vector<float>();
    std::vector<float> tpcseed_pt = std::vector<float>();
    std::vector<float> tpcseed_eta = std::vector<float>();
    std::vector<float> tpcseed_phi = std::vector<float>();
    std::vector<int> tpcseed_crossing = std::vector<int>();
    std::vector<int> tpcseed_crossing_estimate = std::vector<int>();
    std::vector<int> tpcseed_charge = std::vector<int>();
    std::vector<int> tpcseed_nTpc = std::vector<int>();
    std::vector<int> tpcseed_nMms = std::vector<int>();
    std::vector<float> tpcseed_dedx = std::vector<float>();
    std::vector<std::vector<uint64_t>> tpcseed_clusterKeys = std::vector<std::vector<uint64_t>>();
    std::vector<std::vector<unsigned int>> tpcseed_cluster_layer = std::vector<std::vector<unsigned int>>();
    std::vector<std::vector<float>> tpcseed_cluster_globalX = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> tpcseed_cluster_globalY = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> tpcseed_cluster_globalZ = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> tpcseed_cluster_phi = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> tpcseed_cluster_eta = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> tpcseed_cluster_r = std::vector<std::vector<float>>();

    // (intt) cluster information
    std::vector<uint64_t> clusterKey = std::vector<uint64_t>();
    std::vector<unsigned int> cluster_layer = std::vector<unsigned int>();
    std::vector<int> cluster_chip = std::vector<int>();        // for mvtx chip id
    std::vector<int> cluster_stave = std::vector<int>();       // for mvtx stave id
    std::vector<int> cluster_ladderZId = std::vector<int>();   // for intt ladder z id
    std::vector<int> cluster_ladderPhiId = std::vector<int>(); // for intt ladder phi id
    std::vector<float> cluster_globalX = std::vector<float>();
    std::vector<float> cluster_globalY = std::vector<float>();
    std::vector<float> cluster_globalZ = std::vector<float>();
    std::vector<float> cluster_phi = std::vector<float>();
    std::vector<float> cluster_eta = std::vector<float>();
    std::vector<float> cluster_r = std::vector<float>();
    std::vector<int> cluster_phiSize = std::vector<int>();
    std::vector<int> cluster_zSize = std::vector<int>();
    std::vector<int> cluster_adc = std::vector<int>();
    std::vector<int> cluster_timeBucketID = std::vector<int>();
    std::vector<int> cluster_crossing = std::vector<int>();
    std::vector<float> cluster_LocalX = std::vector<float>();
    std::vector<float> cluster_LocalY = std::vector<float>();
    // truth matching for (INTT) clusters by max_truth_particle_by_energy()
    std::vector<int> cluster_matchedG4P_trackID = std::vector<int>();
    std::vector<int> cluster_matchedG4P_PID = std::vector<int>();
    std::vector<float> cluster_matchedG4P_E = std::vector<float>();
    std::vector<float> cluster_matchedG4P_pT = std::vector<float>();
    std::vector<float> cluster_matchedG4P_eta = std::vector<float>();
    std::vector<float> cluster_matchedG4P_phi = std::vector<float>();

    // MVTX clusters on seeds (just flattend per event/trigger frame)
    std::vector<uint64_t> mvtx_seedcluster_key = std::vector<uint64_t>();
    std::vector<unsigned int> mvtx_seedcluster_layer = std::vector<unsigned int>();
    std::vector<int> mvtx_seedcluster_chip = std::vector<int>();  // mvtx chip id
    std::vector<int> mvtx_seedcluster_stave = std::vector<int>(); // mvtx stave id
    std::vector<float> mvtx_seedcluster_globalX = std::vector<float>();
    std::vector<float> mvtx_seedcluster_globalY = std::vector<float>();
    std::vector<float> mvtx_seedcluster_globalZ = std::vector<float>();
    std::vector<float> mvtx_seedcluster_phi = std::vector<float>();
    std::vector<float> mvtx_seedcluster_eta = std::vector<float>();
    std::vector<float> mvtx_seedcluster_r = std::vector<float>();
    std::vector<int> mvtx_seedcluster_phiSize = std::vector<int>();
    std::vector<int> mvtx_seedcluster_zSize = std::vector<int>();
    std::vector<int> mvtx_seedcluster_strobeID = std::vector<int>();
    std::vector<int> mvtx_seedcluster_matchedcrossing = std::vector<int>();
    std::vector<std::vector<float>> mvtx_seedcluster_hitX = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> mvtx_seedcluster_hitY = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> mvtx_seedcluster_hitZ = std::vector<std::vector<float>>();
    std::vector<std::vector<int>> mvtx_seedcluster_hitrow = std::vector<std::vector<int>>();
    std::vector<std::vector<int>> mvtx_seedcluster_hitcol = std::vector<std::vector<int>>();
    // truth info
    // PHG4Particle matching: 1) SvtxClusterEval::max_truth_cluster_by_energy, and 2) max_truth_particle_by_cluster_energy
    std::vector<int> mvtx_seedcluster_matchedG4P_trackID = std::vector<int>();
    std::vector<int> mvtx_seedcluster_matchedG4P_PID = std::vector<int>();
    std::vector<float> mvtx_seedcluster_matchedG4P_E = std::vector<float>();
    std::vector<float> mvtx_seedcluster_matchedG4P_pT = std::vector<float>();
    std::vector<float> mvtx_seedcluster_matchedG4P_eta = std::vector<float>();
    std::vector<float> mvtx_seedcluster_matchedG4P_phi = std::vector<float>();
    // particle ancestor
    std::vector<std::vector<int>> mvtx_seedcluster_matchedG4P_ancestor_trackID = std::vector<std::vector<int>>();
    std::vector<std::vector<int>> mvtx_seedcluster_matchedG4P_ancestor_PID = std::vector<std::vector<int>>();

    // Truth particle Tree and branches
    int N_PrimaryPHG4Ptcl = 0;
    int N_sPHENIXPrimary = 0;
    // primary particle
    std::vector<float> PrimaryPHG4Ptcl_pT = std::vector<float>();
    std::vector<float> PrimaryPHG4Ptcl_eta = std::vector<float>();
    std::vector<float> PrimaryPHG4Ptcl_phi = std::vector<float>();
    std::vector<float> PrimaryPHG4Ptcl_E = std::vector<float>();
    std::vector<int> PrimaryPHG4Ptcl_PID = std::vector<int>();
    std::vector<int> PrimaryPHG4Ptcl_trackID = std::vector<int>();
    // the original collision ancestor of this truth particle
    std::vector<int> PrimaryPHG4Ptcl_originTrackID = std::vector<int>();
    std::vector<int> PrimaryPHG4Ptcl_originVtxID = std::vector<int>();
    std::vector<float> PrimaryPHG4Ptcl_originVtxT = std::vector<float>();
    std::vector<int> PrimaryPHG4Ptcl_originCrossing = std::vector<int>();
    std::vector<int> PrimaryPHG4Ptcl_originIsEmbeded = std::vector<int>();
    std::vector<TString> PrimaryPHG4Ptcl_ParticleClass = std::vector<TString>();
    std::vector<bool> PrimaryPHG4Ptcl_isStable = std::vector<bool>();
    std::vector<double> PrimaryPHG4Ptcl_charge = std::vector<double>();
    std::vector<bool> PrimaryPHG4Ptcl_isChargedHadron = std::vector<bool>();
    std::vector<std::vector<int>> PrimaryPHG4Ptcl_ancestor_trackID = std::vector<std::vector<int>>();
    std::vector<std::vector<int>> PrimaryPHG4Ptcl_ancestor_PID = std::vector<std::vector<int>>();
    // sPHENIX primary particle
    std::vector<float> sPHENIXPrimary_pT = std::vector<float>();
    std::vector<float> sPHENIXPrimary_eta = std::vector<float>();
    std::vector<float> sPHENIXPrimary_phi = std::vector<float>();
    std::vector<float> sPHENIXPrimary_E = std::vector<float>();
    std::vector<int> sPHENIXPrimary_PID = std::vector<int>();
    std::vector<int> sPHENIXPrimary_trackID = std::vector<int>();
    std::vector<int> sPHENIXPrimary_originTrackID = std::vector<int>();
    std::vector<int> sPHENIXPrimary_originVtxID = std::vector<int>();
    std::vector<float> sPHENIXPrimary_originVtxT = std::vector<float>();
    std::vector<int> sPHENIXPrimary_originCrossing = std::vector<int>();
    std::vector<int> sPHENIXPrimary_originIsEmbeded = std::vector<int>();
    std::vector<TString> sPHENIXPrimary_ParticleClass = std::vector<TString>();
    std::vector<bool> sPHENIXPrimary_isStable = std::vector<bool>();
    std::vector<double> sPHENIXPrimary_charge = std::vector<double>();
    std::vector<bool> sPHENIXPrimary_isChargedHadron = std::vector<bool>();
    std::vector<std::vector<int>> sPHENIXPrimary_ancestor_trackID = std::vector<std::vector<int>>();
    std::vector<std::vector<int>> sPHENIXPrimary_ancestor_PID = std::vector<std::vector<int>>();
    std::vector<std::vector<float>> sPHENIXPrimary_truthcluster_X = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> sPHENIXPrimary_truthcluster_Y = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> sPHENIXPrimary_truthcluster_Z = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> sPHENIXPrimary_truthcluster_edep = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> sPHENIXPrimary_truthcluster_adc = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> sPHENIXPrimary_truthcluster_r = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> sPHENIXPrimary_truthcluster_phi = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> sPHENIXPrimary_truthcluster_eta = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> sPHENIXPrimary_truthcluster_phisize = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> sPHENIXPrimary_truthcluster_zsize = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> sPHENIXPrimary_recocluster_globalX = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> sPHENIXPrimary_recocluster_globalY = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> sPHENIXPrimary_recocluster_globalZ = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> sPHENIXPrimary_recocluster_r = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> sPHENIXPrimary_recocluster_phi = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> sPHENIXPrimary_recocluster_eta = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> sPHENIXPrimary_recocluster_phisize = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> sPHENIXPrimary_recocluster_zsize = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> sPHENIXPrimary_recocluster_adc = std::vector<std::vector<float>>();
    // all PHG4Particles 
    int N_AllPHG4Ptcl = 0;
    std::vector<float> AllPHG4Ptcl_pT = std::vector<float>();
    std::vector<float> AllPHG4Ptcl_eta = std::vector<float>();
    std::vector<float> AllPHG4Ptcl_phi = std::vector<float>();
    std::vector<float> AllPHG4Ptcl_E = std::vector<float>();
    std::vector<int> AllPHG4Ptcl_PID = std::vector<int>();
    std::vector<int> AllPHG4Ptcl_trackID = std::vector<int>();
    std::vector<int> AllPHG4Ptcl_originTrackID = std::vector<int>();
    std::vector<int> AllPHG4Ptcl_originVtxID = std::vector<int>();
    std::vector<float> AllPHG4Ptcl_originVtxT = std::vector<float>();
    std::vector<int> AllPHG4Ptcl_originCrossing = std::vector<int>();
    std::vector<int> AllPHG4Ptcl_originIsEmbeded = std::vector<int>();
    std::vector<std::vector<int>> AllPHG4Ptcl_ancestor_trackID = std::vector<std::vector<int>>();
    std::vector<std::vector<int>> AllPHG4Ptcl_ancestor_PID = std::vector<std::vector<int>>();
    std::vector<std::vector<float>> AllPHG4Ptcl_truthcluster_X = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> AllPHG4Ptcl_truthcluster_Y = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> AllPHG4Ptcl_truthcluster_Z = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> AllPHG4Ptcl_truthcluster_edep = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> AllPHG4Ptcl_truthcluster_adc = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> AllPHG4Ptcl_truthcluster_r = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> AllPHG4Ptcl_truthcluster_phi = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> AllPHG4Ptcl_truthcluster_eta = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> AllPHG4Ptcl_truthcluster_phisize = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> AllPHG4Ptcl_truthcluster_zsize = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> AllPHG4Ptcl_recocluster_globalX = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> AllPHG4Ptcl_recocluster_globalY = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> AllPHG4Ptcl_recocluster_globalZ = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> AllPHG4Ptcl_recocluster_r = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> AllPHG4Ptcl_recocluster_phi = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> AllPHG4Ptcl_recocluster_eta = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> AllPHG4Ptcl_recocluster_phisize = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> AllPHG4Ptcl_recocluster_zsize = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> AllPHG4Ptcl_recocluster_adc = std::vector<std::vector<float>>();
    // Truth cluster from PHG4Particle (SvtxTruthEval) and the reco cluster matched to the truth cluster (SvtxClusterEval)
    std::vector<std::vector<float>> PrimaryPHG4Ptcl_truthcluster_X = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> PrimaryPHG4Ptcl_truthcluster_Y = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> PrimaryPHG4Ptcl_truthcluster_Z = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> PrimaryPHG4Ptcl_truthcluster_edep = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> PrimaryPHG4Ptcl_truthcluster_adc = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> PrimaryPHG4Ptcl_truthcluster_r = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> PrimaryPHG4Ptcl_truthcluster_phi = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> PrimaryPHG4Ptcl_truthcluster_eta = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> PrimaryPHG4Ptcl_truthcluster_phisize = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> PrimaryPHG4Ptcl_truthcluster_zsize = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> PrimaryPHG4Ptcl_recocluster_globalX = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> PrimaryPHG4Ptcl_recocluster_globalY = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> PrimaryPHG4Ptcl_recocluster_globalZ = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> PrimaryPHG4Ptcl_recocluster_r = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> PrimaryPHG4Ptcl_recocluster_phi = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> PrimaryPHG4Ptcl_recocluster_eta = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> PrimaryPHG4Ptcl_recocluster_phisize = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> PrimaryPHG4Ptcl_recocluster_zsize = std::vector<std::vector<float>>();
    std::vector<std::vector<float>> PrimaryPHG4Ptcl_recocluster_adc = std::vector<std::vector<float>>();
};

#endif // VERTEXCOMPARE_H
