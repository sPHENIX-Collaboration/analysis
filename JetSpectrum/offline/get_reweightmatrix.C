#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TMath.h>
#include <iostream>
#include "/sphenix/user/hanpuj/CaloDataAna24_skimmed/src/draw_template.C"

void get_reweight_hist(TFile* f_data, TH2D* h_respmatrix, TH1D*& h_reweight, std::string input_histname, std::string output_histname) { 
  TH1D* h_data_reco = (TH1D*)f_data->Get(input_histname.c_str());
  TH1D* h_sim_reco = (TH1D*)h_respmatrix->ProjectionX("h_sim_reco");
  h_reweight = (TH1D*)h_data_reco->Clone(output_histname.c_str());
  double normalization_reco = h_data_reco->Integral() / h_sim_reco->Integral();
  h_sim_reco->Scale(normalization_reco);
  h_reweight->Divide(h_sim_reco);

  std::string prefix_to_remove = "h_reweight_";
  std::string new_prefix = "reweightfactor_";
  std::string plot_name = new_prefix + output_histname.substr(prefix_to_remove.length());

  std::vector<TH1F*> h_input;
  std::vector<int> color;
  std::vector<int> markerstyle;
  std::vector<std::string> text;
  std::vector<std::string> legend;

  h_input.push_back((TH1F*)h_sim_reco);
  h_input.push_back((TH1F*)h_data_reco);
  color.push_back(kRed);
  color.push_back(kBlack);
  markerstyle.push_back(20);
  markerstyle.push_back(20);
  text.push_back("#bf{#it{sPHENIX}} Internal");
  text.push_back("Data & PYTHIA8 p+p#sqrt{s} = 200 GeV");
  text.push_back("anti-k_{T}#it{R} = 0.4");
  text.push_back("|#eta^{jet}| < 0.7");
  text.push_back("#DeltaR_{matching} < 0.3");
  legend.push_back("Simulation jet spectrum");
  legend.push_back("Data jet spectrum");
  draw_1D_multiple_plot_ratio(h_input, color, markerstyle,
                              false, 10, true,
                              true, 15, 72, false,
                              false, 0, 0.5, true,
                              true, 0., 2.,
                              true, "p_{T}^{reco jet} [GeV]", "Arbitrary Unit", "Reweight factor", 0,
                              true, text, 0.52, 0.9, 0.04,
                              true, legend, 0.55, 0.65, 0.04,
                              Form("figure/%s.pdf", plot_name.c_str()));
  h_input.clear();
  color.clear();
  markerstyle.clear();
  text.clear();
  legend.clear();
}

void get_reweightmatrix() {
  // Read Files
  TFile* f_data = new TFile("output_data_puritycorr.root", "READ");
  TFile* f_sim = new TFile("output_sim.root", "READ");
  TFile* f_out = new TFile("output_reweightmatrix.root", "RECREATE");

  TH2D* h_respmatrix_calib_dijet = (TH2D*)f_sim->Get("h_respmatrix_calib_dijet");
  TH2D* h_respmatrix_calib_dijet_jesdown = (TH2D*)f_sim->Get("h_respmatrix_calib_dijet_jesdown");
  TH2D* h_respmatrix_calib_dijet_jesup = (TH2D*)f_sim->Get("h_respmatrix_calib_dijet_jesup");
  TH2D* h_respmatrix_calib_dijet_jerdown = (TH2D*)f_sim->Get("h_respmatrix_calib_dijet_jerdown");
  TH2D* h_respmatrix_calib_dijet_jerup = (TH2D*)f_sim->Get("h_respmatrix_calib_dijet_jerup");
  
  TH2D* h_respmatrix_calib_frac = (TH2D*)f_sim->Get("h_respmatrix_calib_frac");
  TH2D* h_respmatrix_calib_frac_jesdown = (TH2D*)f_sim->Get("h_respmatrix_calib_frac_jesdown");
  TH2D* h_respmatrix_calib_frac_jesup = (TH2D*)f_sim->Get("h_respmatrix_calib_frac_jesup");
  TH2D* h_respmatrix_calib_frac_jerdown = (TH2D*)f_sim->Get("h_respmatrix_calib_frac_jerdown");
  TH2D* h_respmatrix_calib_frac_jerup = (TH2D*)f_sim->Get("h_respmatrix_calib_frac_jerup");

  // Form reweight histograms.
  TH1D* h_reweight_calib_dijet; get_reweight_hist(f_data, h_respmatrix_calib_dijet, h_reweight_calib_dijet, "h_calibjet_pt_puritycorr_dijet", "h_reweight_calib_dijet");
  TH1D* h_reweight_calib_dijet_effdown; get_reweight_hist(f_data, h_respmatrix_calib_dijet, h_reweight_calib_dijet_effdown, "h_calibjet_pt_puritycorr_dijet_effdown", "h_reweight_calib_dijet_effdown");
  TH1D* h_reweight_calib_dijet_effup; get_reweight_hist(f_data, h_respmatrix_calib_dijet, h_reweight_calib_dijet_effup, "h_calibjet_pt_puritycorr_dijet_effup", "h_reweight_calib_dijet_effup");
  TH1D* h_reweight_calib_dijet_jesdown; get_reweight_hist(f_data, h_respmatrix_calib_dijet_jesdown, h_reweight_calib_dijet_jesdown, "h_calibjet_pt_puritycorr_dijet_jesdown", "h_reweight_calib_dijet_jesdown");
  TH1D* h_reweight_calib_dijet_jesup; get_reweight_hist(f_data, h_respmatrix_calib_dijet_jesup, h_reweight_calib_dijet_jesup, "h_calibjet_pt_puritycorr_dijet_jesup", "h_reweight_calib_dijet_jesup");
  TH1D* h_reweight_calib_dijet_jerdown; get_reweight_hist(f_data, h_respmatrix_calib_dijet_jerdown, h_reweight_calib_dijet_jerdown, "h_calibjet_pt_puritycorr_dijet_jerdown", "h_reweight_calib_dijet_jerdown");
  TH1D* h_reweight_calib_dijet_jerup; get_reweight_hist(f_data, h_respmatrix_calib_dijet_jerup, h_reweight_calib_dijet_jerup, "h_calibjet_pt_puritycorr_dijet_jerup", "h_reweight_calib_dijet_jerup");

  TH1D* h_reweight_calib_frac; get_reweight_hist(f_data, h_respmatrix_calib_frac, h_reweight_calib_frac, "h_calibjet_pt_puritycorr_frac", "h_reweight_calib_frac");
  TH1D* h_reweight_calib_frac_effdown; get_reweight_hist(f_data, h_respmatrix_calib_frac, h_reweight_calib_frac_effdown, "h_calibjet_pt_puritycorr_frac_effdown", "h_reweight_calib_frac_effdown");
  TH1D* h_reweight_calib_frac_effup; get_reweight_hist(f_data, h_respmatrix_calib_frac, h_reweight_calib_frac_effup, "h_calibjet_pt_puritycorr_frac_effup", "h_reweight_calib_frac_effup");
  TH1D* h_reweight_calib_frac_jesdown; get_reweight_hist(f_data, h_respmatrix_calib_frac_jesdown, h_reweight_calib_frac_jesdown, "h_calibjet_pt_puritycorr_frac_jesdown", "h_reweight_calib_frac_jesdown");
  TH1D* h_reweight_calib_frac_jesup; get_reweight_hist(f_data, h_respmatrix_calib_frac_jesup, h_reweight_calib_frac_jesup, "h_calibjet_pt_puritycorr_frac_jesup", "h_reweight_calib_frac_jesup");
  TH1D* h_reweight_calib_frac_jerdown; get_reweight_hist(f_data, h_respmatrix_calib_frac_jerdown, h_reweight_calib_frac_jerdown, "h_calibjet_pt_puritycorr_frac_jerdown", "h_reweight_calib_frac_jerdown");
  TH1D* h_reweight_calib_frac_jerup; get_reweight_hist(f_data, h_respmatrix_calib_frac_jerup, h_reweight_calib_frac_jerup, "h_calibjet_pt_puritycorr_frac_jerup", "h_reweight_calib_frac_jerup");

  // Form reweighted response matrix.
  TH2D* h_respmatrix_calib_dijet_reweighted = (TH2D*)h_respmatrix_calib_dijet->Clone("h_respmatrix_calib_dijet_reweighted");
  TH2D* h_respmatrix_calib_dijet_effdown_reweighted = (TH2D*)h_respmatrix_calib_dijet->Clone("h_respmatrix_calib_dijet_effdown_reweighted");
  TH2D* h_respmatrix_calib_dijet_effup_reweighted = (TH2D*)h_respmatrix_calib_dijet->Clone("h_respmatrix_calib_dijet_effup_reweighted");
  TH2D* h_respmatrix_calib_dijet_jesdown_reweighted = (TH2D*)h_respmatrix_calib_dijet_jesdown->Clone("h_respmatrix_calib_dijet_jesdown_reweighted");
  TH2D* h_respmatrix_calib_dijet_jesup_reweighted = (TH2D*)h_respmatrix_calib_dijet_jesup->Clone("h_respmatrix_calib_dijet_jesup_reweighted");
  TH2D* h_respmatrix_calib_dijet_jerdown_reweighted = (TH2D*)h_respmatrix_calib_dijet_jerdown->Clone("h_respmatrix_calib_dijet_jerdown_reweighted");
  TH2D* h_respmatrix_calib_dijet_jerup_reweighted = (TH2D*)h_respmatrix_calib_dijet_jerup->Clone("h_respmatrix_calib_dijet_jerup_reweighted");

  TH2D* h_respmatrix_calib_frac_reweighted = (TH2D*)h_respmatrix_calib_frac->Clone("h_respmatrix_calib_frac_reweighted");
  TH2D* h_respmatrix_calib_frac_effdown_reweighted = (TH2D*)h_respmatrix_calib_frac->Clone("h_respmatrix_calib_frac_effdown_reweighted");
  TH2D* h_respmatrix_calib_frac_effup_reweighted = (TH2D*)h_respmatrix_calib_frac->Clone("h_respmatrix_calib_frac_effup_reweighted");
  TH2D* h_respmatrix_calib_frac_jesdown_reweighted = (TH2D*)h_respmatrix_calib_frac_jesdown->Clone("h_respmatrix_calib_frac_jesdown_reweighted");
  TH2D* h_respmatrix_calib_frac_jesup_reweighted = (TH2D*)h_respmatrix_calib_frac_jesup->Clone("h_respmatrix_calib_frac_jesup_reweighted");
  TH2D* h_respmatrix_calib_frac_jerdown_reweighted = (TH2D*)h_respmatrix_calib_frac_jerdown->Clone("h_respmatrix_calib_frac_jerdown_reweighted");
  TH2D* h_respmatrix_calib_frac_jerup_reweighted = (TH2D*)h_respmatrix_calib_frac_jerup->Clone("h_respmatrix_calib_frac_jerup_reweighted");

  int nbin_meas = h_respmatrix_calib_dijet_reweighted->GetNbinsX();
  int nbin_truth = h_respmatrix_calib_dijet_reweighted->GetNbinsY();
  for (int ibm = 1; ibm <= nbin_meas; ++ibm) {
    for (int ibt = 1; ibt <= nbin_truth; ++ibt) {
      h_respmatrix_calib_dijet_reweighted->SetBinContent(ibm, ibt, h_respmatrix_calib_dijet_reweighted->GetBinContent(ibm, ibt) * h_reweight_calib_dijet->GetBinContent(ibm));
      h_respmatrix_calib_dijet_reweighted->SetBinError(ibm, ibt, h_respmatrix_calib_dijet_reweighted->GetBinError(ibm, ibt) * h_reweight_calib_dijet->GetBinContent(ibm));

      h_respmatrix_calib_dijet_effdown_reweighted->SetBinContent(ibm, ibt, h_respmatrix_calib_dijet_effdown_reweighted->GetBinContent(ibm, ibt) * h_reweight_calib_dijet_effdown->GetBinContent(ibm));
      h_respmatrix_calib_dijet_effdown_reweighted->SetBinError(ibm, ibt, h_respmatrix_calib_dijet_effdown_reweighted->GetBinError(ibm, ibt) * h_reweight_calib_dijet_effdown->GetBinContent(ibm));

      h_respmatrix_calib_dijet_effup_reweighted->SetBinContent(ibm, ibt, h_respmatrix_calib_dijet_effup_reweighted->GetBinContent(ibm, ibt) * h_reweight_calib_dijet_effup->GetBinContent(ibm));
      h_respmatrix_calib_dijet_effup_reweighted->SetBinError(ibm, ibt, h_respmatrix_calib_dijet_effup_reweighted->GetBinError(ibm, ibt) * h_reweight_calib_dijet_effup->GetBinContent(ibm));

      h_respmatrix_calib_dijet_jesdown_reweighted->SetBinContent(ibm, ibt, h_respmatrix_calib_dijet_jesdown_reweighted->GetBinContent(ibm, ibt) * h_reweight_calib_dijet_jesdown->GetBinContent(ibm));
      h_respmatrix_calib_dijet_jesdown_reweighted->SetBinError(ibm, ibt, h_respmatrix_calib_dijet_jesdown_reweighted->GetBinError(ibm, ibt) * h_reweight_calib_dijet_jesdown->GetBinContent(ibm));

      h_respmatrix_calib_dijet_jesup_reweighted->SetBinContent(ibm, ibt, h_respmatrix_calib_dijet_jesup_reweighted->GetBinContent(ibm, ibt) * h_reweight_calib_dijet_jesup->GetBinContent(ibm));
      h_respmatrix_calib_dijet_jesup_reweighted->SetBinError(ibm, ibt, h_respmatrix_calib_dijet_jesup_reweighted->GetBinError(ibm, ibt) * h_reweight_calib_dijet_jesup->GetBinContent(ibm));

      h_respmatrix_calib_dijet_jerdown_reweighted->SetBinContent(ibm, ibt, h_respmatrix_calib_dijet_jerdown_reweighted->GetBinContent(ibm, ibt) * h_reweight_calib_dijet_jerdown->GetBinContent(ibm));
      h_respmatrix_calib_dijet_jerdown_reweighted->SetBinError(ibm, ibt, h_respmatrix_calib_dijet_jerdown_reweighted->GetBinError(ibm, ibt) * h_reweight_calib_dijet_jerdown->GetBinContent(ibm));

      h_respmatrix_calib_dijet_jerup_reweighted->SetBinContent(ibm, ibt, h_respmatrix_calib_dijet_jerup_reweighted->GetBinContent(ibm, ibt) * h_reweight_calib_dijet_jerup->GetBinContent(ibm));
      h_respmatrix_calib_dijet_jerup_reweighted->SetBinError(ibm, ibt, h_respmatrix_calib_dijet_jerup_reweighted->GetBinError(ibm, ibt) * h_reweight_calib_dijet_jerup->GetBinContent(ibm));

      h_respmatrix_calib_frac_reweighted->SetBinContent(ibm, ibt, h_respmatrix_calib_frac_reweighted->GetBinContent(ibm, ibt) * h_reweight_calib_frac->GetBinContent(ibm));
      h_respmatrix_calib_frac_reweighted->SetBinError(ibm, ibt, h_respmatrix_calib_frac_reweighted->GetBinError(ibm, ibt) * h_reweight_calib_frac->GetBinContent(ibm));

      h_respmatrix_calib_frac_effdown_reweighted->SetBinContent(ibm, ibt, h_respmatrix_calib_frac_effdown_reweighted->GetBinContent(ibm, ibt) * h_reweight_calib_frac_effdown->GetBinContent(ibm));
      h_respmatrix_calib_frac_effdown_reweighted->SetBinError(ibm, ibt, h_respmatrix_calib_frac_effdown_reweighted->GetBinError(ibm, ibt) * h_reweight_calib_frac_effdown->GetBinContent(ibm));

      h_respmatrix_calib_frac_effup_reweighted->SetBinContent(ibm, ibt, h_respmatrix_calib_frac_effup_reweighted->GetBinContent(ibm, ibt) * h_reweight_calib_frac_effup->GetBinContent(ibm));
      h_respmatrix_calib_frac_effup_reweighted->SetBinError(ibm, ibt, h_respmatrix_calib_frac_effup_reweighted->GetBinError(ibm, ibt) * h_reweight_calib_frac_effup->GetBinContent(ibm));

      h_respmatrix_calib_frac_jesdown_reweighted->SetBinContent(ibm, ibt, h_respmatrix_calib_frac_jesdown_reweighted->GetBinContent(ibm, ibt) * h_reweight_calib_frac_jesdown->GetBinContent(ibm));
      h_respmatrix_calib_frac_jesdown_reweighted->SetBinError(ibm, ibt, h_respmatrix_calib_frac_jesdown_reweighted->GetBinError(ibm, ibt) * h_reweight_calib_frac_jesdown->GetBinContent(ibm));

      h_respmatrix_calib_frac_jesup_reweighted->SetBinContent(ibm, ibt, h_respmatrix_calib_frac_jesup_reweighted->GetBinContent(ibm, ibt) * h_reweight_calib_frac_jesup->GetBinContent(ibm));
      h_respmatrix_calib_frac_jesup_reweighted->SetBinError(ibm, ibt, h_respmatrix_calib_frac_jesup_reweighted->GetBinError(ibm, ibt) * h_reweight_calib_frac_jesup->GetBinContent(ibm));

      h_respmatrix_calib_frac_jerdown_reweighted->SetBinContent(ibm, ibt, h_respmatrix_calib_frac_jerdown_reweighted->GetBinContent(ibm, ibt) * h_reweight_calib_frac_jerdown->GetBinContent(ibm));
      h_respmatrix_calib_frac_jerdown_reweighted->SetBinError(ibm, ibt, h_respmatrix_calib_frac_jerdown_reweighted->GetBinError(ibm, ibt) * h_reweight_calib_frac_jerdown->GetBinContent(ibm));

      h_respmatrix_calib_frac_jerup_reweighted->SetBinContent(ibm, ibt, h_respmatrix_calib_frac_jerup_reweighted->GetBinContent(ibm, ibt) * h_reweight_calib_frac_jerup->GetBinContent(ibm));
      h_respmatrix_calib_frac_jerup_reweighted->SetBinError(ibm, ibt, h_respmatrix_calib_frac_jerup_reweighted->GetBinError(ibm, ibt) * h_reweight_calib_frac_jerup->GetBinContent(ibm));
    }
  }

  // Draw response matrix.
  std::vector<std::string> text;
  text.push_back("#bf{#it{sPHENIX}} Simulation");
  text.push_back("PYTHIA8 p+p#sqrt{s} = 200 GeV");
  text.push_back("anti-k_{T}#it{R} = 0.4");
  text.push_back("|#eta^{jet}| < 0.7");
  text.push_back("#DeltaR_{matching} < 0.3");
  draw_2D_plot((TH2F*)h_respmatrix_calib_dijet,
               false, 1, 1, false,
               false, 0, 40, false,
               false, 0, 40, false,
               true, 1e-9, 2, true,
               true, "p_{T}^{reco jet} [GeV]", "p_{T}^{truth jet} [GeV]", "Arbitrary Unit",
               true, text, 0.16, 0.9, 0.04,
               "figure/respmatrix_calib_dijet.pdf");

  draw_2D_plot((TH2F*)h_respmatrix_calib_dijet_jesdown,
               false, 1, 1, false,
               false, 0, 40, false,
               false, 0, 40, false,
               true, 1e-9, 2, true,
               true, "p_{T}^{reco jet} [GeV]", "p_{T}^{truth jet} [GeV]", "Arbitrary Unit",
               true, text, 0.16, 0.9, 0.04,
               "figure/respmatrix_calib_dijet_jesdown.pdf");

  draw_2D_plot((TH2F*)h_respmatrix_calib_dijet_jesup,
               false, 1, 1, false,
               false, 0, 40, false,
               false, 0, 40, false,
               true, 1e-9, 2, true,
               true, "p_{T}^{reco jet} [GeV]", "p_{T}^{truth jet} [GeV]", "Arbitrary Unit",
               true, text, 0.16, 0.9, 0.04,
               "figure/respmatrix_calib_dijet_jesup.pdf");

  draw_2D_plot((TH2F*)h_respmatrix_calib_dijet_jerdown,
               false, 1, 1, false,
               false, 0, 40, false,
               false, 0, 40, false,
               true, 1e-9, 2, true,
               true, "p_{T}^{reco jet} [GeV]", "p_{T}^{truth jet} [GeV]", "Arbitrary Unit",
               true, text, 0.16, 0.9, 0.04,
               "figure/respmatrix_calib_dijet_jerdown.pdf");

  draw_2D_plot((TH2F*)h_respmatrix_calib_dijet_jerup,
               false, 1, 1, false,
               false, 0, 40, false,
               false, 0, 40, false,
               true, 1e-9, 2, true,
               true, "p_{T}^{reco jet} [GeV]", "p_{T}^{truth jet} [GeV]", "Arbitrary Unit",
               true, text, 0.16, 0.9, 0.04,
               "figure/respmatrix_calib_dijet_jerup.pdf");

  draw_2D_plot((TH2F*)h_respmatrix_calib_frac,
               false, 1, 1, false,
               false, 0, 40, false,
               false, 0, 40, false,
               true, 1e-9, 2, true,
               true, "p_{T}^{reco jet} [GeV]", "p_{T}^{truth jet} [GeV]", "Arbitrary Unit",
               true, text, 0.16, 0.9, 0.04,
               "figure/respmatrix_calib_frac.pdf");

  draw_2D_plot((TH2F*)h_respmatrix_calib_frac_jesdown,
               false, 1, 1, false,
               false, 0, 40, false,
               false, 0, 40, false,
               true, 1e-9, 2, true,
               true, "p_{T}^{reco jet} [GeV]", "p_{T}^{truth jet} [GeV]", "Arbitrary Unit",
               true, text, 0.16, 0.9, 0.04,
               "figure/respmatrix_calib_frac_jesdown.pdf");

  draw_2D_plot((TH2F*)h_respmatrix_calib_frac_jesup,
               false, 1, 1, false,
               false, 0, 40, false,
               false, 0, 40, false,
               true, 1e-9, 2, true,
               true, "p_{T}^{reco jet} [GeV]", "p_{T}^{truth jet} [GeV]", "Arbitrary Unit",
               true, text, 0.16, 0.9, 0.04,
               "figure/respmatrix_calib_frac_jesup.pdf");

  draw_2D_plot((TH2F*)h_respmatrix_calib_frac_jerdown,
               false, 1, 1, false,
               false, 0, 40, false,
               false, 0, 40, false,
               true, 1e-9, 2, true,
               true, "p_{T}^{reco jet} [GeV]", "p_{T}^{truth jet} [GeV]", "Arbitrary Unit",
               true, text, 0.16, 0.9, 0.04,
               "figure/respmatrix_calib_frac_jerdown.pdf");

  draw_2D_plot((TH2F*)h_respmatrix_calib_frac_jerup,
               false, 1, 1, false,
               false, 0, 40, false,
               false, 0, 40, false,
               true, 1e-9, 2, true,
               true, "p_{T}^{reco jet} [GeV]", "p_{T}^{truth jet} [GeV]", "Arbitrary Unit",
               true, text, 0.16, 0.9, 0.04,
               "figure/respmatrix_calib_frac_jerup.pdf");

  draw_2D_plot((TH2F*)h_respmatrix_calib_dijet_reweighted,
               false, 1, 1, false,
               false, 0, 40, false,
               false, 0, 40, false,
               true, 1e-9, 2, true,
               true, "p_{T}^{reco jet} [GeV]", "p_{T}^{truth jet} [GeV]", "Arbitrary Unit",
               true, text, 0.16, 0.9, 0.04,
               "figure/respmatrix_reweighted_calib_dijet.pdf");

  draw_2D_plot((TH2F*)h_respmatrix_calib_dijet_effdown_reweighted,
               false, 1, 1, false,
               false, 0, 40, false,
               false, 0, 40, false,
               true, 1e-9, 2, true,
               true, "p_{T}^{reco jet} [GeV]", "p_{T}^{truth jet} [GeV]", "Arbitrary Unit",
               true, text, 0.16, 0.9, 0.04,
               "figure/respmatrix_reweighted_calib_dijet_effdown.pdf");

  draw_2D_plot((TH2F*)h_respmatrix_calib_dijet_effup_reweighted,
               false, 1, 1, false,
               false, 0, 40, false,
               false, 0, 40, false,
               true, 1e-9, 2, true,
               true, "p_{T}^{reco jet} [GeV]", "p_{T}^{truth jet} [GeV]", "Arbitrary Unit",
               true, text, 0.16, 0.9, 0.04,
               "figure/respmatrix_reweighted_calib_dijet_effup.pdf");

  draw_2D_plot((TH2F*)h_respmatrix_calib_dijet_jesdown_reweighted,
               false, 1, 1, false,
               false, 0, 40, false,
               false, 0, 40, false,
               true, 1e-9, 2, true,
               true, "p_{T}^{reco jet} [GeV]", "p_{T}^{truth jet} [GeV]", "Arbitrary Unit",
               true, text, 0.16, 0.9, 0.04,
               "figure/respmatrix_reweighted_calib_dijet_jesdown.pdf");

  draw_2D_plot((TH2F*)h_respmatrix_calib_dijet_jesup_reweighted,
               false, 1, 1, false,
               false, 0, 40, false,
               false, 0, 40, false,
               true, 1e-9, 2, true,
               true, "p_{T}^{reco jet} [GeV]", "p_{T}^{truth jet} [GeV]", "Arbitrary Unit",
               true, text, 0.16, 0.9, 0.04,
               "figure/respmatrix_reweighted_calib_dijet_jesup.pdf");

  draw_2D_plot((TH2F*)h_respmatrix_calib_dijet_jerdown_reweighted,
               false, 1, 1, false,
               false, 0, 40, false,
               false, 0, 40, false,
               true, 1e-9, 2, true,
               true, "p_{T}^{reco jet} [GeV]", "p_{T}^{truth jet} [GeV]", "Arbitrary Unit",
               true, text, 0.16, 0.9, 0.04,
               "figure/respmatrix_reweighted_calib_dijet_jerdown.pdf");

  draw_2D_plot((TH2F*)h_respmatrix_calib_dijet_jerup_reweighted,
               false, 1, 1, false,
               false, 0, 40, false,
               false, 0, 40, false,
               true, 1e-9, 2, true,
               true, "p_{T}^{reco jet} [GeV]", "p_{T}^{truth jet} [GeV]", "Arbitrary Unit",
               true, text, 0.16, 0.9, 0.04,
               "figure/respmatrix_reweighted_calib_dijet_jerup.pdf");

  draw_2D_plot((TH2F*)h_respmatrix_calib_frac_reweighted,
               false, 1, 1, false,
               false, 0, 40, false,
               false, 0, 40, false,
               true, 1e-9, 2, true,
               true, "p_{T}^{reco jet} [GeV]", "p_{T}^{truth jet} [GeV]", "Arbitrary Unit",
               true, text, 0.16, 0.9, 0.04,
               "figure/respmatrix_reweighted_calib_frac.pdf");

  draw_2D_plot((TH2F*)h_respmatrix_calib_frac_effdown_reweighted,
               false, 1, 1, false,
               false, 0, 40, false,
               false, 0, 40, false,
               true, 1e-9, 2, true,
               true, "p_{T}^{reco jet} [GeV]", "p_{T}^{truth jet} [GeV]", "Arbitrary Unit",
               true, text, 0.16, 0.9, 0.04,
               "figure/respmatrix_reweighted_calib_frac_effdown.pdf");

  draw_2D_plot((TH2F*)h_respmatrix_calib_frac_effup_reweighted,
               false, 1, 1, false,
               false, 0, 40, false,
               false, 0, 40, false,
               true, 1e-9, 2, true,
               true, "p_{T}^{reco jet} [GeV]", "p_{T}^{truth jet} [GeV]", "Arbitrary Unit",
               true, text, 0.16, 0.9, 0.04,
               "figure/respmatrix_reweighted_calib_frac_effup.pdf");

  draw_2D_plot((TH2F*)h_respmatrix_calib_frac_jesdown_reweighted,
               false, 1, 1, false,
               false, 0, 40, false,
               false, 0, 40, false,
               true, 1e-9, 2, true,
               true, "p_{T}^{reco jet} [GeV]", "p_{T}^{truth jet} [GeV]", "Arbitrary Unit",
               true, text, 0.16, 0.9, 0.04,
               "figure/respmatrix_reweighted_calib_frac_jesdown.pdf");

  draw_2D_plot((TH2F*)h_respmatrix_calib_frac_jesup_reweighted,
               false, 1, 1, false,
               false, 0, 40, false,
               false, 0, 40, false,
               true, 1e-9, 2, true,
               true, "p_{T}^{reco jet} [GeV]", "p_{T}^{truth jet} [GeV]", "Arbitrary Unit",
               true, text, 0.16, 0.9, 0.04,
               "figure/respmatrix_reweighted_calib_frac_jesup.pdf");

  draw_2D_plot((TH2F*)h_respmatrix_calib_frac_jerdown_reweighted,
               false, 1, 1, false,
               false, 0, 40, false,
               false, 0, 40, false,
               true, 1e-9, 2, true,
               true, "p_{T}^{reco jet} [GeV]", "p_{T}^{truth jet} [GeV]", "Arbitrary Unit",
               true, text, 0.16, 0.9, 0.04,
               "figure/respmatrix_reweighted_calib_frac_jerdown.pdf");

  draw_2D_plot((TH2F*)h_respmatrix_calib_frac_jerup_reweighted,
               false, 1, 1, false,
               false, 0, 40, false,
               false, 0, 40, false,
               true, 1e-9, 2, true,
               true, "p_{T}^{reco jet} [GeV]", "p_{T}^{truth jet} [GeV]", "Arbitrary Unit",
               true, text, 0.16, 0.9, 0.04,
               "figure/respmatrix_reweighted_calib_frac_jerup.pdf");

  f_out->cd();
  h_respmatrix_calib_dijet->Write();
  h_respmatrix_calib_dijet_jesdown->Write();
  h_respmatrix_calib_dijet_jesup->Write();
  h_respmatrix_calib_dijet_jerdown->Write();
  h_respmatrix_calib_dijet_jerup->Write();
  h_respmatrix_calib_frac->Write();
  h_respmatrix_calib_frac_jesdown->Write();
  h_respmatrix_calib_frac_jesup->Write();
  h_respmatrix_calib_frac_jerdown->Write();
  h_respmatrix_calib_frac_jerup->Write();

  h_reweight_calib_dijet->Write();
  h_reweight_calib_dijet_effdown->Write();
  h_reweight_calib_dijet_effup->Write();
  h_reweight_calib_dijet_jesdown->Write();
  h_reweight_calib_dijet_jesup->Write();
  h_reweight_calib_dijet_jerdown->Write();
  h_reweight_calib_dijet_jerup->Write();
  h_reweight_calib_frac->Write();
  h_reweight_calib_frac_effdown->Write();
  h_reweight_calib_frac_effup->Write();
  h_reweight_calib_frac_jesdown->Write();
  h_reweight_calib_frac_jesup->Write();
  h_reweight_calib_frac_jerdown->Write();
  h_reweight_calib_frac_jerup->Write();

  h_respmatrix_calib_dijet_reweighted->Write();
  h_respmatrix_calib_dijet_effdown_reweighted->Write();
  h_respmatrix_calib_dijet_effup_reweighted->Write();
  h_respmatrix_calib_dijet_jesdown_reweighted->Write();
  h_respmatrix_calib_dijet_jesup_reweighted->Write();
  h_respmatrix_calib_dijet_jerdown_reweighted->Write();
  h_respmatrix_calib_dijet_jerup_reweighted->Write();
  h_respmatrix_calib_frac_reweighted->Write();
  h_respmatrix_calib_frac_effdown_reweighted->Write();
  h_respmatrix_calib_frac_effup_reweighted->Write();
  h_respmatrix_calib_frac_jesdown_reweighted->Write();
  h_respmatrix_calib_frac_jesup_reweighted->Write();
  h_respmatrix_calib_frac_jerdown_reweighted->Write();
  h_respmatrix_calib_frac_jerup_reweighted->Write();
  f_out->Close();
}