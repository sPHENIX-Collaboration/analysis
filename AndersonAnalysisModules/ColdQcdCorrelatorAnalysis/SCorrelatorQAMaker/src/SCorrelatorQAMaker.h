// ----------------------------------------------------------------------------
// 'SCorrelatorQAMaker.h'
// Derek Anderson
// 10.04.2023
//
// A module to produce TNtuples and histograms for QA
// plots relevant to the sPHENIX Cold QCD Energy-Energy
// Correlator analysis.
// ----------------------------------------------------------------------------

#ifndef SCORRELATORQAMAKER_H
#define SCORRELATORQAMAKER_H

// plugin definitions
#include "SCheckTrackPairs.h"

using namespace std;



// SCorrelatorQAMaker definition --------------------------------------------------

namespace SColdQcdCorrelatorAnalysis {

  class SCorrelatorQAMaker {

    public:

      // ctor/dtor
      SCorrelatorQAMaker();
      ~SCorrelatorQAMaker();

      // plugin accessors
      SCheckTrackPairs* CheckTrackPairs() {return m_checkTrackPairs;}

    private:

      // plugins
      SCheckTrackPairs* m_checkTrackPairs;

  };

}  // end SColdQcdCorrelatorAnalysis namespace

#endif

// end ------------------------------------------------------------------------
