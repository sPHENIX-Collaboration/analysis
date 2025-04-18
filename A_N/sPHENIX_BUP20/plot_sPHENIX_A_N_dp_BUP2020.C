//need to run make_ggg_contribution_graphs.C first

#include <fstream>
#include <iostream>
#include "stdio.h"

#include <TChain.h>
#include <TFile.h>
#include <TGraphAsymmErrors.h>
#include <TGraphErrors.h>
#include <TLatex.h>
#include <TLegend.h>
#include <TLine.h>
#include <TMath.h>
#include <TROOT.h>
#include <TString.h>
#include <TTree.h>
#include <TVectorD.h>
#include <TVirtualFitter.h>
#include <cassert>
#include <cmath>
#include <vector>

#include "SaveCanvas.C"
#include "sPhenixStyle.C"

const int NUM_PT_BINS = 4;
const float BIN_CENTERS[NUM_PT_BINS] = {5.39, 6.69, 8.77, 11.9};
const float ASYMMETRY[NUM_PT_BINS] =
    {0, 0, 0, 0};
const float STAT_ERR[NUM_PT_BINS] =
    {0.0010, 0.0013, 0.0025, 0.0035};

//https://arxiv.org/abs/2002.08384
//https://arxiv.org/abs/1410.3448
const int NUM_QGQ_PT_BINS = 12;
const float PT[NUM_QGQ_PT_BINS] =
    {4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
const float QGQ[NUM_QGQ_PT_BINS] =
    {-0.000500, -0.000434, -0.000339, -0.000213, -0.000054, 0.000137, 0.000357,
     0.000603, 0.000874, 0.001166, 0.001477, 0.001804};
const float QGQ_ERR[NUM_QGQ_PT_BINS] =
    {0.000077, 0.000070, 0.000072, 0.000079, 0.000091, 0.000113, 0.000146,
     0.000191, 0.000250, 0.000321, 0.000405, 0.000501};

const double pp_inelastic_crosssec = 42e-3;        // 42 mb [sPH-TRG-000]
const double pp_rec_3year = 62e12;                 // BUP2020
const double pp_rec_5year = pp_rec_3year + 80e12;  // BUP2020
const double pp_beam_pol = 0.57;

TGraphErrors *GraphShiftScaling(TGraphErrors *gr_src, const double x_shift, const double err_scaling)
{
  assert(gr_src);

  const int npoint = gr_src->GetN();

  TVectorD vx(npoint);
  TVectorD vy(npoint);
  TVectorD vex(npoint);
  TVectorD vey(npoint);

  int nfilled = 0;
  for (int i = 0; i < npoint; ++i)
  {
    const double &x = gr_src->GetX()[i];
    //        if (x<x_min or x>x_max) continue;

    vx[nfilled] = x + x_shift;
    vy[nfilled] = gr_src->GetY()[i];
    vex[nfilled] = gr_src->GetEX()[i];
    vey[nfilled] = gr_src->GetEY()[i] * err_scaling;

    ++nfilled;
  }

  TGraphErrors *gr = new TGraphErrors(nfilled, vx.GetMatrixArray(), vy.GetMatrixArray(),
                                      vex.GetMatrixArray(), vey.GetMatrixArray());

  gr->SetMarkerColor(gr_src->GetMarkerColor());
  gr->SetMarkerStyle(gr_src->GetMarkerStyle());
  gr->SetMarkerSize(gr_src->GetMarkerSize());
  gr->SetLineWidth(gr_src->GetLineWidth());
  gr->SetLineColor(gr_src->GetLineColor());

  return gr;
}

void plot_sPHENIX_A_N_dp_BUP2020()
{
  SetsPhenixStyle();

  const double ref_Lum = 100e12;
  const double err_scale = sqrt(ref_Lum / pp_rec_3year);

  //  TAxis *xAxis = frame->GetXaxis();
  //  xAxis->SetTitleSize(0.05);
  //  xAxis->SetLabelSize(0.055);
  //  xAxis->SetTitleOffset(1.2);
  //  TAxis *yAxis = frame->GetYaxis();
  //  yAxis->SetTitleSize(0.07);
  //  yAxis->SetLabelSize(0.055);
  //  yAxis->CenterTitle(true);
  //  yAxis->SetTitleOffset(0.8);
  //  yAxis->SetNdivisions(504);  //310 );

  TGraphErrors *projectedMeasurement_ref =
      new TGraphErrors(NUM_PT_BINS, BIN_CENTERS, ASYMMETRY, 0, STAT_ERR);

  TGraphErrors *projectedMeasurement =
      GraphShiftScaling(projectedMeasurement_ref, 0, err_scale);
  projectedMeasurement->SetMarkerStyle(kFullCircle);
  projectedMeasurement->SetMarkerSize(2);
  projectedMeasurement->SetLineWidth(4);
  projectedMeasurement->SetLineColorAlpha(kBlack, 1);

  TF1 *zeroLine = new TF1("zeroLine", "0", 0, 20);
  zeroLine->SetLineColor(kBlack);
//  zeroLine->SetLineStyle(2);

  TFile *trigluonFile = TFile::Open("gggContribution.root");
  //https://arxiv.org/abs/1112.1161
  TGraphErrors *ggg1 = (TGraphErrors *) trigluonFile->Get("model1");
  ggg1->SetFillStyle(3005);
  ggg1->SetFillColor(kRed);
  ggg1->SetLineColor(kWhite);
  TGraphErrors *ggg2 = (TGraphErrors *) trigluonFile->Get("model2");
  ggg2->SetFillStyle(3004);
  ggg2->SetFillColor(kBlue);
  ggg2->SetLineColor(kWhite);

  TGraphErrors *qgq = new TGraphErrors(NUM_QGQ_PT_BINS, PT, QGQ,
                                       0, QGQ_ERR);
  qgq->SetFillColor(kGreen + 3);
  qgq->SetFillStyle(3003);
  qgq->SetLineColor(kGreen + 2);
  qgq->SetLineWidth(5);

  TCanvas *c1 = new TCanvas("AN_dp_sPHENIX", "AN_dp_sPHENIX", 1100, 800);
  c1->Divide(1, 1);
  int idx = 1;
  TPad *p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();

  p->DrawFrame(4.69, -0.015, 12.6, 0.015)->SetTitle(";#it{p}_{T} [GeV];A_{N}");

  //  TH1 *frame = new TH2F("frame", " ", 1, 4.69, 12.6, 1, -0.018, 0.015);
  //  frame->SetTitle("; p_{T} [GeV/c]; A_{N}");
  //  frame->SetStats(false);

  //  TCanvas *c1 = new TCanvas("c11", "with QGQ", 200, 200, 800, 500);
  //  gPad->SetBottomMargin(0.15);
  //  gPad->SetLeftMargin(0.15);

  //  frame->Draw("");
  ggg1->Draw("3");
  ggg2->Draw("3");
  qgq->Draw("C3");
  projectedMeasurement->Draw("P");
  zeroLine->Draw("same");

  //  TLatex text1;
  //  text1.SetTextSize(0.05);
  //  //  text1.SetTextAlign( 13 );
  //  text1.DrawLatex(5, 0.012, "p^{#uparrow} + p #rightarrow #gamma + X");
  //  text1.DrawLatex(5.25, 0.0095, "#sqrt{s} = 200 GeV");
  //  text1.DrawLatex(10, 0.012, "sPHENIX:");
  //  text1.DrawLatex(10, 0.0095, "L=100/pb P=0.57");

  TLegend *leg = new TLegend(.2, .75, .83, .9);
  leg->SetFillStyle(0);
  //     leg->AddEntry("", "#it{#bf{sPHENIX}} Projection", "");
  leg->AddEntry("", Form("#it{#bf{sPHENIX}} Projection, Years 1-3"), "");
  leg->AddEntry("", Form("%.0f pb^{-1} samp. #it{p}^{#uparrow}+#it{p}#rightarrow #gamma + X, P=%.2f", pp_rec_3year / 1e12, pp_beam_pol), "");
  leg->Draw();

  leg = new TLegend(0.18, 0.18, 0.5, 0.41);
  leg->AddEntry(qgq, "qgq Contribution (D.Pitonyak)", "l");
  leg->AddEntry(ggg1, "Trigluon Contribution Model 1 (S.Yoshida)", "f");
  leg->AddEntry(ggg2, "Trigluon Contribution Model 2 (S.Yoshida)", "f");
//  leg->SetBorderSize(0);
//  leg->SetTextSize(0.04);
  leg->Draw();

  SaveCanvas(c1, TString(c1->GetName()), kTRUE);
    c1->SaveAs("AN_dp_sPHENIX.pdf");
}
