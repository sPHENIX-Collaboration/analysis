#include "IsolatedTrackAnalysis.h"

// Centrality includes
#include <centrality/CentralityInfo.h>

// Tracking includes
#include <trackbase_historic/SvtxTrack.h>
#include <trackbase_historic/SvtxTrackMap.h>
#include <trackbase_historic/SvtxTrackState.h>
#include <trackbase_historic/SvtxVertexMap.h>

#include <trackbase/TrkrCluster.h>
#include <trackbase/TrkrClusterv3.h>
#include <trackbase/TrkrClusterv4.h>
#include <trackbase/TrkrHit.h>
#include <trackbase/TrkrHitSetContainer.h>
#include <trackbase/TrkrDefs.h>
#include <trackbase/ActsGeometry.h>
#include <trackbase/ClusterErrorPara.h>

#include <trackbase/TpcDefs.h>

#include <trackbase/TrkrClusterContainer.h>
#include <trackbase/TrkrHitSet.h>
#include <trackbase/TrkrClusterHitAssoc.h>
#include <trackbase/TrkrClusterIterationMapv1.h>

// Cluster includes
#include <calobase/RawCluster.h>
#include <calobase/RawClusterContainer.h>
#include <calobase/RawClusterUtility.h>
#include <calobase/RawTowerGeomContainer_Cylinderv1.h>

// Tower includes
#include <calobase/RawTower.h>
#include <calobase/RawTowerContainer.h>
#include <calobase/RawTowerGeom.h>
#include <calobase/RawTowerGeomContainer.h>
#include <calobase/RawTowerDefs.h>

// G4 truth includes
#include <g4eval/SvtxEvalStack.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4Hit.h>
#include <g4main/PHG4HitContainer.h>
#include <g4main/PHG4VtxPoint.h>

// HepMC truth includes
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <HepMC/GenEvent.h>
#pragma GCC diagnostic pop
#include <HepMC/GenVertex.h>
#include <phhepmc/PHHepMCGenEvent.h>
#include <phhepmc/PHHepMCGenEventMap.h>

// Fun4All includes
#include <fun4all/Fun4AllReturnCodes.h>
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

// Acts includes
#include <Acts/Definitions/Algebra.hpp>

// ROOT includes
#include <TFile.h>
#include <TTree.h>

// System includes
#include <iostream>
#include <cmath>
#include <string>
#include <vector>
#include <set>

//____________________________________________________________________________..
IsolatedTrackAnalysis::IsolatedTrackAnalysis(const std::string &name, const std::string &fileName):
  SubsysReco(name),
  m_outputFileName(fileName),
  m_minEMClusterEnergy(0.1),
  m_minIHClusterEnergy(0.012),
  m_minOHClusterEnergy(0.025),
  m_minCemcTowerEnergy(0.04),
  m_minHcalTowerEnergy(0.006),
  m_minSimTowerEnergy(0.0),
  m_analyzeTracks(true),
  m_analyzeClusters(true),
  m_analyzeTowers(true),
  m_analyzeSimTowers(true),
  m_analyzeHepMCTruth(true),
  m_analyzeG4Truth(true),
  m_analyzeCentrality(true),
  m_analyzeAddTruth(true)
{
  initializeTrees();
}

//____________________________________________________________________________..
IsolatedTrackAnalysis::~IsolatedTrackAnalysis()
{
  delete m_tracktree;
  delete m_clustertree;
  delete m_towertree;
  delete m_simtowertree;
  delete m_hepmctree;
  delete m_g4tree;
  delete m_centraltree;
  delete m_addtruthtree;
}

//____________________________________________________________________________..
int IsolatedTrackAnalysis::Init(PHCompositeNode *topNode)
{
  m_outputFile = new TFile(m_outputFileName.c_str(), "RECREATE");

  counter = 0;

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int IsolatedTrackAnalysis::InitRun(PHCompositeNode *topNode)
{
  if(m_analyzeTracks){
    RawTowerGeomContainer_Cylinderv1* cemcGeomContainer = findNode::getClass<RawTowerGeomContainer_Cylinderv1>(topNode, "TOWERGEOM_CEMC");
    RawTowerGeomContainer_Cylinderv1* ihcalGeomContainer = findNode::getClass<RawTowerGeomContainer_Cylinderv1>(topNode, "TOWERGEOM_HCALIN");
    RawTowerGeomContainer_Cylinderv1* ohcalGeomContainer = findNode::getClass<RawTowerGeomContainer_Cylinderv1>(topNode, "TOWERGEOM_HCALOUT");

    if(!cemcGeomContainer){
      std::cout << "ERROR: TOWERGEOM_CEMC not found" << std::endl;
    }
    if(!ihcalGeomContainer){
      std::cout << "ERROR: TOWERGEOM_HCALIN not found" << std::endl;
    }
    if(!ohcalGeomContainer){
      std::cout << "ERROR: TOWERGEOM_HCALOUT not found" << std::endl;
    }

    m_cemcRadius = cemcGeomContainer->get_radius();
    m_ihcalRadius = ihcalGeomContainer->get_radius();
    m_ohcalRadius = ohcalGeomContainer->get_radius();
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int IsolatedTrackAnalysis::process_event(PHCompositeNode *topNode)
{
  if(m_analyzeTracks){ getTracks(topNode); }
  if(m_analyzeClusters){ getClusters(topNode); }
  if(m_analyzeTowers){ getTowers(topNode); }
  if(m_analyzeSimTowers){ getSimTowers(topNode); }
  if(m_analyzeHepMCTruth){ getHepMCTruth(topNode); }
  if(m_analyzeG4Truth){ getG4Truth(topNode); }
  if(m_analyzeCentrality){ getCentrality(topNode); }
  if(m_analyzeAddTruth){ getAddTruth(topNode); }

  counter++;

  //if(counter % 100 == 0)
    std::cout << counter << " events processed" << std::endl;

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int IsolatedTrackAnalysis::ResetEvent(PHCompositeNode *topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int IsolatedTrackAnalysis::End(PHCompositeNode *topNode)
{
  m_outputFile->cd();
  m_tracktree->Write();
  m_clustertree->Write();
  m_towertree->Write();
  m_simtowertree->Write();
  m_hepmctree->Write();
  m_g4tree->Write();
  m_centraltree->Write();
  m_addtruthtree->Write();
  m_outputFile->Close();
  return Fun4AllReturnCodes::EVENT_OK;
}


//////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////
// Function extracting centrality //
////////////////////////////////////

void IsolatedTrackAnalysis::getCentrality(PHCompositeNode *topNode) 
{
  central = findNode::getClass<CentralityInfo>(topNode, "CentralityInfo");
  centrality = central->get_centile(CentralityInfo::PROP::bimp);
  m_centraltree->Fill();
}

////////////////////////////////
// Function extracting tracks //
////////////////////////////////

void IsolatedTrackAnalysis::getTracks(PHCompositeNode *topNode)
{
  SvtxTrackMap *trackmap = findNode::getClass<SvtxTrackMap>(topNode, "SvtxTrackMap");

  // Check whether SvtxTrackMap is present in the node tree or not
  if (!trackmap)
  {
    std::cout << PHWHERE
         << "SvtxTrackMap node is missing, can't collect tracks"
         << std::endl;
    return;
  }

  SvtxVertexMap *vertexmap = findNode::getClass<SvtxVertexMap>(topNode, "SvtxVertexMap");

  // Check whether SvtxTrackMap is present in the node tree or not
  if (!vertexmap)
  {
    std::cout << PHWHERE
         << "SvtxVertexMap node is missing, can't collect tracks"
         << std::endl;
    return;
  }

  // EvalStack for truth track matching
  if(!m_svtxEvalStack){ m_svtxEvalStack = new SvtxEvalStack(topNode); }
  
  m_svtxEvalStack->next_event(topNode);

  // Get the track evaluator
  SvtxTrackEval *trackeval = m_svtxEvalStack->get_track_eval();

  // Get the range for primary tracks
  PHG4TruthInfoContainer *truthinfo = findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");

  m_trkmult = 0;
  // Looping over tracks in an event
  for(auto entry : *trackmap)
  {
    SvtxTrack *track = entry.second;

    m_tr_p[m_trkmult] = track->get_p();
    m_tr_pt[m_trkmult] = track->get_pt();
    m_tr_eta[m_trkmult] = track->get_eta();
    m_tr_phi[m_trkmult] = track->get_phi();
  
    m_tr_charge[m_trkmult] = track->get_charge();
    m_tr_chisq[m_trkmult] = track->get_chisq();
    m_tr_ndf[m_trkmult] = track->get_ndf();
    m_tr_silicon_hits[m_trkmult] = track->get_silicon_seed()->size_cluster_keys();

    float dca_xy, dca_z, dca_xy_error, dca_z_error;
    calculateDCA(track, vertexmap, dca_xy, dca_z, dca_xy_error, dca_z_error);

    m_tr_dca_xy[m_trkmult] = dca_xy;
    m_tr_dca_xy_error[m_trkmult] = dca_xy_error;
    m_tr_dca_z[m_trkmult] = dca_z;
    m_tr_dca_z_error[m_trkmult] = dca_z_error;

    m_tr_x[m_trkmult] = track->get_x();
    m_tr_y[m_trkmult] = track->get_y();
    m_tr_z[m_trkmult] = track->get_z();

    m_tr_vertex_id[m_trkmult] = track->get_vertex_id();

    // Project tracks to CEMC
    if(track->find_state(m_cemcRadius) != track->end_states()){
      m_tr_cemc_eta[m_trkmult] = calculateProjectionEta( track->find_state(m_cemcRadius)->second);
      m_tr_cemc_phi[m_trkmult] = calculateProjectionPhi( track->find_state(m_cemcRadius)->second);
    }
    else{
      m_tr_cemc_eta[m_trkmult] = -999;
      m_tr_cemc_phi[m_trkmult] = -999;
    }
    
    // Project tracks to inner HCAL
    if(track->find_state(m_ihcalRadius) != track->end_states()){
      m_tr_ihcal_eta[m_trkmult] = calculateProjectionEta( track->find_state(m_ihcalRadius)->second);
      m_tr_ihcal_phi[m_trkmult] = calculateProjectionPhi( track->find_state(m_ihcalRadius)->second);
    }
    else{
      m_tr_ihcal_eta[m_trkmult] = -999;
      m_tr_ihcal_phi[m_trkmult] = -999;
    }

    // Project tracks to outer HCAL
    if(track->find_state(m_ohcalRadius) != track->end_states()){
      m_tr_ohcal_eta[m_trkmult] = calculateProjectionEta( track->find_state(m_ohcalRadius)->second);
      m_tr_ohcal_phi[m_trkmult] = calculateProjectionPhi( track->find_state(m_ohcalRadius)->second);
    }
    else{
      m_tr_ohcal_eta[m_trkmult] = -999;
      m_tr_ohcal_phi[m_trkmult] = -999;
    }
    
    // Matching SvtxTracks to G4 truth
    //std::cout << "before truth matching" << std::endl;
    PHG4Particle *truthtrack = trackeval->max_truth_particle_by_nclusters(track);
    //std::cout << "after truth matching" << std::endl;

    if(truthtrack){
      //std::cout << "found truth track" << std::endl;
      m_tr_truth_pid[m_trkmult] =truthtrack->get_pid();
      m_tr_truth_is_primary[m_trkmult] =truthinfo->is_primary(truthtrack);
  
      m_tr_truth_e[m_trkmult] =truthtrack->get_e();

      float px = truthtrack->get_px();
      float py = truthtrack->get_py();
      float pz = truthtrack->get_pz();

      m_tr_truth_pt[m_trkmult] = sqrt(px*px+py*py);
      m_tr_truth_phi[m_trkmult] = atan2(py, px);
      m_tr_truth_eta[m_trkmult] = atanh(pz/sqrt(px*px+py*py+pz*pz));
      m_tr_truth_track_id[m_trkmult] = truthtrack->get_track_id();
    }
    else{
      //std::cout << "truth track null" << std::endl;
      m_tr_truth_pid[m_trkmult] = -999;
      m_tr_truth_is_primary[m_trkmult] = -999;
      m_tr_truth_e[m_trkmult] = -999;
      m_tr_truth_pt[m_trkmult] = -999;
      m_tr_truth_eta[m_trkmult] = -999;
      m_tr_truth_phi[m_trkmult] = -999;
      m_tr_truth_track_id[m_trkmult] = -999;
    }

    m_trkmult++;
  }

  m_vtxmult = 0;
  for(auto entry : *vertexmap)
  {
    SvtxVertex *vertex = entry.second;
    
    m_vertex_id[m_vtxmult] = vertex->get_id();
    m_vx[m_vtxmult] = vertex->get_x();
    m_vy[m_vtxmult] = vertex->get_y();
    m_vz[m_vtxmult] = vertex->get_z();
    m_vtxmult++;
  }

  m_tracktree->Fill();
}

//////////////////////////////////
// Function extracting clusters //
//////////////////////////////////
void IsolatedTrackAnalysis::getClusters(PHCompositeNode *topNode){
  
  RawClusterContainer *cemcContainer = findNode::getClass<RawClusterContainer>(topNode,"CLUSTER_CEMC"); 
  
  // Check whether CLUSTER_CEMC is present in the node tree or not
  if (!cemcContainer)
  {
    std::cout << PHWHERE
         << "CLUSTER_CEMC node is missing, can't collect EMCal clusters"
         << std::endl;
    return;
  }

  RawClusterContainer::Map cemcMap = cemcContainer->getClustersMap();
  
  m_clsmult_cemc = 0;
  for(auto entry : cemcMap){
    RawCluster* cluster = entry.second;

    if(cluster->get_energy() > m_minEMClusterEnergy){
      // calculating eta
      // we need the nominal eta, with respect to the origin, not the vertex!
      CLHEP::Hep3Vector origin(0, 0, 0);
      CLHEP::Hep3Vector cluster_vector = RawClusterUtility::GetECoreVec(*cluster, origin);
       
      m_cl_cemc_e[m_clsmult_cemc] = cluster->get_energy();
      m_cl_cemc_eta[m_clsmult_cemc] = cluster_vector.pseudoRapidity();
      m_cl_cemc_phi[m_clsmult_cemc] = cluster->get_phi();
      m_cl_cemc_r[m_clsmult_cemc] = cluster->get_r();
      m_cl_cemc_z[m_clsmult_cemc] = cluster->get_z();

      m_clsmult_cemc++;  
    }
  }

  RawClusterContainer *ihcalContainer = findNode::getClass<RawClusterContainer>(topNode,"CLUSTER_HCALIN"); 
  
   // Check whether CLUSTER_HCALIN is present in the node tree or not
  if (!ihcalContainer)
  {
    std::cout << PHWHERE
         << "CLUSTER_HCALIN node is missing, can't collect EMCal clusters"
         << std::endl;
    return;
  }

  RawClusterContainer::Map ihcalMap = ihcalContainer->getClustersMap();
  
  m_clsmult_ihcal = 0;
  for(auto entry : ihcalMap){
    RawCluster* cluster = entry.second;

    if(cluster->get_energy() > m_minIHClusterEnergy){
      // calculating eta
      // we need the nominal eta, with respect to the origin, not the vertex!
      CLHEP::Hep3Vector origin(0, 0, 0);
      CLHEP::Hep3Vector cluster_vector = RawClusterUtility::GetECoreVec(*cluster, origin);
      
      m_cl_ihcal_e[m_clsmult_ihcal] = cluster->get_energy();
      m_cl_ihcal_eta[m_clsmult_ihcal] = cluster_vector.pseudoRapidity();
      m_cl_ihcal_phi[m_clsmult_ihcal] = cluster->get_phi();
      m_cl_ihcal_r[m_clsmult_ihcal] = cluster->get_r();
      m_cl_ihcal_z[m_clsmult_ihcal] = cluster->get_z();
        
      m_clsmult_ihcal++;
    }
  } 
  
  RawClusterContainer *ohcalContainer = findNode::getClass<RawClusterContainer>(topNode,"CLUSTER_HCALOUT");   

   // Check whether CLUSTER_HCALOUT is present in the node tree or not
  if (!ohcalContainer)
  {
    std::cout << PHWHERE
         << "CLUSTER_HCALOUT node is missing, can't collect EMCal clusters"
         << std::endl;
    return;
  }

  RawClusterContainer::Map ohcalMap = ohcalContainer->getClustersMap();
  
  m_clsmult_ohcal = 0;
  for(auto entry : ohcalMap){
    RawCluster* cluster = entry.second;

    if(cluster->get_energy() > m_minOHClusterEnergy){
      // calculating eta
      // we need the nominal eta, with respect to the origin, not the vertex!
      CLHEP::Hep3Vector origin(0, 0, 0);
      CLHEP::Hep3Vector cluster_vector = RawClusterUtility::GetECoreVec(*cluster, origin);
 
      m_cl_ohcal_e[m_clsmult_ohcal] = cluster->get_energy();
      m_cl_ohcal_eta[m_clsmult_ohcal] = cluster_vector.pseudoRapidity();
      m_cl_ohcal_phi[m_clsmult_ohcal] = cluster->get_phi();
      m_cl_ohcal_r[m_clsmult_ohcal] = cluster->get_r();
      m_cl_ohcal_z[m_clsmult_ohcal] = cluster->get_z();
    
      m_clsmult_ohcal++;  
    }   
  }
  
  m_clustertree->Fill();

}

////////////////////////////////
// Function extracting towers //
////////////////////////////////

void IsolatedTrackAnalysis::getTowers(PHCompositeNode *topNode) {

  m_twrmult_cemc = 0;

  RawTowerContainer *cemcTowerContainer = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_CEMC");
  RawTowerGeomContainer *cemcGeom = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_CEMC");

  if(cemcTowerContainer && cemcGeom)
  {
    RawTowerContainer::ConstRange range = cemcTowerContainer->getTowers();
    for(RawTowerContainer::ConstIterator it = range.first; it != range.second; it++){
      RawTower *tower = it->second;

	    RawTowerGeom *tower_geom = cemcGeom->get_tower_geometry(tower->get_key()); 
	    if(tower->get_energy() > m_minCemcTowerEnergy)
	    {
	      m_twr_cemc_e[m_twrmult_cemc] = tower->get_energy();
        m_twr_cemc_eta[m_twrmult_cemc] = tower_geom->get_eta();
	      m_twr_cemc_phi[m_twrmult_cemc] = tower_geom->get_phi();
        m_twr_cemc_ieta[m_twrmult_cemc] = tower_geom->get_bineta();
	      m_twr_cemc_iphi[m_twrmult_cemc] = tower_geom->get_binphi();
	      m_twrmult_cemc++;
 	    }
    }
  }


  m_twrmult_ihcal = 0;

  RawTowerContainer *ihcalTowerContainer = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_HCALIN");
  RawTowerGeomContainer *ihcalGeom = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALIN");

  if(ihcalTowerContainer && ihcalGeom)
  {
    RawTowerContainer::ConstRange range = ihcalTowerContainer->getTowers();
    for(RawTowerContainer::ConstIterator it = range.first; it != range.second; it++){
      RawTower *tower = it->second;

	    RawTowerGeom *tower_geom = ihcalGeom->get_tower_geometry(tower->get_key()); 
	    if(tower->get_energy() > m_minHcalTowerEnergy)
	    {
	      m_twr_ihcal_e[m_twrmult_ihcal] = tower->get_energy();
        m_twr_ihcal_eta[m_twrmult_ihcal] = tower_geom->get_eta();
	      m_twr_ihcal_phi[m_twrmult_ihcal] = tower_geom->get_phi();
        m_twr_ihcal_ieta[m_twrmult_ihcal] = tower_geom->get_bineta();
	      m_twr_ihcal_iphi[m_twrmult_ihcal] = tower_geom->get_binphi();
	      m_twrmult_ihcal++;
 	    }
    }
  }


  m_twrmult_ohcal = 0;

  RawTowerContainer *ohcalTowerContainer = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_HCALOUT");
  RawTowerGeomContainer *ohcalGeom = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALOUT");

  if(ohcalTowerContainer && ohcalGeom)
  {
    RawTowerContainer::ConstRange range = ohcalTowerContainer->getTowers();
    for(RawTowerContainer::ConstIterator it = range.first; it != range.second; it++){
      RawTower *tower = it->second;

	    RawTowerGeom *tower_geom = ohcalGeom->get_tower_geometry(tower->get_key()); 
	    if(tower->get_energy() > m_minHcalTowerEnergy)
	    {
	      m_twr_ohcal_e[m_twrmult_ohcal] = tower->get_energy();
        m_twr_ohcal_eta[m_twrmult_ohcal] = tower_geom->get_eta();
	      m_twr_ohcal_phi[m_twrmult_ohcal] = tower_geom->get_phi();
        m_twr_ohcal_ieta[m_twrmult_ohcal] = tower_geom->get_bineta();
	      m_twr_ohcal_iphi[m_twrmult_ohcal] = tower_geom->get_binphi();
	      m_twrmult_ohcal++;
 	    }
    }
  }

  m_towertree->Fill();
 
}

////////////////////////////////
// Function extracting towers //
////////////////////////////////

void IsolatedTrackAnalysis::getSimTowers(PHCompositeNode *topNode) {

  m_simtwrmult_cemc = 0;

  RawTowerContainer *cemcTowerContainer = findNode::getClass<RawTowerContainer>(topNode, "TOWER_SIM_CEMC");
  RawTowerGeomContainer *cemcGeom = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_CEMC");

  if(cemcTowerContainer && cemcGeom)
  {
    RawTowerContainer::ConstRange range = cemcTowerContainer->getTowers();
    for(RawTowerContainer::ConstIterator it = range.first; it != range.second; it++){
      RawTower *tower = it->second;

	    RawTowerGeom *tower_geom = cemcGeom->get_tower_geometry(tower->get_key()); 
	    if(tower->get_energy() > m_minSimTowerEnergy)
	    {
	      m_simtwr_cemc_e[m_simtwrmult_cemc] = tower->get_energy();
        m_simtwr_cemc_eta[m_simtwrmult_cemc] = tower_geom->get_eta();
	      m_simtwr_cemc_phi[m_simtwrmult_cemc] = tower_geom->get_phi();
        m_simtwr_cemc_ieta[m_simtwrmult_cemc] = tower_geom->get_bineta();
	      m_simtwr_cemc_iphi[m_simtwrmult_cemc] = tower_geom->get_binphi();
	      m_simtwrmult_cemc++;
 	    }
    }
  }


  m_simtwrmult_ihcal = 0;

  RawTowerContainer *ihcalTowerContainer = findNode::getClass<RawTowerContainer>(topNode, "TOWER_SIM_HCALIN");
  RawTowerGeomContainer *ihcalGeom = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALIN");

  if(ihcalTowerContainer && ihcalGeom)
  {
    RawTowerContainer::ConstRange range = ihcalTowerContainer->getTowers();
    for(RawTowerContainer::ConstIterator it = range.first; it != range.second; it++){
      RawTower *tower = it->second;

	    RawTowerGeom *tower_geom = ihcalGeom->get_tower_geometry(tower->get_key()); 
	    if(tower->get_energy() > m_minSimTowerEnergy)
	    {
	      m_simtwr_ihcal_e[m_simtwrmult_ihcal] = tower->get_energy();
        m_simtwr_ihcal_eta[m_simtwrmult_ihcal] = tower_geom->get_eta();
	      m_simtwr_ihcal_phi[m_simtwrmult_ihcal] = tower_geom->get_phi();
        m_simtwr_ihcal_ieta[m_simtwrmult_ihcal] = tower_geom->get_bineta();
	      m_simtwr_ihcal_iphi[m_simtwrmult_ihcal] = tower_geom->get_binphi();
	      m_simtwrmult_ihcal++;
 	    }
    }
  }

  m_simtwrmult_ohcal = 0;

  RawTowerContainer *ohcalTowerContainer = findNode::getClass<RawTowerContainer>(topNode, "TOWER_SIM_HCALOUT");
  RawTowerGeomContainer *ohcalGeom = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALOUT");

  if(ohcalTowerContainer && ohcalGeom)
  {
    RawTowerContainer::ConstRange range = ohcalTowerContainer->getTowers();
    for(RawTowerContainer::ConstIterator it = range.first; it != range.second; it++){
      RawTower *tower = it->second;

	    RawTowerGeom *tower_geom = ohcalGeom->get_tower_geometry(tower->get_key()); 
	    if(tower->get_energy() > m_minSimTowerEnergy)
	    {
	      m_simtwr_ohcal_e[m_simtwrmult_ohcal] = tower->get_energy();
        m_simtwr_ohcal_eta[m_simtwrmult_ohcal] = tower_geom->get_eta();
	      m_simtwr_ohcal_phi[m_simtwrmult_ohcal] = tower_geom->get_phi();
        m_simtwr_ohcal_ieta[m_simtwrmult_ohcal] = tower_geom->get_bineta();
	      m_simtwr_ohcal_iphi[m_simtwrmult_ohcal] = tower_geom->get_binphi();
	      m_simtwrmult_ohcal++;
 	    }
    }
  }

  m_simtowertree->Fill(); 
}



////////////////////////////////////////////
// Functions extracting truth information //
////////////////////////////////////////////

void IsolatedTrackAnalysis::getHepMCTruth(PHCompositeNode *topNode) {

  PHHepMCGenEventMap *hepmceventmap = findNode::getClass<PHHepMCGenEventMap>(topNode, "PHHepMCGenEventMap");
  if (!hepmceventmap) std::cout << PHWHERE << "HEPMC event map node is missing, can't collected HEPMC truth particles"<< std::endl;

  for (PHHepMCGenEventMap::ConstIter eventIter = hepmceventmap->begin();
         eventIter != hepmceventmap->end(); ++eventIter) {
     
    /// Get the event
    PHHepMCGenEvent *hepmcevent = eventIter->second;
      
    // To fill TTree, require that the event be the primary event (embedding_id > 0)
    if (hepmcevent && hepmcevent->get_embedding_id() == 0)
    {
      /// Get the event characteristics, inherited from HepMC classes
      HepMC::GenEvent *truthevent = hepmcevent->getEvent();
      if (!truthevent) {
        std::cout << PHWHERE
             << "no evt pointer under phhepmvgeneventmap found "
             << std::endl;
      }

      /// Loop over all the truth particles and get their information
      m_hepmc = 0;
      for (HepMC::GenEvent::particle_const_iterator iter = truthevent->particles_begin();
           iter != truthevent->particles_end(); ++iter) {

        m_hepmc_e[m_hepmc] = (*iter)->momentum().e();
        m_hepmc_pid[m_hepmc] = (*iter)->pdg_id();
        m_hepmc_eta[m_hepmc] = (*iter)->momentum().pseudoRapidity();
        m_hepmc_phi[m_hepmc] = (*iter)->momentum().phi();
        m_hepmc_pt[m_hepmc] = sqrt((*iter)->momentum().px() * (*iter)->momentum().px() 
                            + (*iter)->momentum().py() * (*iter)->momentum().py());
        /// Fill the truth tree
        m_hepmc++;
        if (m_hepmc >= 20000) break;
      }
      break;
    } 
  }

  m_hepmctree->Fill();

}

void IsolatedTrackAnalysis::getG4Truth(PHCompositeNode *topNode) {

  PHG4TruthInfoContainer *truthinfo = findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");
  if (!truthinfo) std::cout << PHWHERE << "PHG4TruthInfoContainer node is missing, can't collect G4 truth particles"<< std::endl;

  PHG4TruthInfoContainer::Range range = truthinfo->GetPrimaryParticleRange();
  m_g4 = 0;
  for (PHG4TruthInfoContainer::ConstIterator iter = range.first;
       iter != range.second; ++iter) {

    // Get truth particle
    const PHG4Particle *truth = iter->second;
    if (!truthinfo->is_primary(truth)) continue;

    /// Get this particles momentum, etc.
    m_g4_pt[m_g4] = sqrt(truth->get_px() * truth->get_px()
                            + truth->get_py() * truth->get_py());
    m_g4_e[m_g4] = truth->get_e();
    m_g4_phi[m_g4] = atan2(truth->get_py(), truth->get_px());
    m_g4_eta[m_g4] = atanh(truth->get_pz() / truth->get_e());
    if (m_g4_eta[m_g4] != m_g4_eta[m_g4]) m_g4_eta[m_g4] = -999; // check for nans
    m_g4_pid[m_g4] = truth->get_pid();
    m_g4_track_id[m_g4] = truth->get_track_id();
    m_g4_parent_id[m_g4] = truth->get_parent_id();
    m_g4++;
    if (m_g4 >= 20000) break;
  }

  PHG4TruthInfoContainer::Range second_range = truthinfo->GetSecondaryParticleRange();

  for (PHG4TruthInfoContainer::ConstIterator siter = second_range.first;
        siter != second_range.second; ++siter) {
    if (m_g4 >= 19999) break;
    // Get photons from pi0 decays 
    const PHG4Particle *truth = siter->second;
    if (truth->get_pid() == 22) {
      PHG4Particle *parent = truthinfo->GetParticle(truth->get_parent_id());
      if (parent->get_pid() == 111 && parent->get_track_id() > 0) {
        m_g4_pt[m_g4] = sqrt(truth->get_px() * truth->get_px()
                          + truth->get_py() * truth->get_py());
        m_g4_e[m_g4] = truth->get_e();
        m_g4_phi[m_g4] = atan2(truth->get_py(), truth->get_px());
        m_g4_eta[m_g4] = atanh(truth->get_pz() / truth->get_e());
        if (m_g4_eta[m_g4] != m_g4_eta[m_g4]) m_g4_eta[m_g4] = -999; // check for nans
        m_g4_pid[m_g4] = truth->get_pid();
        m_g4_track_id[m_g4] = truth->get_track_id();
        m_g4_parent_id[m_g4] = truth->get_parent_id();
        m_g4++;
      }
    }
  }

  m_g4tree->Fill();

}

///////////////////////////////////////////////////////
// Functions extracting additional truth information //
///////////////////////////////////////////////////////

void IsolatedTrackAnalysis::getAddTruth(PHCompositeNode *topNode) {

  PHG4TruthInfoContainer *truthinfo = findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");
  if (!truthinfo) std::cout << PHWHERE << "PHG4TruthInfoContainer node is missing, can't collect additional truth particles"<< std::endl;
  PHG4TruthInfoContainer::Range second_range = truthinfo->GetSecondaryParticleRange();

  n_child = 0;
  std::set<int> vertex;
  if (!vertex.empty()) vertex.clear();
  for (PHG4TruthInfoContainer::ConstIterator iter = second_range.first; iter != second_range.second; ++iter) {
    const PHG4Particle *child = iter->second;
    if (child->get_parent_id() > 0) {
      vertex.insert(child->get_vtx_id());
      child_pid[n_child] = child->get_pid();
      child_parent_id[n_child] = child->get_parent_id();
      child_vertex_id[n_child] = child->get_vtx_id();
      child_px[n_child] = child->get_px();
      child_py[n_child] = child->get_py();
      child_pz[n_child] = child->get_pz();
      child_energy[n_child] = child->get_e();
      n_child++;
      if (n_child >= 100000) break;
    }
  }
  
  PHG4TruthInfoContainer::VtxRange vtxrange = truthinfo->GetVtxRange();
  n_vertex = 0;
  for (PHG4TruthInfoContainer::ConstVtxIterator iter = vtxrange.first; iter != vtxrange.second; ++iter) {
    PHG4VtxPoint *vtx = iter->second;
    if (vertex.find(vtx->get_id()) != vertex.end()) {
      vertex_x[n_vertex] = vtx->get_x();
      vertex_y[n_vertex] = vtx->get_y();
      vertex_z[n_vertex] = vtx->get_z();
      vertex_id[n_vertex] = vtx->get_id();
      n_vertex++;
      if (n_vertex >= 100000) break;
    }
  }

  PHG4HitContainer* blackhole = findNode::getClass<PHG4HitContainer>(topNode, "G4HIT_BH_1");
  if (!blackhole) std::cout << "No blackhole" << std::endl;

  _nBH = 0;
  PHG4HitContainer::ConstRange bh_hit_range = blackhole->getHits();
  for (PHG4HitContainer::ConstIterator hit_iter = bh_hit_range.first;
         hit_iter != bh_hit_range.second; hit_iter++) {
    PHG4Hit *this_hit = hit_iter->second;
    assert(this_hit);
    _BH_e[_nBH] = this_hit->get_edep();
    _BH_px[_nBH] = this_hit->get_px(0);
    _BH_py[_nBH] = this_hit->get_py(0);
    _BH_pz[_nBH] = this_hit->get_pz(0);
    PHG4Particle *p = truthinfo->GetParticle(this_hit->get_trkid());
    _BH_track_id[_nBH] = p->get_track_id();
    _nBH++;
  }

  m_addtruthtree->Fill();
}

////////////////////////////
// Track helper functions //
////////////////////////////

float IsolatedTrackAnalysis::calculateProjectionEta(SvtxTrackState* projectedState){
  float x = projectedState->get_x();// - initialState->get_x();
  float y = projectedState->get_y();// - initialState->get_y();
  float z = projectedState->get_z();// - initialState->get_z();

  float theta = acos(z / sqrt(x*x + y*y + z*z) );

  return -log( tan(theta/2.0) );
}

float IsolatedTrackAnalysis::calculateProjectionPhi(SvtxTrackState* projectedState){
  float x = projectedState->get_x();// - initialState->get_x();
  float y = projectedState->get_y();// - initialState->get_y();
 
  return atan2(y,x);
}

// From SvtxEval.cc
void IsolatedTrackAnalysis::calculateDCA(SvtxTrack* track, SvtxVertexMap* vertexmap,
          float& dca3dxy, float& dca3dz,
          float& dca3dxysigma, float& dca3dzsigma)
{
  Acts::Vector3 pos(track->get_x(),
        track->get_y(),
        track->get_z());
  Acts::Vector3 mom(track->get_px(),
        track->get_py(),
        track->get_pz());

  auto vtxid = track->get_vertex_id();
  auto svtxVertex = vertexmap->get(vtxid);
  if(!svtxVertex)
    { return; }
  Acts::Vector3 vertex(svtxVertex->get_x(),
           svtxVertex->get_y(),
           svtxVertex->get_z());

  pos -= vertex;

  Acts::ActsSymMatrix<3> posCov;
  for(int i = 0; i < 3; ++i)
    {
      for(int j = 0; j < 3; ++j)
  {
    posCov(i, j) = track->get_error(i, j);
  } 
    }
  
  Acts::Vector3 r = mom.cross(Acts::Vector3(0.,0.,1.));
  float phi = atan2(r(1), r(0));
  
  Acts::RotationMatrix3 rot;
  Acts::RotationMatrix3 rot_T;
  rot(0,0) = cos(phi);
  rot(0,1) = -sin(phi);
  rot(0,2) = 0;
  rot(1,0) = sin(phi);
  rot(1,1) = cos(phi);
  rot(1,2) = 0;
  rot(2,0) = 0;
  rot(2,1) = 0;
  rot(2,2) = 1;
  
  rot_T = rot.transpose();

  Acts::Vector3 pos_R = rot * pos;
  Acts::ActsSymMatrix<3> rotCov = rot * posCov * rot_T;

  dca3dxy = pos_R(0);
  dca3dz = pos_R(2);
  dca3dxysigma = sqrt(rotCov(0,0));
  dca3dzsigma = sqrt(rotCov(2,2));
  
}


///////////////////////////////////
// Function to set tree branches //
///////////////////////////////////

void IsolatedTrackAnalysis::initializeTrees()
{
  //////////////////
  // Central tree //
  //////////////////
  m_centraltree = new TTree("centraltree", "Tree of centralities");
  m_centraltree->Branch("centrality", &centrality, "centrality/F");
  
  ////////////////
  // Track tree //
  ////////////////
  m_tracktree = new TTree("tracktree", "Tree of svtx tracks");
  
  m_tracktree->Branch("m_trkmult", &m_trkmult, "m_trkmult/I");

  // Basic properties
  m_tracktree->Branch("m_tr_p",  m_tr_p, "m_tr_p[m_trkmult]/F");
  m_tracktree->Branch("m_tr_pt",  m_tr_pt, "m_tr_pt[m_trkmult]/F");
  m_tracktree->Branch("m_tr_eta", m_tr_eta, "m_tr_eta[m_trkmult]/F");
  m_tracktree->Branch("m_tr_phi", m_tr_phi, "m_tr_phi[m_trkmult]/F");
  m_tracktree->Branch("m_tr_charge", m_tr_charge, "m_tr_charge[m_trkmult]/I");
  m_tracktree->Branch("m_tr_chisq",  m_tr_chisq, "m_tr_chisq[m_trkmult]/F");
  m_tracktree->Branch("m_tr_ndf",    m_tr_ndf, "m_tr_ndf[m_trkmult]/I");
  m_tracktree->Branch("m_tr_silicon_hits",    m_tr_silicon_hits, "m_tr_silicon_hits[m_trkmult]/I");
 
  // Distance of closest approach
  m_tracktree->Branch("m_tr_dca_xy", m_tr_dca_xy, "m_tr_dca_xy[m_trkmult]/F");
  m_tracktree->Branch("m_tr_dca_xy_error", m_tr_dca_xy_error, "m_tr_dc_xy_error[m_trkmult]/F");
  m_tracktree->Branch("m_tr_dca_z",  m_tr_dca_z, "m_tr_dca_z[m_trkmult]/F");
  m_tracktree->Branch("m_tr_dca_z_error", m_tr_dca_z_error, "m_tr_dca_z_error[m_trkmult]/F");

  // Initial point of track
  m_tracktree->Branch("m_tr_x", m_tr_x, "m_tr_x[m_trkmult]/F");
  m_tracktree->Branch("m_tr_y", m_tr_y, "m_tr_y[m_trkmult]/F");
  m_tracktree->Branch("m_tr_z", m_tr_z, "m_tr_z[m_trkmult]/F");

  // Vertex id of track
  m_tracktree->Branch("m_tr_vertex_id", m_tr_vertex_id, "m_tr_vertex_id[m_trkmult]/I");
  
  // Vertex ids and positions, also stored on track tree
  m_tracktree->Branch("m_vtxmult", &m_vtxmult, "m_vtxmult/I");
  m_tracktree->Branch("m_vertex_id", m_vertex_id, "m_vertex_id[m_vtxmult]/I");
  m_tracktree->Branch("m_vx", m_vx, "m_vx[m_vtxmult]/F");
  m_tracktree->Branch("m_vy", m_vy, "m_vy[m_vtxmult]/F");
  m_tracktree->Branch("m_vz", m_vz, "m_vz[m_vtxmult]/F");

  // Projection of track to calorimeters
  // CEMC
  m_tracktree->Branch("m_tr_cemc_eta", m_tr_cemc_eta, "m_tr_cemc_eta[m_trkmult]/F");
  m_tracktree->Branch("m_tr_cemc_phi", m_tr_cemc_phi, "m_tr_cemc_phi[m_trkmult]/F");
  // Inner HCAL
  m_tracktree->Branch("m_tr_ihcal_eta", m_tr_ihcal_eta, "m_tr_ihcal_eta[m_trkmult]/F");
  m_tracktree->Branch("m_tr_ihcal_phi", m_tr_ihcal_phi, "m_tr_ihcal_phi[m_trkmult]/F");
  /// Outer HCAL
  m_tracktree->Branch("m_tr_ohcal_eta", m_tr_ohcal_eta, "m_tr_ohcal_eta[m_trkmult]/F");
  m_tracktree->Branch("m_tr_ohcal_phi", m_tr_ohcal_phi, "m_tr_ohcal_phi[m_trkmult]/F");

  // Matched truth track
  m_tracktree->Branch("m_tr_truth_pid", m_tr_truth_pid, "m_tr_truth_pid[m_trkmult]/I");
  m_tracktree->Branch("m_tr_truth_is_primary",m_tr_truth_is_primary,"m_tr_truth_is_primary[m_trkmult]/I");
  m_tracktree->Branch("m_tr_truth_e", m_tr_truth_e, "m_tr_truth_e[m_trkmult]/F");
  m_tracktree->Branch("m_tr_truth_pt", m_tr_truth_pt, "m_tr_truth_pt[m_trkmult]/F");
  m_tracktree->Branch("m_tr_truth_eta", m_tr_truth_eta, "m_tr_truth_eta[m_trkmult]/F");
  m_tracktree->Branch("m_tr_truth_phi", m_tr_truth_phi, "m_tr_truth_phi[m_trkmult]/F");
  m_tracktree->Branch("m_tr_truth_track_id", m_tr_truth_track_id, "m_tr_truth_track_id[m_trkmult]/I");

  //////////////////
  // Cluster tree //
  //////////////////
  m_clustertree = new TTree("clustertree", "Tree of raw clusters");
 
  // CEMC clusters
  m_clustertree->Branch("m_clsmult_cemc", &m_clsmult_cemc, "m_clsmult_cemc/I");
  m_clustertree->Branch("m_cl_cemc_e", m_cl_cemc_e, "m_cl_cemc_e[m_clsmult_cemc]/F");
  m_clustertree->Branch("m_cl_cemc_eta", m_cl_cemc_eta, "m_cl_cemc_eta[m_clsmult_cemc]/F");
  m_clustertree->Branch("m_cl_cemc_phi", m_cl_cemc_phi, "m_cl_cemc_phi[m_clsmult_cemc]/F");
  m_clustertree->Branch("m_cl_cemc_r", m_cl_cemc_r, "m_cl_cemc_r[m_clsmult_cemc]/F");
  m_clustertree->Branch("m_cl_cemc_z", m_cl_cemc_z, "m_cl_cemc_z[m_clsmult_cemc]/F");

  // Inner HCAL clusters
  m_clustertree->Branch("m_clsmult_ihcal", &m_clsmult_ihcal, "m_clsmult_ihcal/I");
  m_clustertree->Branch("m_cl_ihcal_e", m_cl_ihcal_e, "m_cl_ihcal_e[m_clsmult_ihcal]/F");
  m_clustertree->Branch("m_cl_ihcal_eta", m_cl_ihcal_eta, "m_cl_ihcal_eta[m_clsmult_ihcal]/F");
  m_clustertree->Branch("m_cl_ihcal_phi", m_cl_ihcal_phi, "m_cl_ihcal_phi[m_clsmult_ihcal]/F");
  m_clustertree->Branch("m_cl_ihcal_r", m_cl_ihcal_r, "m_cl_ihcal_r[m_clsmult_ihcal]/F");
  m_clustertree->Branch("m_cl_ihcal_z", m_cl_ihcal_z, "m_cl_ihcal_z[m_clsmult_ihcal]/F");

  // Outer HCAL clusters
  m_clustertree->Branch("m_clsmult_ohcal", &m_clsmult_ohcal, "m_clsmult_ohcal/I");
  m_clustertree->Branch("m_cl_ohcal_e", m_cl_ohcal_e, "m_cl_ohcal_e[m_clsmult_ohcal]/F");
  m_clustertree->Branch("m_cl_ohcal_eta", m_cl_ohcal_eta, "m_cl_ohcal_eta[m_clsmult_ohcal]/F");
  m_clustertree->Branch("m_cl_ohcal_phi", m_cl_ohcal_phi, "m_cl_ohcal_phi[m_clsmult_ohcal]/F");
  m_clustertree->Branch("m_cl_ohcal_r", m_cl_ohcal_r, "m_cl_ohcal_r[m_clsmult_ohcal]/F");
  m_clustertree->Branch("m_cl_ohcal_z", m_cl_ohcal_z, "m_cl_ohcal_z[m_clsmult_ohcal]/F");

  ////////////////
  // Tower tree //
  ////////////////
  m_towertree = new TTree("towertree", "Tree of raw towers");

  // CEMC towers
  m_towertree->Branch("m_twrmult_cemc", &m_twrmult_cemc, "m_twrmult_cemc/I");
  m_towertree->Branch("m_twr_cemc_e", m_twr_cemc_e, "m_twr_cemc_e[m_twrmult_cemc]/F");
  m_towertree->Branch("m_twr_cemc_eta", m_twr_cemc_eta, "m_twr_cemc_eta[m_twrmult_cemc]/F");
  m_towertree->Branch("m_twr_cemc_phi", m_twr_cemc_phi, "m_twr_cemc_phi[m_twrmult_cemc]/F");
  m_towertree->Branch("m_twr_cemc_ieta", m_twr_cemc_ieta, "m_twr_cemc_ieta[m_twrmult_cemc]/I");
  m_towertree->Branch("m_twr_cemc_iphi", m_twr_cemc_iphi, "m_twr_cemc_iphi[m_twrmult_cemc]/I");

  // Inner HCAL towers
  m_towertree->Branch("m_twrmult_ihcal", &m_twrmult_ihcal, "m_twrmult_ihcal/I");
  m_towertree->Branch("m_twr_ihcal_e", m_twr_ihcal_e, "m_twr_ihcal_e[m_twrmult_ihcal]/F");
  m_towertree->Branch("m_twr_ihcal_eta", m_twr_ihcal_eta, "m_twr_ihcal_eta[m_twrmult_ihcal]/F");
  m_towertree->Branch("m_twr_ihcal_phi", m_twr_ihcal_phi, "m_twr_ihcal_phi[m_twrmult_ihcal]/F");
  m_towertree->Branch("m_twr_ihcal_ieta", m_twr_ihcal_ieta, "m_twr_ihcal_ieta[m_twrmult_ihcal]/I");
  m_towertree->Branch("m_twr_ihcal_iphi", m_twr_ihcal_iphi, "m_twr_ihcal_iphi[m_twrmult_ihcal]/I");

  // Outer HCAL towers
  m_towertree->Branch("m_twrmult_ohcal", &m_twrmult_ohcal, "m_twrmult_ohcal/I");
  m_towertree->Branch("m_twr_ohcal_e", m_twr_ohcal_e, "m_twr_ohcal_e[m_twrmult_ohcal]/F");
  m_towertree->Branch("m_twr_ohcal_eta", m_twr_ohcal_eta, "m_twr_ohcal_eta[m_twrmult_ohcal]/F");
  m_towertree->Branch("m_twr_ohcal_phi", m_twr_ohcal_phi, "m_twr_ohcal_phi[m_twrmult_ohcal]/F");
  m_towertree->Branch("m_twr_ohcal_ieta", m_twr_ohcal_ieta, "m_twr_ohcal_ieta[m_twrmult_ohcal]/I");
  m_towertree->Branch("m_twr_ohcal_iphi", m_twr_ohcal_iphi, "m_twr_ohcal_iphi[m_twrmult_ohcal]/I");

  ////////////////////
  // Sim tower tree //
  ////////////////////
  m_simtowertree = new TTree("simtowertree", "Tree of raw simtowers");

  // CEMC simtowers
  m_simtowertree->Branch("m_simtwrmult_cemc", &m_simtwrmult_cemc, "m_simtwrmult_cemc/I");
  m_simtowertree->Branch("m_simtwr_cemc_e", m_simtwr_cemc_e, "m_simtwr_cemc_e[m_simtwrmult_cemc]/F");
  m_simtowertree->Branch("m_simtwr_cemc_eta", m_simtwr_cemc_eta, "m_simtwr_cemc_eta[m_simtwrmult_cemc]/F");
  m_simtowertree->Branch("m_simtwr_cemc_phi", m_simtwr_cemc_phi, "m_simtwr_cemc_phi[m_simtwrmult_cemc]/F");
  m_simtowertree->Branch("m_simtwr_cemc_ieta", m_simtwr_cemc_ieta, "m_simtwr_cemc_ieta[m_simtwrmult_cemc]/I");
  m_simtowertree->Branch("m_simtwr_cemc_iphi", m_simtwr_cemc_iphi, "m_simtwr_cemc_iphi[m_simtwrmult_cemc]/I");

  // Inner HCAL simtowers
  m_simtowertree->Branch("m_simtwrmult_ihcal", &m_simtwrmult_ihcal, "m_simtwrmult_ihcal/I");
  m_simtowertree->Branch("m_simtwr_ihcal_e", m_simtwr_ihcal_e, "m_simtwr_ihcal_e[m_simtwrmult_ihcal]/F");
  m_simtowertree->Branch("m_simtwr_ihcal_eta", m_simtwr_ihcal_eta, "m_simtwr_ihcal_eta[m_simtwrmult_ihcal]/F");
  m_simtowertree->Branch("m_simtwr_ihcal_phi", m_simtwr_ihcal_phi, "m_simtwr_ihcal_phi[m_simtwrmult_ihcal]/F");
  m_simtowertree->Branch("m_simtwr_ihcal_ieta", m_simtwr_ihcal_ieta, "m_simtwr_ihcal_ieta[m_simtwrmult_ihcal]/I");
  m_simtowertree->Branch("m_simtwr_ihcal_iphi", m_simtwr_ihcal_iphi, "m_simtwr_ihcal_iphi[m_simtwrmult_ihcal]/I");

  // Outer HCAL simtowers
  m_simtowertree->Branch("m_simtwrmult_ohcal", &m_simtwrmult_ohcal, "m_simtwrmult_ohcal/I");
  m_simtowertree->Branch("m_simtwr_ohcal_e", m_simtwr_ohcal_e, "m_simtwr_ohcal_e[m_simtwrmult_ohcal]/F");
  m_simtowertree->Branch("m_simtwr_ohcal_eta", m_simtwr_ohcal_eta, "m_simtwr_ohcal_eta[m_simtwrmult_ohcal]/F");
  m_simtowertree->Branch("m_simtwr_ohcal_phi", m_simtwr_ohcal_phi, "m_simtwr_ohcal_phi[m_simtwrmult_ohcal]/F");
  m_simtowertree->Branch("m_simtwr_ohcal_ieta", m_simtwr_ohcal_ieta, "m_simtwr_ohcal_ieta[m_simtwrmult_ohcal]/I");
  m_simtowertree->Branch("m_simtwr_ohcal_iphi", m_simtwr_ohcal_iphi, "m_simtwr_ohcal_iphi[m_simtwrmult_ohcal]/I");

  /////////////////////////
  // HepMC particle tree //
  /////////////////////////

  m_hepmctree = new TTree("hepmctree","Tree of truth hepmc info and particles");
  m_hepmctree->Branch("m_hepmc",&m_hepmc, "m_hepmc/I");
  m_hepmctree->Branch("m_hepmc_pid", m_hepmc_pid, "m_hepmc_pid[m_hepmc]/I");
  m_hepmctree->Branch("m_hepmc_e", m_hepmc_e, "m_hepmc_e[m_hepmc]/F");
  m_hepmctree->Branch("m_hepmc_pt", m_hepmc_pt, "m_hepmc_pt[m_hepmc]/F");
  m_hepmctree->Branch("m_hepmc_eta", m_hepmc_eta, "m_hepmc_eta[m_hepmc]/F");
  m_hepmctree->Branch("m_hepmc_phi", m_hepmc_phi, "m_hepmc_phi[m_hepmc]/F");

  //////////////////////
  // G4 particle tree //
  //////////////////////

  m_g4tree = new TTree("g4tree","Tree of truth G4 particles");
  m_g4tree->Branch("m_g4",&m_g4, "m_g4/I");
  m_g4tree->Branch("m_g4_pid", m_g4_pid, "m_g4_pid[m_g4]/I");
  m_g4tree->Branch("m_g4_e", m_g4_e, "m_g4_e[m_g4]/F");
  m_g4tree->Branch("m_g4_pt", m_g4_pt, "m_g4_pt[m_g4]/F");
  m_g4tree->Branch("m_g4_eta", m_g4_eta, "m_g4_eta[m_g4]/F");
  m_g4tree->Branch("m_g4_phi", m_g4_phi, "m_g4_phi[m_g4]/F");
  m_g4tree->Branch("m_g4_track_id", m_g4_track_id, "m_g4_track_id[m_g4]/I");
  m_g4tree->Branch("m_g4_parent_id", m_g4_parent_id, "m_g4_parent_id[m_g4]/I");

  ///////////////////////////
  // Additional truth tree //
  ///////////////////////////

  m_addtruthtree = new TTree("addtruthtree", "Tree of additional truth information");
  
  // child information
  m_addtruthtree->Branch("n_child",&n_child,"n_child/I");
  m_addtruthtree->Branch("child_pid",child_pid,"child_pid[n_child]/I");
  m_addtruthtree->Branch("child_parent_id",child_parent_id,"child_parent_id[n_child]/I");
  m_addtruthtree->Branch("child_vertex_id",child_vertex_id,"child_vertex_id[n_child]/I");
  m_addtruthtree->Branch("child_px",child_px,"child_px[n_child]/F");
  m_addtruthtree->Branch("child_py",child_py,"child_py[n_child]/F");
  m_addtruthtree->Branch("child_pz",child_pz,"child_pz[n_child]/F");
  m_addtruthtree->Branch("child_energy",child_energy,"child_energy[n_child]/F");

  // vertex information
  m_addtruthtree->Branch("n_vertex",&n_vertex,"n_vertex/I");
  m_addtruthtree->Branch("vertex_id",vertex_id,"vertex_id[n_vertex]/I");
  m_addtruthtree->Branch("vertex_x",vertex_x,"vertex_x[n_vertex]/F");
  m_addtruthtree->Branch("vertex_y",vertex_y,"vertex_y[n_vertex]/F");
  m_addtruthtree->Branch("vertex_z",vertex_z,"vertex_z[n_vertex]/F");

  // blackhole information
  m_addtruthtree->Branch("_nBH",&_nBH,"_nBH/I");
  m_addtruthtree->Branch("BH_track_id",_BH_track_id,"BH_track_id[_nBH]/I");
  m_addtruthtree->Branch("BH_px",_BH_px,"BH_px[_nBH]/F");
  m_addtruthtree->Branch("BH_py",_BH_py,"BH_py[_nBH]/F");
  m_addtruthtree->Branch("BH_pz",_BH_pz,"BH_pz[_nBH]/F");
  m_addtruthtree->Branch("BH_e",_BH_e,"BH_e[_nBH]/F");

  
}
