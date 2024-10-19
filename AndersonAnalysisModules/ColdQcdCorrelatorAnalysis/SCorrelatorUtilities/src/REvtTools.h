/// ---------------------------------------------------------------------------
/*! \file   REvtTools.h
 *  \author Derek Anderson
 *  \date   03.06.2024
 *
 *  Collection of frequent event-level reconstruction methods utilized
 *  in the sPHENIX Cold QCD Energy-Energy Correlator analysis.
 */
/// ---------------------------------------------------------------------------

#ifndef SCORRELATORUTILITIES_REVTTOOLS_H
#define SCORRELATORUTILITIES_REVTTOOLS_H

// c++ utilities
#include <cmath>
#include <limits>
#include <string>
#include <vector>
// root libraries
#include <Math/Vector3D.h>
// phool libraries
#include <phool/PHCompositeNode.h>
// calo includes
#include <calobase/RawCluster.h>
#include <calobase/RawClusterUtility.h>
#include <calobase/RawClusterContainer.h>
// tracking includes
#include <trackbase_historic/SvtxTrack.h>
#include <trackbase_historic/SvtxTrackMap.h>
// analysis utilities
#include "Constants.h"
#include "Interfaces.h"

// make common namespaces implicit
using namespace std;



namespace SColdQcdCorrelatorAnalysis {
  namespace Tools {

    // event-level reconstructed tools ----------------------------------------

    int64_t GetNumTrks(PHCompositeNode* topNode);
    double  GetSumTrkMomentum(PHCompositeNode* topNode);
    double  GetSumCaloEne(PHCompositeNode* topNode, const string store);

  }  // end Tools namespace
}  // end SColdQcdCorrealtorAnalysis namespace

#endif

// end ------------------------------------------------------------------------
