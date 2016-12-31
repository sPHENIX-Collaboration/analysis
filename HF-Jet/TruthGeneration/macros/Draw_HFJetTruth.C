// $Id: $                                                                                             

/*!
 * \file Draw_HFJetTruth.C
 * \brief 
 * \author Jin Huang <jhuang@bnl.gov>
 * \version $Revision:   $
 * \date $Date: $
 */

#include <cmath>
#include <TFile.h>
#include <TString.h>
#include <TLine.h>
#include <TTree.h>
#include <TLatex.h>
#include <TGraphErrors.h>
#include <cassert>
#include "SaveCanvas.C"
#include "SetOKStyle.C"

TFile * _file0 = NULL;
TTree * T = NULL;

void
Draw_HFJetTruth(const TString infile =
    "/gpfs/mnt/gpfs02/sphenix/user/jinhuang/HF-jet/event_gen/200pp_pythia8_CTEQ6L/200pp_pythia8_CTEQ6L_ALL.cfg_eneg_DSTReader.root",
    double int_lumi = 789908/4.631e-03 / 1e9, const double dy = 0.6*2)
//Draw_HFJetTruth(const TString infile =
//    "../macros3/G4sPHENIXCells.root_DSTReader.root",
//    double int_lumi = 789908/4.631e-03 / 1e9, const double dy = 0.6*2)
//Draw_HFJetTruth(const TString infile =
//    "../macros2/G4sPHENIXCells.root_DSTReader.root",
//    const double int_lumi = 1842152 / 5.801e-03 / 1e9,
//    const double dy = 0.6 * 2)
{
  SetOKStyle();
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(1111);
  TVirtualFitter::SetDefaultFitter("Minuit2");

  gSystem->Load("libg4eval.so");

  if (!_file0)
    {
      TString chian_str = infile;
      chian_str.ReplaceAll("ALL", "*");

      TChain * t = new TChain("T");
      const int n = t->Add(chian_str);

      int_lumi *= n;
//      cout << "Loaded " << n << " root files with " << chian_str << endl;
      cout << "int_lumi = " << int_lumi << " pb^-1 from "<<n<<" root files with " << chian_str << endl;
      assert(n > 0);


      T = t;

      _file0 = new TFile;
      _file0->SetName(infile);
    }

  TH1 * hall = new TH1F("hall", ";p_{T} (GeV/c)", 100, 0, 100);
  TH1 * h_b = new TH1F("h_b", ";p_{T} (GeV/c)", 100, 0, 100);
  TH1 * h_bq = new TH1F("h_bq", ";p_{T} (GeV/c)", 100, 0, 100);
  TH1 * h_bh = new TH1F("h_bh", ";p_{T} (GeV/c)", 100, 0, 100);
  TH1 * h_bh5 = new TH1F("h_bh5", ";p_{T} (GeV/c)", 100, 0, 100);
  TH1 * h_ch5 = new TH1F("h_ch5", ";p_{T} (GeV/c)", 100, 0, 100);
  TH1 * h_c = new TH1F("h_c", ";p_{T} (GeV/c)", 100, 0, 100);

  T->Draw("AntiKt_Truth_r04.get_pt()>>hall",
      "AntiKt_Truth_r04.get_pt()>20 && abs(AntiKt_Truth_r04.get_eta())<0.6",
      "goff");

  T->Draw("AntiKt_Truth_r04.get_pt()>>h_b",
      "AntiKt_Truth_r04.get_pt()>20 && abs(AntiKt_Truth_r04.get_eta())<0.6 && abs(AntiKt_Truth_r04.get_property(1000))==5",
      "goff");
//  T->Draw(
//      "AntiKt_Truth_r04.get_pt()* AntiKt_Truth_r04.get_property(1001) >>h_bq",
//      "AntiKt_Truth_r04.get_pt()>20 && abs(AntiKt_Truth_r04.get_eta())<0.6 && abs(AntiKt_Truth_r04.get_property(1000))==5",
//      "goff");

//  T->Draw("AntiKt_Truth_r04.get_pt()>>h_bh",
//      "AntiKt_Truth_r04.get_pt()>20 && abs(AntiKt_Truth_r04.get_eta())<0.6 && AntiKt_Truth_r04.get_property(1010)==5",
//      "goff");

  T->Draw("AntiKt_Truth_r04.get_pt()>>h_bh5",
      "AntiKt_Truth_r04.get_pt()>20 && abs(AntiKt_Truth_r04.get_eta())<0.6 && abs(AntiKt_Truth_r04.get_property(1010))==5 &&  AntiKt_Truth_r04.get_property(1011) * AntiKt_Truth_r04.get_pt() > 5",
      "goff");
//
  T->Draw("AntiKt_Truth_r04.get_pt()>>h_c",
      "AntiKt_Truth_r04.get_pt()>20 && abs(AntiKt_Truth_r04.get_eta())<0.6 && abs(AntiKt_Truth_r04.get_property(1000))==4",
      "goff");
  T->Draw("AntiKt_Truth_r04.get_pt()>>h_ch5",
      "AntiKt_Truth_r04.get_pt()>20 && abs(AntiKt_Truth_r04.get_eta())<0.6 && abs(AntiKt_Truth_r04.get_property(1010))==4 &&  AntiKt_Truth_r04.get_property(1011) * AntiKt_Truth_r04.get_pt() > 5",
      "goff");

  Convert2CrossSection(hall, int_lumi, dy);
  Convert2CrossSection(h_b, int_lumi, dy);
  Convert2CrossSection(h_bq, int_lumi, dy);
  Convert2CrossSection(h_bh5, int_lumi, dy);
  Convert2CrossSection(h_ch5, int_lumi, dy);
  Convert2CrossSection(h_c, int_lumi, dy);

  h_b->SetLineColor(kBlue);
  h_b->SetMarkerColor(kBlue);

//  h_bq->SetLineColor(kBlue + 3);
//  h_bq->SetMarkerColor(kBlue + 3);

  h_bh5->SetLineColor(kBlue + 3);
  h_bh5->SetMarkerColor(kBlue + 3);
  h_bh5->SetMarkerStyle(kStar);

  h_c->SetLineColor(kRed);
  h_c->SetMarkerColor(kRed);

  h_ch5->SetLineColor(kRed+3);
  h_ch5->SetMarkerColor(kRed+3);
  h_ch5->SetMarkerStyle(kStar);

  TGraphAsymmErrors * gr_fonll_b = GetFONLL_B();
  gr_fonll_b->SetFillColor(kBlue - 7);
  gr_fonll_b->SetFillStyle(3002);
  TGraphAsymmErrors * gr_fonll_c = GetFONLL_C();
  gr_fonll_c->SetFillColor(kRed - 7);
  gr_fonll_c->SetFillStyle(3003);
  TGraph * gr_phenix = GetPHENIX_jet();
  gr_phenix->SetLineColor(kGray+2);
  gr_phenix->SetMarkerColor(kBlack);
  gr_phenix->SetMarkerStyle(kOpenCross);

  TCanvas *c1 = new TCanvas("Draw_HFJetTruth_PR", "Draw_HFJetTruth_PR", 1000, 860);
//  c1->Divide(2, 2);
  int idx = 1;
  TPad * p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetGridx(0);
  p->SetGridy(0);
  p->SetLogy();

  hall->Draw();

  gr_fonll_b->Draw("3");
//  gr_fonll_c->Draw("3");
  gr_phenix->Draw("pe");

  h_b->Draw("same");
//  h_bh5->Draw("same");
//  h_bq->Draw("same");
//  h_c->Draw("same");
//  h_ch5->Draw("same");

  hall->GetXaxis()->SetRangeUser(18, 60);
  hall->GetYaxis()->SetTitle(
      "d^{2}#sigma/(dp_{T}dy), d^{2}#sigma/(dp_{T}d#eta) (pb/(GeV/c))");

  TLegend * leg = new TLegend(0.4, 0.6, 0.95, 0.95);
  leg->SetFillColor(kWhite);
  leg->SetFillStyle(1001);
  leg->SetHeader("p+p collisions @ sPHENIX, #sqrt{s} = 200 GeV, |#eta|<0.6");
  leg->AddEntry(hall, "Inclusive jet, Pythia8, Truth, anti-k_{t}, R=0.4",
      "lpe");
//  leg->AddEntry(h_c, "c-quark jet, Pythia8, Truth, anti-k_{t}, R=0.4", "lpe");
  leg->AddEntry(h_b, "b-quark jet, Pythia8, Truth, anti-k_{t}, R=0.4", "lpe");
//  leg->AddEntry(h_bh5,
//      "b-hadron jet, Pythia8, Truth, anti-k_{t}, R=0.4, p_{T, b-hadron}>5 GeV/c",
//      "lpe");
  leg->AddEntry(gr_phenix, "PHENIX inclusive jet, PRL 116, 122301 (2016)", "ple");
//  leg->AddEntry(gr_fonll_c, "c-quark, FONLL v1.3.2, CTEQ6.6, |y|<0.6", "f");
  leg->AddEntry(gr_fonll_b, "b-quark, FONLL v1.3.2, CTEQ6.6, |y|<0.6", "f");
  leg->Draw();

  SaveCanvas(c1, TString(_file0->GetName()) + TString(c1->GetName()), kTRUE);
}

void
Convert2CrossSection(TH1* h, const double int_lumi, const double dy)
{
  h->Sumw2();

  for (int i = 1; i <= h->GetXaxis()->GetNbins(); ++i)
    {
      const double pT1 = h->GetXaxis()->GetBinLowEdge(i);
      const double pT2 = h->GetXaxis()->GetBinUpEdge(i);

//      const double dpT2 =  pT2*pT2 - pT1*pT1;
      const double dpT = pT2 - pT1;

//      const double count2sigma = 1./dpT2/dy/int_lumi;
      const double count2sigma = 1. / dpT / dy / int_lumi;

      h->SetBinContent(i, h->GetBinContent(i) * count2sigma);
      h->SetBinError(i, h->GetBinError(i) * count2sigma);
    }

//  h->GetYaxis()->SetTitle("#sigma/(dp_{T}^{2} dy) (pb/(GeV/c)^{2})");

  h->SetMarkerStyle(kFullCircle);
}

TGraphAsymmErrors *
GetFONLL_B()
{
//# Job started on: Wed Aug 24 04:28:34 CEST 2016 .
//# FONLL heavy quark hadroproduction cross section, calculated on Wed Aug 24 04:28:42 CEST 2016
//# FONLL version and perturbative order: ## FONLL v1.3.2 fonll [ds/dpt^2dy (pb/GeV^2)]
//# quark = bottom
//# final state = quark
//# ebeam1 = 100, ebeam2 = 100
//# PDF set = CTEQ6.6
//# ptmin = 10
//# ptmax = 30
//# etamin = -.6
//# etamax = .6
//# Uncertainties from scales, masses combined quadratically
//# cross section is ds/dpt (pb/GeV)
//# pt      central      min       max       min_sc     max_sc     min_mass   max_mass
//  10.0000 4.1645e+03 2.9134e+03 6.0916e+03 2.9634e+03 6.0553e+03 3.8146e+03 4.5365e+03
//  11.0526 2.4830e+03 1.7438e+03 3.6234e+03 1.7667e+03 3.6072e+03 2.3005e+03 2.6743e+03
//  12.1053 1.5046e+03 1.0602e+03 2.1900e+03 1.0710e+03 2.1826e+03 1.4071e+03 1.6056e+03
//  13.1579 9.2707e+02 6.5506e+02 1.3454e+03 6.6029e+02 1.3418e+03 8.7394e+02 9.8137e+02
//  14.2105 5.8027e+02 4.1098e+02 8.3990e+02 4.1358e+02 8.3817e+02 5.5067e+02 6.1015e+02
//  15.2632 3.6865e+02 2.6159e+02 5.3224e+02 2.6290e+02 5.3139e+02 3.5193e+02 3.8532e+02
//  16.3158 2.3744e+02 1.6874e+02 3.4198e+02 1.6941e+02 3.4156e+02 2.2785e+02 2.4686e+02
//  17.3684 1.5484e+02 1.1016e+02 2.2257e+02 1.1051e+02 2.2236e+02 1.4930e+02 1.6021e+02
//  18.4211 1.0215e+02 7.2725e+01 1.4657e+02 7.2904e+01 1.4646e+02 9.8911e+01 1.0522e+02
//  19.4737 6.8101e+01 4.8502e+01 9.7587e+01 4.8594e+01 9.7535e+01 6.6205e+01 6.9866e+01
//  20.5263 4.5818e+01 3.2631e+01 6.5588e+01 3.2678e+01 6.5562e+01 4.4707e+01 4.6827e+01
//  21.5789 3.1111e+01 2.2148e+01 4.4506e+01 2.2171e+01 4.4494e+01 3.0461e+01 3.1684e+01
//  22.6316 2.1287e+01 1.5142e+01 3.0444e+01 1.5153e+01 3.0439e+01 2.0909e+01 2.1607e+01
//  23.6842 1.4663e+01 1.0419e+01 2.0974e+01 1.0424e+01 2.0972e+01 1.4446e+01 1.4838e+01
//  24.7368 1.0175e+01 7.2188e+00 1.4562e+01 7.2214e+00 1.4562e+01 1.0053e+01 1.0266e+01
//  25.7895 7.1026e+00 5.0296e+00 1.0176e+01 5.0307e+00 1.0175e+01 7.0360e+00 7.1470e+00
//  26.8421 4.9796e+00 3.5183e+00 7.1439e+00 3.5187e+00 7.1438e+00 4.9454e+00 4.9976e+00
//  27.8947 3.5103e+00 2.4738e+00 5.0451e+00 2.4739e+00 5.0451e+00 3.4945e+00 3.5143e+00
//  28.9474 2.4892e+00 1.7492e+00 3.5856e+00 1.7493e+00 3.5856e+00 2.4835e+00 2.4892e+00
//  30.0000 1.7693e+00 1.2394e+00 2.5556e+00 1.2394e+00 2.5556e+00 1.7633e+00 1.7693e+00

  const double pts[] =
    {

    10.0000, 11.0526, 12.1053, 13.1579, 14.2105, 15.2632, 16.3158, 17.3684,
        18.4211, 19.4737, 20.5263, 21.5789, 22.6316, 23.6842, 24.7368, 25.7895,
        26.8421, 27.8947, 28.9474, 30.0000

    };

  const double centrals[] =
    { 4.1645e+03, 2.4830e+03, 1.5046e+03, 9.2707e+02, 5.8027e+02, 3.6865e+02,
        2.3744e+02, 1.5484e+02, 1.0215e+02, 6.8101e+01, 4.5818e+01, 3.1111e+01,
        2.1287e+01, 1.4663e+01, 1.0175e+01, 7.1026e+00, 4.9796e+00, 3.5103e+00,
        2.4892e+00, 1.7693e+00 };
  const double min[] =
    { 4.1645e+03 - 2.9134e+03, 2.4830e+03 - 1.7438e+03, 1.5046e+03 - 1.0602e+03,
        9.2707e+02 - 6.5506e+02, 5.8027e+02 - 4.1098e+02, 3.6865e+02
            - 2.6159e+02, 2.3744e+02 - 1.6874e+02, 1.5484e+02 - 1.1016e+02,
        1.0215e+02 - 7.2725e+01, 6.8101e+01 - 4.8502e+01, 4.5818e+01
            - 3.2631e+01, 3.1111e+01 - 2.2148e+01, 2.1287e+01 - 1.5142e+01,
        1.4663e+01 - 1.0419e+01, 1.0175e+01 - 7.2188e+00, 7.1026e+00
            - 5.0296e+00, 4.9796e+00 - 3.5183e+00, 3.5103e+00 - 2.4738e+00,
        2.4892e+00 - 1.7492e+00, 1.7693e+00 - 1.2394e+00 };

  const double max[] =
    { 6.0916e+03 - 4.1645e+03, 3.6234e+03 - 2.4830e+03, 2.1900e+03 - 1.5046e+03,
        1.3454e+03 - 9.2707e+02, 8.3990e+02 - 5.8027e+02, 5.3224e+02
            - 3.6865e+02, 3.4198e+02 - 2.3744e+02, 2.2257e+02 - 1.5484e+02,
        1.4657e+02 - 1.0215e+02, 9.7587e+01 - 6.8101e+01, 6.5588e+01
            - 4.5818e+01, 4.4506e+01 - 3.1111e+01, 3.0444e+01 - 2.1287e+01,
        2.0974e+01 - 1.4663e+01, 1.4562e+01 - 1.0175e+01, 1.0176e+01
            - 7.1026e+00, 7.1439e+00 - 4.9796e+00, 5.0451e+00 - 3.5103e+00,
        3.5856e+00 - 2.4892e+00, 2.5556e+00 - 1.7693e+00 };

  TGraphAsymmErrors * gr = new TGraphAsymmErrors(20, pts, centrals, 0, 0, min,
      max);

  return gr;
}

TGraphAsymmErrors *
GetFONLL_C()
{

//# Job started on: Thu Aug 25 05:08:57 CEST 2016 .
//# FONLL heavy quark hadroproduction cross section, calculated on Thu Aug 25 05:09:06 CEST 2016
//# FONLL version and perturbative order: ## FONLL v1.3.2 fonll [ds/dpt^2dy (pb/GeV^2)]
//# quark = charm
//# final state = quark
//# ebeam1 = 100, ebeam2 = 100
//# PDF set = CTEQ6.6
//# ptmin = 10
//# ptmax = 30
//# etamin = -.6
//# etamax = .6
//# Uncertainties from scales, masses combined quadratically
//# cross section is ds/dpt (pb/GeV)
//# pt      central      min       max       min_sc     max_sc     min_mass   max_mass
//  10.0000 9.7368e+03 6.5051e+03 1.5528e+04 6.5641e+03 1.5522e+04 9.1225e+03 1.0006e+04
//  11.0526 4.9069e+03 3.2787e+03 7.7308e+03 3.3256e+03 7.7226e+03 4.5190e+03 5.1221e+03
//  12.1053 2.5743e+03 1.7187e+03 4.0147e+03 1.7525e+03 4.0069e+03 2.3363e+03 2.7237e+03
//  13.1579 1.3990e+03 9.3274e+02 2.1641e+03 9.5597e+02 2.1578e+03 1.2537e+03 1.4976e+03
//  14.2105 7.8344e+02 5.2138e+02 1.2037e+03 5.3701e+02 1.1989e+03 6.9430e+02 8.4730e+02
//  15.2632 4.5087e+02 2.9938e+02 6.8902e+02 3.0978e+02 6.8543e+02 3.9570e+02 4.9204e+02
//  16.3158 2.6550e+02 1.7584e+02 4.0398e+02 1.8275e+02 4.0140e+02 2.3098e+02 2.9210e+02
//  17.3684 1.5978e+02 1.0551e+02 2.4232e+02 1.1011e+02 2.4049e+02 1.3793e+02 1.7705e+02
//  18.4211 9.7903e+01 6.4459e+01 1.4811e+02 6.7524e+01 1.4682e+02 8.3915e+01 1.0919e+02
//  19.4737 6.0993e+01 4.0030e+01 9.2103e+01 4.2085e+01 9.1200e+01 5.1941e+01 6.8435e+01
//  20.5263 3.8605e+01 2.5248e+01 5.8239e+01 2.6631e+01 5.7606e+01 3.2685e+01 4.3550e+01
//  21.5789 2.4727e+01 1.6108e+01 3.7286e+01 1.7044e+01 3.6842e+01 2.0821e+01 2.8038e+01
//  22.6316 1.6036e+01 1.0402e+01 2.4185e+01 1.1040e+01 2.3872e+01 1.3433e+01 1.8271e+01
//  23.6842 1.0532e+01 6.8006e+00 1.5898e+01 7.2389e+00 1.5678e+01 8.7774e+00 1.2053e+01
//  24.7368 6.9729e+00 4.4803e+00 1.0540e+01 4.7838e+00 1.0385e+01 5.7809e+00 8.0136e+00
//  25.7895 4.6560e+00 2.9760e+00 7.0510e+00 3.1876e+00 6.9411e+00 3.8397e+00 5.3729e+00
//  26.8421 3.1409e+00 1.9970e+00 4.7681e+00 2.1454e+00 4.6902e+00 2.5775e+00 3.6384e+00
//  27.8947 2.1346e+00 1.3500e+00 3.2500e+00 1.4543e+00 3.1946e+00 1.7438e+00 2.4816e+00
//  28.9474 1.4557e+00 9.1535e-01 2.2239e+00 9.8868e-01 2.1845e+00 1.1839e+00 1.6985e+00
//  30.0000 9.9826e-01 6.2400e-01 1.5310e+00 6.7571e-01 1.5029e+00 8.0844e-01 1.1690e+00

  const double pts[] =
    {
//         pt
        10.0000, 11.0526, 12.1053, 13.1579, 14.2105, 15.2632, 16.3158, 17.3684,
        18.4211, 19.4737, 20.5263, 21.5789, 22.6316, 23.6842, 24.7368, 25.7895,
        26.8421, 27.8947, 28.9474, 30.0000

    };

  const double centrals[] =
    {

//            central
        9.7368e+03, 4.9069e+03, 2.5743e+03, 1.3990e+03, 7.8344e+02, 4.5087e+02,
        2.6550e+02, 1.5978e+02, 9.7903e+01, 6.0993e+01, 3.8605e+01, 2.4727e+01,
        1.6036e+01, 1.0532e+01, 6.9729e+00, 4.6560e+00, 3.1409e+00, 2.1346e+00,
        1.4557e+00, 9.9826e-01

    };
  const double min[] =
    {

//       central           min
        9.7368e+03 - 6.5051e+03, 4.9069e+03 - 3.2787e+03, 2.5743e+03
            - 1.7187e+03, 1.3990e+03 - 9.3274e+02, 7.8344e+02 - 5.2138e+02,
        4.5087e+02 - 2.9938e+02, 2.6550e+02 - 1.7584e+02, 1.5978e+02
            - 1.0551e+02, 9.7903e+01 - 6.4459e+01, 6.0993e+01 - 4.0030e+01,
        3.8605e+01 - 2.5248e+01, 2.4727e+01 - 1.6108e+01, 1.6036e+01
            - 1.0402e+01, 1.0532e+01 - 6.8006e+00, 6.9729e+00 - 4.4803e+00,
        4.6560e+00 - 2.9760e+00, 3.1409e+00 - 1.9970e+00, 2.1346e+00
            - 1.3500e+00, 1.4557e+00 - 9.1535e-01, 9.9826e-01 - 6.2400e-01

    };

  const double max[] =
    {

//            max             central
        1.5528e+04 - 9.7368e+03, 7.7308e+03 - 4.9069e+03, 4.0147e+03
            - 2.5743e+03, 2.1641e+03 - 1.3990e+03, 1.2037e+03 - 7.8344e+02,
        6.8902e+02 - 4.5087e+02, 4.0398e+02 - 2.6550e+02, 2.4232e+02
            - 1.5978e+02, 1.4811e+02 - 9.7903e+01, 9.2103e+01 - 6.0993e+01,
        5.8239e+01 - 3.8605e+01, 3.7286e+01 - 2.4727e+01, 2.4185e+01
            - 1.6036e+01, 1.5898e+01 - 1.0532e+01, 1.0540e+01 - 6.9729e+00,
        7.0510e+00 - 4.6560e+00, 4.7681e+00 - 3.1409e+00, 3.2500e+00
            - 2.1346e+00, 2.2239e+00 - 1.4557e+00, 1.5310e+00 - 9.9826e-01

    };

  TGraphAsymmErrors * gr = new TGraphAsymmErrors(20, pts, centrals, 0, 0, min,
      max);

  return gr;
}

TGraph *
GetPHENIX_jet()
{

//  PPG184
//  p+p d^2sigma/dpT deta [mb / GeV]
//
//  pT low     pT high     yval        ystat[%] ysyst[%]
//
//  12.2       14.5        0.0001145   1.0      15.6
//  14.5       17.3        3.774e-05   1.3      16.3
//  17.3       20.7        1.263e-05   1.7      15.6
//  20.7       24.7        4.230e-06   2.3      17.3
//  24.7       29.4        1.224e-06   3.6      19.3
//  29.4       35.1        2.962e-07   6.1      21.0
//  35.1       41.9        5.837e-08   8.9      24.3
//  41.9       50.0        8.882e-09   11       32.3

  const double pt_l[] =
    {

//          pT low

        12.2, 14.5, 17.3, 20.7, 24.7, 29.4, 35.1, 41.9

    };

  const double pt_h[] =
    {

//              pT high

        14.5, 17.3, 20.7, 24.7, 29.4, 35.1, 41.9, 50.0

    };
  const double yval[] =
    {

//          yval

        0.0001145, 3.774e-05, 1.263e-05, 4.230e-06, 1.224e-06, 2.962e-07,
        5.837e-08, 8.882e-09

    };
  const double ystat[] =
    {

//          ystat[%]

        1.0, 1.3, 1.7, 2.3, 3.6, 6.1, 8.9, 11

    };

  const double ysyst[] =
    {

//            ysyst[%]

        15.6, 16.3, 15.6, 17.3, 19.3, 21.0, 24.3, 32.3

    };

  double pT_c[100] =
    { 0 };
  double pT_e[100] =
    { 0 };
  double y_e[100] =
    { 0 };

  for (int i = 0; i < 8; ++i)
    {
      pT_c[i] = 0.5 * (pt_l[i] + pt_h[i]);
      pT_e[i] = 0.5 * (pt_h[i] - pt_l[i]);
      yval[i] *= 1e9; // mb -> pb
      y_e[i] = yval[i] * sqrt(ystat[i] * ystat[i] + ysyst[i] * ysyst[i]) / 100;

    }

  TGraphErrors * gr = new TGraphErrors(8, pT_c, yval, pT_e, y_e);

  return gr;
}
