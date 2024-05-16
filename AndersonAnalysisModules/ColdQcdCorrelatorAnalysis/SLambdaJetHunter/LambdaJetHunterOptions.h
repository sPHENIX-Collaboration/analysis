// ----------------------------------------------------------------------------
// 'LambdaJetHunterOptions.h'
// Derek Anderson
// 01.25.2024
//
// Options for the SLambdaJetHunter module.
// ----------------------------------------------------------------------------

#ifndef LAMBDAJETHUNTEROPTIONS_H
#define LAMBDAJETHUNTEROPTIONS_H 

// c++ utilities
#include <string>
#include <utility>
// analysis utilities
#include "/sphenix/user/danderson/install/include/scorrelatorutilities/Types.h"
#include "/sphenix/user/danderson/install/include/scorrelatorutilities/Constants.h"
#include "/sphenix/user/danderson/install/include/slambdajethunter/SLambdaJetHunter.h"
#include "/sphenix/user/danderson/install/include/slambdajethunter/SLambdaJetHunterConfig.h"

// make common namespacs implicit
using namespace std;
using namespace SColdQcdCorrelatorAnalysis;



namespace LambdaJetHunterOptions {

  // acceptance cuts ----------------------------------------------------------

  // event acceptance
  const pair<float, float> vzEvtRange = {-10., 10.};
  const pair<float, float> vrEvtRange = {0.0,  0.418};

  // particle acceptance
  const pair<float, float> ptParRange  = {0.,   100.};
  const pair<float, float> etaParRange = {-1.1, 1.1};

  // jet acceptance
  const pair<float, float> ptJetRange  = {0.1, 100.};
  const pair<float, float> etaJetRange = {-0.7, 0.7};



  // bundle acceptances into pairs --------------------------------------------

  pair<Types::ParInfo, Types::ParInfo> GetParAccept() {

    // create maximal range
    pair<Types::ParInfo, Types::ParInfo> parAccept = {
      Types::ParInfo(Const::Init::Minimize),
      Types::ParInfo(Const::Init::Maximize)
    };

    // set specific bounds
    parAccept.first.SetPT( ptParRange.first );
    parAccept.first.SetEta( etaParRange.first );
    parAccept.second.SetPT( ptParRange.second );
    parAccept.second.SetEta( etaParRange.second );
    return parAccept;

  }  // end 'GetParAccept()'



  pair<Types::JetInfo, Types::JetInfo> GetJetAccept() {

    // create maximal range
    pair<Types::JetInfo, Types::JetInfo> jetAccept = {
      Types::JetInfo(Const::Init::Minimize),
      Types::JetInfo(Const::Init::Maximize)
    };

    // set specific bounds
    jetAccept.first.SetPT( ptJetRange.first );
    jetAccept.first.SetEta( etaJetRange.first );
    jetAccept.second.SetPT( ptJetRange.second );
    jetAccept.second.SetEta( etaJetRange.second );
    return jetAccept;

  }  // end 'GetJetAccept()'



  // set up configuration -----------------------------------------------------

  SLambdaJetHunterConfig GetConfig(const int verbosity, const string outFile) {

    SLambdaJetHunterConfig cfg {
      .verbosity   = verbosity,
      .isDebugOn   = true,
      .isEmbed     = false,
      .moduleName  = "SLambdaJetHunter",
      .outTreeName = "LambdaJetTree",
      .outFileName = outFile,
      .associator  = SLambdaJetHunter::Associator::Barcode,
      .isCharged   = false,
      .rJet        = 0.4,
      .jetAlgo     = "antikt_algorithm",
      .jetRecomb   = "pt_scheme",
      .vzAccept    = vzEvtRange,
      .vrAccept    = vrEvtRange,
      .parAccept   = GetParAccept(),
      .jetAccept   = GetJetAccept()
    };
    return cfg;

  }  // end 'GetConfig(int, string&)'

}  // end LambdaJetHunterOptions namespace

#endif

// end ------------------------------------------------------------------------
