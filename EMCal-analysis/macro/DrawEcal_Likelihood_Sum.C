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
//        "../../sPHENIX_work/production_analysis_cemc2x2/emcstudies/pidstudies/spacal2d/fieldmap/",

    TString base_dir =
        "../../sPHENIX_work/production_analysis_cemc2x2/embedding/emcstudies/pidstudies/spacal2d/fieldmap/",
//                    TString base_dir =
//                        "../../sPHENIX_work/production_analysis_cemc2x2/embedding/emcstudies/pidstudies/spacal1d/fieldmap/",
    TString kine_config = "eta0"
//            TString kine_config = "eta0.90"
    )
{
  SetOKStyle();
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(1111);
  TVirtualFitter::SetDefaultFitter("Minuit2");

  gSystem->Load("libg4eval.so");
  gSystem->Load("libemcal_ana.so");
  gSystem->Load("libg4vertex.so");

//  RejectionCurve(base_dir, kine_config, "Edep_LL_Distribution_eval_sample",
//      "hll_edep_diff", 0.6);
////
//  RejectionCurvePos(base_dir, kine_config, "Edep_LL_Distribution_eval_sample",
//      "hll_edep_diff", 0.6);

//  RejectionCurve(base_dir, kine_config, "EP_LL_Distribution_eval_sample",
//      "hll_ep_diff", 0.6);

//  RejectionCurve(base_dir, kine_config, "Edep_LL_Distribution_eval_sample",
//      "hll_edep_diff", 0.85);
//  RejectionCurvePos(base_dir, kine_config, "Edep_LL_Distribution_eval_sample",
//      "hll_edep_diff", 0.85);

//  RejectionCurve(base_dir, kine_config, "EP_LL_Distribution_eval_sample",
//      "hll_ep_diff", 0.85);

//  RejectionCurve_PiProton(base_dir, kine_config,
//      "Edep_LL_Distribution_eval_sample", "hll_edep_diff", 0.85);
//  RejectionCurve_PiProtonPos(base_dir, kine_config,
//      "Edep_LL_Distribution_eval_sample", "hll_edep_diff", 0.85);
//
//  RejectionCurve_PiProton(base_dir, kine_config,
//      "EP_LL_Distribution_eval_sample", "hll_ep_diff", 0.85);
//
//  RejectionCurve_RejMethodComparison();
//  RejectionCurve_RejMethodComparison_AntiProton();
//  RejectionCurve_ppSummary();
//  RejectionCurve_ppSummaryPos();

//  RejectionCurve_AuAuSummary("../../sPHENIX_work/production_analysis_cemc2x2/embedding/emcstudies/pidstudies/spacal2d/fieldmap/",
//      "2-D Proj. SPACAL, e^{-} VS h^{-} in Au+Au 10%C");
  RejectionCurve_AuAuSummaryPos("../../sPHENIX_work/production_analysis_cemc2x2/embedding/emcstudies/pidstudies/spacal2d/fieldmap/",
      "2-D Proj. SPACAL, e^{+} VS h^{+} in Au+Au 10%C");
//    RejectionCurve_AuAuSummary("../../sPHENIX_work/production_analysis_cemc2x2/embedding/emcstudies/pidstudies/spacal1d/fieldmap/",
//        "1-D Proj. SPACAL, e^{-} VS h^{-} in Au+Au 10%C");
//    RejectionCurve_AuAuSummaryPos("../../sPHENIX_work/production_analysis_cemc2x2/embedding/emcstudies/pidstudies/spacal1d/fieldmap/",
//        "1-D Proj. SPACAL, e^{+} VS h^{+} in Au+Au 10%C");

  //  RejectionCurve_AuAuSummary_Compare();
//    RejectionCurve_AuAuSummary_Compare2();

}

void
RejectionCurvePos(
    TString base_dir =
        "../../sPHENIX_work/production_analysis_cemc2x2/emcstudies/pidstudies/spacal2d/fieldmap/",
    TString kine_config = "eta0", //
    TString ll_config = "Edep_LL_Distribution_eval_sample", //
    TString hist_name = "hll_edep_diff", //
    const double min_eff = 0.85)
{

  TCanvas *c1 = new TCanvas(
      "RejectionCurvePos" + kine_config + "_" + ll_config + "_" + hist_name,
      "RejectionCurvePos" + kine_config + "_" + ll_config + "_" + hist_name,
      900, 900);
  c1->Divide(1, 1);
  int idx = 1;
  TPad * p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();

  p->DrawFrame(min_eff, 1, 1, 1e3,
      ";Electron Efficiency;Positively Charged Hadron Rejection");

  TGraphErrors *ge = ExtractRejCurve(base_dir, kine_config + "_8GeV", ll_config,
      hist_name, "e+", "pi+");

  ge->SetLineColor(kRed + 2);
  ge->SetMarkerColor(kRed + 2);
  ge->SetFillColor(kRed - 9);
  ge->SetLineWidth(3);
//  ge->Draw("3");
  ge->Draw("lx");
  TGraphErrors *ge = ExtractRejCurve(base_dir, kine_config + "_8GeV", ll_config,
      hist_name, "e+", "kaon+");

  ge->SetLineColor(kRed + 2);
  ge->SetMarkerColor(kRed + 2);
  ge->SetFillColor(kRed - 9);
  ge->SetLineWidth(3);
  ge->SetLineStyle(kDashed);
//  ge->Draw("3");
  ge->Draw("lx");
  TGraphErrors *ge = ExtractRejCurve(base_dir, kine_config + "_8GeV", ll_config,
      hist_name, "e+", "proton");
  ge->SetLineColor(kRed + 2);
  ge->SetMarkerColor(kRed + 2);
  ge->SetFillColor(kRed - 9);
  ge->SetLineWidth(3);
  ge->SetLineStyle(kDotted);
//  ge->Draw("3");
  ge->Draw("lx");

  TGraphErrors *ge = ExtractRejCurve(base_dir, kine_config + "_4GeV", ll_config,
      hist_name, "e+", "pi+");
  ge->SetLineColor(kBlue + 2);
  ge->SetMarkerColor(kBlue + 2);
  ge->SetFillColor(kBlue - 9);
  ge->SetLineWidth(3);
//  ge->Draw("3");
  ge->Draw("lx");
  TGraphErrors *ge = ExtractRejCurve(base_dir, kine_config + "_4GeV", ll_config,
      hist_name, "e+", "kaon+");

  ge->SetLineColor(kBlue + 2);
  ge->SetMarkerColor(kBlue + 2);
  ge->SetFillColor(kBlue - 9);
  ge->SetLineWidth(3);
  ge->SetLineStyle(kDashed);
//  ge->Draw("3");
  ge->Draw("lx");
  TGraphErrors *ge = ExtractRejCurve(base_dir, kine_config + "_4GeV", ll_config,
      hist_name, "e+", "proton");
  ge->SetLineColor(kBlue + 2);
  ge->SetMarkerColor(kBlue + 2);
  ge->SetFillColor(kBlue - 9);
  ge->SetLineWidth(3);
  ge->SetLineStyle(kDotted);
//  ge->Draw("3");
  ge->Draw("lx");

  TGraphErrors *ge = ExtractRejCurve(base_dir, kine_config + "_2GeV", ll_config,
      hist_name, "e+", "pi+");

  ge->SetLineColor(kGreen + 3);
  ge->SetMarkerColor(kGreen + 3);
  ge->SetFillColor(kGreen - 9);
  ge->SetLineWidth(3);
//  ge->Draw("3");
  ge->Draw("lx");
  TGraphErrors *ge = ExtractRejCurve(base_dir, kine_config + "_2GeV", ll_config,
      hist_name, "e+", "kaon+");
  ge->SetLineColor(kGreen + 3);
  ge->SetMarkerColor(kGreen + 3);
  ge->SetFillColor(kGreen - 9);
  ge->SetLineWidth(3);
  ge->SetLineStyle(kDashed);
//  ge->Draw("3");
  ge->Draw("lx");
  TGraphErrors *ge = ExtractRejCurve(base_dir, kine_config + "_2GeV", ll_config,
      hist_name, "e+", "proton");
  ge->SetLineColor(kGreen + 3);
  ge->SetMarkerColor(kGreen + 3);
  ge->SetFillColor(kGreen - 9);
  ge->SetLineWidth(3);
  ge->SetLineStyle(kDotted);
//  ge->Draw("3");
  ge->Draw("lx");

  SaveCanvas(c1,
      base_dir + TString("DrawEcal_Likelihood_Sum_") + TString(c1->GetName()),
      kFALSE);
}

void
RejectionCurve(
    TString base_dir =
        "../../sPHENIX_work/production_analysis_cemc2x2/emcstudies/pidstudies/spacal2d/fieldmap/",
    TString kine_config = "eta0", //
    TString ll_config = "Edep_LL_Distribution_eval_sample", //
    TString hist_name = "hll_edep_diff", //
    const double min_eff = 0.85)
{

  TCanvas *c1 = new TCanvas(
      "RejectionCurve" + kine_config + "_" + ll_config + "_" + hist_name,
      "RejectionCurve" + kine_config + "_" + ll_config + "_" + hist_name, 900,
      900);
  c1->Divide(1, 1);
  int idx = 1;
  TPad * p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();

  p->DrawFrame(min_eff, 1, 1, 1e3, ";Electron Efficiency;Hadron Rejection");

  TGraphErrors *ge = ExtractRejCurve(base_dir, kine_config + "_8GeV", ll_config,
      hist_name);

  ge->SetLineColor(kRed + 2);
  ge->SetMarkerColor(kRed + 2);
  ge->SetFillColor(kRed - 9);
  ge->SetLineWidth(3);
//  ge->Draw("3");
  ge->Draw("lx");
  TGraphErrors *ge = ExtractRejCurve(base_dir, kine_config + "_8GeV", ll_config,
      hist_name, "e-", "kaon-");

  ge->SetLineColor(kRed + 2);
  ge->SetMarkerColor(kRed + 2);
  ge->SetFillColor(kRed - 9);
  ge->SetLineWidth(3);
  ge->SetLineStyle(kDashed);
//  ge->Draw("3");
  ge->Draw("lx");
  TGraphErrors *ge = ExtractRejCurve(base_dir, kine_config + "_8GeV", ll_config,
      hist_name, "e-", "anti_proton");
  ge->SetLineColor(kRed + 2);
  ge->SetMarkerColor(kRed + 2);
  ge->SetFillColor(kRed - 9);
  ge->SetLineWidth(3);
  ge->SetLineStyle(kDotted);
//  ge->Draw("3");
  ge->Draw("lx");

  TGraphErrors *ge = ExtractRejCurve(base_dir, kine_config + "_4GeV", ll_config,
      hist_name);
  ge->SetLineColor(kBlue + 2);
  ge->SetMarkerColor(kBlue + 2);
  ge->SetFillColor(kBlue - 9);
  ge->SetLineWidth(3);
//  ge->Draw("3");
  ge->Draw("lx");
  TGraphErrors *ge = ExtractRejCurve(base_dir, kine_config + "_4GeV", ll_config,
      hist_name, "e-", "kaon-");

  ge->SetLineColor(kBlue + 2);
  ge->SetMarkerColor(kBlue + 2);
  ge->SetFillColor(kBlue - 9);
  ge->SetLineWidth(3);
  ge->SetLineStyle(kDashed);
//  ge->Draw("3");
  ge->Draw("lx");
  TGraphErrors *ge = ExtractRejCurve(base_dir, kine_config + "_4GeV", ll_config,
      hist_name, "e-", "anti_proton");
  ge->SetLineColor(kBlue + 2);
  ge->SetMarkerColor(kBlue + 2);
  ge->SetFillColor(kBlue - 9);
  ge->SetLineWidth(3);
  ge->SetLineStyle(kDotted);
//  ge->Draw("3");
  ge->Draw("lx");

  TGraphErrors *ge = ExtractRejCurve(base_dir, kine_config + "_2GeV", ll_config,
      hist_name);

  ge->SetLineColor(kGreen + 3);
  ge->SetMarkerColor(kGreen + 3);
  ge->SetFillColor(kGreen - 9);
  ge->SetLineWidth(3);
//  ge->Draw("3");
  ge->Draw("lx");
  TGraphErrors *ge = ExtractRejCurve(base_dir, kine_config + "_2GeV", ll_config,
      hist_name, "e-", "kaon-");
  ge->SetLineColor(kGreen + 3);
  ge->SetMarkerColor(kGreen + 3);
  ge->SetFillColor(kGreen - 9);
  ge->SetLineWidth(3);
  ge->SetLineStyle(kDashed);
//  ge->Draw("3");
  ge->Draw("lx");
  TGraphErrors *ge = ExtractRejCurve(base_dir, kine_config + "_2GeV", ll_config,
      hist_name, "e-", "anti_proton");
  ge->SetLineColor(kGreen + 3);
  ge->SetMarkerColor(kGreen + 3);
  ge->SetFillColor(kGreen - 9);
  ge->SetLineWidth(3);
  ge->SetLineStyle(kDotted);
//  ge->Draw("3");
  ge->Draw("lx");

  SaveCanvas(c1,
      base_dir + TString("DrawEcal_Likelihood_Sum_") + TString(c1->GetName()),
      kFALSE);
}

void
RejectionCurve_PiProton(
    TString base_dir =
        "../../sPHENIX_work/production_analysis_cemc2x2/emcstudies/pidstudies/spacal2d/fieldmap/",
    TString kine_config = "eta0", //
    TString ll_config = "Edep_LL_Distribution_eval_sample", //
    TString hist_name = "hll_edep_diff", //
    const double min_eff = 0.85)
{

  TCanvas *c1 = new TCanvas(
      "RejectionCurve_PiProton" + kine_config + "_" + ll_config + "_"
          + hist_name,
      "RejectionCurve_PiProton" + kine_config + "_" + ll_config + "_"
          + hist_name, 900, 900);
  c1->Divide(1, 1);
  int idx = 1;
  TPad * p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();

  p->DrawFrame(min_eff, 10, 1, 1e3, ";Electron Efficiency;Hadron Rejection");

  TGraphErrors *ge = ExtractRejCurve(base_dir, kine_config + "_8GeV", ll_config,
      hist_name);

  ge->SetLineColor(kRed + 2);
  ge->SetMarkerColor(kRed + 2);
  ge->SetFillColor(kRed - 9);
  ge->SetLineWidth(3);
//  ge->Draw("3");
  ge->Draw("lx");
//  TGraphErrors *ge = ExtractRejCurve(base_dir, kine_config + "_8GeV", ll_config,
//      hist_name, "e-", "kaon-");
//
//  ge->SetLineColor(kRed + 2);
//  ge->SetMarkerColor(kRed + 2);
//  ge->SetFillColor(kRed - 9);
//  ge->SetLineWidth(3);
//  ge->SetLineStyle(kDashed);
////  ge->Draw("3");
//  ge->Draw("lx");
  TGraphErrors *ge = ExtractRejCurve(base_dir, kine_config + "_8GeV", ll_config,
      hist_name, "e-", "anti_proton");
  ge->SetLineColor(kRed + 2);
  ge->SetMarkerColor(kRed + 2);
  ge->SetFillColor(kRed - 9);
  ge->SetLineWidth(3);
  ge->SetLineStyle(kDotted);
//  ge->Draw("3");
  ge->Draw("lx");

  TGraphErrors *ge = ExtractRejCurve(base_dir, kine_config + "_4GeV", ll_config,
      hist_name);
  ge->SetLineColor(kBlue + 2);
  ge->SetMarkerColor(kBlue + 2);
  ge->SetFillColor(kBlue - 9);
  ge->SetLineWidth(3);
//  ge->Draw("3");
  ge->Draw("lx");
//  TGraphErrors *ge = ExtractRejCurve(base_dir, kine_config + "_4GeV", ll_config,
//      hist_name, "e-", "kaon-");
//
//  ge->SetLineColor(kBlue + 2);
//  ge->SetMarkerColor(kBlue + 2);
//  ge->SetFillColor(kBlue - 9);
//  ge->SetLineWidth(3);
//  ge->SetLineStyle(kDashed);
////  ge->Draw("3");
//  ge->Draw("lx");
  TGraphErrors *ge = ExtractRejCurve(base_dir, kine_config + "_4GeV", ll_config,
      hist_name, "e-", "anti_proton");
  ge->SetLineColor(kBlue + 2);
  ge->SetMarkerColor(kBlue + 2);
  ge->SetFillColor(kBlue - 9);
  ge->SetLineWidth(3);
  ge->SetLineStyle(kDotted);
//  ge->Draw("3");
  ge->Draw("lx");

  TGraphErrors *ge = ExtractRejCurve(base_dir, kine_config + "_2GeV", ll_config,
      hist_name);

  ge->SetLineColor(kGreen + 3);
  ge->SetMarkerColor(kGreen + 3);
  ge->SetFillColor(kGreen - 9);
  ge->SetLineWidth(3);
//  ge->Draw("3");
  ge->Draw("lx");
//  TGraphErrors *ge = ExtractRejCurve(base_dir, kine_config + "_2GeV", ll_config,
//      hist_name, "e-", "kaon-");
//  ge->SetLineColor(kGreen + 3);
//  ge->SetMarkerColor(kGreen + 3);
//  ge->SetFillColor(kGreen - 9);
//  ge->SetLineWidth(3);
//  ge->SetLineStyle(kDashed);
////  ge->Draw("3");
//  ge->Draw("lx");
  TGraphErrors *ge = ExtractRejCurve(base_dir, kine_config + "_2GeV", ll_config,
      hist_name, "e-", "anti_proton");
  ge->SetLineColor(kGreen + 3);
  ge->SetMarkerColor(kGreen + 3);
  ge->SetFillColor(kGreen - 9);
  ge->SetLineWidth(3);
  ge->SetLineStyle(kDotted);
//  ge->Draw("3");
  ge->Draw("lx");

  SaveCanvas(c1,
      base_dir + TString("DrawEcal_Likelihood_Sum_") + TString(c1->GetName()),
      kFALSE);
}

void
RejectionCurve_PiProtonPos(
    TString base_dir =
        "../../sPHENIX_work/production_analysis_cemc2x2/emcstudies/pidstudies/spacal2d/fieldmap/",
    TString kine_config = "eta0", //
    TString ll_config = "Edep_LL_Distribution_eval_sample", //
    TString hist_name = "hll_edep_diff", //
    const double min_eff = 0.85)
{

  TCanvas *c1 = new TCanvas(
      "RejectionCurve_PiProtonPos" + kine_config + "_" + ll_config + "_"
          + hist_name,
      "RejectionCurve_PiProtonPos" + kine_config + "_" + ll_config + "_"
          + hist_name, 900, 900);
  c1->Divide(1, 1);
  int idx = 1;
  TPad * p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();

  p->DrawFrame(min_eff, 10, 1, 1e3,
      ";Electron Efficiency;Positively Charged Hadron Rejection");

  TGraphErrors *ge = ExtractRejCurve(base_dir, kine_config + "_8GeV", ll_config,
      hist_name, "e+", "pi+");

  ge->SetLineColor(kRed + 2);
  ge->SetMarkerColor(kRed + 2);
  ge->SetFillColor(kRed - 9);
  ge->SetLineWidth(3);
//  ge->Draw("3");
  ge->Draw("lx");
//  TGraphErrors *ge = ExtractRejCurve(base_dir, kine_config + "_8GeV", ll_config,
//      hist_name, "e-", "kaon-");
//
//  ge->SetLineColor(kRed + 2);
//  ge->SetMarkerColor(kRed + 2);
//  ge->SetFillColor(kRed - 9);
//  ge->SetLineWidth(3);
//  ge->SetLineStyle(kDashed);
////  ge->Draw("3");
//  ge->Draw("lx");
  TGraphErrors *ge = ExtractRejCurve(base_dir, kine_config + "_8GeV", ll_config,
      hist_name, "e+", "proton");
  ge->SetLineColor(kRed + 2);
  ge->SetMarkerColor(kRed + 2);
  ge->SetFillColor(kRed - 9);
  ge->SetLineWidth(3);
  ge->SetLineStyle(kDotted);
//  ge->Draw("3");
  ge->Draw("lx");

  TGraphErrors *ge = ExtractRejCurve(base_dir, kine_config + "_4GeV", ll_config,
      hist_name, "e+", "pi+");
  ge->SetLineColor(kBlue + 2);
  ge->SetMarkerColor(kBlue + 2);
  ge->SetFillColor(kBlue - 9);
  ge->SetLineWidth(3);
//  ge->Draw("3");
  ge->Draw("lx");
//  TGraphErrors *ge = ExtractRejCurve(base_dir, kine_config + "_4GeV", ll_config,
//      hist_name, "e-", "kaon-");
//
//  ge->SetLineColor(kBlue + 2);
//  ge->SetMarkerColor(kBlue + 2);
//  ge->SetFillColor(kBlue - 9);
//  ge->SetLineWidth(3);
//  ge->SetLineStyle(kDashed);
////  ge->Draw("3");
//  ge->Draw("lx");
  TGraphErrors *ge = ExtractRejCurve(base_dir, kine_config + "_4GeV", ll_config,
      hist_name, "e+", "proton");
  ge->SetLineColor(kBlue + 2);
  ge->SetMarkerColor(kBlue + 2);
  ge->SetFillColor(kBlue - 9);
  ge->SetLineWidth(3);
  ge->SetLineStyle(kDotted);
//  ge->Draw("3");
  ge->Draw("lx");

  TGraphErrors *ge = ExtractRejCurve(base_dir, kine_config + "_2GeV", ll_config,
      hist_name, "e+", "pi+");

  ge->SetLineColor(kGreen + 3);
  ge->SetMarkerColor(kGreen + 3);
  ge->SetFillColor(kGreen - 9);
  ge->SetLineWidth(3);
//  ge->Draw("3");
  ge->Draw("lx");
//  TGraphErrors *ge = ExtractRejCurve(base_dir, kine_config + "_2GeV", ll_config,
//      hist_name, "e-", "kaon-");
//  ge->SetLineColor(kGreen + 3);
//  ge->SetMarkerColor(kGreen + 3);
//  ge->SetFillColor(kGreen - 9);
//  ge->SetLineWidth(3);
//  ge->SetLineStyle(kDashed);
////  ge->Draw("3");
//  ge->Draw("lx");
  TGraphErrors *ge = ExtractRejCurve(base_dir, kine_config + "_2GeV", ll_config,
      hist_name, "e+", "proton");
  ge->SetLineColor(kGreen + 3);
  ge->SetMarkerColor(kGreen + 3);
  ge->SetFillColor(kGreen - 9);
  ge->SetLineWidth(3);
  ge->SetLineStyle(kDotted);
//  ge->Draw("3");
  ge->Draw("lx");

  SaveCanvas(c1,
      base_dir + TString("DrawEcal_Likelihood_Sum_") + TString(c1->GetName()),
      kFALSE);
}

void
RejectionCurve_RejMethodComparison(
    TString base_dir =
        "../../sPHENIX_work/production_analysis_cemc2x2/emcstudies/pidstudies/spacal2d/fieldmap/",
    TString kine_config = "eta0", //
    const double min_eff = 0.85)
{

  TCanvas *c1 = new TCanvas("RejectionCurve_RejMethodComparison_" + kine_config,
      "RejectionCurve_RejMethodComparison_" + kine_config, 900, 900);
  c1->Divide(1, 1);
  int idx = 1;
  TPad * p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();
  p->SetGridx(0);
  p->SetGridy(0);

  p->DrawFrame(min_eff, 10, 1, 1e3, ";Electron Efficiency;Hadron Rejection");
  TLine * l = new TLine(0.95, 10, 0.95, 1e3);
  l->SetLineColor(kGray);
  l->SetLineWidth(5);
  l->Draw();

  TLatex * text = new TLatex(0.85, 1e3 * 1.1,
      "2-D Proj. SPACAL, e^{-}-#pi^{-} separation @ 0 < #eta < 0.1");
  text->SetTextAlign(11);
  text->SetTextSize(0.044);
  text->SetTextFont(42);
  text->Draw();

  TLegend * leg1 = new TLegend(0.12, 0.15, 0.65, 0.4);
  TLegend * leg2 = new TLegend(0.65, 0.15, 0.85, 0.4);

  TGraphErrors *ge = new TGraphErrors();
  ge->SetLineWidth(3);
  leg1->AddEntry(ge, "EMCal+HCal Likelihood", "lx");
  TGraphErrors *ge = new TGraphErrors();
  ge->SetLineWidth(3);
  ge->SetLineStyle(kDashed);
  leg1->AddEntry(ge, "EMCal E/p Likelihood", "lx");
  TGraphErrors *ge = new TGraphErrors();
  ge->SetLineWidth(3);
  ge->SetLineStyle(kDotted);
  leg1->AddEntry(ge, "Cut on min. E/p", "lx");

  TGraphErrors *ge = ExtractRejCurve(base_dir, kine_config + "_8GeV", //
  "Edep_LL_Distribution_eval_sample", "hll_edep_diff");
  ge->SetLineColor(kRed + 2);
  ge->SetMarkerColor(kRed + 2);
  ge->SetFillColor(kRed - 9);
  ge->SetLineWidth(3);
//  ge->Draw("3");
  ge->Draw("lx");
  leg2->AddEntry(ge, "8 GeV/c", "lx");
  TGraphErrors *ge = ExtractRejCurve(base_dir, kine_config + "_8GeV", //
  "EP_LL_Distribution_eval_sample", "hll_ep_diff");
  ge->SetLineColor(kRed + 2);
  ge->SetMarkerColor(kRed + 2);
  ge->SetFillColor(kRed - 9);
  ge->SetLineWidth(3);
  ge->SetLineStyle(kDashed);
//  ge->Draw("3");
  ge->Draw("lx");
  TGraphErrors *ge = ExtractRejCurve(base_dir, kine_config + "_8GeV", //
  "Ep_Checks_eval_sample", "hEMCalTrk_get_ep");
  ge->SetLineColor(kRed + 2);
  ge->SetMarkerColor(kRed + 2);
  ge->SetFillColor(kRed - 9);
  ge->SetLineWidth(3);
  ge->SetLineStyle(kDotted);
//  ge->Draw("3");
  ge->Draw("lx");

  TGraphErrors *ge = ExtractRejCurve(base_dir, kine_config + "_4GeV", //
  "Edep_LL_Distribution_eval_sample", "hll_edep_diff");
  ge->SetLineColor(kBlue + 2);
  ge->SetMarkerColor(kBlue + 2);
  ge->SetFillColor(kBlue - 9);
  ge->SetLineWidth(3);
//  ge->Draw("3");
  ge->Draw("lx");
  leg2->AddEntry(ge, "4 GeV/c", "lx");
  TGraphErrors *ge = ExtractRejCurve(base_dir, kine_config + "_4GeV", //
  "EP_LL_Distribution_eval_sample", "hll_ep_diff");
  ge->SetLineColor(kBlue + 2);
  ge->SetMarkerColor(kBlue + 2);
  ge->SetFillColor(kBlue - 9);
  ge->SetLineWidth(3);
  ge->SetLineStyle(kDashed);
//  ge->Draw("3");
  ge->Draw("lx");
  TGraphErrors *ge = ExtractRejCurve(base_dir, kine_config + "_4GeV", //
  "Ep_Checks_eval_sample", "hEMCalTrk_get_ep");
  ge->SetLineColor(kBlue + 2);
  ge->SetMarkerColor(kBlue + 2);
  ge->SetFillColor(kBlue - 9);
  ge->SetLineWidth(3);
  ge->SetLineStyle(kDotted);
//  ge->Draw("3");
  ge->Draw("lx");
//
  TGraphErrors *ge = ExtractRejCurve(base_dir, kine_config + "_2GeV", //
  "Edep_LL_Distribution_eval_sample", "hll_edep_diff");
  ge->SetLineColor(kGreen + 3);
  ge->SetMarkerColor(kGreen + 3);
  ge->SetFillColor(kGreen - 9);
  ge->SetLineWidth(3);
//  ge->Draw("3");
  ge->Draw("lx");
  leg2->AddEntry(ge, "2 GeV/c", "lx");
  TGraphErrors *ge = ExtractRejCurve(base_dir, kine_config + "_2GeV", //
  "EP_LL_Distribution_eval_sample", "hll_ep_diff");
  ge->SetLineColor(kGreen + 3);
  ge->SetMarkerColor(kGreen + 3);
  ge->SetFillColor(kGreen - 9);
  ge->SetLineWidth(3);
  ge->SetLineStyle(kDashed);
//  ge->Draw("3");
  ge->Draw("lx");
  TGraphErrors *ge = ExtractRejCurve(base_dir, kine_config + "_2GeV", //
  "Ep_Checks_eval_sample", "hEMCalTrk_get_ep");
  ge->SetLineColor(kGreen + 3);
  ge->SetMarkerColor(kGreen + 3);
  ge->SetFillColor(kGreen - 9);
  ge->SetLineWidth(3);
  ge->SetLineStyle(kDotted);
//  ge->Draw("3");
  ge->Draw("lx");

  leg2->Draw();
  leg1->Draw();

  SaveCanvas(c1,
      base_dir + TString("DrawEcal_Likelihood_Sum_") + TString(c1->GetName()));
}

void
RejectionCurve_ppSummary(
    TString base_dir =
        "../../sPHENIX_work/production_analysis_cemc2x2/emcstudies/pidstudies/spacal2d/fieldmap/",
    //
    const double min_eff = 0.85)
{

  TCanvas *c1 = new TCanvas("RejectionCurve_ppSummary",
      "RejectionCurve_ppSummary", 900, 900);
  c1->Divide(1, 1);
  int idx = 1;
  TPad * p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();
  p->SetGridx(0);
  p->SetGridy(0);

  p->DrawFrame(min_eff, 5, 1, 1e3,
      ";Electron Efficiency;Negatively Charged Hadron Rejection");
  TLine * l = new TLine(0.95, 5, 0.95, 1e3);
  l->SetLineColor(kGray);
  l->SetLineWidth(5);
  l->Draw();

  TLatex * text = new TLatex(0.85, 1e3 * 1.1,
      "2-D Proj. SPACAL, e^{-}-h^{-} separation in p+p");
  text->SetTextAlign(11);
  text->SetTextSize(0.044);
  text->SetTextFont(42);
  text->Draw();

  TLegend * leg1 = new TLegend(0.12, 0.15, 0.42, 0.4);
  TLegend * leg2 = new TLegend(0.42, 0.15, 0.67, 0.4);

  TGraphErrors *ge = new TGraphErrors();
  ge->SetLineWidth(3);
  leg1->AddEntry(ge, "0.0 < #eta < 0.1", "lx");
  TGraphErrors *ge = new TGraphErrors();
  ge->SetLineWidth(3);
  ge->SetLineStyle(kDashed);
  leg1->AddEntry(ge, "0.9 < #eta < 1.0", "lx");
//  TGraphErrors *ge = new TGraphErrors();
//  ge->SetLineWidth(3);
//  ge->SetLineStyle(kDotted);
//  leg1->AddEntry(ge, "Cut on min. E/p", "lx");

  TString kine_config = "eta0";
  const double pt_over_p = 0.998;

  TGraphErrors *ge_pi = ExtractRejCurve(base_dir, kine_config + "_8GeV", //
  "Edep_LL_Distribution_eval_sample", "hll_edep_diff", "e-", "pi-");
//  TGraphErrors *ge_kaon = ExtractRejCurve(base_dir, kine_config + "_8GeV", //
//  "Edep_LL_Distribution_eval_sample", "hll_edep_diff");
  TGraphErrors *ge_anti_proton = ExtractRejCurve(base_dir,
      kine_config + "_8GeV", //
      "Edep_LL_Distribution_eval_sample", "hll_edep_diff", "e-", "anti_proton");
  TGraphErrors *ge = MergeRejCurve(ge_pi,
      1 + GetHPiRatio(8 * pt_over_p, "kaon-", "pp"), ge_anti_proton,
      GetHPiRatio(8 * pt_over_p, "anti_proton", "pp"));
  ge->SetLineColor(kRed + 2);
  ge->SetMarkerColor(kRed + 2);
  ge->SetFillColor(kRed - 9);
  ge->SetLineWidth(3);
//  ge->Draw("3");
  ge->Draw("lx");
  leg2->AddEntry(ge, "8 GeV/c", "lx");

  TGraphErrors *ge_pi = ExtractRejCurve(base_dir, kine_config + "_4GeV", //
  "Edep_LL_Distribution_eval_sample", "hll_edep_diff", "e-", "pi-");
//  TGraphErrors *ge_kaon = ExtractRejCurve(base_dir, kine_config + "_8GeV", //
//  "Edep_LL_Distribution_eval_sample", "hll_edep_diff");
  TGraphErrors *ge_anti_proton = ExtractRejCurve(base_dir,
      kine_config + "_4GeV", //
      "Edep_LL_Distribution_eval_sample", "hll_edep_diff", "e-", "anti_proton");
  TGraphErrors *ge = MergeRejCurve(ge_pi,
      1 + GetHPiRatio(4 * pt_over_p, "kaon-", "pp"), ge_anti_proton,
      GetHPiRatio(4 * pt_over_p, "anti_proton", "pp"));
  ge->SetLineColor(kBlue + 2);
  ge->SetMarkerColor(kBlue + 2);
  ge->SetFillColor(kBlue - 9);
  ge->SetLineWidth(3);
  //  ge->Draw("3");
  ge->Draw("lx");
  leg2->AddEntry(ge, "4 GeV/c", "lx");

  TGraphErrors *ge_pi = ExtractRejCurve(base_dir, kine_config + "_2GeV", //
  "Edep_LL_Distribution_eval_sample", "hll_edep_diff", "e-", "pi-");
//  TGraphErrors *ge_kaon = ExtractRejCurve(base_dir, kine_config + "_8GeV", //
//  "Edep_LL_Distribution_eval_sample", "hll_edep_diff");
  TGraphErrors *ge_anti_proton = ExtractRejCurve(base_dir,
      kine_config + "_2GeV", //
      "Edep_LL_Distribution_eval_sample", "hll_edep_diff", "e-", "anti_proton");
  TGraphErrors *ge = MergeRejCurve(ge_pi,
      1 + GetHPiRatio(2 * pt_over_p, "kaon-", "pp"), ge_anti_proton,
      GetHPiRatio(2 * pt_over_p, "anti_proton", "pp"));
  ge->SetLineColor(kGreen + 3);
  ge->SetMarkerColor(kGreen + 3);
  ge->SetFillColor(kGreen - 9);
  ge->SetLineWidth(3);
  //  ge->Draw("3");
  ge->Draw("lx");
  leg2->AddEntry(ge, "2 GeV/c", "lx");

  TString kine_config = "eta0.90";
  const double pt_over_p = 0.673;

  TGraphErrors *ge_pi = ExtractRejCurve(base_dir, kine_config + "_8GeV", //
  "Edep_LL_Distribution_eval_sample", "hll_edep_diff", "e-", "pi-");
//  TGraphErrors *ge_kaon = ExtractRejCurve(base_dir, kine_config + "_8GeV", //
//  "Edep_LL_Distribution_eval_sample", "hll_edep_diff");
  TGraphErrors *ge_anti_proton = ExtractRejCurve(base_dir,
      kine_config + "_8GeV", //
      "Edep_LL_Distribution_eval_sample", "hll_edep_diff", "e-", "anti_proton");
  TGraphErrors *ge = MergeRejCurve(ge_pi,
      1 + GetHPiRatio(8 * pt_over_p, "kaon-", "pp"), ge_anti_proton,
      GetHPiRatio(8 * pt_over_p, "anti_proton", "pp"));
  ge->SetLineColor(kRed + 2);
  ge->SetMarkerColor(kRed + 2);
  ge->SetFillColor(kRed - 9);
  ge->SetLineWidth(3);
//  ge->Draw("3");
  ge->SetLineStyle(kDashed);
  ge->Draw("lx");

  TGraphErrors *ge_pi = ExtractRejCurve(base_dir, kine_config + "_4GeV", //
  "Edep_LL_Distribution_eval_sample", "hll_edep_diff", "e-", "pi-");
//  TGraphErrors *ge_kaon = ExtractRejCurve(base_dir, kine_config + "_8GeV", //
//  "Edep_LL_Distribution_eval_sample", "hll_edep_diff");
  TGraphErrors *ge_anti_proton = ExtractRejCurve(base_dir,
      kine_config + "_4GeV", //
      "Edep_LL_Distribution_eval_sample", "hll_edep_diff", "e-", "anti_proton");
  TGraphErrors *ge = MergeRejCurve(ge_pi,
      1 + GetHPiRatio(4 * pt_over_p, "kaon-", "pp"), ge_anti_proton,
      GetHPiRatio(4 * pt_over_p, "anti_proton", "pp"));
  ge->SetLineColor(kBlue + 2);
  ge->SetMarkerColor(kBlue + 2);
  ge->SetFillColor(kBlue - 9);
  ge->SetLineWidth(3);
  //  ge->Draw("3");
  ge->SetLineStyle(kDashed);
  ge->Draw("lx");

  TGraphErrors *ge_pi = ExtractRejCurve(base_dir, kine_config + "_2GeV", //
  "Edep_LL_Distribution_eval_sample", "hll_edep_diff", "e-", "pi-");
//  TGraphErrors *ge_kaon = ExtractRejCurve(base_dir, kine_config + "_8GeV", //
//  "Edep_LL_Distribution_eval_sample", "hll_edep_diff");
  TGraphErrors *ge_anti_proton = ExtractRejCurve(base_dir,
      kine_config + "_2GeV", //
      "Edep_LL_Distribution_eval_sample", "hll_edep_diff", "e-", "anti_proton");
  TGraphErrors *ge = MergeRejCurve(ge_pi,
      1 + GetHPiRatio(2 * pt_over_p, "kaon-", "pp"), ge_anti_proton,
      GetHPiRatio(2 * pt_over_p, "anti_proton", "pp"));
  ge->SetLineColor(kGreen + 3);
  ge->SetMarkerColor(kGreen + 3);
  ge->SetFillColor(kGreen - 9);
  ge->SetLineWidth(3);
  //  ge->Draw("3");
  ge->SetLineStyle(kDashed);
  ge->Draw("lx");

  leg2->Draw();
  leg1->Draw();

  SaveCanvas(c1,
      base_dir + TString("DrawEcal_Likelihood_Sum_") + TString(c1->GetName()));
}

void
RejectionCurve_ppSummaryPos(
    TString base_dir =
        "../../sPHENIX_work/production_analysis_cemc2x2/emcstudies/pidstudies/spacal2d/fieldmap/",
    //
    const double min_eff = 0.85)
{

  TCanvas *c1 = new TCanvas("RejectionCurve_ppSummaryPos",
      "RejectionCurve_ppSummaryPos", 900, 900);
  c1->Divide(1, 1);
  int idx = 1;
  TPad * p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();
  p->SetGridx(0);
  p->SetGridy(0);

  p->DrawFrame(min_eff, 5, 1, 1e3,
      ";Electron Efficiency;Positively Charged Hadron Rejection");
  TLine * l = new TLine(0.95, 5, 0.95, 1e3);
  l->SetLineColor(kGray);
  l->SetLineWidth(5);
  l->Draw();

  TLatex * text = new TLatex(0.85, 1e3 * 1.1,
      "2-D Proj. SPACAL, e^{+}-h^{+} separation in p+p");
  text->SetTextAlign(11);
  text->SetTextSize(0.044);
  text->SetTextFont(42);
  text->Draw();

  TLegend * leg1 = new TLegend(0.12, 0.15, 0.42, 0.4);
  TLegend * leg2 = new TLegend(0.42, 0.15, 0.67, 0.4);

  TGraphErrors *ge = new TGraphErrors();
  ge->SetLineWidth(3);
  leg1->AddEntry(ge, "0.0 < #eta < 0.1", "lx");
  TGraphErrors *ge = new TGraphErrors();
  ge->SetLineWidth(3);
  ge->SetLineStyle(kDashed);
  leg1->AddEntry(ge, "0.9 < #eta < 1.0", "lx");
//  TGraphErrors *ge = new TGraphErrors();
//  ge->SetLineWidth(3);
//  ge->SetLineStyle(kDotted);
//  leg1->AddEntry(ge, "Cut on min. E/p", "lx");

  TString kine_config = "eta0";
  const double pt_over_p = 0.998;

  TGraphErrors *ge_pi = ExtractRejCurve(base_dir, kine_config + "_8GeV", //
  "Edep_LL_Distribution_eval_sample", "hll_edep_diff", "e+", "pi+");
//  TGraphErrors *ge_kaon = ExtractRejCurve(base_dir, kine_config + "_8GeV", //
//  "Edep_LL_Distribution_eval_sample", "hll_edep_diff", "e+", "kaon+");
  TGraphErrors *ge_anti_proton = ExtractRejCurve(base_dir,
      kine_config + "_8GeV", //
      "Edep_LL_Distribution_eval_sample", "hll_edep_diff", "e+", "proton");
  TGraphErrors *ge = MergeRejCurve(ge_pi,
      1 + GetHPiRatio(8 * pt_over_p, "kaon+", "pp"), ge_anti_proton,
      GetHPiRatio(8 * pt_over_p, "proton", "pp"));
  ge->SetLineColor(kRed + 2);
  ge->SetMarkerColor(kRed + 2);
  ge->SetFillColor(kRed - 9);
  ge->SetLineWidth(3);
//  ge->Draw("3");
  ge->Draw("lx");
  leg2->AddEntry(ge, "8 GeV/c", "lx");

  TGraphErrors *ge_pi = ExtractRejCurve(base_dir, kine_config + "_4GeV", //
  "Edep_LL_Distribution_eval_sample", "hll_edep_diff", "e+", "pi+");
//  TGraphErrors *ge_kaon = ExtractRejCurve(base_dir, kine_config + "_4GeV", //
//  "Edep_LL_Distribution_eval_sample", "hll_edep_diff", "e+", "kaon+");
  TGraphErrors *ge_anti_proton = ExtractRejCurve(base_dir,
      kine_config + "_4GeV", //
      "Edep_LL_Distribution_eval_sample", "hll_edep_diff", "e+", "proton");
  TGraphErrors *ge = MergeRejCurve(ge_pi,
      1 + GetHPiRatio(4 * pt_over_p, "kaon+", "pp"), ge_anti_proton,
      GetHPiRatio(4 * pt_over_p, "proton", "pp"));
  ge->SetLineColor(kBlue + 2);
  ge->SetMarkerColor(kBlue + 2);
  ge->SetFillColor(kBlue - 9);
  ge->SetLineWidth(3);
  //  ge->Draw("3");
  ge->Draw("lx");
  leg2->AddEntry(ge, "4 GeV/c", "lx");

  TGraphErrors *ge_pi = ExtractRejCurve(base_dir, kine_config + "_2GeV", //
  "Edep_LL_Distribution_eval_sample", "hll_edep_diff", "e+", "pi+");
//  TGraphErrors *ge_kaon = ExtractRejCurve(base_dir, kine_config + "_2GeV", //
//  "Edep_LL_Distribution_eval_sample", "hll_edep_diff", "e+", "kaon+");
  TGraphErrors *ge_anti_proton = ExtractRejCurve(base_dir,
      kine_config + "_2GeV", //
      "Edep_LL_Distribution_eval_sample", "hll_edep_diff", "e+", "proton");
  TGraphErrors *ge = MergeRejCurve(ge_pi,
      1 + GetHPiRatio(2 * pt_over_p, "kaon+", "pp"), ge_anti_proton,
      GetHPiRatio(2 * pt_over_p, "proton", "pp"));
  ge->SetLineColor(kGreen + 3);
  ge->SetMarkerColor(kGreen + 3);
  ge->SetFillColor(kGreen - 9);
  ge->SetLineWidth(3);
  //  ge->Draw("3");
  ge->Draw("lx");
  leg2->AddEntry(ge, "2 GeV/c", "lx");

  TString kine_config = "eta0.90";
  const double pt_over_p = 0.673;

  TGraphErrors *ge_pi = ExtractRejCurve(base_dir, kine_config + "_8GeV", //
  "Edep_LL_Distribution_eval_sample", "hll_edep_diff", "e+", "pi+");
//  TGraphErrors *ge_kaon = ExtractRejCurve(base_dir, kine_config + "_8GeV", //
//  "Edep_LL_Distribution_eval_sample", "hll_edep_diff", "e+", "kaon+");
  TGraphErrors *ge_anti_proton = ExtractRejCurve(base_dir,
      kine_config + "_8GeV", //
      "Edep_LL_Distribution_eval_sample", "hll_edep_diff", "e+", "proton");
  TGraphErrors *ge = MergeRejCurve(ge_pi,
      1 + GetHPiRatio(8 * pt_over_p, "kaon+", "pp"), ge_anti_proton,
      GetHPiRatio(8 * pt_over_p, "proton", "pp"));
  ge->SetLineColor(kRed + 2);
  ge->SetMarkerColor(kRed + 2);
  ge->SetFillColor(kRed - 9);
  ge->SetLineWidth(3);
//  ge->Draw("3");
  ge->SetLineStyle(kDashed);
  ge->Draw("lx");

  TGraphErrors *ge_pi = ExtractRejCurve(base_dir, kine_config + "_4GeV", //
  "Edep_LL_Distribution_eval_sample", "hll_edep_diff", "e+", "pi+");
//  TGraphErrors *ge_kaon = ExtractRejCurve(base_dir, kine_config + "_4GeV", //
//  "Edep_LL_Distribution_eval_sample", "hll_edep_diff", "e+", "kaon+");
  TGraphErrors *ge_anti_proton = ExtractRejCurve(base_dir,
      kine_config + "_4GeV", //
      "Edep_LL_Distribution_eval_sample", "hll_edep_diff", "e+", "proton");
  TGraphErrors *ge = MergeRejCurve(ge_pi,
      1 + GetHPiRatio(4 * pt_over_p, "kaon+", "pp"), ge_anti_proton,
      GetHPiRatio(4 * pt_over_p, "proton", "pp"));
  ge->SetLineColor(kBlue + 2);
  ge->SetMarkerColor(kBlue + 2);
  ge->SetFillColor(kBlue - 9);
  ge->SetLineWidth(3);
  //  ge->Draw("3");
  ge->SetLineStyle(kDashed);
  ge->Draw("lx");

  TGraphErrors *ge_pi = ExtractRejCurve(base_dir, kine_config + "_2GeV", //
  "Edep_LL_Distribution_eval_sample", "hll_edep_diff", "e+", "pi+");
//  TGraphErrors *ge_kaon = ExtractRejCurve(base_dir, kine_config + "_2GeV", //
//  "Edep_LL_Distribution_eval_sample", "hll_edep_diff", "e+", "kaon+");
  TGraphErrors *ge_anti_proton = ExtractRejCurve(base_dir,
      kine_config + "_2GeV", //
      "Edep_LL_Distribution_eval_sample", "hll_edep_diff", "e+", "proton");
  TGraphErrors *ge = MergeRejCurve(ge_pi,
      1 + GetHPiRatio(2 * pt_over_p, "kaon+", "pp"), ge_anti_proton,
      GetHPiRatio(2 * pt_over_p, "proton", "pp"));
  ge->SetLineColor(kGreen + 3);
  ge->SetMarkerColor(kGreen + 3);
  ge->SetFillColor(kGreen - 9);
  ge->SetLineWidth(3);
  //  ge->Draw("3");
  ge->SetLineStyle(kDashed);
  ge->Draw("lx");

  leg2->Draw();
  leg1->Draw();

  SaveCanvas(c1,
      base_dir + TString("DrawEcal_Likelihood_Sum_") + TString(c1->GetName()));
}

void
RejectionCurve_AuAuSummary(
    TString base_dir =
        "../../sPHENIX_work/production_analysis_cemc2x2/embedding/emcstudies/pidstudies/spacal2d/fieldmap/",
    TString label = "2-D Proj. SPACAL, e^{-} VS h^{-} in Au+Au 10%C", //
    const double min_eff = 0.6)
{

  TCanvas *c1 = new TCanvas("RejectionCurve_AuAuSummary",
      "RejectionCurve_AuAuSummary", 900, 900);
  c1->Divide(1, 1);
  int idx = 1;
  TPad * p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();
  p->SetGridx(0);
  p->SetGridy(0);

  p->DrawFrame(min_eff, 1, 1, 1e3,
      ";Electron Efficiency;Negatively Charged Hadron Rejection");
  TLine * l = new TLine(0.9, 1, 0.9, 1e3);
  l->SetLineColor(kGray);
  l->SetLineWidth(5);
  l->Draw();

  TLatex * text = new TLatex(0.6, 1e3 * 1.2, label);
  text->SetTextAlign(11);
  text->SetTextSize(0.044);
  text->SetTextFont(42);
  text->Draw();

  TLegend * leg1 = new TLegend(0.12, 0.15, 0.42, 0.4);
  TLegend * leg2 = new TLegend(0.42, 0.15, 0.67, 0.4);

  TGraphErrors *ge = new TGraphErrors();
  ge->SetLineWidth(3);
  leg1->AddEntry(ge, "0.0 < #eta < 0.1", "lx");
  TGraphErrors *ge = new TGraphErrors();
  ge->SetLineWidth(3);
  ge->SetLineStyle(kDashed);
  leg1->AddEntry(ge, "0.9 < #eta < 1.0", "lx");
//  TGraphErrors *ge = new TGraphErrors();
//  ge->SetLineWidth(3);
//  ge->SetLineStyle(kDotted);
//  leg1->AddEntry(ge, "Cut on min. E/p", "lx");

  TString kine_config = "eta0";
  const double pt_over_p = 0.998;

  TGraphErrors *ge_pi = ExtractRejCurve(base_dir, kine_config + "_8GeV", //
  "Edep_LL_Distribution_eval_sample", "hll_edep_diff", "e-", "pi-");
  TGraphErrors *ge_kaon = ExtractRejCurve(base_dir, kine_config + "_8GeV", //
  "Edep_LL_Distribution_eval_sample", "hll_edep_diff", "e-", "kaon-");
  TGraphErrors *ge_anti_proton = ExtractRejCurve(base_dir,
      kine_config + "_8GeV", //
      "Edep_LL_Distribution_eval_sample", "hll_edep_diff", "e-", "anti_proton");
  TGraphErrors *ge = MergeRejCurve(ge_pi, 1, ge_kaon,
      GetHPiRatio(8, "kaon-", "AuAu10%C"), ge_anti_proton,
      GetHPiRatio(8, "anti_proton", "AuAu10%C"));
  ge->SetLineColor(kRed + 2);
  ge->SetMarkerColor(kRed + 2);
  ge->SetFillColor(kRed - 9);
  ge->SetLineWidth(3);
//  ge->Draw("3");
  ge->Draw("lx");

  ge->SetName("MergeExtractRejCurve_" + kine_config + "_8GeV");
  leg2->AddEntry(ge, "8 GeV/c", "lx");

  TGraphErrors *ge_pi = ExtractRejCurve(base_dir, kine_config + "_4GeV", //
  "Edep_LL_Distribution_eval_sample", "hll_edep_diff", "e-", "pi-");
  TGraphErrors *ge_kaon = ExtractRejCurve(base_dir, kine_config + "_8GeV", //
  "Edep_LL_Distribution_eval_sample", "hll_edep_diff", "e-", "kaon-");
  TGraphErrors *ge_anti_proton = ExtractRejCurve(base_dir,
      kine_config + "_4GeV", //
      "Edep_LL_Distribution_eval_sample", "hll_edep_diff", "e-", "anti_proton");
  TGraphErrors *ge = MergeRejCurve(ge_pi, 1, ge_kaon,
      GetHPiRatio(4, "kaon-", "AuAu10%C"), ge_anti_proton,
      GetHPiRatio(4, "anti_proton", "AuAu10%C"));
  ge->SetLineColor(kBlue + 2);
  ge->SetMarkerColor(kBlue + 2);
  ge->SetFillColor(kBlue - 9);
  ge->SetLineWidth(3);
  //  ge->Draw("3");
  ge->Draw("lx");
  ge->SetName("MergeExtractRejCurve_" + kine_config + "_4GeV");
  leg2->AddEntry(ge, "4 GeV/c", "lx");

  TGraphErrors *ge_pi = ExtractRejCurve(base_dir, kine_config + "_2GeV", //
  "Edep_LL_Distribution_eval_sample", "hll_edep_diff", "e-", "pi-");
  TGraphErrors *ge_kaon = ExtractRejCurve(base_dir, kine_config + "_8GeV", //
  "Edep_LL_Distribution_eval_sample", "hll_edep_diff", "e-", "kaon-");
  TGraphErrors *ge_anti_proton = ExtractRejCurve(base_dir,
      kine_config + "_2GeV", //
      "Edep_LL_Distribution_eval_sample", "hll_edep_diff", "e-", "anti_proton");
  TGraphErrors *ge = MergeRejCurve(ge_pi, 1, ge_kaon,
      GetHPiRatio(2, "kaon-", "AuAu10%C"), ge_anti_proton,
      GetHPiRatio(2, "anti_proton", "AuAu10%C"));
  ge->SetLineColor(kGreen + 3);
  ge->SetMarkerColor(kGreen + 3);
  ge->SetFillColor(kGreen - 9);
  ge->SetLineWidth(3);
  //  ge->Draw("3");
  ge->Draw("lx");
  ge->SetName("MergeExtractRejCurve_" + kine_config + "_2GeV");
  leg2->AddEntry(ge, "2 GeV/c", "lx");

  TString kine_config = "eta0.90";
  const double pt_over_p = 0.673;

  TGraphErrors *ge_pi = ExtractRejCurve(base_dir, kine_config + "_8GeV", //
  "Edep_LL_Distribution_eval_sample", "hll_edep_diff", "e-", "pi-");
  TGraphErrors *ge_kaon = ExtractRejCurve(base_dir, kine_config + "_8GeV", //
  "Edep_LL_Distribution_eval_sample", "hll_edep_diff", "e-", "kaon-");
  TGraphErrors *ge_anti_proton = ExtractRejCurve(base_dir,
      kine_config + "_8GeV", //
      "Edep_LL_Distribution_eval_sample", "hll_edep_diff", "e-", "anti_proton");
  TGraphErrors *ge = MergeRejCurve(ge_pi, 1, ge_kaon,
      GetHPiRatio(8, "kaon-", "AuAu10%C"), ge_anti_proton,
      GetHPiRatio(8, "anti_proton", "AuAu10%C"));
  ge->SetLineColor(kRed + 2);
  ge->SetMarkerColor(kRed + 2);
  ge->SetFillColor(kRed - 9);
  ge->SetLineWidth(3);
//  ge->Draw("3");
  ge->SetLineStyle(kDashed);
  ge->SetName("MergeExtractRejCurve_" + kine_config + "_8GeV");
  ge->Draw("lx");

  TGraphErrors *ge_pi = ExtractRejCurve(base_dir, kine_config + "_4GeV", //
  "Edep_LL_Distribution_eval_sample", "hll_edep_diff", "e-", "pi-");
  TGraphErrors *ge_kaon = ExtractRejCurve(base_dir, kine_config + "_8GeV", //
  "Edep_LL_Distribution_eval_sample", "hll_edep_diff", "e-", "kaon-");
  TGraphErrors *ge_anti_proton = ExtractRejCurve(base_dir,
      kine_config + "_4GeV", //
      "Edep_LL_Distribution_eval_sample", "hll_edep_diff", "e-", "anti_proton");
  TGraphErrors *ge = MergeRejCurve(ge_pi, 1, ge_kaon,
      GetHPiRatio(4, "kaon-", "AuAu10%C"), ge_anti_proton,
      GetHPiRatio(4, "anti_proton", "AuAu10%C"));
  ge->SetLineColor(kBlue + 2);
  ge->SetMarkerColor(kBlue + 2);
  ge->SetFillColor(kBlue - 9);
  ge->SetLineWidth(3);
  //  ge->Draw("3");
  ge->SetLineStyle(kDashed);
  ge->SetName("MergeExtractRejCurve_" + kine_config + "_4GeV");
  ge->Draw("lx");

  TGraphErrors *ge_pi = ExtractRejCurve(base_dir, kine_config + "_2GeV", //
  "Edep_LL_Distribution_eval_sample", "hll_edep_diff", "e-", "pi-");
  TGraphErrors *ge_kaon = ExtractRejCurve(base_dir, kine_config + "_8GeV", //
  "Edep_LL_Distribution_eval_sample", "hll_edep_diff", "e-", "kaon-");
  TGraphErrors *ge_anti_proton = ExtractRejCurve(base_dir,
      kine_config + "_2GeV", //
      "Edep_LL_Distribution_eval_sample", "hll_edep_diff", "e-", "anti_proton");
  TGraphErrors *ge = MergeRejCurve(ge_pi, 1, ge_kaon,
      GetHPiRatio(2, "kaon-", "AuAu10%C"), ge_anti_proton,
      GetHPiRatio(2, "anti_proton", "AuAu10%C"));
  ge->SetLineColor(kGreen + 3);
  ge->SetMarkerColor(kGreen + 3);
  ge->SetFillColor(kGreen - 9);
  ge->SetLineWidth(3);
  //  ge->Draw("3");
  ge->SetLineStyle(kDashed);
  ge->SetName("MergeExtractRejCurve_" + kine_config + "_2GeV");
  ge->Draw("lx");

  leg2->Draw();
  leg1->Draw();

  SaveCanvas(c1,
      base_dir + TString("DrawEcal_Likelihood_Sum_") + TString(c1->GetName()));
}

void
RejectionCurve_AuAuSummaryPos(
    TString base_dir =
        "../../sPHENIX_work/production_analysis_cemc2x2/embedding/emcstudies/pidstudies/spacal2d/fieldmap/",
    TString label = "2-D Proj. SPACAL, e^{-} VS h^{+} in Au+Au 10%C", //
    const double min_eff = 0.6)
{

  TCanvas *c1 = new TCanvas("RejectionCurve_AuAuSummaryPos",
      "RejectionCurve_AuAuSummaryPos: " + label, 900, 900);
  c1->Divide(1, 1);
  int idx = 1;
  TPad * p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();
  p->SetGridx(0);
  p->SetGridy(0);

  p->DrawFrame(min_eff, 1, 1, 1e3,
      ";Electron Efficiency;Positively Charged Hadron Rejection");
  TLine * l = new TLine(0.9, 1, 0.9, 1e3);
  l->SetLineColor(kGray);
  l->SetLineWidth(5);
  l->Draw();

  TLatex * text = new TLatex(0.6, 1e3 * 1.2, label);
  text->SetTextAlign(11);
  text->SetTextSize(0.044);
  text->SetTextFont(42);
  text->Draw();

  TLegend * leg1 = new TLegend(0.12, 0.15, 0.42, 0.4);
  TLegend * leg2 = new TLegend(0.42, 0.15, 0.67, 0.4);

  TGraphErrors *ge = new TGraphErrors();
  ge->SetLineWidth(3);
  leg1->AddEntry(ge, "0.0 < #eta < 0.1", "lx");
  TGraphErrors *ge = new TGraphErrors();
  ge->SetLineWidth(3);
  ge->SetLineStyle(kDashed);
  leg1->AddEntry(ge, "0.9 < #eta < 1.0", "lx");
//  TGraphErrors *ge = new TGraphErrors();
//  ge->SetLineWidth(3);
//  ge->SetLineStyle(kDotted);
//  leg1->AddEntry(ge, "Cut on min. E/p", "lx");

  TString kine_config = "eta0";
  const double pt_over_p = 0.998;

  TGraphErrors *ge_pi = ExtractRejCurve(base_dir, kine_config + "_8GeV", //
  "Edep_LL_Distribution_eval_sample", "hll_edep_diff", "e+", "pi+");
  TGraphErrors *ge_kaon = ExtractRejCurve(base_dir, kine_config + "_8GeV", //
  "Edep_LL_Distribution_eval_sample", "hll_edep_diff", "e+", "kaon+");
  TGraphErrors *ge_anti_proton = ExtractRejCurve(base_dir,
      kine_config + "_8GeV", //
      "Edep_LL_Distribution_eval_sample", "hll_edep_diff", "e+", "proton");
  TGraphErrors *ge = MergeRejCurve(ge_pi, 1, ge_kaon,
      GetHPiRatio(8, "kaon+", "AuAu10%C"), ge_anti_proton,
      GetHPiRatio(8, "proton", "AuAu10%C"));
  ge->SetLineColor(kRed + 2);
  ge->SetMarkerColor(kRed + 2);
  ge->SetFillColor(kRed - 9);
  ge->SetLineWidth(3);
//  ge->Draw("3");
  ge->SetName("MergeExtractRejCurve_" + kine_config + "_8GeV");
  ge->Draw("lx");
  leg2->AddEntry(ge, "8 GeV/c", "lx");

  TGraphErrors *ge_pi = ExtractRejCurve(base_dir, kine_config + "_4GeV", //
  "Edep_LL_Distribution_eval_sample", "hll_edep_diff", "e+", "pi+");
  TGraphErrors *ge_kaon = ExtractRejCurve(base_dir, kine_config + "_8GeV", //
  "Edep_LL_Distribution_eval_sample", "hll_edep_diff", "e+", "kaon+");
  TGraphErrors *ge_anti_proton = ExtractRejCurve(base_dir,
      kine_config + "_4GeV", //
      "Edep_LL_Distribution_eval_sample", "hll_edep_diff", "e+", "proton");
  TGraphErrors *ge = MergeRejCurve(ge_pi, 1, ge_kaon,
      GetHPiRatio(4, "kaon+", "AuAu10%C"), ge_anti_proton,
      GetHPiRatio(4, "proton", "AuAu10%C"));
  ge->SetLineColor(kBlue + 2);
  ge->SetMarkerColor(kBlue + 2);
  ge->SetFillColor(kBlue - 9);
  ge->SetLineWidth(3);
  //  ge->Draw("3");
  ge->SetName("MergeExtractRejCurve_" + kine_config + "_4GeV");
  ge->Draw("lx");
  leg2->AddEntry(ge, "4 GeV/c", "lx");

  TGraphErrors *ge_pi = ExtractRejCurve(base_dir, kine_config + "_2GeV", //
  "Edep_LL_Distribution_eval_sample", "hll_edep_diff", "e+", "pi+");
  TGraphErrors *ge_kaon = ExtractRejCurve(base_dir, kine_config + "_8GeV", //
  "Edep_LL_Distribution_eval_sample", "hll_edep_diff", "e+", "kaon+");
  TGraphErrors *ge_anti_proton = ExtractRejCurve(base_dir,
      kine_config + "_2GeV", //
      "Edep_LL_Distribution_eval_sample", "hll_edep_diff", "e+", "proton");
  TGraphErrors *ge = MergeRejCurve(ge_pi, 1, ge_kaon,
      GetHPiRatio(2, "kaon+", "AuAu10%C"), ge_anti_proton,
      GetHPiRatio(2, "proton", "AuAu10%C"));
  ge->SetLineColor(kGreen + 3);
  ge->SetMarkerColor(kGreen + 3);
  ge->SetFillColor(kGreen - 9);
  ge->SetLineWidth(3);
  //  ge->Draw("3");
  ge->Draw("lx");
  ge->SetName("MergeExtractRejCurve_" + kine_config + "_2GeV");
  leg2->AddEntry(ge, "2 GeV/c", "lx");

  TString kine_config = "eta0.90";
  const double pt_over_p = 0.673;

  TGraphErrors *ge_pi = ExtractRejCurve(base_dir, kine_config + "_8GeV", //
  "Edep_LL_Distribution_eval_sample", "hll_edep_diff", "e+", "pi+");
  TGraphErrors *ge_kaon = ExtractRejCurve(base_dir, kine_config + "_8GeV", //
  "Edep_LL_Distribution_eval_sample", "hll_edep_diff", "e+", "kaon+");
  TGraphErrors *ge_anti_proton = ExtractRejCurve(base_dir,
      kine_config + "_8GeV", //
      "Edep_LL_Distribution_eval_sample", "hll_edep_diff", "e+", "proton");
  TGraphErrors *ge = MergeRejCurve(ge_pi, 1, ge_kaon,
      GetHPiRatio(8, "kaon+", "AuAu10%C"), ge_anti_proton,
      GetHPiRatio(8, "proton", "AuAu10%C"));
  ge->SetLineColor(kRed + 2);
  ge->SetMarkerColor(kRed + 2);
  ge->SetFillColor(kRed - 9);
  ge->SetLineWidth(3);
//  ge->Draw("3");
  ge->SetLineStyle(kDashed);
  ge->SetName("MergeExtractRejCurve_" + kine_config + "_8GeV");
  ge->Draw("lx");

  TGraphErrors *ge_pi = ExtractRejCurve(base_dir, kine_config + "_4GeV", //
  "Edep_LL_Distribution_eval_sample", "hll_edep_diff", "e+", "pi+");
  TGraphErrors *ge_kaon = ExtractRejCurve(base_dir, kine_config + "_8GeV", //
  "Edep_LL_Distribution_eval_sample", "hll_edep_diff", "e+", "kaon+");
  TGraphErrors *ge_anti_proton = ExtractRejCurve(base_dir,
      kine_config + "_4GeV", //
      "Edep_LL_Distribution_eval_sample", "hll_edep_diff", "e+", "proton");
  TGraphErrors *ge = MergeRejCurve(ge_pi, 1, ge_kaon,
      GetHPiRatio(4, "kaon+", "AuAu10%C"), ge_anti_proton,
      GetHPiRatio(4, "proton", "AuAu10%C"));
  ge->SetLineColor(kBlue + 2);
  ge->SetMarkerColor(kBlue + 2);
  ge->SetFillColor(kBlue - 9);
  ge->SetLineWidth(3);
  //  ge->Draw("3");
  ge->SetLineStyle(kDashed);
  ge->SetName("MergeExtractRejCurve_" + kine_config + "_4GeV");
  ge->Draw("lx");

  TGraphErrors *ge_pi = ExtractRejCurve(base_dir, kine_config + "_2GeV", //
  "Edep_LL_Distribution_eval_sample", "hll_edep_diff", "e+", "pi+");
  TGraphErrors *ge_kaon = ExtractRejCurve(base_dir, kine_config + "_8GeV", //
  "Edep_LL_Distribution_eval_sample", "hll_edep_diff", "e+", "kaon+");
  TGraphErrors *ge_anti_proton = ExtractRejCurve(base_dir,
      kine_config + "_2GeV", //
      "Edep_LL_Distribution_eval_sample", "hll_edep_diff", "e+", "proton");
  TGraphErrors *ge = MergeRejCurve(ge_pi, 1, ge_kaon,
      GetHPiRatio(2, "kaon+", "AuAu10%C"), ge_anti_proton,
      GetHPiRatio(2, "proton", "AuAu10%C"));
  ge->SetLineColor(kGreen + 3);
  ge->SetMarkerColor(kGreen + 3);
  ge->SetFillColor(kGreen - 9);
  ge->SetLineWidth(3);
  //  ge->Draw("3");
  ge->SetLineStyle(kDashed);
  ge->SetName("MergeExtractRejCurve_" + kine_config + "_2GeV");
  ge->Draw("lx");

  leg2->Draw();
  leg1->Draw();

  SaveCanvas(c1,
      base_dir + TString("DrawEcal_Likelihood_Sum_") + TString(c1->GetName()));
}

TGraph *
rejection_ratio(TFile * f_2d, TFile * f_1d, TString graph_name)
{
  cout << "rejection_ratio - processing " << graph_name << endl;

  TGraph * g_2d = (TGraph *) f_2d->GetObjectChecked(graph_name, "TGraph");
  assert(g_2d);
  TGraph * g_1d = (TGraph *) f_1d->GetObjectChecked(graph_name, "TGraph");
  assert(g_1d);

  assert(g_1d -> GetN() == g_2d -> GetN());

  double xs[10000] =
    { 0 };
  double ys[10000] =
    { 0 };

//  for (int i = 0; i < g_2d->GetN(); i++)
//    {
//
//      (g_r->GetY())[i] = (g_2d->GetY())[i] / (g_1d->GetY())[i];
//
////      cout <<"At "<<(g_2d->GetX())[i] <<" -> "
////          <<(g_2d->GetY())[i] <<"/"<< (g_1d->GetY())[i] <<" = "<<(g_r->GetY())[i] <<endl;
//
//    }

  int i = 0;
  for (double x = 0.6; x < 0.99; x += 0.01)
    {
      xs[i] = x;

      ys[i] = g_2d->Eval(x) / g_1d->Eval(x);

      cout << "At " <<  xs[i]  << " -> " << g_2d->Eval(x) << "/"
          << g_1d->Eval(x) << " = " << ys[i] << endl;
      i++;
    }

  TGraph * g_r = new TGraph(i, xs, ys);
  g_r->SetName(TString(g_1d->GetName()) + TString("_RejRatio"));

  g_r->SetLineStyle(g_2d->GetLineStyle());

  g_r->SetLineColor(g_2d->GetLineColor());
  g_r->SetLineWidth(g_2d->GetLineWidth());

  return g_r;
}

void
RejectionCurve_AuAuSummary_Compare()
{

  TString base_dir =
      "../../sPHENIX_work/production_analysis_cemc2x2/embedding/emcstudies/pidstudies/";
  TString label = "1D VS 2D rejection ratio, e^{-} VS h^{-} in Au+Au 10%C"; //
  const double min_eff = 0.6;

  TFile * f_2d =
      new TFile(
          base_dir
              + "/spacal2d/fieldmap/DrawEcal_Likelihood_Sum_RejectionCurve_AuAuSummary.root");
  assert(f_2d -> IsOpen());
  TFile * f_1d =
      new TFile(
          base_dir
              + "/spacal1d/fieldmap/DrawEcal_Likelihood_Sum_RejectionCurve_AuAuSummary.root");
  assert(f_1d -> IsOpen());

  TCanvas *c1 = new TCanvas("RejectionCurve_AuAuSummary_Compare",
      "RejectionCurve_AuAuSummary_Compare", 900, 900);
  c1->Divide(1, 1);
  int idx = 1;
  TPad * p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
//  p->SetLogy();
  p->SetGridx(0);
  p->SetGridy(0);

  p->DrawFrame(min_eff, .5, 1, 1.5,
      ";Electron Efficiency;h^{-} Rejection Ratio, (2D proj.)/(1D proj.)");
  TLine * l = new TLine(min_eff, 1, 1, 1);
  l->SetLineColor(kGray);
  l->SetLineWidth(2);
  l->Draw();

  TLatex * text = new TLatex(0.6, 1.5 +0.03, label);
  text->SetTextAlign(11);
  text->SetTextSize(0.044);
  text->SetTextFont(42);
  text->Draw();

  TLegend * leg1 = new TLegend(0.12, 0.15, 0.42, 0.4);
  TLegend * leg2 = new TLegend(0.42, 0.15, 0.67, 0.4);

  TGraph *ge = new TGraphErrors();
  ge->SetLineWidth(3);
  leg1->AddEntry(ge, "0.0 < #eta < 0.1", "lx");
  TGraph *ge = new TGraphErrors();
  ge->SetLineWidth(3);
  ge->SetLineStyle(kDashed);
  leg1->AddEntry(ge, "0.9 < #eta < 1.0", "lx");
//  TGraphErrors *ge = new TGraphErrors();
//  ge->SetLineWidth(3);
//  ge->SetLineStyle(kDotted);
//  leg1->AddEntry(ge, "Cut on min. E/p", "lx");

  TString kine_config = "eta0";

  TString name = ("MergeExtractRejCurve_" + kine_config + "_8GeV");
  TGraph * ge = rejection_ratio(f_2d, f_1d, name);

//  ge->Draw("lx");
//  leg2->AddEntry(ge, "8 GeV/c", "lx");

  TString name = ("MergeExtractRejCurve_" + kine_config + "_4GeV");
  TGraph * ge = rejection_ratio(f_2d, f_1d, name);
  ge->Draw("lx");
  leg2->AddEntry(ge, "4 GeV/c", "lx");

  TString name = ("MergeExtractRejCurve_" + kine_config + "_2GeV");
  TGraph * ge = rejection_ratio(f_2d, f_1d, name);
  ge->Draw("lx");
  leg2->AddEntry(ge, "2 GeV/c", "lx");

  TString kine_config = "eta0.90";

  TString name = ("MergeExtractRejCurve_" + kine_config + "_8GeV");
  TGraph * ge = rejection_ratio(f_2d, f_1d, name);
//  ge->Draw("lx");

  TString name = ("MergeExtractRejCurve_" + kine_config + "_4GeV");
  TGraph * ge = rejection_ratio(f_2d, f_1d, name);
  ge->Draw("lx");

  TString name = ("MergeExtractRejCurve_" + kine_config + "_2GeV");
  TGraph * ge = rejection_ratio(f_2d, f_1d, name);
  ge->Draw("lx");

  leg2->Draw();
  leg1->Draw();

  SaveCanvas(c1,
      base_dir + TString("DrawEcal_Likelihood_Sum_") + TString(c1->GetName()));
}

void
RejectionCurve_AuAuSummary_Compare2()
{

  TString base_dir =
      "../../sPHENIX_work/production_analysis_cemc2x2/embedding/emcstudies/pidstudies/";
  TString base_dir2 =
      "../../sPHENIX_work/production_analysis/embedding/emcstudies/pidstudies/";
  TString label = "2x2-Ganging rej. ratio, e^{-} VS h^{-} in Au+Au 10%C"; //
  const double min_eff = 0.6;

  TFile * f_2d =
      new TFile(
          base_dir
              + "/spacal2d/fieldmap/DrawEcal_Likelihood_Sum_RejectionCurve_AuAuSummary.root");
  assert(f_2d -> IsOpen());
  TFile * f_1d =
      new TFile(
          base_dir2
              + "/spacal2d/fieldmap/DrawEcal_Likelihood_Sum_RejectionCurve_AuAuSummary.root");
  assert(f_1d -> IsOpen());

  TCanvas *c1 = new TCanvas("RejectionCurve_AuAuSummary_Compare2",
      "RejectionCurve_AuAuSummary_Compare2", 900, 900);
  c1->Divide(1, 1);
  int idx = 1;
  TPad * p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
//  p->SetLogy();
  p->SetGridx(0);
  p->SetGridy(0);

  p->DrawFrame(min_eff, .0, 1, 1.1,
      ";Electron Efficiency;h^{-} Rejection Ratio, (2x2 Ganging)/(Nominal)");
  TLine * l = new TLine(min_eff, 1, 1, 1);
  l->SetLineColor(kGray);
  l->SetLineWidth(2);
  l->Draw();

  TLatex * text = new TLatex(0.6, 1.1 +0.03, label);
  text->SetTextAlign(11);
  text->SetTextSize(0.044);
  text->SetTextFont(42);
  text->Draw();

  TLegend * leg1 = new TLegend(0.12, 0.15, 0.42, 0.4);
  TLegend * leg2 = new TLegend(0.42, 0.15, 0.67, 0.4);

  TGraph *ge = new TGraphErrors();
  ge->SetLineWidth(3);
  leg1->AddEntry(ge, "0.0 < #eta < 0.1", "lx");
  TGraph *ge = new TGraphErrors();
  ge->SetLineWidth(3);
  ge->SetLineStyle(kDashed);
  leg1->AddEntry(ge, "0.9 < #eta < 1.0", "lx");

  TString kine_config = "eta0";

  TString name = ("MergeExtractRejCurve_" + kine_config + "_8GeV");
  TGraph * ge = rejection_ratio(f_2d, f_1d, name);

  ge->Draw("lx");
  leg2->AddEntry(ge, "8 GeV/c", "lx");

  TString name = ("MergeExtractRejCurve_" + kine_config + "_4GeV");
  TGraph * ge = rejection_ratio(f_2d, f_1d, name);
  ge->Draw("lx");
  leg2->AddEntry(ge, "4 GeV/c", "lx");

  TString name = ("MergeExtractRejCurve_" + kine_config + "_2GeV");
  TGraph * ge = rejection_ratio(f_2d, f_1d, name);
  ge->Draw("lx");
  leg2->AddEntry(ge, "2 GeV/c", "lx");

  TString kine_config = "eta0.90";

  TString name = ("MergeExtractRejCurve_" + kine_config + "_8GeV");
  TGraph * ge = rejection_ratio(f_2d, f_1d, name);
  ge->Draw("lx");

  TString name = ("MergeExtractRejCurve_" + kine_config + "_4GeV");
  TGraph * ge = rejection_ratio(f_2d, f_1d, name);
  ge->Draw("lx");

  TString name = ("MergeExtractRejCurve_" + kine_config + "_2GeV");
  TGraph * ge = rejection_ratio(f_2d, f_1d, name);
  ge->Draw("lx");

  leg2->Draw();
  leg1->Draw();

  SaveCanvas(c1,
      base_dir + TString("DrawEcal_Likelihood_Sum_") + TString(c1->GetName()));
}

void
RejectionCurve_RejMethodComparison_AntiProton(
    TString base_dir =
        "../../sPHENIX_work/production_analysis_cemc2x2/emcstudies/pidstudies/spacal2d/fieldmap/",
    TString kine_config = "eta0", //
    const double min_eff = 0.85)
{

  TCanvas *c1 = new TCanvas(
      "RejectionCurve_RejMethodComparison_AntiProton_" + kine_config,
      "RejectionCurve_RejMethodComparison_AntiProton_" + kine_config, 900, 900);
  c1->Divide(1, 1);
  int idx = 1;
  TPad * p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();
  p->SetGridx(0);
  p->SetGridy(0);

  p->DrawFrame(min_eff, 1, 1, 1e3, ";Electron Efficiency;Hadron Rejection");
  TLine * l = new TLine(0.95, 1, 0.95, 1e3);
  l->SetLineColor(kGray);
  l->SetLineWidth(5);
  l->Draw();

  TLatex * text = new TLatex(0.85, 1e3 * 1.2,
      "2-D Proj. SPACAL, e^{-}-#bar{p} separation @ 0 < #eta < 0.1");
  text->SetTextAlign(11);
  text->SetTextSize(0.044);
  text->SetTextFont(42);
  text->Draw();

  TLegend * leg1 = new TLegend(0.12, 0.15, 0.65, 0.4);
  TLegend * leg2 = new TLegend(0.65, 0.15, 0.85, 0.4);

  TGraphErrors *ge = new TGraphErrors();
  ge->SetLineWidth(3);
  leg1->AddEntry(ge, "EMCal+HCal Likelihood", "lx");
  TGraphErrors *ge = new TGraphErrors();
  ge->SetLineWidth(3);
  ge->SetLineStyle(kDashed);
  leg1->AddEntry(ge, "EMCal E/p Likelihood", "lx");
  TGraphErrors *ge = new TGraphErrors();
  ge->SetLineWidth(3);
  ge->SetLineStyle(kDotted);
  leg1->AddEntry(ge, "Cut on min. E/p", "lx");

  TGraphErrors *ge = ExtractRejCurve(base_dir, kine_config + "_8GeV", //
  "Edep_LL_Distribution_eval_sample", "hll_edep_diff", "e-", "anti_proton");
  ge->SetLineColor(kRed + 2);
  ge->SetMarkerColor(kRed + 2);
  ge->SetFillColor(kRed - 9);
  ge->SetLineWidth(3);
//  ge->Draw("3");
  ge->Draw("lx");
  leg2->AddEntry(ge, "8 GeV/c", "lx");
  TGraphErrors *ge = ExtractRejCurve(base_dir, kine_config + "_8GeV", //
  "EP_LL_Distribution_eval_sample", "hll_ep_diff", "e-", "anti_proton");
  ge->SetLineColor(kRed + 2);
  ge->SetMarkerColor(kRed + 2);
  ge->SetFillColor(kRed - 9);
  ge->SetLineWidth(3);
  ge->SetLineStyle(kDashed);
//  ge->Draw("3");
  ge->Draw("lx");
  TGraphErrors *ge = ExtractRejCurve(base_dir, kine_config + "_8GeV", //
  "Ep_Checks_eval_sample", "hEMCalTrk_get_ep", "e-", "anti_proton");
  ge->SetLineColor(kRed + 2);
  ge->SetMarkerColor(kRed + 2);
  ge->SetFillColor(kRed - 9);
  ge->SetLineWidth(3);
  ge->SetLineStyle(kDotted);
//  ge->Draw("3");
  ge->Draw("lx");

  TGraphErrors *ge = ExtractRejCurve(base_dir, kine_config + "_4GeV", //
  "Edep_LL_Distribution_eval_sample", "hll_edep_diff", "e-", "anti_proton");
  ge->SetLineColor(kBlue + 2);
  ge->SetMarkerColor(kBlue + 2);
  ge->SetFillColor(kBlue - 9);
  ge->SetLineWidth(3);
//  ge->Draw("3");
  ge->Draw("lx");
  leg2->AddEntry(ge, "4 GeV/c", "lx");
  TGraphErrors *ge = ExtractRejCurve(base_dir, kine_config + "_4GeV", //
  "EP_LL_Distribution_eval_sample", "hll_ep_diff", "e-", "anti_proton");
  ge->SetLineColor(kBlue + 2);
  ge->SetMarkerColor(kBlue + 2);
  ge->SetFillColor(kBlue - 9);
  ge->SetLineWidth(3);
  ge->SetLineStyle(kDashed);
//  ge->Draw("3");
  ge->Draw("lx");
  TGraphErrors *ge = ExtractRejCurve(base_dir, kine_config + "_4GeV", //
  "Ep_Checks_eval_sample", "hEMCalTrk_get_ep", "e-", "anti_proton");
  ge->SetLineColor(kBlue + 2);
  ge->SetMarkerColor(kBlue + 2);
  ge->SetFillColor(kBlue - 9);
  ge->SetLineWidth(3);
  ge->SetLineStyle(kDotted);
//  ge->Draw("3");
  ge->Draw("lx");
//
  TGraphErrors *ge = ExtractRejCurve(base_dir, kine_config + "_2GeV", //
  "Edep_LL_Distribution_eval_sample", "hll_edep_diff", "e-", "anti_proton");
  ge->SetLineColor(kGreen + 3);
  ge->SetMarkerColor(kGreen + 3);
  ge->SetFillColor(kGreen - 9);
  ge->SetLineWidth(3);
//  ge->Draw("3");
  ge->Draw("lx");
  leg2->AddEntry(ge, "2 GeV/c", "lx");
  TGraphErrors *ge = ExtractRejCurve(base_dir, kine_config + "_2GeV", //
  "EP_LL_Distribution_eval_sample", "hll_ep_diff", "e-", "anti_proton");
  ge->SetLineColor(kGreen + 3);
  ge->SetMarkerColor(kGreen + 3);
  ge->SetFillColor(kGreen - 9);
  ge->SetLineWidth(3);
  ge->SetLineStyle(kDashed);
//  ge->Draw("3");
  ge->Draw("lx");
  TGraphErrors *ge = ExtractRejCurve(base_dir, kine_config + "_2GeV", //
  "Ep_Checks_eval_sample", "hEMCalTrk_get_ep", "e-", "anti_proton");
  ge->SetLineColor(kGreen + 3);
  ge->SetMarkerColor(kGreen + 3);
  ge->SetFillColor(kGreen - 9);
  ge->SetLineWidth(3);
  ge->SetLineStyle(kDotted);
//  ge->Draw("3");
  ge->Draw("lx");

  leg2->Draw();
  leg1->Draw();

  SaveCanvas(c1,
      base_dir + TString("DrawEcal_Likelihood_Sum_") + TString(c1->GetName()));
}

void
RejectionCurve_PIDComparison(
    TString base_dir =
        "../../sPHENIX_work/production_analysis_cemc2x2/emcstudies/pidstudies/spacal2d/fieldmap/",
    TString kine_config = "eta0", //
    const double min_eff = 0.85)
{

  TCanvas *c1 = new TCanvas("RejectionCurve_PIDComparison_" + kine_config,
      "RejectionCurve_PIDComparison_" + kine_config, 900, 900);
  c1->Divide(1, 1);
  int idx = 1;
  TPad * p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();
  p->SetGridx(0);
  p->SetGridy(0);

  p->DrawFrame(min_eff, 10, 1, 1e3, ";Electron Efficiency;Hadron Rejection");
  TLine * l = new TLine(0.95, 10, 0.95, 1e3);
  l->SetLineColor(kGray);
  l->SetLineWidth(5);
  l->Draw();

  TLatex * text = new TLatex(0.85, 1e3 * 1.1,
      "2-D Proj. SPACAL, e^{-}-#pi^{-} separation @ 0 < #eta < 0.1");
  text->SetTextAlign(11);
  text->SetTextSize(0.044);
  text->SetTextFont(42);
  text->Draw();

  TLegend * leg1 = new TLegend(0.12, 0.15, 0.65, 0.4);
  TLegend * leg2 = new TLegend(0.65, 0.15, 0.85, 0.4);

  TGraphErrors *ge = new TGraphErrors();
  ge->SetLineWidth(3);
  leg1->AddEntry(ge, "EMCal+HCal Likelihood", "lx");
  TGraphErrors *ge = new TGraphErrors();
  ge->SetLineWidth(3);
  ge->SetLineStyle(kDashed);
  leg1->AddEntry(ge, "EMCal E/p Likelihood", "lx");
  TGraphErrors *ge = new TGraphErrors();
  ge->SetLineWidth(3);
  ge->SetLineStyle(kDotted);
  leg1->AddEntry(ge, "Cut on min. E/p", "lx");

  TGraphErrors *ge = ExtractRejCurve(base_dir, kine_config + "_8GeV", //
  "Edep_LL_Distribution_eval_sample", "hll_edep_diff");
  ge->SetLineColor(kRed + 2);
  ge->SetMarkerColor(kRed + 2);
  ge->SetFillColor(kRed - 9);
  ge->SetLineWidth(3);
//  ge->Draw("3");
  ge->Draw("lx");
  leg2->AddEntry(ge, "8 GeV/c", "lx");
  TGraphErrors *ge = ExtractRejCurve(base_dir, kine_config + "_8GeV", //
  "EP_LL_Distribution_eval_sample", "hll_ep_diff");
  ge->SetLineColor(kRed + 2);
  ge->SetMarkerColor(kRed + 2);
  ge->SetFillColor(kRed - 9);
  ge->SetLineWidth(3);
  ge->SetLineStyle(kDashed);
//  ge->Draw("3");
  ge->Draw("lx");
  TGraphErrors *ge = ExtractRejCurve(base_dir, kine_config + "_8GeV", //
  "Ep_Checks_eval_sample", "hEMCalTrk_get_ep");
  ge->SetLineColor(kRed + 2);
  ge->SetMarkerColor(kRed + 2);
  ge->SetFillColor(kRed - 9);
  ge->SetLineWidth(3);
  ge->SetLineStyle(kDotted);
//  ge->Draw("3");
  ge->Draw("lx");

  TGraphErrors *ge = ExtractRejCurve(base_dir, kine_config + "_4GeV", //
  "Edep_LL_Distribution_eval_sample", "hll_edep_diff");
  ge->SetLineColor(kBlue + 2);
  ge->SetMarkerColor(kBlue + 2);
  ge->SetFillColor(kBlue - 9);
  ge->SetLineWidth(3);
//  ge->Draw("3");
  ge->Draw("lx");
  leg2->AddEntry(ge, "4 GeV/c", "lx");
  TGraphErrors *ge = ExtractRejCurve(base_dir, kine_config + "_4GeV", //
  "EP_LL_Distribution_eval_sample", "hll_ep_diff");
  ge->SetLineColor(kBlue + 2);
  ge->SetMarkerColor(kBlue + 2);
  ge->SetFillColor(kBlue - 9);
  ge->SetLineWidth(3);
  ge->SetLineStyle(kDashed);
//  ge->Draw("3");
  ge->Draw("lx");
  TGraphErrors *ge = ExtractRejCurve(base_dir, kine_config + "_4GeV", //
  "Ep_Checks_eval_sample", "hEMCalTrk_get_ep");
  ge->SetLineColor(kBlue + 2);
  ge->SetMarkerColor(kBlue + 2);
  ge->SetFillColor(kBlue - 9);
  ge->SetLineWidth(3);
  ge->SetLineStyle(kDotted);
//  ge->Draw("3");
  ge->Draw("lx");
//
  TGraphErrors *ge = ExtractRejCurve(base_dir, kine_config + "_2GeV", //
  "Edep_LL_Distribution_eval_sample", "hll_edep_diff");
  ge->SetLineColor(kGreen + 3);
  ge->SetMarkerColor(kGreen + 3);
  ge->SetFillColor(kGreen - 9);
  ge->SetLineWidth(3);
//  ge->Draw("3");
  ge->Draw("lx");
  leg2->AddEntry(ge, "2 GeV/c", "lx");
  TGraphErrors *ge = ExtractRejCurve(base_dir, kine_config + "_2GeV", //
  "EP_LL_Distribution_eval_sample", "hll_ep_diff");
  ge->SetLineColor(kGreen + 3);
  ge->SetMarkerColor(kGreen + 3);
  ge->SetFillColor(kGreen - 9);
  ge->SetLineWidth(3);
  ge->SetLineStyle(kDashed);
//  ge->Draw("3");
  ge->Draw("lx");
  TGraphErrors *ge = ExtractRejCurve(base_dir, kine_config + "_2GeV", //
  "Ep_Checks_eval_sample", "hEMCalTrk_get_ep");
  ge->SetLineColor(kGreen + 3);
  ge->SetMarkerColor(kGreen + 3);
  ge->SetFillColor(kGreen - 9);
  ge->SetLineWidth(3);
  ge->SetLineStyle(kDotted);
//  ge->Draw("3");
  ge->Draw("lx");

  leg2->Draw();
  leg1->Draw();

  SaveCanvas(c1,
      base_dir + TString("DrawEcal_Likelihood_Sum_") + TString(c1->GetName()));
}

TGraphErrors *
ExtractRejCurve(
    TString base_dir =
        "../../sPHENIX_work/production_analysis_cemc2x2/emcstudies/pidstudies/spacal2d/fieldmap/",
    TString kine_config = "eta0_4GeV", //
    TString ll_config = "EP_LL_Distribution_ll_sample", //
    TString hist_name = "hll_ep_diff", //
    TString signal = "e-", TString background = "pi-")
{
  TH1F * hll_edep_diff_e = NULL;
  TH1F * hll_edep_diff_pi = NULL;
    {
      TFile * f =
          new TFile(
              base_dir + "G4Hits_sPHENIX_" + signal + "_" + kine_config
                  + "-ALL.root_Ana.root.lst_EMCalLikelihood.root_DrawEcal_Likelihood_"
                  + ll_config + ".root");

      assert(f);

      hll_edep_diff_e = (TH1F *) f->GetObjectChecked(hist_name, "TH1F");

      assert(hll_edep_diff_e);
    }

    {
      TFile * f =
          new TFile(
              base_dir + "/G4Hits_sPHENIX_" + background + "_" + kine_config
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

TGraphErrors *
MergeRejCurve(TGraphErrors * ge1, const double weight1, TGraphErrors * ge2,
    const double weight2, TGraphErrors * ge3 = NULL, const double weight3 = 0)
{
  assert(ge1);

  const double sum_weight = weight1 + weight2 + weight3;
  assert(sum_weight>0);

  TGraphErrors * ge = (TGraphErrors *) ge1->Clone("MergeExtractRejCurve");
  assert(ge);

  for (int i = 0; i < ge->GetN(); ++i)
    {
      double pass = 0;

      double pass_err2 = 0;

      if (weight1 != 0)
        {
          assert(ge1);
//          assert(ge1 -> GetN() == ge1 -> GetN());

          const double rej = (ge1->GetY())[i];
          const double rej_err = (ge1->GetEY())[i];

          pass += 1 / rej * weight1;
          pass_err2 += pow(rej_err / rej * (1 / rej * weight1), 2);
        }

      if (weight2 != 0)
        {
          assert(ge2);
          assert(ge2 -> GetN() == ge1 -> GetN());

          const double rej = (ge2->GetY())[i];
          const double rej_err = (ge2->GetEY())[i];

          pass += 1 / rej * weight2;
          pass_err2 += pow(rej_err / rej * (1 / rej * weight2), 2);
        }

      if (weight3 != 0)
        {
          assert(ge3);
          assert(ge3 -> GetN() == ge1 -> GetN());

          const double rej = (ge3->GetY())[i];
          const double rej_err = (ge3->GetEY())[i];

          pass += 1 / rej * weight3;
          pass_err2 += pow(rej_err / rej * (1 / rej * weight3), 2);
        }

      pass /= sum_weight;
      pass_err2 /= sum_weight * sum_weight;

      (ge->GetY())[i] = 1. / pass;
      (ge->GetEY())[i] = sqrt(pass_err2) / pass * 1. / pass;

    }

  return ge;
}

double
GetHPiRatio(const double pt, TString hadron, TString collision)
{
//  Phys. Rev. C 88, 024906 (2013)
// A.k.a. PHENIX/PPG146

  double pt1 = 0;
  double pt2 = 1;
  double ratio1 = 0;
  double ratio2 = 0;

  if (hadron == "kaon-" && collision == "pp")
    {
      pt1 = 1;
      ratio1 = 0.25;
      pt2 = 2;
      ratio2 = 0.39;
    }
  else if (hadron == "kaon-" && collision == "AuAu10%C")
    {
      pt1 = 1;
      ratio1 = 0.3;
      pt2 = 4;
      ratio2 = 0.7;
    }
  else if (hadron == "kaon+" && collision == "pp")
    {
      pt1 = 1;
      ratio1 = 0.25;
      pt2 = 2;
      ratio2 = 0.45;
    }
  else if (hadron == "kaon+" && collision == "AuAu10%C")
    {
      pt1 = 1;
      ratio1 = 0.32;
      pt2 = 4;
      ratio2 = 0.9;
    }
  else

  if (hadron == "proton" && collision == "pp")
    {
      pt1 = 1.6;
      ratio1 = 0.25;
      pt2 = 3;
      ratio2 = 0.21;
    }
  else if (hadron == "proton" && collision == "AuAu10%C")
    {
      pt1 = 2.6;
      ratio1 = 0.77;
      pt2 = 6;
      ratio2 = 0.35;
    }
  else if (hadron == "anti_proton" && collision == "pp")
    {
      pt1 = 1.6;
      ratio1 = 0.21;
      pt2 = 3;
      ratio2 = 0.2;
    }
  else if (hadron == "anti_proton" && collision == "AuAu10%C")
    {
      pt1 = 2.6;
      ratio1 = 0.55;
      pt2 = 6;
      ratio2 = 0.3;
    }
  else
    {
      cout << "GetHPiRatio - invalid inputs!" << endl;
      assert(0);
    }

  assert(pt>1);
  const double ratio = ratio2 * (pt - pt1) / (pt2 - pt1)
      + ratio1 * (pt2 - pt) / (pt2 - pt1);
  assert(ratio > 0);

  return ratio;
}

