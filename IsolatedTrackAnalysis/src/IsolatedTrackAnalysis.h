// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef ISOLATEDTRACKANALYSIS_H
#define ISOLATEDTRACKANALYSIS_H


#include <fun4all/SubsysReco.h>
#include <centrality/CentralityInfo.h>

#include <string>
#include <vector>

class TFile;
class TTree;
class PHCompositeNode;
class RawClusterContainer;
class RawCluster;
class RawTowerContainer;
class RawTower;
class SvtxTrackMap;
class SvtxTrack;
class SvtxTrackState;
class SvtxVertexMap;
class SvtxEvalStack;
class PHHepMCGenEventMap;
class PHHepMCGenEvent;
class P4G4TruthInfoContainer;
class P4G4TruthInfo;
class CentralityInfo;
class TrkrCluster;
class PHG4HitContainer;

class IsolatedTrackAnalysis : public SubsysReco
{
  public:
    IsolatedTrackAnalysis(const std::string &name = "IsolatedTrackAnalysis",
                          const std::string &fname = "IsolatedTrackAnalysis.root");
    ~IsolatedTrackAnalysis() override;

    int Init(PHCompositeNode *topNode) override;
    int InitRun(PHCompositeNode *topNode) override;
    int process_event(PHCompositeNode *topNode) override;
    int ResetEvent(PHCompositeNode *topNode) override;
    int End(PHCompositeNode *topNode) override;

    void setMinEMClusterEnergy(float minEMClusterEnergy) { m_minEMClusterEnergy = minEMClusterEnergy; }
    void setMinIHClusterEnergy(float minIHClusterEnergy) { m_minIHClusterEnergy = minIHClusterEnergy; }
    void setMinOHClusterEnergy(float minOHClusterEnergy) { m_minOHClusterEnergy = minOHClusterEnergy; }
    void setMinCemcTowerEnergy(float minCemcTowerEnergy) { m_minCemcTowerEnergy = minCemcTowerEnergy; }
    void setMinHcalTowerEnergy(float minHcalTowerEnergy) { m_minHcalTowerEnergy = minHcalTowerEnergy; }
    void setMinSimTowerEnergy(float minSimTowerEnergy) { m_minSimTowerEnergy = minSimTowerEnergy; }

    void analyzeTracks(bool doAnalyzeTracks) { m_analyzeTracks = doAnalyzeTracks; }
    void analyzeClusters(bool doAnalyzeClusters) { m_analyzeClusters = doAnalyzeClusters; }
    void analyzeTowers(bool doAnalyzeTowers) { m_analyzeTowers = doAnalyzeTowers; }
    void analyzeSimTowers(bool doAnalyzeSimTowers) { m_analyzeSimTowers = doAnalyzeSimTowers; }
    void analyzeHepMCTruth(bool doAnalyzeHepMCTruth) { m_analyzeHepMCTruth = doAnalyzeHepMCTruth; }
    void analyzeG4Truth(bool doAnalyzeG4Truth) { m_analyzeG4Truth = doAnalyzeG4Truth; }
    void analyzeCentrality(bool doAnalyzeCentrality) { m_analyzeCentrality = doAnalyzeCentrality; }
    void analyzeAddTruth(bool doAnalyzeAddTruth) { m_analyzeAddTruth = doAnalyzeAddTruth; }

  private:
    /// String to contain the outfile name containing the trees
    std::string m_outputFileName;

    /// A float for cutting on EMCal cluster energy
    float m_minEMClusterEnergy;

    /// A float for cutting on iHCal cluster energy 
    float m_minIHClusterEnergy;

    /// A float for cutting on oHCal cluster energy 
    float m_minOHClusterEnergy;

    /// A float for cutting on EMCal tower energy
    float m_minCemcTowerEnergy;

    /// A float for cutting on HCal tower energy 
    float m_minHcalTowerEnergy;

     /// A float for cutting on sim tower energy
    float m_minSimTowerEnergy;  

    /// A boolean for running over tracks
    bool m_analyzeTracks;

    /// A boolean for running over clusters
    bool m_analyzeClusters;

    /// A boolean for running over towers
    bool m_analyzeTowers;
 
    /// A boolean for running over sim towers
    bool m_analyzeSimTowers;
   
    /// A boolean for collecting hepmc information
    bool m_analyzeHepMCTruth;

    /// A boolean for collecting g4 information
    bool m_analyzeG4Truth;

    /// A boolean for collecting centrality information
    bool m_analyzeCentrality;

    /// A boolean for collecting additional truth information
    /// NOTE: this requires a large about of storage space
    bool m_analyzeAddTruth;

    /// TFile to hold the following TTrees and histograms
    TFile *m_outputFile;
    TTree *m_tracktree;
    TTree *m_clustertree;
    TTree *m_towertree;
    TTree *m_simtowertree;
    TTree *m_hepmctree;
    TTree *m_g4tree;
    TTree *m_centraltree;
    TTree *m_addtruthtree;

    SvtxEvalStack *m_svtxEvalStack = nullptr;
    CentralityInfo *central = nullptr;

    /// Counter
    int counter;

    /// Methods for grabbing the data
    void getTracks(PHCompositeNode *topNode);
    void getClusters(PHCompositeNode *topNode);
    void getTowers(PHCompositeNode *topNode);
    void getSimTowers(PHCompositeNode *topNode);
    void getHepMCTruth(PHCompositeNode *topNode);
    void getG4Truth(PHCompositeNode *topNode);
    void getCentrality(PHCompositeNode *topNode);
    void getAddTruth(PHCompositeNode *topNode);

    void initializeTrees(); 
    void initializeVariables(); 

    // Track helper functions
    float calculateProjectionEta(SvtxTrackState *projectedState);        
    float calculateProjectionPhi(SvtxTrackState *projectedState);        
    void calculateDCA(SvtxTrack* track, SvtxVertexMap* vertexmap, float& dca3dxy, float& dca3dz, float& dca3dxysigma, float& dca3dzsigma);

    // Calorimeter radii
    float m_cemcRadius;
    float m_ihcalRadius;
    float m_ohcalRadius;

    // Multiplicities
    int m_trkmult;
    int m_vtxmult;
    int m_clsmult_cemc;
    int m_clsmult_ihcal;
    int m_clsmult_ohcal;
    int m_twrmult_cemc;
    int m_twrmult_ihcal;
    int m_twrmult_ohcal;
    int m_simtwrmult_cemc;
    int m_simtwrmult_ihcal;
    int m_simtwrmult_ohcal;
    int m_g4;
    int m_hepmc;

    // Basic track properties
    float m_tr_p[2000];
    float m_tr_pt[2000];
    float m_tr_eta[2000];
    float m_tr_phi[2000];
    int m_tr_charge[2000];
    float m_tr_chisq[2000];
    int m_tr_ndf[2000];
    int m_tr_silicon_hits[2000];

    // Distance of closest approach
    float m_tr_dca_xy[2000];
    float m_tr_dca_xy_error[2000];
    float m_tr_dca_z[2000];
    float m_tr_dca_z_error[2000];

    // Initial point of track
    float m_tr_x[2000];
    float m_tr_y[2000];
    float m_tr_z[2000];

    // Vertex id of track
    int m_tr_vertex_id[2000];

    // Vertex ids and positions, also stored on track tree
    int m_vertex_id[100];
    float m_vx[100];
    float m_vy[100];
    float m_vz[100];

    // Projection of track to calorimeters
    // CEMC
    float m_tr_cemc_eta[2000];
    float m_tr_cemc_phi[2000];
    // Inner HCAL
    float m_tr_ihcal_eta[2000];
    float m_tr_ihcal_phi[2000];
    /// Outer HCAL
    float m_tr_ohcal_eta[2000];
    float m_tr_ohcal_phi[2000];

    // Matched truth track
    int m_tr_truth_is_primary[2000];
    int m_tr_truth_pid[2000];
    float m_tr_truth_e[2000];
    float m_tr_truth_pt[2000];
    float m_tr_truth_eta[2000];
    float m_tr_truth_phi[2000];
    int m_tr_truth_track_id[2000];

    //////////////////////////
    // Centrality variables //
    //////////////////////////

    // Event Centrality
    float centrality;

    ///////////////////////
    // Cluster variables //
    ///////////////////////
 
    // CEMC clusters
    float m_cl_cemc_e[5000];
    float m_cl_cemc_eta[5000];
    float m_cl_cemc_phi[5000];
    float m_cl_cemc_r[5000];
    float m_cl_cemc_z[5000];

    // Inner HCAL clusters
    float m_cl_ihcal_e[1000];
    float m_cl_ihcal_eta[1000];
    float m_cl_ihcal_phi[1000];
    float m_cl_ihcal_r[1000];
    float m_cl_ihcal_z[1000];

    // Outer HCAL clusters
    float m_cl_ohcal_e[1000];
    float m_cl_ohcal_eta[1000];
    float m_cl_ohcal_phi[1000];
    float m_cl_ohcal_r[1000];
    float m_cl_ohcal_z[1000];

    /////////////////////
    // Tower variables //
    /////////////////////

    float m_twr_cemc_e[25000];
    float m_twr_cemc_eta[25000];
    float m_twr_cemc_phi[25000];
    int m_twr_cemc_ieta[25000];
    int m_twr_cemc_iphi[25000];

    float m_twr_ihcal_e[2000];
    float m_twr_ihcal_eta[2000];
    float m_twr_ihcal_phi[2000];
    int m_twr_ihcal_ieta[2000];
    int m_twr_ihcal_iphi[2000];

    float m_twr_ohcal_e[2000];
    float m_twr_ohcal_eta[2000];
    float m_twr_ohcal_phi[2000];
    int m_twr_ohcal_ieta[2000];
    int m_twr_ohcal_iphi[2000];

    /////////////////////////
    // Sim tower variables //
    /////////////////////////
    
    float m_simtwr_cemc_e[25000];
    float m_simtwr_cemc_eta[25000];
    float m_simtwr_cemc_phi[25000];
    int m_simtwr_cemc_ieta[25000];
    int m_simtwr_cemc_iphi[25000];

    float m_simtwr_ihcal_e[2000];
    float m_simtwr_ihcal_eta[2000];
    float m_simtwr_ihcal_phi[2000];
    int m_simtwr_ihcal_ieta[2000];
    int m_simtwr_ihcal_iphi[2000];

    float m_simtwr_ohcal_e[2000];
    float m_simtwr_ohcal_eta[2000];
    float m_simtwr_ohcal_phi[2000];
    int m_simtwr_ohcal_ieta[2000];
    int m_simtwr_ohcal_iphi[2000];

    /////////////////////////
    // HepMC particle tree //
    /////////////////////////
  
    int m_hepmc_pid[20000];
    float m_hepmc_e[20000];
    float m_hepmc_pt[20000];
    float m_hepmc_eta[20000];
    float m_hepmc_phi[20000];

    //////////////////////
    // G4 particle tree //
    //////////////////////

    int m_g4_pid[20000];
    float m_g4_e[20000];
    float m_g4_pt[20000];
    float m_g4_eta[20000];
    float m_g4_phi[20000];
    int m_g4_track_id[20000];
    int m_g4_parent_id[20000];

    ///////////////////////////
    // Additional truth info //
    ///////////////////////////

    // Secondary truth particles

    int n_child = 0;
    int child_pid[100000];
    int child_parent_id[100000];
    int child_vertex_id[100000];
    float child_px[100000];
    float child_py[100000];
    float child_pz[100000];
    float child_energy[100000];

    // Truth particle vertices 

    int n_vertex = 0;
    int vertex_id[100000];
    float vertex_x[100000];
    float vertex_y[100000];
    float vertex_z[100000];

    // BH particles

    int _nBH = 0;
    float _BH_e[20000];
    float _BH_px[20000];
    float _BH_py[20000];
    float _BH_pz[20000];
    int _BH_track_id[20000];

};

#endif // ISOLATEDTRACKANALYSIS_H
