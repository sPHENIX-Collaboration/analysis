// ----------------------------------------------------------------------------
// 'MatcherComparatorOptions.h'
// Derek Anderson
// 01.30.2024
//
// Options for the STrackMatcherComparator module.
// ----------------------------------------------------------------------------

#ifndef MATCHERCOMPARATOROPTIONS_H
#define MATCHERCOMPARATOROPTIONS_H 

// c++ utilities
#include <string>
#include <vector>
#include <utility>
// analysis utilities
#include "/sphenix/user/danderson/install/include/strackmatchercomparator/STrackMatcherComparatorConfig.h"
#include "/sphenix/user/danderson/install/include/strackmatchercomparator/STrackMatcherComparatorHistDef.h"

// make common namespaces implicit
using namespace std;



namespace MatcherComparatorOptions {

  // set up configuration -----------------------------------------------------

  STrackMatcherComparatorConfig GetConfig() {

    // simulation info
    const vector<string> cfg_info = {
      "#bf{#it{sPHENIX}} Simulation",
      "1 #pi^{-}/event, p_{T} = 10 GeV/c",
      "#bf{Only #pi^{-}}"
    };

    // phi sectors
    const vector<pair<float, float>> cfg_sectors = {
      make_pair(-2.92, 0.12),
      make_pair(-2.38, 0.05),
      make_pair(-1.93, 0.18),
      make_pair(-1.33, 0.07),
      make_pair(-0.90, 0.24),
      make_pair(-0.29, 0.09),
      make_pair(0.23,  0.11),
      make_pair(0.73,  0.10),
      make_pair(1.28,  0.10),
      make_pair(1.81,  0.08),
      make_pair(2.23,  0.18),
      make_pair(2.80,  0.17)
    };

    STrackMatcherComparatorConfig cfg {
      .outFileName     = "matchComparison.noVtxDistribution_vzM1010.pt10num1evt500pim.d8m2y2024.root",
      .newInFileName   = "input/merged/sPhenixG4_oneMatchPerParticleAndEmbedScanOn_noVtxDistribution_newMatcher.pt10num1evt500pim.d8m2y2024.root",
      .oldInFileName   = "input/merged/sPhenixG4_oneMatchPerParticleAndEmbedScanOn_noVtxDistribution_oldEval.pt10num1evt500pim.d8m2y2024.root",
      .useOnlyPrimTrks = true,
      .doZVtxCut       = true,
      .doPhiCut        = true,
      .oddPtFrac       = {0.5,  1.50},
      .zVtxRange       = {-10., 10.},
      .phiSectors      = cfg_sectors,
      .info            = cfg_info,
      .doIntNorm       = true,
      .matchVertScales = true
    };
    return cfg;

  }  // end 'GetConfig()'



  // set up histogram definition ----------------------------------------------

  STrackMatcherComparatorHistDef GetHistDef() {

    // no. of histogram bins
    const uint32_t def_nPtBins = 101;

    // histogram bin ranges
    const pair<float, float> def_xPtBins = {-0.5, 100.5};

    STrackMatcherComparatorHistDef def;
    def.nPtBins = def_nPtBins;
    def.xPtBins = def_xPtBins;
    return def;

  }  // end 'GetHistDef()'

}  // end MatcherComparatorOptions namespace

#endif

// end ------------------------------------------------------------------------
