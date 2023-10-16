// ----------------------------------------------------------------------------
// 'DoPercentDifference.C'
// Derek Anderson
// 04.27.2023
//
// Calculates the percent difference between multiple
// histograms and plots them.
//
// NOTE: 'FSubMode' controls how the difference is
// calculated.
//   FSubMode = 0: (ref - comp) / ref
//   FSubMode = 1: (ln(ref) - ln(comp)) / ln(ref)
//   FSubMode = 2: ref - comp
//   FSubMode = 3: ln(ref) - ln(comp)
// ----------------------------------------------------------------------------

// c includes
#include <iostream>
// root includes
#include "TH1.h"
#include "TPad.h"
#include "TFile.h"
#include "TMath.h"
#include "TLine.h"
#include "TError.h"
#include "TString.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TPaveText.h"

using namespace std;

// global constants
static const UInt_t NModes(4);
static const UInt_t NRange(2);
static const UInt_t NComp(7);
static const UInt_t NPlot(2);
static const UInt_t NPad(2);
static const UInt_t NVtx(4);
static const UInt_t NTxt(3);

// calculation parameters
static const UInt_t FSubMode(0);
static const Bool_t DoRebin(false);
static const Bool_t DoIntNorm(true);
static const Bool_t DoFluctuationRemoval(true);



void DoPercentDifference() {

  // lower verbosity
  gErrorIgnoreLevel = kWarning;
  cout << "\n  Beginning percent difference script..." << endl;

  // output and reference parameters
  const TString sOut("tuningDetEffects.testingPerDiffVsDiff_perDiffOverComp_pt15.pp200py8jet40.d27m4y2023.root");
  const TString sRef("tuning_detector_effects/SphenixRecoUnscaledWithConstitHistos_output.root");
  const TString sHistRef("EEC0");
  const TString sNameRef("hRecoEEC_pt15");
  const TString sLabelRef("Reco.");

  // comparison parameters
  const TString sComp[NComp]      = {"tuning_detector_effects/SphenixTruthShift0Check_output.root",
                                     "tuning_detector_effects/SphenixTruthShift.001Check_output.root",
                                     "tuning_detector_effects/SphenixTruthShift.005Check_output.root",
                                     "tuning_detector_effects/SphenixTruthShift.01Check_output.root",
                                     "tuning_detector_effects/SphenixTruthShift.015Check_output.root",
                                     "tuning_detector_effects/SphenixTruthShift.02Check_output.root",
                                     "tuning_detector_effects/SphenixTruthShift.025Check_output.root"};
  const TString sHistComp[NComp]  = {"EEC0",
                                     "EEC0",
                                     "EEC0",
                                     "EEC0",
                                     "EEC0",
                                     "EEC0",
                                     "EEC0"};
  const TString sNameComp[NComp]  = {"hTrueEEC_pt15_noSmear",
                                     "hTrueEEC_pt15_smear001",
                                     "hTrueEEC_pt15_smear005",
                                     "hTrueEEC_pt15_smear01",
                                     "hTrueEEC_pt15_smear015",
                                     "hTrueEEC_pt15_smear02",
                                     "hTrueEEC_pt15_smear025"};
  const TString sNameSub[NComp]   = {"hSub_pt15_noSmear",
                                     "hSub_pt15_smear001",
                                     "hSub_pt15_smear005",
                                     "hSub_pt15_smear01",
                                     "hSub_pt15_smear015",
                                     "hSub_pt15_smear02",
                                     "hSub_pt15_smear025"};
  const TString sLabelComp[NComp] = {"Truth (no smearing)",
                                     "Truth (smear width = 0.001)",
                                     "Truth (smear width = 0.005)",
                                     "Truth (smear width = 0.01)",
                                     "Truth (smear width = 0.015)",
                                     "Truth (smear width = 0.02)",
                                     "Truth (smear width = 0.025)"};

  // calculation parameters
  const UInt_t  nRebin(2);
  const Float_t rMainPeak[NRange]  = {0.001, 1.};
  const TString sTitleSubs[NModes] = {"\%-difference", "\%-diff. of logs", "difference", "diff. of logs"};

  // plot parameters
  const TString sOptRef("");
  const TString sOptComp[NComp]   = {"SAME",   "SAME",        "SAME",        "SAME",        "SAME",        "SAME",         "SAME"};
  const TString sOptSub[NComp]    = {"HIST P", "SAME HIST P", "SAME HIST P", "SAME HIST P", "SAME HIST P", "SAME HIST P", "SAME HIST p"};
  const Float_t xPlotRange[NPlot] = {0.0001, 1.};

  // style parameters
  const TString sTitle("");
  const TString sTitleX("#DeltaR");
  const TString sTitleY("Normalized EEC");
  const UInt_t  fColDen(923);
  const UInt_t  fMarDen(20);
  const UInt_t  fColNum[NComp] = {921, 799, 819, 839, 859, 879, 899};
  const UInt_t  fMarNum[NComp] = {24,  26,  32,  25,  27,  28,  30};

  // text parameters
  const TString sHeader("");
  const TString sTxt[NTxt] = {"#bf{#it{sPHENIX}} Simulation [Run6]", "PYTHIA-8, JS 40 GeV jet sample", "#bf{p_{T}^{jet} #in (15, 20) GeV/c}"};

  // open output and reference files
  TFile *fOut = new TFile(sOut.Data(), "recreate");
  TFile *fRef  = new TFile(sRef.Data(), "read");
  if (!fOut || !fRef) {
    cerr << "PANIC: couldn't open output or reference file!\n"
         << "       fOut = " << fOut << ", fRef = " << fRef
         << endl;
    return;
  }
  cout << "    Opened output and reference files." << endl;

  // open comparison files
  TFile *fComp[NComp];
  for (UInt_t iComp = 0; iComp < NComp; iComp++) {
    fComp[iComp] = new TFile(sComp[iComp].Data(), "read");
    if (!fComp[iComp]) {
      cerr << "PANIC: couldn't open comparison file #" << iComp << "!" << endl;
      return;
    }
  }
  cout << "    Opened comparison files." << endl;

  // grab reference histogram
  TH1D *hRef = (TH1D*) fRef -> Get(sHistRef.Data());
  if (!hRef) {
    cerr << "PANIC: couldn't grab reference histogram!" << endl;
    return;
  }
  hRef -> SetName(sNameRef.Data());
  cout << "    Grabbed reference histogram." << endl;

  // grab comparison histograms
  TH1D *hComp[NComp];
  for (UInt_t iComp = 0; iComp < NComp; iComp++) {
    hComp[iComp] = (TH1D*) fComp[iComp] -> Get(sHistComp[iComp]);
    if (!hComp[iComp]) {
      cerr << "PANIC: couldn't grab comparison histogram #" << iComp << "!" << endl;
      return;
    }
    hComp[iComp] -> SetName(sNameComp[iComp].Data());
  }
  cout << "    Grabbed comparison histograms." << endl;

  // rebin histograms (if needed)
  if (DoRebin) {
    for (UInt_t iComp = 0; iComp < NComp; iComp++) {
      hComp[iComp] -> Rebin(nRebin);
    }
    hRef -> Rebin(nRebin);
    cout << "    Rebinned histograms." << endl;
  }

  // do normalization (if needed)
  if (DoIntNorm) {

    // remove fluctuations (if needed)
    if (DoFluctuationRemoval) {

      // remove fluctions for reference histogram
      {

        // no. of bins
        const UInt_t nBins = hRef -> GetNbinsX();

        // loop over bins and determine height of main peak
        Double_t yMainPeak = 0.;
        for (UInt_t iBin = 1; iBin < (nBins + 1); iBin++) {

          // check if bin is in range
          const Double_t xBinCenter        = hRef -> GetBinCenter(iBin);
          const Bool_t   isInMainPeakRange = ((xBinCenter >= rMainPeak[0]) && (xBinCenter < rMainPeak[1]));
          if (!isInMainPeakRange) continue;

          // check if bigger than current max
          const Double_t yBin = hRef -> GetBinContent(iBin);
          if (yBin > yMainPeak) {
            yMainPeak = yBin;
          }
        }  // end bin loop

        // loop over bins below main peak and 0 out any which are unusually high
        for (UInt_t iBin = 1; iBin < (nBins + 1); iBin++) {

          // check if bin is less than main peak range
          const Double_t xBinCenter      = hRef -> GetBinCenter(iBin);
          const Bool_t   isBelowMainPeak = (xBinCenter < rMainPeak[0]);
          if (!isBelowMainPeak) continue;

          // 0 out bin if above main peak
          const Double_t yBin = hRef -> GetBinContent(iBin);
          if (yBin >= yMainPeak) {
            hRef -> SetBinContent(iBin, 0.);
            hRef -> SetBinError(iBin, 0.);
          }
        }  // end bin loop
      }  // end reference fluctuation removal
      cout << "    Removed fluctuations for reference histogram." << endl;

      // loop over comparisons histograms
      for (UInt_t iComp = 0; iComp < NComp; iComp++) {

        // no. of bins
        const UInt_t nBins = hRef -> GetNbinsX();

        // loop over bins and determine height of main peak
        Double_t yMainPeak = 0.;
        for (UInt_t iBin = 1; iBin < (nBins + 1); iBin++) {

          // check if bin is in range
          const Double_t xBinCenter        = hComp[iComp] -> GetBinCenter(iBin);
          const Bool_t   isInMainPeakRange = ((xBinCenter >= rMainPeak[0]) && (xBinCenter < rMainPeak[1]));
          if (!isInMainPeakRange) continue;

          // check if bigger than current max
          const Double_t yBin = hComp[iComp] -> GetBinContent(iBin);
          if (yBin > yMainPeak) {
            yMainPeak = yBin;
          }
        }  // end bin loop

        // loop over bins below main peak and 0 out any which are unusually high
        for (UInt_t iBin = 1; iBin < (nBins + 1); iBin++) {

          // check if bin is less than main peak range
          const Double_t xBinCenter      = hComp[iComp] -> GetBinCenter(iBin);
          const Bool_t   isBelowMainPeak = (xBinCenter < rMainPeak[0]);
          if (!isBelowMainPeak) continue;

          // 0 out bin if above main peak
          const Double_t yBin = hComp[iComp] -> GetBinContent(iBin);
          if (yBin >= yMainPeak) {
            hComp[iComp] -> SetBinContent(iBin, 0.);
            hComp[iComp] -> SetBinError(iBin, 0.);
          }
        }  // end bin loop
      }  // end comparison loop
      cout << "    Removed fluctuations for comparison histograms." << endl;
    }  // end if (DoFluctuationRemoval)

    for (UInt_t iComp = 0; iComp < NComp; iComp++) {
      const Double_t intComp = hComp[iComp] -> Integral();
      if (intComp > 0.) hComp[iComp] -> Scale(1. / intComp);
    }

    const Double_t intRef = hRef -> Integral();
    if (intRef > 0.) hRef -> Scale(1. / intRef);
    cout << "    Normalized histograms." << endl;
  }

  // do subtractions
  TH1D *hSub[NComp];
  for (UInt_t iComp = 0; iComp < NComp; iComp++) {

    // initialize subtraction histograms
    hSub[iComp] = (TH1D*) hRef -> Clone();
    hSub[iComp] -> Reset("ICE");
    hSub[iComp] -> SetName(sNameSub[iComp]);

    // loop over bins
    const UInt_t nBins = hSub[iComp] -> GetNbinsX();
    for (UInt_t iBin = 1; iBin < (nBins + 1); iBin++) {

      // get content and errors
      const Double_t yRef  = hRef         -> GetBinContent(iBin);
      const Double_t yComp = hComp[iComp] -> GetBinContent(iBin);
      const Double_t eRef  = hRef         -> GetBinError(iBin);
      const Double_t eComp = hComp[iComp] -> GetBinError(iBin);

      // do subtraction
      Double_t ySub(-1.);
      Double_t eSub(0.);
      switch (FSubMode) {
        case 0:
          ySub  = (yRef - yComp) / yComp;
          eSub  = 0;  // FIXME do propagation of errors
          break;
        case 1:
          ySub  = (TMath::Log(yRef) - TMath::Log(yComp)) / TMath::Log(yRef);
          eSub  = 0;  // FIXME do propagation of errors
          break;
        case 2:
          ySub  = yRef - yComp;
          eSub  = 0;  // FIXME do propagation of errors
          break;
        case 3:
          ySub  = TMath::Log(yRef) - TMath::Log(yComp);
          eSub  = 0;  // FIXME do propagation of errors
          break;
        default:
          ySub  = (yRef - yComp) / yRef;
          eSub  = 0;  // FIXME do propagation of errors
          break;
      }

      // set bin content/errors
      if (yRef > 0.) {
        hSub[iComp] -> SetBinContent(iBin, ySub);
        hSub[iComp] -> SetBinError(iBin, eSub);
      } else {
        hSub[iComp] -> SetBinContent(iBin, 0.);
        hSub[iComp] -> SetBinError(iBin, 0.);
      }
    }  // end bin loop
  }  // end comparison loop
  cout << "    Calculated ratios." << endl;

  // pick out subtraction title
  TString sTitleS("");
  switch (FSubMode) {
    case 0:
      sTitleS = sTitleSubs[0];
      break;
    case 1:
      sTitleS = sTitleSubs[1];
      break;
    case 2:
      sTitleS = sTitleSubs[2];
      break;
    case 3:
      sTitleS = sTitleSubs[3];
      break;
    default:
      sTitleS = sTitleSubs[0];
      break;
  }

  // set styles
  const UInt_t  fFil(0);
  const UInt_t  fLin(1);
  const UInt_t  fWid(1);
  const UInt_t  fTxt(42);
  const UInt_t  fAln(12);
  const UInt_t  fCnt(1);
  const Float_t fLab[NPad]  = {0.074, 0.04};
  const Float_t fTit[NPad]  = {0.074, 0.04};
  const Float_t fOffX[NPad] = {1.1, 1.};
  const Float_t fOffY[NPad] = {0.7, 1.3};
  hRef -> SetMarkerColor(fColDen);
  hRef -> SetMarkerStyle(fMarDen);
  hRef -> SetFillColor(fColDen);
  hRef -> SetFillStyle(fFil);
  hRef -> SetLineColor(fColDen);
  hRef -> SetLineStyle(fLin);
  hRef -> SetLineWidth(fWid);
  hRef -> SetTitle(sTitle.Data());
  hRef -> SetTitleFont(fTxt);
  hRef -> GetXaxis() -> SetRangeUser(xPlotRange[0], xPlotRange[1]);
  hRef -> GetXaxis() -> SetTitle(sTitleX.Data());
  hRef -> GetXaxis() -> SetTitleFont(fTxt);
  hRef -> GetXaxis() -> SetTitleSize(fTit[1]);
  hRef -> GetXaxis() -> SetTitleOffset(fOffX[1]);
  hRef -> GetXaxis() -> SetLabelFont(fTxt);
  hRef -> GetXaxis() -> SetLabelSize(fLab[1]);
  hRef -> GetXaxis() -> CenterTitle(fCnt);
  hRef -> GetYaxis() -> SetTitle(sTitleY.Data());
  hRef -> GetYaxis() -> SetTitleFont(fTxt);
  hRef -> GetYaxis() -> SetTitleSize(fTit[1]);
  hRef -> GetYaxis() -> SetTitleOffset(fOffY[1]);
  hRef -> GetYaxis() -> SetLabelFont(fTxt);
  hRef -> GetYaxis() -> SetLabelSize(fLab[1]);
  hRef -> GetYaxis() -> CenterTitle(fCnt);
  for (UInt_t iComp = 0; iComp < NComp; iComp++) {
    hComp[iComp] -> SetMarkerColor(fColNum[iComp]);
    hComp[iComp] -> SetMarkerStyle(fMarNum[iComp]);
    hComp[iComp] -> SetFillColor(fColNum[iComp]);
    hComp[iComp] -> SetFillStyle(fFil);
    hComp[iComp] -> SetLineColor(fColNum[iComp]);
    hComp[iComp] -> SetLineStyle(fLin);
    hComp[iComp] -> SetLineWidth(fWid);
    hComp[iComp] -> SetTitle(sTitle.Data());
    hComp[iComp] -> SetTitleFont(fTxt);
    hComp[iComp] -> GetXaxis() -> SetRangeUser(xPlotRange[0], xPlotRange[1]);
    hComp[iComp] -> GetXaxis() -> SetTitle(sTitleX.Data());
    hComp[iComp] -> GetXaxis() -> SetTitleFont(fTxt);
    hComp[iComp] -> GetXaxis() -> SetTitleSize(fTit[1]);
    hComp[iComp] -> GetXaxis() -> SetTitleOffset(fOffX[1]);
    hComp[iComp] -> GetXaxis() -> SetLabelFont(fTxt);
    hComp[iComp] -> GetXaxis() -> SetLabelSize(fLab[1]);
    hComp[iComp] -> GetXaxis() -> CenterTitle(fCnt);
    hComp[iComp] -> GetYaxis() -> SetTitle(sTitleY.Data());
    hComp[iComp] -> GetYaxis() -> SetTitleFont(fTxt);
    hComp[iComp] -> GetYaxis() -> SetTitleSize(fTit[1]);
    hComp[iComp] -> GetYaxis() -> SetTitleOffset(fOffY[1]);
    hComp[iComp] -> GetYaxis() -> SetLabelFont(fTxt);
    hComp[iComp] -> GetYaxis() -> SetLabelSize(fLab[1]);
    hComp[iComp] -> GetYaxis() -> CenterTitle(fCnt);
    hSub[iComp]  -> SetMarkerColor(fColNum[iComp]);
    hSub[iComp]  -> SetMarkerStyle(fMarNum[iComp]);
    hSub[iComp]  -> SetFillColor(fColNum[iComp]);
    hSub[iComp]  -> SetFillStyle(fFil);
    hSub[iComp]  -> SetLineColor(fColNum[iComp]);
    hSub[iComp]  -> SetLineStyle(fLin);
    hSub[iComp]  -> SetLineWidth(fWid);
    hSub[iComp]  -> SetTitle(sTitle.Data());
    hSub[iComp]  -> SetTitleFont(fTxt);
    hSub[iComp]  -> GetXaxis() -> SetRangeUser(xPlotRange[0], xPlotRange[1]);
    hSub[iComp]  -> GetXaxis() -> SetTitle(sTitleX.Data());
    hSub[iComp]  -> GetXaxis() -> SetTitleFont(fTxt);
    hSub[iComp]  -> GetXaxis() -> SetTitleSize(fTit[0]);
    hSub[iComp]  -> GetXaxis() -> SetTitleOffset(fOffX[0]);
    hSub[iComp]  -> GetXaxis() -> SetLabelFont(fTxt);
    hSub[iComp]  -> GetXaxis() -> SetLabelSize(fLab[0]);
    hSub[iComp]  -> GetXaxis() -> CenterTitle(fCnt);
    hSub[iComp]  -> GetYaxis() -> SetTitle(sTitleS.Data());
    hSub[iComp]  -> GetYaxis() -> SetTitleFont(fTxt);
    hSub[iComp]  -> GetYaxis() -> SetTitleSize(fTit[0]);
    hSub[iComp]  -> GetYaxis() -> SetTitleOffset(fOffY[0]);
    hSub[iComp]  -> GetYaxis() -> SetLabelFont(fTxt);
    hSub[iComp]  -> GetYaxis() -> SetLabelSize(fLab[0]);
    hSub[iComp]  -> GetYaxis() -> CenterTitle(fCnt);
  }
  cout << "    Set styles." << endl;

  // make legend
  const UInt_t  fColLe       = 0;
  const UInt_t  fFilLe       = 0;
  const UInt_t  fLinLe       = 0;
  const Float_t yObjLe       = 0.1 + ((NComp + 1) * 0.05);
  const Float_t fLegXY[NVtx] = {0.1, 0.1, 0.3, yObjLe};

  TLegend *leg = new TLegend(fLegXY[0], fLegXY[1], fLegXY[2], fLegXY[3], sHeader.Data());
  leg -> SetFillColor(fColLe);
  leg -> SetFillStyle(fFilLe);
  leg -> SetLineColor(fColLe);
  leg -> SetLineStyle(fLinLe);
  leg -> SetTextFont(fTxt);
  leg -> SetTextAlign(fAln);
  leg -> AddEntry(hRef, sLabelRef.Data(), "pf");
  for (UInt_t iComp = 0; iComp < NComp; iComp++) {
    leg -> AddEntry(hComp[iComp], sLabelComp[iComp], "pf");
  }
  cout << "    Made legend." << endl;

  // make text
  const UInt_t  fColTx       = 0;
  const UInt_t  fFilTx       = 0;
  const UInt_t  fLinTx       = 0;
  const Float_t yObjTx       = 0.1 + (NTxt * 0.05);
  const Float_t fTxtXY[NVtx] = {0.3, 0.1, 0.5, yObjTx};

  TPaveText *txt = new TPaveText(fTxtXY[0], fTxtXY[1], fTxtXY[2], fTxtXY[3], "NDC NB");
  txt -> SetFillColor(fColTx);
  txt -> SetFillStyle(fFilTx);
  txt -> SetLineColor(fColTx);
  txt -> SetLineStyle(fLinTx);
  txt -> SetTextFont(fTxt);
  txt -> SetTextAlign(fAln);
  for (UInt_t iTxt = 0; iTxt < NTxt; iTxt++) {
    txt -> AddText(sTxt[iTxt].Data());
  }
  cout << "    Made text." << endl;

  // make line
  const UInt_t  fColLi(1);
  const UInt_t  fLinLi(9);
  const UInt_t  fWidLi(1);
  const Float_t fLinXY[NVtx] = {xPlotRange[0], 0., xPlotRange[1], 0.};

  TLine *line = new TLine(fLinXY[0], fLinXY[1], fLinXY[2], fLinXY[3]);
  line -> SetLineColor(fColLi);
  line -> SetLineStyle(fLinLi);
  line -> SetLineWidth(fWidLi);
  cout << "    Made line." << endl;

  // make plot
  const UInt_t  width(750);
  const UInt_t  height(950);
  const UInt_t  fMode(0);
  const UInt_t  fBord(2);
  const UInt_t  fGrid(0);
  const UInt_t  fTick(1);
  const UInt_t  fLogX(1);
  const UInt_t  fLogY1(0);
  const UInt_t  fLogY2(1);
  const UInt_t  fFrame(0);
  const Float_t fMarginL(0.15);
  const Float_t fMarginR(0.02);
  const Float_t fMarginT1(0.005);
  const Float_t fMarginT2(0.02);
  const Float_t fMarginTNR(0.02);
  const Float_t fMarginB1(0.25);
  const Float_t fMarginB2(0.005);
  const Float_t fMarginBNR(0.15);
  const Float_t fPadXY1[NVtx] = {0., 0.,   1., 0.35};
  const Float_t fPadXY2[NVtx] = {0., 0.35, 1., 1.};

  TCanvas *cPlot = new TCanvas("cPlot", "", width, height);
  TPad    *pPad1 = new TPad("pPad1", "", fPadXY1[0], fPadXY1[1], fPadXY1[2], fPadXY1[3]);
  TPad    *pPad2 = new TPad("pPad2", "", fPadXY2[0], fPadXY2[1], fPadXY2[2], fPadXY2[3]);
  cPlot   -> SetGrid(fGrid, fGrid);
  cPlot   -> SetTicks(fTick, fTick);
  cPlot   -> SetBorderMode(fMode);
  cPlot   -> SetBorderSize(fBord);
  pPad1   -> SetGrid(fGrid, fGrid);
  pPad1   -> SetTicks(fTick, fTick);
  pPad1   -> SetLogx(fLogX);
  pPad1   -> SetLogy(fLogY1);
  pPad1   -> SetBorderMode(fMode);
  pPad1   -> SetBorderSize(fBord);
  pPad1   -> SetFrameBorderMode(fFrame);
  pPad1   -> SetLeftMargin(fMarginL);
  pPad1   -> SetRightMargin(fMarginR);
  pPad1   -> SetTopMargin(fMarginT1);
  pPad1   -> SetBottomMargin(fMarginB1);
  pPad2   -> SetGrid(fGrid, fGrid);
  pPad2   -> SetTicks(fTick, fTick);
  pPad2   -> SetLogx(fLogX);
  pPad2   -> SetLogy(fLogY2);
  pPad2   -> SetBorderMode(fMode);
  pPad2   -> SetBorderSize(fBord);
  pPad2   -> SetFrameBorderMode(fFrame);
  pPad2   -> SetLeftMargin(fMarginL);
  pPad2   -> SetRightMargin(fMarginR);
  pPad2   -> SetTopMargin(fMarginT2);
  pPad2   -> SetBottomMargin(fMarginB2);
  cPlot   -> cd();
  pPad1   -> Draw();
  pPad2   -> Draw();
  pPad1   -> cd();
  hSub[0] -> Draw(sOptSub[0].Data());
  for (UInt_t iComp = 1; iComp < NComp; iComp++) {
    hSub[iComp] -> Draw(sOptSub[iComp].Data());
  }
  line  -> Draw();
  pPad2 -> cd();
  hRef  -> Draw(sOptRef.Data());
  for(UInt_t iComp = 0; iComp < NComp; iComp++) {
    hComp[iComp] -> Draw(sOptComp[iComp].Data());
  }
  leg   -> Draw();
  txt   -> Draw();
  fOut  -> cd();
  cPlot -> Write();
  cPlot -> Close();
  cout << "    Made plot." << endl;

  // save histograms
  fOut -> cd();
  hRef  -> Write();
  for (UInt_t iComp = 0; iComp < NComp; iComp++) {
    hComp[iComp] -> Write();
    hSub[iComp] -> Write();
  }
  cout << "    Saved histograms." << endl;

  // close files
  fOut -> cd();
  fOut -> Close();
  fRef -> cd();
  fRef -> Close();
  for (UInt_t iComp = 0; iComp < NComp; iComp++) {
    fComp[iComp] -> cd();
    fComp[iComp] -> Close();
  }
  cout << "  Finished plot!\n" << endl;

}

// end ------------------------------------------------------------------------
