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

void do_normalization(TH1D* h_unfolded, double luminosity, TH1D* h_etacorrection, float jet_radius);
TH1D* get_statistical_uncertainty(TH1D* h_unfold, string name);
void get_uncertainty(TH1D* &h_uncertainty_up, TH1D* &h_uncertainty_down, TH1D* h_nominal, TH1D* h_shiftup, TH1D* h_shiftdown, string name);
void get_unfold_uncertainty(TH1D* &h_uncertainty_up, TH1D* &h_uncertainty_down, TH1D* h_nominal, TH1D* h_shift, string name);
void get_total_uncertainty(TH1D* &h_total_up, TH1D* &h_total_down,TH1D* h_statistical, std::vector<TH1D*> h_systematic_up, std::vector<TH1D*> h_systematic_down);
void draw_total_uncertainty(TH1D* h_total_up, TH1D* h_total_down, TH1D* h_statistical, std::vector<TH1D*> h_systematic_up, std::vector<TH1D*> h_systematic_down, std::vector<int> color, std::vector<string> legend, float jet_radius);

void get_finalspectrum(float jet_radius = 0.4) {
  // Read files.
  ifstream f_luminosity("luminosity.txt");
  if (!f_luminosity.is_open()) {
    std::cout << "Failed to open luminosity file!" << std::endl;
    return;
  }
  double luminosity22;
  f_luminosity >> luminosity22;
  std::cout << "Luminosity: " << luminosity22 << std::endl;

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

  TH1D* h_unfold_nominal = (TH1D*)f_spectrum->Get("h_unfold_nominal"); do_normalization(h_unfold_nominal, luminosity22, h_etacorrection, jet_radius);
  TH1D* h_unfold_jesup = (TH1D*)f_spectrum->Get("h_unfold_jesup"); do_normalization(h_unfold_jesup, luminosity22, h_etacorrection, jet_radius);
  TH1D* h_unfold_jesdown = (TH1D*)f_spectrum->Get("h_unfold_jesdown"); do_normalization(h_unfold_jesdown, luminosity22, h_etacorrection, jet_radius);
  TH1D* h_unfold_jerup = (TH1D*)f_spectrum->Get("h_unfold_jerup"); do_normalization(h_unfold_jerup, luminosity22, h_etacorrection, jet_radius);
  TH1D* h_unfold_jerdown = (TH1D*)f_spectrum->Get("h_unfold_jerdown"); do_normalization(h_unfold_jerdown, luminosity22, h_etacorrection, jet_radius);
  TH1D* h_unfold_jettrigup = (TH1D*)f_spectrum->Get("h_unfold_jettrigup"); do_normalization(h_unfold_jettrigup, luminosity22, h_etacorrection, jet_radius);
  TH1D* h_unfold_jettrigdown = (TH1D*)f_spectrum->Get("h_unfold_jettrigdown"); do_normalization(h_unfold_jettrigdown, luminosity22, h_etacorrection, jet_radius);
  TH1D* h_unfold_jettimingup = (TH1D*)f_spectrum->Get("h_unfold_jettimingup"); do_normalization(h_unfold_jettimingup, luminosity22, h_etacorrection, jet_radius);
  TH1D* h_unfold_jettimingdown = (TH1D*)f_spectrum->Get("h_unfold_jettimingdown"); do_normalization(h_unfold_jettimingdown, luminosity22, h_etacorrection, jet_radius);
  TH1D* h_unfold_unfolditerup = (TH1D*)f_spectrum->Get("h_unfold_unfolditerup"); do_normalization(h_unfold_unfolditerup, luminosity22, h_etacorrection, jet_radius);
  TH1D* h_unfold_unfolditerdown = (TH1D*)f_spectrum->Get("h_unfold_unfolditerdown"); do_normalization(h_unfold_unfolditerdown, luminosity22, h_etacorrection, jet_radius);
  TH1D* h_uncertainty_stat = get_statistical_uncertainty(h_unfold_nominal, "h_uncertainty_stat");
  TH1D* h_uncertainty_jesup,* h_uncertainty_jesdown; get_uncertainty(h_uncertainty_jesup, h_uncertainty_jesdown, h_unfold_nominal, h_unfold_jesup, h_unfold_jesdown, "h_uncertainty_jes");
  TH1D* h_uncertainty_jerup,* h_uncertainty_jerdown; get_uncertainty(h_uncertainty_jerup, h_uncertainty_jerdown, h_unfold_nominal, h_unfold_jerup, h_unfold_jerdown, "h_uncertainty_jer");
  TH1D* h_uncertainty_jettrigup,* h_uncertainty_jettrigdown; get_uncertainty(h_uncertainty_jettrigup, h_uncertainty_jettrigdown, h_unfold_nominal, h_unfold_jettrigup, h_unfold_jettrigdown, "h_uncertainty_jettrig");
  TH1D* h_uncertainty_jettimingup,* h_uncertainty_jettimingdown; get_uncertainty(h_uncertainty_jettimingup, h_uncertainty_jettimingdown, h_unfold_nominal, h_unfold_jettimingup, h_unfold_jettimingdown, "h_uncertainty_jettiming");
  TH1D* h_uncertainty_unfolditerup,* h_uncertainty_unfolditerdown; get_uncertainty(h_uncertainty_unfolditerup, h_uncertainty_unfolditerdown, h_unfold_nominal, h_unfold_unfolditerup, h_unfold_unfolditerdown, "h_uncertainty_unfolditer");
  std::vector<TH1D*> h_uncertainty_up = {h_uncertainty_jesup, h_uncertainty_jerup, h_uncertainty_jettrigup, h_uncertainty_jettimingup, h_uncertainty_unfolditerup};
  std::vector<TH1D*> h_uncertainty_down = {h_uncertainty_jesdown, h_uncertainty_jerdown, h_uncertainty_jettrigdown, h_uncertainty_jettimingdown, h_uncertainty_unfolditerdown};
  TH1D* h_uncertainty_total_up, *h_uncertainty_total_down; get_total_uncertainty(h_uncertainty_total_up, h_uncertainty_total_down, h_uncertainty_stat, h_uncertainty_up, h_uncertainty_down);

  std::vector<int> color;
  std::vector<string> legend;
  color = {kRed, kYellow+1, kAzure+2, kGreen+2, kOrange-2, kMagenta};
  legend = {"Jet Energy Scale", "Jet Energy Resolution", "Jet Trigger Efficiency", "Jet Timing Cut Efficiency", "Unfolding Iterations"};
  draw_total_uncertainty(h_uncertainty_total_up, h_uncertainty_total_down, h_uncertainty_stat, h_uncertainty_up, h_uncertainty_down, color, legend, jet_radius);
  color.clear();
  legend.clear();

  // sPHENIX result
  double g_sphenix_x[truthnpt], g_sphenix_xerrdown[truthnpt], g_sphenix_xerrup[truthnpt], g_sphenix_y[truthnpt], g_sphenix_yerrdown[truthnpt], g_sphenix_yerrup[truthnpt], g_sphenix_stat[truthnpt];
  for (int ib = 0; ib < truthnpt; ++ib) {
    g_sphenix_x[ib] = (truthptbins[ib] + truthptbins[ib+1]) / 2.;
    g_sphenix_xerrdown[ib] = (truthptbins[ib+1] - truthptbins[ib]) / 2.;
    g_sphenix_xerrup[ib] = (truthptbins[ib+1] - truthptbins[ib]) / 2.;
    g_sphenix_y[ib] = h_unfold_nominal->GetBinContent(ib + 1);
    g_sphenix_yerrdown[ib] = -1 * h_uncertainty_total_down->GetBinContent(ib + 1) * h_unfold_nominal->GetBinContent(ib + 1);
    g_sphenix_yerrup[ib] = h_uncertainty_total_up->GetBinContent(ib + 1) * h_unfold_nominal->GetBinContent(ib + 1);
    g_sphenix_stat[ib] = h_uncertainty_stat->GetBinContent(ib + 1) * h_unfold_nominal->GetBinContent(ib + 1);
    std::cout << "sPHENIX pt bin: " << g_sphenix_x[ib] << "  " << g_sphenix_y[ib] << "  " << g_sphenix_yerrdown[ib] << "  " << g_sphenix_yerrup[ib] << " " << g_sphenix_stat[ib] << std::endl;
  }
  TGraphAsymmErrors* g_sphenix_result = new TGraphAsymmErrors(truthnpt, g_sphenix_x, g_sphenix_y, g_sphenix_xerrdown, g_sphenix_xerrup, g_sphenix_yerrdown, g_sphenix_yerrup);
  g_sphenix_result->SetMarkerStyle(20);
  g_sphenix_result->SetMarkerSize(1.5);
  g_sphenix_result->SetMarkerColor(kAzure+2);
  g_sphenix_result->SetLineColor(kAzure+2);
  g_sphenix_result->SetLineWidth(0);
  g_sphenix_result->SetFillColorAlpha(kAzure + 1, 0.5);
  TGraphErrors* g_sphenix_result_point = new TGraphErrors(truthnpt, g_sphenix_x, g_sphenix_y, g_sphenix_xerrdown, g_sphenix_stat);
  g_sphenix_result_point->SetMarkerStyle(20);
  g_sphenix_result_point->SetMarkerSize(1.5);
  g_sphenix_result_point->SetMarkerColor(kAzure+2);
  g_sphenix_result_point->SetLineColor(kAzure+2);
  g_sphenix_result_point->SetLineWidth(2);

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
  TH1D* h_pythia_spectrum = (TH1D*)f_sim_specturm->Get("h_truth_nominal");
  h_pythia_spectrum->Multiply(h_etacorrection);
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

  TCanvas *can_cross_section = new TCanvas("can_cross_section", "", 2550, 2400);
  gStyle->SetPalette(57);
  can_cross_section->SetTopMargin(0.03);
  can_cross_section->SetLeftMargin(0.15);
  can_cross_section->SetBottomMargin(0.12);
  can_cross_section->SetRightMargin(0.05);
  can_cross_section->SetLogy();
  TH2F *frame_cross_section = new TH2F("frame_cross_section", "", 10, 15, 75, 10, 1e-05, 1e+05);
  frame_cross_section->GetXaxis()->SetTitle("p_{T}^{jet} [GeV]");
  frame_cross_section->GetYaxis()->SetTitle("d^{2}#sigma/(d#eta dp_{T}) [pb/GeV]");
  frame_cross_section->GetXaxis()->SetTitleOffset(0.98);
  frame_cross_section->GetYaxis()->SetTitleOffset(1.15);
  frame_cross_section->GetXaxis()->SetLabelSize(0.045);
  frame_cross_section->GetYaxis()->SetLabelSize(0.045);
  frame_cross_section->GetXaxis()->CenterTitle();
  frame_cross_section->GetYaxis()->CenterTitle();
  frame_cross_section->Draw();
  g_sphenix_result->Draw("2 same");
  myText(0.6, 0.9, 1, "#bf{#it{sPHENIX}} Internal", 0.04);
  myText(0.6, 0.8, 1, "anti-k_{t} #kern[-0.3]{#it{R}} = 0.4", 0.04);
  myText(0.6, 0.75, 1, "|#eta^{jet}| < 0.7", 0.04);
  TLegend *leg_cross_section = new TLegend(0.15, 0.18, 0.9, 0.33);
  leg_cross_section->SetBorderSize(0);
  leg_cross_section->SetFillStyle(0);
  leg_cross_section->SetTextSize(0.03);
  leg_cross_section->SetTextFont(42);
  g_pythia_result->SetMarkerStyle(54);
  g_herwig_result_04->SetMarkerStyle(56);
  g_pythia_result->SetMarkerSize(4.5);
  g_herwig_result_04->SetMarkerSize(6.5);
  g_sphenix_result->SetMarkerSize(4.5);
  g_sphenix_result_point->SetMarkerSize(4.5);
  leg_cross_section->AddEntry(g_sphenix_result, "sPHENIX Run 2024 data", "pf");
  leg_cross_section->AddEntry(g_pythia_result, "PYTHIA8 truth jet spectrum", "pl");
  leg_cross_section->AddEntry(g_herwig_result_04, "HERWIG truth jet spectrum", "pl");
  leg_cross_section->AddEntry(tjet_shadow, "NLO pQCD (No Hadronization)", "f");
  leg_cross_section->AddEntry(tjet_shadow, "Werner Vogelsang", "");
  leg_cross_section->Draw();
  g_sphenix_result->Draw("P same");
  g_sphenix_result_point->Draw("P same");
  g_pythia_result->Draw("p same");
  g_herwig_result_04->Draw("p same");
  tjet_shadow->Draw("F same");
  can_cross_section->SaveAs(Form("figure/cross_section_jet0%d.png", (int)(10*jet_radius)));
}

void do_normalization(TH1D* h_unfolded, double luminosity, TH1D* h_etacorrection, float jet_radius) {
  h_unfolded->Multiply(h_etacorrection);
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
    if (h_nominal->GetBinContent(i) != 0) {
      double diff_up = (h_shiftup->GetBinContent(i) - h_nominal->GetBinContent(i)) / (double)h_nominal->GetBinContent(i);
      if (diff_up >= 0) up = TMath::Sqrt(TMath::Power(diff_up, 2) + TMath::Power(up, 2));
      else down = TMath::Sqrt(TMath::Power(diff_up, 2) + TMath::Power(down, 2));
      double diff_down = (h_shiftdown->GetBinContent(i) - h_nominal->GetBinContent(i)) / (double)h_nominal->GetBinContent(i);
      if (diff_down >= 0) up = TMath::Sqrt(TMath::Power(diff_down, 2) + TMath::Power(up, 2));
      else down = TMath::Sqrt(TMath::Power(diff_down, 2) + TMath::Power(down, 2));
    }
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

void draw_total_uncertainty(TH1D* h_total_up, TH1D* h_total_down, TH1D* h_statistical, std::vector<TH1D*> h_systematic_up, std::vector<TH1D*> h_systematic_down, std::vector<int> color, std::vector<string> legend, float jet_radius) {
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
  TH2F *frame_unc = new TH2F("frame_unc", "", 10, 15, 75, 2, -1.8, 2.5);
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
  TLine *line = new TLine(15, 0, 75, 0);
  line->SetLineStyle(10);
  line->Draw("same");
  can_unc->SaveAs(Form("figure/uncertainty_jet0%d.png", (int)(10*jet_radius)));

  delete can_unc;
  delete frame_unc;
}