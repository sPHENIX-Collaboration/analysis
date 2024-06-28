// ----------------------------------------------------------------------------
// 'GenTools.h'
// Derek Anderson
// 03.06.2024
//
// Collection of frequent event-level generator methods utilized
// in the sPHENIX Cold QCD Energy-Energy Correlator analysis.
// ----------------------------------------------------------------------------

#ifndef SCORRELATORUTILITIES_GENTOOLS_H
#define SCORRELATORUTILITIES_GENTOOLS_H

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

// c++ utilities
#include <limits>
#include <string>
#include <vector>
#include <optional>
// root libraries
#include <Math/Vector3D.h>
// phool libraries
#include <phool/PHCompositeNode.h>
// hepmc includes
#include <HepMC/GenEvent.h>
#include <HepMC/GenVertex.h>
#include <HepMC/GenParticle.h>
#include <phhepmc/PHHepMCGenEvent.h>
#include <phhepmc/PHHepMCGenEventMap.h>
// analysis utilities
#include "ParInfo.h"
#include "ParTools.h"
#include "Constants.h"
#include "Interfaces.h"

#pragma GCC diagnostic pop

// make common namespaces implicit
using namespace std;



namespace SColdQcdCorrelatorAnalysis {
  namespace Tools {

    // gen tools --------------------------------------------------------------

    int64_t        GetNumFinalStatePars(PHCompositeNode* topNode, const vector<int> evtsToGrab, const Const::Subset subset, optional<float> chargeToGrab = nullopt);
    double         GetSumFinalStateParEne(PHCompositeNode* topNode, const vector<int> evtsToGrab, const Const::Subset subset, optional<float> chargeToGrab = nullopt);
    Types::ParInfo GetPartonInfo(PHCompositeNode* topNode, const int event, const int status);

  }  // end Tools namespace
}  // end SColdQcdCorrealtorAnalysis namespace

#endif

// end ------------------------------------------------------------------------
