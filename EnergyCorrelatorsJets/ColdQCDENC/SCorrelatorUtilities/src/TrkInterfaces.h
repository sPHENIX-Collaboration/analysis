/// ---------------------------------------------------------------------------
/*! \file   TrkInterfaces.h
 *  \author Derek Anderson
 *  \date   03.05.2024
 *
 *  Track-related interfaces.
 */
/// ---------------------------------------------------------------------------


#ifndef SCORRELATORUTILITIES_TRKINTERFACES_H
#define SCORRELATORUTILITIES_TRKINTERFACES_H

// c++ utilities
#include <string>
#include <vector>
#include <utility>
#include <cassert>
// root libraries
#include <TF1.h>
// phool libraries
#include <phool/phool.h>
#include <phool/getClass.h>
#include <phool/PHIODataNode.h>
#include <phool/PHNodeIterator.h>
#include <phool/PHCompositeNode.h>
// tracking libraries
#include <trackbase_historic/SvtxTrack.h>
#include <trackbase_historic/SvtxTrackMap.h>
// analysis utilities
#include "Constants.h"

// make common namespaces implicit
using namespace std;



namespace SColdQcdCorrelatorAnalysis {
  namespace Interfaces {

    // track interfaces -------------------------------------------------------

    TF1*          GetSigmaDcaTF1(const string name, const vector<float> params, const pair<float, float> range);
    SvtxTrackMap* GetTrackMap(PHCompositeNode* topNode);

  }  // end Interfaces namespace
}  // end SColdQcdCorrealtorAnalysis namespace

#endif

// end ------------------------------------------------------------------------
