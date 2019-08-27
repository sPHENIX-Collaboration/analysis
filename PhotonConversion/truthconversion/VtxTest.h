/**
 * @VtxTest.h
 * @author Francesco Vassalli <Francesco.Vassalli@colorado.edu>
 * @version 1.0
 *
 * @section Uses truth particle information to find photon conversions. 
 * Infomation about the conversions is recored in a TTree.
 * Finally they are associated with clusters for latter analysis
 */
#ifndef TRUTHCONVERSIONEVAL_H__
#define TRUTHCONVERSIONEVAL_H__

#include <fun4all/SubsysReco.h>
#include <calobase/RawClusterContainer.h>
#include <queue>

class PHCompositeNode;
class PHG4TruthInfoContainer;
class PHG4Particle;
class PHG4VtxPoint;
class Conversion;
class SvtxTrackEval;
class SvtxHitMap;
class SvtxHit;
class SvtxClusterMap;
class SvtxCluster;
class RawClusterContainer;
class TTree;
class TFile;
class SVReco;
class TrkrClusterContainer;

class VtxTest: public SubsysReco
{

  public:
    /**
     *
     * @param name name of the output file
     * @param runnumber printed in TTree for condor jobs
     * @param particleEmbed the embedID of particles embeded by Fun4All
     * @param pythiaEmbed the embedID for an embeded pythia event
     * @param makeTTree true-writes the TTree false-won't make TTree will still find clusters
     */
    VtxTest(const std::string &name,unsigned int runnumber, 
        int particleEmbed, int pythiaEmbed,bool makeTTree);
    ~VtxTest();
    int InitRun(PHCompositeNode*);
    /**
     * Find the conversions pass them to numUnique.
     * Fill the TTree
     * @return event status */
    int process_event(PHCompositeNode*);
    int End(PHCompositeNode*);
    /** get the clusters associated with converions*/
    const RawClusterContainer* getClusters()const;

  private:
    void doNodePointers(PHCompositeNode* topNode);
    /** helper function for process_event
     * fills the member fields with information from the conversions 
     * finds the clusters associated with the conversions
     * @return currently will return nothing 
     * but can easily be changed to return a structure for the converions with only 1 truth associated track*/
    std::queue<std::pair<int,int>> numUnique(std::map<int,Conversion>* map,SvtxTrackEval* trackEval,RawClusterContainer* mainClusterContainer);
    /** attempts to find other truth associated tracks for conversions with only 1 truth associated track*/
    void findChildren(std::queue<std::pair<int,int>> missing,PHG4TruthInfoContainer* truthinfo);
    /** @param map should contain Conversion objects which hold background events i.e. not conversions
     * fills the fields for {@link _backgroundCutTree*/
    void processBackground(std::map<int,Conversion>* map,SvtxTrackEval* trackEval,TTree* tree);

    int get_embed(PHG4Particle* particle, PHG4TruthInfoContainer* truthinfo) const;
    float vtoR(PHG4VtxPoint* vtx)const;

    const static int s_kMAXParticles=200; ///< increase this number if arrays go out of bounds
    const static int s_kMAXRecoMatch=20; ///< increase this number if arrays go out of bounds
    const unsigned int _kRunNumber;
    const int _kParticleEmbed; ///< primary embedID
    const int _kPythiaEmbed; ///< background event embedID i.e. pythia or AA
    const bool _kMakeTTree;//< if false no TTrees are output
    int _runNumber; ///<for the TTree do not change
    TFile *_f=NULL; ///< output file
    TTree *_tree=NULL; ///< stores most of the data about the conversions
    TTree *_signalCutTree=NULL; ///<signal data for making track pair cuts
    TTree *_h_backgroundCutTree=NULL; ///<hadronic background data for making track pair cuts
    TTree *_e_backgroundCutTree=NULL; ///<EM background data for making track pair cuts
    RawClusterContainer *_mainClusterContainer; ///< contain 1 cluster associated with each conversion
    PHG4TruthInfoContainer *_truthinfo;
    TrkrClusterContainer* _clusterMap;
    SvtxHitMap *_hitMap;
    std::string _foutname; ///< name of the output file
    SVReco *_vertexer=NULL; ///< for reco vertex finding
    /** \defgroup mainTreeVars Variables for {@link _tree}
      @{*/
    int _b_event;
    int _b_nVtx;  ///<total conversions
    int _b_Tpair; ///<count acceptance e pairs in truth
    int _b_Rpair; ///<count acceptance e pairs in reco
    double _b_rVtx[s_kMAXParticles];  ///<truth conversion radius used for the signal tree
    bool _b_pythia[s_kMAXParticles];  ///<record if the conversion is from pythia or G4 particle
    float _b_electron_pt[s_kMAXParticles];
    float _b_positron_pt[s_kMAXParticles];
    float _b_electron_reco_pt[s_kMAXParticles];
    float _b_positron_reco_pt[s_kMAXParticles];
    float _b_e_deta[s_kMAXParticles];
    float _b_e_dphi[s_kMAXParticles];
    float _b_parent_pt  [s_kMAXParticles];
    float _b_parent_eta [s_kMAXParticles];
    float _b_parent_phi [s_kMAXParticles];
    int   _b_grandparent_id [s_kMAXParticles]; ///<pid of the source of the photon 0 for prompt
    /** # of clusters associated with each conversion that has 2 reco tracks
    * 1 indicates the reco tracks go to the same cluster ~15% of conversions*/
    int   _b_nCluster [s_kMAXRecoMatch]; 
    int   _b_fLayer [s_kMAXRecoMatch]; 
    float _b_cluster_dphi [s_kMAXRecoMatch];
    float _b_cluster_deta [s_kMAXRecoMatch];
    float _b_Mcluster_prob[s_kMAXRecoMatch]; ///<cluster prob for merged clusters
    float _b_Scluster_prob[s_kMAXRecoMatch]; ///<cluster prob for split clusters
    /**@}*/
    /** \defgroup signalTreeVars Variables for {@link _signalCutTree}
      @{*/
    float _b_track_deta ;
    int _b_track_layer ;
    int _b_track_dlayer ;
    float _b_track_pT;
    float _b_ttrack_pT;
    double _b_approach  ;
    double _b_vtx_radius ;
    double _b_vtx_phi ;
    double _b_vtx_eta ;
    double _b_vtx_x ;
    double _b_vtx_y ;
    double _b_vtx_z ;
    double _b_tvtx_eta ;
    double _b_tvtx_x ;
    double _b_tvtx_y ;
    double _b_tvtx_z ;
    double _b_tvtx_radius ;
    double _b_tvtx_phi ;
    double _b_vtxTrack_dist;
    float _b_vtx_chi2;
    float _b_photon_m;
    float _b_photon_pT;
    float _b_cluster_prob;
    float _b_track_dphi;
    /**@}*/
    /** \defgroup backTreeVars Variables for {@link _signalCutTree}
      @{*/
    //bb stands for background branch
    float _bb_track_deta ;
    int _bb_track_layer ;
    int _bb_track_dlayer ;
    float _bb_track_pT;
    double _bb_approach ;
    double _bb_vtx_radius;
    double _bb_vtxTrack_dist;
    float _bb_vtx_chi2;
    float _bb_photon_m;
    float _bb_photon_pT;
    float _bb_cluster_prob;
    float _bb_track_dphi;
    int _bb_pid;
    /**@}*/
    /** RawClusters associated with truth conversions
     * processed by other modules*/
    RawClusterContainer _conversionClusters;

    const static int s_kTPCRADIUS=21; //in cm there is a way to get this from the simulation I should implement?
    ///<TPC radius currently hardcoded
    float _kRAPIDITYACCEPT=1; //<acceptance rapidity currently hard coded to |1|
};



#endif // __TRUTHCONVERSIONEVAL_H__



