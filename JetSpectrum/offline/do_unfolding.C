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
void do_unfolding() {
  //********** General Set up **********//
  const float PI = TMath::Pi();
  const float jet_radius = 0.4;
 
  //********** Files **********//
  TFile *f_out = new TFile("output_unfolded.root", "RECREATE");

  TFile *f_data = new TFile("output_data_puritycorr.root", "READ");
  TFile *f_in_rm = new TFile("output_reweightmatrix.root", "READ");
  TFile *f_efficiency = new TFile("output_purityefficiency.root", "READ");
  TFile *f_mbd = new TFile("output_mbdtrigeff.root", "READ");

  // Get data spectrum
  TH1D* h_calibjet_pt_puritycorr_dijet = (TH1D*)f_data->Get("h_calibjet_pt_puritycorr_dijet");
  TH1D* h_calibjet_pt_puritycorr_dijet_effdown = (TH1D*)f_data->Get("h_calibjet_pt_puritycorr_dijet_effdown");
  TH1D* h_calibjet_pt_puritycorr_dijet_effup = (TH1D*)f_data->Get("h_calibjet_pt_puritycorr_dijet_effup");
  TH1D* h_calibjet_pt_puritycorr_dijet_jesdown = (TH1D*)f_data->Get("h_calibjet_pt_puritycorr_dijet_jesdown");
  TH1D* h_calibjet_pt_puritycorr_dijet_jesup = (TH1D*)f_data->Get("h_calibjet_pt_puritycorr_dijet_jesup");
  TH1D* h_calibjet_pt_puritycorr_dijet_jerdown = (TH1D*)f_data->Get("h_calibjet_pt_puritycorr_dijet_jerdown");
  TH1D* h_calibjet_pt_puritycorr_dijet_jerup = (TH1D*)f_data->Get("h_calibjet_pt_puritycorr_dijet_jerup");

  TH1D* h_calibjet_pt_puritycorr_frac = (TH1D*)f_data->Get("h_calibjet_pt_puritycorr_frac");
  TH1D* h_calibjet_pt_puritycorr_frac_effdown = (TH1D*)f_data->Get("h_calibjet_pt_puritycorr_frac_effdown");
  TH1D* h_calibjet_pt_puritycorr_frac_effup = (TH1D*)f_data->Get("h_calibjet_pt_puritycorr_frac_effup");
  TH1D* h_calibjet_pt_puritycorr_frac_jesdown = (TH1D*)f_data->Get("h_calibjet_pt_puritycorr_frac_jesdown");
  TH1D* h_calibjet_pt_puritycorr_frac_jesup = (TH1D*)f_data->Get("h_calibjet_pt_puritycorr_frac_jesup");
  TH1D* h_calibjet_pt_puritycorr_frac_jerdown = (TH1D*)f_data->Get("h_calibjet_pt_puritycorr_frac_jerdown");
  TH1D* h_calibjet_pt_puritycorr_frac_jerup = (TH1D*)f_data->Get("h_calibjet_pt_puritycorr_frac_jerup");

  // Response matrix
  TH2D* h_respmatrix_calib_dijet_reweighted = (TH2D*)f_in_rm->Get("h_respmatrix_calib_dijet_reweighted");
  TH2D* h_respmatrix_calib_dijet_effdown_reweighted = (TH2D*)f_in_rm->Get("h_respmatrix_calib_dijet_effdown_reweighted");
  TH2D* h_respmatrix_calib_dijet_effup_reweighted = (TH2D*)f_in_rm->Get("h_respmatrix_calib_dijet_effup_reweighted");
  TH2D* h_respmatrix_calib_dijet_jesdown_reweighted = (TH2D*)f_in_rm->Get("h_respmatrix_calib_dijet_jesdown_reweighted");
  TH2D* h_respmatrix_calib_dijet_jesup_reweighted = (TH2D*)f_in_rm->Get("h_respmatrix_calib_dijet_jesup_reweighted");
  TH2D* h_respmatrix_calib_dijet_jerdown_reweighted = (TH2D*)f_in_rm->Get("h_respmatrix_calib_dijet_jerdown_reweighted");
  TH2D* h_respmatrix_calib_dijet_jerup_reweighted = (TH2D*)f_in_rm->Get("h_respmatrix_calib_dijet_jerup_reweighted");

  TH2D* h_respmatrix_calib_frac_reweighted = (TH2D*)f_in_rm->Get("h_respmatrix_calib_frac_reweighted");
  TH2D* h_respmatrix_calib_frac_effdown_reweighted = (TH2D*)f_in_rm->Get("h_respmatrix_calib_frac_effdown_reweighted");
  TH2D* h_respmatrix_calib_frac_effup_reweighted = (TH2D*)f_in_rm->Get("h_respmatrix_calib_frac_effup_reweighted");
  TH2D* h_respmatrix_calib_frac_jesdown_reweighted = (TH2D*)f_in_rm->Get("h_respmatrix_calib_frac_jesdown_reweighted");
  TH2D* h_respmatrix_calib_frac_jesup_reweighted = (TH2D*)f_in_rm->Get("h_respmatrix_calib_frac_jesup_reweighted");
  TH2D* h_respmatrix_calib_frac_jerdown_reweighted = (TH2D*)f_in_rm->Get("h_respmatrix_calib_frac_jerdown_reweighted");
  TH2D* h_respmatrix_calib_frac_jerup_reweighted = (TH2D*)f_in_rm->Get("h_respmatrix_calib_frac_jerup_reweighted");

  TH2D* h_respmatrix_calib_dijet = (TH2D*)f_in_rm->Get("h_respmatrix_calib_dijet");
  TH2D* h_respmatrix_calib_frac = (TH2D*)f_in_rm->Get("h_respmatrix_calib_frac");

  // Get efficiency
  TH1D* h_efficiency_calib_dijet = (TH1D*)f_efficiency->Get("h_efficiency_calib_dijet");
  TH1D* h_efficiency_calib_dijet_jesdown = (TH1D*)f_efficiency->Get("h_efficiency_calib_dijet_jesdown");
  TH1D* h_efficiency_calib_dijet_jesup = (TH1D*)f_efficiency->Get("h_efficiency_calib_dijet_jesup");
  TH1D* h_efficiency_calib_dijet_jerdown = (TH1D*)f_efficiency->Get("h_efficiency_calib_dijet_jerdown");
  TH1D* h_efficiency_calib_dijet_jerup = (TH1D*)f_efficiency->Get("h_efficiency_calib_dijet_jerup");
  TH1D* h_efficiency_calib_frac = (TH1D*)f_efficiency->Get("h_efficiency_calib_frac");
  TH1D* h_efficiency_calib_frac_jesdown = (TH1D*)f_efficiency->Get("h_efficiency_calib_frac_jesdown");
  TH1D* h_efficiency_calib_frac_jesup = (TH1D*)f_efficiency->Get("h_efficiency_calib_frac_jesup");
  TH1D* h_efficiency_calib_frac_jerdown = (TH1D*)f_efficiency->Get("h_efficiency_calib_frac_jerdown");
  TH1D* h_efficiency_calib_frac_jerup = (TH1D*)f_efficiency->Get("h_efficiency_calib_frac_jerup");

  // Get MBD efficiency
  TH1D* h_mbd_efficiency = (TH1D*)f_mbd->Get("Ratio_h_TJetSpectrumCBin1_tzvtx_PMTHit40_MBDNS1_Over_h_TJetpTSpectrumCBin1_tzvtx");
  TH1D* h_mbd_efficiency_down = (TH1D*)f_mbd->Get("Ratio_h_TJetSpectrumCBin1_tzvtx_PMTHit30_MBDNS1_Over_h_TJetpTSpectrumCBin1_tzvtx");
  TH1D* h_mbd_efficiency_up = (TH1D*)f_mbd->Get("Ratio_h_TJetSpectrumCBin1_tzvtx_PMTHit50_MBDNS1_Over_h_TJetpTSpectrumCBin1_tzvtx");

  //********** Response Matrix **********//
  TH1D* h_unfold_calib_dijet_reweighted_1; get_unfolded_spectrum(h_respmatrix_calib_dijet_reweighted, h_calibjet_pt_puritycorr_dijet, h_unfold_calib_dijet_reweighted_1, 1, "h_unfold_calib_dijet_reweighted_1", h_efficiency_calib_dijet);
  TH1D* h_unfold_calib_dijet_effdown_reweighted_1; get_unfolded_spectrum(h_respmatrix_calib_dijet_effdown_reweighted, h_calibjet_pt_puritycorr_dijet_effdown, h_unfold_calib_dijet_effdown_reweighted_1, 1, "h_unfold_calib_dijet_effdown_reweighted_1", h_efficiency_calib_dijet);
  TH1D* h_unfold_calib_dijet_effup_reweighted_1; get_unfolded_spectrum(h_respmatrix_calib_dijet_effup_reweighted, h_calibjet_pt_puritycorr_dijet_effup, h_unfold_calib_dijet_effup_reweighted_1, 1, "h_unfold_calib_dijet_effup_reweighted_1", h_efficiency_calib_dijet);
  TH1D* h_unfold_calib_dijet_jesdown_reweighted_1; get_unfolded_spectrum(h_respmatrix_calib_dijet_jesdown_reweighted, h_calibjet_pt_puritycorr_dijet_jesdown, h_unfold_calib_dijet_jesdown_reweighted_1, 1, "h_unfold_calib_dijet_jesdown_reweighted_1", h_efficiency_calib_dijet_jesdown);
  TH1D* h_unfold_calib_dijet_jesup_reweighted_1; get_unfolded_spectrum(h_respmatrix_calib_dijet_jesup_reweighted, h_calibjet_pt_puritycorr_dijet_jesup, h_unfold_calib_dijet_jesup_reweighted_1, 1, "h_unfold_calib_dijet_jesup_reweighted_1", h_efficiency_calib_dijet_jesup);
  TH1D* h_unfold_calib_dijet_jerdown_reweighted_1; get_unfolded_spectrum(h_respmatrix_calib_dijet_jerdown_reweighted, h_calibjet_pt_puritycorr_dijet_jerdown, h_unfold_calib_dijet_jerdown_reweighted_1, 1, "h_unfold_calib_dijet_jerdown_reweighted_1", h_efficiency_calib_dijet_jerdown);
  TH1D* h_unfold_calib_dijet_jerup_reweighted_1; get_unfolded_spectrum(h_respmatrix_calib_dijet_jerup_reweighted, h_calibjet_pt_puritycorr_dijet_jerup, h_unfold_calib_dijet_jerup_reweighted_1, 1, "h_unfold_calib_dijet_jerup_reweighted_1", h_efficiency_calib_dijet_jerup);

  TH1D* h_unfold_calib_frac_reweighted_1; get_unfolded_spectrum(h_respmatrix_calib_frac_reweighted, h_calibjet_pt_puritycorr_frac, h_unfold_calib_frac_reweighted_1, 1, "h_unfold_calib_frac_reweighted_1", h_efficiency_calib_frac);
  TH1D* h_unfold_calib_frac_effdown_reweighted_1; get_unfolded_spectrum(h_respmatrix_calib_frac_effdown_reweighted, h_calibjet_pt_puritycorr_frac_effdown, h_unfold_calib_frac_effdown_reweighted_1, 1, "h_unfold_calib_frac_effdown_reweighted_1", h_efficiency_calib_frac);
  TH1D* h_unfold_calib_frac_effup_reweighted_1; get_unfolded_spectrum(h_respmatrix_calib_frac_effup_reweighted, h_calibjet_pt_puritycorr_frac_effup, h_unfold_calib_frac_effup_reweighted_1, 1, "h_unfold_calib_frac_effup_reweighted_1", h_efficiency_calib_frac);
  TH1D* h_unfold_calib_frac_jesdown_reweighted_1; get_unfolded_spectrum(h_respmatrix_calib_frac_jesdown_reweighted, h_calibjet_pt_puritycorr_frac_jesdown, h_unfold_calib_frac_jesdown_reweighted_1, 1, "h_unfold_calib_frac_jesdown_reweighted_1", h_efficiency_calib_frac_jesdown);
  TH1D* h_unfold_calib_frac_jesup_reweighted_1; get_unfolded_spectrum(h_respmatrix_calib_frac_jesup_reweighted, h_calibjet_pt_puritycorr_frac_jesup, h_unfold_calib_frac_jesup_reweighted_1, 1, "h_unfold_calib_frac_jesup_reweighted_1", h_efficiency_calib_frac_jesup);
  TH1D* h_unfold_calib_frac_jerdown_reweighted_1; get_unfolded_spectrum(h_respmatrix_calib_frac_jerdown_reweighted, h_calibjet_pt_puritycorr_frac_jerdown, h_unfold_calib_frac_jerdown_reweighted_1, 1, "h_unfold_calib_frac_jerdown_reweighted_1", h_efficiency_calib_frac_jerdown);
  TH1D* h_unfold_calib_frac_jerup_reweighted_1; get_unfolded_spectrum(h_respmatrix_calib_frac_jerup_reweighted, h_calibjet_pt_puritycorr_frac_jerup, h_unfold_calib_frac_jerup_reweighted_1, 1, "h_unfold_calib_frac_jerup_reweighted_1", h_efficiency_calib_frac_jerup);

  TH1D* h_unfold_calib_dijet_reweighted_2; get_unfolded_spectrum(h_respmatrix_calib_dijet_reweighted, h_calibjet_pt_puritycorr_dijet, h_unfold_calib_dijet_reweighted_2, 2, "h_unfold_calib_dijet_reweighted_2", h_efficiency_calib_dijet);
  TH1D* h_unfold_calib_dijet_reweighted_3; get_unfolded_spectrum(h_respmatrix_calib_dijet_reweighted, h_calibjet_pt_puritycorr_dijet, h_unfold_calib_dijet_reweighted_3, 3, "h_unfold_calib_dijet_reweighted_3", h_efficiency_calib_dijet);
  TH1D* h_unfold_calib_dijet_mbddown_reweighted_1; get_unfolded_spectrum(h_respmatrix_calib_dijet_reweighted, h_calibjet_pt_puritycorr_dijet, h_unfold_calib_dijet_mbddown_reweighted_1, 1, "h_unfold_calib_dijet_mbddown_reweighted_1", h_efficiency_calib_dijet);
  TH1D* h_unfold_calib_dijet_mbdup_reweighted_1; get_unfolded_spectrum(h_respmatrix_calib_dijet_reweighted, h_calibjet_pt_puritycorr_dijet, h_unfold_calib_dijet_mbdup_reweighted_1, 1, "h_unfold_calib_dijet_mbdup_reweighted_1", h_efficiency_calib_dijet);
  TH1D* h_unfold_calib_frac_reweighted_2; get_unfolded_spectrum(h_respmatrix_calib_frac_reweighted, h_calibjet_pt_puritycorr_frac, h_unfold_calib_frac_reweighted_2, 2, "h_unfold_calib_frac_reweighted_2", h_efficiency_calib_frac);
  TH1D* h_unfold_calib_frac_reweighted_3; get_unfolded_spectrum(h_respmatrix_calib_frac_reweighted, h_calibjet_pt_puritycorr_frac, h_unfold_calib_frac_reweighted_3, 3, "h_unfold_calib_frac_reweighted_3", h_efficiency_calib_frac);
  TH1D* h_unfold_calib_frac_mbddown_reweighted_1; get_unfolded_spectrum(h_respmatrix_calib_frac_reweighted, h_calibjet_pt_puritycorr_frac, h_unfold_calib_frac_mbddown_reweighted_1, 1, "h_unfold_calib_frac_mbddown_reweighted_1", h_efficiency_calib_frac);
  TH1D* h_unfold_calib_frac_mbdup_reweighted_1; get_unfolded_spectrum(h_respmatrix_calib_frac_reweighted, h_calibjet_pt_puritycorr_frac, h_unfold_calib_frac_mbdup_reweighted_1, 1, "h_unfold_calib_frac_mbdup_reweighted_1", h_efficiency_calib_frac);

  TH1D* h_unfold_calib_dijet_1; get_unfolded_spectrum(h_respmatrix_calib_dijet, h_calibjet_pt_puritycorr_dijet, h_unfold_calib_dijet_1, 2, "h_unfold_calib_dijet_1", h_efficiency_calib_dijet);
  TH1D* h_unfold_calib_frac_1; get_unfolded_spectrum(h_respmatrix_calib_frac, h_calibjet_pt_puritycorr_frac, h_unfold_calib_frac_1, 2, "h_unfold_calib_frac_1", h_efficiency_calib_frac);
  for (int ib = 1; ib <= h_unfold_calib_dijet_1->GetNbinsX(); ++ib) {
    std::cout << "bin " << ib << " : " << h_unfold_calib_dijet_1->GetBinContent(ib) << "\t" << h_unfold_calib_dijet_reweighted_1->GetBinContent(ib) << "\t\t" << h_unfold_calib_frac_1->GetBinContent(ib) << "\t" << h_unfold_calib_frac_reweighted_1->GetBinContent(ib) << std::endl;
  }

  //********** MBD Efficiency Correction **********//
  int n_underflow_bin = 2;
  int n_overflow_bin = 1;
  for (int ib = 1 + n_underflow_bin; ib <= h_unfold_calib_dijet_reweighted_1->GetNbinsX() - n_overflow_bin; ++ib) {
    double mbd_efficiency = h_mbd_efficiency->GetBinContent(ib - n_underflow_bin) / 0.57 * (26.1 / 42.);

    h_unfold_calib_dijet_reweighted_1->SetBinContent(ib, h_unfold_calib_dijet_reweighted_1->GetBinContent(ib) / mbd_efficiency); h_unfold_calib_dijet_reweighted_1->SetBinError(ib, h_unfold_calib_dijet_reweighted_1->GetBinError(ib) / mbd_efficiency);
    h_unfold_calib_dijet_effdown_reweighted_1->SetBinContent(ib, h_unfold_calib_dijet_effdown_reweighted_1->GetBinContent(ib) / mbd_efficiency); h_unfold_calib_dijet_effdown_reweighted_1->SetBinError(ib, h_unfold_calib_dijet_effdown_reweighted_1->GetBinError(ib) / mbd_efficiency);
    h_unfold_calib_dijet_effup_reweighted_1->SetBinContent(ib, h_unfold_calib_dijet_effup_reweighted_1->GetBinContent(ib) / mbd_efficiency); h_unfold_calib_dijet_effup_reweighted_1->SetBinError(ib, h_unfold_calib_dijet_effup_reweighted_1->GetBinError(ib) / mbd_efficiency);
    h_unfold_calib_dijet_jesdown_reweighted_1->SetBinContent(ib, h_unfold_calib_dijet_jesdown_reweighted_1->GetBinContent(ib) / mbd_efficiency); h_unfold_calib_dijet_jesdown_reweighted_1->SetBinError(ib, h_unfold_calib_dijet_jesdown_reweighted_1->GetBinError(ib) / mbd_efficiency);
    h_unfold_calib_dijet_jesup_reweighted_1->SetBinContent(ib, h_unfold_calib_dijet_jesup_reweighted_1->GetBinContent(ib) / mbd_efficiency); h_unfold_calib_dijet_jesup_reweighted_1->SetBinError(ib, h_unfold_calib_dijet_jesup_reweighted_1->GetBinError(ib) / mbd_efficiency);
    h_unfold_calib_dijet_jerdown_reweighted_1->SetBinContent(ib, h_unfold_calib_dijet_jerdown_reweighted_1->GetBinContent(ib) / mbd_efficiency); h_unfold_calib_dijet_jerdown_reweighted_1->SetBinError(ib, h_unfold_calib_dijet_jerdown_reweighted_1->GetBinError(ib) / mbd_efficiency);
    h_unfold_calib_dijet_jerup_reweighted_1->SetBinContent(ib, h_unfold_calib_dijet_jerup_reweighted_1->GetBinContent(ib) / mbd_efficiency); h_unfold_calib_dijet_jerup_reweighted_1->SetBinError(ib, h_unfold_calib_dijet_jerup_reweighted_1->GetBinError(ib) / mbd_efficiency);

    h_unfold_calib_frac_reweighted_1->SetBinContent(ib, h_unfold_calib_frac_reweighted_1->GetBinContent(ib) / mbd_efficiency); h_unfold_calib_frac_reweighted_1->SetBinError(ib, h_unfold_calib_frac_reweighted_1->GetBinError(ib) / mbd_efficiency);
    h_unfold_calib_frac_effdown_reweighted_1->SetBinContent(ib, h_unfold_calib_frac_effdown_reweighted_1->GetBinContent(ib) / mbd_efficiency); h_unfold_calib_frac_effdown_reweighted_1->SetBinError(ib, h_unfold_calib_frac_effdown_reweighted_1->GetBinError(ib) / mbd_efficiency);
    h_unfold_calib_frac_effup_reweighted_1->SetBinContent(ib, h_unfold_calib_frac_effup_reweighted_1->GetBinContent(ib) / mbd_efficiency); h_unfold_calib_frac_effup_reweighted_1->SetBinError(ib, h_unfold_calib_frac_effup_reweighted_1->GetBinError(ib) / mbd_efficiency);
    h_unfold_calib_frac_jesdown_reweighted_1->SetBinContent(ib, h_unfold_calib_frac_jesdown_reweighted_1->GetBinContent(ib) / mbd_efficiency); h_unfold_calib_frac_jesdown_reweighted_1->SetBinError(ib, h_unfold_calib_frac_jesdown_reweighted_1->GetBinError(ib) / mbd_efficiency);
    h_unfold_calib_frac_jesup_reweighted_1->SetBinContent(ib, h_unfold_calib_frac_jesup_reweighted_1->GetBinContent(ib) / mbd_efficiency); h_unfold_calib_frac_jesup_reweighted_1->SetBinError(ib, h_unfold_calib_frac_jesup_reweighted_1->GetBinError(ib) / mbd_efficiency);
    h_unfold_calib_frac_jerdown_reweighted_1->SetBinContent(ib, h_unfold_calib_frac_jerdown_reweighted_1->GetBinContent(ib) / mbd_efficiency); h_unfold_calib_frac_jerdown_reweighted_1->SetBinError(ib, h_unfold_calib_frac_jerdown_reweighted_1->GetBinError(ib) / mbd_efficiency);
    h_unfold_calib_frac_jerup_reweighted_1->SetBinContent(ib, h_unfold_calib_frac_jerup_reweighted_1->GetBinContent(ib) / mbd_efficiency); h_unfold_calib_frac_jerup_reweighted_1->SetBinError(ib, h_unfold_calib_frac_jerup_reweighted_1->GetBinError(ib) / mbd_efficiency);

    h_unfold_calib_dijet_reweighted_2->SetBinContent(ib, h_unfold_calib_dijet_reweighted_2->GetBinContent(ib) / mbd_efficiency); h_unfold_calib_dijet_reweighted_2->SetBinError(ib, h_unfold_calib_dijet_reweighted_2->GetBinError(ib) / mbd_efficiency);
    h_unfold_calib_dijet_reweighted_3->SetBinContent(ib, h_unfold_calib_dijet_reweighted_3->GetBinContent(ib) / mbd_efficiency); h_unfold_calib_dijet_reweighted_3->SetBinError(ib, h_unfold_calib_dijet_reweighted_3->GetBinError(ib) / mbd_efficiency);
    h_unfold_calib_frac_reweighted_2->SetBinContent(ib, h_unfold_calib_frac_reweighted_2->GetBinContent(ib) / mbd_efficiency); h_unfold_calib_frac_reweighted_2->SetBinError(ib, h_unfold_calib_frac_reweighted_2->GetBinError(ib) / mbd_efficiency);
    h_unfold_calib_frac_reweighted_3->SetBinContent(ib, h_unfold_calib_frac_reweighted_3->GetBinContent(ib) / mbd_efficiency); h_unfold_calib_frac_reweighted_3->SetBinError(ib, h_unfold_calib_frac_reweighted_3->GetBinError(ib) / mbd_efficiency);

    h_unfold_calib_dijet_1->SetBinContent(ib, h_unfold_calib_dijet_1->GetBinContent(ib) / mbd_efficiency); h_unfold_calib_dijet_1->SetBinError(ib, h_unfold_calib_dijet_1->GetBinError(ib) / mbd_efficiency);
    h_unfold_calib_frac_1->SetBinContent(ib, h_unfold_calib_frac_1->GetBinContent(ib) / mbd_efficiency); h_unfold_calib_frac_1->SetBinError(ib, h_unfold_calib_frac_1->GetBinError(ib) / mbd_efficiency);

    double mbd_efficiency_down = h_mbd_efficiency_down->GetBinContent(ib - n_underflow_bin) / 0.57 * (26.1 / 42.);
    h_unfold_calib_dijet_mbddown_reweighted_1->SetBinContent(ib, h_unfold_calib_dijet_mbddown_reweighted_1->GetBinContent(ib) / mbd_efficiency_down); h_unfold_calib_dijet_mbddown_reweighted_1->SetBinError(ib, h_unfold_calib_dijet_mbddown_reweighted_1->GetBinError(ib) / mbd_efficiency_down);
    h_unfold_calib_frac_mbddown_reweighted_1->SetBinContent(ib, h_unfold_calib_frac_mbddown_reweighted_1->GetBinContent(ib) / mbd_efficiency_down); h_unfold_calib_frac_mbddown_reweighted_1->SetBinError(ib, h_unfold_calib_frac_mbddown_reweighted_1->GetBinError(ib) / mbd_efficiency_down);
    double mbd_efficiency_up = h_mbd_efficiency_up->GetBinContent(ib - n_underflow_bin) / 0.57 * (26.1 / 42.);
    h_unfold_calib_dijet_mbdup_reweighted_1->SetBinContent(ib, h_unfold_calib_dijet_mbdup_reweighted_1->GetBinContent(ib) / mbd_efficiency_up); h_unfold_calib_dijet_mbdup_reweighted_1->SetBinError(ib, h_unfold_calib_dijet_mbdup_reweighted_1->GetBinError(ib) / mbd_efficiency_up);
    h_unfold_calib_frac_mbdup_reweighted_1->SetBinContent(ib, h_unfold_calib_frac_mbdup_reweighted_1->GetBinContent(ib) / mbd_efficiency_up); h_unfold_calib_frac_mbdup_reweighted_1->SetBinError(ib, h_unfold_calib_frac_mbdup_reweighted_1->GetBinError(ib) / mbd_efficiency_up);
  }
 
  //********** Writing **********//
  std::cout << "Writing histograms..." << std::endl;
  f_out->cd();
  h_unfold_calib_dijet_reweighted_1->Write();
  h_unfold_calib_dijet_effdown_reweighted_1->Write();
  h_unfold_calib_dijet_effup_reweighted_1->Write();
  h_unfold_calib_dijet_jesdown_reweighted_1->Write();
  h_unfold_calib_dijet_jesup_reweighted_1->Write();
  h_unfold_calib_dijet_jerdown_reweighted_1->Write();
  h_unfold_calib_dijet_jerup_reweighted_1->Write();

  h_unfold_calib_frac_reweighted_1->Write();
  h_unfold_calib_frac_effdown_reweighted_1->Write();
  h_unfold_calib_frac_effup_reweighted_1->Write();
  h_unfold_calib_frac_jesdown_reweighted_1->Write();
  h_unfold_calib_frac_jesup_reweighted_1->Write();
  h_unfold_calib_frac_jerdown_reweighted_1->Write();
  h_unfold_calib_frac_jerup_reweighted_1->Write();

  h_unfold_calib_dijet_reweighted_2->Write();
  h_unfold_calib_dijet_reweighted_3->Write();
  h_unfold_calib_dijet_mbddown_reweighted_1->Write();
  h_unfold_calib_dijet_mbdup_reweighted_1->Write();
  h_unfold_calib_frac_reweighted_2->Write();
  h_unfold_calib_frac_reweighted_3->Write();
  h_unfold_calib_frac_mbddown_reweighted_1->Write();
  h_unfold_calib_frac_mbdup_reweighted_1->Write();

  h_unfold_calib_dijet_1->Write();
  h_unfold_calib_frac_1->Write();
  f_out->Close();
  std::cout << "All done!" << std::endl;
}
