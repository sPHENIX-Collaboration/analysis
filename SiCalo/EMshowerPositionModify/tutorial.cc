#include "tutorial.h"

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

#include <calobase/RawClusterContainer.h>
#include <calobase/RawTowerGeomContainer.h>
#include <calobase/RawCluster.h>
#include <calobase/RawClusterUtility.h>
#include <calobase/RawTowerDefs.h>
#include <calobase/RawTowerGeom.h>
#include <calobase/RawTowerGeomv5.h>
#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfo.h>
#include <calobase/TowerInfoDefs.h>

#include <ffaobjects/EventHeaderv1.h>

#include <fun4all/Fun4AllReturnCodes.h>

#include <globalvertex/GlobalVertex.h>
#include <globalvertex/GlobalVertexMap.h>
#include <globalvertex/MbdVertexMap.h>
#include <globalvertex/MbdVertex.h>
#include <globalvertex/SvtxVertexMap.h>
#include <globalvertex/SvtxVertex.h>

#include <phool/getClass.h>
#include <phool/PHCompositeNode.h>

#include <Acts/Geometry/GeometryIdentifier.hpp>
#include <Acts/MagneticField/ConstantBField.hpp>
#include <Acts/MagneticField/MagneticFieldProvider.hpp>
#include <Acts/Surfaces/CylinderSurface.hpp>
#include <Acts/Surfaces/PerigeeSurface.hpp>
#include <Acts/Geometry/TrackingGeometry.hpp>

#include <CLHEP/Vector/ThreeVector.h>
#include <math.h>
#include <vector>
#include <unordered_map>
#include <cmath>

#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TLorentzVector.h>

//____________________________________________________________________________..
tutorial::tutorial(
    const std::string & name_in,
    std::string output_path_in,
    std::string output_rootfile_name_in):
    SubsysReco(name_in),
    output_path(output_path_in),
    output_rootfile_name(output_rootfile_name_in),
    file_out(nullptr),
    output( nullptr )
{
  std::cout << "tutorial::tutorial(const std::string &name) Calling ctor" << std::endl;
}

//____________________________________________________________________________..
tutorial::~tutorial()
{
  std::cout << "tutorial::~tutorial() Calling dtor" << std::endl;
}

//____________________________________________________________________________..
int tutorial::Init(PHCompositeNode *topNode)
{
    std::cout << topNode << std::endl;
  std::cout << "tutorial::Init(PHCompositeNode *topNode) Initializing" << std::endl;

  ////////////////////////////////////////////////////////
  // Initialization of the member                       //
  ////////////////////////////////////////////////////////
//   output = new TFile( output_path.c_str(), "RECREATE" );
  
    std::cout << "tutorial::Init(PHCompositeNode *topNode) Initializing" << std::endl;

    eventID = 0;
    NClus = 0;
    clus_system.clear();
    clus_layer.clear();
    clus_adc.clear();
    clus_X.clear();
    clus_Y.clear();
    clus_Z.clear();
    clus_size.clear();
    clus_phi_size.clear();
    clus_z_size.clear();

    nTowers = 0;
    tower_X.clear();
    tower_Y.clear();
    tower_Z.clear();
    tower_R.clear();
    tower_Eta.clear();
    tower_Phi.clear();
    tower_Eta_test.clear();
    tower_Phi_test.clear();
    tower_Eta_bin.clear();
    tower_Phi_bin.clear();
    tower_edep.clear();
    tower_system.clear();

    tower_int_X.clear();
    tower_int_Y.clear();
    tower_int_Z.clear();
    tower_int_R.clear();

    nCaloClus = 0;
    caloClus_X.clear();
    caloClus_Y.clear();
    caloClus_Z.clear();
    caloClus_R.clear();
    caloClus_Phi.clear();
    caloClus_edep.clear();
    caloClus_system.clear();

    caloClus_innr_X.clear();
    caloClus_innr_Y.clear();
    caloClus_innr_Z.clear();
    caloClus_innr_R.clear();
    caloClus_innr_Phi.clear();
    caloClus_innr_edep.clear();

    calo_tower_ieta.clear();
    calo_tower_iphi.clear();
    calo_tower_e.clear();
    calo_tower_x.clear();
    calo_tower_y.clear();
    calo_tower_z.clear();
    calo_tower_r.clear();
    calo_tower_eta.clear();
    calo_tower_phi.clear();
    calo_tower_time.clear();

    track_proj_X.clear();
    track_proj_Y.clear();
    track_proj_Z.clear();
    track_proj_R.clear();
    track_proj_Phi.clear();

    // note : Truth primary vertex information
    TruthPV_trig_x_ = -999;
    TruthPV_trig_y_ = -999;
    TruthPV_trig_z_ = -999;
    NTruthVtx_ = 0;

    // note : PHG4 information (from all PHG4Particles)
    NPrimaryG4P_ = 0;
    NPrimaryG4P_promptChargeHadron_ = 0;
    PrimaryG4P_Pt_.clear();
    PrimaryG4P_Eta_.clear();
    PrimaryG4P_Phi_.clear();
    PrimaryG4P_E_.clear();
    PrimaryG4P_PID_.clear();
    PrimaryG4P_ParticleClass_.clear();
    PrimaryG4P_isStable_.clear();
    PrimaryG4P_Charge_.clear();
    PrimaryG4P_isChargeHadron_.clear();

    _CEMC_Hit_Evis.clear();
    _CEMC_Hit_Edep.clear();
    _CEMC_Hit_ch.clear();
    _CEMC_Hit_x.clear();
    _CEMC_Hit_y.clear();
    _CEMC_Hit_z.clear();

    _CEMC_Hit_CoG_x.clear();
    _CEMC_Hit_CoG_y.clear();
    _CEMC_Hit_CoG_z.clear();
    _CEMC_Hit_CoG_R.clear();
    _CEMC_Hit_CoG_Edep.clear();

    _CEMC_Pr_Hit_x.clear();
    _CEMC_Pr_Hit_y.clear();
    _CEMC_Pr_Hit_z.clear();
    _CEMC_Pr_Hit_R.clear();
    _CEMC_Pr_Hit_deltaT.clear();

    primary_particles_tracks.clear();

    ////////////////////////////////////////////////////////
    // Initialization of the member                       //
    ////////////////////////////////////////////////////////
    file_out = new TFile((output_path + "/" + output_rootfile_name).c_str(), "RECREATE");
    tree_out = new TTree("tree", "sPHENIX info.");
    
    // note : the tracker cluster
    tree_out -> Branch("trk_NClus", & NClus);
    tree_out -> Branch("trk_system", & clus_system);
    tree_out -> Branch("trk_layer", & clus_layer);
    tree_out -> Branch("trk_adc", & clus_adc);
    tree_out -> Branch("trk_X", & clus_X);
    tree_out -> Branch("trk_Y", & clus_Y);
    tree_out -> Branch("trk_Z", & clus_Z);
    tree_out -> Branch("trk_size", & clus_size);
    tree_out -> Branch("trk_phi_size", & clus_phi_size);
    tree_out -> Branch("trk_Z_size", & clus_z_size);


    // note : the calorimeter raw tower information with the calibration, and introduction of noise hits
    tree_out -> Branch("nTowers", & nTowers);
    tree_out -> Branch("tower_system", & tower_system);
    tree_out -> Branch("tower_X", & tower_X);
    tree_out -> Branch("tower_Y", & tower_Y);
    tree_out -> Branch("tower_Z", & tower_Z);
    tree_out -> Branch("tower_R", & tower_R);
    tree_out -> Branch("tower_Eta", & tower_Eta);
    tree_out -> Branch("tower_Phi", & tower_Phi);
    tree_out -> Branch("tower_Eta_test", & tower_Eta_test);
    tree_out -> Branch("tower_Phi_test", & tower_Phi_test);
    tree_out -> Branch("tower_Eta_bin", & tower_Eta_bin);
    tree_out -> Branch("tower_Phi_bin", & tower_Phi_bin);
    tree_out -> Branch("tower_edep", & tower_edep);

    tree_out -> Branch("tower_int_X", & tower_int_X);
    tree_out -> Branch("tower_int_Y", & tower_int_Y);
    tree_out -> Branch("tower_int_Z", & tower_int_Z);
    tree_out -> Branch("tower_int_R", & tower_int_R);


    // note : the calorimeter raw tower information with the calibration, and introduction of noise hits
    tree_out -> Branch("nCaloClus", & nCaloClus);
    tree_out -> Branch("caloClus_system", & caloClus_system);
    tree_out -> Branch("caloClus_X", & caloClus_X);
    tree_out -> Branch("caloClus_Y", & caloClus_Y);
    tree_out -> Branch("caloClus_Z", & caloClus_Z);
    tree_out -> Branch("caloClus_R", & caloClus_R);
    tree_out -> Branch("caloClus_Phi", & caloClus_Phi);
    tree_out -> Branch("caloClus_edep", & caloClus_edep);

    tree_out -> Branch("caloClus_innr_X", & caloClus_innr_X);
    tree_out -> Branch("caloClus_innr_Y", & caloClus_innr_Y);
    tree_out -> Branch("caloClus_innr_Z", & caloClus_innr_Z);
    tree_out -> Branch("caloClus_innr_R", & caloClus_innr_R);
    tree_out -> Branch("caloClus_innr_Phi", & caloClus_innr_Phi);
    tree_out -> Branch("caloClus_innr_edep", & caloClus_innr_edep);

    tree_out -> Branch("calo_tower_ieta", & calo_tower_ieta);
    tree_out -> Branch("calo_tower_iphi", & calo_tower_iphi);
    tree_out -> Branch("calo_tower_e", & calo_tower_e);
    tree_out -> Branch("calo_tower_x", & calo_tower_x);
    tree_out -> Branch("calo_tower_y", & calo_tower_y);
    tree_out -> Branch("calo_tower_z", & calo_tower_z);
    tree_out -> Branch("calo_tower_r", & calo_tower_r);
    tree_out -> Branch("calo_tower_eta", & calo_tower_eta);
    tree_out -> Branch("calo_tower_phi", & calo_tower_phi);
    tree_out -> Branch("calo_tower_time", & calo_tower_time);

    tree_out -> Branch("track_proj_X", & track_proj_X);
    tree_out -> Branch("track_proj_Y", & track_proj_Y);
    tree_out -> Branch("track_proj_Z", & track_proj_Z);
    tree_out -> Branch("track_proj_R", & track_proj_R);
    tree_out -> Branch("track_proj_Phi", & track_proj_Phi);

    // note : true vertex information from G4Particle 
    tree_out->Branch("NTruthVtx", &NTruthVtx_);
    tree_out->Branch("TruthPV_trig_x", &TruthPV_trig_x_);
    tree_out->Branch("TruthPV_trig_y", &TruthPV_trig_y_);
    tree_out->Branch("TruthPV_trig_z", &TruthPV_trig_z_);
    // note : Primary PHG4Particle information
    tree_out->Branch("NPrimaryG4P", &NPrimaryG4P_);
    tree_out->Branch("PrimaryG4P_Pt", &PrimaryG4P_Pt_);
    tree_out->Branch("PrimaryG4P_Eta", &PrimaryG4P_Eta_);
    tree_out->Branch("PrimaryG4P_Phi", &PrimaryG4P_Phi_);
    tree_out->Branch("PrimaryG4P_E", &PrimaryG4P_E_);
    tree_out->Branch("PrimaryG4P_PID", &PrimaryG4P_PID_);
    tree_out->Branch("PrimaryG4P_isChargeHadron", &PrimaryG4P_isChargeHadron_);

    // truthinfo G4hit info
    tree_out->Branch("CEMC_Hit_Evis", &_CEMC_Hit_Evis);
    tree_out->Branch("CEMC_Hit_Edep", &_CEMC_Hit_Edep);
    tree_out->Branch("CEMC_Hit_ch", &_CEMC_Hit_ch); 
    tree_out->Branch("CEMC_Hit_x", &_CEMC_Hit_x);
    tree_out->Branch("CEMC_Hit_y", &_CEMC_Hit_y);
    tree_out->Branch("CEMC_Hit_z", &_CEMC_Hit_z);
    tree_out->Branch("CEMC_Hit_CoG_x", &_CEMC_Hit_CoG_x);
    tree_out->Branch("CEMC_Hit_CoG_y", &_CEMC_Hit_CoG_y);
    tree_out->Branch("CEMC_Hit_CoG_z", &_CEMC_Hit_CoG_z);
    tree_out->Branch("CEMC_Hit_CoG_R", &_CEMC_Hit_CoG_R);
    tree_out->Branch("CEMC_Hit_CoG_Edep", &_CEMC_Hit_CoG_Edep);

    // Primary particle truth hit on cemc 
    tree_out->Branch("CEMC_Pr_Hit_x", &_CEMC_Pr_Hit_x);
    tree_out->Branch("CEMC_Pr_Hit_y", &_CEMC_Pr_Hit_y);
    tree_out->Branch("CEMC_Pr_Hit_z", &_CEMC_Pr_Hit_z);
    tree_out->Branch("CEMC_Pr_Hit_R", &_CEMC_Pr_Hit_R);
    tree_out->Branch("_CEMC_Pr_Hit_deltaT", &_CEMC_Pr_Hit_deltaT);

    tree_out->Branch("PrG4_TTPRO_dD", &_PrG4_TTPRO_dD);
    tree_out->Branch("PrG4_TTPRO_dR", &_PrG4_TTPRO_dR);
    tree_out->Branch("PrG4_TTPRO_dphi", &_PrG4_TTPRO_dphi);

    return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int tutorial::InitRun(PHCompositeNode * /*topNode*/)
{
  std::cout << "tutorial::InitRun(PHCompositeNode *topNode) Initializing for Run XXX" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}



//____________________________________________________________________________..
int tutorial::prepareTracker(PHCompositeNode * topNode) 
{
  std::cout << "tutorial::process_event(PHCompositeNode *topNode) Processing Event" << std::endl;

  //////////////////////////////////////////////////////////////////////////////
  // Getting Nodes                                                            //
  /////////////////////////////////////////////////////////////////////////////  
  // TRKR_CLUSTER node: Information of TrkrCluster
  node_cluster_map = findNode::getClass<TrkrClusterContainerv4>(topNode, "TRKR_CLUSTER");

  if (!node_cluster_map)
    {
      std::cerr << PHWHERE << "TrkrClusterContainer node is missing." << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }

  // ActsGeometry node: for the global coordinate
  node_acts = findNode::getClass<ActsGeometry>(topNode, "ActsGeometry");
  if ( !node_acts )
    {
      std::cout << PHWHERE << "No ActsGeometry on node tree. Bailing." << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }

  
    std::vector <TrkrCluster *> mvtx_clusters; mvtx_clusters.clear();
    std::vector <TrkrCluster *> intt_clusters; intt_clusters.clear();
    std::vector <TrkrCluster *> tpc_clusters; tpc_clusters.clear();

    // note : mvtx
    for (unsigned int mvtxlayer = 0; mvtxlayer < 3; mvtxlayer++) {
        //      cout << " INTT layer " << mvtxlayer << endl;
        //      int layer= ( mvtxlayer < 2 ? 0 : 1 );

        // loop over all hits
        for (const auto & hitsetkey: node_cluster_map -> getHitSetKeys(TrkrDefs::TrkrId::mvtxId, mvtxlayer)) {

            // type: std::pair<ConstIterator, ConstIterator> ConstRange
            // here, MMap::const_iterator ConstIterator;
            auto range = node_cluster_map -> getClusters(hitsetkey);

            // loop over iterators of this cluster
            for (auto clusIter = range.first; clusIter != range.second; ++clusIter) {
                const auto cluskey = clusIter -> first;
                const auto cluster = clusIter -> second;
                mvtx_clusters.push_back(cluster);

                const auto globalPos = node_acts -> getGlobalPosition(cluskey, cluster);

                // int ladder_z   = InttDefs::getLadderZId(cluskey);
                // int ladder_phi = InttDefs::getLadderPhiId(cluskey);
                int size = cluster -> getSize();

                cluster -> setPosition(0, globalPos.x());
                cluster -> setPosition(1, globalPos.y());
                cluster -> setPosition(2, globalPos.z());

                clus_system.push_back(0); // note : the cluster system is 0 for MVTX
                clus_layer.push_back(mvtxlayer);
                clus_adc.push_back(cluster -> getAdc());
                clus_X.push_back(globalPos.x());
                clus_Y.push_back(globalPos.y());
                clus_Z.push_back(globalPos.z());
                clus_size.push_back(size);
                int phisize = cluster -> getPhiSize();
                // if (phisize <= 0) {phisize += 256;}
                clus_phi_size.push_back(phisize);
                clus_z_size.push_back(cluster -> getZSize());

                // cluster->setPosition(0,  globalPos.x() );
                // cluster->setPosition(1,  globalPos.y() );
                // cluster->setPosition(2,  globalPos.z() );
            }
        }
    }

    // note : intt
    for (unsigned int inttlayer = 0; inttlayer < 4; inttlayer++) {
        //      cout << " INTT layer " << inttlayer << endl;
        //      int layer= ( inttlayer < 2 ? 0 : 1 );

        // loop over all hits
        for (const auto & hitsetkey: node_cluster_map -> getHitSetKeys(TrkrDefs::TrkrId::inttId, inttlayer + 3)) {

            // type: std::pair<ConstIterator, ConstIterator> ConstRange
            // here, MMap::const_iterator ConstIterator;
            auto range = node_cluster_map -> getClusters(hitsetkey);

            // loop over iterators of this cluster
            for (auto clusIter = range.first; clusIter != range.second; ++clusIter) {
                const auto cluskey = clusIter -> first;
                const auto cluster = clusIter -> second;
                intt_clusters.push_back(cluster);

                const auto globalPos = node_acts -> getGlobalPosition(cluskey, cluster);

                // int ladder_z   = InttDefs::getLadderZId(cluskey);
                // int ladder_phi = InttDefs::getLadderPhiId(cluskey);
                int size = cluster -> getSize();

                cluster -> setPosition(0, globalPos.x());
                cluster -> setPosition(1, globalPos.y());
                cluster -> setPosition(2, globalPos.z());

                clus_system.push_back(1); // note : the cluster system is 1 for INTT
                clus_layer.push_back(inttlayer + 3);
                clus_adc.push_back(cluster -> getAdc());
                clus_X.push_back(globalPos.x());
                clus_Y.push_back(globalPos.y());
                clus_Z.push_back(globalPos.z());
                clus_size.push_back(size);
                int phisize = cluster -> getPhiSize();
                if (phisize <= 0) {
                    phisize += 256;
                }
                clus_phi_size.push_back(phisize);
                clus_z_size.push_back(cluster -> getZSize());


                // cluster->setPosition(0,  globalPos.x() );
                // cluster->setPosition(1,  globalPos.y() );
                // cluster->setPosition(2,  globalPos.z() );
            }
        }
    }

    // note : TPC
    for (const auto & hitsetkey: node_cluster_map -> getHitSetKeys(TrkrDefs::TrkrId::tpcId)) {

        // type: std::pair<ConstIterator, ConstIterator> ConstRange
        // here, MMap::const_iterator ConstIterator;
        auto range = node_cluster_map -> getClusters(hitsetkey);

        // loop over iterators of this cluster
        for (auto clusIter = range.first; clusIter != range.second; ++clusIter) {
            const auto cluskey = clusIter -> first;
            const auto cluster = clusIter -> second;
            tpc_clusters.push_back(cluster);

            const auto globalPos = node_acts -> getGlobalPosition(cluskey, cluster);

            // int ladder_z   = InttDefs::getLadderZId(cluskey);
            // int ladder_phi = InttDefs::getLadderPhiId(cluskey);
            int size = cluster -> getSize();

            cluster -> setPosition(0, globalPos.x());
            cluster -> setPosition(1, globalPos.y());
            cluster -> setPosition(2, globalPos.z());

            clus_system.push_back(2); // note : the cluster system is 2 for TPC
            clus_layer.push_back(-999);
            clus_adc.push_back(cluster -> getAdc());
            clus_X.push_back(globalPos.x());
            clus_Y.push_back(globalPos.y());
            clus_Z.push_back(globalPos.z());
            clus_size.push_back(size);
            int phisize = cluster -> getPhiSize();
            // if (phisize <= 0) {phisize += 256;}
            clus_phi_size.push_back(phisize);
            clus_z_size.push_back(cluster -> getZSize());

            // cluster->setPosition(0,  globalPos.x() );
            // cluster->setPosition(1,  globalPos.y() );
            // cluster->setPosition(2,  globalPos.z() );
        }
    }

    NClus = mvtx_clusters.size() + intt_clusters.size() + tpc_clusters.size();

    std::cout << "N mvtx cluster : " << mvtx_clusters.size() << std::endl;
    std::cout << "N intt cluster : " << intt_clusters.size() << std::endl;
    std::cout << "N tpc cluster : " << tpc_clusters.size() << std::endl;

    return Fun4AllReturnCodes::EVENT_OK;

}


// CEMC (PHCompositeNode)/
//        G4HIT_ABSORBER_CEMC (IO,PHG4HitContainer)
//        G4HIT_CEMC (IO,PHG4HitContainer)
//        G4CELL_CEMC (IO,PHG4CellContainer)
//        TOWER_SIM_CEMC (IO,RawTowerContainer)
//        TOWERINFO_SIM_CEMC (IO,TowerInfoContainerv1)
//        TOWER_RAW_CEMC (IO,RawTowerContainer)
//        TOWERINFO_RAW_CEMC (IO,TowerInfoContainerv1)
//        TOWER_CALIB_CEMC (IO,RawTowerContainer)
//        TOWERINFO_CALIB_CEMC (IO,TowerInfoContainerv1)
//        CLUSTER_CEMC (IO,RawClusterContainer)
//        CLUSTER_POS_COR_CEMC (IO,RawClusterContainer)

int tutorial::prepareEMCal(PHCompositeNode * topNode) 
{
    // Geometry used by RawClusterBuilderTemplate is normally TOWERGEOM_CEMC.
    // Keep a small fallback list because some private/detailed-geometry macros
    // write/read a versioned node name.
    geomEM = findNode::getClass<RawTowerGeomContainer>(topNode, m_TowerGeomNodeName);
    if (!geomEM && m_TowerGeomNodeName != "TOWERGEOM_CEMC")
    {
        geomEM = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_CEMC");
    }
    if (!geomEM)
    {
        geomEM = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_CEMCv3");
    }

    EMCal_tower_sim_info = findNode::getClass<TowerInfoContainerv1>(topNode, "TOWERINFO_SIM_CEMC");
    EMCal_tower_sim = findNode::getClass<RawTowerContainer>(topNode, "TOWER_SIM_CEMC");
    EMCal_tower_calib = findNode::getClass<TowerInfoContainer>(topNode, emcal_node_name);
    EMCal_cluster_cont = findNode::getClass<RawClusterContainer>(topNode, emcalClus_node_name);

    if (!geomEM)
    {
        std::cout << "tutorial::prepareEMCal Could not find EMCal geometry node. Tried "
                  << m_TowerGeomNodeName << ", TOWERGEOM_CEMC, TOWERGEOM_CEMCv3" << std::endl;
        return Fun4AllReturnCodes::ABORTEVENT;
    }

    if (!EMCal_cluster_cont)
    {
        std::cout << "tutorial::prepareEMCal Could not find node " << emcalClus_node_name << std::endl;
        return Fun4AllReturnCodes::ABORTEVENT;
    }

    // IMPORTANT:
    // Do NOT loop over the full TowerInfoContainer here.  The requested tower output
    // should be the towers associated to each RawCluster.  RawClusterBuilderTemplate
    // stores those via RawCluster::addTower(twrkey, energy), where twrkey is the
    // RawTowerDefs key using index1=eta and index2=phi.
    //
    // Also do NOT call RawTowerGeom::get_center_int_* here.  In the base class those
    // virtual functions are dummy implementations and print the warnings seen in the log.
    for (unsigned int iclus = 0; iclus < EMCal_cluster_cont->size(); ++iclus)
    {
        RawCluster *cluster = EMCal_cluster_cont->getCluster(iclus);
        if (!cluster) { continue; }
        if (cluster->get_energy() <= 0.5) { continue; }

        std::vector<int> v_ieta;
        std::vector<int> v_iphi;
        std::vector<float> v_e;
        std::vector<float> v_x;
        std::vector<float> v_y;
        std::vector<float> v_z;
        std::vector<float> v_r;
        std::vector<float> v_eta;
        std::vector<float> v_phi;
        std::vector<float> v_time;

        auto tower_range = cluster->get_towers();

        for (auto tower_it = tower_range.first; tower_it != tower_range.second; ++tower_it)
        {
            RawTowerDefs::keytype tower_key = tower_it->first;
            const float tower_e_in_cluster = tower_it->second;

            const int ieta = RawTowerDefs::decode_index1(tower_key);
            const int iphi = RawTowerDefs::decode_index2(tower_key);

            RawTowerGeom *tower_geom = geomEM->get_tower_geometry(tower_key);

            // Safety fallback: if the cluster key somehow lost/mismatched the CEMC id,
            // rebuild a canonical CEMC key from the decoded eta/phi bins.
            if (!tower_geom)
            {
                const RawTowerDefs::keytype geomkey =
                    RawTowerDefs::encode_towerid(RawTowerDefs::CalorimeterId::CEMC, ieta, iphi);
                tower_geom = geomEM->get_tower_geometry(geomkey);
            }

            float tower_x = NAN;
            float tower_y = NAN;
            float tower_z = NAN;
            float tower_r = NAN;
            float tower_eta = NAN;
            float tower_phi = NAN;
            float tower_time = NAN;  // not filled; avoids expensive channel scan

            if (tower_geom)
            {
                tower_x = tower_geom->get_center_x();
                tower_y = tower_geom->get_center_y();
                tower_z = tower_geom->get_center_z();
                tower_r = std::sqrt(tower_x * tower_x + tower_y * tower_y);
                tower_eta = tower_geom->get_eta();
                tower_phi = tower_geom->get_phi();
            }
            else
            {
                std::cout << "tutorial::prepareEMCal missing tower geometry for key="
                          << tower_key << " ieta=" << ieta << " iphi=" << iphi << std::endl;
            }

            // Per-cluster nested tower vectors
            v_ieta.push_back(ieta);
            v_iphi.push_back(iphi);
            v_e.push_back(tower_e_in_cluster);
            v_x.push_back(tower_x);
            v_y.push_back(tower_y);
            v_z.push_back(tower_z);
            v_r.push_back(tower_r);
            v_eta.push_back(tower_eta);
            v_phi.push_back(tower_phi);
            v_time.push_back(tower_time);

            // Flat tower vectors: now also only associated EMCal towers.
            tower_system.push_back(0);
            tower_X.push_back(tower_x);
            tower_Y.push_back(tower_y);
            tower_Z.push_back(tower_z);
            tower_R.push_back(tower_r);
            tower_Eta.push_back(tower_eta);
            tower_Phi.push_back(tower_phi);
            tower_Eta_bin.push_back(ieta);
            tower_Phi_bin.push_back(iphi);
            tower_edep.push_back(tower_e_in_cluster);
        }

        calo_tower_ieta.push_back(v_ieta);
        calo_tower_iphi.push_back(v_iphi);
        calo_tower_e.push_back(v_e);
        calo_tower_x.push_back(v_x);
        calo_tower_y.push_back(v_y);
        calo_tower_z.push_back(v_z);
        calo_tower_r.push_back(v_r);
        calo_tower_eta.push_back(v_eta);
        calo_tower_phi.push_back(v_phi);
        calo_tower_time.push_back(v_time);
    }

    return Fun4AllReturnCodes::EVENT_OK;
}

int tutorial::prepareiHCal(PHCompositeNode * topNode) {
    // HCALIN (PHCompositeNode)/
    //   G4HIT_ABSORBER_HCALIN (IO,PHG4HitContainer)
    //   G4HIT_HCALIN (IO,PHG4HitContainer)
    //   G4CELL_HCALIN (IO,PHG4CellContainer)
    //   TOWER_SIM_HCALIN (IO,RawTowerContainer)
    //   TOWERINFO_SIM_HCALIN (IO,TowerInfoContainerv1)
    //   TOWER_RAW_HCALIN (IO,RawTowerContainer)
    //   TOWERINFO_RAW_HCALIN (IO,TowerInfoContainerv1)
    //   TOWER_CALIB_HCALIN (IO,RawTowerContainer)
    //   TOWERINFO_CALIB_HCALIN (IO,TowerInfoContainerv1)
    //   CLUSTER_HCALIN (IO,RawClusterContainer)

    geomIH = findNode::getClass <RawTowerGeomContainer> (topNode, "TOWERGEOM_HCALIN");
    iHCal_tower_sim = findNode::getClass <RawTowerContainer> (topNode, "TOWER_SIM_HCALIN");
    iHCal_tower_calib = findNode::getClass <TowerInfoContainerv2> (topNode, ihcal_node_name);

    if (!iHCal_tower_calib) {
        std::cout << "tutorial::process_event Could not find node " << ihcal_node_name << std::endl;
        exit(1);
    }

    // note : iHCal
    for (int i = 0; i < iHCal_tower_calib -> size(); ++i) //loop over channels 
    {
        TowerInfo * tower = iHCal_tower_calib -> get_tower_at_channel(i); //get iHCal tower
        int key = iHCal_tower_calib -> encode_key(i);
        int etabin = iHCal_tower_calib -> getTowerEtaBin(key);
        int phibin = iHCal_tower_calib -> getTowerPhiBin(key);
        // float time = iHCal_tower_calib->get_tower_at_channel(i)->get_time_short(); //get time
        float time = 0.0;
        if (tower -> get_energy() <= 0.07) {continue;}

        const RawTowerDefs::keytype geomkey = RawTowerDefs::encode_towerid(RawTowerDefs::CalorimeterId::HCALIN, etabin, phibin);
        RawTowerGeom * tower_geom = geomIH -> get_tower_geometry(geomkey); //encode tower geometry
        double iHCal_pos_x   = tower_geom -> get_center_x();
        double iHCal_pos_y   = tower_geom -> get_center_y();
        double iHCal_pos_z   = tower_geom -> get_center_z();
        double iHCal_pos_eta = tower_geom -> get_eta();
        double iHCal_pos_phi = tower_geom -> get_phi();
        double iHCal_energy  = tower -> get_energy();

        tower_system.push_back(1);
        tower_X.push_back(iHCal_pos_x);
        tower_Y.push_back(iHCal_pos_y);
        tower_Z.push_back(iHCal_pos_z);
        tower_Eta.push_back(iHCal_pos_eta);
        tower_Phi.push_back(iHCal_pos_phi);
        tower_Eta_bin.push_back(etabin);
        tower_Phi_bin.push_back(phibin);
        tower_edep.push_back(iHCal_energy);
        // std::cout<<"test iHCal 15"<<std::endl;
    }

    return Fun4AllReturnCodes::EVENT_OK;
}

int tutorial::prepareoHCal(PHCompositeNode * topNode) {
    //     HCALOUT (PHCompositeNode)/
    //        G4HIT_ABSORBER_HCALOUT (IO,PHG4HitContainer)
    //        G4HIT_HCALOUT (IO,PHG4HitContainer)
    //        G4CELL_HCALOUT (IO,PHG4CellContainer)
    //        TOWER_SIM_HCALOUT (IO,RawTowerContainer)
    //        TOWERINFO_SIM_HCALOUT (IO,TowerInfoContainerv1)
    //        TOWER_RAW_HCALOUT (IO,RawTowerContainer)
    //        TOWERINFO_RAW_HCALOUT (IO,TowerInfoContainerv1)
    //        TOWER_CALIB_HCALOUT (IO,RawTowerContainer)
    //        TOWERINFO_CALIB_HCALOUT (IO,TowerInfoContainerv1)
    //        CLUSTER_HCALOUT (IO,RawClusterContainer)

    geomOH = findNode::getClass <RawTowerGeomContainer> (topNode, "TOWERGEOM_HCALOUT");
    oHCal_tower_sim = findNode::getClass <RawTowerContainer> (topNode, "TOWER_SIM_HCALOUT");
    oHCal_tower_calib = findNode::getClass <TowerInfoContainerv2> (topNode, ohcal_node_name);

    if (!oHCal_tower_calib) {
        std::cout << "tutorial::process_event Could not find node " << ohcal_node_name << std::endl;
        exit(1);
    }

    // note : oHCal
    for (int i = 0; i < oHCal_tower_calib -> size(); ++i) //loop over channels 
    {
        // std::cout<<"test oHCal 1"<<std::endl;
        TowerInfo * tower = oHCal_tower_calib -> get_tower_at_channel(i); //get oHCal tower
        // std::cout<<"test oHCal 2"<<std::endl;
        int key = oHCal_tower_calib -> encode_key(i);
        int etabin = oHCal_tower_calib -> getTowerEtaBin(key);
        int phibin = oHCal_tower_calib -> getTowerPhiBin(key);

        if (tower -> get_energy() <= 0.07) {continue;}

        const RawTowerDefs::keytype geomkey = RawTowerDefs::encode_towerid(RawTowerDefs::CalorimeterId::HCALOUT, etabin, phibin);
        RawTowerGeom * tower_geom = geomOH -> get_tower_geometry(geomkey); //encode tower geometry
        double oHCal_pos_x   = tower_geom -> get_center_x();
        double oHCal_pos_y   = tower_geom -> get_center_y();
        double oHCal_pos_z   = tower_geom -> get_center_z();
        double oHCal_pos_eta = tower_geom -> get_eta();
        double oHCal_pos_phi = tower_geom -> get_phi();
        double oHCal_energy  = tower -> get_energy();

        tower_system.push_back(2);
        tower_X.push_back(oHCal_pos_x);
        tower_Y.push_back(oHCal_pos_y);
        tower_Z.push_back(oHCal_pos_z);
        tower_Eta.push_back(oHCal_pos_eta);
        tower_Phi.push_back(oHCal_pos_phi);
        tower_Eta_bin.push_back(etabin);
        tower_Phi_bin.push_back(phibin);
        tower_edep.push_back(oHCal_energy);
    }

    return Fun4AllReturnCodes::EVENT_OK;
}

// === s === ChcKuma add Cal Clus ===============================================
int tutorial::prepareEMCalClus(PHCompositeNode * topNode) {
    EMCal_cluster_cont = findNode::getClass <RawClusterContainer> (topNode, "CLUSTER_CEMC");
    // EMCal_cluster_innr = findNode::getClass <RawClusterContainer> (topNode, "CLUSTERINNER_CEMC");
    EMCal_cluster_innr = findNode::getClass <RawClusterContainer> (topNode, "CLUSTER_CEMC");

    if (!EMCal_cluster_cont) {
        std::cout << "tutorial::process_event Could not find node " << emcalClus_node_name << std::endl;
        exit(1);
    }
    if (!EMCal_cluster_innr) {
        std::cout << "tutorial::process_event Could not find node " << emcalClus_inner_node_name << std::endl;
        exit(1);
    }

    // note : EMCal clus center
    for (int i = 0; i < EMCal_cluster_cont -> size(); ++i) //loop over channels 
    {
        RawCluster * cluster = EMCal_cluster_cont  -> getCluster(i); //get EMCal tower
        if (cluster -> get_energy() <= 0.5) {continue;}

        double EMCal_pos_x   = cluster -> get_x();
        double EMCal_pos_y   = cluster -> get_y();
        double EMCal_pos_z   = cluster -> get_z();
        double EMCal_pos_r   = cluster -> get_r();
        double EMCal_pos_phi = cluster -> get_phi();
        double EMCal_energy  = cluster -> get_energy();

        caloClus_system.push_back(0);
        caloClus_X.push_back(EMCal_pos_x);
        caloClus_Y.push_back(EMCal_pos_y);
        caloClus_Z.push_back(EMCal_pos_z);
        caloClus_R.push_back(EMCal_pos_r);
        caloClus_Phi.push_back(EMCal_pos_phi);
        caloClus_edep.push_back(EMCal_energy);
    }   

    // note : EMCal clus innerface center
    for (int i = 0; i < EMCal_cluster_innr -> size(); ++i) //loop over channels 
    {
        RawCluster * cluster_innr = EMCal_cluster_innr  -> getCluster(i); //get EMCal tower
        if (cluster_innr -> get_energy() <= 0.5) {continue;}

        double EMCal_innr_x   = cluster_innr -> get_x();
        double EMCal_innr_y   = cluster_innr -> get_y();
        double EMCal_innr_z   = cluster_innr -> get_z();
        double EMCal_innr_r   = cluster_innr -> get_r();
        double EMCal_innr_phi = cluster_innr -> get_phi();
        double EMCal_innr_energy  = cluster_innr -> get_energy();

        caloClus_system.push_back(321);
        caloClus_innr_X.push_back(EMCal_innr_x);
        caloClus_innr_Y.push_back(EMCal_innr_y);
        caloClus_innr_Z.push_back(EMCal_innr_z);
        caloClus_innr_R.push_back(EMCal_innr_r);
        caloClus_innr_Phi.push_back(EMCal_innr_phi);
        caloClus_innr_edep.push_back(EMCal_innr_energy);
    }

    return Fun4AllReturnCodes::EVENT_OK;
}


int tutorial::prepareiHCalClus(PHCompositeNode * topNode) {
    iHCal_cluster_cont = findNode::getClass <RawClusterContainer> (topNode, "CLUSTER_HCALIN");

    if (!iHCal_cluster_cont) {
        std::cout << "tutorial::process_event Could not find node " << ihcalClus_node_name << std::endl;
        exit(1);
    }

    // note : iHCal
    for (int i = 0; i < iHCal_cluster_cont -> size(); ++i) //loop over channels 
    {
        RawCluster * cluster = iHCal_cluster_cont  -> getCluster(i); //get EMCal tower
        if (cluster -> get_energy() <= 0.5) {continue;}

        double iHCal_pos_x   = cluster -> get_x();
        double iHCal_pos_y   = cluster -> get_y();
        double iHCal_pos_z   = cluster -> get_z();
        double iHCal_pos_r   = cluster -> get_r();
        double iHCal_pos_phi = cluster -> get_phi();
        double iHCal_energy  = cluster -> get_energy();

        caloClus_system.push_back(1);
        caloClus_X.push_back(iHCal_pos_x);
        caloClus_Y.push_back(iHCal_pos_y);
        caloClus_Z.push_back(iHCal_pos_z);
        caloClus_R.push_back(iHCal_pos_r);
        caloClus_Phi.push_back(iHCal_pos_phi);
        caloClus_edep.push_back(iHCal_energy);
    }   
    return Fun4AllReturnCodes::EVENT_OK;
}


int tutorial::prepareoHCalClus(PHCompositeNode * topNode) {
    oHCal_cluster_cont = findNode::getClass <RawClusterContainer> (topNode, "CLUSTER_HCALIN");

    if (!oHCal_cluster_cont) {
        std::cout << "tutorial::process_event Could not find node " << ohcalClus_node_name << std::endl;
        exit(1);
    }

    // note : oHCal
    for (int i = 0; i < oHCal_cluster_cont -> size(); ++i) //loop over channels 
    {
        RawCluster * cluster = oHCal_cluster_cont  -> getCluster(i); //get EMCal tower
        if (cluster -> get_energy() <= 0.5) {continue;}

        double oHCal_pos_x   = cluster -> get_x();
        double oHCal_pos_y   = cluster -> get_y();
        double oHCal_pos_z   = cluster -> get_z();
        double oHCal_pos_r   = cluster -> get_r();
        double oHCal_pos_phi = cluster -> get_phi();
        double oHCal_energy  = cluster -> get_energy();

        caloClus_system.push_back(2);
        caloClus_X.push_back(oHCal_pos_x);
        caloClus_Y.push_back(oHCal_pos_y);
        caloClus_Z.push_back(oHCal_pos_z);
        caloClus_R.push_back(oHCal_pos_r);
        caloClus_Phi.push_back(oHCal_pos_phi);
        caloClus_edep.push_back(oHCal_energy);
    }   
    return Fun4AllReturnCodes::EVENT_OK;
}

// === e === ChcKuma add Cal Clus ===============================================

int tutorial::prepareG4Turth(PHCompositeNode * topNode){
    std::cout << "Get PHG4 info.: truth primary vertex" << std::endl;
    m_truth_info = findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");   

    if (!m_truth_info)
    {
        std::cout << PHWHERE << "Error, can't find G4TruthInfo" << std::endl;
        exit(1);
    }

    // note : Truth vertex
    auto vrange = m_truth_info->GetPrimaryVtxRange();
    int NTruthPV = 0, NTruthPV_Embeded0 = 0;
    for (auto iter = vrange.first; iter != vrange.second; ++iter) // process all primary vertices
    {
        const int point_id = iter->first;
        PHG4VtxPoint *point = iter->second;
        if (point)
        {
            if (m_truth_info->isEmbededVtx(point_id) == 0)
            {
                TruthPV_trig_x_ = point->get_x();
                TruthPV_trig_y_ = point->get_y();
                TruthPV_trig_z_ = point->get_z();
                
                NTruthPV_Embeded0++;
            }
            NTruthPV++;
        }
    }

    NTruthVtx_ = NTruthPV;

    // note : PHG4Particle
    std::vector<int> tmpv_chargehadron;
    tmpv_chargehadron.clear();
    std::cout << "Get PHG4 info.: truth primary G4Particle" << std::endl;
    const auto prange = m_truth_info->GetPrimaryParticleRange();
    // const auto prange = m_truth_info->GetParticleRange();
    for (auto iter = prange.first; iter != prange.second; ++iter)
    {
        PHG4Particle *ptcl = iter->second;
        // particle->identify();
        if (ptcl)
        {
            PrimaryG4P_PID_.push_back(ptcl->get_pid());
            TLorentzVector p;
            p.SetPxPyPzE(ptcl->get_px(), ptcl->get_py(), ptcl->get_pz(), ptcl->get_e());
            PrimaryG4P_E_.push_back(ptcl->get_e());
            PrimaryG4P_Pt_.push_back(p.Pt());
            PrimaryG4P_Eta_.push_back(p.Eta());
            PrimaryG4P_Phi_.push_back(p.Phi());

            TString particleclass = TString(TDatabasePDG::Instance()->GetParticle(ptcl->get_pid())->ParticleClass());
            bool isStable = (TDatabasePDG::Instance()->GetParticle(ptcl->get_pid())->Stable() == 1) ? true : false;
            double charge = TDatabasePDG::Instance()->GetParticle(ptcl->get_pid())->Charge();
            bool isHadron = (particleclass.Contains("Baryon") || particleclass.Contains("Meson"));
            bool isChargeHadron = (isStable && (charge != 0) && isHadron);
            if (isChargeHadron)
                tmpv_chargehadron.push_back(ptcl->get_pid());

            PrimaryG4P_ParticleClass_.push_back(particleclass);
            PrimaryG4P_isStable_.push_back(isStable);
            PrimaryG4P_Charge_.push_back(charge);
            PrimaryG4P_isChargeHadron_.push_back(isChargeHadron);

            // 找到pr electron track id
            // if (abs(ptcl->get_pid()) == 11)  // 电子的 PDG ID 是 ±11
            // {
            //     primary_particles_tracks.insert(ptcl->get_track_id());
            // }
            primary_particles_tracks.insert(ptcl->get_track_id());
        }
    }
    NPrimaryG4P_ = PrimaryG4P_PID_.size();
    NPrimaryG4P_promptChargeHadron_ = tmpv_chargehadron.size();

    return Fun4AllReturnCodes::EVENT_OK;
}

// -------------------------------------------------------------------------------------------------------------------------- 
int tutorial::prepareG4HIT(PHCompositeNode * topNode)
{
    if (primary_particles_tracks.empty())
    {
        std::cout << "No primary electrons found in this event!" << std::endl;
        return 0;
    }

    hits_CEMC = findNode::getClass<PHG4HitContainer>(topNode, "G4HIT_CEMC");
    if(!hits_CEMC)
    {
      std::cout << "PhotonEMC::process_event: G4HIT_CEMC not found!!!" << std::endl;
    }

    int prhit0 = 0;
    double t_earlist = 0.;

    double g4hit_sumE = 0.;
    double g4hit_sumX = 0.;
    double g4hit_sumY = 0.;
    double g4hit_sumZ = 0.;

    PHG4HitContainer::ConstRange hit_range = hits_CEMC->getHits();
    for (PHG4HitContainer::ConstIterator hit_iter = hit_range.first; hit_iter != hit_range.second; hit_iter++)
    {
        PHG4Hit *this_hit = hit_iter->second;
        float light_yield = hit_iter->second->get_light_yield();
        float edep = hit_iter->second->get_edep();
        int ch = hit_iter->second->get_layer();
        float x = hit_iter->second->get_x(0);
        float y = hit_iter->second->get_y(0);
        float z = hit_iter->second->get_z(0);

        if (edep > 0)
        {
            const double x_mid = 0.5 * (this_hit->get_x(0) + this_hit->get_x(1));
            const double y_mid = 0.5 * (this_hit->get_y(0) + this_hit->get_y(1));
            const double z_mid = 0.5 * (this_hit->get_z(0) + this_hit->get_z(1));

            g4hit_sumE += edep;
            g4hit_sumX += edep * x_mid;
            g4hit_sumY += edep * y_mid;
            g4hit_sumZ += edep * z_mid;
        }
        
        // int trkid = hit_iter->second->get_trkid();
        // PHG4Particle *part = truthinfo->GetParticle(trkid);
        // v4.SetPxPyPzE(part->get_px(), part->get_py(), part->get_pz(), part->get_e());
        // int pid = part->get_pid();
        // _CEMC_Hit_pid.push_back(pid);

        // int vtxid = part->get_vtx_id();
        // PHG4VtxPoint *vtx = truthinfo->GetVtx(vtxid);

        // // add trkid to a set
        // _CEMC_Hit_Evis.push_back(light_yield);
        // _CEMC_Hit_Edep.push_back(edep);
        // _CEMC_Hit_ch.push_back(ch);
        // _CEMC_Hit_x.push_back(x);
        // _CEMC_Hit_y.push_back(y);
        // _CEMC_Hit_z.push_back(z);

        // _CEMC_Hit_particle_x.push_back(vtx->get_x());
        // _CEMC_Hit_particle_y.push_back(vtx->get_y());
        // _CEMC_Hit_particle_z.push_back(vtx->get_z());

        // get primary electron hits on emcal
        int track_id = this_hit->get_trkid();
        if (primary_particles_tracks.find(track_id) == primary_particles_tracks.end()) continue;
        // prhit0 += 1;

        double x0 = this_hit->get_x(0);  // 入口 x0, x1-出口 
        double y0 = this_hit->get_y(0);  // 入口 y
        double z0 = this_hit->get_z(0);  // 入口 z 
        double r0 = sqrt(x0*x0 + y0*y0);

        double t_compare = this_hit->get_t(0); // 时间

        auto &slot = first_by_tid[track_id];
        if (!slot.set || t_compare < slot.t0) {
            slot.set = true;
            slot.t0   = t_compare;
            slot.x0  = x0;
            slot.y0  = y0;
            slot.z0  = z0;
            slot.r0  = r0;
        }

        // if (prhit0 == 1) 
        // {
        //     t_earlist = t0;

        //     _CEMC_Pr_Hit_x.push_back(x0);
        //     _CEMC_Pr_Hit_y.push_back(y0);
        //     _CEMC_Pr_Hit_z.push_back(z0);
        //     _CEMC_Pr_Hit_R.push_back(r0);
        // }
        // _CEMC_Pr_Hit_deltaT.push_back(delta_t);
    }

    if (g4hit_sumE > 0)
    {
        const double cog_x = g4hit_sumX / g4hit_sumE;
        const double cog_y = g4hit_sumY / g4hit_sumE;
        const double cog_z = g4hit_sumZ / g4hit_sumE;
        const double cog_r = std::sqrt(cog_x * cog_x + cog_y * cog_y);

        _CEMC_Hit_CoG_x.push_back(cog_x);
        _CEMC_Hit_CoG_y.push_back(cog_y);
        _CEMC_Hit_CoG_z.push_back(cog_z);
        _CEMC_Hit_CoG_R.push_back(cog_r);
        _CEMC_Hit_CoG_Edep.push_back(g4hit_sumE);
    }

    // 每个 track_id 只保留“它自己的第一个 G4Hit”
    for (const auto &kv : first_by_tid) 
    {
        const auto &h = kv.second;
        _CEMC_Pr_Hit_x.push_back(h.x0);
        _CEMC_Pr_Hit_y.push_back(h.y0);
        _CEMC_Pr_Hit_z.push_back(h.z0);
        _CEMC_Pr_Hit_R.push_back(h.r0);
    }

    return Fun4AllReturnCodes::EVENT_OK;
}

int tutorial::prepareTruthTrack(PHCompositeNode * topNode) 
{
    trackMap = findNode::getClass<SvtxTrackMap>(topNode, "SvtxTrackMap");
    if(!trackMap)
    {
      std::cout << "TrackCaloMatch::process_event not found! Aborting!" << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }

    for (auto &iter : *trackMap)
    {
        truth_track = iter.second;
        
        double truth_tk_pt = truth_track->get_pt();
        std::cout<< "Truth track pt: " << truth_tk_pt << std::endl;

        thisState = truth_track->get_state(99);
        double em_x = thisState->get_x();
        double em_y = thisState->get_y();
        double em_r = sqrt(em_x*em_x + em_y*em_y);
        double em_phi = thisState->get_phi();
        double em_z = thisState->get_z();
        // std::cout << "Truth track state at EMCal: (x=" << em_x
        //           << ", y=" << em_y << ", r=" << em_r << ", phi=" << em_phi
        //           << ", z=" << em_z << ")" << std::endl;

        _PrG4_TTPRO_dD.push_back(sqrt((em_x-_CEMC_Pr_Hit_x[0])*(em_x-_CEMC_Pr_Hit_x[0])+(em_y-_CEMC_Pr_Hit_y[0])*(em_y-_CEMC_Pr_Hit_y[0]))); // 计算与初级电子的距离差
        _PrG4_TTPRO_dR.push_back(em_r-_CEMC_Pr_Hit_R[0]);
        _PrG4_TTPRO_dphi.push_back(em_phi-atan2(_CEMC_Pr_Hit_y[0], _CEMC_Pr_Hit_x[0])); // 计算与初级电子的角度差
    }

    return Fun4AllReturnCodes::EVENT_OK;
}

int tutorial::prepareTrackStates(PHCompositeNode * topNode) 
{
    trackMap = findNode::getClass<SvtxTrackMap>(topNode, "SvtxTrackMap");
    if(!trackMap)
    {
      std::cout << "TrackCaloMatch::process_event not found! Aborting!" << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }

    for (auto &iter : *trackMap)
    {
        truth_track = iter.second;
        
        double truth_tk_pt = truth_track->get_pt();
        std::cout<< "Truth track pt: " << truth_tk_pt << std::endl;

        emcProjState = truth_track->get_state(93.5);
        if (!emcProjState) 
        {
            std::cout << "No track state at EMCal radius found!" << std::endl;
            continue;   
        }
        double em_x = emcProjState->get_x();
        double em_y = emcProjState->get_y();
        double em_r = sqrt(em_x*em_x + em_y*em_y);
        double em_phi = emcProjState->get_phi();
        double em_z = emcProjState->get_z();
        // std::cout << "Truth track state at EMCal: (x=" << em_x
        //           << ", y=" << em_y << ", r=" << em_r << ", phi=" << em_phi
        //           << ", z=" << em_z << ")" << std::endl;
        track_proj_X.push_back(em_x);
        track_proj_Y.push_back(em_y);
        track_proj_Z.push_back(em_z);
        track_proj_R.push_back(em_r);
        track_proj_Phi.push_back(em_phi);
    }

    return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int tutorial::process_event(PHCompositeNode * topNode) {

    prepareTracker(topNode);
    prepareEMCal(topNode);
    // prepareiHCal(topNode);
    // prepareoHCal(topNode);
    prepareEMCalClus(topNode);
    // prepareiHCalClus(topNode);
    // prepareoHCalClus(topNode);
    
    prepareG4Turth(topNode);
    prepareG4HIT(topNode);

    // prepareTruthTrack(topNode);

    prepareTrackStates(topNode);

    nTowers = tower_system.size();
    
    tree_out -> Fill();

    return Fun4AllReturnCodes::EVENT_OK;
}


//____________________________________________________________________________..
int tutorial::ResetEvent(PHCompositeNode *topNode)
{
    std::cout << "tutorial::ResetEvent(PHCompositeNode *topNode) Resetting internal structures, prepare for next event" << std::endl;

    NClus = 0;
    clus_system.clear();
    clus_layer.clear();
    clus_adc.clear();
    clus_X.clear();
    clus_Y.clear();
    clus_Z.clear();
    clus_size.clear();
    clus_phi_size.clear();
    clus_z_size.clear();

    nTowers = 0;
    tower_system.clear();
    tower_X.clear();
    tower_Y.clear();
    tower_Z.clear();
    tower_R.clear();
    tower_Eta.clear();
    tower_Phi.clear();
    tower_Eta_test.clear();
    tower_Phi_test.clear();
    tower_Eta_bin.clear();
    tower_Phi_bin.clear();
    tower_edep.clear();

    tower_int_X.clear();
    tower_int_Y.clear();
    tower_int_Z.clear();
    tower_int_R.clear();

    nCaloClus = 0;
    caloClus_system.clear();
    caloClus_X.clear();
    caloClus_Y.clear();
    caloClus_Z.clear();
    caloClus_R.clear();
    caloClus_Phi.clear();
    caloClus_edep.clear();

    caloClus_innr_X.clear();
    caloClus_innr_Y.clear();
    caloClus_innr_Z.clear();
    caloClus_innr_R.clear();
    caloClus_innr_Phi.clear();
    caloClus_innr_edep.clear();

    calo_tower_ieta.clear();
    calo_tower_iphi.clear();
    calo_tower_e.clear();
    calo_tower_x.clear();
    calo_tower_y.clear();
    calo_tower_z.clear();
    calo_tower_r.clear();
    calo_tower_eta.clear();
    calo_tower_phi.clear();
    calo_tower_time.clear();

    track_proj_X.clear();
    track_proj_Y.clear();
    track_proj_Z.clear();
    track_proj_R.clear();
    track_proj_Phi.clear();

    // note : Truth primary vertex information
    TruthPV_trig_x_ = -999;
    TruthPV_trig_y_ = -999;
    TruthPV_trig_z_ = -999;
    NTruthVtx_ = 0;
    // note : PHG4 information (from all PHG4Particles)
    NPrimaryG4P_ = 0;
    NPrimaryG4P_promptChargeHadron_ = 0;
    PrimaryG4P_Pt_.clear();
    PrimaryG4P_Eta_.clear();
    PrimaryG4P_Phi_.clear();
    PrimaryG4P_E_.clear();
    PrimaryG4P_PID_.clear();
    PrimaryG4P_ParticleClass_.clear();
    PrimaryG4P_isStable_.clear();
    PrimaryG4P_Charge_.clear();
    PrimaryG4P_isChargeHadron_.clear();

    _CEMC_Hit_Evis.clear();
    _CEMC_Hit_Edep.clear();
    _CEMC_Hit_ch.clear();
    _CEMC_Hit_x.clear();
    _CEMC_Hit_y.clear();
    _CEMC_Hit_z.clear();

    _CEMC_Hit_CoG_x.clear();
    _CEMC_Hit_CoG_y.clear();
    _CEMC_Hit_CoG_z.clear();
    _CEMC_Hit_CoG_R.clear();
    _CEMC_Hit_CoG_Edep.clear();

    _CEMC_Pr_Hit_x.clear();
    _CEMC_Pr_Hit_y.clear();
    _CEMC_Pr_Hit_z.clear();
    _CEMC_Pr_Hit_R.clear();
    _CEMC_Pr_Hit_deltaT.clear();

    primary_particles_tracks.clear();
    first_by_tid.clear();

    eventID += 1;


    // 2. 再对所有 vector 进行 shrink_to_fit 操作，释放多余内存
    clus_system.shrink_to_fit();
    clus_layer.shrink_to_fit();
    clus_adc.shrink_to_fit();
    clus_X.shrink_to_fit();
    clus_Y.shrink_to_fit();
    clus_Z.shrink_to_fit();
    clus_size.shrink_to_fit();
    clus_phi_size.shrink_to_fit();
    clus_z_size.shrink_to_fit();

    tower_system.shrink_to_fit();
    tower_X.shrink_to_fit();
    tower_Y.shrink_to_fit();
    tower_Z.shrink_to_fit();
    tower_R.shrink_to_fit();
    tower_Eta.shrink_to_fit();
    tower_Phi.shrink_to_fit();
    tower_Eta_test.shrink_to_fit();
    tower_Phi_test.shrink_to_fit();
    tower_Eta_bin.shrink_to_fit();
    tower_Phi_bin.shrink_to_fit();
    tower_edep.shrink_to_fit();

    tower_int_X.shrink_to_fit();
    tower_int_Y.shrink_to_fit();
    tower_int_Z.shrink_to_fit();
    tower_int_R.shrink_to_fit();

    caloClus_system.shrink_to_fit();
    caloClus_X.shrink_to_fit();
    caloClus_Y.shrink_to_fit();
    caloClus_Z.shrink_to_fit();
    caloClus_R.shrink_to_fit();
    caloClus_Phi.shrink_to_fit();
    caloClus_edep.shrink_to_fit();

    caloClus_innr_X.shrink_to_fit();
    caloClus_innr_Y.shrink_to_fit();
    caloClus_innr_Z.shrink_to_fit();
    caloClus_innr_R.shrink_to_fit();
    caloClus_innr_Phi.shrink_to_fit();
    caloClus_innr_edep.shrink_to_fit();

    track_proj_X.shrink_to_fit();
    track_proj_Y.shrink_to_fit();
    track_proj_Z.shrink_to_fit();
    track_proj_R.shrink_to_fit();
    track_proj_Phi.shrink_to_fit();

    PrimaryG4P_Pt_.shrink_to_fit();
    PrimaryG4P_Eta_.shrink_to_fit();
    PrimaryG4P_Phi_.shrink_to_fit();
    PrimaryG4P_E_.shrink_to_fit();
    PrimaryG4P_PID_.shrink_to_fit();
    PrimaryG4P_ParticleClass_.shrink_to_fit();
    PrimaryG4P_isStable_.shrink_to_fit();
    PrimaryG4P_Charge_.shrink_to_fit();
    PrimaryG4P_isChargeHadron_.shrink_to_fit();

    _CEMC_Hit_Evis.shrink_to_fit();
    _CEMC_Hit_Edep.shrink_to_fit();
    _CEMC_Hit_ch.shrink_to_fit();
    _CEMC_Hit_x.shrink_to_fit();
    _CEMC_Hit_y.shrink_to_fit();
    _CEMC_Hit_z.shrink_to_fit();

    _CEMC_Hit_CoG_x.shrink_to_fit();
    _CEMC_Hit_CoG_y.shrink_to_fit();
    _CEMC_Hit_CoG_z.shrink_to_fit();
    _CEMC_Hit_CoG_R.shrink_to_fit();
    _CEMC_Hit_CoG_Edep.shrink_to_fit();

    _CEMC_Pr_Hit_x.shrink_to_fit();
    _CEMC_Pr_Hit_y.shrink_to_fit();
    _CEMC_Pr_Hit_z.shrink_to_fit();
    _CEMC_Pr_Hit_R.shrink_to_fit();
    _CEMC_Pr_Hit_deltaT.shrink_to_fit();

    return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int tutorial::EndRun(const int runnumber)
{
    std::cout << "tutorial::EndRun(const int runnumber) Ending Run for Run " << runnumber << std::endl;

    return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int tutorial::End(PHCompositeNode *topNode)
{
  std::cout << "tutorial::End(PHCompositeNode *topNode) This is the End..." << std::endl;

  ////////////////////////////////////////////////////////
  // Writing objects to the output file                 //
  ////////////////////////////////////////////////////////
  file_out -> cd();
  tree_out -> Write();
  file_out -> Close();

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int tutorial::Reset(PHCompositeNode *topNode)
{
 std::cout << "tutorial::Reset(PHCompositeNode *topNode) being Reset" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
void tutorial::Print(const std::string &what) const
{
  std::cout << "tutorial::Print(const std::string &what) const Printing info for " << what << std::endl;
}



//____________________________________________________________________________..
bool tutorial::checkTrack(SvtxTrack* track)
{
    return true;
}