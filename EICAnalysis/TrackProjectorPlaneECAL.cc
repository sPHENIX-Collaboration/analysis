
#include "TrackProjectorPlaneECAL.h"

/* Fun4All includes */
#include <trackbase_historic/SvtxTrack.h>
#include <trackbase_historic/SvtxTrackMap.h>
#include <trackbase_historic/SvtxTrack_FastSim.h> 

#include <phool/PHCompositeNode.h>
#include <phool/PHIODataNode.h>

#include <fun4all/Fun4AllReturnCodes.h>

#include <phgeom/PHGeomUtility.h>

#include <trackbase_historic/SvtxTrack.h>
#include <trackbase_historic/SvtxTrackState_v1.h>
//#include <g4hough/PHG4HoughTransform.h>
#include <trackbase_historic/SvtxTrackMap_v1.h>

#include <calobase/RawCluster.h>
#include <calobase/RawTowerDefs.h>

#include <phfield/PHFieldUtility.h>

#include <phgenfit/Fitter.h>
#include <phgenfit/PlanarMeasurement.h>
#include <phgenfit/Track.h>
#include <phgenfit/SpacepointMeasurement.h>

#include <GenFit/RKTrackRep.h>
#include <GenFit/FieldManager.h>



using namespace std;

TrackProjectorPlaneECAL::TrackProjectorPlaneECAL( PHCompositeNode *topNode ) :
  _fitter(nullptr)
{

  TGeoManager* tgeo_manager = PHGeomUtility::GetTGeoManager(topNode);
  PHField * field = PHFieldUtility::GetFieldMapNode(nullptr, topNode);

  _fitter = PHGenFit::Fitter::getInstance(tgeo_manager,field,
                                          "DafRef",
                                          "RKTrackRep", false);

  _fitter->set_verbosity(10);

  if (!_fitter) {
    cerr << "No fitter found at: " << endl;
    cerr << PHWHERE << endl;
  }
}

bool
TrackProjectorPlaneECAL::get_projected_position(  SvtxTrack_FastSim * track, RawCluster* cluster, double arr_pos[3], const PROJECTION_SURFACE surf, const float surface_par )
{
  /* set position components to 0 */
  arr_pos[0] = 0;
  arr_pos[1] = 0;
  arr_pos[2] = 0;

  /* project track */
  auto state = project_track( track, cluster, surf, surface_par );

  /* Set position at extrapolate position */
  if ( state )
    {
      arr_pos[0] = state->get_x();
      arr_pos[1] = state->get_y();
      arr_pos[2] = state->get_z();
      return true;
    }

  return false;
}

bool
TrackProjectorPlaneECAL::get_projected_momentum(  SvtxTrack_FastSim * track, RawCluster *cluster, double arr_mom[3], const PROJECTION_SURFACE surf, const float surface_par )
{
  /* set momentum components to 0 */
  arr_mom[0] = 0;
  arr_mom[1] = 0;
  arr_mom[2] = 0;

  /* project track */
  auto state = project_track( track, cluster, surf, surface_par );

  /* Set momentum at extrapolate position */
  if ( state )
    {
      arr_mom[0] = state->get_px();
      arr_mom[1] = state->get_py();
      arr_mom[2] = state->get_pz();
      return true;
    }

  return false;
}
// Iterate through all tracks in the SvtxTrackMap. For every track, we iterate among all of its TrackStates. Each
// track state corresponds to a point in space where the track object has been extrapolated to. Currently, we 
// extrapolate via coresoftware/simulation/g4simulation/g4trackfastsim/PHG4TrackFastSim.cc. There are four
// detectors in which we perform extrapolation to, the CEMC, FEMC, EEMC, and FHCAL. For every track, we find
// the best extrapolation to the inputted 'RawCluster' object. We then compare the best states and return the
// best extrapolated track. 
SvtxTrack_FastSim*
TrackProjectorPlaneECAL::get_best_track( SvtxTrackMap* trackmap, RawCluster* cluster)
{
  std::vector< float > distance_from_track_to_cluster;
  bool there_is_a_track=false;
  
  float deltaR = -1;
 
  // Iterate all tracks in the trackmap
  for(SvtxTrackMap::ConstIter track_itr = trackmap->begin(); track_itr != trackmap->end(); track_itr++)
    {
      SvtxTrack_FastSim* the_track = dynamic_cast<SvtxTrack_FastSim*>(track_itr->second);
      /* Check if the_track is null ptr */
      if(the_track == NULL)
	 {
	   distance_from_track_to_cluster.push_back(NAN);
	   continue;
	 }
      else
	{
	  there_is_a_track = true;
	}
      float distance_from_state_to_cluster = 9990;
      // Iterate all track states in the track object
      for(SvtxTrack::ConstStateIter state_itr = the_track->begin_states(); state_itr != the_track->end_states(); state_itr++)
	{
	  float distance_from_state_to_cluster_temp = 9999;
	 
	  SvtxTrackState* the_state = dynamic_cast<SvtxTrackState*>(state_itr->second);
	  /* Check if the_state is a NULL pointer */
	  if(the_state==NULL)
	    {
	      cout << "State is NULL, skipping..." << endl;
	      continue;
	    }
	  //cout<<the_state->get_x()<< " : " <<the_state->get_y() << " : " <<the_state->get_z() << endl;
	  distance_from_state_to_cluster_temp = ( sqrt( (cluster->get_x()-the_state->get_x())*(cluster->get_x()-the_state->get_x()) + (cluster->get_y()-the_state->get_y())*(cluster->get_y()-the_state->get_y()) + (cluster->get_z()-the_state->get_z())*(cluster->get_z()-the_state->get_z())));
	  
	  /*cout << "Cluster : " << cluster->get_x() << " " << cluster->get_y() << " " << cluster->get_z() << endl;
	  cout << "State : " << the_state->get_x() << " " << the_state->get_y() << " " << the_state->get_z() << endl;
	  cout << "Name : " << the_state->get_name() << endl;
	  cout << " " << endl;*/
	  if(distance_from_state_to_cluster_temp < distance_from_state_to_cluster)
	    {
	      distance_from_state_to_cluster = distance_from_state_to_cluster_temp;	      
	    }
	}
      if(distance_from_state_to_cluster!=9990)
	{
	  distance_from_track_to_cluster.push_back(distance_from_state_to_cluster);
	  
	}
     
      /* Position vector of extrapolated track */
      //double posv[3] = {0.,0.,0.};
      //if(!get_projected_position( the_track,cluster, posv, TrackProjectorPlaneECAL::PLANE_CYLINDER, -1))
      //	{
	  //std::cout << "Track Projection Position NOT FOUND; next iteration" << std::endl;
      //	  distance_from_track_to_cluster.push_back(NAN);
      //	  continue;
      //	}
      // else
	//{
      // distance_from_track_to_cluster.push_back(  sqrt( (cluster->get_x()-posv[0])*(cluster->get_x()-posv[0]) + 
      //						   (cluster->get_y()-posv[1])*(cluster->get_y()-posv[1]) + 
      //						   (cluster->get_z()-posv[2])*(cluster->get_z()-posv[2]) )  );
      //  
      //}
    }
  
  /* If no track was found, return NULL */
  if(distance_from_track_to_cluster.size()==0)
    return NULL;
  else if(there_is_a_track==false)
    return NULL;
  /* Find the track with minimum distance */
  /* TODO: What if two tracks are within deltaR */
  float min_distance = 9999;
  int min_distance_index = 0;
  for(unsigned i = 0; i<distance_from_track_to_cluster.size(); i++)
    {
      if(distance_from_track_to_cluster.at(i)<min_distance)
	{
	  min_distance = distance_from_track_to_cluster.at(i);
	  min_distance_index = i;
	}
    }
  /* Test min_distance with deltaR */
  if(deltaR<0) 
    return dynamic_cast<SvtxTrack_FastSim*>(trackmap->get(min_distance_index));
  else if(min_distance<deltaR) 
    return dynamic_cast<SvtxTrack_FastSim*>(trackmap->get(min_distance_index));
  else 
    return NULL;
}


SvtxTrackState*
TrackProjectorPlaneECAL::get_best_state( SvtxTrack_FastSim* the_track, RawCluster* cluster)
{
  /* Check if the_track is null ptr */
  float distance_from_state_to_cluster = 9990;
  int best_state_index = -1;
  int count = 0; 
  // Iterate all track states in the track object
  for(SvtxTrack::ConstStateIter state_itr = the_track->begin_states(); state_itr != the_track->end_states(); state_itr++)
    {
      float distance_from_state_to_cluster_temp = 9999;
      
      SvtxTrackState* the_state = dynamic_cast<SvtxTrackState*>(state_itr->second);
      /* Check if the_state is a NULL pointer */
      if(the_state==NULL)
	{
	  cout << "State is NULL, skipping..." << endl;
	  continue;
	}
      //cout<<the_state->get_x()<< " : " <<the_state->get_y() << " : " <<the_state->get_z() << endl;
      distance_from_state_to_cluster_temp = ( sqrt( (cluster->get_x()-the_state->get_x())*(cluster->get_x()-the_state->get_x()) + (cluster->get_y()-the_state->get_y())*(cluster->get_y()-the_state->get_y()) + (cluster->get_z()-the_state->get_z())*(cluster->get_z()-the_state->get_z())));
      if(distance_from_state_to_cluster_temp < distance_from_state_to_cluster)
	{
	  best_state_index = count;
	  distance_from_state_to_cluster = distance_from_state_to_cluster_temp;
	}
      count++;
    }
  if(distance_from_state_to_cluster==-1||best_state_index==-1)
    {
      cout << "State Map issue, returning NULL" << endl;
    }
  count = 0;
  for(SvtxTrack::ConstStateIter state_itr = the_track->begin_states(); state_itr != the_track->end_states(); state_itr++)
    {
      if(count!=best_state_index)
	{
	  count++;
	  continue;
	}
      else
	{
	  SvtxTrackState* the_state = dynamic_cast<SvtxTrackState*>(state_itr->second);
	  return the_state;
	}
    }

  return NULL;
}






char
TrackProjectorPlaneECAL::get_detector()
{
  switch(detector){
  case CEMC:
    return 'C';
    break;
  case EEMC:
    return 'E';
    break;
  case FEMC:
    return 'F';
    break;
  default:
    cout << "WARNING: get_detector was unable to find a defined detector" << endl;
    return ' ';
  }
}

SvtxTrackState*
TrackProjectorPlaneECAL::project_track(  SvtxTrack_FastSim * track, RawCluster* cluster, const PROJECTION_SURFACE surf, const float surface_par )
{  
  auto last_state_iter = --track->end_states();
  SvtxTrackState * trackstate = last_state_iter->second;
 
  if(!trackstate) {
    cout << "No state found here!" << endl;
    return NULL;
  }
  
  int _pid_guess = 11;
  auto rep = unique_ptr<genfit::AbsTrackRep> (new genfit::RKTrackRep(_pid_guess));

  unique_ptr<genfit::MeasuredStateOnPlane> msop80 = nullptr;
  
  {
    cout << track->size_states() << endl;
    TVector3 pos(trackstate->get_x(), trackstate->get_y(), trackstate->get_z());
    cout << pos(0) << " : " << pos(1) << " : " << pos(2) << endl;
    //pos(0) = 0.0;
    //pos(1) = 0.0;
    //pos(2) = 0.0;
    TVector3 mom(trackstate->get_px(), trackstate->get_py(), trackstate->get_pz());
    TMatrixDSym cov(6);
    for (int i = 0; i < 6; ++i) {
      for (int j = 0; j < 6; ++j) {
        cov[i][j] = trackstate->get_error(i, j);
      }
    } 
    msop80 = unique_ptr<genfit::MeasuredStateOnPlane> (new genfit::MeasuredStateOnPlane(rep.get()));  
    msop80->setPosMomCov(pos, mom, cov);
  }

  /* This is where the actual extrapolation of the track to a surface (cylinder, plane, cone, sphere) happens. */
  try {

    /* 'rep 'is of type AbsTrackRep (see documentation or header for extrapolation function options ) */
    if ( surf == SPHERE )
      rep->extrapolateToSphere(*msop80, surface_par, TVector3(0,0,0), false, false);

    else if ( surf == CYLINDER )
      {
	rep->extrapolateToCylinder(*msop80, surface_par, TVector3(0,0,0),  TVector3(0,0,1));
      }
    else if ( surf == PLANE_CYLINDER )
      {
	// Get position of cluster
        TVector3 cluster_pos(cluster->get_x(),cluster->get_y(),cluster->get_z());
	// Get detector normal vector
	double xvect=0; double yvect = 0; double zvect = 0;
	switch(detector){
	case CEMC:
	  xvect = cos(cluster->get_phi());
	  yvect = sin(cluster->get_phi());
	  break;
	case EEMC:
	  zvect = 1;
	  break;
	case FEMC:
	  cluster_pos(0)=0;
	  cluster_pos(1)=0;
	  cluster_pos(2)=295;
	  zvect = -1;
	  break;
	default:
	  cout << "WARNING: detector variable native to TrackProjectorPlaneECAL not defined" << endl;
	  break;
	}
	// Get normal vector to detector
	TVector3 cluster_norm(xvect,yvect,zvect);
	// Case in which norm vector remained undefined 
	if(xvect==0&&yvect==0&&zvect==0)
	  {
	    cout << "WARNING: Cluster normal vector uninitialized. Aborting track" << endl;
	    return NULL;
	  }


	// Extrapolate to detector 
	rep->extrapolateToCylinder(*msop80, 95, TVector3(0,0,0),  TVector3(0,0,1));
	//rep->extrapolateToPlane(*msop80, genfit::SharedPlanePtr( new genfit::DetPlane( cluster_pos, cluster_norm ) ), false, false);
	
      }
    else if ( surf == PLANEXY )
      {
	rep->extrapolateToPlane(*msop80, genfit::SharedPlanePtr( new genfit::DetPlane( TVector3(0, 0, surface_par), TVector3(0, 0, 1) ) ), false, false);
	
      }
  } catch (...) {
    cout << "track extrapolateToXX failed" << endl;
    return NULL;
  }

  /* Create SvtxTrackState object as storage version of the track state
   * to pass projected track state information to other member functions. */
  SvtxTrackState_v1 *svtx_state = new SvtxTrackState_v1();

  svtx_state->set_x( msop80->getPos().X() );
  svtx_state->set_y( msop80->getPos().Y() );
  svtx_state->set_z( msop80->getPos().Z() );

  svtx_state->set_px( msop80->getMom().x() );
  svtx_state->set_py( msop80->getMom().y() );
  svtx_state->set_pz( msop80->getMom().z() );

  for (int i = 0; i < 6; i++) {
    for (int j = i; j < 6; j++) {
      svtx_state->set_error(i, j, msop80->get6DCov()[i][j]);
    }
  }

  return svtx_state;
}

void
TrackProjectorPlaneECAL::set_detector( char c )
{
  switch(c){
  case 'C':
  case 'c':
    detector = CEMC;
    break;
  case 'E':
  case 'e':
    detector = EEMC;
    break;
  case 'F':
  case 'f':
    detector = FEMC;
    break;
  default:
    cout << "WARNING: set_detector call received unrecognized char " << c << endl;
    break;
  }
}

char TrackProjectorPlaneECAL::get_detector_from_cluster(RawCluster* cluster)
{
  RawCluster::TowerConstIterator rtiter = cluster->get_towers().first;
  //caloid = RawTowerDefs::decode_caloid( rtiter->first );
  const char *caloid_to_name = RawTowerDefs::convert_caloid_to_name( RawTowerDefs::decode_caloid( rtiter->first ) ).c_str();
  if(strcmp(caloid_to_name,"EEMC")==0) return 'E';
  else if(strcmp(caloid_to_name,"CEMC")==0) return 'C';
  else if(strcmp(caloid_to_name,"FEMC")==0) return 'F';
  else 
    {
      cout << "Unrecognized calorimeter. Default to CEMC" << endl;
      return 'C';
    }
}
