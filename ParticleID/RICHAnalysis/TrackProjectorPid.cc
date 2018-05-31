#include "TrackProjectorPid.h"

/* Fun4All includes */
#include <phool/PHCompositeNode.h>
#include <phool/PHIODataNode.h>

#include <fun4all/Fun4AllReturnCodes.h>

#include <phgeom/PHGeomUtility.h>

#include <g4hough/SvtxTrack.h>
#include <g4hough/SvtxTrackState_v1.h>
#include <g4hough/PHG4HoughTransform.h>

#include <phfield/PHFieldUtility.h>

#include <phgenfit/Fitter.h>
#include <phgenfit/PlanarMeasurement.h>
#include <phgenfit/Track.h>
#include <phgenfit/SpacepointMeasurement.h>

#include <GenFit/RKTrackRep.h>
#include <GenFit/FieldManager.h>

using namespace std;

TrackProjectorPid::TrackProjectorPid( PHCompositeNode *topNode ) :
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
TrackProjectorPid::get_projected_position(  SvtxTrack * track, double arr_pos[3], const PROJECTION_SURFACE surf, const float surface_par )
{
  /* set position components to 0 */
  arr_pos[0] = 0;
  arr_pos[1] = 0;
  arr_pos[2] = 0;

  /* project track */
  auto state = project_track( track, surf, surface_par );

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
TrackProjectorPid::get_projected_momentum(  SvtxTrack * track, double arr_mom[3], const PROJECTION_SURFACE surf, const float surface_par )
{
  /* set momentum components to 0 */
  arr_mom[0] = 0;
  arr_mom[1] = 0;
  arr_mom[2] = 0;

  /* project track */
  auto state = project_track( track, surf, surface_par );

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

SvtxTrackState*
TrackProjectorPid::project_track(  SvtxTrack * track, const PROJECTION_SURFACE surf, const float surface_par )
{
  /* Do projection */
  std::vector<double> point;
  point.assign(3, -9999.);

  auto last_state_iter = --track->end_states();

  SvtxTrackState * trackstate = last_state_iter->second;

  if(!trackstate) {
    cout << "No state found here!" << endl;
    return NULL;
  }

  int _pid_guess = -211;
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
      rep->extrapolateToCylinder(*msop80, surface_par, TVector3(0,0,0),  TVector3(0,0,1));


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

bool
TrackProjectorPid::is_in_RICH( double momv[3] )
{
  double eta = atanh( momv[2] );

  // rough pseudorapidity cut
  if (eta > 1.45 && eta < 7.5)
    return true;

  return false;
}
