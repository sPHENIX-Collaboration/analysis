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

  TrackProjectorPid( PHCompositeNode* topNode );

  bool get_projected_position( SvtxTrack * track, double arr_pos[3] ); // Get mean track position

  bool get_projected_momentum( SvtxTrack * track, double arr_mom[3] ); // Get momentum of track

  //std::unique_ptr<genfit::MeasuredStateOnPlane> project_track( SvtxTrack * track );
  //genfit::MeasuredStateOnPlane*
  SvtxTrackState* project_track( SvtxTrack * track );

  bool is_in_RICH( double momv[3] ); // Check if track pass through RICH

private:

  PHGenFit::Fitter * _fitter;
};

#endif
