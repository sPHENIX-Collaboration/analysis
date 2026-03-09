// ----------------------------------------------------------------------------
// 'QuickTreePlotter.C
// Derek Anderson
// 08.24.2023
//
// Use to quick plot some leaves from a TTree
// ----------------------------------------------------------------------------

#include <vector>
#include <iostream>
#include "TH1.h"
#include "TH2.h"
#include "TTree.h"
#include "TFile.h"
#include "TError.h"
#include "TString.h"
#include "TDirectory.h"

using namespace std;



void QuickTreePlotter() {

  // lower verbosity
  gErrorIgnoreLevel = kError;
  cout << "\n  Beginning quick tree plotting..." << endl;

  // i/o parameters
  const TString sOutput  = "newMatcher.allVsWeirdVsNormal.pt020n15pim.d24m8y2023.root";
  const TString sInput   = "input/merged/sPhenixG4_testingNewMatcher_newMatcher_run0.pt020n15pim.d14m8y2023.root";
  const TString sInTree = "T";

  // cuts to apply and labels
  const vector<TString> vecCutsToApply = {
    "(is_matched==1)",
    "(is_matched==1)",
    "(is_matched==1)"
  };
  const vector<TString> vecCutLabels = {
    "All",
    "Weird",
    "Normal"
  };

  // 1d things to draw and histogram names
  const vector<TString> vecToDraw1D = {
    "deltapt/pt",
    "pt/gpt",
    "gphi"
  };
  const vector<TString> vecHistNames1D = {
    "hDeltaPtOverPt",
    "hFracPt",
    "hTruthPhi"
  };

  // 2d things to draw, histogram names, and options
  const vector<TString> vecToDraw2D = {
    "deltapt/pt:pt",
    "deltapt/pt:pt/gpt",
    "gphi:pt/gpt"
  };
  const vector<TString> vecHistNames2D = {
    "hDeltaPtOverPtVsPt",
    "hDeltaPtOverPtVsFracPt",
    "hTruthPhiVsFracPt"
  };
  const vector<TString> vecHistOpts2D = {
    "colz",
    "colz",
    "colz"
  };

  // construct list of output histograms
  const Ssiz_t nToDraw1D    = vecToDraw1D.size();
  const Ssiz_t nToDraw2D    = vecToDraw2D.size();
  const Ssiz_t nCutsToApply = vecCutsToApply.size();

  TString         sHistToDraw1D;
  TString         sHistToDraw2D;
  vector<TString> vecHistToDraw1D;
  vector<TString> vecHistToDraw2D;
  for (Ssiz_t iCutToApply = 0; iCutToApply < nCutsToApply; iCutToApply++) {
    for (Ssiz_t iToDraw1D = 0; iToDraw1D < nToDraw1D; iToDraw1D++) {

      // construct 1d name
      sHistToDraw1D = vecHistNames1D[iToDraw1D].Data();
      sHistToDraw1D.Append("_");
      sHistToDraw1D.Append(vecCutLabels[iCutToApply].Data());

      // add to list
      vecHistToDraw1D.push_back(sHistToDraw1D.Data());
    } 
    for (Ssiz_t iToDraw2D = 0; iToDraw2D < nToDraw2D; iToDraw2D++) {

      // construct 2d name
      sHistToDraw2D = vecHistNames2D[iToDraw2D].Data();
      sHistToDraw2D.Append("_");
      sHistToDraw2D.Append(vecCutLabels[iCutToApply].Data());

      // add to list
      vecHistToDraw2D.push_back(sHistToDraw2D.Data());
    }
  }  // end cut loop
  cout << "    Constructed histogram lists." << endl;

  // open files
  TFile* fOutput = new TFile(sOutput.Data(), "recreate");
  TFile* fInput  = new TFile(sInput.Data(),  "read");
  if (!fOutput || !fInput) {
    cerr << "PANIC: couldn't open a file!\n"
         << "       fOutput = " << fOutput << ", fInput = " << fInput << "\n"
         << endl;
    return;
  }
  cout << "    Opened files." << endl;

  // grab input tuple
  TTree* tToDrawFrom = (TNtuple*) fInput -> Get(sInTree.Data());
  if (!tToDrawFrom) {
    cerr << "PANIC: couldn't grab input tuple!\n" << endl;
    return;
  }
  cout << "    Grabbed input tuple.\n"
       << "    Beginning draw loop..."
       << endl;

  // draw histograms
  Ssiz_t  iHistToDraw1D = 0;
  Ssiz_t  iHistToDraw2D = 0;
  TString sDrawArg1D    = "";
  TString sDrawArg2D    = "";
  for (Ssiz_t iCutToApply = 0; iCutToApply < nCutsToApply; iCutToApply++) {
    for (Ssiz_t iToDraw1D = 0; iToDraw1D < nToDraw1D; iToDraw1D++) {

      // construct draw arg
      sDrawArg1D = vecToDraw1D[iToDraw1D].Data();
      sDrawArg1D.Append(">>");
      sDrawArg1D.Append(vecHistToDraw1D[iHistToDraw1D].Data());
      cout << "      Drawing '" << sDrawArg1D.Data() << "'..." << endl;

      // draw to histogram
      tToDrawFrom -> Draw(sDrawArg1D.Data(), vecCutsToApply[iCutToApply].Data());
      ++iHistToDraw1D;
    } 
    for (Ssiz_t iToDraw2D = 0; iToDraw2D < nToDraw2D; iToDraw2D++) {

      // construct draw arg
      sDrawArg2D = vecToDraw2D[iToDraw2D].Data();
      sDrawArg2D.Append(">>");
      sDrawArg2D.Append(vecHistToDraw2D[iHistToDraw2D].Data());
      cout << "      Drawing '" << sDrawArg2D.Data() << "'..." << endl;

      // draw to histogram
      tToDrawFrom -> Draw(sDrawArg2D.Data(), vecCutsToApply[iCutToApply].Data());
      ++iHistToDraw2D;
    }
  }  // end cut loop
  cout << "    Drew histograms from tuple." << endl;

  // grab histograms and save
  const Ssiz_t nHistToDraw1D = vecHistToDraw1D.size();
  const Ssiz_t nHistToDraw2D = vecHistToDraw2D.size();

  TH1D* hHistToDraw1D[nHistToDraw1D];
  TH2D* hHistToDraw2D[nHistToDraw2D];
  for (Ssiz_t iHist1D = 0; iHist1D < nHistToDraw1D; iHist1D++) {

    // grab histogram
    fInput                 -> cd();
    hHistToDraw1D[iHist1D] = (TH1D*) gDirectory -> Get(vecHistToDraw1D[iHist1D].Data());

    // save histogram
    fOutput                -> cd();
    hHistToDraw1D[iHist1D] -> Write();
  }
  for (Ssiz_t iHist2D = 0; iHist2D < nHistToDraw2D; iHist2D++) {

    // grab histogram
    fInput                 -> cd();
    hHistToDraw2D[iHist2D] = (TH2D*) gDirectory -> Get(vecHistToDraw2D[iHist2D].Data());

    // save histogram
    fOutput                -> cd();
    hHistToDraw2D[iHist2D] -> Write();
  }
  cout << "    Saved histograms." << endl;

  // close files
  fOutput -> cd();
  fOutput -> Close();
  fInput  -> cd();
  fInput  -> Close();
  cout << "    Closed files." << endl;

  // announce end and exit
  cout << "  Finished quick tree plotting!\n" << endl;
  return;

}

// end ------------------------------------------------------------------------
