#include "TrackProjectionTools.h"

/* STL includes */
#include <cassert>

/* Fun4All includes */
#include <trackbase_historic/SvtxTrackMap_v1.h>

#include <phool/getClass.h>

#include <calobase/RawTowerGeomContainer.h>
#include <calobase/RawTowerContainer.h>
#include <calobase/RawTowerGeom.h>
#include <calobase/RawTowerv1.h>

#include <calobase/RawClusterContainer.h>
#include <calobase/RawCluster.h>

#include <g4vertex/GlobalVertexMap.h>
#include <g4vertex/GlobalVertex.h>

#include <g4eval/CaloEvalStack.h>
#include <g4eval/CaloRawTowerEval.h>

using namespace std;

TrackProjectionTools::TrackProjectionTools( PHCompositeNode *topNode )
{
  _topNode = topNode;
}


SvtxTrack* TrackProjectionTools::FindClosestTrack( RawCluster* cluster, float& best_track_dr )
{

  /* best matching track */
  SvtxTrack* best_track = NULL;
  best_track_dr = NAN;

  /* find name of calorimeter for this cluster */
  string caloname = "NONE";

  /* C++11 range loop */
  for (auto& towit : cluster->get_towermap() )
    {
      caloname = RawTowerDefs::convert_caloid_to_name( RawTowerDefs::decode_caloid(towit.first) );
      break;
    }

  /* Get track collection with all tracks in this event */
  SvtxTrackMap* trackmap =
    findNode::getClass<SvtxTrackMap>(_topNode,"SvtxTrackMap_FastSim");
  if (!trackmap)
    {
      cout << PHWHERE << "SvtxTrackMap node not found on node tree"
           << endl;
    }

  /* Loop over all tracks from BARREL tracking and see if one points to the same
   * cluster as the reference clusters (i.e. matching ID in the same calorimeter) */
  /*
  for (SvtxTrackMap::ConstIter track_itr = trackmap->begin();
       track_itr != trackmap->end(); track_itr++)
    {
      SvtxTrack* track =  dynamic_cast<SvtxTrack*>(track_itr->second);
  
      if ( caloname == "CEMC" &&
           track->get_cal_cluster_id(SvtxTrack::CEMC) == cluster->get_id() )
        {
          best_track = track;
        }
    }
  */

  /* If track found with barrel tracking, return it here- if not, proceed with forward tracking below. */
  if ( best_track )
    return best_track;
  
  /* Cluster / track matching for barrel calorimeters and tracking */
  float max_dr = 10;

  /* cluster position for easy reference */
  float cx = cluster->get_x();
  float cy = cluster->get_y();

  /* If track map found: Loop over all tracks to find best match for cluster (forward calorimeters)*/
  if ( trackmap &&
       ( caloname == "FEMC" || caloname == "EEMC" ) )
    {
      for (SvtxTrackMap::ConstIter track_itr = trackmap->begin();
           track_itr != trackmap->end(); track_itr++)
        {
          /* get pointer to track */
          SvtxTrack* track =  dynamic_cast<SvtxTrack*>(track_itr->second);

          /* distance between track and cluster */
          float dr = NAN;

          /* loop over track states (projections) sotred for this track */
          for (SvtxTrack::ConstStateIter state_itr = track->begin_states();
               state_itr != track->end_states(); state_itr++)
            {
              /* get pointer to current track state */
              SvtxTrackState *temp = dynamic_cast<SvtxTrackState*>(state_itr->second);

              /* check if track state projection name matches calorimeter where cluster was found */
              if( (temp->get_name()==caloname) )
                {
                  dr = sqrt( pow( cx - temp->get_x(), 2 ) + pow( cy - temp->get_y(), 2 ) );
		  break;
                }
            }
	  
          /* check dr and update best_track and best_track_dr if this track is closest to cluster */
          if ( ( best_track_dr != best_track_dr ) ||
	       ( dr < max_dr &&
		 dr < best_track_dr )
	       )
            {
              best_track = track;
              best_track_dr = dr;
            }
        }
    }


  /* If track found with barrel tracking, return it here- if not, proceed with alternative barrel cluster-track matching below. */
  if ( best_track )
    return best_track;
  

  /* Cluster / track matching for barrel calorimeters and tracking */
  float max_dr_barrel = 10;

  float ctheta = atan2( cluster->get_r() , cluster->get_z() );
  float ceta =  -log( tan( ctheta / 2.0 ) );
  float cphi = cluster->get_phi();

  /* If track map found: Loop over all tracks to find best match for cluster (barrel calorimeters)*/
  if ( trackmap &&
       ( caloname == "CEMC" ) )
    {
      for (SvtxTrackMap::ConstIter track_itr = trackmap->begin();
           track_itr != trackmap->end(); track_itr++)
        {
          /* get pointer to track */
          SvtxTrack* track =  dynamic_cast<SvtxTrack*>(track_itr->second);

          /* distance between track and cluster */
          float dr = NAN;

          /* loop over track states (projections) sotred for this track */
          for (SvtxTrack::ConstStateIter state_itr = track->begin_states();
               state_itr != track->end_states(); state_itr++)
            {
              /* get pointer to current track state */
              SvtxTrackState *temp = dynamic_cast<SvtxTrackState*>(state_itr->second);

              /* check if track state projection name matches calorimeter where cluster was found */
              if( (temp->get_name()==caloname) )
                {
                  dr = sqrt( pow( ceta - temp->get_eta(), 2 ) + pow( cphi - temp->get_phi(), 2 ) );
                  break;
                }
            }

          /* check dr and update best_track and best_track_dr if this track is closest to cluster */
	  if ( ( best_track_dr != best_track_dr ) || 
	       (dr < max_dr_barrel &&
               dr < best_track_dr) )
            {
              best_track = track;
              best_track_dr = dr;
            }
        }
    }

  return best_track;
}

RawCluster*
TrackProjectionTools::getCluster( SvtxTrack* track, string detName )
{
  /* track direction */
  float eta = 0;
  float phi = 0;

  /* closest cluster */
  RawCluster* cluster_min_dr = NULL;

  // pull the clusters
  string clusternodename = "CLUSTER_" + detName;
  RawClusterContainer *clusterList = findNode::getClass<RawClusterContainer>(_topNode,clusternodename.c_str());
  if (!clusterList) {
    cerr << PHWHERE << " ERROR: Can't find node " << clusternodename << endl;
    return NULL;
  }

  // loop over all clusters and find nearest
  double min_r = NAN;

  for (unsigned int k = 0; k < clusterList->size(); ++k) {

    RawCluster *cluster = clusterList->getCluster(k);

    double dphi = atan2(sin(phi-cluster->get_phi()),cos(phi-cluster->get_phi()));

    double cluster_theta = atan2( cluster->get_r() , cluster->get_z() );
    double cluster_eta =  -log(tan(cluster_theta/2.0));

    double deta = eta-cluster_eta;
    double r = sqrt(pow(dphi,2)+pow(deta,2));

    if (r < min_r) {
      min_r = r;
      cluster_min_dr = cluster;
    }
  }

//  if(detName == "FEMC") {
//    if(!secondFlag){
//      femc_cldr = min_r;
//      femc_clE = min_e;
//      femc_clN = min_n;
//    }
//    else{
//      femc_cldr2 = min_r;
//      femc_clE2 = min_e;
//      femc_clN2 = min_n;
//    }
//  }
//  else{
//    if(!secondFlag){
//      fhcal_cldr = min_r;
//      fhcal_clE = min_e;
//      fhcal_clN = min_n;
//    }
//    else{
//      fhcal_cldr2 = min_r;
//      fhcal_clE2 = min_e;
//      fhcal_clN2 = min_n;
//    }
//  }
//
  return cluster_min_dr;
}



float
TrackProjectionTools::getE33Barrel( string detName, float phi, float eta ){

  string towernodename = "TOWER_CALIB_" + detName;
  // Grab the towers
  RawTowerContainer* towerList = findNode::getClass<RawTowerContainer>(_topNode, towernodename.c_str());
  if (!towerList)
    {
      std::cout << PHWHERE << ": Could not find node " << towernodename.c_str() << std::endl;
      return -1;
    }
  string towergeomnodename = "TOWERGEOM_" + detName;
  RawTowerGeomContainer *towergeo = findNode::getClass<RawTowerGeomContainer>(_topNode, towergeomnodename.c_str());
  if (! towergeo)
    {
      cout << PHWHERE << ": Could not find node " << towergeomnodename.c_str() << endl;
      return -1;
    }

  // calculate 3x3 and 5x5 tower energies
  int binphi = towergeo->get_phibin(phi);
  int bineta = towergeo->get_etabin(eta);

  float energy_3x3 = 0.0;
  float energy_5x5 = 0.0;

  for (int iphi = binphi-2; iphi <= binphi+2; ++iphi) {
    for (int ieta = bineta-2; ieta <= bineta+2; ++ieta) {

      // wrap around
      int wrapphi = iphi;
      if (wrapphi < 0) {
        wrapphi = towergeo->get_phibins() + wrapphi;
      }
      if (wrapphi >= towergeo->get_phibins()) {
        wrapphi = wrapphi - towergeo->get_phibins();
      }

      // edges
      if (ieta < 0) continue;
      if (ieta >= towergeo->get_etabins()) continue;

      RawTower* tower = towerList->getTower(ieta,wrapphi);
      if (tower) {
        energy_5x5 += tower->get_energy();
        if (abs(iphi - binphi)<=1 and abs(ieta - bineta)<=1 )
          energy_3x3 += tower->get_energy();
      }

    }
  }

  return energy_3x3;
}


float
TrackProjectionTools::getE33Forward( string detName, float tkx, float tky )
{
  float twr_sum = 0;

  string towernodename = "TOWER_CALIB_" + detName;
  // Grab the towers
  RawTowerContainer* towers = findNode::getClass<RawTowerContainer>(_topNode, towernodename.c_str());
  if (!towers)
    {
      std::cout << PHWHERE << ": Could not find node " << towernodename.c_str() << std::endl;
      return -1;
    }
  string towergeomnodename = "TOWERGEOM_" + detName;
  RawTowerGeomContainer *towergeom = findNode::getClass<RawTowerGeomContainer>(_topNode, towergeomnodename.c_str());
  if (! towergeom)
    {
      cout << PHWHERE << ": Could not find node " << towergeomnodename.c_str() << endl;
      return -1;
    }

  // Locate the central tower
  float r_dist = 9999.0;
  int twr_j = -1;
  int twr_k = -1;
  RawTowerDefs::CalorimeterId calo_id_ = RawTowerDefs::convert_name_to_caloid( detName );

  RawTowerContainer::ConstRange begin_end  = towers->getTowers();
  RawTowerContainer::ConstIterator itr = begin_end.first;
  for (; itr != begin_end.second; ++itr)
    {
      RawTowerDefs::keytype towerid = itr->first;
      RawTowerGeom *tgeo = towergeom->get_tower_geometry(towerid);

      float x = tgeo->get_center_x();
      float y = tgeo->get_center_y();

      float temp_rdist = sqrt(pow(tkx-x,2) + pow(tky-y,2)) ;
      if(temp_rdist< r_dist){
        r_dist = temp_rdist;
        twr_j = RawTowerDefs::decode_index1(towerid);
        twr_k = RawTowerDefs::decode_index2(towerid);
      }

      if( (fabs(tkx-x)<(tgeo->get_size_x()/2.0)) &&
          (fabs(tky-y)<(tgeo->get_size_y()/2.0)) ) break;

    }

  // Use the central tower to sum up the 3x3 energy
  if(twr_j>=0 && twr_k>=0){
    for(int ij = -1; ij <=1; ij++){
      for(int ik = -1; ik <=1; ik++){
        RawTowerDefs::keytype temp_towerid = RawTowerDefs::encode_towerid( calo_id_ , twr_j + ij , twr_k + ik );
        RawTower *rawtower = towers->getTower(temp_towerid);
        if(rawtower) twr_sum += rawtower->get_energy();
      }
    }
  }

  return twr_sum;
}
