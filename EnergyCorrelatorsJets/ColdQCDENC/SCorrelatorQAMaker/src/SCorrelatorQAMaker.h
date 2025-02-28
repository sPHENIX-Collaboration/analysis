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
#include <cassert>
#include <optional>
// plugin definitions
#include "SCheckTrackPairs.h"
#include "SMakeTrackQATuple.h"
#include "SMakeClustQATree.h"
#include "SReadLambdaJetTree.h"

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
      template <typename T> void InitPlugin(const T& config, optional<string> name = nullopt);

      // plugin accessors
      SCheckTrackPairs*   CheckTrackPairs()   {return m_checkTrackPairs;}
      SMakeTrackQATuple*  MakeTrackQATuple()  {return m_makeTrackQATuple;}
      SMakeClustQATree*   MakeClustQATree()   {return m_makeClustQATree;}
      SReadLambdaJetTree* ReadLambdaJetTree() {return m_readLambdaJetTree;}

    private:

      // plugins
      SCheckTrackPairs*   m_checkTrackPairs   = NULL;
      SMakeTrackQATuple*  m_makeTrackQATuple  = NULL;
      SMakeClustQATree*   m_makeClustQATree   = NULL;
      SReadLambdaJetTree* m_readLambdaJetTree = NULL;

  };

}  // end SColdQcdCorrelatorAnalysis namespace

#endif

// end ------------------------------------------------------------------------
