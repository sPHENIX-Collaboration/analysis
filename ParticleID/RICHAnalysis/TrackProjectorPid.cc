#include "TrackProjectorPid.h"

/* Fun4All includes */
#include <phool/PHCompositeNode.h>
#include <phool/PHIODataNode.h>

#include <fun4all/Fun4AllReturnCodes.h>

#include <phgeom/PHGeomUtility.h>

#include <g4hough/SvtxTrack.h>
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
  genfit::MeasuredStateOnPlane* state = project_track( track );

  /* Set position at extrapolate position */
  if ( state )
    {
      arr_pos[0] = state->getPos().X();
      arr_pos[1] = state->getPos().Y();
      arr_pos[2] = state->getPos().Z();
      delete state;
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
  genfit::MeasuredStateOnPlane* state = project_track( track );

  /* Set momentum at extrapolate position */
  if ( state )
    {
      arr_mom[0] = state->getMom().x();
      arr_mom[1] = state->getMom().y();
      arr_mom[2] = state->getMom().z();
      delete state;
      return true;
    }

  return false;
}

genfit::MeasuredStateOnPlane*
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

  msop80.release();
  return &*msop80;
}
