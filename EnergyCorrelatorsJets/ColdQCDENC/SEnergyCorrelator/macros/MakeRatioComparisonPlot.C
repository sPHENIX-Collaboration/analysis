// ----------------------------------------------------------------------------
// 'MakeRatioComparisonPlot.C'
// Derek Anderson
// 01.03.2022
//
// Use this quickly plot a set of numerator
// distributions against a set of denominator
// distributions
// ----------------------------------------------------------------------------

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
static const UInt_t NHist(2);
static const UInt_t NPlot(2);
static const UInt_t NPad(2);
static const UInt_t NVtx(4);
static const UInt_t NTxt(3);



void MakeRatioComparisonPlot() {

  // lower verbosity
  gErrorIgnoreLevel = kError;
  cout << "\n  Beginning plot macro..." << endl;

  // file parameters
  const TString sOutput("debugCorrelators.alexVsF4A_truthVsRecoAfterPhiFix_ptJet30.pp200run6jet40.d18m4y2023.root");
  const TString sInDenom[NHist] = {"reference/alex_output/pp200run6jet40.SphenixTruthUnscaled_output_trksAndChrgPars.root",
                                   "reference/alex_code/SphenixRecoUnscaled_output_derekTestRun.root"};
  const TString sInNumer[NHist] = {"./output/pp200run6jet40.forDebug_true_withPhiFix.d18m4y2023.root",
                                   "./output/pp200run6jet40.forDebug_reco_withPhiFix.d18m4y2023.root"};

  // denominator parameters
  const TString sHeadDenom("#bf{Reference Output}");
  const TString sHistDenom[NHist]  = {"EEC2",           "EEC2"};
  const TString sNameDenom[NHist]  = {"hReferencTruth", "hReferenceReco"};
  const TString sLabelDenom[NHist] = {"Truth",
                                      "Reco."};

  // numerator parameters
  const TString sHeadNumer("#bf{F4A Implementation}");
  const TString sHistNumer[NHist]  = {"hCorrelatorDrAxis_ptJet30", "hCorrelatorDrAxis_ptJet30"};
  const TString sNameNumer[NHist]  = {"hFun4AllTruth",             "hFun4AllReco"};
  const TString sLabelNumer[NHist] = {"Truth",
                                      "Reco."};

  // plot parameters
  const TString sTitle("");
  const TString sTitleX("#DeltaR");
  const TString sTitleY("EEC");
  const TString sTitleR("[f4a] / [reference]");
  const TString sNameRatio[NHist] = {"hRatioTruth", "hRatioReco"};
  const TString sOptDenom[NHist]  = {"",           "sames"};
  const TString sOptNumer[NHist]  = {"sames",      "sames"};
  const TString sOptRatio[NHist]  = {"",           "sames"};
  const TString sText[NTxt]       = {"#bf{#it{sPHENIX}} Simulation [Run6]", "JS 40 GeV Jet Sample", "p_{T}^{jet} #in (30, 50) GeV"};
  const Float_t xPlotRange[NPlot] = {0.001, 1.};
  const UInt_t  fColDen[NHist]    = {923, 634};
  const UInt_t  fColNum[NHist]    = {921, 895};
  const UInt_t  fMarDen[NHist]    = {20, 29};
  const UInt_t  fMarNum[NHist]    = {24, 30};

  // norm/rebin parameters
  const Bool_t doIntNorm(false);
  const Bool_t doRebinDenom[NHist] = {false, false};
  const Bool_t doRebinNumer[NHist] = {false, false};
  const UInt_t nRebinDenom[NHist]  = {2, 2};
  const UInt_t nRebinNumer[NHist]  = {2, 2};

  // open files
  TFile *fOutput = new TFile(sOutput.Data(), "recreate");
  if (!fOutput) {
    cerr << "PANIC: couldn't open output file!\n" << endl;
    return;
  }

  TFile *fDenom[NHist];
  TFile *fNumer[NHist];
  for (UInt_t iHist = 0; iHist < NHist; iHist++) {
    fDenom[iHist] = new TFile(sInDenom[iHist].Data(), "read");
    fNumer[iHist] = new TFile(sInNumer[iHist].Data(), "read");
    if (!fDenom[iHist] || !fNumer[iHist]) {
      cerr << "PANIC: couldn't open denominator or numerator file #" << iHist << "!\n"
           << "       fDenom = " << fDenom[iHist] << ", fNumer = " << fNumer[iHist] << "\n"
           << endl;
      return;
    }
  }
  cout << "    Opened files." << endl;

  // grab histograms
  TH1D *hDenom[NHist];
  TH1D *hNumer[NHist];
  for (UInt_t iHist = 0; iHist < NHist; iHist++) {
    hDenom[iHist] = (TH1D*) fDenom[iHist] -> Get(sHistDenom[iHist].Data());
    hNumer[iHist] = (TH1D*) fNumer[iHist] -> Get(sHistNumer[iHist].Data());
    if (!hDenom[iHist] || !hNumer[iHist]) {
      cerr << "PANIC: couldn't grab numerator or denominator histogram # " << iHist << "!\n"
           << "       hDenom = " << hDenom[iHist] << ", hNumer = " << hNumer[iHist] << "\n"
           << endl;
      return;
    }
    hDenom[iHist] -> SetName(sNameDenom[iHist].Data());
    hNumer[iHist] -> SetName(sNameNumer[iHist].Data());
  }
  cout << "    Grabbed histograms." << endl;

  // rebin histograms
  Bool_t doRebin(false);
  for (UInt_t iHist = 0; iHist < NHist; iHist++) {
    doRebin = (doRebinDenom[iHist] || doRebinNumer[iHist]);
    if (doRebin) break;
  }

  if (doRebin) {
    for (UInt_t iHist = 0; iHist < NHist; iHist++) {
      if (doRebinDenom[iHist]) hDenom[iHist] -> Rebin(nRebinDenom[iHist]);
      if (doRebinNumer[iHist]) hNumer[iHist] -> Rebin(nRebinNumer[iHist]);
    }
    cout << "    Rebinned histograms." << endl;
  }

  // normalize by integrals )if needed)
  if (doIntNorm) {
    for (UInt_t iHist = 0; iHist < NHist; iHist++) {
      const Double_t intDenom = hDenom[iHist] -> Integral();
      const Double_t intNumer = hNumer[iHist] -> Integral();
      hDenom[iHist] -> Scale(1. / intDenom);
      hNumer[iHist] -> Scale(1. / intNumer);
    }
    cout << "    Normalized histograms by integral." << endl;
  }

  // calculate ratios
  TH1D *hRatio[NHist];
  for (UInt_t iHist = 0; iHist < NHist; iHist++) {
    hRatio[iHist] = (TH1D*) hDenom[iHist] -> Clone();
    hRatio[iHist] -> Reset("ICE");
    hRatio[iHist] -> Divide(hNumer[iHist], hDenom[iHist], 1., 1.);
    hRatio[iHist] -> SetName(sNameRatio[iHist]);
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
  for (UInt_t iHist = 0; iHist < NHist; iHist++) {
    hDenom[iHist] -> SetMarkerColor(fColDen[iHist]);
    hDenom[iHist] -> SetMarkerStyle(fMarDen[iHist]);
    hDenom[iHist] -> SetFillColor(fColDen[iHist]);
    hDenom[iHist] -> SetFillStyle(fFil);
    hDenom[iHist] -> SetLineColor(fColDen[iHist]);
    hDenom[iHist] -> SetLineStyle(fLin);
    hDenom[iHist] -> SetLineWidth(fWid);
    hDenom[iHist] -> SetTitle(sTitle.Data());
    hDenom[iHist] -> SetTitleFont(fTxt);
    hDenom[iHist] -> GetXaxis() -> SetRangeUser(xPlotRange[0], xPlotRange[1]);
    hDenom[iHist] -> GetXaxis() -> SetTitle(sTitleX.Data());
    hDenom[iHist] -> GetXaxis() -> SetTitleFont(fTxt);
    hDenom[iHist] -> GetXaxis() -> SetTitleSize(fTit[1]);
    hDenom[iHist] -> GetXaxis() -> SetTitleOffset(fOffX[1]);
    hDenom[iHist] -> GetXaxis() -> SetLabelFont(fTxt);
    hDenom[iHist] -> GetXaxis() -> SetLabelSize(fLab[1]);
    hDenom[iHist] -> GetXaxis() -> CenterTitle(fCnt);
    hDenom[iHist] -> GetYaxis() -> SetTitle(sTitleY.Data());
    hDenom[iHist] -> GetYaxis() -> SetTitleFont(fTxt);
    hDenom[iHist] -> GetYaxis() -> SetTitleSize(fTit[1]);
    hDenom[iHist] -> GetYaxis() -> SetTitleOffset(fOffY[1]);
    hDenom[iHist] -> GetYaxis() -> SetLabelFont(fTxt);
    hDenom[iHist] -> GetYaxis() -> SetLabelSize(fLab[1]);
    hDenom[iHist] -> GetYaxis() -> CenterTitle(fCnt);
    hNumer[iHist] -> SetMarkerColor(fColNum[iHist]);
    hNumer[iHist] -> SetMarkerStyle(fMarNum[iHist]);
    hNumer[iHist] -> SetFillColor(fColNum[iHist]);
    hNumer[iHist] -> SetFillStyle(fFil);
    hNumer[iHist] -> SetLineColor(fColNum[iHist]);
    hNumer[iHist] -> SetLineStyle(fLin);
    hNumer[iHist] -> SetLineWidth(fWid);
    hNumer[iHist] -> SetTitle(sTitle.Data());
    hNumer[iHist] -> SetTitleFont(fTxt);
    hNumer[iHist] -> GetXaxis() -> SetRangeUser(xPlotRange[0], xPlotRange[1]);
    hNumer[iHist] -> GetXaxis() -> SetTitle(sTitleX.Data());
    hNumer[iHist] -> GetXaxis() -> SetTitleFont(fTxt);
    hNumer[iHist] -> GetXaxis() -> SetTitleSize(fTit[1]);
    hNumer[iHist] -> GetXaxis() -> SetTitleOffset(fOffX[1]);
    hNumer[iHist] -> GetXaxis() -> SetLabelFont(fTxt);
    hNumer[iHist] -> GetXaxis() -> SetLabelSize(fLab[1]);
    hNumer[iHist] -> GetXaxis() -> CenterTitle(fCnt);
    hNumer[iHist] -> GetYaxis() -> SetTitle(sTitleY.Data());
    hNumer[iHist] -> GetYaxis() -> SetTitleFont(fTxt);
    hNumer[iHist] -> GetYaxis() -> SetTitleSize(fTit[1]);
    hNumer[iHist] -> GetYaxis() -> SetTitleOffset(fOffY[1]);
    hNumer[iHist] -> GetYaxis() -> SetLabelFont(fTxt);
    hNumer[iHist] -> GetYaxis() -> SetLabelSize(fLab[1]);
    hNumer[iHist] -> GetYaxis() -> CenterTitle(fCnt);
    hRatio[iHist] -> SetMarkerColor(fColNum[iHist]);
    hRatio[iHist] -> SetMarkerStyle(fMarNum[iHist]);
    hRatio[iHist] -> SetFillColor(fColNum[iHist]);
    hRatio[iHist] -> SetFillStyle(fFil);
    hRatio[iHist] -> SetLineColor(fColNum[iHist]);
    hRatio[iHist] -> SetLineStyle(fLin);
    hRatio[iHist] -> SetLineWidth(fWid);
    hRatio[iHist] -> SetTitle(sTitle.Data());
    hRatio[iHist] -> SetTitleFont(fTxt);
    hRatio[iHist] -> GetXaxis() -> SetRangeUser(xPlotRange[0], xPlotRange[1]);
    hRatio[iHist] -> GetXaxis() -> SetTitle(sTitleX.Data());
    hRatio[iHist] -> GetXaxis() -> SetTitleFont(fTxt);
    hRatio[iHist] -> GetXaxis() -> SetTitleSize(fTit[0]);
    hRatio[iHist] -> GetXaxis() -> SetTitleOffset(fOffX[0]);
    hRatio[iHist] -> GetXaxis() -> SetLabelFont(fTxt);
    hRatio[iHist] -> GetXaxis() -> SetLabelSize(fLab[0]);
    hRatio[iHist] -> GetXaxis() -> CenterTitle(fCnt);
    hRatio[iHist] -> GetYaxis() -> SetTitle(sTitleR.Data());
    hRatio[iHist] -> GetYaxis() -> SetTitleFont(fTxt);
    hRatio[iHist] -> GetYaxis() -> SetTitleSize(fTit[0]);
    hRatio[iHist] -> GetYaxis() -> SetTitleOffset(fOffY[0]);
    hRatio[iHist] -> GetYaxis() -> SetLabelFont(fTxt);
    hRatio[iHist] -> GetYaxis() -> SetLabelSize(fLab[0]);
    hRatio[iHist] -> GetYaxis() -> CenterTitle(fCnt);
  }
  cout << "    Set styles." << endl;

  // make legend
  const UInt_t  fColLe(0);
  const UInt_t  fFilLe(0);
  const UInt_t  fLinLe(0);
  const UInt_t  nObjLe(2 * (NHist + 1));
  const Float_t hObjLe(nObjLe * 0.05);
  const Float_t yObjLe(0.1 + hObjLe);
  const Float_t fLegXY[NVtx] = {0.1, 0.1, 0.3, yObjLe};

  TLegend *leg = new TLegend(fLegXY[0], fLegXY[1], fLegXY[2], fLegXY[3]);
  leg -> SetFillColor(fColLe);
  leg -> SetFillStyle(fFilLe);
  leg -> SetLineColor(fColLe);
  leg -> SetLineStyle(fLinLe);
  leg -> SetTextFont(fTxt);
  leg -> SetTextAlign(fAln);
  leg -> AddEntry((TObject*)0, sHeadDenom.Data(), "");
  for (UInt_t iHist = 0; iHist < NHist; iHist++) {
    leg -> AddEntry(hDenom[iHist], sLabelDenom[iHist], "pf");
  }
  leg -> AddEntry((TObject*)0, sHeadNumer.Data(), "");
  for (UInt_t iHist = 0; iHist < NHist; iHist++) {
    leg -> AddEntry(hNumer[iHist], sLabelNumer[iHist], "pf");
  }
  cout << "    Made legend." << endl;

  // make text
  const UInt_t  fColTx(0);
  const UInt_t  fFilTx(0);
  const UInt_t  fLinTx(0);
  const UInt_t  nObjTx(NTxt);
  const Float_t hObjTx(nObjTx * 0.05);
  const Float_t yObjTx(0.1 + hObjTx);
  const Float_t fTxtXY[NVtx] = {0.3, 0.1, 0.5, yObjTx};

  TPaveText *txt = new TPaveText(fTxtXY[0], fTxtXY[1], fTxtXY[2], fTxtXY[3], "NDC NB");
  txt -> SetFillColor(fColTx);
  txt -> SetFillStyle(fFilTx);
  txt -> SetLineColor(fColTx);
  txt -> SetLineStyle(fLinTx);
  txt -> SetTextFont(fTxt);
  txt -> SetTextAlign(fAln);
  for (UInt_t iTxt = 0; iTxt < NTxt; iTxt++) {
    txt -> AddText(sText[iTxt].Data());
  }
  cout << "    Made text." << endl;

  // make line
  const UInt_t  fColLi(923);
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
  const UInt_t  fLogY1(1);
  const UInt_t  fLogY2(1);
  const UInt_t  fFrame(0);
  const Float_t fMarginL(0.15);
  const Float_t fMarginR(0.02);
  const Float_t fMarginT1(0.005);
  const Float_t fMarginT2(0.02);
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
  for (UInt_t iHist = 1; iHist < NHist; iHist++) {
    hRatio[iHist] -> Draw(sOptRatio[iHist].Data());
  }
  line      -> Draw();
  pPad2     -> cd();
  hDenom[0] -> Draw(sOptDenom[0].Data());
  hNumer[0] -> Draw(sOptNumer[0].Data());
  for(UInt_t iHist = 1; iHist < NHist; iHist++) {
    hDenom[iHist] -> Draw(sOptDenom[iHist].Data());
    hNumer[iHist] -> Draw(sOptNumer[iHist].Data());
  }
  leg     -> Draw();
  txt     -> Draw();
  fOutput -> cd();
  cPlot   -> Write();
  cPlot   -> Close();

  TCanvas *cPlotNR = new TCanvas("cPlot_NoRatio", "", width, heightNR);
  cPlotNR   -> SetGrid(fGrid, fGrid);
  cPlotNR   -> SetTicks(fTick, fTick);
  cPlotNR   -> SetLogx(fLogX);
  cPlotNR   -> SetLogy(fLogY2);
  cPlotNR   -> SetBorderMode(fMode);
  cPlotNR   -> SetBorderSize(fBord);
  cPlotNR   -> SetFrameBorderMode(fFrame);
  cPlotNR   -> SetLeftMargin(fMarginL);
  cPlotNR   -> SetRightMargin(fMarginR);
  cPlotNR   -> SetTopMargin(fMarginT1);
  cPlotNR   -> SetBottomMargin(fMarginBNR);
  hDenom[0] -> Draw(sOptDenom[0].Data());
  hNumer[0] -> Draw(sOptNumer[0].Data());
  for(UInt_t iHist = 1; iHist < NHist; iHist++) {
    hDenom[iHist] -> Draw(sOptDenom[iHist].Data());
    hNumer[iHist] -> Draw(sOptNumer[iHist].Data());
  }
  leg     -> Draw();
  txt     -> Draw();
  fOutput -> cd();
  cPlotNR -> Write();
  cPlotNR -> Close();
  cout << "    Made plot." << endl;

  // save histograms
  fOutput -> cd();
  for (UInt_t iHist = 0; iHist < NHist; iHist++) {
    hDenom[iHist] -> Write();
    hNumer[iHist] -> Write();
    hRatio[iHist] -> Write();
  }
  cout << "    Saved histograms." << endl;

  // close files
  fOutput -> cd();
  fOutput -> Close();
  for (UInt_t iHist = 0; iHist < NHist; iHist++) {
    fDenom[iHist] -> cd();
    fDenom[iHist] -> Close();
    fNumer[iHist] -> cd();
    fNumer[iHist] -> Close();
  }
  cout << "  Finished plot!\n" << endl;

}

// end ------------------------------------------------------------------------
