// ----------------------------------------------------------------------------
// 'DoTrackCutStudy.C'
// Derek Anderson
// 12.15.2022
//
// Runs the 'STrackCutStudy' class with
// maximal output.  (Has to be run via
// condor.)
// ----------------------------------------------------------------------------

#ifndef DOTRACKCUTSTUDY_C
#define DOTRACKCUTSTUDY_C

// standard c includes
#include <cstdlib>
#include <utility>
// root includes
#include "TROOT.h"
#include "TString.h"
// user includes
#include </sphenix/user/danderson/install/include/strackcutstudy/STrackCutStudy.h>

using namespace std;

// load libraries
R__LOAD_LIBRARY(/sphenix/user/danderson/install/lib/libstrackcutstudy.so)

// global constants
static const Ssiz_t NTxt     = 3;
static const Bool_t DefBatch = false;



void DoTrackCutStudy(const Bool_t inBatchMode = DefBatch) {

  // lower verbosity
  gErrorIgnoreLevel = kWarning;

  // i/o parameters
  const TString sOutFile("trackCutStudy.testFastCutStudy_slow.pt020n20pim.d30m3y2023.root");
  const TString sInFileEO("input/embed_only/final_merge/sPhenixG4_run6hijing50khz_embedScanOn_embedOnly.pt020n20pim.d24m3y2023.root");
  const TString sInFilePU("input/with_pileup/merged/sPhenixG4_forTrackCutStudy_hijing50khz_embedOnly0t19_g4svtxeval.d14m11y2022.root");
  const TString sInTupleEO("ntp_track");
  const TString sInTuplePU("ntp_gtrack");
  const TString sInClusterEO("ntp_cluster");

  // study parameters
  const Bool_t   makePlots(true);
  const Bool_t   doPileup(true);
  const Bool_t   doIntNorm(false);
  const Bool_t   doBeforeCuts(true);
  const Bool_t   doAvgClusterCalc(false);
  const Double_t normalPtFracMin(0.20);
  const Double_t normalPtFracMax(1.20);

  // cut flags
  const Bool_t doPrimaryCut = false;
  const Bool_t doMVtxCut    = true;
  const Bool_t doTpcCut     = true;
  const Bool_t doVzCut      = true;
  const Bool_t doDcaXyCut   = false;
  const Bool_t doDcaZcut    = false;
  const Bool_t doQualityCut = true;

  // track cuts
  const pair<UInt_t,   UInt_t>   nMVtxRange   = {2,    100};
  const pair<UInt_t,   UInt_t>   nTpcRange    = {20,   100};
  const pair<Double_t, Double_t> vzRange      = {-10.,  10.};
  const pair<Double_t, Double_t> dcaXyRange   = {-20., 20.};
  const pair<Double_t, Double_t> dcaZrange    = {-20., 20.};
  const pair<Double_t, Double_t> qualityRange = {0.,   10.};

  // text for plot
  const TString sTxtEO[NTxt] = {"#bf{#it{sPHENIX}} Simulation", "20 #pi^{-}/event, p_{T} #in (0, 20) GeV/c", "#bf{Only #pi^{-}}"};
  const TString sTxtPU[NTxt] = {"#bf{#it{sPHENIX}} Simulation", "0-20 fm Hijing, 50 kHz pileup #oplus 5 #pi^{-}/event", "#bf{With pileup}"};

  // run track cut study
  STrackCutStudy *study = new STrackCutStudy();
  study -> SetBatchMode(inBatchMode);
  study -> SetInputOutputFiles(sInFileEO, sInFilePU, sOutFile);
  study -> SetInputTuples(sInTupleEO, sInTuplePU);
  study -> SetStudyParameters(makePlots, doPileup, doIntNorm, doBeforeCuts, doAvgClusterCalc, normalPtFracMin, normalPtFracMax);
  study -> SetCutFlags(doPrimaryCut, doMVtxCut, doTpcCut, doVzCut, doDcaXyCut, doDcaZcut, doQualityCut);
  study -> SetTrackCuts(nMVtxRange, nTpcRange, vzRange, dcaXyRange, dcaZrange, qualityRange);
  study -> SetPlotText(NTxt, NTxt, sTxtEO, sTxtPU);
  study -> Init();
  study -> Analyze();
  study -> End();

}  // end 'DoTrackCutStudy()'

#endif

// end ------------------------------------------------------------------------
