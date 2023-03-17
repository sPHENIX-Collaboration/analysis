// ----------------------------------------------------------------------------
// 'DoTrackCutStudy.C'
// Derek Anderson
// 12.15.2022
//
// Runs the 'STrackCutStudy'
// class.
// ----------------------------------------------------------------------------

#ifndef DOTRACKCUTSTUDY_C
#define DOTRACKCUTSTUDY_C

// standard c includes
#include <cstdlib>
#include <utility>
// root includes
#include <TROOT.h>
#include <TString.h>
// user includes
#include </sphenix/user/danderson/install/include/strackcutstudy/STrackCutStudy.h>

using namespace std;

// load libraries
R__LOAD_LIBRARY(/sphenix/user/danderson/install/lib/libstrackcutstudy.so)

// global constants
static const Ssiz_t NTxt = 3;



void DoTrackCutStudy() {

  // lower verbosity
  gErrorIgnoreLevel = kWarning;

  // i/o parameters
  const TString sOutFile("trackCutStudy.applyCuts_lookingAtWeirdTracksBeforeDca.pt020n5pim.d16m3y2023.root");
  const TString sInFileEO("input/embed_only/final_merge/sPhenixG4_forTrackCutStudy_embedOnly0t1099_g4svtxeval.pt020n5pim.d12m1y2023.root");
  const TString sInFilePU("input/test/sPhenixG4_testWithPileup001_g4svtxEval.d18m12y2022.root");
  const TString sInTupleEO("ntp_track");
  const TString sInTuplePU("ntp_gtrack");
  const TString sInClusterEO("ntp_cluster");

  // study parameters
  const Bool_t   doIntNorm(false);
  const Bool_t   doAvgClusterCalc(false);
  const Double_t normalPtFracMin(0.20);
  const Double_t normalPtFracMax(1.20);

  // cut flags
  const Bool_t doPrimaryCut = false;
  const Bool_t doMVtxCut    = true;
  const Bool_t doTpcCut     = true;
  const Bool_t doVzCut      = false;
  const Bool_t doDcaXyCut   = false;
  const Bool_t doDcaZcut    = false;
  const Bool_t doQualityCut = false;

  // track cuts
  const pair<UInt_t,   UInt_t>   nMVtxRange   = {2,    100};
  const pair<UInt_t,   UInt_t>   nTpcRange    = {20,   100};
  const pair<Double_t, Double_t> vzRange      = {-10.,  10.};
  const pair<Double_t, Double_t> dcaXyRange   = {-20., 20.};
  const pair<Double_t, Double_t> dcaZrange    = {-20., 20.};
  const pair<Double_t, Double_t> qualityRange = {0.,   10.};

  // text for plot
  const TString sTxtEO[NTxt] = {"#bf{#it{sPHENIX}} Simulation", "5 #pi^{-}/event, p_{T} #in (0, 20) GeV/c", "#bf{Only #pi^{-}}"};
  const TString sTxtPU[NTxt] = {"#bf{#it{sPHENIX}} Simulation", "0-20 fm Hijing, 50 kHz pileup #oplus 5 #pi^{-}/event", "#bf{With pileup}"};

  // run track cut study
  STrackCutStudy *study = new STrackCutStudy();
  study -> SetInputOutputFiles(sInFileEO, sInFilePU, sOutFile);
  study -> SetInputTuples(sInTupleEO, sInTuplePU);
  study -> SetStudyParameters(doIntNorm, doAvgClusterCalc, normalPtFracMin, normalPtFracMax);
  study -> SetCutFlags(doPrimaryCut, doMVtxCut, doTpcCut, doVzCut, doDcaXyCut, doDcaZcut, doQualityCut);
  study -> SetTrackCuts(nMVtxRange, nTpcRange, vzRange, dcaXyRange, dcaZrange, qualityRange);
  study -> SetPlotText(NTxt, NTxt, sTxtEO, sTxtPU);
  study -> Init();
  study -> Analyze();
  study -> End();

}  // end 'DoTrackCutStudy()'

#endif

// end ------------------------------------------------------------------------
