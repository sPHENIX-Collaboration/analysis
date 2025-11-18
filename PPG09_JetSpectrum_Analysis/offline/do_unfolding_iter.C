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
}

////////////////////////////////////////// Main Function //////////////////////////////////////////
void do_unfolding_iter(int radius_index = 4) {
  //********** General Set up **********//
  const float PI = TMath::Pi();
  float jet_radius = 0.1 * radius_index;
 
  //********** Files **********//
  TFile *f_out = new TFile(Form("output_unfolded_r0%d.root", radius_index), "RECREATE");
  TFile *f_data = new TFile(Form("output_data_puritycorr_r0%d.root", radius_index), "READ");
  TFile *f_in_rm = new TFile(Form("output_sim_r0%d.root", radius_index), "READ");
  TFile *f_efficiency = new TFile(Form("output_purityefficiency_r0%d.root", radius_index), "READ");

  // Get data spectrum
  TH1D* h_calibjet_pt_puritycorr_nominal = (TH1D*)f_data->Get("h_calibjet_pt_puritycorr_nominal");
  TH1D* h_calibjet_pt_puritycorr_jesup = (TH1D*)f_data->Get("h_calibjet_pt_puritycorr_jesup");
  TH1D* h_calibjet_pt_puritycorr_jesdown = (TH1D*)f_data->Get("h_calibjet_pt_puritycorr_jesdown");
  TH1D* h_calibjet_pt_puritycorr_jerup = (TH1D*)f_data->Get("h_calibjet_pt_puritycorr_jerup");
  TH1D* h_calibjet_pt_puritycorr_jerdown = (TH1D*)f_data->Get("h_calibjet_pt_puritycorr_jerdown");
  TH1D* h_calibjet_pt_puritycorr_jettrigup = (TH1D*)f_data->Get("h_calibjet_pt_puritycorr_jettrigup");
  TH1D* h_calibjet_pt_puritycorr_jettrigdown = (TH1D*)f_data->Get("h_calibjet_pt_puritycorr_jettrigdown");
  TH1D* h_calibjet_pt_puritycorr_jettimingup = (TH1D*)f_data->Get("h_calibjet_pt_puritycorr_jettimingup");
  TH1D* h_calibjet_pt_puritycorr_jettimingdown = (TH1D*)f_data->Get("h_calibjet_pt_puritycorr_jettimingdown");

  // Response matrix
  TH2D* h_respmatrix_nominal = (TH2D*)f_in_rm->Get("h_respmatrix_nominal");
  TH2D* h_respmatrix_jesup = (TH2D*)f_in_rm->Get("h_respmatrix_jesup");
  TH2D* h_respmatrix_jesdown = (TH2D*)f_in_rm->Get("h_respmatrix_jesdown");
  TH2D* h_respmatrix_jerup = (TH2D*)f_in_rm->Get("h_respmatrix_jerup");
  TH2D* h_respmatrix_jerdown = (TH2D*)f_in_rm->Get("h_respmatrix_jerdown");
  TH2D* h_respmatrix_jettrigup = (TH2D*)h_respmatrix_nominal->Clone("h_respmatrix_jettrigup");
  TH2D* h_respmatrix_jettrigdown = (TH2D*)h_respmatrix_nominal->Clone("h_respmatrix_jettrigdown");
  TH2D* h_respmatrix_jettimingup = (TH2D*)h_respmatrix_nominal->Clone("h_respmatrix_jettimingup");
  TH2D* h_respmatrix_jettimingdown = (TH2D*)h_respmatrix_nominal->Clone("h_respmatrix_jettimingdown");

  // Get efficiency
  TH1D* h_efficiency_nominal = (TH1D*)f_efficiency->Get("h_efficiency_nominal");
  TH1D* h_efficiency_jesup = (TH1D*)f_efficiency->Get("h_efficiency_jesup");
  TH1D* h_efficiency_jesdown = (TH1D*)f_efficiency->Get("h_efficiency_jesdown");
  TH1D* h_efficiency_jerup = (TH1D*)f_efficiency->Get("h_efficiency_jerup");
  TH1D* h_efficiency_jerdown = (TH1D*)f_efficiency->Get("h_efficiency_jerdown");
  TH1D* h_efficiency_jettrigup = (TH1D*)f_efficiency->Get("h_efficiency_jettrigup");
  TH1D* h_efficiency_jettrigdown = (TH1D*)f_efficiency->Get("h_efficiency_jettrigdown");
  TH1D* h_efficiency_jettimingup = (TH1D*)f_efficiency->Get("h_efficiency_jettimingup");
  TH1D* h_efficiency_jettimingdown = (TH1D*)f_efficiency->Get("h_efficiency_jettimingdown");

  //********** Response Matrix **********//
  do_unfolding_iteration_check(h_respmatrix_nominal, h_calibjet_pt_puritycorr_nominal, Form("unfolding_iteration_check_nominal_r0%d", radius_index));
  int iteration = 4;

  TH1D* h_unfold_nominal; get_unfolded_spectrum(h_respmatrix_nominal, h_calibjet_pt_puritycorr_nominal, h_unfold_nominal, iteration, "h_unfold_nominal", h_efficiency_nominal);
  TH1D* h_unfold_jesup; get_unfolded_spectrum(h_respmatrix_jesup, h_calibjet_pt_puritycorr_jesup, h_unfold_jesup, iteration, "h_unfold_jesup", h_efficiency_jesup);
  TH1D* h_unfold_jesdown; get_unfolded_spectrum(h_respmatrix_jesdown, h_calibjet_pt_puritycorr_jesdown, h_unfold_jesdown, iteration, "h_unfold_jesdown", h_efficiency_jesdown);
  TH1D* h_unfold_jerup; get_unfolded_spectrum(h_respmatrix_jerup, h_calibjet_pt_puritycorr_jerup, h_unfold_jerup, iteration, "h_unfold_jerup", h_efficiency_jerup);
  TH1D* h_unfold_jerdown; get_unfolded_spectrum(h_respmatrix_jerdown, h_calibjet_pt_puritycorr_jerdown, h_unfold_jerdown, iteration, "h_unfold_jerdown", h_efficiency_jerdown);
  TH1D* h_unfold_jettrigup; get_unfolded_spectrum(h_respmatrix_jettrigup, h_calibjet_pt_puritycorr_jettrigup, h_unfold_jettrigup, iteration, "h_unfold_jettrigup", h_efficiency_jettrigup);
  TH1D* h_unfold_jettrigdown; get_unfolded_spectrum(h_respmatrix_jettrigdown, h_calibjet_pt_puritycorr_jettrigdown, h_unfold_jettrigdown, iteration, "h_unfold_jettrigdown", h_efficiency_jettrigdown);
  TH1D* h_unfold_jettimingup; get_unfolded_spectrum(h_respmatrix_jettimingup, h_calibjet_pt_puritycorr_jettimingup, h_unfold_jettimingup, iteration, "h_unfold_jettimingup", h_efficiency_jettimingup);
  TH1D* h_unfold_jettimingdown; get_unfolded_spectrum(h_respmatrix_jettimingdown, h_calibjet_pt_puritycorr_jettimingdown, h_unfold_jettimingdown, iteration, "h_unfold_jettimingdown", h_efficiency_jettimingdown);
  TH1D* h_unfold_unfolditerup; get_unfolded_spectrum(h_respmatrix_nominal, h_calibjet_pt_puritycorr_nominal, h_unfold_unfolditerup, iteration+1, "h_unfold_unfolditerup", h_efficiency_nominal);
  TH1D* h_unfold_unfolditerdown; get_unfolded_spectrum(h_respmatrix_nominal, h_calibjet_pt_puritycorr_nominal, h_unfold_unfolditerdown, iteration-1, "h_unfold_unfolditerdown", h_efficiency_nominal);

  //********** Writing **********//
  std::cout << "Writing histograms..." << std::endl;
  f_out->cd();

  h_unfold_nominal->Write();
  h_unfold_jesup->Write();
  h_unfold_jesdown->Write();
  h_unfold_jerup->Write();
  h_unfold_jerdown->Write();
  h_unfold_jettrigup->Write();
  h_unfold_jettrigdown->Write();
  h_unfold_jettimingup->Write();
  h_unfold_jettimingdown->Write();
  h_unfold_unfolditerup->Write();
  h_unfold_unfolditerdown->Write();

  f_out->Close();
  std::cout << "All done!" << std::endl;
}
