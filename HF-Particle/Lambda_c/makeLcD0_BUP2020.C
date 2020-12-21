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
#include <TString.h>
#include <TTree.h>
#include <TVectorD.h>
#include <TVirtualFitter.h>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <vector>

#include "SaveCanvas.C"
#include "sPhenixStyle.C"

const double refAuAuMB = 240e9;
const double refAuAuXSec = 6.8252;  // b
const double Psi2_resolution = 0.5;

const double AuAu_Ncoll_C0_10 = 960.2;   // [DOI:?10.1103/PhysRevC.87.034911?]
const double AuAu_Ncoll_C0_20 = 770.6;   // [DOI:?10.1103/PhysRevC.91.064904?]
const double AuAu_Ncoll_60_70 = 29.8;    //PHYSICAL REVIEW C 87, 034911 (2013)
const double AuAu_Ncoll_70_80 = 12.6;    //PHYSICAL REVIEW C 87, 034911 (2013)
const double AuAu_Ncoll_C0_100 = 238.5;  // BUP2020
const double pAu_Ncoll_C0_100 = 4.7;     // pb^-1 [sPH-TRG-000]

const double AuAu_rec_3year = (5.7 + 15) * 1e9;       // BUP2020
const double AuAu_rec_5year = AuAu_rec_3year + 30e9;  // BUP2020

const double pp_inelastic_crosssec = 42e-3;        // 42 mb [sPH-TRG-000]
const double pp_rec_3year = 6.2e12;                // BUP2020
const double pp_rec_5year = pp_rec_3year + 80e12;  // BUP2020
const double pp_beam_pol = 0.57;

const double OO_rec_5year = 37e9;              // BUP2020
const double OO_Ncoll_C0_100 = 9.6;            // BUP2020
const double OO_inelastic_crosssec = 1.12214;  // BUP2020

const double ArAr_rec_5year = 12e9;             // BUP2020
const double ArAr_Ncoll_C0_100 = 28.5;          // BUP2020
const double ArAr_inelastic_crosssec = 2.3423;  // BUP2020

void makeLcD0_BUP2020()
{
  //  gROOT->LoadMacro("sPhenixStyle.C");
  SetsPhenixStyle();

  gROOT->Reset();

  const Int_t NConfig = 4;
  const Int_t NMax = 5;
  const Int_t N[NConfig] = {3, 5, 4, 1};  // ALICE_pp, ALICE_pPb, STAR_AuAu
  const Char_t *ExpName[NConfig] = {"STAR", "ALICE", "ALICE", "ALICE"};
  const Char_t *CollName[NConfig] = {"AuAu_10_80_200GeV", "pp_7TeV", "pPb_5p02TeV", "PbPb_5p02TeV"};
  double x[NConfig][NMax], y[NConfig][NMax], ye[NConfig][NMax], yes_u[NConfig][NMax], yes_d[NConfig][NMax];
  TGraphErrors *gr[NConfig];

  // STAR data points
  ifstream inData;
  for (int i = 0; i < NConfig; i++)
  {
    inData.open(Form("dat/%s_LcD0_%s.txt", ExpName[i], CollName[i]));
    for (int j = 0; j < N[i]; j++)
    {
      inData >> x[i][j] >> y[i][j] >> ye[i][j] >> yes_u[i][j] >> yes_d[i][j];
    }
    inData.close();
    gr[i] = new TGraphErrors(N[i], x[i], y[i], 0, ye[i]);
    cout << Form("dat/%s_LcD0_%s.txt", ExpName[i], CollName[i]) << endl;
    gr[i]->Print();
  }

  // STAR Central data
  // Updated with https://drupal.star.bnl.gov/STAR/files/starpublications/318/data.html
  double x_St[1] = {3.96};
  double y_St[1] = {1.0772};
  double ye_St[1] = {0.1643};
  double yes_St[1] = {0.2647};
  double yesx_St[1] = {0.1};
  TGraphErrors *gr_St = new TGraphErrors(1, x_St, y_St, 0, ye_St);
  gr_St->SetMarkerStyle(29);
  gr_St->SetMarkerSize(2.5);
  gr_St->SetMarkerColor(kRed - 6);
  gr_St->SetLineWidth(4);
  gr_St->SetLineColor(kRed - 6);
  TGraphAsymmErrors* grs_St = new TGraphAsymmErrors(1, x_St, y_St, yesx_St, yesx_St, yes_St, yes_St);
  grs_St->SetFillColor(kRed - 10);
  grs_St->SetMarkerStyle(29);
  grs_St->SetMarkerSize(2.5);
  grs_St->SetMarkerColor(kRed - 6);
  grs_St->SetLineWidth(4);
  grs_St->SetLineColor(kRed - 6);

  // Theory calculations
  // pythia6
  TFile *fin = new TFile("root/pythia_D_ratio.root");
  TGraphErrors *gr_pythia_tmp = (TGraphErrors *) fin->Get("Ratio_Lc_D0");
  TGraph *gr_pythia = new TGraph(*gr_pythia_tmp);
  gr_pythia->SetLineWidth(2);
  gr_pythia->SetLineColor(16);
  // pythia8
  fin = new TFile("root/PYTHIA8_D_ratio_200GeV_20181029_0.root");
  TDirectoryFile *fLc = (TDirectoryFile *) fin->Get("Lc");
  TH1D *h_pythia8 = (TH1D *) fLc->Get("LctoD0_pt_whole_rebin");
  TGraph *gr_pythia8 = new TGraph(h_pythia8);
  gr_pythia8->SetLineWidth(2);
  gr_pythia8->SetLineColor(1);
  gr_pythia8->SetLineStyle(2);

  fin = new TFile("root/PYTHIA8_D_ratio_200GeV_20181029_CR.root");
  fLc = (TDirectoryFile *) fin->Get("Lc");
  TH1D *h_pythia8_CR = (TH1D *) fLc->Get("LctoD0_pt_whole_rebin");
  TGraph *gr_pythia8_CR = new TGraph(h_pythia8_CR);
  gr_pythia8_CR->SetLineWidth(2);
  gr_pythia8_CR->SetLineColor(1);
  gr_pythia8_CR->SetLineStyle(1);

  TGraph *gr_pythia_7TeV = new TGraph("dat/PYTHIA_Lc_D0_pp_7TeV.txt", "%lg %lg");
  TGraph *gr_pythia_7TeV_CR = new TGraph("dat/PYTHIACR_Lc_D0_pp_7TeV.txt", "%lg %lg");
  TGraph *gr_Greco_LHC_1 = new TGraph("dat/Greco_LcD0_1_PbPb5p02TeV.txt", "%lg %lg");
  TGraph *gr_Greco_LHC_2 = new TGraph("dat/Greco_LcD0_2_PbPb5p02TeV.txt", "%lg %lg");
  TGraph *gr_ShaoSong_LHC_1 = new TGraph("dat/ShaoSong_LcD0_1_PbPb5p02TeV.txt", "%lg %lg");
  TGraph *gr_ShaoSong_LHC_2 = new TGraph("dat/ShaoSong_LcD0_2_PbPb5p02TeV.txt", "%lg %lg");
  /*
  const Int_t N_Greco = 14;
  Double_t pt_Greco[N_Greco], y_Greco[N_Greco], ye_Greco[N_Greco];
  inData.open("dat/LcD0_Greco_1407.5069.txt");
  for(int i=0;i<N_Greco;i++) {
    double a, b, c;
    inData >> a >> b >> c;
    pt_Greco[i] = a;
    y_Greco[i] = (b+c)/2.;
    ye_Greco[i] = fabs(b-c)/2.;
  }
  inData.close();
  TGraphErrors *gr_Greco = new TGraphErrors(N_Greco, pt_Greco, y_Greco, 0, ye_Greco);
  gr_Greco->SetFillColor(16);
  gr_Greco->SetLineColor(16);
  */
  TGraph *gr_Greco_1 = new TGraph("dat/Greco_LcD0_1_AuAu200GeV_0_20.dat", "%lg %lg");
  gr_Greco_1->SetLineWidth(2);
  gr_Greco_1->SetLineStyle(2);
  gr_Greco_1->SetLineColor(4);
  TGraph *gr_Greco_2 = new TGraph("dat/Greco_LcD0_2_AuAu200GeV_0_20.dat", "%lg %lg");
  gr_Greco_2->SetLineWidth(2);
  gr_Greco_2->SetLineStyle(1);
  gr_Greco_2->SetLineColor(4);
  TGraph *gr_Tsinghua_1 = new TGraph("dat/Tsinghua_LcD0_1_AuAu200GeV_10_80.txt", "%lg %lg");
  gr_Tsinghua_1->SetLineWidth(2);
  gr_Tsinghua_1->SetLineStyle(2);
  gr_Tsinghua_1->SetLineColor(kGreen - 6);
  TGraph *gr_Tsinghua_2 = new TGraph("dat/Tsinghua_LcD0_2_AuAu200GeV_10_80.txt", "%lg %lg");
  gr_Tsinghua_2->SetLineWidth(2);
  gr_Tsinghua_2->SetLineStyle(1);
  gr_Tsinghua_2->SetLineColor(kGreen - 6);

  const Int_t N_Ko = 12;
  Double_t pt_Ko[N_Ko], y1_Ko[N_Ko], y2_Ko[N_Ko], y3_Ko[N_Ko];
  inData.open("dat/LcD0_Ko_0901.1382.txt");
  for (int i = 0; i < N_Ko; i++)
  {
    inData >> pt_Ko[i] >> y1_Ko[i] >> y2_Ko[i] >> y3_Ko[i];
  }
  inData.close();
  TGraph *gr_Ko_1 = new TGraph(N_Ko, pt_Ko, y1_Ko);
  gr_Ko_1->SetLineWidth(2);
  gr_Ko_1->SetLineColor(2);
  gr_Ko_1->SetLineStyle(3);
  TGraph *gr_Ko_2 = new TGraph(N_Ko, pt_Ko, y2_Ko);
  gr_Ko_2->SetLineWidth(2);
  gr_Ko_2->SetLineColor(2);
  gr_Ko_2->SetLineStyle(2);
  TGraph *gr_Ko_3 = new TGraph(N_Ko, pt_Ko, y3_Ko);
  gr_Ko_3->SetLineWidth(2);
  gr_Ko_3->SetLineColor(2);
  gr_Ko_3->SetLineStyle(1);

  // **************************
  // sPHENIX projection
  // **************************

  TGraphErrors *gr_sPH_noPID_AuAu_0_10(nullptr);
  TGraphErrors *gr_sPH_noPID_pp(nullptr);

  ///////////////////////////////////////////////////////////////////////////
  // LC projection 0-10%
  ///////////////////////////////////////////////////////////////////////////

  {
    const double significance_scaling = sqrt((AuAu_rec_3year * refAuAuXSec) / (refAuAuMB));

    const Int_t N_sPH = 20;
    double pT_sPH[N_sPH], sig_sPH_noPID[N_sPH], sig_sPH_ideal[N_sPH];
    double r_sPH[N_sPH], r_err_sPH_noPID[N_sPH], r_err_sPH_ideal[N_sPH];
    double pT_sPH_noPID[N_sPH], pT_sPH_ideal[N_sPH];
    const Double_t offset = 0.06;
    //  TFile *fin = new TFile("signi_0_10.root");

    fin = new TFile("significance/signi_0.root");
    TH1D *h1 = (TH1D *) fin->Get("NoPID_signi");
    TH1D *h2 = (TH1D *) fin->Get("Cleanideal_signi");
    // cout<<" ok "<<endl;

    vector<int> rebin{3, 1,1, 1, 1, 1, 1, 1, 1, 1, 1};

    int index_input = 1;
    int index_output = 0;
    for (int rebin_count : rebin)
    {
      vector<double> sum_x;
      vector<double> sum_sig2;
      for (int i = 0; i < rebin_count; ++i)
      {
        sum_x.push_back(h1->GetBinCenter(index_input));
        sum_sig2.push_back(pow(h1->GetBinContent(index_input), 2));

        ++index_input;
        if (index_input > h1->GetNbinsX()) break;
      }

      int i(index_output);
      pT_sPH[i] = accumulate(sum_x.begin(), sum_x.end(), 0.) / sum_x.size();
      sig_sPH_noPID[i] = significance_scaling * sqrt(accumulate(sum_sig2.begin(), sum_sig2.end(), 0.)) / sum_sig2.size();
      //      sig_sPH_ideal[i] = h2->GetBinContent(i + 1);

      cout << "gr_sPH_noPID_AuAu_0_10, bin" << i << " pT = "
           << pT_sPH[i] << " Sig = " << sig_sPH_noPID[i]
           << "\t| sum_x.begin() = " << *sum_x.begin() << " sum_x.size() = " << sum_x.size()<<"\t| scaling = "<<significance_scaling << endl;

      pT_sPH_noPID[i] = pT_sPH[i];
      //      pT_sPH_ideal[i] = pT_sPH[i] + offset;
      r_sPH[i] = gr[0]->Eval(pT_sPH[i]);
      r_err_sPH_noPID[i] = r_sPH[i] / sig_sPH_noPID[i];
      //      r_err_sPH_ideal[i] = r_sPH[i] / sig_sPH_ideal[i];

      ++index_output;
      if (index_input > h1->GetNbinsX()) break;
    }

    int n_sPH = index_output;

    gr_sPH_noPID_AuAu_0_10 = new TGraphErrors(n_sPH, pT_sPH_noPID, r_sPH, nullptr, r_err_sPH_noPID);

    gr_sPH_noPID_AuAu_0_10->SetMarkerStyle(kFullCircle);
    // gr_sPH_noPID->SetMarkerSize(1.8);
    gr_sPH_noPID_AuAu_0_10->SetMarkerSize(2);
    gr_sPH_noPID_AuAu_0_10->SetMarkerColor(kBlue+1);
    gr_sPH_noPID_AuAu_0_10->SetLineColor(kBlue+1);
    gr_sPH_noPID_AuAu_0_10->SetLineWidth(4);
  }

  ///////////////////////////////////////////////////////////////////////////
  // LC projection 60-80% to pp
  ///////////////////////////////////////////////////////////////////////////

  {
    const double significance_scaling = sqrt((pp_rec_3year * pp_inelastic_crosssec) / (refAuAuMB * .1 * (AuAu_Ncoll_60_70 + AuAu_Ncoll_70_80)));

    const Int_t N_sPH = 20;
    double pT_sPH[N_sPH], sig_sPH_noPID[N_sPH], sig_sPH_ideal[N_sPH];
    double r_sPH[N_sPH], r_err_sPH_noPID[N_sPH], r_err_sPH_ideal[N_sPH];
    double pT_sPH_noPID[N_sPH], pT_sPH_ideal[N_sPH];
    const Double_t offset = 0.06;
    //  TFile *fin = new TFile("signi_0_10.root");

    fin = new TFile("significance/signi_4.root");
    TH1D *h1 = (TH1D *) fin->Get("NoPID_signi");
    TH1D *h2 = (TH1D *) fin->Get("Cleanideal_signi");
    // cout<<" ok "<<endl;

    vector<int> rebin{3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

    int index_input = 1;
    int index_output = 0;
    for (int rebin_count : rebin)
    {
      vector<double> sum_x;
      vector<double> sum_sig2;
      for (int i = 0; i < rebin_count; ++i)
      {
        sum_x.push_back(h1->GetBinCenter(index_input));
        sum_sig2.push_back(pow(h1->GetBinContent(index_input), 2));

        ++index_input;
        if (index_input > h1->GetNbinsX()) break;
      }

      int i(index_output);
      pT_sPH[i] = accumulate(sum_x.begin(), sum_x.end(), 0.) / sum_x.size();
      sig_sPH_noPID[i] = significance_scaling * sqrt(accumulate(sum_sig2.begin(), sum_sig2.end(), 0)) / sum_sig2.size();
      //      sig_sPH_ideal[i] = h2->GetBinContent(i + 1);

      cout << "gr_sPH_noPID_AuAu_0_10, bin" << i << " pT = "
           << pT_sPH[i] << " Sig = " << sig_sPH_noPID[i]
           << "\t| sum_x.begin() = " << *sum_x.begin() << " sum_x.size() = " << sum_x.size()<<"\t| scaling = "<<significance_scaling << endl;

      pT_sPH_noPID[i] = pT_sPH[i];
      //      pT_sPH_ideal[i] = pT_sPH[i] + offset;
      r_sPH[i] = gr_pythia8_CR->Eval(pT_sPH[i]);
      r_err_sPH_noPID[i] = r_sPH[i] / sig_sPH_noPID[i];
      //      r_err_sPH_ideal[i] = r_sPH[i] / sig_sPH_ideal[i];

      ++index_output;
      if (index_input > h1->GetNbinsX()) break;
    }

    int n_sPH = index_output;

    gr_sPH_noPID_pp = new TGraphErrors(n_sPH, pT_sPH_noPID, r_sPH, nullptr, r_err_sPH_noPID);

    gr_sPH_noPID_pp->SetMarkerStyle(kFullSquare);
    // gr_sPH_noPID->SetMarkerSize(1.8);
    gr_sPH_noPID_pp->SetMarkerSize(2);
    gr_sPH_noPID_pp->SetMarkerColor(kBlack);
    gr_sPH_noPID_pp->SetLineColor(kBlack);
    gr_sPH_noPID_pp->SetLineWidth(4);
  }



  TCanvas *c1 = new TCanvas("Lc_BUP2020_AuAu_3yr", "Lc_BUP2020_AuAu_3yr", 1100, 800);
  c1->Divide(1, 1);
  int idx = 1;
  TPad *p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();

  p->DrawFrame(0, 0, 9, 4.5)->SetTitle(";#it{p}_{T} [GeV];(#Lambda_{c}^{+}+#bar{#Lambda}_{c}^{-})/(D^{0}+#bar{D}^{0})");
  //  TCanvas *c1 = new TCanvas("c1", "c1", 0, 0, 800, 600);
  //  gStyle->SetOptFit(0);
  //  gStyle->SetOptStat(0);
  //  gStyle->SetEndErrorSize(0);

  //  c1->SetFillColor(10);
  //  c1->SetFillStyle(0);
  //  c1->SetBorderMode(0);
  //  c1->SetBorderSize(0);
  //  c1->SetFrameFillColor(10);
  //  c1->SetFrameFillStyle(0);
  //  c1->SetFrameBorderMode(0);
  //  c1->SetLeftMargin(0.11);
  //  c1->SetRightMargin(0.03);
  //  c1->SetTopMargin(0.03);
  //  c1->SetBottomMargin(0.14);
  //  c1->SetLogy();
  // c1->SetTickx();
  // c1->SetTicky();
//  c1->Draw();
//  c1->cd();

  double x1 = 0.01;
  double x2 = 8.99;
  double y1 = 0.001;
  double y2 = 4;
  //  TH1 *h0 = new TH1D("h0", "", 1, x1, x2);
  //  h0->SetMinimum(y1);
  //  h0->SetMaximum(y2);
  //  h0->GetXaxis()->SetNdivisions(208);
  //  h0->GetXaxis()->CenterTitle();
  //  h0->GetXaxis()->SetTitle("Transverse Momentum p_{T} (GeV/c)");
  //  h0->GetXaxis()->SetTitleOffset(0.95);
  //  h0->GetXaxis()->SetTitleSize(0.06);
  //  h0->GetXaxis()->SetLabelOffset(0.01);
  //  h0->GetXaxis()->SetLabelSize(0.045);
  //  h0->GetXaxis()->SetLabelFont(42);
  //  h0->GetXaxis()->SetTitleFont(42);
  //  h0->GetYaxis()->SetNdivisions(505);
  //  h0->GetYaxis()->SetTitle("(#Lambda_{c}^{+}+#bar{#Lambda}_{c}^{-})/(D^{0}+#bar{D}^{0})");
  //  h0->GetYaxis()->SetTitleOffset(0.8);
  //  h0->GetYaxis()->SetTitleSize(0.06);
  //  h0->GetYaxis()->SetLabelOffset(0.018);
  //  h0->GetYaxis()->SetLabelSize(0.045);
  //  h0->GetYaxis()->SetLabelFont(42);
  //  h0->GetYaxis()->SetTitleFont(42);
  //  h0->Draw("c");

//  TLine *l1 = new TLine(x1, y1, x2, y1);
//  l1->SetLineWidth(3);
//  l1->Draw("same");
//  TLine *l2 = new TLine(x1, y2, x2, y2);
//  l2->SetLineWidth(3);
//  l2->Draw("same");
//  TLine *l3 = new TLine(x1, y1, x1, y2);
//  l3->SetLineWidth(3);
//  l3->Draw("same");
//  TLine *l4 = new TLine(x2, y1, x2, y2);
//  l4->SetLineWidth(3);
//  l4->Draw("same");

//   gr_pythia->SetFillColor(16);
//   gr_pythia->SetLineWidth(2);
//   gr_pythia->SetLineStyle(2);
//   gr_pythia->SetLineColor(1);
//   gr_pythia->Draw("c");

  gr_pythia8->Draw("c");
  gr_pythia8_CR->Draw("c");

  gr_Greco_1->Draw("c");
  gr_Greco_2->Draw("c");
  //  gr_Ko_1->Draw("c");
  gr_Ko_2->Draw("c");
  gr_Ko_3->Draw("c");  // pythia
  gr_Tsinghua_1->Draw("c");
  gr_Tsinghua_2->Draw("c");

  TLine *la = new TLine(0.2, 0.24, 1.5, 0.24);
  la->SetLineWidth(5);
  la->SetLineColor(18);
  //  la->Draw("same");

  TLatex *tex = new TLatex(0.5, 0.26, "SHM");
  tex->SetTextFont(12);
  tex->SetTextSize(0.06);
  //  tex->Draw("same");

  /*
  // plotting the data points
  const Int_t kStyle[NConfig] = {20, 20, 24, 20};
  const Int_t kColor[NConfig] = {1, 2, 4, 1};
  const double xo = fabs(x2-x1)/80.;
  const double yo = fabs(y2-y1)/80.;
  for(int i=0;i<1;i++) {  // STAR

    for(int j=0;j<N[i];j++) { 
      double x1 = x[i][j] - xo;
      double x2 = x[i][j] + xo;
      double y1 = y[i][j] - yes_d[i][j];
      double y2 = y[i][j] + yes_u[i][j];
      
      TLine *la = new TLine(x1, y1, x1, y1+yo);
      la->SetLineColor(kColor[i]);
      la->SetLineWidth(1);
      la->Draw("same");
      TLine *lb = new TLine(x2, y1, x2, y1+yo);
      lb->SetLineColor(kColor[i]);
      lb->SetLineWidth(1);
      lb->Draw("same");
      TLine *lc = new TLine(x1, y2, x1, y2-yo);
      lc->SetLineColor(kColor[i]);
      lc->SetLineWidth(1);
      lc->Draw("same");
      TLine *ld = new TLine(x2, y2, x2, y2-yo);
      ld->SetLineColor(kColor[i]);
      ld->SetLineWidth(1);
      ld->Draw("same");
      TLine *le = new TLine(x1, y1, x2, y1);
      le->SetLineColor(kColor[i]);
      le->SetLineWidth(2);
      le->Draw("same");
      TLine *lf = new TLine(x1, y2, x2, y2);
      lf->SetLineColor(kColor[i]);
      lf->SetLineWidth(2);
      lf->Draw("same");
    }
    gr[i]->SetMarkerStyle(kStyle[i]);
    gr[i]->SetMarkerColor(kColor[i]);
    gr[i]->SetMarkerSize(1.8);
    gr[i]->SetLineColor(kColor[i]);
    gr[i]->SetLineWidth(2);
    gr[i]->Draw("p");
  }
  */
  grs_St->Draw("2");
  gr_St->Draw("p");
  gr_sPH_noPID_AuAu_0_10->Draw("p");
  gr_sPH_noPID_pp->Draw("p");
  //  gr_sPH_ideal->Draw("p");

//  tex = new TLatex(x1 + 0.2, y2 * 0.9, "Au+Au, #sqrt{s_{NN}}=200GeV");
//  tex->SetTextFont(42);
//  tex->SetTextSize(0.05);
//  tex->Draw("same");
//  tex = new TLatex(x1 + 0.2, y2 * 0.9, "#sqrt{s_{NN}}=200GeV");
//  tex->SetTextFont(42);
//  tex->SetTextSize(0.055);
  //  tex->Draw("same");

  TLegend *leg = new TLegend(0.1998067,0.688525,0.8001561,0.8282708);
  leg->AddEntry(gr_sPH_noPID_pp, Form("%.1f pb^{-1} str. #it{p}+#it{p}", pp_rec_3year/1e12), "pl");
  leg->AddEntry(gr_sPH_noPID_AuAu_0_10, Form("%.0f nb^{-1} rec. Au+Au, 0-10%%", AuAu_rec_3year/1e9), "pl");
  leg->AddEntry(grs_St, "STAR, Au+Au, 0-20%, PRL#bf{124}", "plf");
  leg->Draw();


  leg = new TLegend(0.04741459,0.8467278,0.7472027,0.8968254);
  leg->SetFillStyle(0);
  leg->AddEntry("", "#it{#bf{sPHENIX}} Projection, Years 1-3", "");
//  leg->AddEntry("", ("Years 1-3"), "");
  leg->Draw();

  leg = new TLegend(0.6905134,0.44,0.9302814,0.64);
//  leg->SetFillStyle(0);
  leg->AddEntry(gr_Tsinghua_2, "Tsinghua", "l");
  leg->AddEntry(gr_Greco_2, "Catania", "l");
  leg->AddEntry(gr_Ko_3, "TAMU", "l");
  leg->AddEntry(gr_pythia8_CR, "PYTHIA8", "l");
  leg->Draw();

  leg = new TLegend(0.5, 0.55, 0.94, 0.7);
  leg->AddEntry(gr_Tsinghua_2, "Tsinghua: seq. coal, 10-80%", "l");
  leg->AddEntry(gr_Tsinghua_1, "Tsinghua: simul. coal, 10-80%", "l");
  leg->AddEntry(gr_Greco_2, "Catania: coal only, 0-20%", "l");
  leg->AddEntry(gr_Greco_1, "Catania: coal+frag, 0-20%", "l");
//  leg->Draw();

  leg = new TLegend(0.64, 0.48, 0.94, 0.64);
//  leg->SetFillColor(10);
//  leg->SetFillStyle(10);
//  leg->SetLineStyle(4000);
//  leg->SetLineColor(10);
//  leg->SetLineWidth(0.);
//  leg->SetTextFont(42);
//  leg->SetTextSize(0.035);
  leg->AddEntry(gr_Ko_3, "TAMU: di-quark 0-5%", "l");
  leg->AddEntry(gr_Ko_2, "TAMU: 3-quark 0-5%", "l");
  leg->AddEntry(gr_pythia8_CR, "PYTHIA8 (CR)", "l");
  leg->AddEntry(gr_pythia8, "PYTHIA8 (Monash)", "l");
//  leg->Draw();

  c1->Update();

  //  c1->SaveAs("fig/LcD0_proj_0_10_24B.pdf");
  //  c1->SaveAs("fig/LcD0_proj_0_10_24B.png");
  SaveCanvas(c1, "fig/" + TString(c1->GetName()), kTRUE);
}
