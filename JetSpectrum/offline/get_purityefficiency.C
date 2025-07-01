#include <iostream>
#include <TFile.h>
#include <TH1D.h>
#include "/sphenix/user/hanpuj/CaloDataAna24_skimmed/src/draw_template.C"
#include "unfold_Def.h"

void get_purityefficiency() {
  // Read Files
  TFile *f_in = new TFile("output_sim.root", "READ");
  if (!f_in) {
    std::cout << "Error: cannot open output_sim.root" << std::endl;
    return;
  }

  // Read histograms
  TH1D* h_truth_calib_dijet = (TH1D*)f_in->Get("h_truth_calib_dijet");
  TH1D* h_measure_calib_dijet = (TH1D*)f_in->Get("h_measure_calib_dijet");
  TH2D* h_respmatrix_calib_dijet = (TH2D*)f_in->Get("h_respmatrix_calib_dijet");

  TH1D* h_truth_calib_dijet_jesdown = (TH1D*)f_in->Get("h_truth_calib_dijet_jesdown");
  TH1D* h_measure_calib_dijet_jesdown = (TH1D*)f_in->Get("h_measure_calib_dijet_jesdown");
  TH2D* h_respmatrix_calib_dijet_jesdown = (TH2D*)f_in->Get("h_respmatrix_calib_dijet_jesdown");

  TH1D* h_truth_calib_dijet_jesup = (TH1D*)f_in->Get("h_truth_calib_dijet_jesup");
  TH1D* h_measure_calib_dijet_jesup = (TH1D*)f_in->Get("h_measure_calib_dijet_jesup");
  TH2D* h_respmatrix_calib_dijet_jesup = (TH2D*)f_in->Get("h_respmatrix_calib_dijet_jesup");

  TH1D* h_truth_calib_dijet_jerdown = (TH1D*)f_in->Get("h_truth_calib_dijet_jerdown");
  TH1D* h_measure_calib_dijet_jerdown = (TH1D*)f_in->Get("h_measure_calib_dijet_jerdown");
  TH2D* h_respmatrix_calib_dijet_jerdown = (TH2D*)f_in->Get("h_respmatrix_calib_dijet_jerdown");

  TH1D* h_truth_calib_dijet_jerup = (TH1D*)f_in->Get("h_truth_calib_dijet_jerup");
  TH1D* h_measure_calib_dijet_jerup = (TH1D*)f_in->Get("h_measure_calib_dijet_jerup");
  TH2D* h_respmatrix_calib_dijet_jerup = (TH2D*)f_in->Get("h_respmatrix_calib_dijet_jerup");
  
  TH1D* h_truth_calib_frac = (TH1D*)f_in->Get("h_truth_calib_frac");
  TH1D* h_measure_calib_frac = (TH1D*)f_in->Get("h_measure_calib_frac");
  TH2D* h_respmatrix_calib_frac = (TH2D*)f_in->Get("h_respmatrix_calib_frac");

  TH1D* h_truth_calib_frac_jesdown = (TH1D*)f_in->Get("h_truth_calib_frac_jesdown");
  TH1D* h_measure_calib_frac_jesdown = (TH1D*)f_in->Get("h_measure_calib_frac_jesdown");
  TH2D* h_respmatrix_calib_frac_jesdown = (TH2D*)f_in->Get("h_respmatrix_calib_frac_jesdown");

  TH1D* h_truth_calib_frac_jesup = (TH1D*)f_in->Get("h_truth_calib_frac_jesup");
  TH1D* h_measure_calib_frac_jesup = (TH1D*)f_in->Get("h_measure_calib_frac_jesup");
  TH2D* h_respmatrix_calib_frac_jesup = (TH2D*)f_in->Get("h_respmatrix_calib_frac_jesup");

  TH1D* h_truth_calib_frac_jerdown = (TH1D*)f_in->Get("h_truth_calib_frac_jerdown");
  TH1D* h_measure_calib_frac_jerdown = (TH1D*)f_in->Get("h_measure_calib_frac_jerdown");
  TH2D* h_respmatrix_calib_frac_jerdown = (TH2D*)f_in->Get("h_respmatrix_calib_frac_jerdown");

  TH1D* h_truth_calib_frac_jerup = (TH1D*)f_in->Get("h_truth_calib_frac_jerup");
  TH1D* h_measure_calib_frac_jerup = (TH1D*)f_in->Get("h_measure_calib_frac_jerup");
  TH2D* h_respmatrix_calib_frac_jerup = (TH2D*)f_in->Get("h_respmatrix_calib_frac_jerup");

  //TH1D* h_truth_calib_frac_half1 = (TH1D*)f_in->Get("h_truth_calib_frac_half1");
  //TH1D* h_measure_calib_frac_half1 = (TH1D*)f_in->Get("h_measure_calib_frac_half1");
  //TH2D* h_respmatrix_calib_frac_half1 = (TH2D*)f_in->Get("h_respmatrix_calib_frac_half1");

  // Form purity and efficiency histograms
  TH1D* h_purity_calib_dijet = (TH1D*)h_respmatrix_calib_dijet->ProjectionX("h_purity_calib_dijet");
  TH1D* h_efficiency_calib_dijet = (TH1D*)h_respmatrix_calib_dijet->ProjectionY("h_efficiency_calib_dijet");

  TH1D* h_purity_calib_dijet_jesdown = (TH1D*)h_respmatrix_calib_dijet_jesdown->ProjectionX("h_purity_calib_dijet_jesdown");
  TH1D* h_efficiency_calib_dijet_jesdown = (TH1D*)h_respmatrix_calib_dijet_jesdown->ProjectionY("h_efficiency_calib_dijet_jesdown");

  TH1D* h_purity_calib_dijet_jesup = (TH1D*)h_respmatrix_calib_dijet_jesup->ProjectionX("h_purity_calib_dijet_jesup");
  TH1D* h_efficiency_calib_dijet_jesup = (TH1D*)h_respmatrix_calib_dijet_jesup->ProjectionY("h_efficiency_calib_dijet_jesup");

  TH1D* h_purity_calib_dijet_jerdown = (TH1D*)h_respmatrix_calib_dijet_jerdown->ProjectionX("h_purity_calib_dijet_jerdown");
  TH1D* h_efficiency_calib_dijet_jerdown = (TH1D*)h_respmatrix_calib_dijet_jerdown->ProjectionY("h_efficiency_calib_dijet_jerdown");

  TH1D* h_purity_calib_dijet_jerup = (TH1D*)h_respmatrix_calib_dijet_jerup->ProjectionX("h_purity_calib_dijet_jerup");
  TH1D* h_efficiency_calib_dijet_jerup = (TH1D*)h_respmatrix_calib_dijet_jerup->ProjectionY("h_efficiency_calib_dijet_jerup");

  TH1D* h_purity_calib_frac = (TH1D*)h_respmatrix_calib_frac->ProjectionX("h_purity_calib_frac");
  TH1D* h_efficiency_calib_frac = (TH1D*)h_respmatrix_calib_frac->ProjectionY("h_efficiency_calib_frac");

  TH1D* h_purity_calib_frac_jesdown = (TH1D*)h_respmatrix_calib_frac_jesdown->ProjectionX("h_purity_calib_frac_jesdown");
  TH1D* h_efficiency_calib_frac_jesdown = (TH1D*)h_respmatrix_calib_frac_jesdown->ProjectionY("h_efficiency_calib_frac_jesdown");

  TH1D* h_purity_calib_frac_jesup = (TH1D*)h_respmatrix_calib_frac_jesup->ProjectionX("h_purity_calib_frac_jesup");
  TH1D* h_efficiency_calib_frac_jesup = (TH1D*)h_respmatrix_calib_frac_jesup->ProjectionY("h_efficiency_calib_frac_jesup");

  TH1D* h_purity_calib_frac_jerdown = (TH1D*)h_respmatrix_calib_frac_jerdown->ProjectionX("h_purity_calib_frac_jerdown");
  TH1D* h_efficiency_calib_frac_jerdown = (TH1D*)h_respmatrix_calib_frac_jerdown->ProjectionY("h_efficiency_calib_frac_jerdown");

  TH1D* h_purity_calib_frac_jerup = (TH1D*)h_respmatrix_calib_frac_jerup->ProjectionX("h_purity_calib_frac_jerup");
  TH1D* h_efficiency_calib_frac_jerup = (TH1D*)h_respmatrix_calib_frac_jerup->ProjectionY("h_efficiency_calib_frac_jerup");

  //TH1D* h_purity_calib_frac_half1 = (TH1D*)h_respmatrix_calib_frac_half1->ProjectionX("h_purity_calib_frac_half1");
  //TH1D* h_efficiency_calib_frac_half1 = (TH1D*)h_respmatrix_calib_frac_half1->ProjectionY("h_efficiency_calib_frac_half1");

  // Calculate purity and efficiency
  h_purity_calib_dijet->Divide(h_purity_calib_dijet, h_measure_calib_dijet, 1, 1, "B");
  h_efficiency_calib_dijet->Divide(h_efficiency_calib_dijet, h_truth_calib_dijet, 1, 1, "B");

  h_purity_calib_dijet_jesdown->Divide(h_purity_calib_dijet_jesdown, h_measure_calib_dijet_jesdown, 1, 1, "B");
  h_efficiency_calib_dijet_jesdown->Divide(h_efficiency_calib_dijet_jesdown, h_truth_calib_dijet_jesdown, 1, 1, "B");

  h_purity_calib_dijet_jesup->Divide(h_purity_calib_dijet_jesup, h_measure_calib_dijet_jesup, 1, 1, "B");
  h_efficiency_calib_dijet_jesup->Divide(h_efficiency_calib_dijet_jesup, h_truth_calib_dijet_jesup, 1, 1, "B");

  h_purity_calib_dijet_jerdown->Divide(h_purity_calib_dijet_jerdown, h_measure_calib_dijet_jerdown, 1, 1, "B");
  h_efficiency_calib_dijet_jerdown->Divide(h_efficiency_calib_dijet_jerdown, h_truth_calib_dijet_jerdown, 1, 1, "B");

  h_purity_calib_dijet_jerup->Divide(h_purity_calib_dijet_jerup, h_measure_calib_dijet_jerup, 1, 1, "B");
  h_efficiency_calib_dijet_jerup->Divide(h_efficiency_calib_dijet_jerup, h_truth_calib_dijet_jerup, 1, 1, "B");

  h_purity_calib_frac->Divide(h_purity_calib_frac, h_measure_calib_frac, 1, 1, "B");
  h_efficiency_calib_frac->Divide(h_efficiency_calib_frac, h_truth_calib_frac, 1, 1, "B");

  h_purity_calib_frac_jesdown->Divide(h_purity_calib_frac_jesdown, h_measure_calib_frac_jesdown, 1, 1, "B");
  h_efficiency_calib_frac_jesdown->Divide(h_efficiency_calib_frac_jesdown, h_truth_calib_frac_jesdown, 1, 1, "B");

  h_purity_calib_frac_jesup->Divide(h_purity_calib_frac_jesup, h_measure_calib_frac_jesup, 1, 1, "B");
  h_efficiency_calib_frac_jesup->Divide(h_efficiency_calib_frac_jesup, h_truth_calib_frac_jesup, 1, 1, "B");

  h_purity_calib_frac_jerdown->Divide(h_purity_calib_frac_jerdown, h_measure_calib_frac_jerdown, 1, 1, "B");
  h_efficiency_calib_frac_jerdown->Divide(h_efficiency_calib_frac_jerdown, h_truth_calib_frac_jerdown, 1, 1, "B");

  h_purity_calib_frac_jerup->Divide(h_purity_calib_frac_jerup, h_measure_calib_frac_jerup, 1, 1, "B");
  h_efficiency_calib_frac_jerup->Divide(h_efficiency_calib_frac_jerup, h_truth_calib_frac_jerup, 1, 1, "B");

  //h_purity_calib_frac_half1->Divide(h_purity_calib_frac_half1, h_measure_calib_frac_half1, 1, 1, "B");
  //h_efficiency_calib_frac_half1->Divide(h_efficiency_calib_frac_half1, h_truth_calib_frac_half1, 1, 1, "B");

  // Write histograms
  TFile *f_out = new TFile("output_purityefficiency.root", "RECREATE");
  f_out->cd();
  h_purity_calib_dijet->Write();
  h_efficiency_calib_dijet->Write();
  
  h_purity_calib_dijet_jesdown->Write();
  h_efficiency_calib_dijet_jesdown->Write();

  h_purity_calib_dijet_jesup->Write();
  h_efficiency_calib_dijet_jesup->Write();

  h_purity_calib_dijet_jerdown->Write();
  h_efficiency_calib_dijet_jerdown->Write();

  h_purity_calib_dijet_jerup->Write();
  h_efficiency_calib_dijet_jerup->Write();

  h_purity_calib_frac->Write();
  h_efficiency_calib_frac->Write();

  h_purity_calib_frac_jesdown->Write();
  h_efficiency_calib_frac_jesdown->Write();

  h_purity_calib_frac_jesup->Write();
  h_efficiency_calib_frac_jesup->Write();

  h_purity_calib_frac_jerdown->Write();
  h_efficiency_calib_frac_jerdown->Write();

  h_purity_calib_frac_jerup->Write();
  h_efficiency_calib_frac_jerup->Write();

  //h_purity_calib_frac_half1->Write();
  //h_efficiency_calib_frac_half1->Write();
  f_out->Close();

  // Apply purity correction.
  TFile* f_data = new TFile("output_data.root", "READ");
  TFile* f_dataout = new TFile("output_data_puritycorr.root", "RECREATE");

  // Read histograms
  TH1D* h_calibjet_pt_dijet_eff = (TH1D*)f_data->Get("h_calibjet_pt_dijet_eff");
  TH1D* h_calibjet_pt_dijet_effdown = (TH1D*)f_data->Get("h_calibjet_pt_dijet_effdown");
  TH1D* h_calibjet_pt_dijet_effup = (TH1D*)f_data->Get("h_calibjet_pt_dijet_effup");
  TH1D* h_calibjet_pt_frac_eff = (TH1D*)f_data->Get("h_calibjet_pt_frac_eff");
  TH1D* h_calibjet_pt_frac_effdown = (TH1D*)f_data->Get("h_calibjet_pt_frac_effdown");
  TH1D* h_calibjet_pt_frac_effup = (TH1D*)f_data->Get("h_calibjet_pt_frac_effup");

  // Histograms for purity-corrected jet pT.
  TH1D* h_calibjet_pt_puritycorr_dijet = (TH1D*)h_calibjet_pt_dijet_eff->Clone("h_calibjet_pt_puritycorr_dijet");
  TH1D* h_calibjet_pt_puritycorr_dijet_effdown = (TH1D*)h_calibjet_pt_dijet_effdown->Clone("h_calibjet_pt_puritycorr_dijet_effdown");
  TH1D* h_calibjet_pt_puritycorr_dijet_effup = (TH1D*)h_calibjet_pt_dijet_effup->Clone("h_calibjet_pt_puritycorr_dijet_effup");
  TH1D* h_calibjet_pt_puritycorr_dijet_jesdown = (TH1D*)h_calibjet_pt_dijet_eff->Clone("h_calibjet_pt_puritycorr_dijet_jesdown");
  TH1D* h_calibjet_pt_puritycorr_dijet_jesup = (TH1D*)h_calibjet_pt_dijet_eff->Clone("h_calibjet_pt_puritycorr_dijet_jesup");
  TH1D* h_calibjet_pt_puritycorr_dijet_jerdown = (TH1D*)h_calibjet_pt_dijet_eff->Clone("h_calibjet_pt_puritycorr_dijet_jerdown");
  TH1D* h_calibjet_pt_puritycorr_dijet_jerup = (TH1D*)h_calibjet_pt_dijet_eff->Clone("h_calibjet_pt_puritycorr_dijet_jerup");

  TH1D* h_calibjet_pt_puritycorr_frac = (TH1D*)h_calibjet_pt_frac_eff->Clone("h_calibjet_pt_puritycorr_frac");
  TH1D* h_calibjet_pt_puritycorr_frac_effdown = (TH1D*)h_calibjet_pt_frac_effdown->Clone("h_calibjet_pt_puritycorr_frac_effdown");
  TH1D* h_calibjet_pt_puritycorr_frac_effup = (TH1D*)h_calibjet_pt_frac_effup->Clone("h_calibjet_pt_puritycorr_frac_effup");
  TH1D* h_calibjet_pt_puritycorr_frac_jesdown = (TH1D*)h_calibjet_pt_frac_eff->Clone("h_calibjet_pt_puritycorr_frac_jesdown");
  TH1D* h_calibjet_pt_puritycorr_frac_jesup = (TH1D*)h_calibjet_pt_frac_eff->Clone("h_calibjet_pt_puritycorr_frac_jesup");
  TH1D* h_calibjet_pt_puritycorr_frac_jerdown = (TH1D*)h_calibjet_pt_frac_eff->Clone("h_calibjet_pt_puritycorr_frac_jerdown");
  TH1D* h_calibjet_pt_puritycorr_frac_jerup = (TH1D*)h_calibjet_pt_frac_eff->Clone("h_calibjet_pt_puritycorr_frac_jerup");

  h_calibjet_pt_puritycorr_dijet->Divide(h_purity_calib_dijet);
  h_calibjet_pt_puritycorr_dijet_effdown->Divide(h_purity_calib_dijet);
  h_calibjet_pt_puritycorr_dijet_effup->Divide(h_purity_calib_dijet);
  h_calibjet_pt_puritycorr_dijet_jesdown->Divide(h_purity_calib_dijet_jesdown);
  h_calibjet_pt_puritycorr_dijet_jesup->Divide(h_purity_calib_dijet_jesup);
  h_calibjet_pt_puritycorr_dijet_jerdown->Divide(h_purity_calib_dijet_jerdown);
  h_calibjet_pt_puritycorr_dijet_jerup->Divide(h_purity_calib_dijet_jerup);

  h_calibjet_pt_puritycorr_frac->Divide(h_purity_calib_frac);
  h_calibjet_pt_puritycorr_frac_effdown->Divide(h_purity_calib_frac);
  h_calibjet_pt_puritycorr_frac_effup->Divide(h_purity_calib_frac);
  h_calibjet_pt_puritycorr_frac_jesdown->Divide(h_purity_calib_frac_jesdown);
  h_calibjet_pt_puritycorr_frac_jesup->Divide(h_purity_calib_frac_jesup);
  h_calibjet_pt_puritycorr_frac_jerdown->Divide(h_purity_calib_frac_jerdown);
  h_calibjet_pt_puritycorr_frac_jerup->Divide(h_purity_calib_frac_jerup);

  // Write histograms
  f_dataout->cd();
  h_calibjet_pt_puritycorr_dijet->Write();
  h_calibjet_pt_puritycorr_dijet_effdown->Write();
  h_calibjet_pt_puritycorr_dijet_effup->Write();
  h_calibjet_pt_puritycorr_dijet_jesdown->Write();
  h_calibjet_pt_puritycorr_dijet_jesup->Write();
  h_calibjet_pt_puritycorr_dijet_jerdown->Write();
  h_calibjet_pt_puritycorr_dijet_jerup->Write();

  h_calibjet_pt_puritycorr_frac->Write();
  h_calibjet_pt_puritycorr_frac_effdown->Write();
  h_calibjet_pt_puritycorr_frac_effup->Write();
  h_calibjet_pt_puritycorr_frac_jesdown->Write();
  h_calibjet_pt_puritycorr_frac_jesup->Write();
  h_calibjet_pt_puritycorr_frac_jerdown->Write();
  h_calibjet_pt_puritycorr_frac_jerup->Write();
  f_dataout->Close();

  // Draw purity and efficiency.
  TH1D* h_purity_calib_dijet_proj = (TH1D*)h_respmatrix_calib_dijet->ProjectionX("h_purity_calib_dijet_proj");
  TH1D* h_efficiency_calib_dijet_proj = (TH1D*)h_respmatrix_calib_dijet->ProjectionY("h_efficiency_calib_dijet_proj");

  TH1D* h_purity_calib_dijet_jesdown_proj = (TH1D*)h_respmatrix_calib_dijet_jesdown->ProjectionX("h_purity_calib_dijet_jesdown_proj");
  TH1D* h_efficiency_calib_dijet_jesdown_proj = (TH1D*)h_respmatrix_calib_dijet_jesdown->ProjectionY("h_efficiency_calib_dijet_jesdown_proj");

  TH1D* h_purity_calib_dijet_jesup_proj = (TH1D*)h_respmatrix_calib_dijet_jesup->ProjectionX("h_purity_calib_dijet_jesup_proj");
  TH1D* h_efficiency_calib_dijet_jesup_proj = (TH1D*)h_respmatrix_calib_dijet_jesup->ProjectionY("h_efficiency_calib_dijet_jesup_proj");

  TH1D* h_purity_calib_dijet_jerdown_proj = (TH1D*)h_respmatrix_calib_dijet_jerdown->ProjectionX("h_purity_calib_dijet_jerdown_proj");
  TH1D* h_efficiency_calib_dijet_jerdown_proj = (TH1D*)h_respmatrix_calib_dijet_jerdown->ProjectionY("h_efficiency_calib_dijet_jerdown_proj");

  TH1D* h_purity_calib_dijet_jerup_proj = (TH1D*)h_respmatrix_calib_dijet_jerup->ProjectionX("h_purity_calib_dijet_jerup_proj");
  TH1D* h_efficiency_calib_dijet_jerup_proj = (TH1D*)h_respmatrix_calib_dijet_jerup->ProjectionY("h_efficiency_calib_dijet_jerup_proj");

  TH1D* h_purity_calib_frac_proj = (TH1D*)h_respmatrix_calib_frac->ProjectionX("h_purity_calib_frac_proj");
  TH1D* h_efficiency_calib_frac_proj = (TH1D*)h_respmatrix_calib_frac->ProjectionY("h_efficiency_calib_frac_proj");

  TH1D* h_purity_calib_frac_jesdown_proj = (TH1D*)h_respmatrix_calib_frac_jesdown->ProjectionX("h_purity_calib_frac_jesdown_proj");
  TH1D* h_efficiency_calib_frac_jesdown_proj = (TH1D*)h_respmatrix_calib_frac_jesdown->ProjectionY("h_efficiency_calib_frac_jesdown_proj");

  TH1D* h_purity_calib_frac_jesup_proj = (TH1D*)h_respmatrix_calib_frac_jesup->ProjectionX("h_purity_calib_frac_jesup_proj");
  TH1D* h_efficiency_calib_frac_jesup_proj = (TH1D*)h_respmatrix_calib_frac_jesup->ProjectionY("h_efficiency_calib_frac_jesup_proj");

  TH1D* h_purity_calib_frac_jerdown_proj = (TH1D*)h_respmatrix_calib_frac_jerdown->ProjectionX("h_purity_calib_frac_jerdown_proj");
  TH1D* h_efficiency_calib_frac_jerdown_proj = (TH1D*)h_respmatrix_calib_frac_jerdown->ProjectionY("h_efficiency_calib_frac_jerdown_proj");

  TH1D* h_purity_calib_frac_jerup_proj = (TH1D*)h_respmatrix_calib_frac_jerup->ProjectionX("h_purity_calib_frac_jerup_proj");
  TH1D* h_efficiency_calib_frac_jerup_proj = (TH1D*)h_respmatrix_calib_frac_jerup->ProjectionY("h_efficiency_calib_frac_jerup_proj");

  std::vector<TH1F*> h_input;
  std::vector<int> color;
  std::vector<int> markerstyle;
  std::vector<std::string> text;
  std::vector<std::string> legend;

  h_input.push_back((TH1F*)h_measure_calib_dijet);
  h_input.push_back((TH1F*)h_purity_calib_dijet_proj);
  color.push_back(kBlack);
  color.push_back(kRed);
  markerstyle.push_back(20);
  markerstyle.push_back(20);
  text.push_back("#bf{#it{sPHENIX}} Simulation");
  text.push_back("PYTHIA8 p+p#sqrt{s} = 200 GeV");
  text.push_back("anti-k_{T}#it{R} = 0.4");
  text.push_back("|#eta^{jet}| < 0.7");
  text.push_back("#DeltaR_{matching} < 0.3");
  legend.push_back("Reco jet spectrum");
  legend.push_back("Matched reco jet spectrum");
  draw_1D_multiple_plot_ratio(h_input, color, markerstyle,
                              false, 10, true,
                              true, 15, 72, false,
                              false, 0, 0.5, true,
                              true, 0.75, 1.1,
                              true, "p_{T}^{reco jet} [GeV]", "Arbitrary Unit", "Purity", 1,
                              true, text, 0.55, 0.9, 0.04,
                              true, legend, 0.58, 0.65, 0.04,
                              "figure/purity_calib_dijet.pdf");
  h_input.clear();
  color.clear();
  markerstyle.clear();
  text.clear();
  legend.clear();

  h_input.push_back((TH1F*)h_measure_calib_dijet_jesdown);
  h_input.push_back((TH1F*)h_purity_calib_dijet_jesdown_proj);
  color.push_back(kBlack);
  color.push_back(kRed);
  markerstyle.push_back(20);
  markerstyle.push_back(20);
  text.push_back("#bf{#it{sPHENIX}} Simulation");
  text.push_back("PYTHIA8 p+p#sqrt{s} = 200 GeV");
  text.push_back("anti-k_{T}#it{R} = 0.4");
  text.push_back("|#eta^{jet}| < 0.7");
  text.push_back("#DeltaR_{matching} < 0.3");
  text.push_back("With JES calibration");
  legend.push_back("Reco jet spectrum");
  legend.push_back("Matched reco jet spectrum");
  draw_1D_multiple_plot_ratio(h_input, color, markerstyle,
                              false, 10, true,
                              true, 15, 72, false,
                              false, 0, 0.5, true,
                              true, 0.75, 1.1,
                              true, "p_{T}^{reco jet} [GeV]", "Arbitrary Unit", "Purity", 1,
                              true, text, 0.55, 0.9, 0.04,
                              true, legend, 0.58, 0.6, 0.04,
                              "figure/purity_calib_dijet_jesdown.pdf");
  h_input.clear();
  color.clear();
  markerstyle.clear();
  text.clear();
  legend.clear();

  h_input.push_back((TH1F*)h_measure_calib_dijet_jesup);
  h_input.push_back((TH1F*)h_purity_calib_dijet_jesup_proj);
  color.push_back(kBlack);
  color.push_back(kRed);
  markerstyle.push_back(20);
  markerstyle.push_back(20);
  text.push_back("#bf{#it{sPHENIX}} Simulation");
  text.push_back("PYTHIA8 p+p#sqrt{s} = 200 GeV");
  text.push_back("anti-k_{T}#it{R} = 0.4");
  text.push_back("|#eta^{jet}| < 0.7");
  text.push_back("#DeltaR_{matching} < 0.3");
  legend.push_back("Truth jet spectrum");
  legend.push_back("Matched truth jet spectrum");
  draw_1D_multiple_plot_ratio(h_input, color, markerstyle,
                              false, 10, true,
                              true, 15, 72, false,
                              false, 0, 0.5, true,
                              true, 0.75, 1.1,
                              true, "p_{T}^{truth jet} [GeV]", "Arbitrary Unit", "Purity", 1,
                              true, text, 0.55, 0.9, 0.04,
                              true, legend, 0.58, 0.65, 0.04,
                              "figure/purity_calib_dijet_jesup.pdf");
  h_input.clear();
  color.clear();
  markerstyle.clear();
  text.clear();
  legend.clear();

  h_input.push_back((TH1F*)h_measure_calib_dijet_jerdown);
  h_input.push_back((TH1F*)h_purity_calib_dijet_jerdown_proj);
  color.push_back(kBlack);
  color.push_back(kRed);
  markerstyle.push_back(20);
  markerstyle.push_back(20);
  text.push_back("#bf{#it{sPHENIX}} Simulation");
  text.push_back("PYTHIA8 p+p#sqrt{s} = 200 GeV");
  text.push_back("anti-k_{T}#it{R} = 0.4");
  text.push_back("|#eta^{jet}| < 0.7");
  text.push_back("#DeltaR_{matching} < 0.3");
  text.push_back("With JES calibration");
  legend.push_back("Truth jet spectrum");
  legend.push_back("Matched truth jet spectrum");
  draw_1D_multiple_plot_ratio(h_input, color, markerstyle,
                              false, 10, true,
                              true, 15, 72, false,
                              false, 0, 0.5, true,
                              true, 0.75, 1.1,
                              true, "p_{T}^{truth jet} [GeV]", "Arbitrary Unit", "Purity", 1,
                              true, text, 0.55, 0.9, 0.04,
                              true, legend, 0.58, 0.6, 0.04,
                              "figure/purity_calib_dijet_jerdown.pdf");
  h_input.clear();
  color.clear();
  markerstyle.clear();
  text.clear();
  legend.clear();

  h_input.push_back((TH1F*)h_measure_calib_dijet_jerup);
  h_input.push_back((TH1F*)h_purity_calib_dijet_jerup_proj);
  color.push_back(kBlack);
  color.push_back(kRed);
  markerstyle.push_back(20);
  markerstyle.push_back(20);
  text.push_back("#bf{#it{sPHENIX}} Simulation");
  text.push_back("PYTHIA8 p+p#sqrt{s} = 200 GeV");
  text.push_back("anti-k_{T}#it{R} = 0.4");
  text.push_back("|#eta^{jet}| < 0.7");
  text.push_back("#DeltaR_{matching} < 0.3");
  text.push_back("With JES calibration");
  legend.push_back("Truth jet spectrum");
  legend.push_back("Matched truth jet spectrum");
  draw_1D_multiple_plot_ratio(h_input, color, markerstyle,
                              false, 10, true,
                              true, 15, 72, false,
                              false, 0, 0.5, true,
                              true, 0.75, 1.1,
                              true, "p_{T}^{truth jet} [GeV]", "Arbitrary Unit", "Purity", 1,
                              true, text, 0.55, 0.9, 0.04,
                              true, legend, 0.58, 0.6, 0.04,
                              "figure/purity_calib_dijet_jerup.pdf");
  h_input.clear();
  color.clear();
  markerstyle.clear();
  text.clear();
  legend.clear();

  h_input.push_back((TH1F*)h_measure_calib_frac);
  h_input.push_back((TH1F*)h_purity_calib_frac_proj);
  color.push_back(kBlack);
  color.push_back(kRed);
  markerstyle.push_back(20);
  markerstyle.push_back(20);
  text.push_back("#bf{#it{sPHENIX}} Simulation");
  text.push_back("PYTHIA8 p+p#sqrt{s} = 200 GeV");
  text.push_back("anti-k_{T}#it{R} = 0.4");
  text.push_back("|#eta^{jet}| < 0.7");
  text.push_back("#DeltaR_{matching} < 0.3");
  legend.push_back("Reco jet spectrum");
  legend.push_back("Matched reco jet spectrum");
  draw_1D_multiple_plot_ratio(h_input, color, markerstyle,
                              false, 10, true,
                              true, 15, 72, false,
                              false, 0, 0.5, true,
                              true, 0.75, 1.1,
                              true, "p_{T}^{reco jet} [GeV]", "Arbitrary Unit", "Purity", 1,
                              true, text, 0.55, 0.9, 0.04,
                              true, legend, 0.58, 0.65, 0.04,
                              "figure/purity_calib_frac.pdf");
  h_input.clear();
  color.clear();
  markerstyle.clear();
  text.clear();
  legend.clear();

  h_input.push_back((TH1F*)h_measure_calib_frac_jesdown);
  h_input.push_back((TH1F*)h_purity_calib_frac_jesdown_proj);
  color.push_back(kBlack);
  color.push_back(kRed);
  markerstyle.push_back(20);
  markerstyle.push_back(20);
  text.push_back("#bf{#it{sPHENIX}} Simulation");
  text.push_back("PYTHIA8 p+p#sqrt{s} = 200 GeV");
  text.push_back("anti-k_{T}#it{R} = 0.4");
  text.push_back("|#eta^{jet}| < 0.7");
  text.push_back("#DeltaR_{matching} < 0.3");
  text.push_back("With JES calibration");
  legend.push_back("Reco jet spectrum");
  legend.push_back("Matched reco jet spectrum");
  draw_1D_multiple_plot_ratio(h_input, color, markerstyle,
                              false, 10, true,
                              true, 15, 72, false,
                              false, 0, 0.5, true,
                              true, 0.75, 1.1,
                              true, "p_{T}^{reco jet} [GeV]", "Arbitrary Unit", "Purity", 1,
                              true, text, 0.55, 0.9, 0.04,
                              true, legend, 0.58, 0.6, 0.04,
                              "figure/purity_calib_frac_jesdown.pdf");
  h_input.clear();
  color.clear();
  markerstyle.clear();
  text.clear();
  legend.clear();

  h_input.push_back((TH1F*)h_measure_calib_frac_jesup);
  h_input.push_back((TH1F*)h_purity_calib_frac_jesup_proj);
  color.push_back(kBlack);
  color.push_back(kRed);
  markerstyle.push_back(20);
  markerstyle.push_back(20);
  text.push_back("#bf{#it{sPHENIX}} Simulation");
  text.push_back("PYTHIA8 p+p#sqrt{s} = 200 GeV");
  text.push_back("anti-k_{T}#it{R} = 0.4");
  text.push_back("|#eta^{jet}| < 0.7");
  text.push_back("#DeltaR_{matching} < 0.3");
  legend.push_back("Truth jet spectrum");
  legend.push_back("Matched truth jet spectrum");
  draw_1D_multiple_plot_ratio(h_input, color, markerstyle,
                              false, 10, true,
                              true, 15, 72, false,
                              false, 0, 0.5, true,
                              true, 0.75, 1.1,
                              true, "p_{T}^{truth jet} [GeV]", "Arbitrary Unit", "Purity", 1,
                              true, text, 0.55, 0.9, 0.04,
                              true, legend, 0.58, 0.65, 0.04,
                              "figure/purity_calib_frac_jesup.pdf");
  h_input.clear();
  color.clear();
  markerstyle.clear();
  text.clear();
  legend.clear();

  h_input.push_back((TH1F*)h_measure_calib_frac_jerdown);
  h_input.push_back((TH1F*)h_purity_calib_frac_jerdown_proj);
  color.push_back(kBlack);
  color.push_back(kRed);
  markerstyle.push_back(20);
  markerstyle.push_back(20);
  text.push_back("#bf{#it{sPHENIX}} Simulation");
  text.push_back("PYTHIA8 p+p#sqrt{s} = 200 GeV");
  text.push_back("anti-k_{T}#it{R} = 0.4");
  text.push_back("|#eta^{jet}| < 0.7");
  text.push_back("#DeltaR_{matching} < 0.3");
  text.push_back("With JES calibration");
  legend.push_back("Truth jet spectrum");
  legend.push_back("Matched truth jet spectrum");
  draw_1D_multiple_plot_ratio(h_input, color, markerstyle,
                              false, 10, true,
                              true, 15, 72, false,
                              false, 0, 0.5, true,
                              true, 0.75, 1.1,
                              true, "p_{T}^{truth jet} [GeV]", "Arbitrary Unit", "Purity", 1,
                              true, text, 0.55, 0.9, 0.04,
                              true, legend, 0.58, 0.6, 0.04,
                              "figure/purity_calib_frac_jerdown.pdf");
  h_input.clear();
  color.clear();
  markerstyle.clear();
  text.clear();
  legend.clear();

  h_input.push_back((TH1F*)h_measure_calib_frac_jerup);
  h_input.push_back((TH1F*)h_purity_calib_frac_jerup_proj);
  color.push_back(kBlack);
  color.push_back(kRed);
  markerstyle.push_back(20);
  markerstyle.push_back(20);
  text.push_back("#bf{#it{sPHENIX}} Simulation");
  text.push_back("PYTHIA8 p+p#sqrt{s} = 200 GeV");
  text.push_back("anti-k_{T}#it{R} = 0.4");
  text.push_back("|#eta^{jet}| < 0.7");
  text.push_back("#DeltaR_{matching} < 0.3");
  text.push_back("With JES calibration");
  legend.push_back("Truth jet spectrum");
  legend.push_back("Matched truth jet spectrum");
  draw_1D_multiple_plot_ratio(h_input, color, markerstyle,
                              false, 10, true,
                              true, 15, 72, false,
                              false, 0, 0.5, true,
                              true, 0.75, 1.1,
                              true, "p_{T}^{truth jet} [GeV]", "Arbitrary Unit", "Purity", 1,
                              true, text, 0.55, 0.9, 0.04,
                              true, legend, 0.58, 0.6, 0.04,
                              "figure/purity_calib_frac_jerup.pdf");
  h_input.clear();
  color.clear();
  markerstyle.clear();
  text.clear();
  legend.clear();

  h_input.push_back((TH1F*)h_truth_calib_dijet);
  h_input.push_back((TH1F*)h_efficiency_calib_dijet_proj);
  color.push_back(kBlack);
  color.push_back(kRed);
  markerstyle.push_back(20);
  markerstyle.push_back(20);
  text.push_back("#bf{#it{sPHENIX}} Simulation");
  text.push_back("PYTHIA8 p+p#sqrt{s} = 200 GeV");
  text.push_back("anti-k_{T}#it{R} = 0.4");
  text.push_back("|#eta^{jet}| < 0.7");
  text.push_back("#DeltaR_{matching} < 0.3");
  legend.push_back("Reco jet spectrum");
  legend.push_back("Matched reco jet spectrum");
  draw_1D_multiple_plot_ratio(h_input, color, markerstyle,
                              false, 10, true,
                              true, 15, 72, false,
                              true, 1e-9, 1e-1, true,
                              true, 0., 1.2,
                              true, "p_{T}^{reco jet} [GeV]", "Arbitrary Unit", "Efficiency", 1,
                              true, text, 0.55, 0.9, 0.04,
                              true, legend, 0.58, 0.65, 0.04,
                              "figure/efficiency_calib_dijet.pdf");
  h_input.clear();
  color.clear();
  markerstyle.clear();
  text.clear();
  legend.clear();

  h_input.push_back((TH1F*)h_truth_calib_dijet_jesdown);
  h_input.push_back((TH1F*)h_efficiency_calib_dijet_jesdown_proj);
  color.push_back(kBlack);
  color.push_back(kRed);
  markerstyle.push_back(20);
  markerstyle.push_back(20);
  text.push_back("#bf{#it{sPHENIX}} Simulation");
  text.push_back("PYTHIA8 p+p#sqrt{s} = 200 GeV");
  text.push_back("anti-k_{T}#it{R} = 0.4");
  text.push_back("|#eta^{jet}| < 0.7");
  text.push_back("#DeltaR_{matching} < 0.3");
  text.push_back("With JES calibration");
  legend.push_back("Reco jet spectrum");
  legend.push_back("Matched reco jet spectrum");
  draw_1D_multiple_plot_ratio(h_input, color, markerstyle,
                              false, 10, true,
                              true, 15, 72, false,
                              true, 1e-9, 1e-1, true,
                              true, 0., 1.2,
                              true, "p_{T}^{reco jet} [GeV]", "Arbitrary Unit", "Efficiency", 1,
                              true, text, 0.55, 0.9, 0.04,
                              true, legend, 0.58, 0.6, 0.04,
                              "figure/efficiency_calib_dijet_jesdown.pdf");
  h_input.clear();
  color.clear();
  markerstyle.clear();
  text.clear();
  legend.clear();

  h_input.push_back((TH1F*)h_truth_calib_dijet_jesup);
  h_input.push_back((TH1F*)h_efficiency_calib_dijet_jesup_proj);
  color.push_back(kBlack);
  color.push_back(kRed);
  markerstyle.push_back(20);
  markerstyle.push_back(20);
  text.push_back("#bf{#it{sPHENIX}} Simulation");
  text.push_back("PYTHIA8 p+p#sqrt{s} = 200 GeV");
  text.push_back("anti-k_{T}#it{R} = 0.4");
  text.push_back("|#eta^{jet}| < 0.7");
  text.push_back("#DeltaR_{matching} < 0.3");
  legend.push_back("Truth jet spectrum");
  legend.push_back("Matched truth jet spectrum");
  draw_1D_multiple_plot_ratio(h_input, color, markerstyle,
                              false, 10, true,
                              true, 15, 72, false,
                              true, 1e-9, 1e-1, true,
                              true, 0, 1.2,
                              true, "p_{T}^{truth jet} [GeV]", "Arbitrary Unit", "Efficiency", 1,
                              true, text, 0.55, 0.9, 0.04,
                              true, legend, 0.58, 0.65, 0.04,
                              "figure/efficiency_calib_dijet_jesup.pdf");
  h_input.clear();
  color.clear();
  markerstyle.clear();
  text.clear();
  legend.clear();

  h_input.push_back((TH1F*)h_truth_calib_dijet_jerdown);
  h_input.push_back((TH1F*)h_efficiency_calib_dijet_jerdown_proj);
  color.push_back(kBlack);
  color.push_back(kRed);
  markerstyle.push_back(20);
  markerstyle.push_back(20);
  text.push_back("#bf{#it{sPHENIX}} Simulation");
  text.push_back("PYTHIA8 p+p#sqrt{s} = 200 GeV");
  text.push_back("anti-k_{T}#it{R} = 0.4");
  text.push_back("|#eta^{jet}| < 0.7");
  text.push_back("#DeltaR_{matching} < 0.3");
  text.push_back("With JES calibration");
  legend.push_back("Truth jet spectrum");
  legend.push_back("Matched truth jet spectrum");
  draw_1D_multiple_plot_ratio(h_input, color, markerstyle,
                              false, 10, true,
                              true, 15, 72, false,
                              true, 1e-9, 1e-1, true,
                              true, 0, 1.2,
                              true, "p_{T}^{truth jet} [GeV]", "Arbitrary Unit", "Efficiency", 1,
                              true, text, 0.55, 0.9, 0.04,
                              true, legend, 0.58, 0.6, 0.04,
                              "figure/efficiency_calib_dijet_jerdown.pdf");
  h_input.clear();
  color.clear();
  markerstyle.clear();
  text.clear();
  legend.clear();

  h_input.push_back((TH1F*)h_truth_calib_dijet_jerup);
  h_input.push_back((TH1F*)h_efficiency_calib_dijet_jerup_proj);
  color.push_back(kBlack);
  color.push_back(kRed);
  markerstyle.push_back(20);
  markerstyle.push_back(20);
  text.push_back("#bf{#it{sPHENIX}} Simulation");
  text.push_back("PYTHIA8 p+p#sqrt{s} = 200 GeV");
  text.push_back("anti-k_{T}#it{R} = 0.4");
  text.push_back("|#eta^{jet}| < 0.7");
  text.push_back("#DeltaR_{matching} < 0.3");
  text.push_back("With JES calibration");
  legend.push_back("Truth jet spectrum");
  legend.push_back("Matched truth jet spectrum");
  draw_1D_multiple_plot_ratio(h_input, color, markerstyle,
                              false, 10, true,
                              true, 15, 72, false,
                              true, 1e-9, 1e-1, true,
                              true, 0, 1.2,
                              true, "p_{T}^{truth jet} [GeV]", "Arbitrary Unit", "Efficiency", 1,
                              true, text, 0.55, 0.9, 0.04,
                              true, legend, 0.58, 0.6, 0.04,
                              "figure/efficiency_calib_dijet_jerup.pdf");
  h_input.clear();
  color.clear();
  markerstyle.clear();
  text.clear();
  legend.clear();

  h_input.push_back((TH1F*)h_truth_calib_frac);
  h_input.push_back((TH1F*)h_efficiency_calib_frac_proj);
  color.push_back(kBlack);
  color.push_back(kRed);
  markerstyle.push_back(20);
  markerstyle.push_back(20);
  text.push_back("#bf{#it{sPHENIX}} Simulation");
  text.push_back("PYTHIA8 p+p#sqrt{s} = 200 GeV");
  text.push_back("anti-k_{T}#it{R} = 0.4");
  text.push_back("|#eta^{jet}| < 0.7");
  text.push_back("#DeltaR_{matching} < 0.3");
  legend.push_back("Reco jet spectrum");
  legend.push_back("Matched reco jet spectrum");
  draw_1D_multiple_plot_ratio(h_input, color, markerstyle,
                              false, 10, true,
                              true, 15, 72, false,
                              true, 1e-9, 1e-1, true,
                              true, 0., 1.2,
                              true, "p_{T}^{reco jet} [GeV]", "Arbitrary Unit", "Efficiency", 1,
                              true, text, 0.55, 0.9, 0.04,
                              true, legend, 0.58, 0.65, 0.04,
                              "figure/efficiency_calib_frac.pdf");
  h_input.clear();
  color.clear();
  markerstyle.clear();
  text.clear();
  legend.clear();

  h_input.push_back((TH1F*)h_truth_calib_frac_jesdown);
  h_input.push_back((TH1F*)h_efficiency_calib_frac_jesdown_proj);
  color.push_back(kBlack);
  color.push_back(kRed);
  markerstyle.push_back(20);
  markerstyle.push_back(20);
  text.push_back("#bf{#it{sPHENIX}} Simulation");
  text.push_back("PYTHIA8 p+p#sqrt{s} = 200 GeV");
  text.push_back("anti-k_{T}#it{R} = 0.4");
  text.push_back("|#eta^{jet}| < 0.7");
  text.push_back("#DeltaR_{matching} < 0.3");
  text.push_back("With JES calibration");
  legend.push_back("Reco jet spectrum");
  legend.push_back("Matched reco jet spectrum");
  draw_1D_multiple_plot_ratio(h_input, color, markerstyle,
                              false, 10, true,
                              true, 15, 72, false,
                              true, 1e-9, 1e-1, true,
                              true, 0., 1.2,
                              true, "p_{T}^{reco jet} [GeV]", "Arbitrary Unit", "Efficiency", 1,
                              true, text, 0.55, 0.9, 0.04,
                              true, legend, 0.58, 0.6, 0.04,
                              "figure/efficiency_calib_frac_jesdown.pdf");
  h_input.clear();
  color.clear();
  markerstyle.clear();
  text.clear();
  legend.clear();

  h_input.push_back((TH1F*)h_truth_calib_frac_jesup);
  h_input.push_back((TH1F*)h_efficiency_calib_frac_jesup_proj);
  color.push_back(kBlack);
  color.push_back(kRed);
  markerstyle.push_back(20);
  markerstyle.push_back(20);
  text.push_back("#bf{#it{sPHENIX}} Simulation");
  text.push_back("PYTHIA8 p+p#sqrt{s} = 200 GeV");
  text.push_back("anti-k_{T}#it{R} = 0.4");
  text.push_back("|#eta^{jet}| < 0.7");
  text.push_back("#DeltaR_{matching} < 0.3");
  legend.push_back("Truth jet spectrum");
  legend.push_back("Matched truth jet spectrum");
  draw_1D_multiple_plot_ratio(h_input, color, markerstyle,
                              false, 10, true,
                              true, 15, 72, false,
                              true, 1e-9, 1e-1, true,
                              true, 0, 1.2,
                              true, "p_{T}^{truth jet} [GeV]", "Arbitrary Unit", "Efficiency", 1,
                              true, text, 0.55, 0.9, 0.04,
                              true, legend, 0.58, 0.65, 0.04,
                              "figure/efficiency_calib_frac_jesup.pdf");
  h_input.clear();
  color.clear();
  markerstyle.clear();
  text.clear();
  legend.clear();

  h_input.push_back((TH1F*)h_truth_calib_frac_jerdown);
  h_input.push_back((TH1F*)h_efficiency_calib_frac_jerdown_proj);
  color.push_back(kBlack);
  color.push_back(kRed);
  markerstyle.push_back(20);
  markerstyle.push_back(20);
  text.push_back("#bf{#it{sPHENIX}} Simulation");
  text.push_back("PYTHIA8 p+p#sqrt{s} = 200 GeV");
  text.push_back("anti-k_{T}#it{R} = 0.4");
  text.push_back("|#eta^{jet}| < 0.7");
  text.push_back("#DeltaR_{matching} < 0.3");
  text.push_back("With JES calibration");
  legend.push_back("Truth jet spectrum");
  legend.push_back("Matched truth jet spectrum");
  draw_1D_multiple_plot_ratio(h_input, color, markerstyle,
                              false, 10, true,
                              true, 15, 72, false,
                              true, 1e-9, 1e-1, true,
                              true, 0, 1.2,
                              true, "p_{T}^{truth jet} [GeV]", "Arbitrary Unit", "Efficiency", 1,
                              true, text, 0.55, 0.9, 0.04,
                              true, legend, 0.58, 0.6, 0.04,
                              "figure/efficiency_calib_frac_jerdown.pdf");
  h_input.clear();
  color.clear();
  markerstyle.clear();
  text.clear();
  legend.clear();

  h_input.push_back((TH1F*)h_truth_calib_frac_jerup);
  h_input.push_back((TH1F*)h_efficiency_calib_frac_jerup_proj);
  color.push_back(kBlack);
  color.push_back(kRed);
  markerstyle.push_back(20);
  markerstyle.push_back(20);
  text.push_back("#bf{#it{sPHENIX}} Simulation");
  text.push_back("PYTHIA8 p+p#sqrt{s} = 200 GeV");
  text.push_back("anti-k_{T}#it{R} = 0.4");
  text.push_back("|#eta^{jet}| < 0.7");
  text.push_back("#DeltaR_{matching} < 0.3");
  text.push_back("With JES calibration");
  legend.push_back("Truth jet spectrum");
  legend.push_back("Matched truth jet spectrum");
  draw_1D_multiple_plot_ratio(h_input, color, markerstyle,
                              false, 10, true,
                              true, 15, 72, false,
                              true, 1e-9, 1e-1, true,
                              true, 0, 1.2,
                              true, "p_{T}^{truth jet} [GeV]", "Arbitrary Unit", "Efficiency", 1,
                              true, text, 0.55, 0.9, 0.04,
                              true, legend, 0.58, 0.6, 0.04,
                              "figure/efficiency_calib_frac_jerup.pdf");
  h_input.clear();
  color.clear();
  markerstyle.clear();
  text.clear();
  legend.clear();
}