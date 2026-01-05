#include <iostream>
#include <fstream>
#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TMath.h>
#include <vector>
#include "TLatex.h"
#include <string>
#include "unfold_Def.h"
#include "/sphenix/user/hanpuj/CaloDataAna24_skimmed/src/draw_template.C"               

void do_normalization(TH1D* h_unfolded, double luminosity, TH1D* h_etacorrection, float jet_radius) {
  double delta_eta = 2 * (1.1 - jet_radius);
  for (int i = 1; i <= h_unfolded->GetNbinsX(); ++i) {
    double binwidth = h_unfolded->GetBinWidth(i);
    h_unfolded->SetBinContent(i, h_unfolded->GetBinContent(i) / (double)(luminosity * binwidth * delta_eta));
    h_unfolded->SetBinError(i, h_unfolded->GetBinError(i) / (double)(luminosity * binwidth * delta_eta));
  }
}

TH1D* get_statistical_uncertainty(TH1D* h_unfold, string name) {
  TH1D* h_uncertainty = (TH1D*)h_unfold->Clone(name.c_str());
  h_uncertainty->Reset();
  for (int i = 1; i <= h_unfold->GetNbinsX(); ++i) {
    double content = h_unfold->GetBinContent(i);
    if (content == 0) continue;
    double error = h_unfold->GetBinError(i);
    double uncertainty = error / content;
    h_uncertainty->SetBinContent(i, uncertainty);
    h_uncertainty->SetBinError(i, 0);
  }
  return h_uncertainty;
}

void get_uncertainty(TH1D* &h_uncertainty_up, TH1D* &h_uncertainty_down, TH1D* h_nominal, TH1D* h_shiftup, TH1D* h_shiftdown, string name) {
  h_uncertainty_up = (TH1D*)h_nominal->Clone((name + "_up").c_str());
  h_uncertainty_down = (TH1D*)h_nominal->Clone((name + "_down").c_str());
  h_uncertainty_up->Reset();
  h_uncertainty_down->Reset();
  for (int i = 1; i <= h_nominal->GetNbinsX(); ++i) {
    double up = 0, down = 0;
    if (h_nominal->GetBinContent(i) == 0) continue;

    double diff_up = (h_shiftup->GetBinContent(i) - h_nominal->GetBinContent(i)) / (double)h_nominal->GetBinContent(i);
    if (diff_up >= 0) up = TMath::Sqrt(TMath::Power(diff_up, 2) + TMath::Power(up, 2));
    else down = TMath::Sqrt(TMath::Power(diff_up, 2) + TMath::Power(down, 2));
    double diff_down = (h_shiftdown->GetBinContent(i) - h_nominal->GetBinContent(i)) / (double)h_nominal->GetBinContent(i);
    if (diff_down >= 0) up = TMath::Sqrt(TMath::Power(diff_down, 2) + TMath::Power(up, 2));
    else down = TMath::Sqrt(TMath::Power(diff_down, 2) + TMath::Power(down, 2));

    h_uncertainty_up->SetBinContent(i, up);
    h_uncertainty_up->SetBinError(i, 0);
    h_uncertainty_down->SetBinContent(i, down);
    h_uncertainty_down->SetBinError(i, 0);
  }
}

void get_unfold_uncertainty(TH1D* &h_uncertainty_up, TH1D* &h_uncertainty_down, TH1D* h_nominal, TH1D* h_shift, string name) {
  h_uncertainty_up = (TH1D*)h_nominal->Clone((name + "_up").c_str());
  h_uncertainty_down = (TH1D*)h_nominal->Clone((name + "_down").c_str());
  h_uncertainty_up->Reset();
  h_uncertainty_down->Reset();
  for (int i = 1; i <= h_nominal->GetNbinsX(); ++i) {
    double up = 0, down = 0;
    if (h_nominal->GetBinContent(i) == 0) continue;

    double diff_up = (h_shift->GetBinContent(i) - h_nominal->GetBinContent(i)) / (double)h_nominal->GetBinContent(i);
    if (diff_up >= 0) up = TMath::Sqrt(TMath::Power(diff_up, 2) + TMath::Power(up, 2));
    else down = TMath::Sqrt(TMath::Power(diff_up, 2) + TMath::Power(down, 2));
    double diff_down = -1*(h_shift->GetBinContent(i) - h_nominal->GetBinContent(i)) / (double)h_nominal->GetBinContent(i);
    if (diff_down >= 0) up = TMath::Sqrt(TMath::Power(diff_down, 2) + TMath::Power(up, 2));
    else down = TMath::Sqrt(TMath::Power(diff_down, 2) + TMath::Power(down, 2));

    h_uncertainty_up->SetBinContent(i, up);
    h_uncertainty_up->SetBinError(i, 0);
    h_uncertainty_down->SetBinContent(i, down);
    h_uncertainty_down->SetBinError(i, 0);
  }
}

void get_total_uncertainty(TH1D* &h_total_up, TH1D* &h_total_down,TH1D* h_statistical, std::vector<TH1D*> h_systematic_up, std::vector<TH1D*> h_systematic_down) {
  h_total_up = (TH1D*)h_statistical->Clone("h_total_up");
  h_total_down = (TH1D*)h_statistical->Clone("h_total_down");
  h_total_up->Reset();
  h_total_down->Reset();
  for (int i = 1; i <= h_statistical->GetNbinsX(); ++i) {
    double stat_error = h_statistical->GetBinContent(i);

    double sys_error_up = 0;
    for (auto& h_up : h_systematic_up) {
      if (h_up->GetBinContent(i) > 0) {
        sys_error_up += h_up->GetBinContent(i) * h_up->GetBinContent(i);
      }
    }
    double total_error_up = TMath::Sqrt(stat_error * stat_error + sys_error_up);
    h_total_up->SetBinContent(i, total_error_up);
    h_total_up->SetBinError(i, 0);

    double sys_error_down = 0;
    for (auto& h_down : h_systematic_down) {
      if (h_down->GetBinContent(i) > 0) {
        sys_error_down += h_down->GetBinContent(i) * h_down->GetBinContent(i);
      }
    }
    double total_error_down = TMath::Sqrt(stat_error * stat_error + sys_error_down);
    h_total_down->SetBinContent(i, total_error_down);
    h_total_down->SetBinError(i, 0);
  }
}

void draw_total_uncertainty(TH1D* h_total_up, TH1D* h_total_down, TH1D* h_statistical, std::vector<TH1D*> h_systematic_up, std::vector<TH1D*> h_systematic_down, string label, std::vector<int> color, std::vector<string> legend) {
  TH1D* h_statistical_up = (TH1D*)h_statistical->Clone("h_statistical_up");
  TH1D* h_statistical_down = (TH1D*)h_statistical->Clone("h_statistical_down");
  h_statistical_down->Scale(-1.0);
  h_statistical_up->SetLineColor(kViolet-1);
  h_statistical_down->SetLineColor(kViolet-1);
  for (int i = 0; i < h_systematic_down.size(); ++i) {
    h_systematic_up[i]->SetLineColor(color[i]);
    h_systematic_down[i]->SetLineColor(color[i]);
    h_systematic_down[i]->Scale(-1.0);
  }
  h_total_up->SetMarkerStyle(20);
  h_total_down->SetMarkerStyle(20);
  h_total_up->SetLineColor(kBlack);
  h_total_down->SetLineColor(kBlack);
  h_total_up->SetMarkerColor(kBlack);
  h_total_down->SetMarkerColor(kBlack);
  h_total_down->Scale(-1.0);

  TCanvas *can_unc = new TCanvas("can_unc", "", 850, 800);
  gStyle->SetPalette(57);
  can_unc->SetTopMargin(0.03);
  can_unc->SetLeftMargin(0.15);
  can_unc->SetBottomMargin(0.12);
  can_unc->SetRightMargin(0.05);
  TH2F *frame_unc = new TH2F("frame_unc", "", 10, 15, 82, 2, -1.8, 2.5);
  frame_unc->GetXaxis()->SetTitle("p_{T}^{jet} [GeV]");
  frame_unc->GetYaxis()->SetTitle("Relative Uncertainty");
  frame_unc->GetXaxis()->SetTitleOffset(0.98);
  frame_unc->GetYaxis()->SetTitleOffset(1.17);
  frame_unc->GetXaxis()->SetLabelSize(0.045);
  frame_unc->GetYaxis()->SetLabelSize(0.045);
  frame_unc->GetXaxis()->CenterTitle();
  frame_unc->GetYaxis()->CenterTitle();
  frame_unc->Draw();
  h_statistical_up->Draw("hist same");
  h_statistical_down->Draw("hist same");
  for (int i = 0; i < h_systematic_up.size(); ++i) {
    h_systematic_up[i]->Draw("hist same");
    h_systematic_down[i]->Draw("hist same");
  }
  h_total_up->Draw("p same");
  h_total_down->Draw("p same");

  myText(0.17, 0.92, 1, "#bf{#it{sPHENIX}} Internal", 0.05);
  myText(0.17, 0.86, 1, "p+p#sqrt{s} = 200 GeV", 0.05);
  myText(0.17, 0.8, 1, "anti-k_{t} #kern[-0.3]{#it{R}} = 0.4", 0.05);
  myText(0.17, 0.74, 1, "|#eta^{jet}| < 0.7", 0.05);
  TLegend *leg_unc1 = new TLegend(0.15, 0.13, 0.75, 0.25);
  leg_unc1->SetNColumns(2);
  leg_unc1->SetBorderSize(0);
  leg_unc1->SetFillStyle(0);
  leg_unc1->SetTextFont(42);
  leg_unc1->AddEntry(h_statistical_up, "Statistical Uncertainty", "l");
  for (int i = 0; i < h_systematic_up.size(); ++i) {
    leg_unc1->AddEntry(h_systematic_up[i], legend[i].c_str(), "l");
  }
  leg_unc1->AddEntry(h_total_up, "Total Uncertainty", "p");
  leg_unc1->Draw();
  TLine *line = new TLine(15, 0, 82, 0);
  line->SetLineStyle(10);
  line->Draw("same");
  can_unc->SaveAs(("figure/spectrum_uncertainty" + label + ".png").c_str());

  delete can_unc;
  delete frame_unc;
}


void get_finalspectrum(float jet_radius = 0.4) {

  ifstream f_luminosity("luminosity.txt");
  double luminosity22_30, luminosity22_60, luminosity22_all;
  f_luminosity >> luminosity22_30 >> luminosity22_60 >> luminosity22_all;
  std::cout << "Luminosity: " << luminosity22_30 << "  " << luminosity22_60 << "  " << luminosity22_all << std::endl;

  TFile* f_spectrum = new TFile(Form("output_unfolded_r0%d.root", (int)(10*jet_radius)), "READ");
  if (!f_spectrum->IsOpen()) {
    std::cout << "Failed to open unfolded spectrum file!" << std::endl;
    return;
  }
  TFile* f_etacorrection = new TFile("input_etacorrection.root", "READ");
  if (!f_etacorrection->IsOpen()) {
    std::cout << "Failed to open eta correction file!" << std::endl;
    return;
  }
  TH1D* h_etacorrection = (TH1D*)f_etacorrection->Get("h_etacorrection");
  if (!h_etacorrection) {
    std::cout << "Failed to get eta correction histogram!" << std::endl;
    return;
  }


  TH1D* h_unfold_all = (TH1D*)f_spectrum->Get("h_unfold_all"); do_normalization(h_unfold_all, luminosity22_all, h_etacorrection, jet_radius);
  TH1D* h_unfold_all_jesup = (TH1D*)f_spectrum->Get("h_unfold_all_jesup"); do_normalization(h_unfold_all_jesup, luminosity22_all, h_etacorrection, jet_radius);
  TH1D* h_unfold_all_jesdown = (TH1D*)f_spectrum->Get("h_unfold_all_jesdown"); do_normalization(h_unfold_all_jesdown, luminosity22_all, h_etacorrection, jet_radius);
  TH1D* h_unfold_all_jerup = (TH1D*)f_spectrum->Get("h_unfold_all_jerup"); do_normalization(h_unfold_all_jerup, luminosity22_all, h_etacorrection, jet_radius);
  TH1D* h_unfold_all_jerdown = (TH1D*)f_spectrum->Get("h_unfold_all_jerdown"); do_normalization(h_unfold_all_jerdown, luminosity22_all, h_etacorrection, jet_radius);
  TH1D* h_unfold_all_jetup = (TH1D*)f_spectrum->Get("h_unfold_all_jetup"); do_normalization(h_unfold_all_jetup, luminosity22_all, h_etacorrection, jet_radius);
  TH1D* h_unfold_all_jetdown = (TH1D*)f_spectrum->Get("h_unfold_all_jetdown"); do_normalization(h_unfold_all_jetdown, luminosity22_all, h_etacorrection, jet_radius);
  TH1D* h_unfold_all_unfoldunc = (TH1D*)f_spectrum->Get("h_unfold_all_unfoldunc"); do_normalization(h_unfold_all_unfoldunc, luminosity22_all, h_etacorrection, jet_radius);
  TH1D* h_uncertainty_all_stat = get_statistical_uncertainty(h_unfold_all, "h_uncertainty_all_stat");
  TH1D* h_uncertainty_all_jesup, *h_uncertainty_all_jesdown; get_uncertainty(h_uncertainty_all_jesup, h_uncertainty_all_jesdown, h_unfold_all, h_unfold_all_jesup, h_unfold_all_jesdown, "h_uncertainty_all_jes");
  TH1D* h_uncertainty_all_jerup, *h_uncertainty_all_jerdown; get_uncertainty(h_uncertainty_all_jerup, h_uncertainty_all_jerdown, h_unfold_all, h_unfold_all_jerup, h_unfold_all_jerdown, "h_uncertainty_all_jer");
  TH1D* h_uncertainty_all_jetup, *h_uncertainty_all_jetdown; get_uncertainty(h_uncertainty_all_jetup, h_uncertainty_all_jetdown, h_unfold_all, h_unfold_all_jetup, h_unfold_all_jetdown, "h_uncertainty_all_jet");
  TH1D* h_uncertainty_all_unfoldup, *h_uncertainty_all_unfolddown; get_unfold_uncertainty(h_uncertainty_all_unfoldup, h_uncertainty_all_unfolddown, h_unfold_all, h_unfold_all_unfoldunc, "h_uncertainty_all_unfold");
  std::vector<TH1D*> h_uncertainty_all_up = {h_uncertainty_all_jesup, h_uncertainty_all_jerup, h_uncertainty_all_jetup, h_uncertainty_all_unfoldup};
  std::vector<TH1D*> h_uncertainty_all_down = {h_uncertainty_all_jesdown, h_uncertainty_all_jerdown, h_uncertainty_all_jetdown, h_uncertainty_all_unfolddown};
  TH1D* h_uncertainty_all_total_up, *h_uncertainty_all_total_down; get_total_uncertainty(h_uncertainty_all_total_up, h_uncertainty_all_total_down, h_uncertainty_all_stat, h_uncertainty_all_up, h_uncertainty_all_down);

  TH1D* h_unfold_zvertex30 = (TH1D*)f_spectrum->Get("h_unfold_zvertex30"); do_normalization(h_unfold_zvertex30, luminosity22_30, h_etacorrection, jet_radius);
  TH1D* h_unfold_zvertex30_jesup = (TH1D*)f_spectrum->Get("h_unfold_zvertex30_jesup"); do_normalization(h_unfold_zvertex30_jesup, luminosity22_30, h_etacorrection, jet_radius);
  TH1D* h_unfold_zvertex30_jesdown = (TH1D*)f_spectrum->Get("h_unfold_zvertex30_jesdown"); do_normalization(h_unfold_zvertex30_jesdown, luminosity22_30, h_etacorrection, jet_radius);
  TH1D* h_unfold_zvertex30_jerup = (TH1D*)f_spectrum->Get("h_unfold_zvertex30_jerup"); do_normalization(h_unfold_zvertex30_jerup, luminosity22_30, h_etacorrection, jet_radius);
  TH1D* h_unfold_zvertex30_jerdown = (TH1D*)f_spectrum->Get("h_unfold_zvertex30_jerdown"); do_normalization(h_unfold_zvertex30_jerdown, luminosity22_30, h_etacorrection, jet_radius);
  TH1D* h_unfold_zvertex30_jetup = (TH1D*)f_spectrum->Get("h_unfold_zvertex30_jetup"); do_normalization(h_unfold_zvertex30_jetup, luminosity22_30, h_etacorrection, jet_radius);
  TH1D* h_unfold_zvertex30_jetdown = (TH1D*)f_spectrum->Get("h_unfold_zvertex30_jetdown"); do_normalization(h_unfold_zvertex30_jetdown, luminosity22_30, h_etacorrection, jet_radius);
  TH1D* h_unfold_zvertex30_mbdup = (TH1D*)f_spectrum->Get("h_unfold_zvertex30_mbdup"); do_normalization(h_unfold_zvertex30_mbdup, luminosity22_30, h_etacorrection, jet_radius);
  TH1D* h_unfold_zvertex30_mbddown = (TH1D*)f_spectrum->Get("h_unfold_zvertex30_mbddown"); do_normalization(h_unfold_zvertex30_mbddown, luminosity22_30, h_etacorrection, jet_radius);
  TH1D* h_unfold_zvertex30_unfoldunc = (TH1D*)f_spectrum->Get("h_unfold_zvertex30_unfoldunc"); do_normalization(h_unfold_zvertex30_unfoldunc, luminosity22_30, h_etacorrection, jet_radius);
  TH1D* h_uncertainty_zvertex30_stat = get_statistical_uncertainty(h_unfold_zvertex30, "h_uncertainty_zvertex30_stat");
  TH1D* h_uncertainty_zvertex30_jesup, *h_uncertainty_zvertex30_jesdown; get_uncertainty(h_uncertainty_zvertex30_jesup, h_uncertainty_zvertex30_jesdown, h_unfold_zvertex30, h_unfold_zvertex30_jesup, h_unfold_zvertex30_jesdown, "h_uncertainty_zvertex30_jes");
  TH1D* h_uncertainty_zvertex30_jerup, *h_uncertainty_zvertex30_jerdown; get_uncertainty(h_uncertainty_zvertex30_jerup, h_uncertainty_zvertex30_jerdown, h_unfold_zvertex30, h_unfold_zvertex30_jerup, h_unfold_zvertex30_jerdown, "h_uncertainty_zvertex30_jer");
  TH1D* h_uncertainty_zvertex30_jetup, *h_uncertainty_zvertex30_jetdown; get_uncertainty(h_uncertainty_zvertex30_jetup, h_uncertainty_zvertex30_jetdown, h_unfold_zvertex30, h_unfold_zvertex30_jetup, h_unfold_zvertex30_jetdown, "h_uncertainty_zvertex30_jet");
  TH1D* h_uncertainty_zvertex30_mbdup, *h_uncertainty_zvertex30_mbddown; get_uncertainty(h_uncertainty_zvertex30_mbdup, h_uncertainty_zvertex30_mbddown, h_unfold_zvertex30, h_unfold_zvertex30_mbdup, h_unfold_zvertex30_mbddown, "h_uncertainty_zvertex30_mbd");
  TH1D* h_uncertainty_zvertex30_unfoldup, *h_uncertainty_zvertex30_unfolddown; get_unfold_uncertainty(h_uncertainty_zvertex30_unfoldup, h_uncertainty_zvertex30_unfolddown, h_unfold_zvertex30, h_unfold_zvertex30_unfoldunc, "h_uncertainty_zvertex30_unfold");
  std::vector<TH1D*> h_uncertainty_zvertex30_up = {h_uncertainty_zvertex30_jesup, h_uncertainty_zvertex30_jerup, h_uncertainty_zvertex30_jetup, h_uncertainty_zvertex30_mbdup, h_uncertainty_zvertex30_unfoldup};
  std::vector<TH1D*> h_uncertainty_zvertex30_down = {h_uncertainty_zvertex30_jesdown, h_uncertainty_zvertex30_jerdown, h_uncertainty_zvertex30_jetdown, h_uncertainty_zvertex30_mbddown, h_uncertainty_zvertex30_unfolddown};
  TH1D* h_uncertainty_zvertex30_total_up, *h_uncertainty_zvertex30_total_down; get_total_uncertainty(h_uncertainty_zvertex30_total_up, h_uncertainty_zvertex30_total_down, h_uncertainty_zvertex30_stat, h_uncertainty_zvertex30_up, h_uncertainty_zvertex30_down);

  TH1D* h_unfold_zvertex60 = (TH1D*)f_spectrum->Get("h_unfold_zvertex60"); do_normalization(h_unfold_zvertex60, luminosity22_60, h_etacorrection, jet_radius);
  TFile *f_output_check = new TFile(Form("output_final_spectrum_check_r0%d.root", (int)(10*jet_radius)), "RECREATE"); h_unfold_zvertex60->Write(); f_output_check->Close();
  TH1D* h_unfold_zvertex60_jesup = (TH1D*)f_spectrum->Get("h_unfold_zvertex60_jesup"); do_normalization(h_unfold_zvertex60_jesup, luminosity22_60, h_etacorrection, jet_radius);
  TH1D* h_unfold_zvertex60_jesdown = (TH1D*)f_spectrum->Get("h_unfold_zvertex60_jesdown"); do_normalization(h_unfold_zvertex60_jesdown, luminosity22_60, h_etacorrection, jet_radius);
  TH1D* h_unfold_zvertex60_jerup = (TH1D*)f_spectrum->Get("h_unfold_zvertex60_jerup"); do_normalization(h_unfold_zvertex60_jerup, luminosity22_60, h_etacorrection, jet_radius);
  TH1D* h_unfold_zvertex60_jerdown = (TH1D*)f_spectrum->Get("h_unfold_zvertex60_jerdown"); do_normalization(h_unfold_zvertex60_jerdown, luminosity22_60, h_etacorrection, jet_radius);
  TH1D* h_unfold_zvertex60_jetup = (TH1D*)f_spectrum->Get("h_unfold_zvertex60_jetup"); do_normalization(h_unfold_zvertex60_jetup, luminosity22_60, h_etacorrection, jet_radius);
  TH1D* h_unfold_zvertex60_jetdown = (TH1D*)f_spectrum->Get("h_unfold_zvertex60_jetdown"); do_normalization(h_unfold_zvertex60_jetdown, luminosity22_60, h_etacorrection, jet_radius);
  TH1D* h_unfold_zvertex60_mbdup = (TH1D*)f_spectrum->Get("h_unfold_zvertex60_mbdup"); do_normalization(h_unfold_zvertex60_mbdup, luminosity22_60, h_etacorrection, jet_radius);
  TH1D* h_unfold_zvertex60_mbddown = (TH1D*)f_spectrum->Get("h_unfold_zvertex60_mbddown"); do_normalization(h_unfold_zvertex60_mbddown, luminosity22_60, h_etacorrection, jet_radius);
  TH1D* h_unfold_zvertex60_unfoldunc = (TH1D*)f_spectrum->Get("h_unfold_zvertex60_unfoldunc"); do_normalization(h_unfold_zvertex60_unfoldunc, luminosity22_60, h_etacorrection, jet_radius);
  TH1D* h_uncertainty_zvertex60_stat = get_statistical_uncertainty(h_unfold_zvertex60, "h_uncertainty_zvertex60_stat");
  TH1D* h_uncertainty_zvertex60_jesup,* h_uncertainty_zvertex60_jesdown; get_uncertainty(h_uncertainty_zvertex60_jesup, h_uncertainty_zvertex60_jesdown, h_unfold_zvertex60, h_unfold_zvertex60_jesup, h_unfold_zvertex60_jesdown, "h_uncertainty_zvertex60_jes");
  TH1D* h_uncertainty_zvertex60_jerup,* h_uncertainty_zvertex60_jerdown; get_uncertainty(h_uncertainty_zvertex60_jerup, h_uncertainty_zvertex60_jerdown, h_unfold_zvertex60, h_unfold_zvertex60_jerup, h_unfold_zvertex60_jerdown, "h_uncertainty_zvertex60_jer");
  TH1D* h_uncertainty_zvertex60_jetup,* h_uncertainty_zvertex60_jetdown; get_uncertainty(h_uncertainty_zvertex60_jetup, h_uncertainty_zvertex60_jetdown, h_unfold_zvertex60, h_unfold_zvertex60_jetup, h_unfold_zvertex60_jetdown, "h_uncertainty_zvertex60_jet");
  TH1D* h_uncertainty_zvertex60_mbdup,* h_uncertainty_zvertex60_mbddown; get_uncertainty(h_uncertainty_zvertex60_mbdup, h_uncertainty_zvertex60_mbddown, h_unfold_zvertex60, h_unfold_zvertex60_mbdup, h_unfold_zvertex60_mbddown, "h_uncertainty_zvertex60_mbd");
  TH1D* h_uncertainty_zvertex60_unfoldup,* h_uncertainty_zvertex60_unfolddown; get_unfold_uncertainty(h_uncertainty_zvertex60_unfoldup, h_uncertainty_zvertex60_unfolddown, h_unfold_zvertex60, h_unfold_zvertex60_unfoldunc, "h_uncertainty_zvertex60_unfold");
  std::vector<TH1D*> h_uncertainty_zvertex60_up = {h_uncertainty_zvertex60_jesup, h_uncertainty_zvertex60_jerup, h_uncertainty_zvertex60_jetup, h_uncertainty_zvertex60_mbdup, h_uncertainty_zvertex60_unfoldup};
  std::vector<TH1D*> h_uncertainty_zvertex60_down = {h_uncertainty_zvertex60_jesdown, h_uncertainty_zvertex60_jerdown, h_uncertainty_zvertex60_jetdown, h_uncertainty_zvertex60_mbddown, h_uncertainty_zvertex60_unfolddown};
  TH1D* h_uncertainty_zvertex60_total_up, *h_uncertainty_zvertex60_total_down; get_total_uncertainty(h_uncertainty_zvertex60_total_up, h_uncertainty_zvertex60_total_down, h_uncertainty_zvertex60_stat, h_uncertainty_zvertex60_up, h_uncertainty_zvertex60_down);

  std::vector<int> color;
  std::vector<string> legend;

  color = {kYellow+1, kAzure+2, kGreen+2, kMagenta};
  legend = {"Jet Energy Scale", "Jet Energy Resolution", "Jet Trigger Efficiency", "Unfolding"};
  draw_total_uncertainty(h_uncertainty_all_total_up, h_uncertainty_all_total_down, h_uncertainty_all_stat, h_uncertainty_all_up, h_uncertainty_all_down, "all", color, legend);
  color.clear();
  legend.clear();

  color = {kRed, kYellow+1, kAzure+2, kGreen+2, kOrange-2};
  legend = {"Jet Energy Scale", "Jet Energy Resolution", "Jet Trigger Efficiency", "MBD Trigger Efficiency", "Unfolding"};
  draw_total_uncertainty(h_uncertainty_zvertex60_total_up, h_uncertainty_zvertex60_total_down, h_uncertainty_zvertex60_stat, h_uncertainty_zvertex60_up, h_uncertainty_zvertex60_down, "zvertex60", color, legend);
  color.clear();
  legend.clear();

  color = {kRed, kYellow+1, kAzure+2, kGreen+2, kOrange-2};
  legend = {"Jet Energy Scale", "Jet Energy Resolution", "Jet Trigger Efficiency", "MBD Trigger Efficiency", "Unfolding"};
  draw_total_uncertainty(h_uncertainty_zvertex30_total_up, h_uncertainty_zvertex30_total_down, h_uncertainty_zvertex30_stat, h_uncertainty_zvertex30_up, h_uncertainty_zvertex30_down, "zvertex30", color, legend);
  color.clear();
  legend.clear();

  // sPHENIX all result
  double g_sphenix_x[calibnpt], g_sphenix_xerrdown[calibnpt], g_sphenix_xerrup[calibnpt], g_sphenix_y[calibnpt], g_sphenix_yerrdown[calibnpt], g_sphenix_yerrup[calibnpt], g_sphenix_stat[calibnpt];
  for (int ib = 0; ib < calibnpt; ++ib) {
    g_sphenix_x[ib] = (calibptbins[ib] + calibptbins[ib+1]) / 2.;
    g_sphenix_xerrdown[ib] = (calibptbins[ib+1] - calibptbins[ib]) / 2.;
    g_sphenix_xerrup[ib] = (calibptbins[ib+1] - calibptbins[ib]) / 2.;
    g_sphenix_y[ib] = h_unfold_all->GetBinContent(ib + n_underflowbin + 1);
    g_sphenix_yerrdown[ib] = -1 * h_uncertainty_all_total_down->GetBinContent(ib + n_underflowbin + 1) * h_unfold_all->GetBinContent(ib + n_underflowbin + 1);
    g_sphenix_yerrup[ib] = h_uncertainty_all_total_up->GetBinContent(ib + n_underflowbin + 1) * h_unfold_all->GetBinContent(ib + n_underflowbin + 1);
    g_sphenix_stat[ib] = h_uncertainty_all_stat->GetBinContent(ib + n_underflowbin + 1) * h_unfold_all->GetBinContent(ib + n_underflowbin + 1);
    std::cout << "sPHENIX pt bin: " << g_sphenix_x[ib] << "  " << g_sphenix_y[ib] << "  " << g_sphenix_yerrdown[ib] << "  " << g_sphenix_yerrup[ib] << " " << g_sphenix_stat[ib] << std::endl;
  }
  TGraphAsymmErrors* g_sphenix_result = new TGraphAsymmErrors(calibnpt, g_sphenix_x, g_sphenix_y, g_sphenix_xerrdown, g_sphenix_xerrup, g_sphenix_yerrdown, g_sphenix_yerrup);
  g_sphenix_result->SetMarkerStyle(20);
  g_sphenix_result->SetMarkerSize(1.5);
  g_sphenix_result->SetMarkerColor(kAzure+2);
  g_sphenix_result->SetLineColor(kAzure+2);
  g_sphenix_result->SetLineWidth(0);
  g_sphenix_result->SetFillColorAlpha(kAzure + 1, 0.5);
  TGraphErrors* g_sphenix_result_point = new TGraphErrors(calibnpt, g_sphenix_x, g_sphenix_y, g_sphenix_xerrdown, g_sphenix_stat);
  g_sphenix_result_point->SetMarkerStyle(20);
  g_sphenix_result_point->SetMarkerSize(1.5);
  g_sphenix_result_point->SetMarkerColor(kAzure+2);
  g_sphenix_result_point->SetLineColor(kAzure+2);
  g_sphenix_result_point->SetLineWidth(2);

  // sPHENIX 30 zvertex result
  double g_sphenix_zvertex30_x[calibnpt], g_sphenix_zvertex30_xerrdown[calibnpt], g_sphenix_zvertex30_xerrup[calibnpt], g_sphenix_zvertex30_y[calibnpt], g_sphenix_zvertex30_yerrdown[calibnpt], g_sphenix_zvertex30_yerrup[calibnpt], g_sphenix_zvertex30_stat[calibnpt];
  for (int ib = 0; ib < calibnpt; ++ib) {
    g_sphenix_zvertex30_x[ib] = (calibptbins[ib] + calibptbins[ib+1]) / 2.;
    g_sphenix_zvertex30_xerrdown[ib] = (calibptbins[ib+1] - calibptbins[ib]) / 2.;
    g_sphenix_zvertex30_xerrup[ib] = (calibptbins[ib+1] - calibptbins[ib]) / 2.;
    g_sphenix_zvertex30_y[ib] = h_unfold_zvertex30->GetBinContent(ib + n_underflowbin + 1);
    g_sphenix_zvertex30_yerrdown[ib] = -1 * h_uncertainty_zvertex30_total_down->GetBinContent(ib + n_underflowbin + 1) * h_unfold_zvertex30->GetBinContent(ib + n_underflowbin + 1);
    g_sphenix_zvertex30_yerrup[ib] = h_uncertainty_zvertex30_total_up->GetBinContent(ib + n_underflowbin + 1) * h_unfold_zvertex30->GetBinContent(ib + n_underflowbin + 1);
    g_sphenix_zvertex30_stat[ib] = h_uncertainty_zvertex30_stat->GetBinContent(ib + n_underflowbin + 1) * h_unfold_zvertex30->GetBinContent(ib + n_underflowbin + 1);
    std::cout << "sPHENIX zvertex30 pt bin: " << g_sphenix_zvertex30_x[ib] << "  " << g_sphenix_zvertex30_y[ib] << "  " << g_sphenix_zvertex30_yerrdown[ib] << "  " << g_sphenix_zvertex30_yerrup[ib] << " " << g_sphenix_zvertex30_stat[ib] << std::endl;
  }
  TGraphAsymmErrors* g_sphenix_zvertex30_result = new TGraphAsymmErrors(calibnpt, g_sphenix_zvertex30_x, g_sphenix_zvertex30_y, g_sphenix_zvertex30_xerrdown, g_sphenix_zvertex30_xerrup, g_sphenix_zvertex30_yerrdown, g_sphenix_zvertex30_yerrup);
  g_sphenix_zvertex30_result->SetMarkerStyle(20);
  g_sphenix_zvertex30_result->SetMarkerSize(1.5);
  g_sphenix_zvertex30_result->SetMarkerColor(kAzure+2);
  g_sphenix_zvertex30_result->SetLineColor(kAzure+2);
  g_sphenix_zvertex30_result->SetLineWidth(0);
  g_sphenix_zvertex30_result->SetFillColorAlpha(kAzure+1, 0.5);
  TGraphErrors* g_sphenix_zvertex30_result_point = new TGraphErrors(calibnpt, g_sphenix_zvertex30_x, g_sphenix_zvertex30_y, g_sphenix_zvertex30_xerrdown, g_sphenix_zvertex30_stat);
  g_sphenix_zvertex30_result_point->SetMarkerStyle(20);
  g_sphenix_zvertex30_result_point->SetMarkerSize(1.5);
  g_sphenix_zvertex30_result_point->SetMarkerColor(kAzure+2);
  g_sphenix_zvertex30_result_point->SetLineColor(kAzure+2);
  g_sphenix_zvertex30_result_point->SetLineWidth(2); 

  // sPHENIX 60 zvertex result
  double g_sphenix_zvertex60_x[calibnpt], g_sphenix_zvertex60_xerrdown[calibnpt], g_sphenix_zvertex60_xerrup[calibnpt], g_sphenix_zvertex60_y[calibnpt], g_sphenix_zvertex60_yerrdown[calibnpt], g_sphenix_zvertex60_yerrup[calibnpt], g_sphenix_zvertex60_stat[calibnpt];
  for (int ib = 0; ib < calibnpt; ++ib) {
    g_sphenix_zvertex60_x[ib] = (calibptbins[ib] + calibptbins[ib+1]) / 2.;
    g_sphenix_zvertex60_xerrdown[ib] = (calibptbins[ib+1] - calibptbins[ib]) / 2.;
    g_sphenix_zvertex60_xerrup[ib] = (calibptbins[ib+1] - calibptbins[ib]) / 2.;
    g_sphenix_zvertex60_y[ib] = h_unfold_zvertex60->GetBinContent(ib + n_underflowbin + 1);
    g_sphenix_zvertex60_yerrdown[ib] = -1 * h_uncertainty_zvertex60_total_down->GetBinContent(ib + n_underflowbin + 1) * h_unfold_zvertex60->GetBinContent(ib + n_underflowbin + 1);
    g_sphenix_zvertex60_yerrup[ib] = h_uncertainty_zvertex60_total_up->GetBinContent(ib + n_underflowbin + 1) * h_unfold_zvertex60->GetBinContent(ib + n_underflowbin + 1);
    g_sphenix_zvertex60_stat[ib] = h_uncertainty_zvertex60_stat->GetBinContent(ib + n_underflowbin + 1) * h_unfold_zvertex60->GetBinContent(ib + n_underflowbin + 1);
    std::cout << "sPHENIX zvertex60 pt bin: " << g_sphenix_zvertex60_x[ib] << "  " << g_sphenix_zvertex60_y[ib] << "  " << g_sphenix_zvertex60_yerrdown[ib] << "  " << g_sphenix_zvertex60_yerrup[ib] << " " << g_sphenix_zvertex60_stat[ib] << std::endl;
  }
  TGraphAsymmErrors* g_sphenix_zvertex60_result = new TGraphAsymmErrors(calibnpt, g_sphenix_zvertex60_x, g_sphenix_zvertex60_y, g_sphenix_zvertex60_xerrdown, g_sphenix_zvertex60_xerrup, g_sphenix_zvertex60_yerrdown, g_sphenix_zvertex60_yerrup);
  g_sphenix_zvertex60_result->SetMarkerStyle(20);
  g_sphenix_zvertex60_result->SetMarkerSize(1.5);
  g_sphenix_zvertex60_result->SetMarkerColor(kAzure+2);
  g_sphenix_zvertex60_result->SetLineColor(kAzure+2);
  g_sphenix_zvertex60_result->SetLineWidth(0);
  g_sphenix_zvertex60_result->SetFillColorAlpha(kAzure+1, 0.5);
  TGraphErrors* g_sphenix_zvertex60_result_point = new TGraphErrors(calibnpt, g_sphenix_zvertex60_x, g_sphenix_zvertex60_y, g_sphenix_zvertex60_xerrdown, g_sphenix_zvertex60_stat);
  g_sphenix_zvertex60_result_point->SetMarkerStyle(20);
  g_sphenix_zvertex60_result_point->SetMarkerSize(1.5);
  g_sphenix_zvertex60_result_point->SetMarkerColor(kAzure+2);
  g_sphenix_zvertex60_result_point->SetLineColor(kAzure+2);
  g_sphenix_zvertex60_result_point->SetLineWidth(2); 

  // sPHENIX QM result
  double g_sphenix_qm_x[9], g_sphenix_qm_xerrdown[9], g_sphenix_qm_xerrup[9], g_sphenix_qm_y[9], g_sphenix_qm_yerrdown[9], g_sphenix_qm_yerrup[9], g_sphenix_qm_stat[9];
  ifstream file_qm("sphenix_qm_result_point.txt");
  for (int ib = 0; ib < 9; ++ib) {
    file_qm >> g_sphenix_qm_x[ib] >> g_sphenix_qm_xerrdown[ib] >> g_sphenix_qm_xerrup[ib] >> g_sphenix_qm_y[ib] >> g_sphenix_qm_yerrdown[ib] >> g_sphenix_qm_yerrup[ib] >> g_sphenix_qm_stat[ib];
  }
  TGraphAsymmErrors* g_sphenix_qm_result = new TGraphAsymmErrors(9, g_sphenix_qm_x, g_sphenix_qm_y, g_sphenix_qm_xerrdown, g_sphenix_qm_xerrup, g_sphenix_qm_yerrdown, g_sphenix_qm_yerrup);
  g_sphenix_qm_result->SetMarkerStyle(20);
  g_sphenix_qm_result->SetMarkerSize(1.5);
  g_sphenix_qm_result->SetMarkerColor(kRed+1);
  g_sphenix_qm_result->SetLineColor(kRed+1);
  g_sphenix_qm_result->SetLineWidth(0);
  g_sphenix_qm_result->SetFillColorAlpha(kRed-4, 0.5);
  TGraphErrors* g_sphenix_qm_result_point = new TGraphErrors(9, g_sphenix_qm_x, g_sphenix_qm_y, g_sphenix_qm_xerrdown, g_sphenix_qm_stat);
  g_sphenix_qm_result_point->SetMarkerStyle(20);
  g_sphenix_qm_result_point->SetMarkerSize(1.5);
  g_sphenix_qm_result_point->SetMarkerColor(kRed+1);
  g_sphenix_qm_result_point->SetLineColor(kRed+1);
  g_sphenix_qm_result_point->SetLineWidth(2);

  // Pythia result
  TFile *f_sim_specturm = new TFile("output_sim_r04.root", "READ");
  TH1D* h_pythia_spectrum = (TH1D*)f_sim_specturm->Get("h_truthjet_pt_record_all");
  h_pythia_spectrum->SetName("h_pythia_spectrum");
  for (int ib = 1; ib <= h_pythia_spectrum->GetNbinsX(); ++ib) {
    h_pythia_spectrum->SetBinContent(ib, h_pythia_spectrum->GetBinContent(ib)/ (float)(1.4 * h_pythia_spectrum->GetBinWidth(ib)));
    h_pythia_spectrum->SetBinError(ib, h_pythia_spectrum->GetBinError(ib)/ (float)(1.4 * h_pythia_spectrum->GetBinWidth(ib)));
  }
  double g_pythia_x[truthnpt], g_pythia_xerrdown[truthnpt], g_pythia_xerrup[truthnpt], g_pythia_y[truthnpt], g_pythia_yerrdown[truthnpt], g_pythia_yerrup[truthnpt];
  for (int ib = 0; ib < truthnpt; ++ib) {
    g_pythia_x[ib] = (truthptbins[ib] + truthptbins[ib+1]) / 2;
    g_pythia_xerrdown[ib] = (truthptbins[ib+1] - truthptbins[ib]) / 2;
    g_pythia_xerrup[ib] = (truthptbins[ib+1] - truthptbins[ib]) / 2;
    g_pythia_y[ib] = h_pythia_spectrum->GetBinContent(ib+1);
    g_pythia_yerrdown[ib] = h_pythia_spectrum->GetBinError(ib+1);
    g_pythia_yerrup[ib] = h_pythia_spectrum->GetBinError(ib+1);
  }
  TGraphAsymmErrors* g_pythia_result = new TGraphAsymmErrors(truthnpt, g_pythia_x, g_pythia_y, g_pythia_xerrdown, g_pythia_xerrup, g_pythia_yerrdown, g_pythia_yerrup);
  g_pythia_result->SetMarkerStyle(21);
  g_pythia_result->SetMarkerSize(1.5);
  g_pythia_result->SetMarkerColor(kMagenta+2);
  g_pythia_result->SetLineColor(kMagenta+2);

  // Herwig result
  TFile *f_sim_specturm_herwig = new TFile("herwigXsections.root", "READ");
  TH1D* h_herwig_spectrum_04 = (TH1D*)f_sim_specturm_herwig->Get("Herwigxsections");
  double g_herwig_x_04[11], g_herwig_y_04[11], g_herwig_xerrdown_04[11], g_herwig_xerrup_04[11], g_herwig_yerrdown_04[11], g_herwig_yerrup_04[11];
  for (int ib = 0; ib < 11; ++ib) {
    g_herwig_x_04[ib] = h_herwig_spectrum_04->GetBinCenter(ib+1);
    g_herwig_xerrdown_04[ib] = h_herwig_spectrum_04->GetBinWidth(ib+1) / 2;
    g_herwig_xerrup_04[ib] = h_herwig_spectrum_04->GetBinWidth(ib+1) / 2;
    g_herwig_y_04[ib] = h_herwig_spectrum_04->GetBinContent(ib+1);
    g_herwig_yerrdown_04[ib] = h_herwig_spectrum_04->GetBinError(ib+1);
    g_herwig_yerrup_04[ib] = h_herwig_spectrum_04->GetBinError(ib+1);
  }
  TGraphAsymmErrors* g_herwig_result_04 = new TGraphAsymmErrors(11, g_herwig_x_04, g_herwig_y_04, g_herwig_xerrdown_04, g_herwig_xerrup_04, g_herwig_yerrdown_04, g_herwig_yerrup_04);
  g_herwig_result_04->SetMarkerStyle(33);
  g_herwig_result_04->SetMarkerSize(1.5);
  g_herwig_result_04->SetMarkerColor(kGreen+2);
  g_herwig_result_04->SetLineColor(kGreen+2);

  // NLO result
  ifstream myfile1, myfile05, myfile2;
  TGraph *tjet = new TGraph();
  TGraph *tjet05 = new TGraph();
  TGraph *tjet2 = new TGraph();
  double scalefactor = 2.0 * 3.14159;
  myfile1.open("sphenix_nlo/sPhenix-sc1.dat");
  myfile05.open("sphenix_nlo/sPhenix-sc05.dat");
  myfile2.open("sphenix_nlo/sPhenix-sc2.dat");
  double drop1, drop2, drop3, pt1, yield1, pt05, yield05, pt2, yield2;
  std::vector<double> xpoint, ypoint05, ypoint2;
  for (int index=0; index<66; index++) {
    myfile1 >> drop1 >> drop2 >> drop3 >> pt1 >> yield1;
    yield1 = yield1 * scalefactor * pt1;
    tjet->SetPoint(index, pt1, yield1);
    xpoint.push_back(pt1);

    myfile05 >> drop1 >> drop2 >> drop3 >> pt05 >> yield05;
    yield05 = yield05 * scalefactor * pt05;
    tjet05->SetPoint(index, pt05, yield05);
    ypoint05.push_back(yield05);

    myfile2 >> drop1 >> drop2 >> drop3 >> pt2 >> yield2;
    yield2 = yield2 * scalefactor * pt2;
    tjet2->SetPoint(index, pt2, yield2);
    ypoint2.push_back(yield2);
  }
  std::vector <double> xpoint_shadow, ypoint_shadow;
  for (int i = 0; i < xpoint.size(); i++) {
    xpoint_shadow.push_back(xpoint[i]);
    ypoint_shadow.push_back(ypoint05[i]);
  }
  for (int i = xpoint.size(); i > 0; i--) {
    xpoint_shadow.push_back(xpoint[i-1]);
    ypoint_shadow.push_back(ypoint2[i-1]);
  }
  TGraph *tjet_shadow = new TGraph(2*xpoint.size(), &xpoint_shadow[0], &ypoint_shadow[0]);
  tjet_shadow->SetMarkerStyle(20);
  tjet_shadow->SetMarkerSize(1.5);
  tjet_shadow->SetMarkerColor(kRed-4);
  tjet_shadow->SetLineColor(kRed);
  tjet_shadow->SetFillColorAlpha(kRed-4, 0.5);

  myfile1.close();
  myfile05.close();
  myfile2.close();
  tjet->SetMarkerStyle(21);
  tjet->SetMarkerColor(2);
  tjet->SetLineColor(2);
  tjet05->SetMarkerStyle(21);
  tjet05->SetMarkerColor(2);
  tjet05->SetLineColor(2);
  tjet2->SetMarkerStyle(21);
  tjet2->SetMarkerColor(2);
  tjet2->SetLineColor(2);

  // sPHENIX all ratio
  double g_sphenix_ratio_x[calibnpt], g_sphenix_ratio_xerrdown[calibnpt], g_sphenix_ratio_xerrup[calibnpt], g_sphenix_ratio_y[calibnpt], g_sphenix_ratio_yerrdown[calibnpt], g_sphenix_ratio_yerrup[calibnpt], g_sphenix_ratio_yerrforpoint[calibnpt];
  for (int ib = 0; ib < calibnpt; ++ib) {
    g_sphenix_ratio_x[ib] = (calibptbins[ib] + calibptbins[ib+1]) / 2.;
    g_sphenix_ratio_xerrdown[ib] = (calibptbins[ib+1] - calibptbins[ib]) / 2.;
    g_sphenix_ratio_xerrup[ib] = (calibptbins[ib+1] - calibptbins[ib]) / 2.;
    double nlo_y = tjet->Eval(g_sphenix_ratio_x[ib]);
    double nlo_y_up = tjet2->Eval(g_sphenix_ratio_x[ib]) - nlo_y;
    double nlo_y_down = tjet05->Eval(g_sphenix_ratio_x[ib]) - nlo_y;
    g_sphenix_ratio_y[ib] = g_sphenix_y[ib] / nlo_y;
    g_sphenix_ratio_yerrdown[ib] = g_sphenix_ratio_y[ib] * TMath::Sqrt(TMath::Power(g_sphenix_yerrdown[ib]/g_sphenix_y[ib], 2) + TMath::Power(nlo_y_down/nlo_y, 2));
    g_sphenix_ratio_yerrup[ib] = g_sphenix_ratio_y[ib] * TMath::Sqrt(TMath::Power(g_sphenix_yerrup[ib]/g_sphenix_y[ib], 2) + TMath::Power(nlo_y_up/nlo_y, 2));
    g_sphenix_ratio_yerrforpoint[ib] = 0;
  }
  TGraphAsymmErrors* g_sphenix_ratio_all = new TGraphAsymmErrors(calibnpt, g_sphenix_ratio_x, g_sphenix_ratio_y, g_sphenix_ratio_xerrdown, g_sphenix_ratio_xerrup, g_sphenix_ratio_yerrdown, g_sphenix_ratio_yerrup);
  g_sphenix_ratio_all->SetMarkerStyle(20);
  g_sphenix_ratio_all->SetMarkerSize(1.5);
  g_sphenix_ratio_all->SetMarkerColor(kAzure+2);
  g_sphenix_ratio_all->SetLineColor(kAzure+2);
  g_sphenix_ratio_all->SetLineWidth(0);
  g_sphenix_ratio_all->SetFillColorAlpha(kAzure + 1, 0.5);
  TGraphErrors* g_sphenix_ratio_all_point = new TGraphErrors(calibnpt, g_sphenix_ratio_x, g_sphenix_ratio_y, g_sphenix_ratio_xerrdown, g_sphenix_ratio_yerrforpoint);
  g_sphenix_ratio_all_point->SetMarkerStyle(20);
  g_sphenix_ratio_all_point->SetMarkerSize(1.5);
  g_sphenix_ratio_all_point->SetMarkerColor(kAzure+2);
  g_sphenix_ratio_all_point->SetLineColor(kAzure+2);
  g_sphenix_ratio_all_point->SetLineWidth(2);

  // sPHENIX zvertex30 ratio
  double g_sphenix_zvertex30_ratio_x[calibnpt], g_sphenix_zvertex30_ratio_xerrdown[calibnpt], g_sphenix_zvertex30_ratio_xerrup[calibnpt], g_sphenix_zvertex30_ratio_y[calibnpt], g_sphenix_zvertex30_ratio_yerrdown[calibnpt], g_sphenix_zvertex30_ratio_yerrup[calibnpt], g_sphenix_zvertex30_ratio_yerrforpoint[calibnpt];
  for (int ib = 0; ib < calibnpt; ++ib) {
    g_sphenix_zvertex30_ratio_x[ib] = (calibptbins[ib] + calibptbins[ib+1]) / 2.;
    g_sphenix_zvertex30_ratio_xerrdown[ib] = (calibptbins[ib+1] - calibptbins[ib]) / 2.;
    g_sphenix_zvertex30_ratio_xerrup[ib] = (calibptbins[ib+1] - calibptbins[ib]) / 2.;
    double nlo_y = tjet->Eval(g_sphenix_zvertex30_ratio_x[ib]);
    double nlo_y_up = tjet2->Eval(g_sphenix_zvertex30_ratio_x[ib]) - nlo_y;
    double nlo_y_down = tjet05->Eval(g_sphenix_zvertex30_ratio_x[ib]) - nlo_y;
    g_sphenix_zvertex30_ratio_y[ib] = g_sphenix_zvertex30_y[ib] / nlo_y;
    g_sphenix_zvertex30_ratio_yerrdown[ib] = g_sphenix_zvertex30_ratio_y[ib] * TMath::Sqrt(TMath::Power(g_sphenix_zvertex30_yerrdown[ib]/g_sphenix_zvertex30_y[ib], 2) + TMath::Power(nlo_y_down/nlo_y, 2));
    g_sphenix_zvertex30_ratio_yerrup[ib] = g_sphenix_zvertex30_ratio_y[ib] * TMath::Sqrt(TMath::Power(g_sphenix_zvertex30_yerrup[ib]/g_sphenix_zvertex30_y[ib], 2) + TMath::Power(nlo_y_up/nlo_y, 2));
    g_sphenix_zvertex30_ratio_yerrforpoint[ib] = 0;
  }
  TGraphAsymmErrors* g_sphenix_ratio_zvertex30 = new TGraphAsymmErrors(calibnpt, g_sphenix_zvertex30_ratio_x, g_sphenix_zvertex30_ratio_y, g_sphenix_zvertex30_ratio_xerrdown, g_sphenix_zvertex30_ratio_xerrup, g_sphenix_zvertex30_ratio_yerrdown, g_sphenix_zvertex30_ratio_yerrup);
  g_sphenix_ratio_zvertex30->SetMarkerStyle(20);
  g_sphenix_ratio_zvertex30->SetMarkerSize(1.5);
  g_sphenix_ratio_zvertex30->SetMarkerColor(kAzure+2);
  g_sphenix_ratio_zvertex30->SetLineColor(kAzure+2);
  g_sphenix_ratio_zvertex30->SetLineWidth(0);
  g_sphenix_ratio_zvertex30->SetFillColorAlpha(kAzure + 1, 0.5);
  TGraphErrors* g_sphenix_ratio_zvertex30_point = new TGraphErrors(calibnpt, g_sphenix_zvertex30_ratio_x, g_sphenix_zvertex30_ratio_y, g_sphenix_zvertex30_ratio_xerrdown, g_sphenix_zvertex30_ratio_yerrforpoint);
  g_sphenix_ratio_zvertex30_point->SetMarkerStyle(20);
  g_sphenix_ratio_zvertex30_point->SetMarkerSize(1.5);
  g_sphenix_ratio_zvertex30_point->SetMarkerColor(kAzure+2);
  g_sphenix_ratio_zvertex30_point->SetLineColor(kAzure+2);
  g_sphenix_ratio_zvertex30_point->SetLineWidth(2);

  // sPHENIX zvertex60 ratio
  double g_sphenix_zvertex60_ratio_x[calibnpt], g_sphenix_zvertex60_ratio_xerrdown[calibnpt], g_sphenix_zvertex60_ratio_xerrup[calibnpt], g_sphenix_zvertex60_ratio_y[calibnpt], g_sphenix_zvertex60_ratio_yerrdown[calibnpt], g_sphenix_zvertex60_ratio_yerrup[calibnpt], g_sphenix_zvertex60_ratio_yerrforpoint[calibnpt];
  for (int ib = 0; ib < calibnpt; ++ib) {
    g_sphenix_zvertex60_ratio_x[ib] = (calibptbins[ib] + calibptbins[ib+1]) / 2.;
    g_sphenix_zvertex60_ratio_xerrdown[ib] = (calibptbins[ib+1] - calibptbins[ib]) / 2.;
    g_sphenix_zvertex60_ratio_xerrup[ib] = (calibptbins[ib+1] - calibptbins[ib]) / 2.;
    double nlo_y = tjet->Eval(g_sphenix_zvertex60_ratio_x[ib]);
    double nlo_y_up = tjet2->Eval(g_sphenix_zvertex60_ratio_x[ib]) - nlo_y;
    double nlo_y_down = tjet05->Eval(g_sphenix_zvertex60_ratio_x[ib]) - nlo_y;
    g_sphenix_zvertex60_ratio_y[ib] = g_sphenix_zvertex60_y[ib] / nlo_y;
    g_sphenix_zvertex60_ratio_yerrdown[ib] = g_sphenix_zvertex60_ratio_y[ib] * TMath::Sqrt(TMath::Power(g_sphenix_zvertex60_yerrdown[ib]/g_sphenix_zvertex60_y[ib], 2) + TMath::Power(nlo_y_down/nlo_y, 2));
    g_sphenix_zvertex60_ratio_yerrup[ib] = g_sphenix_zvertex60_ratio_y[ib] * TMath::Sqrt(TMath::Power(g_sphenix_zvertex60_yerrup[ib]/g_sphenix_zvertex60_y[ib], 2) + TMath::Power(nlo_y_up/nlo_y, 2));
    g_sphenix_zvertex60_ratio_yerrforpoint[ib] = 0;
  }
  TGraphAsymmErrors* g_sphenix_ratio_zvertex60 = new TGraphAsymmErrors(calibnpt, g_sphenix_zvertex60_ratio_x, g_sphenix_zvertex60_ratio_y, g_sphenix_zvertex60_ratio_xerrdown, g_sphenix_zvertex60_ratio_xerrup, g_sphenix_zvertex60_ratio_yerrdown, g_sphenix_zvertex60_ratio_yerrup);
  g_sphenix_ratio_zvertex60->SetMarkerStyle(20);
  g_sphenix_ratio_zvertex60->SetMarkerSize(1.5);
  g_sphenix_ratio_zvertex60->SetMarkerColor(kAzure+2);
  g_sphenix_ratio_zvertex60->SetLineColor(kAzure+2);
  g_sphenix_ratio_zvertex60->SetLineWidth(0);
  g_sphenix_ratio_zvertex60->SetFillColorAlpha(kAzure + 1, 0.5);
  TGraphErrors* g_sphenix_ratio_zvertex60_point = new TGraphErrors(calibnpt, g_sphenix_zvertex60_ratio_x, g_sphenix_zvertex60_ratio_y, g_sphenix_zvertex60_ratio_xerrdown, g_sphenix_zvertex60_ratio_yerrforpoint);
  g_sphenix_ratio_zvertex60_point->SetMarkerStyle(20);
  g_sphenix_ratio_zvertex60_point->SetMarkerSize(1.5);
  g_sphenix_ratio_zvertex60_point->SetMarkerColor(kAzure+2);
  g_sphenix_ratio_zvertex60_point->SetLineColor(kAzure+2);
  g_sphenix_ratio_zvertex60_point->SetLineWidth(2);


  ////////////// Plotting //////////////
  TCanvas *can_simnlo_shadow_new = new TCanvas("can_simnlo_shadow_new", "", 2550, 2400);
  gStyle->SetPalette(57);
  can_simnlo_shadow_new->SetTopMargin(0.03);
  can_simnlo_shadow_new->SetLeftMargin(0.15);
  can_simnlo_shadow_new->SetBottomMargin(0.12);
  can_simnlo_shadow_new->SetRightMargin(0.05);
  can_simnlo_shadow_new->SetLogy();
  TH2F *frame_simnlo_shadow_new = new TH2F("frame_simnlo_shadow_new", "", 10, calibptbins[1], calibptbins[calibnpt], 10, 1e-05, 1e+05);
  frame_simnlo_shadow_new->GetXaxis()->SetTitle("p_{T}^{jet} [GeV]");
  frame_simnlo_shadow_new->GetYaxis()->SetTitle("d^{2}#sigma/(d#eta dp_{T}) [pb/GeV]");
  frame_simnlo_shadow_new->GetXaxis()->SetTitleOffset(0.98);
  frame_simnlo_shadow_new->GetYaxis()->SetTitleOffset(1.15);
  frame_simnlo_shadow_new->GetXaxis()->SetLabelSize(0.045);
  frame_simnlo_shadow_new->GetYaxis()->SetLabelSize(0.045);
  frame_simnlo_shadow_new->GetXaxis()->CenterTitle();
  frame_simnlo_shadow_new->GetYaxis()->CenterTitle();
  frame_simnlo_shadow_new->Draw();
  g_sphenix_result->Draw("2 same");
  myText(0.6, 0.9, 1, "#bf{#it{sPHENIX}} Internal", 0.04);
  myText(0.6, 0.8, 1, "anti-k_{t} #kern[-0.3]{#it{R}} = 0.4", 0.04);
  myText(0.6, 0.75, 1, "|#eta^{jet}| < 0.7", 0.04);
  TLegend *leg_simnlo_shadow_new = new TLegend(0.15, 0.18, 0.9, 0.33);
  leg_simnlo_shadow_new->SetBorderSize(0);
  leg_simnlo_shadow_new->SetFillStyle(0);
  leg_simnlo_shadow_new->SetTextSize(0.03);
  leg_simnlo_shadow_new->SetTextFont(42);
  g_pythia_result->SetMarkerStyle(54);
  g_herwig_result_04->SetMarkerStyle(56);
  g_pythia_result->SetMarkerSize(4.5);
  g_herwig_result_04->SetMarkerSize(6.5);
  g_sphenix_result->SetMarkerSize(4.5);
  g_sphenix_result_point->SetMarkerSize(4.5);
  leg_simnlo_shadow_new->AddEntry(g_sphenix_result, "sPHENIX Run 2024 data", "pf");
  leg_simnlo_shadow_new->AddEntry(g_pythia_result, "PYTHIA8 truth jet spectrum", "pl");
  leg_simnlo_shadow_new->AddEntry(g_herwig_result_04, "HERWIG truth jet spectrum", "pl");
  leg_simnlo_shadow_new->AddEntry(tjet_shadow, "NLO pQCD (No Hadronization)", "f");
  leg_simnlo_shadow_new->AddEntry(tjet_shadow, "Werner Vogelsang", "");
  leg_simnlo_shadow_new->Draw();
  g_sphenix_result->Draw("P same");
  g_sphenix_result_point->Draw("P same");
  g_pythia_result->Draw("p same");
  g_herwig_result_04->Draw("p same");
  tjet_shadow->Draw("F same");
  can_simnlo_shadow_new->SaveAs("figure/new_spectrum_calib_simnlocompare_shadow.png");
  gStyle->SetPalette(55);
  g_pythia_result->SetMarkerSize(1.5);
  g_sphenix_result->SetMarkerSize(1.5);
  g_sphenix_result_point->SetMarkerSize(1.5);


  TCanvas *can_qm_compare = new TCanvas("can_qm_compare", "", 2550, 2400);
  gStyle->SetPalette(57);
  can_qm_compare->SetTopMargin(0.03);
  can_qm_compare->SetLeftMargin(0.15);
  can_qm_compare->SetBottomMargin(0.12);
  can_qm_compare->SetRightMargin(0.05);
  can_qm_compare->SetLogy();
  TH2F *frame_qm_compare = new TH2F("frame_qm_compare", "", 10, calibptbins[1], calibptbins[calibnpt], 10, 1e-05, 1e+05);
  frame_qm_compare->GetXaxis()->SetTitle("p_{T}^{jet} [GeV]");
  frame_qm_compare->GetYaxis()->SetTitle("d^{2}#sigma/(d#eta dp_{T}) [pb/GeV]");
  frame_qm_compare->GetXaxis()->SetTitleOffset(0.98);
  frame_qm_compare->GetYaxis()->SetTitleOffset(1.15);
  frame_qm_compare->GetXaxis()->SetLabelSize(0.045);
  frame_qm_compare->GetYaxis()->SetLabelSize(0.045);
  frame_qm_compare->GetXaxis()->CenterTitle();
  frame_qm_compare->GetYaxis()->CenterTitle();
  frame_qm_compare->Draw();
  g_sphenix_result->Draw("2 same");
  g_sphenix_qm_result->Draw("2 same");
  myText(0.6, 0.9, 1, "#bf{#it{sPHENIX}} Internal", 0.04);
  myText(0.6, 0.8, 1, "anti-k_{t} #kern[-0.3]{#it{R}} = 0.4", 0.04);
  myText(0.6, 0.75, 1, "|#eta^{jet}| < 0.7", 0.04);
  TLegend *leg_qm_compare = new TLegend(0.15, 0.18, 0.9, 0.33);
  leg_qm_compare->SetBorderSize(0);
  leg_qm_compare->SetFillStyle(0);
  leg_qm_compare->SetTextSize(0.03);
  leg_qm_compare->SetTextFont(42);
  g_sphenix_result->SetMarkerSize(4.5);
  g_sphenix_result_point->SetMarkerSize(4.5);
  g_sphenix_qm_result->SetMarkerSize(4.5);
  g_sphenix_qm_result_point->SetMarkerSize(4.5);
  leg_qm_compare->AddEntry(g_sphenix_result, "sPHENIX Run 2024 data", "pf");
  leg_qm_compare->AddEntry(g_sphenix_qm_result, "sPHENIX Run 2024 data (QM)", "pf");
  //leg_qm_compare->AddEntry(tjet_shadow, "NLO pQCD (No Hadronization)", "f");
  //leg_qm_compare->AddEntry(tjet_shadow, "Werner Vogelsang", "");
  leg_qm_compare->Draw();
  g_sphenix_result_point->Draw("P same");
  g_sphenix_qm_result_point->Draw("p same");
  //tjet_shadow->Draw("F same");
  can_qm_compare->SaveAs("figure/new_spectrum_calib_qm_compare.png");
  gStyle->SetPalette(55);
  g_sphenix_result->SetMarkerSize(1.5);
  g_sphenix_result_point->SetMarkerSize(1.5);





  TCanvas *can_zvertex30_result = new TCanvas("can_zvertex30_result", "", 2550, 2400);
  gStyle->SetPalette(57);
  can_zvertex30_result->SetTopMargin(0.03);
  can_zvertex30_result->SetLeftMargin(0.15);
  can_zvertex30_result->SetBottomMargin(0.12);
  can_zvertex30_result->SetRightMargin(0.05);
  can_zvertex30_result->SetLogy();
  TH2F *frame_zvertex30_result = new TH2F("frame_zvertex30_result", "", 10, calibptbins[1], calibptbins[calibnpt], 10, 1e-05, 1e+05);
  frame_zvertex30_result->GetXaxis()->SetTitle("p_{T}^{jet} [GeV]");
  frame_zvertex30_result->GetYaxis()->SetTitle("d^{2}#sigma/(d#eta dp_{T}) [pb/GeV]");
  frame_zvertex30_result->GetXaxis()->SetTitleOffset(0.98);
  frame_zvertex30_result->GetYaxis()->SetTitleOffset(1.15);
  frame_zvertex30_result->GetXaxis()->SetLabelSize(0.045);
  frame_zvertex30_result->GetYaxis()->SetLabelSize(0.045);
  frame_zvertex30_result->GetXaxis()->CenterTitle();
  frame_zvertex30_result->GetYaxis()->CenterTitle();
  frame_zvertex30_result->Draw();
  g_sphenix_zvertex30_result->Draw("2 same");
  myText(0.6, 0.9, 1, "#bf{#it{sPHENIX}} Internal", 0.04);
  myText(0.6, 0.8, 1, "anti-k_{t} #kern[-0.3]{#it{R}} = 0.4", 0.04);
  myText(0.6, 0.75, 1, "|#eta^{jet}| < 0.7", 0.04);
  TLegend *leg_zvertex30_result = new TLegend(0.15, 0.18, 0.9, 0.33);
  leg_zvertex30_result->SetBorderSize(0);
  leg_zvertex30_result->SetFillStyle(0);
  leg_zvertex30_result->SetTextSize(0.03);
  leg_zvertex30_result->SetTextFont(42);
  g_pythia_result->SetMarkerStyle(54);
  g_herwig_result_04->SetMarkerStyle(56);
  g_pythia_result->SetMarkerSize(4.5);
  g_herwig_result_04->SetMarkerSize(6.5);
  g_sphenix_zvertex30_result->SetMarkerSize(4.5);
  g_sphenix_zvertex30_result_point->SetMarkerSize(4.5);
  leg_zvertex30_result->AddEntry(g_sphenix_zvertex30_result, "sPHENIX Run 2024 data", "pf");
  leg_zvertex30_result->AddEntry(g_pythia_result, "PYTHIA8 truth jet spectrum", "pl");
  leg_zvertex30_result->AddEntry(g_herwig_result_04, "HERWIG truth jet spectrum", "pl");
  leg_zvertex30_result->AddEntry(tjet_shadow, "NLO pQCD (No Hadronization)", "f");
  leg_zvertex30_result->AddEntry(tjet_shadow, "Werner Vogelsang", "");
  leg_zvertex30_result->Draw();
  g_sphenix_zvertex30_result->Draw("P same");
  g_sphenix_zvertex30_result_point->Draw("P same");
  g_pythia_result->Draw("p same");
  g_herwig_result_04->Draw("p same");
  tjet_shadow->Draw("F same");
  can_zvertex30_result->SaveAs("figure/new_spectrum_zvertex30_result.png");
  gStyle->SetPalette(55);
  g_pythia_result->SetMarkerSize(1.5);
  g_sphenix_result->SetMarkerSize(1.5);
  g_sphenix_result_point->SetMarkerSize(1.5);


  TCanvas *can_zvertex30_qm_compare = new TCanvas("can_zvertex30_qm_compare", "", 2550, 2400);
  gStyle->SetPalette(57);
  can_zvertex30_qm_compare->SetTopMargin(0.03);
  can_zvertex30_qm_compare->SetLeftMargin(0.15);
  can_zvertex30_qm_compare->SetBottomMargin(0.12);
  can_zvertex30_qm_compare->SetRightMargin(0.05);
  can_zvertex30_qm_compare->SetLogy();
  TH2F *frame_zvertex30_qm_compare = new TH2F("frame_zvertex30_qm_compare", "", 10, calibptbins[1], calibptbins[calibnpt], 10, 1e-05, 1e+05);
  frame_zvertex30_qm_compare->GetXaxis()->SetTitle("p_{T}^{jet} [GeV]");
  frame_zvertex30_qm_compare->GetYaxis()->SetTitle("d^{2}#sigma/(d#eta dp_{T}) [pb/GeV]");
  frame_zvertex30_qm_compare->GetXaxis()->SetTitleOffset(0.98);
  frame_zvertex30_qm_compare->GetYaxis()->SetTitleOffset(1.15);
  frame_zvertex30_qm_compare->GetXaxis()->SetLabelSize(0.045);
  frame_zvertex30_qm_compare->GetYaxis()->SetLabelSize(0.045);
  frame_zvertex30_qm_compare->GetXaxis()->CenterTitle();
  frame_zvertex30_qm_compare->GetYaxis()->CenterTitle();
  frame_zvertex30_qm_compare->Draw();
  g_sphenix_zvertex30_result->Draw("2 same");
  g_sphenix_qm_result->Draw("2 same");
  myText(0.6, 0.9, 1, "#bf{#it{sPHENIX}} Internal", 0.04);
  myText(0.6, 0.8, 1, "anti-k_{t} #kern[-0.3]{#it{R}} = 0.4", 0.04);
  myText(0.6, 0.75, 1, "|#eta^{jet}| < 0.7", 0.04);
  TLegend *leg_zvertex30_qm_compare = new TLegend(0.15, 0.18, 0.9, 0.33);
  leg_zvertex30_qm_compare->SetBorderSize(0);
  leg_zvertex30_qm_compare->SetFillStyle(0);
  leg_zvertex30_qm_compare->SetTextSize(0.03);
  leg_zvertex30_qm_compare->SetTextFont(42);
  g_sphenix_zvertex30_result->SetMarkerSize(4.5);
  g_sphenix_zvertex30_result_point->SetMarkerSize(4.5);
  g_sphenix_qm_result->SetMarkerSize(4.5);
  g_sphenix_qm_result_point->SetMarkerSize(4.5);
  leg_zvertex30_qm_compare->AddEntry(g_sphenix_zvertex30_result, "sPHENIX Run 2024 data", "pf");
  leg_zvertex30_qm_compare->AddEntry(g_sphenix_qm_result, "sPHENIX Run 2024 data (QM)", "pf");
  //leg_zvertex30_qm_compare->AddEntry(tjet_shadow, "NLO pQCD (No Hadronization)", "f");
  //leg_zvertex30_qm_compare->AddEntry(tjet_shadow, "Werner Vogelsang", "");
  leg_zvertex30_qm_compare->Draw();
  g_sphenix_zvertex30_result_point->Draw("P same");
  g_sphenix_qm_result_point->Draw("p same");
  //tjet_shadow->Draw("F same");
  can_zvertex30_qm_compare->SaveAs("figure/new_spectrum_zvertex30_qm_compare.png");
  gStyle->SetPalette(55);
  g_sphenix_zvertex30_result->SetMarkerSize(1.5);
  g_sphenix_zvertex30_result_point->SetMarkerSize(1.5);





  TCanvas *can_zvertex60_result = new TCanvas("can_zvertex60_result", "", 2550, 2400);
  gStyle->SetPalette(57);
  can_zvertex60_result->SetTopMargin(0.03);
  can_zvertex60_result->SetLeftMargin(0.15);
  can_zvertex60_result->SetBottomMargin(0.12);
  can_zvertex60_result->SetRightMargin(0.05);
  can_zvertex60_result->SetLogy();
  TH2F *frame_zvertex60_result = new TH2F("frame_zvertex60_result", "", 10, calibptbins[1], calibptbins[calibnpt], 10, 1e-05, 1e+05);
  frame_zvertex60_result->GetXaxis()->SetTitle("p_{T}^{jet} [GeV]");
  frame_zvertex60_result->GetYaxis()->SetTitle("d^{2}#sigma/(d#eta dp_{T}) [pb/GeV]");
  frame_zvertex60_result->GetXaxis()->SetTitleOffset(0.98);
  frame_zvertex60_result->GetYaxis()->SetTitleOffset(1.15);
  frame_zvertex60_result->GetXaxis()->SetLabelSize(0.045);
  frame_zvertex60_result->GetYaxis()->SetLabelSize(0.045);
  frame_zvertex60_result->GetXaxis()->CenterTitle();
  frame_zvertex60_result->GetYaxis()->CenterTitle();
  frame_zvertex60_result->Draw();
  g_sphenix_zvertex60_result->Draw("2 same");
  myText(0.6, 0.9, 1, "#bf{#it{sPHENIX}} Internal", 0.04);
  myText(0.6, 0.8, 1, "anti-k_{t} #kern[-0.3]{#it{R}} = 0.4", 0.04);
  myText(0.6, 0.75, 1, "|#eta^{jet}| < 0.7", 0.04);
  TLegend *leg_zvertex60_result = new TLegend(0.15, 0.18, 0.9, 0.33);
  leg_zvertex60_result->SetBorderSize(0);
  leg_zvertex60_result->SetFillStyle(0);
  leg_zvertex60_result->SetTextSize(0.03);
  leg_zvertex60_result->SetTextFont(42);
  g_pythia_result->SetMarkerStyle(54);
  g_herwig_result_04->SetMarkerStyle(56);
  g_pythia_result->SetMarkerSize(4.5);
  g_herwig_result_04->SetMarkerSize(6.5);
  g_sphenix_zvertex60_result->SetMarkerSize(4.5);
  g_sphenix_zvertex60_result_point->SetMarkerSize(4.5);
  leg_zvertex60_result->AddEntry(g_sphenix_zvertex60_result, "sPHENIX Run 2024 data", "pf");
  leg_zvertex60_result->AddEntry(g_pythia_result, "PYTHIA8 truth jet spectrum", "pl");
  leg_zvertex60_result->AddEntry(g_herwig_result_04, "HERWIG truth jet spectrum", "pl");
  leg_zvertex60_result->AddEntry(tjet_shadow, "NLO pQCD (No Hadronization)", "f");
  leg_zvertex60_result->AddEntry(tjet_shadow, "Werner Vogelsang", "");
  leg_zvertex60_result->Draw();
  g_sphenix_zvertex60_result->Draw("P same");
  g_sphenix_zvertex60_result_point->Draw("P same");
  g_pythia_result->Draw("p same");
  g_herwig_result_04->Draw("p same");
  tjet_shadow->Draw("F same");
  can_zvertex60_result->SaveAs("figure/new_spectrum_zvertex60_result.png");
  gStyle->SetPalette(55);
  g_pythia_result->SetMarkerSize(1.5);
  g_sphenix_result->SetMarkerSize(1.5);
  g_sphenix_result_point->SetMarkerSize(1.5);


  TCanvas *can_zvertex60_qm_compare = new TCanvas("can_zvertex60_qm_compare", "", 2550, 2400);
  gStyle->SetPalette(57);
  can_zvertex60_qm_compare->SetTopMargin(0.03);
  can_zvertex60_qm_compare->SetLeftMargin(0.15);
  can_zvertex60_qm_compare->SetBottomMargin(0.12);
  can_zvertex60_qm_compare->SetRightMargin(0.05);
  can_zvertex60_qm_compare->SetLogy();
  TH2F *frame_zvertex60_qm_compare = new TH2F("frame_zvertex60_qm_compare", "", 10, calibptbins[1], calibptbins[calibnpt], 10, 1e-05, 1e+05);
  frame_zvertex60_qm_compare->GetXaxis()->SetTitle("p_{T}^{jet} [GeV]");
  frame_zvertex60_qm_compare->GetYaxis()->SetTitle("d^{2}#sigma/(d#eta dp_{T}) [pb/GeV]");
  frame_zvertex60_qm_compare->GetXaxis()->SetTitleOffset(0.98);
  frame_zvertex60_qm_compare->GetYaxis()->SetTitleOffset(1.15);
  frame_zvertex60_qm_compare->GetXaxis()->SetLabelSize(0.045);
  frame_zvertex60_qm_compare->GetYaxis()->SetLabelSize(0.045);
  frame_zvertex60_qm_compare->GetXaxis()->CenterTitle();
  frame_zvertex60_qm_compare->GetYaxis()->CenterTitle();
  frame_zvertex60_qm_compare->Draw();
  g_sphenix_zvertex60_result->Draw("2 same");
  g_sphenix_qm_result->Draw("2 same");
  myText(0.6, 0.9, 1, "#bf{#it{sPHENIX}} Internal", 0.04);
  myText(0.6, 0.8, 1, "anti-k_{t} #kern[-0.3]{#it{R}} = 0.4", 0.04);
  myText(0.6, 0.75, 1, "|#eta^{jet}| < 0.7", 0.04);
  TLegend *leg_zvertex60_qm_compare = new TLegend(0.15, 0.18, 0.9, 0.33);
  leg_zvertex60_qm_compare->SetBorderSize(0);
  leg_zvertex60_qm_compare->SetFillStyle(0);
  leg_zvertex60_qm_compare->SetTextSize(0.03);
  leg_zvertex60_qm_compare->SetTextFont(42);
  g_sphenix_zvertex60_result->SetMarkerSize(4.5);
  g_sphenix_zvertex60_result_point->SetMarkerSize(4.5);
  g_sphenix_qm_result->SetMarkerSize(4.5);
  g_sphenix_qm_result_point->SetMarkerSize(4.5);
  leg_zvertex60_qm_compare->AddEntry(g_sphenix_zvertex60_result, "sPHENIX Run 2024 data", "pf");
  leg_zvertex60_qm_compare->AddEntry(g_sphenix_qm_result, "sPHENIX Run 2024 data (QM)", "pf");
  //leg_zvertex60_qm_compare->AddEntry(tjet_shadow, "NLO pQCD (No Hadronization)", "f");
  //leg_zvertex60_qm_compare->AddEntry(tjet_shadow, "Werner Vogelsang", "");
  leg_zvertex60_qm_compare->Draw();
  g_sphenix_zvertex60_result_point->Draw("P same");
  g_sphenix_qm_result_point->Draw("p same");
  //tjet_shadow->Draw("F same");
  can_zvertex60_qm_compare->SaveAs("figure/new_spectrum_zvertex60_qm_compare.png");
  gStyle->SetPalette(55);
  g_sphenix_zvertex60_result->SetMarkerSize(1.5);
  g_sphenix_zvertex60_result_point->SetMarkerSize(1.5);


  // Draw ratio plot
  TCanvas *can_ratio_all = new TCanvas("can_ratio_all", "", 850, 1091);
  can_ratio_all->Divide(1, 2);
  gStyle->SetPalette(57);
  TPad *pad_1_all = (TPad*)can_ratio_all->cd(1);
  pad_1_all->SetPad(0, 0.33333, 1, 1);
  pad_1_all->SetTopMargin(0.03);
  pad_1_all->SetLeftMargin(0.15);
  pad_1_all->SetBottomMargin(0.035);
  pad_1_all->SetRightMargin(0.05);
  pad_1_all->SetLogy();
  TH2F *frame_ratio_all = new TH2F("frame_ratio_all", "", 10, calibptbins[1], calibptbins[calibnpt], 10, 1e-05, 1e+05);
  frame_ratio_all->GetXaxis()->SetTitle("p_{T}^{jet} [GeV]");
  frame_ratio_all->GetYaxis()->SetTitle("d^{2}#sigma/(d#eta dp_{T}) [pb/GeV]");
  frame_ratio_all->GetXaxis()->SetTitleOffset(0.98);
  frame_ratio_all->GetYaxis()->SetTitleOffset(1.15);
  frame_ratio_all->GetXaxis()->SetLabelSize(0.045);
  frame_ratio_all->GetYaxis()->SetLabelSize(0.045);
  frame_ratio_all->GetXaxis()->SetLabelOffset(2);
  frame_ratio_all->GetXaxis()->CenterTitle();
  frame_ratio_all->GetYaxis()->CenterTitle();
  frame_ratio_all->Draw();
  myText(0.6, 0.9, 1, "#bf{#it{sPHENIX}} Internal", 0.04);
  myText(0.6, 0.8, 1, "anti-k_{t} #kern[-0.3]{#it{R}} = 0.4", 0.04);
  myText(0.6, 0.75, 1, "|#eta^{jet}| < 0.7", 0.04);
  TLegend *leg_ratio_all = new TLegend(0.15, 0.18, 0.9, 0.33);
  leg_ratio_all->SetBorderSize(0);
  leg_ratio_all->SetFillStyle(0);
  leg_ratio_all->SetTextSize(0.03);
  leg_ratio_all->SetTextFont(42);
  leg_ratio_all->AddEntry(g_sphenix_result, "sPHENIX Run 2024 data", "pf");
  leg_ratio_all->AddEntry(tjet_shadow, "NLO pQCD (No Hadronization)", "f");
  leg_ratio_all->AddEntry(tjet_shadow, "Werner Vogelsang", "");
  leg_ratio_all->Draw();
  g_sphenix_result->Draw("2 same");
  g_sphenix_result->Draw("P same");
  g_sphenix_result_point->Draw("P same");
  tjet_shadow->Draw("F same");
  TPad *pad_2_all = (TPad*)can_ratio_all->cd(2);
  pad_2_all->SetPad(0, 0, 1, 0.333333);
  pad_2_all->SetTopMargin(0.02);
  pad_2_all->SetLeftMargin(0.15);
  pad_2_all->SetBottomMargin(0.25);
  pad_2_all->SetRightMargin(0.05);
  TH2F *frame_ratio_all2 = new TH2F("frame_ratio_all2", "", 10, calibptbins[1], calibptbins[calibnpt], 2, 0., 1.1);
  frame_ratio_all2->GetXaxis()->SetTitle("p_{T}^{jet} [GeV]");
  frame_ratio_all2->GetYaxis()->SetTitle("Ratio");
  frame_ratio_all2->GetXaxis()->SetTitleOffset(0.98);
  frame_ratio_all2->GetYaxis()->SetTitleOffset(0.5);
  frame_ratio_all2->GetXaxis()->SetLabelSize(0.045);
  frame_ratio_all2->GetYaxis()->SetLabelSize(0.045);
  frame_ratio_all2->GetXaxis()->CenterTitle();
  frame_ratio_all2->GetYaxis()->CenterTitle();
  frame_ratio_all2->GetYaxis()->SetTitleOffset(frame_ratio_all->GetYaxis()->GetTitleOffset()*1/2.);
  frame_ratio_all2->GetYaxis()->SetLabelOffset(frame_ratio_all->GetYaxis()->GetLabelOffset()*1/2.);
  frame_ratio_all2->GetXaxis()->SetLabelSize(frame_ratio_all->GetXaxis()->GetLabelSize()*2);
  frame_ratio_all2->GetYaxis()->SetLabelSize(frame_ratio_all->GetYaxis()->GetLabelSize()*2);
  frame_ratio_all2->GetXaxis()->SetTitleSize(frame_ratio_all->GetXaxis()->GetTitleSize()*2);
  frame_ratio_all2->GetYaxis()->SetTitleSize(frame_ratio_all->GetYaxis()->GetTitleSize()*2);
  frame_ratio_all2->Draw();
  g_sphenix_ratio_all->Draw("2 same");
  g_sphenix_ratio_all->Draw("P same");
  g_sphenix_ratio_all_point->Draw("P same");
  TLine *line_ratio_all = new TLine(calibptbins[1], 1, calibptbins[calibnpt], 1);
  line_ratio_all->SetLineStyle(3);
  line_ratio_all->Draw("same");
  TLine *line2_ratio_all = new TLine(calibptbins[1], 0.25, calibptbins[calibnpt], 0.25);
  line2_ratio_all->SetLineStyle(3);
  line2_ratio_all->Draw("same");
  can_ratio_all->SaveAs("figure/new_spectrum_ratio_all.png");

  TCanvas *can_ratio_zvertex30 = new TCanvas("can_ratio_zvertex30", "", 850, 1091);
  can_ratio_zvertex30->Divide(1, 2);
  gStyle->SetPalette(57);
  TPad *pad_1_zvertex30 = (TPad*)can_ratio_zvertex30->cd(1);
  pad_1_zvertex30->SetPad(0, 0.33333, 1, 1);
  pad_1_zvertex30->SetTopMargin(0.03);
  pad_1_zvertex30->SetLeftMargin(0.15);
  pad_1_zvertex30->SetBottomMargin(0.035);
  pad_1_zvertex30->SetRightMargin(0.05);
  pad_1_zvertex30->SetLogy();
  TH2F *frame_ratio_zvertex30 = new TH2F("frame_ratio_zvertex30", "", 10, calibptbins[1], calibptbins[calibnpt], 10, 1e-05, 1e+05);
  frame_ratio_zvertex30->GetXaxis()->SetTitle("p_{T}^{jet} [GeV]");
  frame_ratio_zvertex30->GetYaxis()->SetTitle("d^{2}#sigma/(d#eta dp_{T}) [pb/GeV]");
  frame_ratio_zvertex30->GetXaxis()->SetTitleOffset(0.98);
  frame_ratio_zvertex30->GetYaxis()->SetTitleOffset(1.15);
  frame_ratio_zvertex30->GetXaxis()->SetLabelSize(0.045);
  frame_ratio_zvertex30->GetYaxis()->SetLabelSize(0.045);
  frame_ratio_zvertex30->GetXaxis()->SetLabelOffset(2);
  frame_ratio_zvertex30->GetXaxis()->CenterTitle();
  frame_ratio_zvertex30->GetYaxis()->CenterTitle();
  frame_ratio_zvertex30->Draw();
  myText(0.6, 0.9, 1, "#bf{#it{sPHENIX}} Internal", 0.04);
  myText(0.6, 0.8, 1, "anti-k_{t} #kern[-0.3]{#it{R}} = 0.4", 0.04);
  myText(0.6, 0.75, 1, "|#eta^{jet}| < 0.7", 0.04);
  TLegend *leg_ratio_zvertex30 = new TLegend(0.15, 0.18, 0.9, 0.33);
  leg_ratio_zvertex30->SetBorderSize(0);
  leg_ratio_zvertex30->SetFillStyle(0);
  leg_ratio_zvertex30->SetTextSize(0.03);
  leg_ratio_zvertex30->SetTextFont(42);
  leg_ratio_zvertex30->AddEntry(g_sphenix_zvertex30_result, "sPHENIX Run 2024 data", "pf");
  leg_ratio_zvertex30->AddEntry(tjet_shadow, "NLO pQCD (No Hadronization)", "f");
  leg_ratio_zvertex30->AddEntry(tjet_shadow, "Werner Vogelsang", "");
  leg_ratio_zvertex30->Draw();
  g_sphenix_zvertex30_result->Draw("2 same");
  g_sphenix_zvertex30_result->Draw("P same");
  g_sphenix_zvertex30_result_point->Draw("P same");
  tjet_shadow->Draw("F same");
  TPad *pad_2_zvertex30 = (TPad*)can_ratio_zvertex30->cd(2);
  pad_2_zvertex30->SetPad(0, 0, 1, 0.333333);
  pad_2_zvertex30->SetTopMargin(0.02);
  pad_2_zvertex30->SetLeftMargin(0.15);
  pad_2_zvertex30->SetBottomMargin(0.25);
  pad_2_zvertex30->SetRightMargin(0.05);
  TH2F *frame_ratio_zvertex302 = new TH2F("frame_ratio_zvertex302", "", 10, calibptbins[1], calibptbins[calibnpt], 2, 0., 1.1);
  frame_ratio_zvertex302->GetXaxis()->SetTitle("p_{T}^{jet} [GeV]");
  frame_ratio_zvertex302->GetYaxis()->SetTitle("Ratio");
  frame_ratio_zvertex302->GetXaxis()->SetTitleOffset(0.98);
  frame_ratio_zvertex302->GetYaxis()->SetTitleOffset(0.5);
  frame_ratio_zvertex302->GetXaxis()->SetLabelSize(0.045);
  frame_ratio_zvertex302->GetYaxis()->SetLabelSize(0.045);
  frame_ratio_zvertex302->GetXaxis()->CenterTitle();
  frame_ratio_zvertex302->GetYaxis()->CenterTitle();
  frame_ratio_zvertex302->GetYaxis()->SetTitleOffset(frame_ratio_zvertex30->GetYaxis()->GetTitleOffset()*1/2.);
  frame_ratio_zvertex302->GetYaxis()->SetLabelOffset(frame_ratio_zvertex30->GetYaxis()->GetLabelOffset()*1/2.);
  frame_ratio_zvertex302->GetXaxis()->SetLabelSize(frame_ratio_zvertex30->GetXaxis()->GetLabelSize()*2);
  frame_ratio_zvertex302->GetYaxis()->SetLabelSize(frame_ratio_zvertex30->GetYaxis()->GetLabelSize()*2);
  frame_ratio_zvertex302->GetXaxis()->SetTitleSize(frame_ratio_zvertex30->GetXaxis()->GetTitleSize()*2);
  frame_ratio_zvertex302->GetYaxis()->SetTitleSize(frame_ratio_zvertex30->GetYaxis()->GetTitleSize()*2);
  frame_ratio_zvertex302->Draw();
  g_sphenix_ratio_zvertex30->Draw("2 same");
  g_sphenix_ratio_zvertex30->Draw("P same");
  g_sphenix_ratio_zvertex30_point->Draw("P same");
  TLine *line_ratio_zvertex30 = new TLine(calibptbins[1], 1, calibptbins[calibnpt], 1);
  line_ratio_zvertex30->SetLineStyle(3);
  line_ratio_zvertex30->Draw("same");
  TLine *line2_ratio_zvertex30 = new TLine(calibptbins[1], 0.25, calibptbins[calibnpt], 0.25);
  line2_ratio_zvertex30->SetLineStyle(3);
  line2_ratio_zvertex30->Draw("same");
  can_ratio_zvertex30->SaveAs("figure/new_spectrum_ratio_zvertex30.png");

  TCanvas *can_ratio_zvertex60 = new TCanvas("can_ratio_zvertex60", "", 850, 1091);
  can_ratio_zvertex60->Divide(1, 2);
  gStyle->SetPalette(57);
  TPad *pad_1_zvertex60 = (TPad*)can_ratio_zvertex60->cd(1);
  pad_1_zvertex60->SetPad(0, 0.33333, 1, 1);
  pad_1_zvertex60->SetTopMargin(0.03);
  pad_1_zvertex60->SetLeftMargin(0.15);
  pad_1_zvertex60->SetBottomMargin(0.035);
  pad_1_zvertex60->SetRightMargin(0.05);
  pad_1_zvertex60->SetLogy();
  TH2F *frame_ratio_zvertex60 = new TH2F("frame_ratio_zvertex60", "", 10, calibptbins[1], calibptbins[calibnpt], 10, 1e-05, 1e+05);
  frame_ratio_zvertex60->GetXaxis()->SetTitle("p_{T}^{jet} [GeV]");
  frame_ratio_zvertex60->GetYaxis()->SetTitle("d^{2}#sigma/(d#eta dp_{T}) [pb/GeV]");
  frame_ratio_zvertex60->GetXaxis()->SetTitleOffset(0.98);
  frame_ratio_zvertex60->GetYaxis()->SetTitleOffset(1.15);
  frame_ratio_zvertex60->GetXaxis()->SetLabelSize(0.045);
  frame_ratio_zvertex60->GetYaxis()->SetLabelSize(0.045);
  frame_ratio_zvertex60->GetXaxis()->SetLabelOffset(2);
  frame_ratio_zvertex60->GetXaxis()->CenterTitle();
  frame_ratio_zvertex60->GetYaxis()->CenterTitle();
  frame_ratio_zvertex60->Draw();
  myText(0.6, 0.9, 1, "#bf{#it{sPHENIX}} Internal", 0.04);
  myText(0.6, 0.8, 1, "anti-k_{t} #kern[-0.3]{#it{R}} = 0.4", 0.04);
  myText(0.6, 0.75, 1, "|#eta^{jet}| < 0.7", 0.04);
  TLegend *leg_ratio_zvertex60 = new TLegend(0.15, 0.18, 0.9, 0.33);
  leg_ratio_zvertex60->SetBorderSize(0);
  leg_ratio_zvertex60->SetFillStyle(0);
  leg_ratio_zvertex60->SetTextSize(0.03);
  leg_ratio_zvertex60->SetTextFont(42);
  leg_ratio_zvertex60->AddEntry(g_sphenix_zvertex60_result, "sPHENIX Run 2024 data", "pf");
  leg_ratio_zvertex60->AddEntry(tjet_shadow, "NLO pQCD (No Hadronization)", "f");
  leg_ratio_zvertex60->AddEntry(tjet_shadow, "Werner Vogelsang", "");
  leg_ratio_zvertex60->Draw();
  g_sphenix_zvertex60_result->Draw("2 same");
  g_sphenix_zvertex60_result->Draw("P same");
  g_sphenix_zvertex60_result_point->Draw("P same");
  tjet_shadow->Draw("F same");
  TPad *pad_2_zvertex60 = (TPad*)can_ratio_zvertex60->cd(2);
  pad_2_zvertex60->SetPad(0, 0, 1, 0.333333);
  pad_2_zvertex60->SetTopMargin(0.02);
  pad_2_zvertex60->SetLeftMargin(0.15);
  pad_2_zvertex60->SetBottomMargin(0.25);
  pad_2_zvertex60->SetRightMargin(0.05);
  TH2F *frame_ratio_zvertex602 = new TH2F("frame_ratio_zvertex602", "", 10, calibptbins[1], calibptbins[calibnpt], 2, 0., 1.1);
  frame_ratio_zvertex602->GetXaxis()->SetTitle("p_{T}^{jet} [GeV]");
  frame_ratio_zvertex602->GetYaxis()->SetTitle("Ratio");
  frame_ratio_zvertex602->GetXaxis()->SetTitleOffset(0.98);
  frame_ratio_zvertex602->GetYaxis()->SetTitleOffset(0.5);
  frame_ratio_zvertex602->GetXaxis()->SetLabelSize(0.045);
  frame_ratio_zvertex602->GetYaxis()->SetLabelSize(0.045);
  frame_ratio_zvertex602->GetXaxis()->CenterTitle();
  frame_ratio_zvertex602->GetYaxis()->CenterTitle();
  frame_ratio_zvertex602->GetYaxis()->SetTitleOffset(frame_ratio_zvertex60->GetYaxis()->GetTitleOffset()*1/2.);
  frame_ratio_zvertex602->GetYaxis()->SetLabelOffset(frame_ratio_zvertex60->GetYaxis()->GetLabelOffset()*1/2.);
  frame_ratio_zvertex602->GetXaxis()->SetLabelSize(frame_ratio_zvertex60->GetXaxis()->GetLabelSize()*2);
  frame_ratio_zvertex602->GetYaxis()->SetLabelSize(frame_ratio_zvertex60->GetYaxis()->GetLabelSize()*2);
  frame_ratio_zvertex602->GetXaxis()->SetTitleSize(frame_ratio_zvertex60->GetXaxis()->GetTitleSize()*2);
  frame_ratio_zvertex602->GetYaxis()->SetTitleSize(frame_ratio_zvertex60->GetYaxis()->GetTitleSize()*2);
  frame_ratio_zvertex602->Draw();
  g_sphenix_ratio_zvertex60->Draw("2 same");
  g_sphenix_ratio_zvertex60->Draw("P same");
  g_sphenix_ratio_zvertex60_point->Draw("P same");
  TLine *line_ratio_zvertex60 = new TLine(calibptbins[1], 1, calibptbins[calibnpt], 1);
  line_ratio_zvertex60->SetLineStyle(3);
  line_ratio_zvertex60->Draw("same");
  TLine *line2_ratio_zvertex60 = new TLine(calibptbins[1], 0.25, calibptbins[calibnpt], 0.25);
  line2_ratio_zvertex60->SetLineStyle(3);
  line2_ratio_zvertex60->Draw("same");
  can_ratio_zvertex60->SaveAs("figure/new_spectrum_ratio_zvertex60.png");
}