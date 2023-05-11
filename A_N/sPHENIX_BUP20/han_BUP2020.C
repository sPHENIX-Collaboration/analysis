
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

const double pp_inelastic_crosssec = 42e-3;        // 42 mb [sPH-TRG-000]
const double pp_rec_3year = 6.2e12;                 // BUP2020
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

void draw_phenix(float an)
{
  const int nn = 1;
  float xx[nn] = {4.4};
  float sx[nn] = {2.6};
  float yy[nn] = {3.14e-2};
  //  float yy[nn] = {1e-2};
  float sy[nn] = {0.37e-2};

  yy[0] = an;

  TGraphErrors *gr = new TGraphErrors(nn, xx, yy, sx, sy);
  gr->SetFillColor(3);
  gr->SetFillStyle(3001);
  //  gr->Draw("A2");
  //  gr->Draw("P");

  int icol = 29;
  int iw = 10;       // Line width
  float wa = 0.025;  // Arrow width
  TArrow *arx = new TArrow(xx[0] - sx[0], yy[0], xx[0] + sx[0], yy[0], wa, "<|>");
  arx->SetLineWidth(iw);
  arx->SetLineColor(icol);
  arx->SetFillColor(icol);
  arx->Draw();

  TArrow *ary = new TArrow(xx[0], yy[0] - sy[0], xx[0], yy[0] + sy[0], wa, "<|>");
  ary->SetLineWidth(iw);
  ary->SetLineColor(icol);
  ary->SetFillColor(icol);
  ary->Draw();
}

void han_BUP2020()
{
  SetsPhenixStyle();

  const double ref_Lum = 10e12;
  const double err_scale = sqrt(ref_Lum / pp_rec_3year);

  const int nxf = 20;
  TFile *f = new TFile("han.root");
  TGraphErrors *gan[nxf];

  const float dy = 0.01;  // to plot different xF bins

  char hname[100];
  for (int ix = 0; ix < nxf; ix++)
  {
    sprintf(hname, "gan%d", ix);
    gan[ix] =
        GraphShiftScaling((TGraphErrors *) f->Get(hname), 0, err_scale);
  }

  //  gStyle->SetOptStat(0);
  TCanvas *c1 = new TCanvas("sphenix_han", "sphenix_han", 1100, 800);
  //  c3->Range(0,0,1,1);
  //  pad31 = new TPad("pad31","This is pad1",0.0,0.0,1.0,1.0);
  //  pad31->Draw();
  c1->Divide(1, 1);
  int idx = 1;
  TPad *p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();

  p->DrawFrame(0, -0., 12, 0.065)->SetTitle(";#it{p}_{T} [GeV];A_{N}");

  //  TH1F* h = new TH1F("h"," ",10,0,11);
  //  //  h->SetTitle("#pi^{+} #delta(A_{N}) proj.");
  //  h->SetYTitle("A_{N}");
  //  h->SetXTitle("p_{T} (GeV/c)");
  //  h->SetMinimum(0.);
  //  h->SetMaximum(0.053);

  //  pad31->cd();
  //  h->Draw();
  draw_phenix(0.025);
  TLatex *tex = new TLatex();
  tex->SetTextSize(0.035);
  char str[100];
  float xfbin = 1. / float(nxf);
  for (int i = 1; i <= 4; i++)
  {
    gan[i]->SetMarkerStyle(20);
    gan[i]->SetMarkerColor(kRed + 1);
    gan[i]->SetLineColor(kRed + 1);
    gan[i]->SetMarkerSize(2);
    gan[i]->SetLineWidth(4);

    gan[i]->Draw("P");
    sprintf(str, "x_{F}=%4.2f-%4.2f", xfbin * i, xfbin * (i + 1));
    tex->DrawLatex(9.5, dy * i - 0.0003, str);
  }

  //  tex->DrawLatex(0.4, 0.048, "sPHENIX:");
  //  tex->DrawLatex(0.4, 0.044, "p+p, #sqrt{s}=200 GeV");
  //  tex->DrawLatex(0.4, 0.040, "L=10/pb P=0.57");
//      tex->DrawLatex(0.4,0.0265,"0<#eta<3");
    tex->DrawLatex(0.4, 0.0025, "Shown only points with #delta(A_{N}) <~ 1%");

  TLegend *leg = new TLegend(.0, .75, .83, .9);
  leg->SetFillStyle(0);
  //     leg->AddEntry("", "#it{#bf{sPHENIX}} Projection", "");
  leg->AddEntry("", Form("#it{#bf{sPHENIX}} Projection, Years 1-3"), "");
  leg->AddEntry("", Form("%.1f pb^{-1} str. #it{p}^{#uparrow}+#it{p}#rightarrow h^{+} + X, P=%.2f", pp_rec_3year / 1e12, pp_beam_pol), "");
  leg->Draw();

  SaveCanvas(c1, TString(c1->GetName()), kTRUE);
  //    c1->SaveAs("AN_dp_sPHENIX.pdf");
}
