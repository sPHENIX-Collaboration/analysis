// 'DoTrackCutStudy.C'
// Derek Anderson
// 12.15.2022
//
// Runs the 'STrackCutStudy'
// class.

#ifndef DOTRACKCUTSTUDY_C
#define DOTRACKCUTSTUDY_C

// root includes
#include <TROOT.h>
#include <TString.h>
// user includes
#include </sphenix/u/danderson/install/include/strackcutstudy/STrackCutStudy.h>

// load libraries
R__LOAD_LIBRARY(/sphenix/u/danderson/install/lib/libstrackcutstudy.so)



void DoTrackCutStudy() {

  // lower verbosity
  gErrorIgnoreLevel = kWarning;

  // i/o parameters
  const TString sOutFile("trackCutStudy.onlyPrimariesInEmbed_nhitsNotNlayers_weirdNMapsG3.pt020n5pim.d24m1y2022.root");
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
  const Double_t qualityMin(0.);
  const Double_t qualityMax(2.);

  // run track cut study
  STrackCutStudy *study = new STrackCutStudy();
  study -> SetInputOutputFiles(sInFileEO, sInFilePU, sOutFile);
  study -> SetInputTuples(sInTupleEO, sInTuplePU);
  study -> SetStudyParameters(doIntNorm, useOnlyPrimary, normalPtFracMin, normalPtFracMax);
  study -> SetTrackCuts(qualityMin, qualityMax);
  study -> Init();
  study -> Analyze();
  study -> End();

}  // end 'DoTrackCutStudy()'

#endif

// end ------------------------------------------------------------------------
