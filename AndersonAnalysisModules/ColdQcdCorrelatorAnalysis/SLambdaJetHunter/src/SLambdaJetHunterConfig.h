// ----------------------------------------------------------------------------
// 'SLambdaJetHunterConfig.h'
// Derek Anderson
// 01.18.2024
//
// Configuration struct for 'SLambdaJetHunter' module.
// ----------------------------------------------------------------------------

#ifndef SLAMBDAJETHUNTERCONFIG_H
#define SLAMBDAJETHUNTERCONFIG_H

// c++ utilities
#include <string>
#include <vector>
#include <utility>
// analysis utilities
#include "/sphenix/user/danderson/eec/SCorrelatorUtilities/GenTools.h"
#include "/sphenix/user/danderson/eec/SCorrelatorUtilities/JetTools.h"

// make common namespaces implicit
using namespace std;
using namespace SColdQcdCorrelatorAnalysis::SCorrelatorUtilities;



namespace SColdQcdCorrelatorAnalysis {

  // SLambdaJetHunterConfig definition ----------------------------------------

  struct SLambdaJetHunterConfig {

    // system options
    int    verbosity     = 0;
    bool   isDebugOn     = false;
    bool   isEmbed       = false;
    string moduleName    = "SLambdaJetHunter";
    string outFileName   = "";
    string outTreeName   = "LambdaJetTree";

    // jet options
    float    rJet      = 0.4;
    uint32_t jetAlgo   = 0;
    uint32_t jetRecomb = 0;

    // acceptance parameters
    pair<float,   float>   vzAccept;
    pair<float,   float>   vrAccept;
    pair<ParInfo, ParInfo> parAccept;
    pair<JetInfo, JetInfo> jetAccept;

  };

}  // end SColdQcdCorrelatorAnalysis namespace

#endif

// end ------------------------------------------------------------------------

