#include <iostream>
#include <TFile.h>
#include <TH1D.h>
#include "/sphenix/user/hanpuj/CaloDataAna24_skimmed/src/draw_template.C"
#include "unfold_Def.h"

void get_purityefficiency_hist(TH1D *&h_purity, TH1D *&h_efficiency, string surfix, TFile *f_in, TFile *f_out, int jet_radius_index) {
  // Get histograms from input file
  TH1D* h_truth = (TH1D*)f_in->Get(("h_truth"+surfix).c_str());
  TH1D* h_measure = (TH1D*)f_in->Get(("h_measure"+surfix).c_str());
  TH2D* h_respmatrix = (TH2D*)f_in->Get(("h_respmatrix"+surfix).c_str());
  TH1D* h_fake = (TH1D*)f_in->Get(("h_fake"+surfix).c_str());
  TH1D* h_miss = (TH1D*)f_in->Get(("h_miss"+surfix).c_str());
  if (!h_truth || !h_measure || !h_respmatrix || !h_fake || !h_miss) {
    std::cerr << "Error: One or more histograms not found in input file for surfix: " << surfix << std::endl;
    return;
  }
  // Get purity and efficiency histograms
  h_purity = (TH1D*)h_respmatrix->ProjectionX(("h_purity"+surfix).c_str());
  h_purity->Divide(h_purity, h_measure, 1, 1, "B");
  h_efficiency = (TH1D*)h_respmatrix->ProjectionY(("h_efficiency"+surfix).c_str());
  h_efficiency->Divide(h_efficiency, h_truth, 1, 1, "B");
  // Check purity and efficiency
  TH1D* h_purity_check = (TH1D*)h_respmatrix->ProjectionX(("h_purity"+surfix+"_check").c_str());
  h_purity_check->Add(h_fake);
  TH1D* h_efficiency_check = (TH1D*)h_respmatrix->ProjectionY(("h_efficiency"+surfix+"_check").c_str());
  h_efficiency_check->Add(h_miss);
  for (int i = 1; i <= h_purity_check->GetNbinsX(); i++) {
    float purity_check = h_purity_check->GetBinContent(i) / (double)h_measure->GetBinContent(i);
    if (purity_check != 1.0) std::cout << "Case: " << surfix << " in purity check failed at bin " << i << " with ratio = " << purity_check << " with purity content = " << h_purity_check->GetBinContent(i) << " and measurement content = " << h_measure->GetBinContent(i) << std::endl;
  }
  for (int i = 1; i <= h_efficiency_check->GetNbinsX(); i++) {
    float efficiency_check = h_efficiency_check->GetBinContent(i) / (double)h_truth->GetBinContent(i);
    if (efficiency_check != 1.0) std::cout << "Case: " << surfix << " in efficiency check failed at bin " << i << " with ratio = " << efficiency_check << " with purity content = " << h_efficiency_check->GetBinContent(i) << " and measurement content = " << h_truth->GetBinContent(i) << std::endl;
  }
  // Write histograms to output file
  f_out->cd();
  h_purity->Write();
  h_efficiency->Write();

  TH1D* h_purity_proj = (TH1D*)h_respmatrix->ProjectionX("h_purity_proj");
  TH1D* h_efficiency_proj = (TH1D*)h_respmatrix->ProjectionY("h_efficiency_proj");
  std::vector<TH1F*> h_input;
  std::vector<int> color;
  std::vector<int> markerstyle;
  std::vector<std::string> text;
  std::vector<std::string> legend;

  h_input.push_back((TH1F*)h_measure);
  h_input.push_back((TH1F*)h_purity_proj);
  color.push_back(kBlack);
  color.push_back(kRed);
  markerstyle.push_back(20);
  markerstyle.push_back(20);
  text.push_back("#bf{#it{sPHENIX}} Simulation");
  text.push_back("PYTHIA8 p+p#sqrt{s} = 200 GeV");
  text.push_back(("anti-k_{t} #kern[-0.5]{#it{R}} = 0." + std::to_string(jet_radius_index)).c_str());
  text.push_back(("|#eta^{jet}| < 0." + std::to_string((int)(11-jet_radius_index))).c_str());
  text.push_back("No z-vertex cut");
  legend.push_back("Reco jet spectrum");
  legend.push_back("Matched reco jet spectrum");
  draw_1D_multiple_plot_ratio(h_input, color, markerstyle,
                              false, 10, true,
                              true, 15, 75, false,
                              false, 0, 0.5, true,
                              false, 0.75, 1.1,
                              true, "p_{T}^{reco jet} [GeV]", "Arbitrary Unit", "Purity", 1,
                              true, text, 0.45, 0.9, 0.05,
                              true, legend, 0.25, 0.2, 0.05,
                              Form("figure_purityefficiency/purity%s_r0%d.png", surfix.c_str(), jet_radius_index));
  h_input.clear();
  legend.clear();

  h_input.push_back((TH1F*)h_truth);
  h_input.push_back((TH1F*)h_efficiency_proj);
  legend.push_back("Truth jet spectrum");
  legend.push_back("Matched truth jet spectrum");
  draw_1D_multiple_plot_ratio(h_input, color, markerstyle,
                              false, 10, true,
                              true, 15, 75, false,
                              false, 1e-9, 1e-1, true,
                              false, 0., 1.2,
                              true, "p_{T}^{truth jet} [GeV]", "Arbitrary Unit", "Efficiency", 1,
                              true, text, 0.45, 0.9, 0.05,
                              true, legend, 0.25, 0.2, 0.05,
                              Form("figure_purityefficiency/efficiency%s_r0%d.png", surfix.c_str(), jet_radius_index));
  h_input.clear();
  color.clear();
  markerstyle.clear();
  text.clear();
  legend.clear();
}

void get_purityefficiency_clonehist(TH1D *&h_purity, TH1D *&h_efficiency, string surfix, TH1D *h_purity_forclone, TH1D *h_efficiency_forclone, TFile *f_out) {
  h_purity = dynamic_cast<TH1D*>(h_purity_forclone->Clone(("h_purity"+surfix).c_str()));
  h_efficiency = dynamic_cast<TH1D*>((TH1D*)h_efficiency_forclone->Clone(("h_efficiency"+surfix).c_str()));
  f_out->cd();
  h_purity->Write();
  h_efficiency->Write();
}

void do_puritycorr(TH1D* h_result, TH1D* h_purity, string result_surfix, string data_surfix, TFile* f_data, TFile* f_dataout) {
  TH1D* h_calibjet_pt = (TH1D*)f_data->Get(("h_calibjet_pt_scaled" + data_surfix).c_str());
  h_result = (TH1D*)h_calibjet_pt->Clone(("h_calibjet_pt_puritycorr" + result_surfix).c_str());
  h_result->Multiply(h_purity);
  f_dataout->cd();
  h_result->Write();
}


void get_purityefficiency(int radius_index = 4) {
  // Read Files
  TFile *f_in = new TFile(Form("output_sim_r0%d.root", radius_index), "READ");
  if (!f_in) {
    std::cout << "Error: cannot open output_sim_r0" << radius_index << ".root" << std::endl;
    return;
  }
  TFile *f_out = new TFile(Form("output_purityefficiency_r0%d.root", radius_index), "RECREATE");
  TFile* f_data = new TFile(Form("output_data_r0%d.root", radius_index), "READ");
  TFile* f_dataout = new TFile(Form("output_data_puritycorr_r0%d.root", radius_index), "RECREATE");

  // Get purity and efficiency histograms
  TH1D* h_purity_nominal, *h_efficiency_nominal; get_purityefficiency_hist(h_purity_nominal, h_efficiency_nominal, "_nominal", f_in, f_out, radius_index);
  TH1D* h_purity_jesup, *h_efficiency_jesup; get_purityefficiency_hist(h_purity_jesup, h_efficiency_jesup, "_jesup", f_in, f_out, radius_index);
  TH1D* h_purity_jesdown, *h_efficiency_jesdown; get_purityefficiency_hist(h_purity_jesdown, h_efficiency_jesdown, "_jesdown", f_in, f_out, radius_index);
  TH1D* h_purity_jerup, *h_efficiency_jerup; get_purityefficiency_hist(h_purity_jerup, h_efficiency_jerup, "_jerup", f_in, f_out, radius_index);
  TH1D* h_purity_jerdown, *h_efficiency_jerdown; get_purityefficiency_hist(h_purity_jerdown, h_efficiency_jerdown, "_jerdown", f_in, f_out, radius_index);
  TH1D* h_purity_jettrigup, *h_efficiency_jettrigup; get_purityefficiency_clonehist(h_purity_jettrigup, h_efficiency_jettrigup, "_jettrigup", h_purity_nominal, h_efficiency_nominal, f_out);
  TH1D* h_purity_jettrigdown, *h_efficiency_jettrigdown; get_purityefficiency_clonehist(h_purity_jettrigdown, h_efficiency_jettrigdown, "_jettrigdown", h_purity_nominal, h_efficiency_nominal, f_out);
  TH1D* h_purity_jettimingup, *h_efficiency_jettimingup; get_purityefficiency_clonehist(h_purity_jettimingup, h_efficiency_jettimingup, "_jettimingup", h_purity_nominal, h_efficiency_nominal, f_out);
  TH1D* h_purity_jettimingdown, *h_efficiency_jettimingdown; get_purityefficiency_clonehist(h_purity_jettimingdown, h_efficiency_jettimingdown, "_jettimingdown", h_purity_nominal, h_efficiency_nominal, f_out);

  // Do purity correction
  TH1D* h_calibjet_pt_puritycorr_nominal; do_puritycorr(h_calibjet_pt_puritycorr_nominal, h_purity_nominal, "_nominal", "_nominal", f_data, f_dataout);
  TH1D* h_calibjet_pt_puritycorr_jesup; do_puritycorr(h_calibjet_pt_puritycorr_jesup, h_purity_jesup, "_jesup", "_nominal", f_data, f_dataout);
  TH1D* h_calibjet_pt_puritycorr_jesdown; do_puritycorr(h_calibjet_pt_puritycorr_jesdown, h_purity_jesdown, "_jesdown", "_nominal", f_data, f_dataout);
  TH1D* h_calibjet_pt_puritycorr_jerup; do_puritycorr(h_calibjet_pt_puritycorr_jerup, h_purity_jerup, "_jerup", "_nominal", f_data, f_dataout);
  TH1D* h_calibjet_pt_puritycorr_jerdown; do_puritycorr(h_calibjet_pt_puritycorr_jerdown, h_purity_jerdown, "_jerdown", "_nominal", f_data, f_dataout);
  TH1D* h_calibjet_pt_puritycorr_jettrigup; do_puritycorr(h_calibjet_pt_puritycorr_jettrigup, h_purity_jettrigup, "_jettrigup", "_jettrigup", f_data, f_dataout);
  TH1D* h_calibjet_pt_puritycorr_jettrigdown; do_puritycorr(h_calibjet_pt_puritycorr_jettrigdown, h_purity_jettrigdown, "_jettrigdown", "_jettrigdown", f_data, f_dataout);
  TH1D* h_calibjet_pt_puritycorr_jettimingup; do_puritycorr(h_calibjet_pt_puritycorr_jettimingup, h_purity_jettimingup, "_jettimingup", "_jettimingup", f_data, f_dataout);
  TH1D* h_calibjet_pt_puritycorr_jettimingdown; do_puritycorr(h_calibjet_pt_puritycorr_jettimingdown, h_purity_jettimingdown, "_jettimingdown", "_jettimingdown", f_data, f_dataout);
}