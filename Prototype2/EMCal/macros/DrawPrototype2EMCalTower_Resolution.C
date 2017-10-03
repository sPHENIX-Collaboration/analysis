// $Id: $                                                                                             

/*!
 * \file Draw.C
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
using namespace std;

void
DrawPrototype2EMCalTower_Resolution(
    //
    const TString base =
        "/gpfs/mnt/gpfs02/sphenix/user/jinhuang/Prototype_2016/Production_0417_CEMC_MIP_set2_v3/",
    TString cut = "col1_row2_5x5_Valid_HODO_center_col1_row2")
{
  SetOKStyle();
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);
  TVirtualFitter::SetDefaultFitter("Minuit2");

  const int N = 6;
  const double Es[] =
    { 2, 3, 4, 8, 12, 16 };
  const double runs[] =
    { 2042, 2040, 2039, 2038, 2067, 2063 };

  double mean[1000];
  double dmean[1000];
  double res[1000];
  double dres[1000];

  for (int i = 0; i < N; ++i)
    {
      const double E = Es[i];

      vector<double> v(4);

      v = GetOnePlot(base, runs[i], cut);

      mean[i] = v[0];
      dmean[i] = v[1];
      res[i] = v[2] / v[0];
      dres[i] = v[3] / v[0];

      cout << "mean[i] = " << mean[i] << ", " //
          << "dmean[i] = " << dmean[i] << ", " //
          << "res[i] = " << res[i] << ", " //
          << "dres[i] = " << dres[i] << endl;
    }

  TGraphErrors * ge_linear = new TGraphErrors(N, Es, mean, 0, dmean);
  TGraphErrors * ge_res = new TGraphErrors(N, Es, res, 0, dres);

  ge_linear->SetLineColor(kBlue + 3);
  ge_linear->SetMarkerColor(kBlue + 3);
  ge_linear->SetLineWidth(2);
  ge_linear->SetMarkerStyle(kFullCircle);
  ge_linear->SetMarkerSize(1.5);

  ge_res->SetLineColor(kBlue + 3);
  ge_res->SetMarkerColor(kBlue + 3);
  ge_res->SetLineWidth(2);
  ge_res->SetMarkerStyle(kFullCircle);
  ge_res->SetMarkerSize(1.5);

  TF1 * f_calo_r = new TF1("f_calo_r", "sqrt([0]*[0]+[1]*[1]/x)/100", 0.5, 25);
  TF1 * f_calo_l = new TF1("f_calo_l", "pol2", 0.5, 25);

  TF1 * f_calo_sim = new TF1("f_calo_sim", "sqrt([0]*[0]+[1]*[1]/x)/100", 0.5,
      25);
  f_calo_sim->SetParameters(2.4, 11.8);
  f_calo_sim->SetLineWidth(1);
  f_calo_sim->SetLineColor(kGreen + 2);

  TF1 * f_calo_l_sim = new TF1("f_calo_l_sim", "pol2", 0.5, 25);
  f_calo_l_sim->SetParameters(-0.03389, 0.9666, -0.0002822);
  f_calo_l_sim->SetLineWidth(1);
  f_calo_l_sim->SetLineColor(kGreen + 2);

  TText * t;
  TCanvas *c1 = new TCanvas(
      Form("DrawPrototype2EMCalTower_Resolution_Run%.0f_", runs[0]) + cut,
      Form("DrawPrototype2EMCalTower_Resolution_Run%.0f_", runs[0]) + cut, 1300,
      600);
  c1->Divide(2, 1);
  int idx = 1;
  TPad * p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
//  p->SetLogy();
  p->SetGridx(0);
  p->SetGridy(0);

  p->DrawFrame(0, 0, 25, 30,
      Form("Linearity;Input energy (GeV);Measured Energy (GeV)", E));
  TLine * l = new TLine(0, 0, 25, 25);
  l->SetLineColor(kGray);
//  l->Draw();

  ge_linear->Draw("p");
  ge_linear->Fit(f_calo_l, "RM0");
  f_calo_l->Draw("same");
  f_calo_l_sim->Draw("same");

  p = (TPad *) c1->cd(idx++);
  c1->Update();
//  p->SetLogy();
  p->SetGridx(0);
  p->SetGridy(0);

  TH1 * hframe = p->DrawFrame(0, 0, 25, 0.2,
      Form("Resolution;Input energy (GeV);#DeltaE/<E>", E));

  ge_res->Draw("p");
  ge_res->Fit(f_calo_r, "RM0");
  f_calo_r->Draw("same");
  f_calo_sim->Draw("same");

  hframe->SetTitle(
      Form("#DeltaE/E = %.1f%% #oplus %.1f%%/#sqrt{E}",
          f_calo_r->GetParameter(0), f_calo_r->GetParameter(1)));

  SaveCanvas(c1, base + TString(c1->GetName()), kTRUE);
}

vector<double>
GetOnePlot(const TString base, const int run, TString cut)
{
//  beam_00002063-0000_DSTReader.root_DrawPrototype2EMCalTower_EMCDistribution_SUM_Energy_Sum_col1_row2_5x5_tower_center_col1_row2.png
//  TString fname = base + TString("Prototype_") + particle + "_" + sE
//      + "_SegALL_DSTReader.root_DrawPrototype2EMCalTower_EMCDistribution_SUM_all_event.root";
  TString fname =
      base
          + Form(
              "/beam_0000%d-0000_DSTReader.root_DrawPrototype2EMCalTower_EMCDistribution_SUM_Energy_Sum_",
              run) + cut + ".root";
//  Prototype_e-_2_Seg0_DSTReader.root_DrawPrototype2EMCalTower_EMCDistribution_SUMall_event.root

  cout << "Process " << fname << endl;
  TFile * f = new TFile(fname);
  assert(f->IsOpen());

  TH1 * hEnergySum = (TH1 *) f->GetObjectChecked("EnergySum_LG", "TH1");
  assert(hEnergySum);
  new TCanvas();
  TH1 * h = hEnergySum->DrawClone();

  hEnergySum->Scale(1. / hEnergySum->Integral(1, -1));
//  TF1 * fgaus = hEnergySum->GetFunction("fgaus_LG");
//  assert(fgaus);

  TF1 * fgaus_g = new TF1("fgaus_LG_g", "gaus", h->GetMean() - 1 * h->GetRMS(),
      h->GetMean() + 4 * h->GetRMS());
  fgaus_g->SetParameters(1, h->GetMean() - 2 * h->GetRMS(),
      h->GetMean() + 2 * h->GetRMS());
  h->Fit(fgaus_g, "MR0N");

  TF1 * fgaus = new TF1("fgaus_LG", "gaus",
      fgaus_g->GetParameter(1) - 2 * fgaus_g->GetParameter(2),
      fgaus_g->GetParameter(1) + 2 * fgaus_g->GetParameter(2));
  fgaus->SetParameters(fgaus_g->GetParameter(0), fgaus_g->GetParameter(1),
      fgaus_g->GetParameter(2));
  fgaus->SetLineColor(kRed);
  fgaus->SetLineWidth(4);
  h->Fit(fgaus, "MR");

  vector<double> v(4);
  v[0] = fgaus->GetParameter(1);
  v[1] = fgaus->GetParError(1);
  v[2] = fgaus->GetParameter(2);
  v[3] = fgaus->GetParError(2);

  return v;
}

