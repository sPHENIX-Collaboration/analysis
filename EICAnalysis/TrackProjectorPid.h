#ifndef __TRACKPROJECTORPID_H_
#define __TRACKPROJECTORPID_H_

/* STL includes */
#include <string>
#include <memory>

/* Forward declarations */
class PHCompositeNode;
class PHFieldUtility;
class SvtxTrack;
class SvtxTrackState;

namespace PHGenFit{
  class Fitter;
}

namespace genfit{
  class MeasuredStateOnPlane;
}

/**
 * Class to project tracks to RICH volumes.
 * Based on clas coresoftware/simulation/g4simulation/g4hough/PHG4GenFitTrackProjection
 */
class TrackProjectorPid {

public:

  enum PROJECTION_SURFACE {SPHERE, CYLINDER, PLANEXY};

  TrackProjectorPid( PHCompositeNode* topNode );

  bool get_projected_position( SvtxTrack * track, double arr_pos[3], const PROJECTION_SURFACE surf, const float surface_par ); // Get mean track position

  bool get_projected_momentum( SvtxTrack * track, double arr_mom[3], const PROJECTION_SURFACE surf, const float surface_par ); // Get momentum of track

  SvtxTrackState* project_track( SvtxTrack * track, const PROJECTION_SURFACE surf, const float surface_par );

  bool is_in_RICH( double momv[3] ); // Check if track pass through RICH

private:

  PHGenFit::Fitter * _fitter;
};

#endif
