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

// root includes
#include <TROOT.h>
#include <TString.h>
// user includes
#include </sphenix/user/danderson/install/include/strackcutstudy/STrackCutStudy.h>

// load libraries
R__LOAD_LIBRARY(/sphenix/user/danderson/install/lib/libstrackcutstudy.so)

// global constants
static const Ssiz_t NCut = 2;
static const Ssiz_t NTxt = 3;



void DoTrackCutStudy() {

  // lower verbosity
  gErrorIgnoreLevel = kWarning;

  // i/o parameters
  const TString sOutFile("trackCutStudy.forMvtxCheck_withMvtxCut_finePtBinsWithNoIntNorm.pt020n5pim.d21m2y2023.root");
  const TString sInFileEO("input/embed_only/final_merge/sPhenixG4_forTrackCutStudy_embedOnly0t1099_g4svtxeval.pt020n5pim.d12m1y2023.root");
  const TString sInFilePU("input/test/sPhenixG4_testWithPileup001_g4svtxEval.d18m12y2022.root");
  const TString sInTupleEO("ntp_track");
  const TString sInTuplePU("ntp_gtrack");

  // study parameters
  const Bool_t   doIntNorm(false);
  const Bool_t   useOnlyPrimary(true);
  const Double_t normalPtFracMin(0.20);
  const Double_t normalPtFracMax(1.20);

  // track cuts
  const Double_t vzRange[NCut]      = {-5., 5.};
  const Double_t qualityRange[NCut] = {0.,  2.};

  // text for plot
  const TString sTxtEO[NTxt] = {"#bf{#it{sPHENIX}} Simulation", "single #pi^{-}, p_{T} #in (0, 20) GeV/c", "#bf{Embedded Only Tracks}"};
  const TString sTxtPU[NTxt] = {"#bf{#it{sPHENIX}} Simulation", "0-20 fm Hijing, 50 kHz pileup", "#bf{With Pileup Tracks}"};

  // run track cut study
  STrackCutStudy *study = new STrackCutStudy();
  study -> SetInputOutputFiles(sInFileEO, sInFilePU, sOutFile);
  study -> SetInputTuples(sInTupleEO, sInTuplePU);
  study -> SetStudyParameters(doIntNorm, useOnlyPrimary, normalPtFracMin, normalPtFracMax);
  study -> SetTrackCuts(vzRange[0], vzRange[1], qualityRange[0], qualityRange[1]);
  study -> SetPlotText(NTxt, NTxt, sTxtEO, sTxtPU);
  study -> Init();
  study -> Analyze();
  study -> End();

}  // end 'DoTrackCutStudy()'

#endif

// end ------------------------------------------------------------------------
