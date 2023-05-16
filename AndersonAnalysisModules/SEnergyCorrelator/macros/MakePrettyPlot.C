// 'MakePrettyPlot.C'
// Derek Anderson
// 04.18.2021
//
// Use this quickly plot a set of numerator
// distributions against a denominator
// distribution

#include <iostream>
#include "TH1.h"
#include "TPad.h"
#include "TFile.h"
#include "TLine.h"
#include "TError.h"
#include "TString.h"
#include "TLegend.h"
#include "TCanvas.h"
#include "TPaveText.h"

using namespace std;

// global constants
static const UInt_t NNumer(14);
static const UInt_t NPlot(2);
static const UInt_t NPad(2);
static const UInt_t NVtx(4);



void MakePrettyPlot() {

  // lower verbosity
  gErrorIgnoreLevel = kError;
  cout << "\n  Beginning plot macro..." << endl;

  // output and denominator parameters
  const TString sOutput("recoCorrelator.forDebug_checkingHowCorrChanges.d9m4y2023.root");
  const TString sDenom("reference/alex_output/pp200run6jet40.SphenixRecoUnscaled_output_trksAndChrgPars.root");
  const TString sHistDenom("EEC0");
  const TString sNameDenom("hReferenceReco");
  const TString sLabelDenom("Reference output [Reco]");

  // numerator parameters
  const TString sNumer[NNumer]      = {"reference/alex_code/SphenixRecoUnscaled_output_derekTestRun.root",
                                       "reference/alex_code/SphenixRecoUnscaled_output_derekTestRun2.root",
                                       "reference/alex_code/SphenixRecoUnscaled_output_derekTestRun3.root",
                                       "pp200run6jet40.forDebug_reco_initialRun.d7m4y2023.root",
                                       "pp200run6jet40.forDebug_reco_usingTVectors.d7m4y2023.root",
                                       "pp200run6jet40.forDebug_reco_testingMinDr.d7m4y2023.root",
                                       "pp200run6jet40.forDebug_reco_noCstCuts.d7m4y2023.root",
                                       "pp200run6jet40.forDebug_reco_beingMoreCarefulWithJetPt.d7m4y2023.root",
                                       "pp200run6jet40.forDebug_reco_onlyOnePtJetRange.d9m4y2023.root",
                                       "pp200run6jet40.forDebug_reco_checkingJetPtRange.d9m4y2023.root",
                                       "pp200run6jet40.forDebug_reco_usingAbsValEta.d9m4y2023.root",
                                       "pp200run6jet40.forDebug_reco_numCstsG0.d9m4y2023.root",
                                       "pp200run6jet40.forDebug_reco_checkingNumCst.d10m4y2023.root",
                                       "pp200run6jet40.forDebug_reco_hardCodingNPoint.d10m4y2023.root"};
  const TString sHistNumer[NNumer]  = {"EEC0",
                                       "EEC0",
                                       "EEC0",
                                       "hCorrelatorDrAxis_ptJet15",
                                       "hCorrelatorDrAxis_ptJet15",
                                       "hCorrelatorDrAxis_ptJet15",
                                       "hCorrelatorDrAxis_ptJet15",
                                       "hCorrelatorDrAxis_ptJet15",
                                       "hCorrelatorDrAxis_ptJet15",
                                       "hCorrelatorDrAxis_ptJet15",
                                       "hCorrelatorDrAxis_ptJet15",
                                       "hCorrelatorDrAxis_ptJet15",
                                       "hCorrelatorDrAxis_ptJet15",
                                       "hCorrelatorDrAxis_ptJet15"};
  const TString sNameNumer[NNumer]  = {"hLocalAlexTest",
                                       "hLocalAlexTest_WithMyEECInit",
                                       "hLocalAlexTest_Only1520",
                                       "hF4A_InitialRun",
                                       "hF4A_UsingTVectors",
                                       "hF4A_TestingMinDr",
                                       "hF4A_NoCstCuts",
                                       "hF4A_BeingMoreCarefulWithJetPt",
                                       "hF4A_OnlyOnePtJetRange",
                                       "hF4A_CheckingPtJetRange",
                                       "hF4A_UsingAbsValEta",
                                       "hF4A_NumCstsG0",
                                       "hF4A_CheckingNumCst",
                                       "hF4A_HardCodedNPoint"};
  const TString sNameRatio[NNumer]  = {"hRatio_LocalAlexTest",
                                       "hRatio_LocalAlexTest_WithMyEECInit",
                                       "hRatio_LocalAlexTest_Only1520",
                                       "hRatio_InitialRun",
                                       "hRatio_UsingTVectors",
                                       "hRatio_TestingMinDr",
                                       "hRatio_NoCstCuts",
                                       "hRatio_BeingMoreCarefulWithJetPt",
                                       "hRatio_OnlyOnePtJetRange",
                                       "hRatio_CheckingPtJetRange",
                                       "hRatio_UsingAbsValEta",
                                       "hRatio_NumCstsG0",
                                       "hRatio_CheckingNumCst",
                                       "hRatio_HardCodedNPoint"};
  const TString sLabelNumer[NNumer] = {"Local test of Alex's Code",
                                       "Local test of Alex's Code (using my EEC initialization/header)",
                                       "Local test of Alex's Code (only pTjet = 15 - 20 GeV/c)",
                                       "Initial F4A run",
                                       "Using TVectors for pTcst calc",
                                       "Testing minimum #DeltaR",
                                       "No cst. cuts",
                                       "Being careful with pTjet selection",
                                       "Only 1 pTjet range",
                                       "Checking pTjet range",
                                       "Using |eta| in cut",
                                       "N_{cst} must be > 0",
                                       "Checking N_{cst} (no truth calc done)",
                                       "Hard-coded N-point"};

  // rebin parameters
  const UInt_t nRebin(2);
  const Bool_t doRebin(false);

  // plot parameters
  const TString sOptDenom("");
  const TString sOptNumer[NNumer] = {"SAME", "SAME", "SAME", "SAME", "SAME", "SAME", "SAME", "SAME", "SAME", "SAME", "SAME", "SAME", "SAME", "SAME"};
  const TString sOptRatio[NNumer] = {"",     "SAME", "SAME", "SAME", "SAME", "SAME", "SAME", "SAME", "SAME", "SAME", "SAME", "SAME", "SAME", "SAME"};
  const Float_t xPlotRange[NPlot] = {0.001, 1.};

  // style parameters
  const TString sTitle("");
  const TString sTitleX("#DeltaR");
  const TString sTitleY("EEC");
  const TString sTitleR("[change] / [reference]");
  const UInt_t  fColDen(923);
  const UInt_t  fMarDen(20);
  const UInt_t  fColNum[NNumer] = {799, 809, 899, 909, 879, 889, 859, 869, 839, 849, 819, 829, 921, 922};
  const UInt_t  fMarNum[NNumer] = {24,  26,  32,  25,  27,  28,  30,  30,  46,  24,  26,  32,  25,  26};

  // text parameters
  const TString sSys("#bf{#it{sPHENIX}} Simulation [Run6]");
  const TString sTrg("JS 40 GeV Jet Sample");
  const TString sJet("p_{T}^{jet} #in (15, 20) GeV");
  const TString sTyp("");
  const TString sHeader("");

  // open output and target files
  TFile *fOutput = new TFile(sOutput.Data(), "recreate");
  TFile *fDenom  = new TFile(sDenom.Data(), "read");
  if (!fOutput || !fDenom) {
    cerr << "PANIC: couldn't open output or denominator file!\n"
         << "       fOutput = " << fOutput << ", fDenom = " << fDenom
         << endl;
    return;
  }
  cout << "    Opened output and denominator files." << endl;

  // open unfolding files
  TFile *fNumer[NNumer];
  for (UInt_t iNumer = 0; iNumer < NNumer; iNumer++) {
    fNumer[iNumer] = new TFile(sNumer[iNumer].Data(), "read");
    if (!fNumer[iNumer]) {
      cerr << "PANIC: couldn't open numerator file #" << iNumer << "!" << endl;
      return;
    }
  }
  cout << "    Opened numerator files." << endl;

  // grab target histogram
  TH1D *hDenom = (TH1D*) fDenom -> Get(sHistDenom.Data());
  if (!hDenom) {
    cerr << "PANIC: couldn't grab denominator histogram!" << endl;
    return;
  }
  hDenom -> SetName(sNameDenom.Data());
  cout << "    Grabbed denominator histogram." << endl;

  // grab unfolding histograms
  TH1D *hNumer[NNumer];
  for (UInt_t iNumer = 0; iNumer < NNumer; iNumer++) {
    hNumer[iNumer] = (TH1D*) fNumer[iNumer] -> Get(sHistNumer[iNumer]);
    if (!hNumer[iNumer]) {
      cerr << "PANIC: couldn't grab numerator histogram #" << iNumer << "!" << endl;
      return;
    }
    hNumer[iNumer] -> SetName(sNameNumer[iNumer].Data());
  }
  cout << "    Grabbed numerator histograms." << endl;

  // rebin histograms (if needed)
  if (doRebin) {
    for (UInt_t iNumer = 0; iNumer < NNumer; iNumer++) {
      hNumer[iNumer] -> Rebin(nRebin);
    }
    hDenom -> Rebin(nRebin);
    cout << "    Rebinned histograms." << endl;
  }

  // calculate ratios
  TH1D *hRatio[NNumer];
  for (UInt_t iNumer = 0; iNumer < NNumer; iNumer++) {
    hRatio[iNumer] = (TH1D*) hDenom -> Clone();
    hRatio[iNumer] -> Reset("ICE");
    hRatio[iNumer] -> Divide(hNumer[iNumer], hDenom, 1., 1.);
    hRatio[iNumer] -> SetName(sNameRatio[iNumer]);
  }
  cout << "    Calculated ratios." << endl;

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
  hDenom -> SetMarkerColor(fColDen);
  hDenom -> SetMarkerStyle(fMarDen);
  hDenom -> SetFillColor(fColDen);
  hDenom -> SetFillStyle(fFil);
  hDenom -> SetLineColor(fColDen);
  hDenom -> SetLineStyle(fLin);
  hDenom -> SetLineWidth(fWid);
  hDenom -> SetTitle(sTitle.Data());
  hDenom -> SetTitleFont(fTxt);
  hDenom -> GetXaxis() -> SetRangeUser(xPlotRange[0], xPlotRange[1]);
  hDenom -> GetXaxis() -> SetTitle(sTitleX.Data());
  hDenom -> GetXaxis() -> SetTitleFont(fTxt);
  hDenom -> GetXaxis() -> SetTitleSize(fTit[1]);
  hDenom -> GetXaxis() -> SetTitleOffset(fOffX[1]);
  hDenom -> GetXaxis() -> SetLabelFont(fTxt);
  hDenom -> GetXaxis() -> SetLabelSize(fLab[1]);
  hDenom -> GetXaxis() -> CenterTitle(fCnt);
  hDenom -> GetYaxis() -> SetTitle(sTitleY.Data());
  hDenom -> GetYaxis() -> SetTitleFont(fTxt);
  hDenom -> GetYaxis() -> SetTitleSize(fTit[1]);
  hDenom -> GetYaxis() -> SetTitleOffset(fOffY[1]);
  hDenom -> GetYaxis() -> SetLabelFont(fTxt);
  hDenom -> GetYaxis() -> SetLabelSize(fLab[1]);
  hDenom -> GetYaxis() -> CenterTitle(fCnt);
  for (UInt_t iNumer = 0; iNumer < NNumer; iNumer++) {
    hNumer[iNumer] -> SetMarkerColor(fColNum[iNumer]);
    hNumer[iNumer] -> SetMarkerStyle(fMarNum[iNumer]);
    hNumer[iNumer] -> SetFillColor(fColNum[iNumer]);
    hNumer[iNumer] -> SetFillStyle(fFil);
    hNumer[iNumer] -> SetLineColor(fColNum[iNumer]);
    hNumer[iNumer] -> SetLineStyle(fLin);
    hNumer[iNumer] -> SetLineWidth(fWid);
    hNumer[iNumer] -> SetTitle(sTitle.Data());
    hNumer[iNumer] -> SetTitleFont(fTxt);
    hNumer[iNumer] -> GetXaxis() -> SetRangeUser(xPlotRange[0], xPlotRange[1]);
    hNumer[iNumer] -> GetXaxis() -> SetTitle(sTitleX.Data());
    hNumer[iNumer] -> GetXaxis() -> SetTitleFont(fTxt);
    hNumer[iNumer] -> GetXaxis() -> SetTitleSize(fTit[1]);
    hNumer[iNumer] -> GetXaxis() -> SetTitleOffset(fOffX[1]);
    hNumer[iNumer] -> GetXaxis() -> SetLabelFont(fTxt);
    hNumer[iNumer] -> GetXaxis() -> SetLabelSize(fLab[1]);
    hNumer[iNumer] -> GetXaxis() -> CenterTitle(fCnt);
    hNumer[iNumer] -> GetYaxis() -> SetTitle(sTitleY.Data());
    hNumer[iNumer] -> GetYaxis() -> SetTitleFont(fTxt);
    hNumer[iNumer] -> GetYaxis() -> SetTitleSize(fTit[1]);
    hNumer[iNumer] -> GetYaxis() -> SetTitleOffset(fOffY[1]);
    hNumer[iNumer] -> GetYaxis() -> SetLabelFont(fTxt);
    hNumer[iNumer] -> GetYaxis() -> SetLabelSize(fLab[1]);
    hNumer[iNumer] -> GetYaxis() -> CenterTitle(fCnt);
    hRatio[iNumer]  -> SetMarkerColor(fColNum[iNumer]);
    hRatio[iNumer]  -> SetMarkerStyle(fMarNum[iNumer]);
    hRatio[iNumer]  -> SetFillColor(fColNum[iNumer]);
    hRatio[iNumer]  -> SetFillStyle(fFil);
    hRatio[iNumer]  -> SetLineColor(fColNum[iNumer]);
    hRatio[iNumer]  -> SetLineStyle(fLin);
    hRatio[iNumer]  -> SetLineWidth(fWid);
    hRatio[iNumer]  -> SetTitle(sTitle.Data());
    hRatio[iNumer]  -> SetTitleFont(fTxt);
    hRatio[iNumer]  -> GetXaxis() -> SetRangeUser(xPlotRange[0], xPlotRange[1]);
    hRatio[iNumer]  -> GetXaxis() -> SetTitle(sTitleX.Data());
    hRatio[iNumer]  -> GetXaxis() -> SetTitleFont(fTxt);
    hRatio[iNumer]  -> GetXaxis() -> SetTitleSize(fTit[0]);
    hRatio[iNumer]  -> GetXaxis() -> SetTitleOffset(fOffX[0]);
    hRatio[iNumer]  -> GetXaxis() -> SetLabelFont(fTxt);
    hRatio[iNumer]  -> GetXaxis() -> SetLabelSize(fLab[0]);
    hRatio[iNumer]  -> GetXaxis() -> CenterTitle(fCnt);
    hRatio[iNumer]  -> GetYaxis() -> SetTitle(sTitleR.Data());
    hRatio[iNumer]  -> GetYaxis() -> SetTitleFont(fTxt);
    hRatio[iNumer]  -> GetYaxis() -> SetTitleSize(fTit[0]);
    hRatio[iNumer]  -> GetYaxis() -> SetTitleOffset(fOffY[0]);
    hRatio[iNumer]  -> GetYaxis() -> SetLabelFont(fTxt);
    hRatio[iNumer]  -> GetYaxis() -> SetLabelSize(fLab[0]);
    hRatio[iNumer]  -> GetYaxis() -> CenterTitle(fCnt);
  }
  cout << "    Set styles." << endl;

  // make legend
  const UInt_t  fColLe(0);
  const UInt_t  fFilLe(0);
  const UInt_t  fLinLe(0);
  const Float_t fLegXY[NVtx] = {0.1, 0.1, 0.3, 0.3};
  TLegend *leg = new TLegend(fLegXY[0], fLegXY[1], fLegXY[2], fLegXY[3], sHeader.Data());
  leg -> SetFillColor(fColLe);
  leg -> SetFillStyle(fFilLe);
  leg -> SetLineColor(fColLe);
  leg -> SetLineStyle(fLinLe);
  leg -> SetTextFont(fTxt);
  leg -> SetTextAlign(fAln);
  leg -> AddEntry(hDenom, sLabelDenom.Data(), "pf");
  for (UInt_t iNumer = 0; iNumer < NNumer; iNumer++) {
    leg -> AddEntry(hNumer[iNumer], sLabelNumer[iNumer], "pf");
  }
  cout << "    Made legend." << endl;

  // make text
  const UInt_t fColTx(0);
  const UInt_t fFilTx(0);
  const UInt_t fLinTx(0);
  const Float_t fTxtXY[NVtx] = {0.3, 0.1, 0.5, 0.3};
  TPaveText *txt = new TPaveText(fTxtXY[0], fTxtXY[1], fTxtXY[2], fTxtXY[3], "NDC NB");
  txt -> SetFillColor(fColTx);
  txt -> SetFillStyle(fFilTx);
  txt -> SetLineColor(fColTx);
  txt -> SetLineStyle(fLinTx);
  txt -> SetTextFont(fTxt);
  txt -> SetTextAlign(fAln);
  txt -> AddText(sSys.Data());
  txt -> AddText(sTrg.Data());
  txt -> AddText(sJet.Data());
  txt -> AddText(sTyp.Data());
  cout << "    Made text." << endl;

  // make line
  const UInt_t  fColLi(1);
  const UInt_t  fLinLi(9);
  const UInt_t  fWidLi(1);
  const Float_t fLinXY[NVtx] = {xPlotRange[0], 1., xPlotRange[1], 1.};
  TLine *line = new TLine(fLinXY[0], fLinXY[1], fLinXY[2], fLinXY[3]);
  line -> SetLineColor(fColLi);
  line -> SetLineStyle(fLinLi);
  line -> SetLineWidth(fWidLi);
  cout << "    Made line." << endl;

  // make plot
  const UInt_t  width(750);
  const UInt_t  height(950);
  const UInt_t  heightNR(750);
  const UInt_t  fMode(0);
  const UInt_t  fBord(2);
  const UInt_t  fGrid(0);
  const UInt_t  fTick(1);
  const UInt_t  fLogX(1);
  const UInt_t  fLogY1(0);
  const UInt_t  fLogY2(1);
  const UInt_t  fLogYNR(1);
  const UInt_t  fFrame(0);
  const Float_t fMarginL(0.15);
  const Float_t fMarginR(0.02);
  const Float_t fMarginT1(0.005);
  const Float_t fMarginT2(0.02);
  const Float_t fMarginTNR(0.02);
  const Float_t fMarginB1(0.25);
  const Float_t fMarginB2(0.005);
  const Float_t fMarginBNR(0.15);
  const Float_t fPadXY1[NVtx] = {0., 0., 1., 0.35};
  const Float_t fPadXY2[NVtx] = {0., 0.35, 1., 1.};

  TCanvas *cPlot = new TCanvas("cPlot", "", width, height);
  TPad    *pPad1 = new TPad("pPad1", "", fPadXY1[0], fPadXY1[1], fPadXY1[2], fPadXY1[3]);
  TPad    *pPad2 = new TPad("pPad2", "", fPadXY2[0], fPadXY2[1], fPadXY2[2], fPadXY2[3]);
  cPlot     -> SetGrid(fGrid, fGrid);
  cPlot     -> SetTicks(fTick, fTick);
  cPlot     -> SetBorderMode(fMode);
  cPlot     -> SetBorderSize(fBord);
  pPad1     -> SetGrid(fGrid, fGrid);
  pPad1     -> SetTicks(fTick, fTick);
  pPad1     -> SetLogx(fLogX);
  pPad1     -> SetLogy(fLogY1);
  pPad1     -> SetBorderMode(fMode);
  pPad1     -> SetBorderSize(fBord);
  pPad1     -> SetFrameBorderMode(fFrame);
  pPad1     -> SetLeftMargin(fMarginL);
  pPad1     -> SetRightMargin(fMarginR);
  pPad1     -> SetTopMargin(fMarginT1);
  pPad1     -> SetBottomMargin(fMarginB1);
  pPad2     -> SetGrid(fGrid, fGrid);
  pPad2     -> SetTicks(fTick, fTick);
  pPad2     -> SetLogx(fLogX);
  pPad2     -> SetLogy(fLogY2);
  pPad2     -> SetBorderMode(fMode);
  pPad2     -> SetBorderSize(fBord);
  pPad2     -> SetFrameBorderMode(fFrame);
  pPad2     -> SetLeftMargin(fMarginL);
  pPad2     -> SetRightMargin(fMarginR);
  pPad2     -> SetTopMargin(fMarginT2);
  pPad2     -> SetBottomMargin(fMarginB2);
  cPlot     -> cd();
  pPad1     -> Draw();
  pPad2     -> Draw();
  pPad1     -> cd();
  hRatio[0] -> Draw(sOptRatio[0].Data());
  for (UInt_t iNumer = 1; iNumer < NNumer; iNumer++) {
    hRatio[iNumer] -> Draw(sOptRatio[iNumer].Data());
  }
  line   -> Draw();
  pPad2  -> cd();
  hDenom -> Draw(sOptDenom.Data());
  for(UInt_t iNumer = 0; iNumer < NNumer; iNumer++) {
    hNumer[iNumer] -> Draw(sOptNumer[iNumer].Data());
  }
  leg     -> Draw();
  txt     -> Draw();
  fOutput -> cd();
  cPlot   -> Write();
  cPlot   -> Close();

  TCanvas *cPlotNR = new TCanvas("cPlotNoRatio", "", width, heightNR);
  cPlotNR -> SetGrid(fGrid, fGrid);
  cPlotNR -> SetTicks(fTick, fTick);
  cPlotNR -> SetBorderMode(fMode);
  cPlotNR -> SetBorderSize(fBord);
  cPlotNR -> SetFrameBorderMode(fFrame);
  cPlotNR -> SetLeftMargin(fMarginL);
  cPlotNR -> SetRightMargin(fMarginR);
  cPlotNR -> SetTopMargin(fMarginTNR);
  cPlotNR -> SetBottomMargin(fMarginBNR);
  cPlotNR -> SetLogx(fLogX);
  cPlotNR -> SetLogy(fLogYNR);
  cPlotNR -> cd();
  hDenom  -> Draw(sOptDenom.Data());
  for(UInt_t iNumer = 0; iNumer < NNumer; iNumer++) {
    hNumer[iNumer] -> Draw(sOptNumer[iNumer].Data());
  }
  leg     -> Draw();
  txt     -> Draw();
  fOutput -> cd();
  cPlotNR -> Write();
  cPlotNR -> Close();
  cout << "    Made plot." << endl;

  // save histograms
  fOutput -> cd();
  hDenom  -> Write();
  for (UInt_t iNumer = 0; iNumer < NNumer; iNumer++) {
    hNumer[iNumer] -> Write();
    hRatio[iNumer] -> Write();
  }
  cout << "    Saved histograms." << endl;

  // close files
  fOutput -> cd();
  fOutput -> Close();
  fDenom  -> cd();
  fDenom  -> Close();
  for (UInt_t iNumer = 0; iNumer < NNumer; iNumer++) {
    fNumer[iNumer] -> cd();
    fNumer[iNumer] -> Close();
  }
  cout << "  Finished plot!\n" << endl;

}

// end ------------------------------------------------------------------------
