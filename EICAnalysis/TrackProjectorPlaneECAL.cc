#include "TrackProjectorPlaneECAL.h"

/* Fun4All includes */
#include <phool/PHCompositeNode.h>
#include <phool/PHIODataNode.h>

#include <fun4all/Fun4AllReturnCodes.h>

#include <phgeom/PHGeomUtility.h>

#include <g4hough/SvtxTrack.h>
#include <g4hough/SvtxTrackState_v1.h>
#include <g4hough/PHG4HoughTransform.h>
#include <g4hough/SvtxTrackMap_v1.h>

#include <calobase/RawCluster.h>

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
                                          "DafRep",
                                          "RKTrackRep", false);

  _fitter->set_verbosity(10);

  if (!_fitter) {
    cerr << "No fitter found at: " << endl;
    cerr << PHWHERE << endl;
  }
}

bool
TrackProjectorPlaneECAL::get_projected_position(  SvtxTrack * track, RawCluster* cluster, double arr_pos[3], const PROJECTION_SURFACE surf, const float surface_par )
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
TrackProjectorPlaneECAL::get_projected_momentum(  SvtxTrack * track, RawCluster *cluster, double arr_mom[3], const PROJECTION_SURFACE surf, const float surface_par )
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

SvtxTrack*
TrackProjectorPlaneECAL::get_best_track( SvtxTrackMap* trackmap, RawCluster* cluster, const float deltaR = -1 )
{
  std::vector< float > distance_from_track_to_cluster;
  bool there_is_a_track=false;

  // Go through the entire track map and find the track closest to deltaR
  // If deltaR < 0, get closest track

  for(SvtxTrackMap::ConstIter track_itr = trackmap->begin(); track_itr != trackmap->end(); track_itr++)
    {
      SvtxTrack* the_track = dynamic_cast<SvtxTrack*>(track_itr->second);
      
      /* Check if the_track is null ptr */
      if(the_track == NULL)
	{
	  distance_from_track_to_cluster.push_back(NAN);
	  continue;
	}
      
      /* Position vector of extrapolated track */
      double posv[3] = {0.,0.,0.};
      if(!get_projected_position( the_track,cluster, posv, TrackProjectorPlaneECAL::PLANE_CYLINDER, -1))
	{
	  std::cout << "Track Projection Position NOT FOUND; next iteration" << std::endl;
	  distance_from_track_to_cluster.push_back(NAN);
	  continue;
	}
      else
	{
	  distance_from_track_to_cluster.push_back(  sqrt( (cluster->get_x()-posv[0])*(cluster->get_x()-posv[0]) + 
							   (cluster->get_y()-posv[1])*(cluster->get_y()-posv[1]) + 
							   (cluster->get_z()-posv[2])*(cluster->get_z()-posv[2]) )  );
	  there_is_a_track = true;
	}
    }
  
  /* If no track was found, return NULL */
  if(distance_from_track_to_cluster.size()==0)
    return NULL;
  else if(there_is_a_track==false)
    return NULL;

  /* Find the track with minimum distance */
  /* TODO: What if two tracks are within deltaR */
  float min_distance = distance_from_track_to_cluster.at(0);
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
    return trackmap->get(min_distance_index);
  else if(min_distance<deltaR) 
    return trackmap->get(min_distance_index);
  else 
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
TrackProjectorPlaneECAL::project_track(  SvtxTrack * track, RawCluster* cluster, const PROJECTION_SURFACE surf, const float surface_par )
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
    TVector3 pos(trackstate->get_x(), trackstate->get_y(), trackstate->get_z());
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
	rep->extrapolateToPlane(*msop80, genfit::SharedPlanePtr( new genfit::DetPlane( cluster_pos, cluster_norm ) ), false, false);
      }
    else if ( surf == PLANEXY )
      rep->extrapolateToPlane(*msop80, genfit::SharedPlanePtr( new genfit::DetPlane( TVector3(0, 0, surface_par), TVector3(0, 0, 1) ) ), false, false);

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

