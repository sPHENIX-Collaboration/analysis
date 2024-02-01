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

// c++ utilities
#include <string>
#include <optional>
// plugin definitions
#include "SCheckTrackPairs.h"
#include "SMakeTrkQATuples.h"

using namespace std;



// SCorrelatorQAMaker definition --------------------------------------------------

namespace SColdQcdCorrelatorAnalysis {

  class SCorrelatorQAMaker {

    public:

      // ctor/dtor
      SCorrelatorQAMaker() {};
      ~SCorrelatorQAMaker();

      // global setters
      void SetGlobalDebug(const bool debug);
      void SetGlobalOutFile(const string sOutFile);
      void SetGlobalVerbosity(const int verbosity);

      // plugin initializers
      template <typename T> void InitPlugin(const T& config, optional<string> name);

      // plugin accessors
      SCheckTrackPairs* CheckTrackPairs()   {return m_checkTrackPairs;}
      SMakeTrkQATuples* MakeTrackQATuples() {return m_makeTrackQATuples;}

    private:

      // plugins
      SCheckTrackPairs* m_checkTrackPairs;
      SMakeTrkQATuples* m_makeTrackQATuples;

  };

}  // end SColdQcdCorrelatorAnalysis namespace

#endif

// end ------------------------------------------------------------------------
