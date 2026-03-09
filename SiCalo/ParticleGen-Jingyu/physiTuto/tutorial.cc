//____________________________________________________________________________..
//
// This is a template for a Fun4All SubsysReco module with all methods from the
// $OFFLINE_MAIN/include/fun4all/SubsysReco.h baseclass
// You do not have to implement all of them, you can just remove unused methods
// here and in tutorial.h.
//
// tutorial(const std::string &name = "tutorial")
// everything is keyed to tutorial, duplicate names do work but it makes
// e.g. finding culprits in logs difficult or getting a pointer to the module
// from the command line
//
// tutorial::~tutorial()
// this is called when the Fun4AllServer is deleted at the end of running. Be
// mindful what you delete - you do loose ownership of object you put on the node tree
//
// int tutorial::Init(PHCompositeNode *topNode)
// This method is called when the module is registered with the Fun4AllServer. You
// can create historgrams here or put objects on the node tree but be aware that
// modules which haven't been registered yet did not put antyhing on the node tree
//
// int tutorial::InitRun(PHCompositeNode *topNode)
// This method is called when the first event is read (or generated). At
// this point the run number is known (which is mainly interesting for raw data
// processing). Also all objects are on the node tree in case your module's action
// depends on what else is around. Last chance to put nodes under the DST Node
// We mix events during readback if branches are added after the first event
//
// int tutorial::process_event(PHCompositeNode *topNode)
// called for every event. Return codes trigger actions, you find them in
// $OFFLINE_MAIN/include/fun4all/Fun4AllReturnCodes.h
//   everything is good:
//     return Fun4AllReturnCodes::EVENT_OK
//   abort event reconstruction, clear everything and process next event:
//     return Fun4AllReturnCodes::ABORT_EVENT; 
//   proceed but do not save this event in output (needs output manager setting):
//     return Fun4AllReturnCodes::DISCARD_EVENT; 
//   abort processing:
//     return Fun4AllReturnCodes::ABORT_RUN
// all other integers will lead to an error and abort of processing
//
// int tutorial::ResetEvent(PHCompositeNode *topNode)
// If you have internal data structures (arrays, stl containers) which needs clearing
// after each event, this is the place to do that. The nodes under the DST node are cleared
// by the framework
//
// int tutorial::EndRun(const int runnumber)
// This method is called at the end of a run when an event from a new run is
// encountered. Useful when analyzing multiple runs (raw data). Also called at
// the end of processing (before the End() method)
//
// int tutorial::End(PHCompositeNode *topNode)
// This is called at the end of processing. It needs to be called by the macro
// by Fun4AllServer::End(), so do not forget this in your macro
//
// int tutorial::Reset(PHCompositeNode *topNode)
// not really used - it is called before the dtor is called
//
// void tutorial::Print(const std::string &what) const
// Called from the command line - useful to print information when you need it
//
//____________________________________________________________________________..

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

    _CEMC_Pr_Hit_x.clear();
    _CEMC_Pr_Hit_y.clear();
    _CEMC_Pr_Hit_z.clear();
    _CEMC_Pr_Hit_R.clear();
    _CEMC_Pr_Hit_deltaT.clear();

    primary_electron_tracks.clear();

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
    geomEM = findNode::getClass <RawTowerGeomContainer> (topNode, m_TowerGeomNodeName);

    EMCal_tower_sim_info = findNode::getClass <TowerInfoContainerv1> (topNode, "TOWERINFO_SIM_CEMC");
    EMCal_tower_sim = findNode::getClass <RawTowerContainer> (topNode, "TOWER_SIM_CEMC");
    EMCal_tower_calib = findNode::getClass <TowerInfoContainerv2> (topNode, emcal_node_name);

    if (!EMCal_tower_calib) 
    {
        std::cout << "tutorial::process_event Could not find node " << emcal_node_name << std::endl;
        exit(1);
    }

    // note : EMCal
    for (int i = 0; i < EMCal_tower_calib -> size(); ++i) //loop over channels 
    {
        TowerInfo * tower = EMCal_tower_calib -> get_tower_at_channel(i); //get EMCal tower
        int key = EMCal_tower_calib -> encode_key(i);
        int etabin = EMCal_tower_calib -> getTowerEtaBin(key);
        int phibin = EMCal_tower_calib -> getTowerPhiBin(key);
        // float time = EMCal_tower_calib->get_tower_at_channel(i)->get_time_float(); //get time

        if (tower -> get_energy() <= 0.07) {continue;}

        const RawTowerDefs::keytype geomkey = RawTowerDefs::encode_towerid(RawTowerDefs::CalorimeterId::CEMC, etabin, phibin);
        RawTowerGeom * tower_geom = geomEM -> get_tower_geometry(geomkey); //encode tower geometry
        double EMCal_pos_x   = tower_geom -> get_center_x();
        double EMCal_pos_y   = tower_geom -> get_center_y();
        double EMCal_pos_z   = tower_geom -> get_center_z();
        double EMCal_pos_R   = sqrt(EMCal_pos_x*EMCal_pos_x + EMCal_pos_y*EMCal_pos_y);
        double EMCal_pos_eta = tower_geom -> get_eta();
        double EMCal_pos_phi = tower_geom -> get_phi();
        double EMCal_energy  = tower -> get_energy();

        tower_system.push_back(0);
        tower_X.push_back(EMCal_pos_x);
        tower_Y.push_back(EMCal_pos_y);
        tower_Z.push_back(EMCal_pos_z);
        tower_R.push_back(EMCal_pos_R);
        tower_Eta.push_back(EMCal_pos_eta);
        tower_Phi.push_back(EMCal_pos_phi);
        tower_Eta_bin.push_back(etabin);
        tower_Phi_bin.push_back(phibin);
        tower_edep.push_back(EMCal_energy);
    
        double EMCal_int_x   = tower_geom -> get_center_int_x();
        double EMCal_int_y   = tower_geom -> get_center_int_y();
        double EMCal_int_z   = tower_geom -> get_center_int_z();
        double EMCal_int_R   = sqrt(EMCal_int_x*EMCal_int_x + EMCal_int_y*EMCal_int_y);

        tower_int_X.push_back(EMCal_int_x);
        tower_int_Y.push_back(EMCal_int_y);
        tower_int_Z.push_back(EMCal_int_z);
        tower_int_R.push_back(EMCal_int_R);
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
        float time = iHCal_tower_calib->get_tower_at_channel(i)->get_time_float(); //get time
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
    EMCal_cluster_innr = findNode::getClass <RawClusterContainer> (topNode, "CLUSTERINNER_CEMC");

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
            if (abs(ptcl->get_pid()) == 11)  // 电子的 PDG ID 是 ±11
            {
                primary_electron_tracks.insert(ptcl->get_track_id());
            }
        }
    }
    NPrimaryG4P_ = PrimaryG4P_PID_.size();
    NPrimaryG4P_promptChargeHadron_ = tmpv_chargehadron.size();

    return Fun4AllReturnCodes::EVENT_OK;
}

// -------------------------------------------------------------------------------------------------------------------------- 
int tutorial::prepareG4HIT(PHCompositeNode * topNode)
{
    if (primary_electron_tracks.empty())
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
        // if (primary_electron_tracks.find(track_id) == primary_electron_tracks.end()) continue;

        // 只选择初级电子的 G4Hit，忽略由 shower 产生的次级粒子
        if (primary_electron_tracks.find(track_id) != primary_electron_tracks.end())
        {
            prhit0 += 1;

            double x0 = this_hit->get_x(0);  // 入口 x
            double y0 = this_hit->get_y(0);  // 入口 y
            double z0 = this_hit->get_z(0);  // 入口 z
            double t0 = this_hit->get_t(0);  // 时间

            double r0 = sqrt(x0*x0 + y0*y0);

            // std::cout << "Primary electron hit0 EMCal at: (x0=" << x0
            //           << ", y0=" << y0 << ", z0=" << z0 << ", t0=" << t0 << ", r0="<< r0 << " )" << std::endl;

            double x1 = this_hit->get_x(1);  // 出口 x
            double y1 = this_hit->get_y(1);  // 出口 y
            double z1 = this_hit->get_z(1);  // 出口 z
            double t1 = this_hit->get_t(1);  // 时间
          
            // std::cout << "Primary electron hit1 EMCal at: (x1=" << x1
            //           << ", y1=" << y1 << ", z1=" << z1 << ", t1=" << t1 << ")" << std::endl;

            double delta_t = t0 - t_earlist;

            if (prhit0 == 1) 
            {
                t_earlist = t0;

                _CEMC_Pr_Hit_x.push_back(x0);
                _CEMC_Pr_Hit_y.push_back(y0);
                _CEMC_Pr_Hit_z.push_back(z0);
                _CEMC_Pr_Hit_R.push_back(r0);

                double PtG_x = x0; 
                double PtG_y = y0; 
                double PtG_r = r0;  
                double PtG_phi = atan2(PtG_y, PtG_x); 
                double PtG_z = z0; 

                // std::cout << "PrG at EMCal surface: (x=" << PtG_x
                //           << ", y=" << PtG_y << ", r=" << PtG_r << ", phi=" << PtG_phi
                //           << ", z=" << PtG_z << ")" << std::endl;
            }
            _CEMC_Pr_Hit_deltaT.push_back(delta_t);

        }
    }

    return Fun4AllReturnCodes::EVENT_OK;
}

int tutorial::createTracksFromTruth(PHCompositeNode* topNode)
{
    // 获取或创建 SvtxTrackMap 节点
    SvtxTrackMap* trackmap = findNode::getClass<SvtxTrackMap>(topNode, "SvtxTrackMap");
    if (!trackmap)
    {
        trackmap = new SvtxTrackMap_v1();
        PHNodeIterator iter(topNode);
        PHCompositeNode* dstNode = dynamic_cast<PHCompositeNode*>(iter.findFirst("PHCompositeNode", "DST"));

        PHIODataNode<PHObject> *truthtracknode = new PHIODataNode<PHObject>(trackmap, "SvtxTrackMap", "PHObject");
        dstNode->addNode(truthtracknode);
    }

    const auto prange = m_truth_info->GetPrimaryParticleRange();
    for (auto iter = prange.first; iter != prange.second; ++iter)
    {
        PHG4Particle* ptcl = iter->second;
        if (!ptcl) continue;

        TLorentzVector p;
        p.SetPxPyPzE(ptcl->get_px(), ptcl->get_py(), ptcl->get_pz(), ptcl->get_e());

        SvtxTrack_v2* track = new SvtxTrack_v2();
        track->set_id(trackmap->size());
        track->set_px(ptcl->get_px());
        track->set_py(ptcl->get_py());
        track->set_pz(ptcl->get_pz());
        track->set_charge(static_cast<int>(TDatabasePDG::Instance()->GetParticle(ptcl->get_pid())->Charge()));
        track->set_chisq(1.0);
        track->set_ndf(1);
        track->set_vertex_id(0);  // 可改为对应 vtx id

        trackmap->insert(track);
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


//____________________________________________________________________________..
int tutorial::process_event(PHCompositeNode * topNode) {

    prepareTracker(topNode);
    prepareEMCal(topNode);
    prepareiHCal(topNode);
    prepareoHCal(topNode);
    prepareEMCalClus(topNode);
    prepareiHCalClus(topNode);
    prepareoHCalClus(topNode);
    
    prepareG4Turth(topNode);
    prepareG4HIT(topNode);

    prepareTruthTrack(topNode);

    // createTracksFromTruth(topNode);

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

    _CEMC_Pr_Hit_x.clear();
    _CEMC_Pr_Hit_y.clear();
    _CEMC_Pr_Hit_z.clear();
    _CEMC_Pr_Hit_R.clear();
    _CEMC_Pr_Hit_deltaT.clear();

    primary_electron_tracks.clear();

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