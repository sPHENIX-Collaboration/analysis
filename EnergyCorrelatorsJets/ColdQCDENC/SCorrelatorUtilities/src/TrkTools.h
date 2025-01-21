/// ---------------------------------------------------------------------------
/*! \file   TrkTools.h
 *  \author Derek Anderson
 *  \date   10.30.2023
 *
 *  Collection of frequent track-related methods utilized
 *  in the sPHENIX Cold QCD Energy-Energy Correlator analysis.
 */
/// ---------------------------------------------------------------------------

#ifndef SCORRELATORUTILITIES_TRKTOOLS_H
#define SCORRELATORUTILITIES_TRKTOOLS_H

// c++ utilities
#include <array>
#include <limits>
#include <string>
#include <vector>
#include <cassert>
#include <utility>
// root utilities
#include <Math/Vector3D.h>
// phool libraries
#include <phool/phool.h>
#include <phool/getClass.h>
#include <phool/PHIODataNode.h>
#include <phool/PHNodeIterator.h>
#include <phool/PHCompositeNode.h>
// tracking libraries
#include <trackbase_historic/SvtxTrack.h>
#include <trackbase_historic/SvtxTrackMap.h>
#include <trackbase_historic/TrackAnalysisUtils.h>
// track evaluator utilities
#include <g4eval/SvtxTrackEval.h>
// vertex libraries
#include <globalvertex/GlobalVertex.h>
// phenix Geant4 utilities
#include <g4main/PHG4Particle.h>
// analysis utilities
#include "Constants.h"
#include "Interfaces.h"

// make common namespaces implicit
using namespace std;



namespace SColdQcdCorrelatorAnalysis {
  namespace Tools {

    // track methods ----------------------------------------------------------

    int                   GetNumLayer(SvtxTrack* track, const int16_t sys = 0);
    int                   GetNumClust(SvtxTrack* track, const int16_t sys = 0);
    int                   GetMatchID(SvtxTrack* track, SvtxTrackEval* trackEval);
    bool                  IsGoodTrackSeed(SvtxTrack* track, const bool requireSiSeeds = true);
    bool                  IsFromPrimaryVtx(SvtxTrack* track, PHCompositeNode* topNode);
    double                GetTrackDeltaPt(SvtxTrack* track);
    TrackSeed*            GetTrackSeed(SvtxTrack* track, const int16_t sys);
    pair<double, double>  GetTrackDcaPair(SvtxTrack* track, PHCompositeNode* topNode);
    ROOT::Math::XYZVector GetTrackVertex(SvtxTrack* track, PHCompositeNode* topNode);

  }  // end Tools namespace
}  // end SColdQcdCorrealtorAnalysis namespace

#endif

// end ------------------------------------------------------------------------
