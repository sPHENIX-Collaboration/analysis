/// ---------------------------------------------------------------------------
/*! \file   CorrelatorResponseMakerOptions.h
 *  \author Derek Anderson
 *  \date   05.16.2024
 *
 *  Options for the 'SCrrelatorResponseMaker' module.
 */
/// ---------------------------------------------------------------------------

#ifndef CORRELATORRESPONSEMAKEROPTIONS_H
#define CORRELATORRESPONSEMAKEROPTIONS_H 

// c++ utilities
#include <string>
#include <utility>
#include <optional>
// analysis utilities
#include <scorrelatorutilities/Types.h>
#include <scorrelatorutilities/Constants.h>
// module configuration
#include <scorrelatorresponsemaker/SCorrelatorResponseMakerConfig.h>

// make common namespaces implicit
using namespace std;
using namespace SColdQcdCorrelatorAnalysis;



namespace CorrelatorResponseMakerOptions {

  // options===================================================================

  // io options
  const string sOutFile("test.root");
  const vector<string> sInFiles = {
    "/sphenix/tg/tg01/jets/danderson/eec/SCorrelatorJetTreeMakerOutput/test/testJob_outputData_CorrelatorJetTree_00000.root",
    "/sphenix/tg/tg01/jets/danderson/eec/SCorrelatorJetTreeMakerOutput/test/testJob_outputData_CorrelatorJetTree_00000.root"
  };
  const vector<string> sInTrees = {
    "RecoJetTree"
    "TruthJetTree"
  };

  // matching options
  const double               minMatchCstPercent = 0.2;
  const pair<double, double> qtJetMatchRange    = {0.2, 1.5};
  const pair<double, double> drJetMatchRange    = {0.,  0.4};

  // misc options
  const int  verbosity = 0;
  const bool doDebug   = false;



  // set up configurations ====================================================

  // --------------------------------------------------------------------------
  //! Generate configuration for response maker
  // --------------------------------------------------------------------------
  SCorrelatorResponseMakerConfig GetConfig(
    const bool doBatch = false,
    const int verbose = verbosity
  ) {

    // TODO simplify options!
    SCorrelatorResponseMakerConfig cfg {
      .verbosity         = verbose,
      .inDebugMode       = doDebug,
      .inBatchMode       = doBatch,
      .outFileName       = sOutFile,
      .inRecoFileName    = sInFiles[0],
      .inTrueFileName    = sInFiles[1],
      .inRecoTreeName    = sInTrees[0],
      .inTrueTreeName    = sInTrees[1],
      .fracCstMatchRange = make_pair(minMatchCstPercent, 100.),
      .jetMatchQtRange   = qtJetMatchRange,
      .jetMatchDrRange   = drJetMatchRange
    };
    return cfg;

  }  // end 'GetRecoConfig(vector<string>, string, int)'

}  // end CorrelatorResponseMakerOptions namespace

#endif

// end ------------------------------------------------------------------------
