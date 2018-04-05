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
TrackProjectorPid::get_projected_position(  SvtxTrack * track, double arr_pos[3] )
{
  /* set position components to 0 */
  arr_pos[0] = 0;
  arr_pos[1] = 0;
  arr_pos[2] = 0;

  /* project track */
  auto state = project_track( track );

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
TrackProjectorPid::get_projected_momentum(  SvtxTrack * track, double arr_mom[3] )
{
  /* set momentum components to 0 */
  arr_mom[0] = 0;
  arr_mom[1] = 0;
  arr_mom[2] = 0;

  /* project track */
  auto state = project_track( track );

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

//unique_ptr<genfit::MeasuredStateOnPlane>
//genfit::MeasuredStateOnPlane*
SvtxTrackState*
TrackProjectorPid::project_track(  SvtxTrack * track )
{
  /* @TODO: Hard coded extrapolation radius- make it dependent on geometry input for example. */
  float radius = 220;

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
    rep->extrapolateToSphere(*msop80, radius, TVector3(0,0,0), false, false);
  } catch (...) {
    cout << "track extrapolateToXX failed" << endl;
    return NULL;
  }

  /* Having trouble returning unique_ptr msop80 object and using it in other member functions.
   * Workaround: Create SvtxTrackState object to pass projected track state information to
   * other member functions. */
  SvtxTrackState_v1 *svtx_state = new SvtxTrackState_v1();
  svtx_state->set_x( msop80->getPos().X() );
  svtx_state->set_y( msop80->getPos().Y() );
  svtx_state->set_z( msop80->getPos().Z() );
  svtx_state->set_px( msop80->getMom().x() );
  svtx_state->set_py( msop80->getMom().y() );
  svtx_state->set_pz( msop80->getMom().z() );

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
