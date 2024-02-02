// ----------------------------------------------------------------------------
// 'STrackMatcherComparatorConfig.h'
// Derek Anderson
// 01.30.2024
//
// Configuration struct for 'STrackMatcherComparator' module.
// ----------------------------------------------------------------------------

#ifndef STRACKMATCHERCOMPARATORCONFIG_H
#define STRACKMATCHERCOMPARATORCONFIG_H

// c++ utilities
#include <string>
#include <vector>
#include <utility>

// make common namespaces implicit
using namespace std;



// STrackMatcherComparatorConfig definition -----------------------------------

struct STrackMatcherComparatorConfig {

  // file options
  string outFileName   = "test.hists.root";
  string newInFileName = "";
  string oldInFileName = "";

  // tree/tuple options
  string newTreeTrueName  = "T";
  string newTreeRecoName  = "T";
  string newTupleTrueName = "ntForEvalComp";
  string newTupleRecoName = "ntForEvalComp";
  string oldTupleTrueName = "ntp_gtrack";
  string oldTupleRecoName = "ntp_track";

  // histogram labels
  string newTreeLabel  = "NewTree";
  string newTupleLabel = "NewTuple";
  string oldTupleLabel = "OldTuple";

  // directory names
  vector<string> histDirNames = {
    "NewTreeHists",
    "NewTupleHists",
    "OldTupleHists"
  };
  vector<string> ratioDirNames = {
    "TreeRatios",
    "TupleRatios"
  };
  vector<string> plotDirNames = {
    "TreePlots",
    "TuplePlots"
  };

  // cuts
  pair<float, float> oddPtFrac = {0.50, 1.50};

  // style options
  pair<uint32_t, uint32_t> fCol = {923, 899};
  pair<uint32_t, uint32_t> fMar = {20,  25};
  pair<uint32_t, uint32_t> fLin = {1,   1};
  pair<uint32_t, uint32_t> fWid = {1,   1};
  pair<uint32_t, uint32_t> fFil = {0,   0};

  // plot text parameters
  string legOld = "evaluator";
  string legNew = "truth matcher";
  string header = "";
  string count  = "counts";
  string norm   = "normalized counts";
  string ratio  = "ratio";

  // plot information
  vector<string> info = {
    "#bf{#it{sPHENIX}} Simulation",
    "1 #pi^{-}/event, p_{T} = 10 GeV/c",
    "#bf{Only #pi^{-}}"
  };

  // plot options
  bool doIntNorm       = false;
  bool matchVertScales = true;

};

#endif

// end ------------------------------------------------------------------------
