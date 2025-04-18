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
#include <TROOT.h>
#include <TMath.h>
#include <TString.h>
#include <TTree.h>
#include <TVectorD.h>
#include <TVirtualFitter.h>
#include <cassert>
#include <cmath>
#include <vector>

#include "SaveCanvas.C"
#include "sPhenixStyle.C"

using namespace std;

Double_t v1_err(Double_t sig, Double_t v1, Double_t Res)
{
  const Double_t Pi = 3.1415927;
  return 2.3 / sig * sqrt(1 - 16 * v1 * v1 / Pi / Pi) / Res;  // 2.3 - coefficient from simple two rapidity bin linear fit
}

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

void makeV1_BUP2020(const Bool_t sPHENIX = 1)
{
  //  gROOT->LoadMacro("sPhenixStyle.C");
  SetsPhenixStyle();

  const Double_t MassD = 1.86484;

  // STAR data points
  const Int_t NS = 4;
  const Double_t offset = 0.02;
  Double_t yp[NS], v1p[NS], v1pe[NS], v1pes[NS];
  Double_t yn[NS], v1n[NS], v1ne[NS], v1nes[NS];
  ifstream inData;
  inData.open("D0v1_STAR_final.txt");
  for (int i = 0; i < NS; i++)
  {
    inData >> yp[i] >> v1p[i] >> v1pe[i] >> v1pes[i];
    yp[i] -= offset;
  }
  inData.close();
  inData.open("D0barv1_STAR_final.txt");
  for (int i = 0; i < NS; i++)
  {
    inData >> yn[i] >> v1n[i] >> v1ne[i] >> v1nes[i];
    yn[i] += offset;
  }
  inData.close();

  //  TGraphErrors *gr_D_STAR = new TGraphErrors("D0v1_STAR_final.txt","%lg %lg %lg");
  TGraphErrors *gr_D_STAR = new TGraphErrors(NS, yp, v1p, 0, v1pe);
  gr_D_STAR->SetMarkerColor(kRed - 6);
  gr_D_STAR->SetMarkerStyle(kFullStar);
  gr_D_STAR->SetMarkerSize(2);
  gr_D_STAR->SetLineWidth(4);
  gr_D_STAR->SetLineColor(kRed - 6);

  //  TGraphErrors *gr_Dbar_STAR = new TGraphErrors("D0barv1_STAR_final.txt","%lg %lg %lg");
  TGraphErrors *gr_Dbar_STAR = new TGraphErrors(NS, yn, v1n, 0, v1ne);
  gr_Dbar_STAR->SetMarkerColor(kRed - 6);
  gr_Dbar_STAR->SetMarkerStyle(kOpenStar);
  gr_Dbar_STAR->SetMarkerSize(2);
  gr_Dbar_STAR->SetLineWidth(4);
  gr_Dbar_STAR->SetLineColor(kRed - 6);

  TFile *fin = new TFile("D0_significance.root");
  TGraph *gr_sig_D = (TGraph *) fin->Get("gProD0_0_80_noPid");
  double sig2_tot = 0.;
  for (int i = 0; i < gr_sig_D->GetN(); i++)
  {
    sig2_tot += TMath::Power(gr_sig_D->GetY()[i], 2.0);
  }
  double sig_tot = sqrt(sig2_tot);
  cout << " Total D/Dbar significance in |y|<1 = " << sig_tot << endl;
  double sig_tot_D = sig_tot / sqrt(2.);  // one charge sign

  const Double_t EPRes = 0.37;  // STAR 1st EP resolution

  // theory curves
  // Greco - D/Dbar v1 difference
  TGraph *gr_D_Greco = new TGraph("D0v1_RHIC_Greco.txt", "%lg %lg");
  TGraph *gr_Dbar_Greco = new TGraph("D0barv1_RHIC_Greco.txt", "%lg %lg");           // be careful, this calculation has a sign flip
  TGraph *gr_DDbar_Bozek_tmp = new TGraph("DDbarv1_eta_1712.01180.txt", "%lg %lg");  // v1 vs eta, in percentage
  double x[100], y[100];
  for (int i = 0; i < gr_DDbar_Bozek_tmp->GetN(); i++)
  {
    double eta = gr_DDbar_Bozek_tmp->GetX()[i];
    x[i] = eta;  // questionable!!
    y[i] = gr_DDbar_Bozek_tmp->GetY()[i] / 100.;
  }
  TGraph *gr_DDbar_Bozek = new TGraph(gr_DDbar_Bozek_tmp->GetN(), x, y);
  cout << " AAAA " << endl;

  // for projections v1
  const Int_t N_D = 8;
  double y_D[N_D], v1_D[N_D], v1_Dbar[N_D], v1_err_D[N_D], v1_err_Dbar[N_D];

  for (int i = 0; i < N_D; i++)
  {
    y_D[i] = (i + 0.5) * 2.0 / N_D - 1.0;
    v1_D[i] = -1.0 * gr_D_Greco->Eval(y_D[i]) + gr_DDbar_Bozek->Eval(y_D[i]);  // there is a sign flip in Greco's calculation
    v1_Dbar[i] = -1.0 * gr_Dbar_Greco->Eval(y_D[i]) + gr_DDbar_Bozek->Eval(y_D[i]);

    double sig_per_bin = sig_tot_D / sqrt((double) N_D);  // //not combine symmetric rapidity bins
    v1_err_D[i] = v1_err(sig_per_bin, v1_D[i], EPRes);
    v1_err_Dbar[i] = v1_err(sig_per_bin, v1_Dbar[i], EPRes);
    cout << y_D[i] << " " << v1_D[i] << "+/-" << v1_err_D[i] << endl;
  }

  TGraph *gr_D = new TGraph(N_D, y_D, v1_D);
  gr_D->SetMarkerSize(2);
  gr_D->SetMarkerColor(1);
  gr_D->SetMarkerStyle(20);
  gr_D->SetLineWidth(2.);
  gr_D->SetLineStyle(1);
  gr_D->SetLineColor(1);

  TGraph *gr_Dbar = new TGraph(N_D, y_D, v1_Dbar);
  gr_Dbar->SetMarkerSize(2);
  gr_Dbar->SetMarkerColor(4);
  gr_Dbar->SetMarkerStyle(24);
  gr_Dbar->SetLineWidth(2.);
  gr_Dbar->SetLineStyle(1);
  gr_Dbar->SetLineColor(4);

  TGraphErrors *gr_D_proj = new TGraphErrors(N_D / 2, y_D + 4, v1_D + 4, 0, v1_err_D + 4);
  gr_D_proj->SetMarkerSize(2);
  gr_D_proj->SetMarkerColor(1);
  gr_D_proj->SetMarkerStyle(20);
  gr_D_proj->SetLineWidth(4.);
  gr_D_proj->SetLineStyle(1);
  gr_D_proj->SetLineColor(1);

  TGraphErrors *gr_D_proj_2 = new TGraphErrors(N_D / 2, y_D, v1_D, 0, v1_err_D);
  gr_D_proj_2->SetMarkerSize(2);
  gr_D_proj_2->SetMarkerColor(1);
  gr_D_proj_2->SetMarkerStyle(20);
  gr_D_proj_2->SetLineWidth(4.);
  gr_D_proj_2->SetLineStyle(1);
  gr_D_proj_2->SetLineColor(1);

  TGraphErrors *gr_Dbar_proj = new TGraphErrors(N_D / 2, y_D + 4, v1_Dbar + 4, 0, v1_err_Dbar + 4);
  gr_Dbar_proj->SetMarkerSize(2);
  gr_Dbar_proj->SetMarkerColor(4);
  gr_Dbar_proj->SetMarkerStyle(20);
  gr_Dbar_proj->SetLineWidth(4.);
  gr_Dbar_proj->SetLineStyle(1);
  gr_Dbar_proj->SetLineColor(4);

  TGraphErrors *gr_Dbar_proj_2 = new TGraphErrors(N_D / 2, y_D, v1_Dbar, 0, v1_err_Dbar);
  gr_Dbar_proj_2->SetMarkerSize(2);
  gr_Dbar_proj_2->SetMarkerColor(4);
  gr_Dbar_proj_2->SetMarkerStyle(20);
  gr_Dbar_proj_2->SetLineWidth(4.);
  gr_Dbar_proj_2->SetLineStyle(1);
  gr_Dbar_proj_2->SetLineColor(4);

  ///////////////////////////////////////////////////////////////////////////
  //  BUP2020  lumi scaling
  ///////////////////////////////////////////////////////////////////////////

  const double refAuAuMB = 240e9;
  const double refAuAuXSec = 6.8252;  // b

  const double AuAu_Ncoll_C0_10 = 960.2;   // [DOI:?10.1103/PhysRevC.87.034911?]
  const double AuAu_Ncoll_C0_20 = 770.6;   // [DOI:?10.1103/PhysRevC.91.064904?]
  const double AuAu_Ncoll_60_70 = 29.8;    //PHYSICAL REVIEW C 87, 034911 (2013)
  const double AuAu_Ncoll_70_80 = 12.6;    //PHYSICAL REVIEW C 87, 034911 (2013)
  const double AuAu_Ncoll_C0_100 = 238.5;  // BUP2020
  const double pAu_Ncoll_C0_100 = 4.7;     // pb^-1 [sPH-TRG-000]

  const double AuAu_rec_3year = (5.7 + 15) * 1e9;       // BUP2020
  const double AuAu_rec_5year = AuAu_rec_3year + 30e9;  // BUP2020

  const double error_scale_3yr = sqrt((refAuAuMB) / (AuAu_rec_3year * refAuAuXSec));
  const double error_scale_5yr = sqrt((refAuAuMB) / (AuAu_rec_5year * refAuAuXSec));

  TGraphErrors *gr_D_proj_3yr = GraphShiftScaling(gr_D_proj, 0.05, error_scale_3yr);
  TGraphErrors *gr_D_proj_2_3yr = GraphShiftScaling(gr_D_proj_2, 0.05, error_scale_3yr);
  TGraphErrors *gr_Dbar_proj_3yr = GraphShiftScaling(gr_Dbar_proj, 0.05, error_scale_3yr);
  TGraphErrors *gr_Dbar_proj_2_3yr = GraphShiftScaling(gr_Dbar_proj_2, 0.05, error_scale_3yr);

  TGraphErrors *gr_D_proj_5yr = GraphShiftScaling(gr_D_proj, 0., error_scale_5yr);
  TGraphErrors *gr_D_proj_2_5yr = GraphShiftScaling(gr_D_proj_2, 0., error_scale_5yr);
  TGraphErrors *gr_Dbar_proj_5yr = GraphShiftScaling(gr_Dbar_proj, 0., error_scale_5yr);
  TGraphErrors *gr_Dbar_proj_2_5yr = GraphShiftScaling(gr_Dbar_proj_2, 0., error_scale_5yr);

  gr_D_proj_3yr->SetMarkerStyle(kOpenCircle);
  gr_D_proj_2_3yr->SetMarkerStyle(kOpenCircle);
  gr_Dbar_proj_3yr->SetMarkerStyle(kOpenCircle);
  gr_Dbar_proj_2_3yr->SetMarkerStyle(kOpenCircle);

  gr_D_proj_3yr->SetMarkerColor(kGray + 2);
  gr_D_proj_2_3yr->SetMarkerColor(kGray + 2);
  gr_Dbar_proj_3yr->SetMarkerColor(kBlue - 6);
  gr_Dbar_proj_2_3yr->SetMarkerColor(kBlue - 6);

  gr_D_proj_3yr->SetLineColor(kGray + 2);
  gr_D_proj_2_3yr->SetLineColor(kGray + 2);
  gr_Dbar_proj_3yr->SetLineColor(kBlue - 6);
  gr_Dbar_proj_2_3yr->SetLineColor(kBlue - 6);

  //  TCanvas *c1 = new TCanvas("v1_5y_compare", "v1_5y_compare", 0, 0, 800, 600);

  TCanvas *c1 = new TCanvas("D0_BUP2020_AuAu_v1_5y_compare", "D0_BUP2020_AuAu_v1_5y_compare", 1100, 800);
  c1->Divide(1, 1);
  int idx = 1;
  TPad *p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  // gStyle->SetOptFit(0);
  // gStyle->SetOptStat(0);
  // gStyle->SetEndErrorSize(0.01);
  // c1->SetFillColor(10);
  // c1->SetBorderMode(0);
  // c1->SetBorderSize(2);
  // c1->SetFrameFillColor(0);
  // c1->SetFrameBorderMode(0);

  // // c1->SetGridx();
  // // c1->SetGridy();
  // c1->SetTickx();
  // c1->SetTicky();

  // //  c1->SetLogx();
  // //  c1->SetLogy();

  // c1->SetLeftMargin(0.13);
  // c1->SetBottomMargin(0.16);
  // c1->SetTopMargin(0.02);
  // c1->SetRightMargin(0.06);

  double x1 = -1.2;
  double x2 = 1.2;
  double y1 = -0.1;
  double y2 = 0.1;

  TH1D *d0 = new TH1D("d0", "", 1, x1, x2);
  d0->SetMinimum(y1);
  d0->SetMaximum(y2);
  // d0->GetXaxis()->SetNdivisions(208);
  // d0->GetXaxis()->CenterTitle();
  d0->GetXaxis()->SetTitle("Rapidity");
  // d0->GetXaxis()->SetTitleOffset(1.2);
  // d0->GetXaxis()->SetTitleSize(0.06);
  // d0->GetXaxis()->SetLabelOffset(0.01);
  // d0->GetXaxis()->SetLabelSize(0.045);
  // d0->GetXaxis()->SetLabelFont(42);
  // d0->GetXaxis()->SetTitleFont(42);
  // d0->GetYaxis()->SetNdivisions(505);
  // d0->GetYaxis()->CenterTitle();
  d0->GetYaxis()->SetTitle("v_{1}");
  // d0->GetYaxis()->SetTitleOffset(1.0);
  // d0->GetYaxis()->SetTitleSize(0.06);
  // d0->GetYaxis()->SetLabelOffset(0.005);
  // d0->GetYaxis()->SetLabelSize(0.045);
  // d0->GetYaxis()->SetLabelFont(42);
  // d0->GetYaxis()->SetTitleFont(42);
  // d0->SetLineWidth(2);
  d0->Draw("c");

  TLine *l0 = new TLine(x1, 0, x2, 0);
  l0->SetLineWidth(1);
  l0->SetLineColor(kBlack);
  l0->SetLineStyle(1);
  l0->Draw("same");

  gr_D_STAR->Draw("p");
  gr_Dbar_STAR->Draw("p");

  if (sPHENIX)
  {
    gr_D->Draw("c");
    gr_Dbar->Draw("c");

    gr_D_proj_2_3yr->Draw("p");
    gr_Dbar_proj_2_3yr->Draw("p");

    gr_D_proj_3yr->Draw("p");
    gr_Dbar_proj_3yr->Draw("p");

    gr_D_proj_2_5yr->Draw("p");
    gr_Dbar_proj_2_5yr->Draw("p");

    gr_D_proj_5yr->Draw("p");
    gr_Dbar_proj_5yr->Draw("p");
  }

  TLegend *leg = new TLegend(.3, .77, .9, .9);
  leg->SetFillStyle(0);
  leg->AddEntry("", "#it{#bf{sPHENIX}} Projection, Years 1-5", "");
  leg->AddEntry("", ("0-80% Au+Au, Res(#Psi_{1})=0.37"), "");
  //  leg->AddEntry("", Form("%.0f nb^{-1} str. O+O", OO_rec_5year/1e9), "");
  //  leg->AddEntry("", Form("%.0f nb^{-1} str. Ar+Ar", ArAr_rec_5year/1e9), "");
  leg->Draw();

  leg = new TLegend(.2, .18, .4, .4, "D^{0}");
  leg->SetFillStyle(0);
  leg->AddEntry(gr_D_proj_3yr, " ", "pe");
  leg->AddEntry(gr_D_proj_5yr, " ", "pe");
  leg->AddEntry(gr_D_STAR, " ", "pe");
  leg->Draw();

  leg = new TLegend(.25, .18, .5, .4, " #bar{D}^{0}");
  leg->SetFillStyle(0);
  leg->AddEntry(gr_Dbar_proj_3yr, Form("sPHENIX, Year 1-3, %.0fnb^{-1} rec. Au+Au", AuAu_rec_3year / 1e9), "pe");
  leg->AddEntry(gr_Dbar_proj_5yr, Form("sPHENIX, Year 1-5, %.0fnb^{-1} rec.+str. Au+Au", AuAu_rec_5year / 1e9), "pe");
  leg->AddEntry(gr_Dbar_STAR, "STAR, 10-80% Au+Au, PRL#bf{123}, 162301", "pe");
  leg->Draw();

  //  TLegend *leg = new TLegend(0.12, 0.2, 0.52, 0.4);
  //  leg->SetFillColor(10);
  //  leg->SetFillStyle(10);
  //  leg->SetLineStyle(4000);
  //  leg->SetLineColor(10);
  //  leg->SetLineWidth(0.);
  //  leg->SetTextFont(42);
  //  leg->SetTextSize(0.045);
  //  leg->AddEntry("", "#it{#bf{sPHENIX}} Simulation", "");
  //  leg->AddEntry("", "Au+Au #sqrt{s_{NN}}=200 GeV", "");
  //  leg->AddEntry("", "240B MB, EPRes=0.37", "");
  //  if (sPHENIX) leg->Draw();
  //
  //  //  TLegend *leg;
  //  if (sPHENIX)
  //    leg = new TLegend(0.62, 0.66, 0.98, 0.92);
  //  else
  //    leg = new TLegend(0.62, 0.79, 0.98, 0.92);
  //  leg->SetFillColor(10);
  //  leg->SetFillStyle(10);
  //  leg->SetLineStyle(4000);
  //  leg->SetLineColor(10);
  //  leg->SetLineWidth(0.);
  //  leg->SetTextFont(42);
  //  leg->SetTextSize(0.04);
  //  leg->AddEntry(gr_D_STAR, "D^{0} - STAR", "pl");
  //  leg->AddEntry(gr_Dbar_STAR, "#bar{D}^{0} - STAR", "pl");
  //  if (sPHENIX)
  //  {
  //    leg->AddEntry(gr_D_proj, "D^{0} - sPHENIX proj.", "pl");
  //    leg->AddEntry(gr_Dbar_proj, "#bar{D}^{0} - sPHENIX proj.", "pl");
  //  }
  //  // leg->AddEntry(gr_proj_D,"Uncert. of D^{0}","p");
  //  // leg->AddEntry(gr_proj_D_B,"Uncert. of D^{0} from B","p");
  //  leg->Draw();

  //  c1->SaveAs(Form("fig/v1_proj_240B_%d.pdf", sPHENIX));
  //  c1->SaveAs(Form("fig/v1_proj_240B_%d.png", sPHENIX));

  SaveCanvas(c1, "fig_BUP2020/" + TString(c1->GetName()), kTRUE);
}
