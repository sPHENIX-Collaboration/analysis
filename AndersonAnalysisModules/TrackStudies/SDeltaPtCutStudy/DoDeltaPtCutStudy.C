// ----------------------------------------------------------------------------
// 'DoTrackCutStudy.C'
// Derek Anderson
// 06.29.2023
//
// Runs the 'SDeltaPtCutStudy' class.
// ----------------------------------------------------------------------------

#ifndef DODELTAPTCUTSTUDY_C
#define DODELTAPTCUTSTUDY_C

// standard c includes
#include <array>
#include <cstdlib>
#include <utility>
// root includes
#include <TROOT.h>
#include <TString.h>
// user includes
#include "/sphenix/user/danderson/install/include/sdeltaptcutstudy/SDeltaPtCutStudy.h"

using namespace std;

// load libraries
R__LOAD_LIBRARY(/sphenix/user/danderson/install/lib/libsdeltaptcutstudy.so)

// global constants
static const bool   DefBatch = false;
static const size_t NPar     = 3;
static const size_t NTypes   = 3;



void DoDeltaPtCutStudy(const bool inBatchMode = DefBatch) {

  // lower verbosity
  gErrorIgnoreLevel = kWarning;

  // io parameters
  const TString sInFile("../TruthMatching/input/merged/sPhenixG4_testingNewMatcher_oldEvaluator.pt020num10evt500pipm.d15m9y2023.root");
  const TString sOutFile("deltaPtStudy.testingSignChange_piboth.pt020num5evt500pipm.d3m10y2023.root");
  const TString sInTrack("ntp_track");
  const TString sInTruth("ntp_gtrack");

  // projection parameters
  //   <0> = bin center of projection
  //   <1> = histogram suffix
  //   <2> = histogram color
  //   <3> = histogram marker
  //   <4> = fit color
  const vector<tuple<double, TString, uint32_t, uint32_t, uint32_t>> projParams = {
    make_tuple(0.5, "_pt05", 799, 20, 803),
    make_tuple(1.0, "_pt1",  633, 22, 636),
    make_tuple(2.0, "_pt2",  899, 23, 893),
    make_tuple(5.0, "_pt5",  617, 21, 620),
    make_tuple(10., "_pt10", 879, 33, 883),
    make_tuple(20., "_pt20", 859, 34, 863),
    make_tuple(30., "_pt30", 839, 47, 843),
    make_tuple(40., "_pt40", 819, 20, 813)
  };

  // flat delta-pt cut parameters
  //   <0> = max delta-pt
  //   <1> = histogram suffix
  //   <2> = histogram color
  //   <3> = histogram marker
  //   <4> = draw this one?
  //         (only 1st cut set to true will be drawn)
  const vector<tuple<double, TString, uint32_t, uint32_t, bool>> flatParams = {
    make_tuple(0.50, "_dPt50", 899, 24, false),
    make_tuple(0.25, "_dPt25", 909, 26, false),
    make_tuple(0.10, "_dPt10", 879, 32, false),
    make_tuple(0.05, "_dPt05", 889, 25, false),
    make_tuple(0.03, "_dPt03", 859, 27, true),
    make_tuple(0.02, "_dPt02", 869, 28, false),
    make_tuple(0.01, "_dPt01", 839, 30, false)
  };

  // pt-dependent delta-pt cut parameters
  //   <0> = max n-sigma away from mean
  //   <1> = histogram suffix
  //   <2> = histogram color
  //   <3> = histogram marker
  //   <4> = fit color
  //   <5> = draw this one?
  //         (only 1st cut set to true will be drawn)
  const vector<tuple<double, TString, uint32_t, uint32_t, uint32_t, bool>> ptDependParams = {
    make_tuple(1.0, "_sigDPt10", 899, 24, 893, false),
    make_tuple(1.5, "_sigDPt15", 909, 26, 903, false),
    make_tuple(2.0, "_sigDPt20", 879, 32, 873, true),
    make_tuple(2.5, "_sigDPt25", 889, 25, 883, false),
    make_tuple(3.0, "_sigDPt30", 859, 27, 863, false) 
  };

  // fit guesses, and norm and fit ranges
  array<float, NPar> sigHiGuess    = {1., -1., 1.};
  array<float, NPar> sigLoGuess    = {1., -1., 1.};
  pair<float, float> normRange     = {0.2, 1.2};
  pair<float, float> ptFitRange    = {0.5, 40.};
  pair<float, float> deltaFitRange = {0.,  0.1};

  // general track cuts
  const uint32_t nInttTrkMin = 1;
  const uint32_t nMVtxTrkMin = 2;
  const uint32_t nTpcTrkMin  = 35;
  const double   qualTrkMax  = 10.;
  const double   vzTrkMax    = 10.;
  const double   ptTrkMin    = 0.1;

  // general style parameters
  const pair<float, float>      rPtRange    = {0., 60.};
  const pair<float, float>      rFracRange  = {0., 4.};
  const pair<float, float>      rDeltaRange = {0., 0.1};
  const array<uint32_t, NTypes> arrColGraph = {923, 923, 809};
  const array<uint32_t, NTypes> arrMarGraph = {20,  20,  46};

  // general histogram parameters
  const uint32_t fFil = 0;
  const uint32_t fLin = 1;
  const uint32_t fWid = 1;
  const uint32_t fTxt = 42;
  const uint32_t fAln = 12;
  const uint32_t fCnt = 1;

  // bases of histogram names
  const TString sPtProjBase("DeltaPtProj");
  const TString sPtDeltaBase("DeltaPt");
  const TString sPtTrueBase("PtTrue");
  const TString sPtRecoBase("PtReco");
  const TString sPtFracBase("PtFrac");
  const TString sPtTrkTruBase("PtTrkTruth");

  // plot text
  const vector<TString> sPlotTxt = {
    "#bf{#it{sPHENIX}} Simulation",
    "5 #pi^{-} + 5 #pi^{+}/event, p_{T} #in (0, 20) GeV/c",
    "#bf{Only #pi^{#pm}}"
  };

  // misc plot parameters
  bool   doEffRebin = true;
  size_t nEffRebin  = 5;

  // run track cut study
  SDeltaPtCutStudy *study = new SDeltaPtCutStudy();
  study -> SetInputOutputFiles(sInFile, sOutFile);
  study -> SetInputTuples(sInTrack, sInTruth);
  study -> SetGeneralTrackCuts(nInttTrkMin, nMVtxTrkMin, nTpcTrkMin, qualTrkMax, vzTrkMax, ptTrkMin);
  study -> SetSigmaFitGuesses(sigHiGuess, sigLoGuess);
  study -> SetNormAndFitRanges(normRange, ptFitRange, deltaFitRange);
  study -> SetPlotRanges(rPtRange, rFracRange, rDeltaRange);
  study -> SetGeneralStyleParameters(arrColGraph, arrMarGraph);
  study -> SetGeneralHistParameters(fFil, fLin, fWid, fTxt, fAln, fCnt);
  study -> SetHistBaseNames(sPtProjBase, sPtDeltaBase, sPtTrueBase, sPtRecoBase, sPtFracBase, sPtTrkTruBase);
  study -> SetPlotText(sPlotTxt);
  study -> SetEffRebinParameters(doEffRebin, nEffRebin);
  study -> SetProjectionParameters(projParams);
  study -> SetFlatCutParameters(flatParams);
  study -> SetPtDependCutParameters(ptDependParams);
  study -> Init();
  study -> Analyze();
  study -> End();

}  // end 'DoDeltaPtCutStudy()'

#endif

// end ------------------------------------------------------------------------
