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
#include <utility>
// analysis utilities
#include "src/SLambdaJetHunterConfig.h"
#include "/sphenix/user/danderson/eec/SCorrelatorUtilities/EvtTools.h"
#include "/sphenix/user/danderson/eec/SCorrelatorUtilities/GenTools.h"
#include "/sphenix/user/danderson/eec/SCorrelatorUtilities/JetTools.h"

// make common namespacs implicit
using namespace std;
using namespace SColdQcdCorrelatorAnalysis;
using namespace SColdQcdCorrelatorAnalysis::SCorrelatorUtilities;



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

  // set up configuration -----------------------------------------------------

  // set particle acceptance
  pair<ParInfo, ParInfo> cfg_parAccept;
  cfg_parAccept.first.pt   = ptParRange.first;
  cfg_parAccept.first.eta  = etaParRange.first;
  cfg_parAccept.second.pt  = ptParRange.second;
  cfg_parAccept.second.eta = etaParRange.second;

  pair<JetInfo, JetInfo> cfg_jetAccept;
  cfg_jetAccept.first.pt   = ptJetRange.first;
  cfg_jetAccept.first.eta  = etaJetRange.first;
  cfg_jetAccept.second.pt  = ptJetRange.second;
  cfg_jetAccept.second.eta = etaJetRange.second;

  // module configuration
  SLambdaJetHunterConfig Config {
    .isDebugOn   = true,
    .isEmbed     = false,
    .moduleName  = "SLambdaJetHunter",
    .outTreeName = "LambdaJetTree",
    .rJet        = 0.4,
    .jetAlgo     = SLambdaJetHunter::Algo::AntiKt,
    .jetRecomb   = SLambdaJetHunter::Recomb::Pt,
    .vzAccept    = vzEvtRange,
    .vrAccept    = vrEvtRange,
    .parAccept   = cfg_parAccept,
    .jetAccept   = cfg_jetAccept
  };

}  // end LambdaJetHunterOptions namespace

#endif

// end ------------------------------------------------------------------------
