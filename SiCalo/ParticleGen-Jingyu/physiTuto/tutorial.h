// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef TUTORIAL_H
#define TUTORIAL_H

#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

#include <trackbase/TrkrClusterv4.h>
#include <trackbase/TrkrClusterContainerv4.h>
#include <trackbase/ActsGeometry.h>
#include <ffaobjects/EventHeaderv1.h>

#include <phool/getClass.h>

#include <globalvertex/GlobalVertexMap.h>
#include <globalvertex/SvtxVertexMap.h>
#include <ffarawobjects/Gl1Packet.h>
#include <trackbase_historic/SvtxPHG4ParticleMap_v1.h>
#include <trackbase_historic/SvtxTrack.h>
#include <trackbase_historic/SvtxTrack_v1.h>
#include <trackbase_historic/SvtxTrack_v2.h>
#include <trackbase_historic/SvtxTrackMap.h>
#include <trackbase_historic/SvtxTrackMap_v1.h>
#include <trackbase_historic/SvtxTrackState_v1.h>
#include <trackbase_historic/TrackSeed.h>

#include <trackbase/TrkrClusterv4.h>
#include <trackbase/TrkrClusterContainerv4.h>
#include <trackbase/TrkrDefs.h>
#include <trackbase/ActsGeometry.h>

#include <ffaobjects/EventHeaderv1.h>

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

#include <g4main/PHG4Hit.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4VtxPoint.h>
#include <g4main/PHG4HitContainer.h>
#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4Shower.h>
#include <g4main/PHG4HitDefs.h>

#include <fun4all/SubsysReco.h>

#include <globalvertex/GlobalVertexMap.h>

#include <vector>
#include <string>
#include <iostream>
#include <iomanip>

#include <TDatabasePDG.h>
#include <TFile.h>
#include <TH1D.h>
#include <TH2.h>
#include <TH2F.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TString.h>
#include <TPad.h>
#include <TLorentzVector.h>

class PHCompositeNode;

class tutorial : public SubsysReco
{
public:

    tutorial(
        const std::string & name_in = "tutorial",
        std::string output_path_in = "",
        std::string output_rootfile_name_in = ""
    );


    ~tutorial() override;

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
    int InitRun(PHCompositeNode * /*topNode*/) override;

    /* Called for each event.
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

    void SetOutputPath( std::string path ){ output_path = path; };

    void setTowerGeomNodeName(const std::string& name)
    {
        m_TowerGeomNodeName = name;
    }
 
private:
    int prepareTracker(PHCompositeNode * topNode);
    int prepareEMCal(PHCompositeNode * topNode);
    int prepareiHCal(PHCompositeNode * topNode);
    int prepareoHCal(PHCompositeNode * topNode);

    int prepareEMCalClus(PHCompositeNode * topNode);
    int prepareiHCalClus(PHCompositeNode * topNode);
    int prepareoHCalClus(PHCompositeNode * topNode);

    int prepareG4Turth(PHCompositeNode * topNode);
    int prepareG4HIT(PHCompositeNode * topNode);

    int createTracksFromTruth(PHCompositeNode* topNode);
    int prepareTruthTrack(PHCompositeNode* topNode);

    bool checkTrack(SvtxTrack* track);

    std::string output_path;
    std::string output_rootfile_name;

    TFile* file_out;
    TTree* tree_out;
    TFile* output;
  

    TrkrClusterContainerv4 * node_cluster_map = nullptr;
    ActsGeometry * node_acts = nullptr;
    
    RawTowerGeomContainer * geomEM = nullptr;
    TowerInfoContainerv1 * EMCal_tower_sim_info = nullptr;
    RawTowerContainer * EMCal_tower_sim = nullptr;
    TowerInfoContainer * EMCal_tower_calib = nullptr;
    
    RawClusterContainer * EMCal_cluster_cont = nullptr;
    RawClusterContainer * EMCal_cluster_innr = nullptr;
    
    RawTowerGeomContainer * geomIH = nullptr;
    RawTowerContainer * iHCal_tower_sim = nullptr;
    TowerInfoContainer * iHCal_tower_calib = nullptr;

    RawClusterContainer * iHCal_cluster_cont = nullptr;
    
    RawTowerGeomContainer * geomOH = nullptr;
    RawTowerContainer * oHCal_tower_sim = nullptr;
    TowerInfoContainer * oHCal_tower_calib = nullptr;

    RawClusterContainer * oHCal_cluster_cont = nullptr;
    
    PHG4TruthInfoContainer *m_truth_info = nullptr;
    PHG4HitContainer *hits_CEMC = nullptr;

    SvtxTrackMap* trackMap = nullptr;
    SvtxTrack *truth_track = nullptr;
    SvtxTrackState *thisState = nullptr;

    long long eventID;

    int NClus;
    std::vector <int> clus_system;
    std::vector <int> clus_layer;
    std::vector <int> clus_adc;
    std::vector <double> clus_X;
    std::vector <double> clus_Y;
    std::vector <double> clus_Z;
    std::vector <int> clus_size;
    std::vector <int> clus_phi_size;
    std::vector <int> clus_z_size;

    int nTowers;
    std::vector <int> tower_system;
    std::vector <double> tower_X;
    std::vector <double> tower_Y;
    std::vector <double> tower_Z;
    std::vector <double> tower_R;
    std::vector <double> tower_Eta;
    std::vector <double> tower_Phi;
    std::vector <double> tower_Eta_test;
    std::vector <double> tower_Phi_test;
    std::vector <double> tower_Eta_bin;
    std::vector <double> tower_Phi_bin;
    std::vector <double> tower_edep;

    std::vector <double> tower_int_X;
    std::vector <double> tower_int_Y;
    std::vector <double> tower_int_Z;
    std::vector <double> tower_int_R;

    int nCaloClus;
    std::vector <int> caloClus_system;
    std::vector <double> caloClus_X;
    std::vector <double> caloClus_Y;
    std::vector <double> caloClus_Z;
    std::vector <double> caloClus_R;
    std::vector <double> caloClus_Phi;
    std::vector <double> caloClus_edep;

    std::vector <double> caloClus_innr_X;
    std::vector <double> caloClus_innr_Y;
    std::vector <double> caloClus_innr_Z;
    std::vector <double> caloClus_innr_R;
    std::vector <double> caloClus_innr_Phi;
    std::vector <double> caloClus_innr_edep;

    // note : Truth primary vertex information
    float TruthPV_trig_x_;
    float TruthPV_trig_y_;
    float TruthPV_trig_z_;
    int NTruthVtx_;

    // note : PHG4 information (from all PHG4Particles)
    int NPrimaryG4P_;
    int NPrimaryG4P_promptChargeHadron_;
    std::vector<float> PrimaryG4P_Pt_;
    std::vector<float> PrimaryG4P_Eta_;
    std::vector<float> PrimaryG4P_Phi_;
    std::vector<float> PrimaryG4P_E_;
    std::vector<int> PrimaryG4P_PID_;
    std::vector<TString> PrimaryG4P_ParticleClass_;
    std::vector<bool> PrimaryG4P_isStable_;
    std::vector<double> PrimaryG4P_Charge_;
    std::vector<bool> PrimaryG4P_isChargeHadron_;

    // note : PHG4hit information
    std::vector<float> _CEMC_Hit_Evis;
    std::vector<float> _CEMC_Hit_Edep;
    std::vector<float> _CEMC_Hit_ch;
    std::vector<float> _CEMC_Hit_x;
    std::vector<float> _CEMC_Hit_y;
    std::vector<float> _CEMC_Hit_z;

    std::vector<float> _CEMC_Pr_Hit_x;
    std::vector<float> _CEMC_Pr_Hit_y;
    std::vector<float> _CEMC_Pr_Hit_z;
    std::vector<float> _CEMC_Pr_Hit_R;
    std::vector<float> _CEMC_Pr_Hit_deltaT;

    std::vector<float> _PrG4_TTPRO_dD;
    std::vector<float> _PrG4_TTPRO_dR;
    std::vector<float> _PrG4_TTPRO_dphi;

    std::set<int> primary_electron_tracks;

    int nEMCal_chan = 24576;
    int niHCal_chan = 1536;
    int noHCal_chan = 1536;
    const std::string emcal_node_name = "TOWERINFO_CALIB_CEMC";
    const std::string ihcal_node_name = "TOWERINFO_CALIB_HCALIN";
    const std::string ohcal_node_name = "TOWERINFO_CALIB_HCALOUT";

    const std::string emcalClus_node_name = "CLUSTER_CEMC";
    const std::string emcalClus_inner_node_name = "CLUSTERINNER_CEMC";
    const std::string ihcalClus_node_name = "CLUSTER_HCALIN";
    const std::string ohcalClus_node_name = "CLUSTER_HCALOUT";
    
    std::string m_TowerGeomNodeName = "TOWERGEOM_CEMC";
};

#endif // TUTORIAL_H
