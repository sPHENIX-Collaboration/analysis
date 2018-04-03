#ifndef __TRACKPROJECTORRICH_H_
#define __TRACKPROJECTORRICH_H_

/* STL includes */
#include <string>

/* Forward declarations */
class PHCompositeNode;
class PHFieldUtility;
class SvtxTrack;

namespace PHGenFit{
  class Fitter;
}

/**
 * Class to project tracks to RICH volumes.
 * Based on clas coresoftware/simulation/g4simulation/g4hough/PHG4GenFitTrackProjection
 */
class TrackProjectorRICH {

public:

  TrackProjectorRICH( PHCompositeNode* topNode );

  bool get_projected_position( SvtxTrack * track, double arr_pos[3] ); // Get mean track position

  bool get_projected_momentum( SvtxTrack * track, double arr_mom[3] ); // Get momentum of track

  bool project_track( SvtxTrack * track, double arr_pos[3], double arr_mom[3] );

private:

  PHGenFit::Fitter * _fitter;
};

#endif
