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

bool mask = true;

void
DrawPrototype2EMCalTower_ResolutionRecalib(
//
    const TString base =
        "/phenix/u/jinhuang/tmp/miliped_work/Production_0414/calirbated_")
{
  SetOKStyle();
  gStyle->SetOptStat(0);
  gStyle->SetGridStyle(0);
  if (mask)
    gStyle->SetOptFit(0);
  else
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

  TText * t;
  TCanvas *c1 = new TCanvas(
      "DrawPrototype2EMCalTower_ResolutionRecalib_RunSummary"
          + TString(mask ? "Mask" : ""),
      "DrawPrototype2EMCalTower_ResolutionRecalib_RunSummary", 1800, 900);
  c1->Divide(3, 2);
  int idx = 1;
  TPad * p;
  for (int i = 0; i < N; ++i)
    {

      p = (TPad *) c1->cd(idx++);
      c1->Update();

      p->SetGridx(0);
      p->SetGridy(0);

      const double E = Es[i];

      vector<double> v(4);

//      v = GetOnePlot(base, runs[i], cut);
      v = RecalibratorMakeOnePlot(base, runs[i], E);

      mean[i] = v[0];
      dmean[i] = v[1];
      res[i] = v[2] / v[0];
      dres[i] = v[3] / v[0];

      cout << "mean[i] = " << mean[i] << ", " //
          << "dmean[i] = " << dmean[i] << ", " //
          << "res[i] = " << res[i] << ", " //
          << "dres[i] = " << dres[i] << endl;
    }

  SaveCanvas(c1, base + TString(c1->GetName()), kTRUE);

  TGraphErrors * ge_linear = new TGraphErrors(N, Es, mean, 0, dmean);
  TGraphErrors * ge_res = new TGraphErrors(N, Es, res, 0, dres);

  ge_linear->SetLineColor(kBlue + 3);
  ge_linear->SetMarkerColor(kBlue + 3);
  ge_linear->SetLineWidth(2);
  ge_linear->SetMarkerStyle(kFullCircle);
  ge_linear->SetMarkerSize(2);

  ge_res->SetLineColor(kBlue + 3);
  ge_res->SetMarkerColor(kBlue + 3);
  ge_res->SetLineWidth(2);
  ge_res->SetMarkerStyle(kFullCircle);
  ge_res->SetMarkerSize(2);

  TF1 * f_calo_r = new TF1("f_calo_r", "sqrt([0]*[0]+[1]*[1]/x)/100", 0.5, 25);
  TF1 * f_calo_l = new TF1("f_calo_l", "pol2", 0.5, 25);

  f_calo_r->SetLineColor(kBlue + 3);
  f_calo_r->SetLineWidth(3);

  TF1 * f_calo_sim = new TF1("f_calo_sim", "sqrt([0]*[0]+[1]*[1]/x)/100", 0.5,
      25);
  f_calo_sim->SetParameters(2.4, 11.8);
  f_calo_sim->SetLineWidth(3);
  f_calo_sim->SetLineColor(kGreen + 2);

  TF1 * f_calo_l_sim = new TF1("f_calo_l", "pol2", 0.5, 25);
//  f_calo_l_sim->SetParameters(-0.03389, 0.9666, -0.0002822);
  f_calo_l_sim->SetParameters(-0., 1, -0.);
//  f_calo_l_sim->SetLineWidth(1);
  f_calo_l_sim->SetLineColor(kGreen + 2);
  f_calo_l_sim->SetLineWidth(3);

  TFile * f_ref =
      new TFile(
          "/gpfs/mnt/gpfs02/sphenix/user/jinhuang/Prototype_2016/Production_0417_CEMC_MIP_set2_v3/DrawPrototype2EMCalTower_Resolution_Run2042_col1_row2_5x5_Valid_HODO_center_col1_row2.root");
  assert(f_ref->IsOpen());

  TGraphErrors * ge_res_ref = (TGraphErrors *) f_ref->GetObjectChecked("Graph",
      "TGraphErrors");
  assert(ge_res_ref);
  ge_res_ref->SetObjectStat(0);

  TF1 * f_calo_r_ref = (TF1 *) f_ref->GetObjectChecked("f_calo_r", "TF1");
  assert(f_calo_r_ref);

  ge_res_ref->SetLineColor(kBlue);
  ge_res_ref->SetMarkerColor(kBlue);
  ge_res_ref->SetLineWidth(2);
  ge_res_ref->SetMarkerStyle(kOpenCircle);
  ge_res_ref->SetMarkerSize(2);

  f_calo_r_ref->SetLineColor(kBlue);
  f_calo_r_ref->SetMarkerColor(kBlue);
  f_calo_r_ref->SetLineWidth(2);

  TText * t;
  TCanvas *c1 = new TCanvas(
      Form("DrawPrototype2EMCalTower_ResolutionRecalib%.0f_", runs[0]),
      Form("DrawPrototype2EMCalTower_ResolutionRecalib%.0f_", runs[0]), 1300,
      600);
  c1->Divide(2, 1);
  int idx = 1;
  TPad * p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
//  p->SetLogy();
  p->SetGridx(0);
  p->SetGridy(0);

  TLegend* leg = new TLegend(.15, .7, .6, .85);

  p->DrawFrame(0, 0, 25, 25,
      Form("Electron Linearity;Input energy (GeV);Measured Energy (GeV)"));
  TLine * l = new TLine(0, 0, 25, 25);
  l->SetLineColor(kGray);
//  l->Draw();

  f_calo_l_sim->Draw("same");
  ge_linear->Draw("p");
//  ge_linear->Fit(f_calo_l, "RM0");
//  f_calo_l->Draw("same");

  leg->AddEntry(ge_linear, "T-1044 data", "ep");
  leg->AddEntry(f_calo_l_sim, "Unity", "l");
  leg->Draw();

  p = (TPad *) c1->cd(idx++);
  c1->Update();
//  p->SetLogy();
  p->SetGridx(0);
  p->SetGridy(0);

  TH1 * hframe = p->DrawFrame(0, 0, 25, 0.2,
      Form("Resolution;Input energy (GeV);#DeltaE/<E>"));

  ge_res->Fit(f_calo_r, "RM0Q");

  f_calo_r_ref->Draw("same");
  ge_res_ref->Draw("p");
  f_calo_r->Draw("same");
  f_calo_sim->Draw("same");
  ge_res->Draw("p");

  TLegend* leg = new TLegend(.2, .6, .85, .9);
  leg->AddEntry(ge_res_ref, "T-1044, MIP calib.", "ep");
  leg->AddEntry(f_calo_r_ref,
      Form("#DeltaE/E = %.1f%% #oplus %.1f%%/#sqrt{E}",
          f_calo_r_ref->GetParameter(0), f_calo_r_ref->GetParameter(1)), "l");
//  leg->AddEntry(new TH1(), "", "l");
  leg->AddEntry((TObject*)0, " ", "");
  leg->AddEntry(ge_res, "T-1044, e-shower calib.", "ep");
  leg->AddEntry(f_calo_r,
      Form("#DeltaE/E = %.1f%% #oplus %.1f%%/#sqrt{E}",
          f_calo_r->GetParameter(0), f_calo_r->GetParameter(1)), "l");
  leg->Draw();

  TLegend* leg = new TLegend(.2, .15, .85, .3);

  leg->AddEntry(f_calo_sim,
      Form("Simulation, #DeltaE/E = %.1f%% #oplus %.1f%%/#sqrt{E}",
          f_calo_sim->GetParameter(0), f_calo_sim->GetParameter(1)), "l");
  leg->Draw();

  hframe->SetTitle("Electron Resolution");

  SaveCanvas(c1, base + TString(c1->GetName()), kTRUE);
}

vector<double>
RecalibratorMakeOnePlot(const TString base, const int run, const double E)
{
//  /phenix/u/jinhuang/tmp/miliped_work/Production_0414/calirbated_2040.dat

  TString fname = base + Form("%d.dat", run);
//  Prototype_e-_2_Seg0_DSTReader.root_DrawPrototype2EMCalTower_EMCDistribution_SUMall_event.root

  cout << "Process " << fname << endl;

  fstream f(fname, ios_base::in);
  assert(f.is_open());

  TH1 * EnergySum_LG_production = new TH1F(
      Form("EnergySum_LG_production_%d", run),
      Form(";Run %d: 5x5 EMCal Energy Sum (GeV);Count / bin", run), 400, 0, 30);
  TH1 * EnergySum_LG_recalib = new TH1F(Form("EnergySum_LG_recalib_%d", run),
      Form(";Run %d: 5x5 EMCal Energy Sum (GeV);Count / bin", run), 400, 0, 30);

  int count = 0;
  while (!f.eof())
    {
      string line;
      getline(f, line);

      if (line.length() == 0)
        continue;

      double old_E = -1, new_E = -1;
      char tab;

      stringstream sline(line);
      sline >> old_E >> tab >> new_E;

//      cout <<"line -> old_E = "<<old_E<<", new_E = "<<new_E<<endl;
      assert(old_E > 0);
      assert(new_E > 0);

      EnergySum_LG_production->Fill(old_E);
      EnergySum_LG_recalib->Fill(new_E);

    }

  EnergySum_LG_production->SetLineColor(kBlue + 3);
  EnergySum_LG_production->SetLineWidth(2);
  EnergySum_LG_production->SetMarkerStyle(kFullCircle);
  EnergySum_LG_production->SetMarkerColor(kBlue + 3);
  EnergySum_LG_recalib->SetLineColor(kRed + 3);
  EnergySum_LG_recalib->SetLineWidth(2);
  EnergySum_LG_recalib->SetMarkerStyle(kFullCircle);
  EnergySum_LG_recalib->SetMarkerColor(kRed + 3);

  EnergySum_LG_recalib->Sumw2();

  TH1 * h = (TH1 *) EnergySum_LG_recalib->DrawClone();
  if (!mask)
    EnergySum_LG_production->DrawClone("same");

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

//  h->Sumw2();
  h->GetXaxis()->SetRangeUser(h->GetMean() - 5 * h->GetRMS(),
      h->GetMean() + 5 * h->GetRMS());
  if (!mask)
    h->SetTitle(
        Form("#DeltaE/<E> = %.1f%% @ Beam = %.0f GeV",
            100 * fgaus->GetParameter(2) / fgaus->GetParameter(1), E));
  else
    h->SetTitle(Form("Beam = %.0f GeV", E));

  assert(fgaus);

  vector<double> v(4);
  v[0] = fgaus->GetParameter(1);
  v[1] = fgaus->GetParError(1);
  v[2] = fgaus->GetParameter(2);
  v[3] = fgaus->GetParError(2);

  return v;
}
