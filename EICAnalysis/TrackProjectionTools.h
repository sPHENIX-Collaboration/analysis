#ifndef TRACKPROJECTIONTOOLS_H__2
#define TRACKPROJECTIONTOOLS_H__

/* Fun4All includes */
#include <fun4all/SubsysReco.h>
#include <calobase/RawTowerDefs.h>

/* STL includes */
#include <math.h>

class PHCompositeNode;

class CaloEvalStack;
class CaloRawTowerEval;

class RawCluster;
class RawTowerContainer;
class RawTowerGeomContainer;

class SvtxTrack;
class SvtxTrackMap;

class TrackProjectionTools
{

public:
  TrackProjectionTools( PHCompositeNode* );

  virtual ~TrackProjectionTools() {}

  /** Find track with minimum delta R from given cluster */
  SvtxTrack* FindClosestTrack( RawCluster*, float& );

  /** Find closest cluster to track */
  RawCluster* getCluster( SvtxTrack*, std::string );

  /** get energy in 3x3 calorimeter towers around track projection to calorimeter surface.
   * Copied from FastTrackingEval.C */
  float getE33Barrel( std::string, float, float );

  /** get energy in 3x3 calorimeter towers around track projection to calorimeter surface.
   * Copied from FastTrackingEval.C */
  float getE33Forward( std::string, float, float );

protected:

  /** helper pointer to topNode */
  PHCompositeNode *_topNode;

};

#endif
