#include <TChain.h>
#include <TFile.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TF1.h>
#include <TMath.h>
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include "RooUnfold.h"
#include "RooUnfoldResponse.h"
#include "RooUnfoldBayes.h"

void do_unfolding_iteration_check(TH2D* h_respmatrix, TH1D* h_spectrum, string figure_name) {
  TH1D* h_meas = (TH1D*)h_respmatrix->ProjectionX("h_meas");
  TH1D* h_truth = (TH1D*)h_respmatrix->ProjectionY("h_truth");
  const int niter = 20;
  TH1D* h_unfolded[niter];
  for (int i = 0; i < niter; ++i) {
    RooUnfoldResponse* response = new RooUnfoldResponse(h_meas, h_truth, h_respmatrix, "response", "");
    RooUnfoldBayes unfold(response, h_spectrum, i + 1);
    h_unfolded[i] = (TH1D*)unfold.Hunfold(RooUnfold::kErrors);
    h_unfolded[i]->SetName(Form("h_unfolded_iter_%d", i + 1));
    delete response;
  }

  TGraph* g_var_iter = new TGraph(niter-1);
  TGraph* g_var_error = new TGraph(niter-1);
  TGraph* g_var_total = new TGraph(niter-1);
  for (int i = 1; i < niter; ++i) {
    double var_iter = 0;
    double var_error = 0;
    double var_total = 0;
    for (int j = 3; j <= h_unfolded[i]->GetNbinsX()-1; ++j) {
      double diff = h_unfolded[i]->GetBinContent(j) - h_unfolded[i-1]->GetBinContent(j);
      double cont = (h_unfolded[i]->GetBinContent(j) + h_unfolded[i-1]->GetBinContent(j)) / 2.0;
      var_iter += diff * diff / (double)(cont * cont);
      var_error += h_unfolded[i]->GetBinError(j) * h_unfolded[i]->GetBinError(j) / (double)(h_unfolded[i]->GetBinContent(j) * h_unfolded[i]->GetBinContent(j));
    }
    var_iter = std::sqrt(var_iter / (double)(h_unfolded[i]->GetNbinsX()-3));
    var_error = std::sqrt(var_error / (double)(h_unfolded[i]->GetNbinsX()-3));
    var_total = std::sqrt(var_iter * var_iter + var_error * var_error);

    g_var_iter->SetPoint(i-1, i+1, var_iter);
    g_var_error->SetPoint(i-1, i+1, var_error);
    g_var_total->SetPoint(i-1, i+1, var_total);
  }
  TCanvas* can = new TCanvas("can", "Unfolding Iteration Check", 800, 600);
  g_var_iter->SetMarkerStyle(20);
  g_var_iter->SetMarkerColor(kBlue);
  g_var_iter->SetLineColor(kBlue);
  g_var_error->SetMarkerStyle(20);
  g_var_error->SetMarkerColor(kRed);
  g_var_error->SetLineColor(kRed);
  g_var_total->SetMarkerStyle(20);
  g_var_total->SetMarkerColor(kBlack);
  g_var_total->SetLineColor(kBlack);
  g_var_total->SetMinimum(0);
  g_var_total->Draw("AP");
  g_var_iter->Draw("P same");
  g_var_error->Draw("P same");
  g_var_iter->GetXaxis()->SetTitle("Iteration");
  can->SaveAs((figure_name + ".png").c_str());
}

void get_unfolded_spectrum(TH2D* h_respmatrix, TH1D* h_spectrum, TH1D*& h_unfolded, int niter, std::string hist_name, TH1D* h_eff) {
  TH1D* h_meas = (TH1D*)h_respmatrix->ProjectionX("h_meas");
  TH1D* h_truth = (TH1D*)h_respmatrix->ProjectionY("h_truth");
  RooUnfoldResponse* response = new RooUnfoldResponse(h_meas, h_truth, h_respmatrix, "response", "");
  RooUnfoldBayes unfold(response, h_spectrum, niter);
  h_unfolded = (TH1D*)unfold.Hunfold(RooUnfold::kErrors);
  h_unfolded->SetName(hist_name.c_str());
  h_unfolded->Divide(h_eff);
  delete response;
}

////////////////////////////////////////// Main Function //////////////////////////////////////////
void do_unfolding_iter(int radius_index = 4) {
  //********** General Set up **********//
  float jet_radius = 0.1 * radius_index;
 
  //********** Files **********//
  TFile *f_out = new TFile(Form("output_unfolded_r0%d.root", radius_index), "RECREATE");
  TFile *f_data = new TFile(Form("output_data_puritycorr_r0%d.root", radius_index), "READ");
  TFile *f_in_rm = new TFile(Form("output_sim_r0%d.root", radius_index), "READ");
  TFile *f_efficiency = new TFile(Form("output_purityefficiency_r0%d.root", radius_index), "READ");

  // Get data spectrum
  TH1D* h_calibjet_pt_puritycorr_all = (TH1D*)f_data->Get("h_calibjet_pt_puritycorr_all");
  TH1D* h_calibjet_pt_puritycorr_zvertex30 = (TH1D*)f_data->Get("h_calibjet_pt_puritycorr_zvertex30");
  TH1D* h_calibjet_pt_puritycorr_zvertex60 = (TH1D*)f_data->Get("h_calibjet_pt_puritycorr_zvertex60");
  TH1D* h_calibjet_pt_puritycorr_all_jesup = (TH1D*)f_data->Get("h_calibjet_pt_puritycorr_all_jesup");
  TH1D* h_calibjet_pt_puritycorr_all_jesdown = (TH1D*)f_data->Get("h_calibjet_pt_puritycorr_all_jesdown");
  TH1D* h_calibjet_pt_puritycorr_zvertex30_jesup = (TH1D*)f_data->Get("h_calibjet_pt_puritycorr_zvertex30_jesup");
  TH1D* h_calibjet_pt_puritycorr_zvertex30_jesdown = (TH1D*)f_data->Get("h_calibjet_pt_puritycorr_zvertex30_jesdown");
  TH1D* h_calibjet_pt_puritycorr_zvertex60_jesup = (TH1D*)f_data->Get("h_calibjet_pt_puritycorr_zvertex60_jesup");
  TH1D* h_calibjet_pt_puritycorr_zvertex60_jesdown = (TH1D*)f_data->Get("h_calibjet_pt_puritycorr_zvertex60_jesdown");
  TH1D* h_calibjet_pt_puritycorr_all_jerup = (TH1D*)f_data->Get("h_calibjet_pt_puritycorr_all_jerup");
  TH1D* h_calibjet_pt_puritycorr_all_jerdown = (TH1D*)f_data->Get("h_calibjet_pt_puritycorr_all_jerdown");
  TH1D* h_calibjet_pt_puritycorr_zvertex30_jerup = (TH1D*)f_data->Get("h_calibjet_pt_puritycorr_zvertex30_jerup");
  TH1D* h_calibjet_pt_puritycorr_zvertex30_jerdown = (TH1D*)f_data->Get("h_calibjet_pt_puritycorr_zvertex30_jerdown");
  TH1D* h_calibjet_pt_puritycorr_zvertex60_jerup = (TH1D*)f_data->Get("h_calibjet_pt_puritycorr_zvertex60_jerup");
  TH1D* h_calibjet_pt_puritycorr_zvertex60_jerdown = (TH1D*)f_data->Get("h_calibjet_pt_puritycorr_zvertex60_jerdown");
  TH1D* h_calibjet_pt_puritycorr_all_jetup = (TH1D*)f_data->Get("h_calibjet_pt_puritycorr_all_jetup");
  TH1D* h_calibjet_pt_puritycorr_all_jetdown = (TH1D*)f_data->Get("h_calibjet_pt_puritycorr_all_jetdown");
  TH1D* h_calibjet_pt_puritycorr_zvertex30_jetup = (TH1D*)f_data->Get("h_calibjet_pt_puritycorr_zvertex30_jetup");
  TH1D* h_calibjet_pt_puritycorr_zvertex30_jetdown = (TH1D*)f_data->Get("h_calibjet_pt_puritycorr_zvertex30_jetdown");
  TH1D* h_calibjet_pt_puritycorr_zvertex60_jetup = (TH1D*)f_data->Get("h_calibjet_pt_puritycorr_zvertex60_jetup");
  TH1D* h_calibjet_pt_puritycorr_zvertex60_jetdown = (TH1D*)f_data->Get("h_calibjet_pt_puritycorr_zvertex60_jetdown");
  TH1D* h_calibjet_pt_puritycorr_zvertex30_mbdup = (TH1D*)f_data->Get("h_calibjet_pt_puritycorr_zvertex30_mbdup");
  TH1D* h_calibjet_pt_puritycorr_zvertex30_mbddown = (TH1D*)f_data->Get("h_calibjet_pt_puritycorr_zvertex30_mbddown");
  TH1D* h_calibjet_pt_puritycorr_zvertex60_mbdup = (TH1D*)f_data->Get("h_calibjet_pt_puritycorr_zvertex60_mbdup");
  TH1D* h_calibjet_pt_puritycorr_zvertex60_mbddown = (TH1D*)f_data->Get("h_calibjet_pt_puritycorr_zvertex60_mbddown");

  // Response matrix
  TH2D* h_respmatrix_all = (TH2D*)f_in_rm->Get("h_respmatrix_all");
  TH2D* h_respmatrix_zvertex30 = (TH2D*)f_in_rm->Get("h_respmatrix_zvertex30");
  TH2D* h_respmatrix_zvertex60 = (TH2D*)f_in_rm->Get("h_respmatrix_zvertex60");
  TH2D* h_respmatrix_all_jesup = (TH2D*)f_in_rm->Get("h_respmatrix_all_jesup");
  TH2D* h_respmatrix_all_jesdown = (TH2D*)f_in_rm->Get("h_respmatrix_all_jesdown");
  TH2D* h_respmatrix_zvertex30_jesup = (TH2D*)f_in_rm->Get("h_respmatrix_zvertex30_jesup");
  TH2D* h_respmatrix_zvertex30_jesdown = (TH2D*)f_in_rm->Get("h_respmatrix_zvertex30_jesdown");
  TH2D* h_respmatrix_zvertex60_jesup = (TH2D*)f_in_rm->Get("h_respmatrix_zvertex60_jesup");
  TH2D* h_respmatrix_zvertex60_jesdown = (TH2D*)f_in_rm->Get("h_respmatrix_zvertex60_jesdown");
  TH2D* h_respmatrix_all_jerup = (TH2D*)f_in_rm->Get("h_respmatrix_all_jerup");
  TH2D* h_respmatrix_all_jerdown = (TH2D*)f_in_rm->Get("h_respmatrix_all_jerdown");
  TH2D* h_respmatrix_zvertex30_jerup = (TH2D*)f_in_rm->Get("h_respmatrix_zvertex30_jerup");
  TH2D* h_respmatrix_zvertex30_jerdown = (TH2D*)f_in_rm->Get("h_respmatrix_zvertex30_jerdown");
  TH2D* h_respmatrix_zvertex60_jerup = (TH2D*)f_in_rm->Get("h_respmatrix_zvertex60_jerup");
  TH2D* h_respmatrix_zvertex60_jerdown = (TH2D*)f_in_rm->Get("h_respmatrix_zvertex60_jerdown");
  TH2D* h_respmatrix_all_jetup = (TH2D*)h_respmatrix_all->Clone("h_respmatrix_all_jetup");
  TH2D* h_respmatrix_all_jetdown = (TH2D*)h_respmatrix_all->Clone("h_respmatrix_all_jetdown");
  TH2D* h_respmatrix_zvertex30_jetup = (TH2D*)h_respmatrix_zvertex30->Clone("h_respmatrix_zvertex30_jetup");
  TH2D* h_respmatrix_zvertex30_jetdown = (TH2D*)h_respmatrix_zvertex30->Clone("h_respmatrix_zvertex30_jetdown");
  TH2D* h_respmatrix_zvertex60_jetup = (TH2D*)h_respmatrix_zvertex60->Clone("h_respmatrix_zvertex60_jetup");
  TH2D* h_respmatrix_zvertex60_jetdown = (TH2D*)h_respmatrix_zvertex60->Clone("h_respmatrix_zvertex60_jetdown");
  TH2D* h_respmatrix_zvertex30_mbdup = (TH2D*)h_respmatrix_zvertex30->Clone("h_respmatrix_zvertex30_mbdup");
  TH2D* h_respmatrix_zvertex30_mbddown = (TH2D*)h_respmatrix_zvertex30->Clone("h_respmatrix_zvertex30_mbddown");
  TH2D* h_respmatrix_zvertex60_mbdup = (TH2D*)h_respmatrix_zvertex60->Clone("h_respmatrix_zvertex60_mbdup");
  TH2D* h_respmatrix_zvertex60_mbddown = (TH2D*)h_respmatrix_zvertex60->Clone("h_respmatrix_zvertex60_mbddown");

  // Get efficiency
  TH1D* h_efficiency_all = (TH1D*)f_efficiency->Get("h_efficiency_all");
  TH1D* h_efficiency_zvertex30 = (TH1D*)f_efficiency->Get("h_efficiency_zvertex30");
  TH1D* h_efficiency_zvertex60 = (TH1D*)f_efficiency->Get("h_efficiency_zvertex60");
  TH1D* h_efficiency_all_jesup = (TH1D*)f_efficiency->Get("h_efficiency_all_jesup");
  TH1D* h_efficiency_all_jesdown = (TH1D*)f_efficiency->Get("h_efficiency_all_jesdown");
  TH1D* h_efficiency_zvertex30_jesup = (TH1D*)f_efficiency->Get("h_efficiency_zvertex30_jesup");
  TH1D* h_efficiency_zvertex30_jesdown = (TH1D*)f_efficiency->Get("h_efficiency_zvertex30_jesdown");
  TH1D* h_efficiency_zvertex60_jesup = (TH1D*)f_efficiency->Get("h_efficiency_zvertex60_jesup");
  TH1D* h_efficiency_zvertex60_jesdown = (TH1D*)f_efficiency->Get("h_efficiency_zvertex60_jesdown");
  TH1D* h_efficiency_all_jerup = (TH1D*)f_efficiency->Get("h_efficiency_all_jerup");
  TH1D* h_efficiency_all_jerdown = (TH1D*)f_efficiency->Get("h_efficiency_all_jerdown");
  TH1D* h_efficiency_zvertex30_jerup = (TH1D*)f_efficiency->Get("h_efficiency_zvertex30_jerup");
  TH1D* h_efficiency_zvertex30_jerdown = (TH1D*)f_efficiency->Get("h_efficiency_zvertex30_jerdown");
  TH1D* h_efficiency_zvertex60_jerup = (TH1D*)f_efficiency->Get("h_efficiency_zvertex60_jerup");
  TH1D* h_efficiency_zvertex60_jerdown = (TH1D*)f_efficiency->Get("h_efficiency_zvertex60_jerdown");
  TH1D* h_efficiency_all_jetup = (TH1D*)f_efficiency->Get("h_efficiency_all_jetup");
  TH1D* h_efficiency_all_jetdown = (TH1D*)f_efficiency->Get("h_efficiency_all_jetdown");
  TH1D* h_efficiency_zvertex30_jetup = (TH1D*)f_efficiency->Get("h_efficiency_zvertex30_jetup");
  TH1D* h_efficiency_zvertex30_jetdown = (TH1D*)f_efficiency->Get("h_efficiency_zvertex30_jetdown");
  TH1D* h_efficiency_zvertex60_jetup = (TH1D*)f_efficiency->Get("h_efficiency_zvertex60_jetup");
  TH1D* h_efficiency_zvertex60_jetdown = (TH1D*)f_efficiency->Get("h_efficiency_zvertex60_jetdown");
  TH1D* h_efficiency_zvertex30_mbdup = (TH1D*)f_efficiency->Get("h_efficiency_zvertex30_mbdup");
  TH1D* h_efficiency_zvertex30_mbddown = (TH1D*)f_efficiency->Get("h_efficiency_zvertex30_mbddown");
  TH1D* h_efficiency_zvertex60_mbdup = (TH1D*)f_efficiency->Get("h_efficiency_zvertex60_mbdup");
  TH1D* h_efficiency_zvertex60_mbddown = (TH1D*)f_efficiency->Get("h_efficiency_zvertex60_mbddown");

  //********** Response Matrix **********//
  do_unfolding_iteration_check(h_respmatrix_all, h_calibjet_pt_puritycorr_all, "unfolding_iteration_check_all");
  do_unfolding_iteration_check(h_respmatrix_zvertex30, h_calibjet_pt_puritycorr_zvertex30, "unfolding_iteration_check_zvertex30");
  do_unfolding_iteration_check(h_respmatrix_zvertex60, h_calibjet_pt_puritycorr_zvertex60, "unfolding_iteration_check_zvertex60");

  TH1D* h_unfold_all; get_unfolded_spectrum(h_respmatrix_all, h_calibjet_pt_puritycorr_all, h_unfold_all, 1, "h_unfold_all", h_efficiency_all);
  TH1D* h_unfold_all_jesup; get_unfolded_spectrum(h_respmatrix_all_jesup, h_calibjet_pt_puritycorr_all_jesup, h_unfold_all_jesup, 1, "h_unfold_all_jesup", h_efficiency_all_jesup);
  TH1D* h_unfold_all_jesdown; get_unfolded_spectrum(h_respmatrix_all_jesdown, h_calibjet_pt_puritycorr_all_jesdown, h_unfold_all_jesdown, 1, "h_unfold_all_jesdown", h_efficiency_all_jesdown);
  TH1D* h_unfold_all_jerup; get_unfolded_spectrum(h_respmatrix_all_jerup, h_calibjet_pt_puritycorr_all_jerup, h_unfold_all_jerup, 1, "h_unfold_all_jerup", h_efficiency_all_jerup);
  TH1D* h_unfold_all_jerdown; get_unfolded_spectrum(h_respmatrix_all_jerdown, h_calibjet_pt_puritycorr_all_jerdown, h_unfold_all_jerdown, 1, "h_unfold_all_jerdown", h_efficiency_all_jerdown);
  TH1D* h_unfold_all_jetup; get_unfolded_spectrum(h_respmatrix_all_jetup, h_calibjet_pt_puritycorr_all_jetup, h_unfold_all_jetup, 1, "h_unfold_all_jetup", h_efficiency_all_jetup);
  TH1D* h_unfold_all_jetdown; get_unfolded_spectrum(h_respmatrix_all_jetdown, h_calibjet_pt_puritycorr_all_jetdown, h_unfold_all_jetdown, 1, "h_unfold_all_jetdown", h_efficiency_all_jetdown);
  TH1D* h_unfold_all_unfoldunc; get_unfolded_spectrum(h_respmatrix_all, h_calibjet_pt_puritycorr_all, h_unfold_all_unfoldunc, 2, "h_unfold_all_unfoldunc", h_efficiency_all);

  TH1D* h_unfold_zvertex30; get_unfolded_spectrum(h_respmatrix_zvertex30, h_calibjet_pt_puritycorr_zvertex30, h_unfold_zvertex30, 1, "h_unfold_zvertex30", h_efficiency_zvertex30);
  TH1D* h_unfold_zvertex30_jesup; get_unfolded_spectrum(h_respmatrix_zvertex30_jesup, h_calibjet_pt_puritycorr_zvertex30_jesup, h_unfold_zvertex30_jesup, 1, "h_unfold_zvertex30_jesup", h_efficiency_zvertex30_jesup);
  TH1D* h_unfold_zvertex30_jesdown; get_unfolded_spectrum(h_respmatrix_zvertex30_jesdown, h_calibjet_pt_puritycorr_zvertex30_jesdown, h_unfold_zvertex30_jesdown, 1, "h_unfold_zvertex30_jesdown", h_efficiency_zvertex30_jesdown);
  TH1D* h_unfold_zvertex30_jerup; get_unfolded_spectrum(h_respmatrix_zvertex30_jerup, h_calibjet_pt_puritycorr_zvertex30_jerup, h_unfold_zvertex30_jerup, 1, "h_unfold_zvertex30_jerup", h_efficiency_zvertex30_jerup);
  TH1D* h_unfold_zvertex30_jerdown; get_unfolded_spectrum(h_respmatrix_zvertex30_jerdown, h_calibjet_pt_puritycorr_zvertex30_jerdown, h_unfold_zvertex30_jerdown, 1, "h_unfold_zvertex30_jerdown", h_efficiency_zvertex30_jerdown);
  TH1D* h_unfold_zvertex30_jetup; get_unfolded_spectrum(h_respmatrix_zvertex30_jetup, h_calibjet_pt_puritycorr_zvertex30_jetup, h_unfold_zvertex30_jetup, 1, "h_unfold_zvertex30_jetup", h_efficiency_zvertex30_jetup);
  TH1D* h_unfold_zvertex30_jetdown; get_unfolded_spectrum(h_respmatrix_zvertex30_jetdown, h_calibjet_pt_puritycorr_zvertex30_jetdown, h_unfold_zvertex30_jetdown, 1, "h_unfold_zvertex30_jetdown", h_efficiency_zvertex30_jetdown);
  TH1D* h_unfold_zvertex30_mbdup; get_unfolded_spectrum(h_respmatrix_zvertex30_mbdup, h_calibjet_pt_puritycorr_zvertex30_mbdup, h_unfold_zvertex30_mbdup, 1, "h_unfold_zvertex30_mbdup", h_efficiency_zvertex30_mbdup);
  TH1D* h_unfold_zvertex30_mbddown; get_unfolded_spectrum(h_respmatrix_zvertex30_mbddown, h_calibjet_pt_puritycorr_zvertex30_mbddown, h_unfold_zvertex30_mbddown, 1, "h_unfold_zvertex30_mbddown", h_efficiency_zvertex30_mbddown);
  TH1D* h_unfold_zvertex30_unfoldunc; get_unfolded_spectrum(h_respmatrix_zvertex30, h_calibjet_pt_puritycorr_zvertex30, h_unfold_zvertex30_unfoldunc, 2, "h_unfold_zvertex30_unfoldunc", h_efficiency_zvertex30);

  TH1D* h_unfold_zvertex60; get_unfolded_spectrum(h_respmatrix_zvertex60, h_calibjet_pt_puritycorr_zvertex60, h_unfold_zvertex60, 1, "h_unfold_zvertex60", h_efficiency_zvertex60);
  TH1D* h_unfold_zvertex60_jesup; get_unfolded_spectrum(h_respmatrix_zvertex60_jesup, h_calibjet_pt_puritycorr_zvertex60_jesup, h_unfold_zvertex60_jesup, 1, "h_unfold_zvertex60_jesup", h_efficiency_zvertex60_jesup);
  TH1D* h_unfold_zvertex60_jesdown; get_unfolded_spectrum(h_respmatrix_zvertex60_jesdown, h_calibjet_pt_puritycorr_zvertex60_jesdown, h_unfold_zvertex60_jesdown, 1, "h_unfold_zvertex60_jesdown", h_efficiency_zvertex60_jesdown);
  TH1D* h_unfold_zvertex60_jerup; get_unfolded_spectrum(h_respmatrix_zvertex60_jerup, h_calibjet_pt_puritycorr_zvertex60_jerup, h_unfold_zvertex60_jerup, 1, "h_unfold_zvertex60_jerup", h_efficiency_zvertex60_jerup);
  TH1D* h_unfold_zvertex60_jerdown; get_unfolded_spectrum(h_respmatrix_zvertex60_jerdown, h_calibjet_pt_puritycorr_zvertex60_jerdown, h_unfold_zvertex60_jerdown, 1, "h_unfold_zvertex60_jerdown", h_efficiency_zvertex60_jerdown);
  TH1D* h_unfold_zvertex60_jetup; get_unfolded_spectrum(h_respmatrix_zvertex60_jetup, h_calibjet_pt_puritycorr_zvertex60_jetup, h_unfold_zvertex60_jetup, 1, "h_unfold_zvertex60_jetup", h_efficiency_zvertex60_jetup);
  TH1D* h_unfold_zvertex60_jetdown; get_unfolded_spectrum(h_respmatrix_zvertex60_jetdown, h_calibjet_pt_puritycorr_zvertex60_jetdown, h_unfold_zvertex60_jetdown, 1, "h_unfold_zvertex60_jetdown", h_efficiency_zvertex60_jetdown);
  TH1D* h_unfold_zvertex60_mbdup; get_unfolded_spectrum(h_respmatrix_zvertex60_mbdup, h_calibjet_pt_puritycorr_zvertex60_mbdup, h_unfold_zvertex60_mbdup, 1, "h_unfold_zvertex60_mbdup", h_efficiency_zvertex60_mbdup);
  TH1D* h_unfold_zvertex60_mbddown; get_unfolded_spectrum(h_respmatrix_zvertex60_mbddown, h_calibjet_pt_puritycorr_zvertex60_mbddown, h_unfold_zvertex60_mbddown, 1, "h_unfold_zvertex60_mbddown", h_efficiency_zvertex60_mbddown);
  TH1D* h_unfold_zvertex60_unfoldunc; get_unfolded_spectrum(h_respmatrix_zvertex60, h_calibjet_pt_puritycorr_zvertex60, h_unfold_zvertex60_unfoldunc, 2, "h_unfold_zvertex60_unfoldunc", h_efficiency_zvertex60);

  //********** Writing **********//
  std::cout << "Writing histograms..." << std::endl;
  f_out->cd();
  h_unfold_all->Write();
  h_unfold_all_jesup->Write();
  h_unfold_all_jesdown->Write();
  h_unfold_all_jerup->Write();
  h_unfold_all_jerdown->Write();
  h_unfold_all_jetup->Write();
  h_unfold_all_jetdown->Write();
  h_unfold_all_unfoldunc->Write();

  h_unfold_zvertex30->Write();
  h_unfold_zvertex30_jesup->Write();
  h_unfold_zvertex30_jesdown->Write();
  h_unfold_zvertex30_jerup->Write();
  h_unfold_zvertex30_jerdown->Write();
  h_unfold_zvertex30_jetup->Write();
  h_unfold_zvertex30_jetdown->Write();
  h_unfold_zvertex30_mbdup->Write();
  h_unfold_zvertex30_mbddown->Write();
  h_unfold_zvertex30_unfoldunc->Write();

  h_unfold_zvertex60->Write();
  h_unfold_zvertex60_jesup->Write();
  h_unfold_zvertex60_jesdown->Write();
  h_unfold_zvertex60_jerup->Write();
  h_unfold_zvertex60_jerdown->Write();
  h_unfold_zvertex60_jetup->Write();
  h_unfold_zvertex60_jetdown->Write();
  h_unfold_zvertex60_mbdup->Write();
  h_unfold_zvertex60_mbddown->Write();
  h_unfold_zvertex60_unfoldunc->Write();

  f_out->Close();
  std::cout << "All done!" << std::endl;
}
