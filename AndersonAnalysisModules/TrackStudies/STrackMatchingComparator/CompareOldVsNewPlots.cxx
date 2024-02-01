// ----------------------------------------------------------------------------
// 'CompareOldVsNewPlots.cxx'
// Derek Anderson
// 08.29.2023
//
// Construct a series of plots comparing the
// output of the old evalautor vs. the new
// truth-track matcher (ie. the output of
// MakeOldEvaluatorPlots.cxx vs.
// MakeNewMatcherPlots.cxx)
// ----------------------------------------------------------------------------

#include <cmath>
#include <array>
#include <string>
#include <vector>
#include <utility>
#include <iostream>
#include "TH1.h"
#include "TH2.h"
#include "TPad.h"
#include "TFile.h"
#include "TLine.h"
#include "TError.h"
#include "TString.h"
#include "TLegend.h"
#include "TCanvas.h"
#include "TPaveText.h"
#include "TDirectory.h"

using namespace std;

// global constants
static const size_t NDir  = 4;
static const size_t NVtx  = 4;
static const size_t NSide = 4;
static const size_t NAxes = 3;



void CompareOldVsNewPlots() {

  // lower verbosity
  gErrorIgnoreLevel = kError;
  cout << "\n  Beginning old vs. new comparison script..." << endl;

  // options ------------------------------------------------------------------

  // io parameters
  const string sOutput("oldVsNewComparison_addRatios_withIntNorm_oneMatchPerParticle_odd02120.pt020n5evt500pim.d7m12y2023.root");
  const string sInOld("oldEvalPlots_oneMatchPerParticle_odd02120s.pt020n5evt500pim.d7m12y2023.root");
  const string sInNew("newMatcherPlots_oneMatchPerParticle_odd02120.pt020n5evt500pim.d7m12y2023.root");

  // style parameters
  const pair<uint32_t, uint32_t> fCol = {923, 899};
  const pair<uint32_t, uint32_t> fMar = {20,  25};
  const pair<uint32_t, uint32_t> fLin = {1,   1};
  const pair<uint32_t, uint32_t> fWid = {1,   1};
  const pair<uint32_t, uint32_t> fFil = {0,   0};

  // histogram parameters
  const string               sRatioLabel = "Ratio";
  const pair<string, string> sHistLabels = {"Old", "New"};
  const array<string, NDir>  arrDirNames = {"OldHists", "NewHists", "Ratios", "Plots"};

  // accessor for directories
  enum Dir {Old, New, Ratio, Plot};

  // text parameters
  const string         sLegOld = "evaluator";
  const string         sLegNew = "truth matcher";
  const string         sHeader = "";
  const string         sCount  = "counts";
  const string         sNorm   = "normalized counts";
  const string         sRatio  = "ratio";
  const vector<string> vecTxt  = {
    "#bf{#it{sPHENIX}} Simulation",
    "5 #pi^{-}/event, p_{T} #in (0, 20) GeV/c",
    "#bf{Only #pi^{-}}"
  };

  // plot parameters
  const bool doIntNorm       = true;
  const bool matchVertScales = true;

  // histograms to compare
  const vector<string> vecInHists1D = {
    "hTruthNumTot",
    "hTrackNumTot",
    "hWeirdNumTot",
    "hNormNumTot",
    "hTruthNumIntt",
    "hTrackNumIntt",
    "hWeirdNumIntt",
    "hNormNumIntt",
    "hTruthNumMvtx",
    "hTrackNumMvtx",
    "hWeirdNumMvtx",
    "hNormNumMvtx",
    "hTruthNumTpc",
    "hTrackNumTpc",
    "hWeirdNumTpc",
    "hNormNumTpc",
    "hTruthRatTot",
    "hTrackRatTot",
    "hWeirdRatTot",
    "hNormRatTot",
    "hTruthRatIntt",
    "hTrackRatIntt",
    "hWeirdRatIntt",
    "hNormRatIntt",
    "hTruthRatMvtx",
    "hTrackRatMvtx",
    "hWeirdRatMvtx",
    "hNormRatMvtx",
    "hTruthRatTpc",
    "hTrackRatTpc",
    "hWeirdRatTpc",
    "hNormRatTpc",
    "hTruthPhi",
    "hTrackPhi",
    "hWeirdPhi",
    "hNormPhi",
    "hTruthEta",
    "hTrackEta",
    "hWeirdEta",
    "hNormEta",
    "hTruthPt",
    "hTrackPt",
    "hWeirdPt",
    "hNormPt",
    "hTruthFrac",
    "hTrackFrac",
    "hWeirdFrac",
    "hNormFrac"
  };
  const vector<string> vecInHists2D = {
    "hTruthNumTotVsTruthPt",
    "hTruthNumTotVsNumTpc",
    "hTrackNumTotVsTruthPt",
    "hTrackNumTotVsNumTpc",
    "hWeirdNumTotVsTruthPt",
    "hWeirdNumTotVsNumTpc",
    "hNormNumTotVsTruthPt",
    "hNormNumTotVsNumTpc",
    "hTruthNumInttVsTruthPt",
    "hTruthNumInttVsNumTpc",
    "hTrackNumInttVsTruthPt",
    "hTrackNumInttVsNumTpc",
    "hWeirdNumInttVsTruthPt",
    "hWeirdNumInttVsNumTpc",
    "hNormNumInttVsTruthPt",
    "hNormNumInttVsNumTpc",
    "hTruthNumMvtxVsTruthPt",
    "hTruthNumMvtxVsNumTpc",
    "hTrackNumMvtxVsTruthPt",
    "hTrackNumMvtxVsNumTpc",
    "hWeirdNumMvtxVsTruthPt",
    "hWeirdNumMvtxVsNumTpc",
    "hNormNumMvtxVsTruthPt",
    "hNormNumMvtxVsNumTpc",
    "hTruthNumTpcVsTruthPt",
    "hTruthNumTpcVsNumTpc",
    "hTrackNumTpcVsTruthPt",
    "hTrackNumTpcVsNumTpc",
    "hWeirdNumTpcVsTruthPt",
    "hWeirdNumTpcVsNumTpc",
    "hNormNumTpcVsTruthPt",
    "hNormNumTpcVsNumTpc",
    "hTruthRatTotVsTruthPt",
    "hTruthRatTotVsNumTpc",
    "hTrackRatTotVsTruthPt",
    "hTrackRatTotVsNumTpc",
    "hWeirdRatTotVsTruthPt",
    "hWeirdRatTotVsNumTpc",
    "hNormRatTotVsTruthPt",
    "hNormRatTotVsNumTpc",
    "hTruthRatInttVsTruthPt",
    "hTruthRatInttVsNumTpc",
    "hTrackRatInttVsTruthPt",
    "hTrackRatInttVsNumTpc",
    "hWeirdRatInttVsTruthPt",
    "hWeirdRatInttVsNumTpc",
    "hNormRatInttVsTruthPt",
    "hNormRatInttVsNumTpc",
    "hTruthRatMvtxVsTruthPt",
    "hTruthRatMvtxVsNumTpc",
    "hTrackRatMvtxVsTruthPt",
    "hTrackRatMvtxVsNumTpc",
    "hWeirdRatMvtxVsTruthPt",
    "hWeirdRatMvtxVsNumTpc",
    "hNormRatMvtxVsTruthPt",
    "hNormRatMvtxVsNumTpc",
    "hTruthRatTpcVsTruthPt",
    "hTruthRatTpcVsNumTpc",
    "hTrackRatTpcVsTruthPt",
    "hTrackRatTpcVsNumTpc",
    "hWeirdRatTpcVsTruthPt",
    "hWeirdRatTpcVsNumTpc",
    "hNormRatTpcVsTruthPt",
    "hNormRatTpcVsNumTpc",
    "hTruthPhiVsTruthPt",
    "hTruthPhiVsNumTpc",
    "hTrackPhiVsTruthPt",
    "hTrackPhiVsNumTpc",
    "hWeirdPhiVsTruthPt",
    "hWeirdPhiVsNumTpc",
    "hNormPhiVsTruthPt",
    "hNormPhiVsNumTpc",
    "hTruthEtaVsTruthPt",
    "hTruthEtaVsNumTpc",
    "hTrackEtaVsTruthPt",
    "hTrackEtaVsNumTpc",
    "hWeirdEtaVsTruthPt",
    "hWeirdEtaVsNumTpc",
    "hNormEtaVsTruthPt",
    "hNormEtaVsNumTpc",
    "hTruthPtVsTruthPt",
    "hTruthPtVsNumTpc",
    "hTrackPtVsTruthPt",
    "hTrackPtVsNumTpc",
    "hWeirdPtVsTruthPt",
    "hWeirdPtVsNumTpc",
    "hNormPtVsTruthPt",
    "hNormPtVsNumTpc",
    "hTruthFracVsTruthPt",
    "hTruthFracVsNumTpc",
    "hTrackFracVsTruthPt",
    "hTrackFracVsNumTpc",
    "hWeirdFracVsTruthPt",
    "hWeirdFracVsNumTpc",
    "hNormFracVsTruthPt",
    "hNormFracVsNumTpc"
  };

  // canvas names
  vector<string> vecCanvasNames1D = {
    "cTruthNumTot",
    "cTrackNumTot",
    "cWeirdNumTot",
    "cNormNumTot",
    "cTruthNumIntt",
    "cTrackNumIntt",
    "cWeirdNumIntt",
    "cNormNumIntt",
    "cTruthNumMvtx",
    "cTrackNumMvtx",
    "cWeirdNumMvtx",
    "cNormNumMvtx",
    "cTruthNumTpc",
    "cTrackNumTpc",
    "cWeirdNumTpc",
    "cNormNumTpc",
    "cTruthRatTot",
    "cTrackRatTot",
    "cWeirdRatTot",
    "cNormRatTot",
    "cTruthRatIntt",
    "cTrackRatIntt",
    "cWeirdRatIntt",
    "cNormRatIntt",
    "cTruthRatMvtx",
    "cTrackRatMvtx",
    "cWeirdRatMvtx",
    "cNormRatMvtx",
    "cTruthRatTpc",
    "cTrackRatTpc",
    "cWeirdRatTpc",
    "cNormRatTpc",
    "cTruthPhi",
    "cTrackPhi",
    "cWeirdPhi",
    "cNormPhi",
    "cTruthEta",
    "cTrackEta",
    "cWeirdEta",
    "cNormEta",
    "cTruthPt",
    "cTrackPt",
    "cWeirdPt",
    "cNormPt",
    "cTruthFrac",
    "cTrackFrac",
    "cWeirdFrac",
    "cNormFrac"
  };
  vector<string> vecCanvasNames2D = {
    "cTruthNumTotVsTruthPt",
    "cTruthNumTotVsNumTpc",
    "cTrackNumTotVsTruthPt",
    "cTrackNumTotVsNumTpc",
    "cWeirdNumTotVsTruthPt",
    "cWeirdNumTotVsNumTpc",
    "cNormNumTotVsTruthPt",
    "cNormNumTotVsNumTpc",
    "cTruthNumInttVsTruthPt",
    "cTruthNumInttVsNumTpc",
    "cTrackNumInttVsTruthPt",
    "cTrackNumInttVsNumTpc",
    "cWeirdNumInttVsTruthPt",
    "cWeirdNumInttVsNumTpc",
    "cNormNumInttVsTruthPt",
    "cNormNumInttVsNumTpc",
    "cTruthNumMvtxVsTruthPt",
    "cTruthNumMvtxVsNumTpc",
    "cTrackNumMvtxVsTruthPt",
    "cTrackNumMvtxVsNumTpc",
    "cWeirdNumMvtxVsTruthPt",
    "cWeirdNumMvtxVsNumTpc",
    "cNormNumMvtxVsTruthPt",
    "cNormNumMvtxVsNumTpc",
    "cTruthNumTpcVsTruthPt",
    "cTruthNumTpcVsNumTpc",
    "cTrackNumTpcVsTruthPt",
    "cTrackNumTpcVsNumTpc",
    "cWeirdNumTpcVsTruthPt",
    "cWeirdNumTpcVsNumTpc",
    "cNormNumTpcVsTruthPt",
    "cNormNumTpcVsNumTpc",
    "cTruthRatTotVsTruthPt",
    "cTruthRatTotVsNumTpc",
    "cTrackRatTotVsTruthPt",
    "cTrackRatTotVsNumTpc",
    "cWeirdRatTotVsTruthPt",
    "cWeirdRatTotVsNumTpc",
    "cNormRatTotVsTruthPt",
    "cNormRatTotVsNumTpc",
    "cTruthRatInttVsTruthPt",
    "cTruthRatInttVsNumTpc",
    "cTrackRatInttVsTruthPt",
    "cTrackRatInttVsNumTpc",
    "cWeirdRatInttVsTruthPt",
    "cWeirdRatInttVsNumTpc",
    "cNormRatInttVsTruthPt",
    "cNormRatInttVsNumTpc",
    "cTruthRatMvtxVsTruthPt",
    "cTruthRatMvtxVsNumTpc",
    "cTrackRatMvtxVsTruthPt",
    "cTrackRatMvtxVsNumTpc",
    "cWeirdRatMvtxVsTruthPt",
    "cWeirdRatMvtxVsNumTpc",
    "cNormRatMvtxVsTruthPt",
    "cNormRatMvtxVsNumTpc",
    "cTruthRatTpcVsTruthPt",
    "cTruthRatTpcVsNumTpc",
    "cTrackRatTpcVsTruthPt",
    "cTrackRatTpcVsNumTpc",
    "cWeirdRatTpcVsTruthPt",
    "cWeirdRatTpcVsNumTpc",
    "cNormRatTpcVsTruthPt",
    "cNormRatTpcVsNumTpc",
    "cTruthPhiVsTruthPt",
    "cTruthPhiVsNumTpc",
    "cTrackPhiVsTruthPt",
    "cTrackPhiVsNumTpc",
    "cWeirdPhiVsTruthPt",
    "cWeirdPhiVsNumTpc",
    "cNormPhiVsTruthPt",
    "cNormPhiVsNumTpc",
    "cTruthEtaVsTruthPt",
    "cTruthEtaVsNumTpc",
    "cTrackEtaVsTruthPt",
    "cTrackEtaVsNumTpc",
    "cWeirdEtaVsTruthPt",
    "cWeirdEtaVsNumTpc",
    "cNormEtaVsTruthPt",
    "cNormEtaVsNumTpc",
    "cTruthPtVsTruthPt",
    "cTruthPtVsNumTpc",
    "cTrackPtVsTruthPt",
    "cTrackPtVsNumTpc",
    "cWeirdPtVsTruthPt",
    "cWeirdPtVsNumTpc",
    "cNormPtVsTruthPt",
    "cNormPtVsNumTpc",
    "cTruthFracVsTruthPt",
    "cTruthFracVsNumTpc",
    "cTrackFracVsTruthPt",
    "cTrackFracVsNumTpc",
    "cWeirdFracVsTruthPt",
    "cWeirdFracVsNumTpc",
    "cNormFracVsTruthPt",
    "cNormFracVsNumTpc"
  };

  // open files and load histograms -------------------------------------------

  // open files
  TFile* fOutput = new TFile(sOutput.data(), "recreate");
  TFile* fInOld  = new TFile(sInOld.data(),  "read");
  TFile* fInNew  = new TFile(sInNew.data(),  "read");
  if (!fOutput || !fInOld || !fInNew) {
     cerr << "PANIC: couldn't open a file!\n"
          << "       fOutput = " << fOutput << "fInOld = " << fInOld << ", fInNew = " << fInNew << "\n"
          << endl;
    return;
  }
  cout << "    Opened files." << endl;

  // grab histograms
  vector<TH1D*> vecOldHists1D;
  vector<TH1D*> vecNewHists1D;
  vector<TH2D*> vecOldHists2D;
  vector<TH2D*> vecNewHists2D;
  for (const string sInHist1D : vecInHists1D) {
    vecOldHists1D.push_back((TH1D*) fInOld -> Get(sInHist1D.data()));
    vecNewHists1D.push_back((TH1D*) fInNew -> Get(sInHist1D.data()));
    if (!vecOldHists1D.back() || !vecNewHists1D.back()) {
      cerr << "PANIC: couldn't grab the 1D histogram '" << sInHist1D << "' from an input file!\n"
           << "       hInOld1D = " << vecOldHists1D.back() << ", hInNew1D = " << vecNewHists1D.back() << "\n"
           << endl;
      return;
    }
  }
  for (const string sInHist2D : vecInHists2D) {
    vecOldHists2D.push_back((TH2D*) fInOld -> Get(sInHist2D.data()));
    vecNewHists2D.push_back((TH2D*) fInNew -> Get(sInHist2D.data()));
    if (!vecOldHists2D.back() || !vecNewHists2D.back()) {
      cerr << "PANIC: couldn't grab the 2D histogram '" << sInHist2D << "' from an input file!\n"
           << "       hInOld2D = " << vecOldHists2D.back() << ", hInNew2D = " << vecNewHists2D.back() << "\n"
           << endl;
      return;
    }
  }
  cout << "    Grabbed histograms." << endl;

  // normalize histograms and set axes' ranges as needed ----------------------

  // normalize by integral if needed
  if (doIntNorm) {
    for (TH1D* hOldHist1D : vecOldHists1D) {
      const double intOld1D = hOldHist1D -> Integral();
      if (intOld1D > 0.) {
        hOldHist1D -> Scale(1. / intOld1D);
      }
    }
    for (TH1D* hNewHist1D : vecNewHists1D) {
      const double intNew1D = hNewHist1D -> Integral();
      if (intNew1D > 0.) {
        hNewHist1D -> Scale(1. / intNew1D);
      }
    }
    for (TH2D* hOldHist2D : vecOldHists2D) {
      const double intOld2D = hOldHist2D -> Integral();
      if (intOld2D > 0.) {
        hOldHist2D -> Scale(1. / intOld2D);
      }
    }
    for (TH2D* hNewHist2D : vecNewHists2D) {
      const double intNew2D = hNewHist2D -> Integral();
      if (intNew2D > 0.) {
        hNewHist2D -> Scale(1. / intNew2D);
      }
    }
    cout << "    Normalized histograms." << endl;
  }

  // set z-axis ranges if needed
  if (matchVertScales) {
    size_t nHist2D = vecOldHists2D.size();
    for (size_t iHist2D = 0; iHist2D < nHist2D; iHist2D++) {
      const float oldMin = vecOldHists2D[iHist2D] -> GetMinimum(0.);
      const float oldMax = vecOldHists2D[iHist2D] -> GetMaximum();
      const float newMin = vecNewHists2D[iHist2D] -> GetMinimum(0.);
      const float newMax = vecNewHists2D[iHist2D] -> GetMaximum();
      const float setMin = min(oldMin, newMin);
      const float setMax = max(oldMax, newMax);
      vecOldHists2D[iHist2D] -> GetZaxis() -> SetRangeUser(setMin, setMax);
      vecNewHists2D[iHist2D] -> GetZaxis() -> SetRangeUser(setMin, setMax);
    }
    cout << "    Adjusted z-axis scales to match." << endl;
  }

  // pick relevant count
  string sCountUse("");
  if (doIntNorm) {
    sCountUse = sNorm;
  } else {
    sCountUse = sCount;
  }

  // calculate ratios ---------------------------------------------------------

  vector<TH1D*> vecRatios1D(vecOldHists1D.size());
  vector<TH2D*> vecRatios2D(vecOldHists2D.size());
  for (size_t iHist1D = 0; iHist1D < vecOldHists1D.size(); iHist1D++) {

    // make histogram name
    TString sRatioName = vecOldHists1D.at(iHist1D) -> GetName();
    sRatioName.Append("_");
    sRatioName.Append(sRatioLabel.data());

    vecRatios1D.at(iHist1D) = (TH1D*) vecOldHists1D.at(iHist1D) -> Clone();
    vecRatios1D.at(iHist1D) -> SetName(sRatioName.Data());
    vecRatios1D.at(iHist1D) -> Reset("ICES");
    vecRatios1D.at(iHist1D) -> Divide(vecOldHists1D.at(iHist1D), vecNewHists1D.at(iHist1D), 1., 1.);
  }
  for (size_t iHist2D = 0; iHist2D < vecOldHists2D.size(); iHist2D++) {

    // make histogram name
    TString sRatioName = vecOldHists2D.at(iHist2D) -> GetName();
    sRatioName.Append("_");
    sRatioName.Append(sRatioLabel.data());

    vecRatios2D.at(iHist2D) = (TH2D*) vecOldHists2D.at(iHist2D) -> Clone();
    vecRatios2D.at(iHist2D) -> SetName(sRatioName.Data());
    vecRatios2D.at(iHist2D) -> Reset("ICES");
    vecRatios2D.at(iHist2D) -> Divide(vecOldHists2D.at(iHist2D), vecNewHists2D.at(iHist2D), 1., 1.);
  }
  cout << "    Calculated ratios." << endl;

  // set histogram styles -----------------------------------------------------

  // style options
  const uint32_t fTxt(42);
  const uint32_t fAln(12);
  const uint32_t fCnt(1);
  const float    fLabH[NAxes]   = {0.04,  0.04,  0.03};
  const float    fLabR1[NAxes]  = {0.074, 0.074, 0.056};
  const float    fLabR2[NAxes]  = {0.04,  0.04,  0.03};
  const float    fTitH[NAxes]   = {0.04,  0.04,  0.04};
  const float    fTitR1[NAxes]  = {0.074, 0.074, 0.056};
  const float    fTitR2[NAxes]  = {0.04,  0.04,  0.04};
  const float    fOffTH[NAxes]  = {1.0,   1.3,   1.2};
  const float    fOffTR1[NAxes] = {0.8,   0.8,   1.0};
  const float    fOffTR2[NAxes] = {1.0,   1.3,   1.2};
  const float    fOffLH[NAxes]  = {0.005, 0.005, 0.000};
  const float    fOffLR1[NAxes] = {0.005, 0.005, 0.000};
  const float    fOffLR2[NAxes] = {0.005, 0.005, 0.000};

  // set old histogram styles
  for (TH1D* hOldHist1D : vecOldHists1D) {

    // label histogram accordingly
    TString sOldName = hOldHist1D -> GetName();
    sOldName.Append("_");
    sOldName.Append(sHistLabels.first.data());

    // set style
    hOldHist1D -> SetName(sOldName.Data());
    hOldHist1D -> SetMarkerColor(fCol.first);
    hOldHist1D -> SetMarkerStyle(fMar.first);
    hOldHist1D -> SetFillColor(fCol.first);
    hOldHist1D -> SetFillStyle(fFil.first);
    hOldHist1D -> SetLineColor(fCol.first);
    hOldHist1D -> SetLineStyle(fLin.first);
    hOldHist1D -> SetLineWidth(fWid.first);
    hOldHist1D -> SetTitle("");
    hOldHist1D -> SetTitleFont(fTxt);
    hOldHist1D -> GetXaxis() -> SetTitleFont(fTxt);
    hOldHist1D -> GetXaxis() -> SetTitleSize(fTitH[0]);
    hOldHist1D -> GetXaxis() -> SetTitleOffset(fOffTH[0]);
    hOldHist1D -> GetXaxis() -> SetLabelFont(fTxt);
    hOldHist1D -> GetXaxis() -> SetLabelSize(fLabH[0]);
    hOldHist1D -> GetXaxis() -> SetLabelOffset(fOffLH[0]);
    hOldHist1D -> GetXaxis() -> CenterTitle(fCnt);
    hOldHist1D -> GetYaxis() -> SetTitle(sCountUse.data());
    hOldHist1D -> GetYaxis() -> SetTitleFont(fTxt);
    hOldHist1D -> GetYaxis() -> SetTitleSize(fTitH[1]);
    hOldHist1D -> GetYaxis() -> SetTitleOffset(fOffTH[1]);
    hOldHist1D -> GetYaxis() -> SetLabelFont(fTxt);
    hOldHist1D -> GetYaxis() -> SetLabelSize(fLabH[1]);
    hOldHist1D -> GetYaxis() -> SetLabelOffset(fOffLH[1]);
    hOldHist1D -> GetYaxis() -> CenterTitle(fCnt);
  }
  for (TH2D* hOldHist2D : vecOldHists2D) {

    // label histogram accordingly
    TString sOldName = hOldHist2D -> GetName();
    sOldName.Append("_");
    sOldName.Append(sHistLabels.first.data());

    // set style
    hOldHist2D -> SetName(sOldName.Data());
    hOldHist2D -> SetMarkerColor(fCol.first);
    hOldHist2D -> SetMarkerStyle(fMar.first);
    hOldHist2D -> SetFillColor(fCol.first);
    hOldHist2D -> SetFillStyle(fFil.first);
    hOldHist2D -> SetLineColor(fCol.first);
    hOldHist2D -> SetLineStyle(fLin.first);
    hOldHist2D -> SetLineWidth(fWid.first);
    hOldHist2D -> SetTitle(sLegOld.data());
    hOldHist2D -> SetTitleFont(fTxt);
    hOldHist2D -> GetXaxis() -> SetTitleFont(fTxt);
    hOldHist2D -> GetXaxis() -> SetTitleSize(fTitH[0]);
    hOldHist2D -> GetXaxis() -> SetTitleOffset(fOffTH[0]);
    hOldHist2D -> GetXaxis() -> SetLabelFont(fTxt);
    hOldHist2D -> GetXaxis() -> SetLabelSize(fLabH[0]);
    hOldHist2D -> GetXaxis() -> SetLabelOffset(fOffLH[0]);
    hOldHist2D -> GetXaxis() -> CenterTitle(fCnt);
    hOldHist2D -> GetYaxis() -> SetTitleFont(fTxt);
    hOldHist2D -> GetYaxis() -> SetTitleSize(fTitH[1]);
    hOldHist2D -> GetYaxis() -> SetTitleOffset(fOffTH[1]);
    hOldHist2D -> GetYaxis() -> SetLabelFont(fTxt);
    hOldHist2D -> GetYaxis() -> SetLabelSize(fLabH[1]);
    hOldHist2D -> GetYaxis() -> SetLabelOffset(fOffLH[1]);
    hOldHist2D -> GetYaxis() -> CenterTitle(fCnt);
    hOldHist2D -> GetZaxis() -> SetTitle(sCountUse.data());
    hOldHist2D -> GetZaxis() -> SetTitleFont(fTxt);
    hOldHist2D -> GetZaxis() -> SetTitleSize(fTitH[2]);
    hOldHist2D -> GetZaxis() -> SetTitleOffset(fOffTH[2]);
    hOldHist2D -> GetZaxis() -> SetLabelFont(fTxt);
    hOldHist2D -> GetZaxis() -> SetLabelSize(fLabH[2]);
    hOldHist2D -> GetZaxis() -> SetLabelOffset(fOffLH[2]);
    hOldHist2D -> GetZaxis() -> CenterTitle(fCnt);
  }

  // set new histogram styles
  for (TH1D* hNewHist1D : vecNewHists1D) {

    // label histogram accordingly
    TString sNewName = hNewHist1D -> GetName();
    sNewName.Append("_");
    sNewName.Append(sHistLabels.second.data());

    // set style
    hNewHist1D -> SetName(sNewName.Data());
    hNewHist1D -> SetMarkerColor(fCol.second);
    hNewHist1D -> SetMarkerStyle(fMar.second);
    hNewHist1D -> SetFillColor(fCol.second);
    hNewHist1D -> SetFillStyle(fFil.second);
    hNewHist1D -> SetLineColor(fCol.second);
    hNewHist1D -> SetLineStyle(fLin.second);
    hNewHist1D -> SetLineWidth(fWid.second);
    hNewHist1D -> SetTitle("");
    hNewHist1D -> SetTitleFont(fTxt);
    hNewHist1D -> GetXaxis() -> SetTitleFont(fTxt);
    hNewHist1D -> GetXaxis() -> SetTitleSize(fTitH[0]);
    hNewHist1D -> GetXaxis() -> SetTitleOffset(fOffTH[0]);
    hNewHist1D -> GetXaxis() -> SetLabelFont(fTxt);
    hNewHist1D -> GetXaxis() -> SetLabelSize(fLabH[0]);
    hNewHist1D -> GetXaxis() -> SetLabelOffset(fOffLH[0]);
    hNewHist1D -> GetXaxis() -> CenterTitle(fCnt);
    hNewHist1D -> GetYaxis() -> SetTitle(sCountUse.data());
    hNewHist1D -> GetYaxis() -> SetTitleFont(fTxt);
    hNewHist1D -> GetYaxis() -> SetTitleSize(fTitH[1]);
    hNewHist1D -> GetYaxis() -> SetTitleOffset(fOffTH[1]);
    hNewHist1D -> GetYaxis() -> SetLabelFont(fTxt);
    hNewHist1D -> GetYaxis() -> SetLabelSize(fLabH[1]);
    hNewHist1D -> GetYaxis() -> SetLabelOffset(fOffLH[1]);
    hNewHist1D -> GetYaxis() -> CenterTitle(fCnt);
  }
  for (TH2D* hNewHist2D : vecNewHists2D) {

    // label histogram accordingly
    TString sNewName = hNewHist2D -> GetName();
    sNewName.Append("_");
    sNewName.Append(sHistLabels.second.data());

    // set style
    hNewHist2D -> SetName(sNewName.Data());
    hNewHist2D -> SetMarkerColor(fCol.first);
    hNewHist2D -> SetMarkerStyle(fMar.first);
    hNewHist2D -> SetFillColor(fCol.first);
    hNewHist2D -> SetFillStyle(fFil.first);
    hNewHist2D -> SetLineColor(fCol.first);
    hNewHist2D -> SetLineStyle(fLin.first);
    hNewHist2D -> SetLineWidth(fWid.first);
    hNewHist2D -> SetTitle(sLegNew.data());
    hNewHist2D -> SetTitleFont(fTxt);
    hNewHist2D -> GetXaxis() -> SetTitleFont(fTxt);
    hNewHist2D -> GetXaxis() -> SetTitleSize(fTitH[0]);
    hNewHist2D -> GetXaxis() -> SetTitleOffset(fOffTH[0]);
    hNewHist2D -> GetXaxis() -> SetLabelFont(fTxt);
    hNewHist2D -> GetXaxis() -> SetLabelSize(fLabH[0]);
    hNewHist2D -> GetXaxis() -> SetLabelOffset(fOffLH[0]);
    hNewHist2D -> GetXaxis() -> CenterTitle(fCnt);
    hNewHist2D -> GetYaxis() -> SetTitleFont(fTxt);
    hNewHist2D -> GetYaxis() -> SetTitleSize(fTitH[1]);
    hNewHist2D -> GetYaxis() -> SetTitleOffset(fOffTH[1]);
    hNewHist2D -> GetYaxis() -> SetLabelFont(fTxt);
    hNewHist2D -> GetYaxis() -> SetLabelSize(fLabH[1]);
    hNewHist2D -> GetYaxis() -> SetLabelOffset(fOffLH[1]);
    hNewHist2D -> GetYaxis() -> CenterTitle(fCnt);
    hNewHist2D -> GetZaxis() -> SetTitle(sCountUse.data());
    hNewHist2D -> GetZaxis() -> SetTitleFont(fTxt);
    hNewHist2D -> GetZaxis() -> SetTitleSize(fTitH[2]);
    hNewHist2D -> GetZaxis() -> SetTitleOffset(fOffTH[2]);
    hNewHist2D -> GetZaxis() -> SetLabelFont(fTxt);
    hNewHist2D -> GetZaxis() -> SetLabelSize(fLabH[2]);
    hNewHist2D -> GetZaxis() -> SetLabelOffset(fOffLH[2]);
    hNewHist2D -> GetZaxis() -> CenterTitle(fCnt);
  }


  // set ratio styles
  for (TH1D* hRatio1D : vecRatios1D) {
    hRatio1D -> SetMarkerColor(fCol.first);
    hRatio1D -> SetMarkerStyle(fMar.first);
    hRatio1D -> SetFillColor(fCol.first);
    hRatio1D -> SetFillStyle(fFil.first);
    hRatio1D -> SetLineColor(fCol.first);
    hRatio1D -> SetLineStyle(fLin.first);
    hRatio1D -> SetLineWidth(fWid.first);
    hRatio1D -> SetTitle("");
    hRatio1D -> SetTitleFont(fTxt);
    hRatio1D -> GetXaxis() -> SetTitleFont(fTxt);
    hRatio1D -> GetXaxis() -> SetTitleSize(fTitR1[0]);
    hRatio1D -> GetXaxis() -> SetTitleOffset(fOffTR1[0]);
    hRatio1D -> GetXaxis() -> SetLabelFont(fTxt);
    hRatio1D -> GetXaxis() -> SetLabelSize(fLabR1[0]);
    hRatio1D -> GetXaxis() -> SetLabelOffset(fOffLR1[0]);
    hRatio1D -> GetXaxis() -> CenterTitle(fCnt);
    hRatio1D -> GetYaxis() -> SetTitle(sRatio.data());
    hRatio1D -> GetYaxis() -> SetTitleFont(fTxt);
    hRatio1D -> GetYaxis() -> SetTitleSize(fTitR1[1]);
    hRatio1D -> GetYaxis() -> SetTitleOffset(fOffTR1[1]);
    hRatio1D -> GetYaxis() -> SetLabelFont(fTxt);
    hRatio1D -> GetYaxis() -> SetLabelSize(fLabR1[1]);
    hRatio1D -> GetYaxis() -> SetLabelOffset(fOffLR1[1]);
    hRatio1D -> GetYaxis() -> CenterTitle(fCnt);
  }
  for (TH2D* hRatio2D : vecRatios2D) {
    hRatio2D -> SetMarkerColor(fCol.first);
    hRatio2D -> SetMarkerStyle(fMar.first);
    hRatio2D -> SetFillColor(fCol.first);
    hRatio2D -> SetFillStyle(fFil.first);
    hRatio2D -> SetLineColor(fCol.first);
    hRatio2D -> SetLineStyle(fLin.first);
    hRatio2D -> SetLineWidth(fWid.first);
    hRatio2D -> SetTitle("");
    hRatio2D -> SetTitleFont(fTxt);
    hRatio2D -> GetXaxis() -> SetTitleFont(fTxt);
    hRatio2D -> GetXaxis() -> SetTitleSize(fTitR2[0]);
    hRatio2D -> GetXaxis() -> SetTitleOffset(fOffTR2[0]);
    hRatio2D -> GetXaxis() -> SetLabelFont(fTxt);
    hRatio2D -> GetXaxis() -> SetLabelSize(fLabR2[0]);
    hRatio2D -> GetXaxis() -> SetLabelOffset(fOffLR2[0]);
    hRatio2D -> GetXaxis() -> CenterTitle(fCnt);
    hRatio2D -> GetYaxis() -> SetTitleFont(fTxt);
    hRatio2D -> GetYaxis() -> SetTitleSize(fTitR2[1]);
    hRatio2D -> GetYaxis() -> SetTitleOffset(fOffTR2[1]);
    hRatio2D -> GetYaxis() -> SetLabelFont(fTxt);
    hRatio2D -> GetYaxis() -> SetLabelSize(fLabR2[1]);
    hRatio2D -> GetYaxis() -> SetLabelOffset(fOffLR2[1]);
    hRatio2D -> GetYaxis() -> CenterTitle(fCnt);
    hRatio2D -> GetZaxis() -> SetTitle(sRatio.data());
    hRatio2D -> GetZaxis() -> SetTitleFont(fTxt);
    hRatio2D -> GetZaxis() -> SetTitleSize(fTitR2[2]);
    hRatio2D -> GetZaxis() -> SetTitleOffset(fOffTR2[2]);
    hRatio2D -> GetZaxis() -> SetLabelFont(fTxt);
    hRatio2D -> GetZaxis() -> SetLabelSize(fLabR2[2]);
    hRatio2D -> GetZaxis() -> SetLabelOffset(fOffLR2[2]);
    hRatio2D -> GetZaxis() -> CenterTitle(fCnt);
  }
  cout << "    Set styles." << endl;

  // make legends and text boxes ----------------------------------------------

  // make legend
  const uint32_t fColLe(0);
  const uint32_t fFilLe(0);
  const uint32_t fLinLe(0);
  const float    fLegXY[NVtx] = {0.1, 0.1, 0.3, 0.2};

  TLegend *leg = new TLegend(fLegXY[0], fLegXY[1], fLegXY[2], fLegXY[3], sHeader.data());
  leg -> SetFillColor(fColLe);
  leg -> SetFillStyle(fFilLe);
  leg -> SetLineColor(fColLe);
  leg -> SetLineStyle(fLinLe);
  leg -> SetTextFont(fTxt);
  leg -> SetTextAlign(fAln);
  leg -> AddEntry(vecOldHists1D.front(), sLegOld.data(), "pf");
  leg -> AddEntry(vecNewHists1D.front(), sLegNew.data(), "pf");
  cout << "    Made legend." << endl;

  // make text
  const uint32_t fColTx(0);
  const uint32_t fFilTx(0);
  const uint32_t fLinTx(0);
  const float    fTxtXY[NVtx] = {0.3, 0.1, 0.5, 0.25};

  TPaveText *txt = new TPaveText(fTxtXY[0], fTxtXY[1], fTxtXY[2], fTxtXY[3], "NDC NB");
  txt -> SetFillColor(fColTx);
  txt -> SetFillStyle(fFilTx);
  txt -> SetLineColor(fColTx);
  txt -> SetLineStyle(fLinTx);
  txt -> SetTextFont(fTxt);
  txt -> SetTextAlign(fAln);
  for (const string txtLine : vecTxt) {
    txt -> AddText(txtLine.data());
  }
  cout << "    Made text." << endl;

  // make plots ---------------------------------------------------------------

  // create output directories
  array<TDirectory*, NDir> arrOutDirs;
  for (size_t iDir = 0; iDir < NDir; iDir++) {
    arrOutDirs[iDir] = fOutput -> mkdir(arrDirNames[iDir].data());
  }
  cout << "    Made directories." << endl;

  // canvas parameters
  const uint32_t width1D(750);
  const uint32_t width2D(1500);
  const uint32_t width2DR(2250);
  const uint32_t height(750);
  const uint32_t heightR1(900);
  const uint32_t heightR2(500);
  const uint32_t fMode(0);
  const uint32_t fBord(2);
  const uint32_t fGrid(0);
  const uint32_t fTick(1);
  const uint32_t fLogX(0);
  const uint32_t fLogY(1);
  const uint32_t fLogZ(1);
  const uint32_t fFrame(0);
  const string   sOldPadName("pOld");
  const string   sNewPadName("pNew");
  const string   sHistPadName("pHist");
  const string   sRatPadName("pRatio");
  const float    fMargin1D[NSide]  = {0.02,  0.02, 0.15,  0.15};
  const float    fMargin1DH[NSide] = {0.02,  0.02, 0.005, 0.15};
  const float    fMargin1DR[NSide] = {0.005, 0.02, 0.2,   0.15};
  const float    fMargin2D[NSide]  = {0.10, 0.15, 0.15, 0.15};
  const float    xyOldPad[NVtx]    = {0.0,  0.0,  0.5,  1.0};
  const float    xyOldPadR[NVtx]   = {0.0,  0.0,  0.33, 1.0};
  const float    xyNewPad[NVtx]    = {0.5,  0.0,  1.0,  1.0};  
  const float    xyNewPadR[NVtx]   = {0.33, 0.0,  0.66, 1.0};
  const float    xyHistPadR[NVtx]  = {0.0,  0.33, 1.0,  1.0};
  const float    xyRatPadR1[NVtx]  = {0.0,  0.0,  1.0,  0.33};
  const float    xyRatPadR2[NVtx]  = {0.66, 0.0,  1.0,  1.0};

  // make 1D plots
  size_t iDraw1D = 0;
  for (const string sCanvasName : vecCanvasNames1D) {

    // construct canvas
    TCanvas* cPlot1D = new TCanvas(sCanvasName.data(), "", width1D, height);
    cPlot1D -> SetGrid(fGrid, fGrid);
    cPlot1D -> SetTicks(fTick, fTick);
    cPlot1D -> SetBorderMode(fMode);
    cPlot1D -> SetBorderSize(fBord);
    cPlot1D -> SetFrameBorderMode(fFrame);
    cPlot1D -> SetTopMargin(fMargin1D[0]);
    cPlot1D -> SetRightMargin(fMargin1D[1]);
    cPlot1D -> SetBottomMargin(fMargin1D[2]);
    cPlot1D -> SetLeftMargin(fMargin1D[3]);
    cPlot1D -> SetLogx(fLogX);
    cPlot1D -> SetLogy(fLogY);
    cPlot1D -> cd();

    // draw old vs. new histograms
    vecOldHists1D.at(iDraw1D) -> Draw();
    vecNewHists1D.at(iDraw1D) -> Draw("same");

    // draw text and save
    leg                   -> Draw();
    txt                   -> Draw();
    arrOutDirs[Dir::Plot] -> cd();
    cPlot1D               -> Write();
    cPlot1D               -> Close();

    // increment counter
    ++iDraw1D;
  }

  iDraw1D = 0;
  for (const string sCanvasName : vecCanvasNames1D) {

    // make new name
    const string sNameWithRatio = sCanvasName + "_" + sRatioLabel;

    // construct canvas
    TCanvas* cPlot1D = new TCanvas(sNameWithRatio.data(), "", width1D, heightR1);
    TPad*    pPadH1D = new TPad(sHistPadName.data(), "", xyHistPadR[0], xyHistPadR[1], xyHistPadR[2], xyHistPadR[3]);
    TPad*    pPadR1D = new TPad(sRatPadName.data(),  "", xyRatPadR1[0], xyRatPadR1[1], xyRatPadR1[2], xyRatPadR1[3]);
    cPlot1D -> SetGrid(fGrid, fGrid);
    cPlot1D -> SetTicks(fTick, fTick);
    cPlot1D -> SetBorderMode(fMode);
    cPlot1D -> SetBorderSize(fBord);
    pPadH1D -> SetGrid(fGrid, fGrid);
    pPadH1D -> SetTicks(fTick, fTick);
    pPadH1D -> SetBorderMode(fMode);
    pPadH1D -> SetBorderSize(fBord);
    pPadH1D -> SetFrameBorderMode(fFrame);
    pPadH1D -> SetTopMargin(fMargin1DH[0]);
    pPadH1D -> SetRightMargin(fMargin1DH[1]);
    pPadH1D -> SetBottomMargin(fMargin1DH[2]);
    pPadH1D -> SetLeftMargin(fMargin1DH[3]);
    pPadH1D -> SetLogx(fLogX);
    pPadH1D -> SetLogy(fLogY);
    pPadR1D -> SetGrid(fGrid, fGrid);
    pPadR1D -> SetTicks(fTick, fTick);
    pPadR1D -> SetBorderMode(fMode);
    pPadR1D -> SetBorderSize(fBord);
    pPadR1D -> SetFrameBorderMode(fFrame);
    pPadR1D -> SetTopMargin(fMargin1DR[0]);
    pPadR1D -> SetRightMargin(fMargin1DR[1]);
    pPadR1D -> SetBottomMargin(fMargin1DR[2]);
    pPadR1D -> SetLeftMargin(fMargin1DR[3]);
    pPadR1D -> SetLogx(fLogX);
    pPadR1D -> SetLogy(fLogY);
    cPlot1D -> cd();
    pPadH1D -> Draw();
    pPadR1D -> Draw();

    // draw old vs. new histograms
    pPadH1D                   -> cd();
    vecOldHists1D.at(iDraw1D) -> Draw();
    vecNewHists1D.at(iDraw1D) -> Draw("same");
    leg                       -> Draw();
    txt                       -> Draw();

    // draw ratio
    pPadR1D                 -> cd();
    vecRatios1D.at(iDraw1D) -> Draw();

    // draw text and save
    arrOutDirs[Dir::Plot] -> cd();
    cPlot1D               -> Write();
    cPlot1D               -> Close();

    // increment counter
    ++iDraw1D;
  }

  // make 2D plots
  size_t iDraw2D = 0;
  for (const string sCanvasName : vecCanvasNames2D) {

    // construct canvas
    TCanvas* cPlot2D = new TCanvas(sCanvasName.data(), "", width2D, height);
    TPad*    pOld    = new TPad(sOldPadName.data(), "", xyOldPad[0], xyOldPad[1], xyOldPad[2], xyOldPad[3]);
    TPad*    pNew    = new TPad(sNewPadName.data(), "", xyNewPad[0], xyNewPad[1], xyNewPad[2], xyNewPad[3]);
    cPlot2D -> SetGrid(fGrid, fGrid);
    cPlot2D -> SetTicks(fTick, fTick);
    cPlot2D -> SetBorderMode(fMode);
    cPlot2D -> SetBorderSize(fBord);
    pOld    -> SetGrid(fGrid, fGrid);
    pOld    -> SetTicks(fTick, fTick);
    pOld    -> SetBorderMode(fMode);
    pOld    -> SetBorderSize(fBord);
    pOld    -> SetFrameBorderMode(fFrame);
    pOld    -> SetTopMargin(fMargin2D[0]);
    pOld    -> SetRightMargin(fMargin2D[1]);
    pOld    -> SetBottomMargin(fMargin2D[2]);
    pOld    -> SetLeftMargin(fMargin2D[3]);
    pOld    -> SetLogx(fLogX);
    pOld    -> SetLogy(fLogY);
    pOld    -> SetLogz(fLogZ);
    pNew    -> SetGrid(fGrid, fGrid);
    pNew    -> SetTicks(fTick, fTick);
    pNew    -> SetBorderMode(fMode);
    pNew    -> SetBorderSize(fBord);
    pNew    -> SetFrameBorderMode(fFrame);
    pNew    -> SetTopMargin(fMargin2D[0]);
    pNew    -> SetRightMargin(fMargin2D[1]);
    pNew    -> SetBottomMargin(fMargin2D[2]);
    pNew    -> SetLeftMargin(fMargin2D[3]);
    pNew    -> SetLogx(fLogX);
    pNew    -> SetLogy(fLogY);
    pNew    -> SetLogz(fLogZ);
    cPlot2D -> cd();
    pOld    -> Draw();
    pNew    -> Draw();

    // draw old vs. new histograms
    pOld                      -> cd();
    vecOldHists2D.at(iDraw2D) -> Draw("colz");
    pNew                      -> cd();
    vecNewHists2D.at(iDraw2D) -> Draw("colz");

    // draw text and save
    pNew                  -> cd();
    txt                   -> Draw();
    arrOutDirs[Dir::Plot] -> cd();
    cPlot2D               -> Write();
    cPlot2D               -> Close();

    // increment counter
    ++iDraw2D;
  }

  iDraw2D = 0;
  for (const string sCanvasName : vecCanvasNames2D) {

    // make new name
    const string sNameWithRatio = sCanvasName + "_" + sRatioLabel;

    // construct canvas
    TCanvas* cPlot2D = new TCanvas(sNameWithRatio.data(), "", width2D, heightR2);
    TPad*    pOld    = new TPad(sOldPadName.data(), "", xyOldPadR[0],  xyOldPadR[1],  xyOldPadR[2],  xyOldPadR[3]);
    TPad*    pNew    = new TPad(sNewPadName.data(), "", xyNewPadR[0],  xyNewPadR[1],  xyNewPadR[2],  xyNewPadR[3]);
    TPad*    pRat    = new TPad(sRatPadName.data(), "", xyRatPadR2[0], xyRatPadR2[1], xyRatPadR2[2], xyRatPadR2[3]);
    cPlot2D -> SetGrid(fGrid, fGrid);
    cPlot2D -> SetTicks(fTick, fTick);
    cPlot2D -> SetBorderMode(fMode);
    cPlot2D -> SetBorderSize(fBord);
    pOld    -> SetGrid(fGrid, fGrid);
    pOld    -> SetTicks(fTick, fTick);
    pOld    -> SetBorderMode(fMode);
    pOld    -> SetBorderSize(fBord);
    pOld    -> SetFrameBorderMode(fFrame);
    pOld    -> SetTopMargin(fMargin2D[0]);
    pOld    -> SetRightMargin(fMargin2D[1]);
    pOld    -> SetBottomMargin(fMargin2D[2]);
    pOld    -> SetLeftMargin(fMargin2D[3]);
    pOld    -> SetLogx(fLogX);
    pOld    -> SetLogy(fLogY);
    pOld    -> SetLogz(fLogZ);
    pNew    -> SetGrid(fGrid, fGrid);
    pNew    -> SetTicks(fTick, fTick);
    pNew    -> SetBorderMode(fMode);
    pNew    -> SetBorderSize(fBord);
    pNew    -> SetFrameBorderMode(fFrame);
    pNew    -> SetTopMargin(fMargin2D[0]);
    pNew    -> SetRightMargin(fMargin2D[1]);
    pNew    -> SetBottomMargin(fMargin2D[2]);
    pNew    -> SetLeftMargin(fMargin2D[3]);
    pNew    -> SetLogx(fLogX);
    pNew    -> SetLogy(fLogY);
    pNew    -> SetLogz(fLogZ);
    pRat    -> SetGrid(fGrid, fGrid);
    pRat    -> SetTicks(fTick, fTick);
    pRat    -> SetBorderMode(fMode);
    pRat    -> SetBorderSize(fBord);
    pRat    -> SetFrameBorderMode(fFrame);
    pRat    -> SetTopMargin(fMargin2D[0]);
    pRat    -> SetRightMargin(fMargin2D[1]);
    pRat    -> SetBottomMargin(fMargin2D[2]);
    pRat    -> SetLeftMargin(fMargin2D[3]);
    pRat    -> SetLogx(fLogX);
    pRat    -> SetLogy(fLogY);
    pRat    -> SetLogz(fLogZ);
    cPlot2D -> cd();
    pOld    -> Draw();
    pNew    -> Draw();
    pRat    -> Draw();

    // draw old vs. new vs. ratio histograms
    pOld                      -> cd();
    vecOldHists2D.at(iDraw2D) -> Draw("colz");
    txt                       -> Draw();
    pNew                      -> cd();
    vecNewHists2D.at(iDraw2D) -> Draw("colz");
    pRat                      -> cd();
    vecRatios2D.at(iDraw2D)   -> Draw("colz");

    // draw text and save
    arrOutDirs[Dir::Plot] -> cd();
    cPlot2D               -> Write();
    cPlot2D               -> Close();

    // increment counter
    ++iDraw2D;
  }
  cout << "    Made plots." << endl; 

  // save histogarms and close ------------------------------------------------

  // save old histograms
  arrOutDirs[Dir::Old] -> cd();
  for (TH1D* hOldHist1D : vecOldHists1D) {
    hOldHist1D -> Write();
  }
  for (TH2D* hOldHist2D : vecOldHists2D) {
    hOldHist2D -> Write();
  }

  // save new histograms
  arrOutDirs[Dir::New] -> cd();
  for (TH1D* hNewHist1D : vecNewHists1D) {
    hNewHist1D -> Write();
  }
  for (TH2D* hNewHist2D : vecNewHists2D) {
    hNewHist2D -> Write();
  }

  // save ratio histograms
  arrOutDirs[Dir::Ratio] -> cd();
  for (TH1D* hRatio1D : vecRatios1D) {
    hRatio1D -> Write();
  }
  for (TH2D* hRatio2D : vecRatios2D) {
    hRatio2D -> Write();
  }
  cout << "    Saved histograms." << endl;

  // close files
  fOutput -> cd();
  fOutput -> Close();
  fInOld  -> cd();
  fInOld  -> Close();
  fInNew  -> cd();
  fInNew  -> Close();

  // exit
  cout << "  Finished old vs. new comparison script!\n" << endl;
  return;

}

// end ------------------------------------------------------------------------
