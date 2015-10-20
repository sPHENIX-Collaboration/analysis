// $Id: $

/*!
 * \file DrawEcal_Likelihood_Sum.C
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
#include <cassert>
#include "SaveCanvas.C"
#include "SetOKStyle.C"
using namespace std;

TFile * _file0 = NULL;
TTree * T = NULL;
TString cuts = "";

void
DrawEcal_Likelihood_Sum(
//    TString base_dir =
//        "/direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/",

    //        TString base_dir =
    //            "/direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/embedding/emcstudies/pidstudies/spacal2d/fieldmap/",
//            TString kine_config = "eta0"
            TString base_dir =
                "/direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/embedding/emcstudies/pidstudies/spacal1d/fieldmap/",
            TString kine_config = "eta0.90"
                )
{
  SetOKStyle();
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(1111);
  TVirtualFitter::SetDefaultFitter("Minuit2");

  gSystem->Load("libg4eval.so");
  gSystem->Load("libemcal_ana.so");
  gSystem->Load("libg4vertex.so");

  RejectionCurve(base_dir, kine_config, "Edep_LL_Distribution_eval_sample",
      "hll_edep_diff", 0.5);

  RejectionCurve(base_dir, kine_config, "EP_LL_Distribution_eval_sample",
      "hll_ep_diff", 0.5);

}

void
RejectionCurve(
    TString base_dir =
        "/direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/",
    TString kine_config = "eta0", //
    TString ll_config = "Edep_LL_Distribution_eval_sample", //
    TString hist_name = "hll_edep_diff", //
    const double min_eff = 0.85
    )
{

  TCanvas *c1 = new TCanvas("RejectionCurve"+ kine_config + "_" + ll_config + "_" + hist_name, "RejectionCurve"+ kine_config + "_" + ll_config + "_" + hist_name, 900, 900);
  c1->Divide(1, 1);
  int idx = 1;
  TPad * p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();

  p->DrawFrame(min_eff, 1, 1, 1e3, ";Electron Efficiency;Hadron Rejection");


//  TGraphErrors *ge = ExtractRejCurve(base_dir, kine_config + "_8GeV", ll_config,
//      hist_name);
//
//  ge->SetLineColor(kRed + 2);
//  ge->SetMarkerColor(kRed + 2);
//  ge->SetFillColor(kRed - 9);
//  ge->SetLineWidth(3);
//  ge->Draw("3");
//  ge->Draw("lx");

  TGraphErrors *ge = ExtractRejCurve(base_dir, kine_config + "_4GeV", ll_config,
      hist_name);

  ge->SetLineColor(kBlue + 2);
  ge->SetMarkerColor(kBlue + 2);
  ge->SetFillColor(kBlue - 9);
  ge->SetLineWidth(3);
  ge->Draw("3");
  ge->Draw("lx");



  TGraphErrors *ge = ExtractRejCurve(base_dir, kine_config + "_2GeV", ll_config,
      hist_name);

  ge->SetLineColor(kGreen + 3);
  ge->SetMarkerColor(kGreen + 3);
  ge->SetFillColor(kGreen - 9);
  ge->SetLineWidth(3);
  ge->Draw("3");
  ge->Draw("lx");

  SaveCanvas(c1,
      base_dir + TString("DrawEcal_Likelihood_Sum_") + TString(c1->GetName())
          , kFALSE);
}

TGraphErrors *
ExtractRejCurve(
    TString base_dir =
        "/direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/",
    TString kine_config = "eta0_4GeV", //
    TString ll_config = "EP_LL_Distribution_ll_sample", //
    TString hist_name = "hll_ep_diff" //
    )
{
  SetOKStyle();
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(1111);
  TVirtualFitter::SetDefaultFitter("Minuit2");

  gSystem->Load("libg4eval.so");
  gSystem->Load("libemcal_ana.so");
  gSystem->Load("libg4vertex.so");

  TH1F * hll_edep_diff_e = NULL;
  TH1F * hll_edep_diff_pi = NULL;
    {
      TFile * f =
          new TFile(
              base_dir + "G4Hits_sPHENIX_e-_" + kine_config
                  + "-ALL.root_Ana.root.lst_EMCalLikelihood.root_DrawEcal_Likelihood_"
                  + ll_config + ".root");

      assert(f);

      hll_edep_diff_e = (TH1F *) f->GetObjectChecked(hist_name, "TH1F");

      assert(hll_edep_diff_e);
    }

    {
      TFile * f =
          new TFile(
              base_dir + "/G4Hits_sPHENIX_pi-_" + kine_config
                  + "-ALL.root_Ana.root.lst_EMCalLikelihood.root_DrawEcal_Likelihood_"
                  + ll_config + ".root");

      assert(f);

      hll_edep_diff_pi = (TH1F *) f->GetObjectChecked(hist_name, "TH1F");

      assert(hll_edep_diff_pi);
    }
  assert(hll_edep_diff_e->GetNbinsX() == hll_edep_diff_pi->GetNbinsX());

  double threshold[10000] =
    { 0 };
  double eff_e[10000] =
    { 0 };
  double eff_err_e[10000] =
    { 0 };
  double rej_pi[10000] =
    { 0 };
  double rej_err_pi[10000] =
    { 0 };

  const double n_e = hll_edep_diff_e->GetSum();
  double pass = 0;
  int cnt = 0;

  for (int i = hll_edep_diff_e->GetNbinsX(); i >= 1; i--)
    {
      pass += hll_edep_diff_e->GetBinContent(i);

      const double pp = pass / n_e;
      //      const double z = 1.96;
      const double z = 1.;

      const double A = z
          * sqrt(1. / n_e * pp * (1 - pp) + z * z / 4 / n_e / n_e);
      const double B = 1 / (1 + z * z / n_e);

      threshold[cnt] = hll_edep_diff_e->GetBinCenter(i);
      eff_e[cnt] = (pp + z * z / 2 / n_e) * B;
      eff_err_e[cnt] = A * B;

      cnt++;
    }

  const double n_pi = hll_edep_diff_pi->GetSum();
  double pass = 0;
  int cnt = 0;

  for (int i = hll_edep_diff_pi->GetNbinsX(); i >= 1; i--)
    {
      pass += hll_edep_diff_pi->GetBinContent(i);

      const double pp = pass / n_pi;
      //      const double z = 1.96;
      const double z = 1.;

      const double A = z
          * sqrt(1. / n_pi * pp * (1 - pp) + z * z / 4 / n_pi / n_pi);
      const double B = 1 / (1 + z * z / n_pi);

      assert(threshold[cnt] == hll_edep_diff_pi->GetBinCenter(i));
      rej_pi[cnt] = (pp + z * z / 2 / n_pi) * B;
      rej_err_pi[cnt] = A * B;

      // eff -> rej
      rej_err_pi[cnt] = 1 / rej_pi[cnt] * (rej_err_pi[cnt] / rej_pi[cnt]);
      rej_pi[cnt] = 1 / rej_pi[cnt];

      cnt++;
    }

  TGraphErrors * ge = new TGraphErrors(cnt, eff_e, rej_pi, eff_err_e,
      rej_err_pi);

  return ge;

}

