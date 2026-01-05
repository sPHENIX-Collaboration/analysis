#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TMath.h>
#include <iostream>
#include "unfold_Def.h"
#include "/sphenix/user/hanpuj/plotstyle/AtlasStyle.C"
#include "/sphenix/user/hanpuj/plotstyle/AtlasUtils.C"

void draw_1D_multiple_plot_ratio(std::vector<TH1F*> h_input, std::vector<int> color, std::vector<int> markerstyle,
                                 bool do_rebin, int rebin_factor, bool do_normalize,
                                 bool set_xrange, float xlow, float xhigh, bool set_logx,
                                 bool set_yrange, float ylow, float yhigh, bool set_logy,
                                 bool set_ratioyrange, float ratiolow, float ratiohigh, TF1* fit_func,
                                 bool set_title, std::string xtitle, std::string ytitle, std::string ratio_title, bool do_binomial,
                                 bool set_text, std::vector<std::string> text, float xstart, float ystart, float size,
                                 bool set_legend, std::vector<std::string> legend, float xstart_legend, float ystart_legend, float size_legend,
                                 std::string output_name) {
  if (h_input.size() <= 1) {
    std::cout << "Error: no enough input histograms for draw_1D_multiple_plot_ratio" << std::endl;
    return;
  }
  std::vector<TH1F*> h;
  for (int i = 0; i < h_input.size(); ++i) {
    h.push_back((TH1F*)h_input[i]->Clone(Form("h_%d", i)));
  }
  TCanvas *can = new TCanvas("can", "", 800, 963);
  can->Divide(1, 2);
  gStyle->SetPalette(57);
  TPad *pad_1 = (TPad*)can->cd(1);
  pad_1->SetPad(0, 0.4, 1, 1);
  pad_1->SetTopMargin(0.03);
  pad_1->SetLeftMargin(0.15);
  pad_1->SetBottomMargin(0.035);
  pad_1->SetRightMargin(0.08);
  for (int i = 0; i < h.size(); ++i) {
    h.at(i)->SetMarkerStyle(markerstyle.at(i));
    h.at(i)->SetMarkerColor(color.at(i));
    h.at(i)->SetLineColor(color.at(i));
    if (do_rebin) h.at(i)->Rebin(rebin_factor);
    if (do_normalize) {
      for (int ib = 1; ib <= h.at(i)->GetNbinsX(); ++ib) {
        h.at(i)->SetBinContent(ib, h.at(i)->GetBinContent(ib)/h.at(i)->GetBinWidth(ib));
        h.at(i)->SetBinError(ib, h.at(i)->GetBinError(ib)/h.at(i)->GetBinWidth(ib));
      }
    }
  }
  if (set_xrange) h.at(0)->GetXaxis()->SetRangeUser(xlow, xhigh);
  if (set_logx) pad_1->SetLogx();
  if (set_yrange) h.at(0)->GetYaxis()->SetRangeUser(ylow, yhigh);
  if (set_logy) pad_1->SetLogy();
  if (set_title) {
    h.at(0)->GetXaxis()->SetTitle(xtitle.c_str());
    h.at(0)->GetYaxis()->SetTitle(ytitle.c_str());
  }
  h.at(0)->GetXaxis()->SetTitleSize(0.065);
  h.at(0)->GetYaxis()->SetTitleSize(0.065);
  h.at(0)->GetXaxis()->SetTitleOffset(1.04);
  h.at(0)->GetYaxis()->SetTitleOffset(1.06);
  h.at(0)->GetXaxis()->SetLabelSize(0.055);
  h.at(0)->GetYaxis()->SetLabelSize(0.055);
  h.at(0)->GetXaxis()->SetLabelOffset(2);
  h.at(0)->GetXaxis()->CenterTitle();
  h.at(0)->GetYaxis()->CenterTitle();

  h.at(0)->Draw();
  for (int i = 0; i < h.size(); ++i) {
    if (i == 0) continue;
    h.at(i)->Draw("same");
  }

  if (set_text) {
    for (int i = 0; i < text.size(); i++) {
      myText(xstart, ystart-i*(size+0.01), 1, text[i].c_str(), size);
    }
  }
  if (set_legend) {
    for (int i = 0; i < legend.size(); i++) {
      myMarkerLineText(xstart_legend, ystart_legend-i*(size+0.01), 1, color.at(i), markerstyle.at(i), color.at(i), 1, legend.at(i).c_str(), size_legend, true);
    }
  }

  TPad *pad_2 = (TPad*)can->cd(2);
  pad_2->SetPad(0, 0, 1, 0.4);
  pad_2->SetTopMargin(0.03);
  pad_2->SetLeftMargin(0.15);
  pad_2->SetBottomMargin(0.25);
  pad_2->SetRightMargin(0.08);
  std::vector<TH1F*> h_ratio;
  for (int i = 0; i < h.size()-1; ++i) {
    TH1F *h_temp = (TH1F*)h.at(i+1)->Clone(Form("h_ratio_%d", i));
    if (do_binomial) h_temp->Divide(h.at(i+1), h.at(0), 1, 1, "B");
    else h_temp->Divide(h.at(0));
    h_ratio.push_back(h_temp);
    h_ratio.at(i)->SetMarkerStyle(markerstyle.at(i+1));
    h_ratio.at(i)->SetMarkerColor(color.at(i+1));
    h_ratio.at(i)->SetLineColor(color.at(i+1));
  }
  if (set_title) {
    h_ratio.at(0)->GetXaxis()->SetTitle(xtitle.c_str());
    h_ratio.at(0)->GetYaxis()->SetTitle(ratio_title.c_str());
  } else {
    h_ratio.at(0)->GetXaxis()->SetTitle(h.at(0)->GetXaxis()->GetTitle());
    h_ratio.at(0)->GetYaxis()->SetTitle("Ratio");
  }
  if (set_xrange) h_ratio.at(0)->GetXaxis()->SetRangeUser(xlow, xhigh);
  if (set_ratioyrange) h_ratio.at(0)->GetYaxis()->SetRangeUser(ratiolow, ratiohigh);
  h_ratio.at(0)->GetXaxis()->CenterTitle();
  h_ratio.at(0)->GetYaxis()->CenterTitle();
  h_ratio.at(0)->GetXaxis()->SetTitleOffset(0.95);
  h_ratio.at(0)->GetYaxis()->SetTitleOffset(h.at(0)->GetYaxis()->GetTitleOffset()*4/6.);
  //h_ratio.at(0)->GetYaxis()->SetLabelOffset(h.at(0)->GetYaxis()->GetLabelOffset()*4/6.);
  h_ratio.at(0)->GetXaxis()->SetLabelSize(h.at(0)->GetXaxis()->GetLabelSize()*6/4.);
  h_ratio.at(0)->GetYaxis()->SetLabelSize(h.at(0)->GetYaxis()->GetLabelSize()*6/4.);
  h_ratio.at(0)->GetXaxis()->SetTitleSize(h.at(0)->GetXaxis()->GetTitleSize()*6/4.);
  h_ratio.at(0)->GetYaxis()->SetTitleSize(h.at(0)->GetYaxis()->GetTitleSize()*6/4.);
  h_ratio.at(0)->Draw();
  for (int i = 0; i < h_ratio.size(); ++i) {
    if (i == 0) continue;
    h_ratio.at(i)->Draw("same");
  }
  fit_func->SetLineColor(kRed);
  fit_func->Draw("same");

  TLine *line;
  if (set_xrange) line = new TLine(xlow, 1, xhigh, 1);
  else line = new TLine(h.at(0)->GetXaxis()->GetBinLowEdge(1), 1, h.at(0)->GetXaxis()->GetBinUpEdge(h.at(0)->GetNbinsX()), 1);
  line->SetLineColor(kBlack);
  line->SetLineStyle(3);
  line->Draw("same");

  can->SaveAs(output_name.c_str());
  delete can;
  delete line;
}

void get_reweight_hist(TFile* f_out, TH1D*& h_reweight, std::string h_reweightname, TH1D* h_data, TH1D* h_sim) { 
  h_data->Rebin(10); h_data->Scale(1.0 / h_data->Integral());
  h_sim->Rebin(10); h_sim->Scale(1.0 / h_sim->Integral());
  h_reweight = (TH1D*)h_data->Clone(h_reweightname.c_str());
  h_reweight->Divide(h_sim);
  h_reweight->SetName(h_reweightname.c_str());

  std::string prefix_to_remove = "h_reweight_";
  std::string new_prefix = "reweightfunc_";
  std::string plot_name = new_prefix + h_reweightname.substr(prefix_to_remove.length());

  TF1* func_reweight = new TF1(plot_name.c_str(), "[0]*TMath::Exp(-[1]*x) + [2]", 0, 200);
  func_reweight->SetParameters(1., 0.1, 0.5);
  h_reweight->Fit(func_reweight, "", "", 15, 70);

  std::vector<TH1F*> h_input;
  std::vector<int> color;
  std::vector<int> markerstyle;
  std::vector<std::string> text;
  std::vector<std::string> legend;

  h_input.push_back((TH1F*)h_sim);
  h_input.push_back((TH1F*)h_data);
  color.push_back(kRed);
  color.push_back(kBlack);
  markerstyle.push_back(24);
  markerstyle.push_back(24);
  text.push_back("#bf{#it{sPHENIX}} Internal");
  text.push_back("Data & PYTHIA8 p+p#sqrt{s} = 200 GeV");
  text.push_back("anti-k_{t} #kern[-0.5]{#it{R}} = 0.4");
  legend.push_back("Simulation jet spectrum");
  legend.push_back("Data jet spectrum");
  draw_1D_multiple_plot_ratio(h_input, color, markerstyle,
                              false, 10, true,
                              true, calibptbins[0], calibptbins[calibnpt], false,
                              false, 0, 0.5, true,
                              true, 0., 2., func_reweight,
                              true, "p_{T}^{jet} [GeV]", "Arbitrary Unit", "Reweight factor", 0,
                              true, text, 0.4, 0.9, 0.05,
                              true, legend, 0.25, 0.2, 0.05,
                              Form("figure_reweight/%s.png", plot_name.c_str()));
  h_input.clear();
  color.clear();
  markerstyle.clear();
  text.clear();
  legend.clear();

  f_out->cd();
  h_reweight->Write();
  func_reweight->Write();
}

void get_reweighthist(int radius_index = 4) {

  SetAtlasStyle();
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);

  // Read Files
  TFile* f_data = new TFile(Form("output_data_r0%d.root", radius_index), "READ");
  TFile* f_sim = new TFile(Form("output_sim_r0%d.root", radius_index), "READ");
  TFile* f_out = new TFile(Form("output_reweightfunction_r0%d.root", radius_index), "RECREATE");

  // Read data histograms for reweighting
  TH1D* h_calibjet_pt_all = (TH1D*)f_data->Get("h_calibjet_pt_record_all");
  TH1D* h_calibjet_pt_all_jesup = (TH1D*)h_calibjet_pt_all->Clone("h_calibjet_pt_record_all_jesup");
  TH1D* h_calibjet_pt_all_jesdown = (TH1D*)h_calibjet_pt_all->Clone("h_calibjet_pt_record_all_jesdown");
  TH1D* h_calibjet_pt_all_jerup = (TH1D*)h_calibjet_pt_all->Clone("h_calibjet_pt_record_all_jerup");
  TH1D* h_calibjet_pt_all_jerdown = (TH1D*)h_calibjet_pt_all->Clone("h_calibjet_pt_record_all_jerdown");
  TH1D* h_calibjet_pt_all_jetup = (TH1D*)f_data->Get("h_calibjet_pt_record_all_jetup");
  TH1D* h_calibjet_pt_all_jetdown = (TH1D*)f_data->Get("h_calibjet_pt_record_all_jetdown");

  TH1D* h_calibjet_pt_zvertex30 = (TH1D*)f_data->Get("h_calibjet_pt_record_zvertex30");
  TH1D* h_calibjet_pt_zvertex30_jesup = (TH1D*)h_calibjet_pt_zvertex30->Clone("h_calibjet_pt_record_zvertex30_jesup");
  TH1D* h_calibjet_pt_zvertex30_jesdown = (TH1D*)h_calibjet_pt_zvertex30->Clone("h_calibjet_pt_record_zvertex30_jesdown");
  TH1D* h_calibjet_pt_zvertex30_jerup = (TH1D*)h_calibjet_pt_zvertex30->Clone("h_calibjet_pt_record_zvertex30_jerup");
  TH1D* h_calibjet_pt_zvertex30_jerdown = (TH1D*)h_calibjet_pt_zvertex30->Clone("h_calibjet_pt_record_zvertex30_jerdown");
  TH1D* h_calibjet_pt_zvertex30_jetup = (TH1D*)f_data->Get("h_calibjet_pt_record_zvertex30_jetup");
  TH1D* h_calibjet_pt_zvertex30_jetdown = (TH1D*)f_data->Get("h_calibjet_pt_record_zvertex30_jetdown");
  TH1D* h_calibjet_pt_zvertex30_mbdup = (TH1D*)f_data->Get("h_calibjet_pt_record_zvertex30_mbdup");
  TH1D* h_calibjet_pt_zvertex30_mbddown = (TH1D*)f_data->Get("h_calibjet_pt_record_zvertex30_mbddown");

  TH1D* h_calibjet_pt_zvertex60 = (TH1D*)f_data->Get("h_calibjet_pt_record_zvertex60");
  TH1D* h_calibjet_pt_zvertex60_jesup = (TH1D*)h_calibjet_pt_zvertex60->Clone("h_calibjet_pt_record_zvertex60_jesup");
  TH1D* h_calibjet_pt_zvertex60_jesdown = (TH1D*)h_calibjet_pt_zvertex60->Clone("h_calibjet_pt_record_zvertex60_jesdown");
  TH1D* h_calibjet_pt_zvertex60_jerup = (TH1D*)h_calibjet_pt_zvertex60->Clone("h_calibjet_pt_record_zvertex60_jerup");
  TH1D* h_calibjet_pt_zvertex60_jerdown = (TH1D*)h_calibjet_pt_zvertex60->Clone("h_calibjet_pt_record_zvertex60_jerdown");
  TH1D* h_calibjet_pt_zvertex60_jetup = (TH1D*)f_data->Get("h_calibjet_pt_record_zvertex60_jetup");
  TH1D* h_calibjet_pt_zvertex60_jetdown = (TH1D*)f_data->Get("h_calibjet_pt_record_zvertex60_jetdown");
  TH1D* h_calibjet_pt_zvertex60_mbdup = (TH1D*)f_data->Get("h_calibjet_pt_record_zvertex60_mbdup");
  TH1D* h_calibjet_pt_zvertex60_mbddown = (TH1D*)f_data->Get("h_calibjet_pt_record_zvertex60_mbddown");

  // Read sim histograms for reweighting
  TH1D* h_measure_unweighted_all = (TH1D*)f_sim->Get("h_measure_unweighted_all");
  TH1D* h_measure_unweighted_all_jesup = (TH1D*)f_sim->Get("h_measure_unweighted_all_jesup");
  TH1D* h_measure_unweighted_all_jesdown = (TH1D*)f_sim->Get("h_measure_unweighted_all_jesdown");
  TH1D* h_measure_unweighted_all_jerup = (TH1D*)f_sim->Get("h_measure_unweighted_all_jerup");
  TH1D* h_measure_unweighted_all_jerdown = (TH1D*)f_sim->Get("h_measure_unweighted_all_jerdown");
  TH1D* h_measure_unweighted_all_jetup = (TH1D*)f_sim->Get("h_measure_unweighted_all_jetup");
  TH1D* h_measure_unweighted_all_jetdown = (TH1D*)f_sim->Get("h_measure_unweighted_all_jetdown");

  TH1D* h_measure_unweighted_zvertex30 = (TH1D*)f_sim->Get("h_measure_unweighted_zvertex30");
  TH1D* h_measure_unweighted_zvertex30_jesup = (TH1D*)f_sim->Get("h_measure_unweighted_zvertex30_jesup");
  TH1D* h_measure_unweighted_zvertex30_jesdown = (TH1D*)f_sim->Get("h_measure_unweighted_zvertex30_jesdown");
  TH1D* h_measure_unweighted_zvertex30_jerup = (TH1D*)f_sim->Get("h_measure_unweighted_zvertex30_jerup");
  TH1D* h_measure_unweighted_zvertex30_jerdown = (TH1D*)f_sim->Get("h_measure_unweighted_zvertex30_jerdown");
  TH1D* h_measure_unweighted_zvertex30_jetup = (TH1D*)f_sim->Get("h_measure_unweighted_zvertex30_jetup");
  TH1D* h_measure_unweighted_zvertex30_jetdown = (TH1D*)f_sim->Get("h_measure_unweighted_zvertex30_jetdown");
  TH1D* h_measure_unweighted_zvertex30_mbdup = (TH1D*)f_sim->Get("h_measure_unweighted_zvertex30_mbdup");
  TH1D* h_measure_unweighted_zvertex30_mbddown = (TH1D*)f_sim->Get("h_measure_unweighted_zvertex30_mbddown");

  TH1D* h_measure_unweighted_zvertex60 = (TH1D*)f_sim->Get("h_measure_unweighted_zvertex60");
  TH1D* h_measure_unweighted_zvertex60_jesup = (TH1D*)f_sim->Get("h_measure_unweighted_zvertex60_jesup");
  TH1D* h_measure_unweighted_zvertex60_jesdown = (TH1D*)f_sim->Get("h_measure_unweighted_zvertex60_jesdown");
  TH1D* h_measure_unweighted_zvertex60_jerup = (TH1D*)f_sim->Get("h_measure_unweighted_zvertex60_jerup");
  TH1D* h_measure_unweighted_zvertex60_jerdown = (TH1D*)f_sim->Get("h_measure_unweighted_zvertex60_jerdown");
  TH1D* h_measure_unweighted_zvertex60_jetup = (TH1D*)f_sim->Get("h_measure_unweighted_zvertex60_jetup");
  TH1D* h_measure_unweighted_zvertex60_jetdown = (TH1D*)f_sim->Get("h_measure_unweighted_zvertex60_jetdown");
  TH1D* h_measure_unweighted_zvertex60_mbdup = (TH1D*)f_sim->Get("h_measure_unweighted_zvertex60_mbdup");
  TH1D* h_measure_unweighted_zvertex60_mbddown = (TH1D*)f_sim->Get("h_measure_unweighted_zvertex60_mbddown");

  // Form reweighting histograms
  TH1D* h_reweight_all; get_reweight_hist(f_out, h_reweight_all, "h_reweight_all", h_calibjet_pt_all, h_measure_unweighted_all);
  TH1D* h_reweight_all_jesup; get_reweight_hist(f_out, h_reweight_all_jesup, "h_reweight_all_jesup", h_calibjet_pt_all_jesup, h_measure_unweighted_all_jesup);
  TH1D* h_reweight_all_jesdown; get_reweight_hist(f_out, h_reweight_all_jesdown, "h_reweight_all_jesdown", h_calibjet_pt_all_jesdown, h_measure_unweighted_all_jesdown);
  TH1D* h_reweight_all_jerup; get_reweight_hist(f_out, h_reweight_all_jerup, "h_reweight_all_jerup", h_calibjet_pt_all_jerup, h_measure_unweighted_all_jerup);
  TH1D* h_reweight_all_jerdown; get_reweight_hist(f_out, h_reweight_all_jerdown, "h_reweight_all_jerdown", h_calibjet_pt_all_jerdown, h_measure_unweighted_all_jerdown);
  TH1D* h_reweight_all_jetup; get_reweight_hist(f_out, h_reweight_all_jetup, "h_reweight_all_jetup", h_calibjet_pt_all_jetup, h_measure_unweighted_all_jetup);
  TH1D* h_reweight_all_jetdown; get_reweight_hist(f_out, h_reweight_all_jetdown, "h_reweight_all_jetdown", h_calibjet_pt_all_jetdown, h_measure_unweighted_all_jetdown);

  TH1D* h_reweight_zvertex30; get_reweight_hist(f_out, h_reweight_zvertex30, "h_reweight_zvertex30", h_calibjet_pt_zvertex30, h_measure_unweighted_zvertex30);
  TH1D* h_reweight_zvertex30_jesup; get_reweight_hist(f_out, h_reweight_zvertex30_jesup, "h_reweight_zvertex30_jesup", h_calibjet_pt_zvertex30_jesup, h_measure_unweighted_zvertex30_jesup);
  TH1D* h_reweight_zvertex30_jesdown; get_reweight_hist(f_out, h_reweight_zvertex30_jesdown, "h_reweight_zvertex30_jesdown", h_calibjet_pt_zvertex30_jesdown, h_measure_unweighted_zvertex30_jesdown);
  TH1D* h_reweight_zvertex30_jerup; get_reweight_hist(f_out, h_reweight_zvertex30_jerup, "h_reweight_zvertex30_jerup", h_calibjet_pt_zvertex30_jerup, h_measure_unweighted_zvertex30_jerup);
  TH1D* h_reweight_zvertex30_jerdown; get_reweight_hist(f_out, h_reweight_zvertex30_jerdown, "h_reweight_zvertex30_jerdown", h_calibjet_pt_zvertex30_jerdown, h_measure_unweighted_zvertex30_jerdown);
  TH1D* h_reweight_zvertex30_jetup; get_reweight_hist(f_out, h_reweight_zvertex30_jetup, "h_reweight_zvertex30_jetup", h_calibjet_pt_zvertex30_jetup, h_measure_unweighted_zvertex30_jetup);
  TH1D* h_reweight_zvertex30_jetdown; get_reweight_hist(f_out, h_reweight_zvertex30_jetdown, "h_reweight_zvertex30_jetdown", h_calibjet_pt_zvertex30_jetdown, h_measure_unweighted_zvertex30_jetdown);
  TH1D* h_reweight_zvertex30_mbdup; get_reweight_hist(f_out, h_reweight_zvertex30_mbdup, "h_reweight_zvertex30_mbdup", h_calibjet_pt_zvertex30_mbdup, h_measure_unweighted_zvertex30_mbdup);
  TH1D* h_reweight_zvertex30_mbddown; get_reweight_hist(f_out, h_reweight_zvertex30_mbddown, "h_reweight_zvertex30_mbddown", h_calibjet_pt_zvertex30_mbddown, h_measure_unweighted_zvertex30_mbddown);

  TH1D* h_reweight_zvertex60; get_reweight_hist(f_out, h_reweight_zvertex60, "h_reweight_zvertex60", h_calibjet_pt_zvertex60, h_measure_unweighted_zvertex60);
  TH1D* h_reweight_zvertex60_jesup; get_reweight_hist(f_out, h_reweight_zvertex60_jesup, "h_reweight_zvertex60_jesup", h_calibjet_pt_zvertex60_jesup, h_measure_unweighted_zvertex60_jesup);
  TH1D* h_reweight_zvertex60_jesdown; get_reweight_hist(f_out, h_reweight_zvertex60_jesdown, "h_reweight_zvertex60_jesdown", h_calibjet_pt_zvertex60_jesdown, h_measure_unweighted_zvertex60_jesdown);
  TH1D* h_reweight_zvertex60_jerup; get_reweight_hist(f_out, h_reweight_zvertex60_jerup, "h_reweight_zvertex60_jerup", h_calibjet_pt_zvertex60_jerup, h_measure_unweighted_zvertex60_jerup);
  TH1D* h_reweight_zvertex60_jerdown; get_reweight_hist(f_out, h_reweight_zvertex60_jerdown, "h_reweight_zvertex60_jerdown", h_calibjet_pt_zvertex60_jerdown, h_measure_unweighted_zvertex60_jerdown);
  TH1D* h_reweight_zvertex60_jetup; get_reweight_hist(f_out, h_reweight_zvertex60_jetup, "h_reweight_zvertex60_jetup", h_calibjet_pt_zvertex60_jetup, h_measure_unweighted_zvertex60_jetup);
  TH1D* h_reweight_zvertex60_jetdown; get_reweight_hist(f_out, h_reweight_zvertex60_jetdown, "h_reweight_zvertex60_jetdown", h_calibjet_pt_zvertex60_jetdown, h_measure_unweighted_zvertex60_jetdown);
  TH1D* h_reweight_zvertex60_mbdup; get_reweight_hist(f_out, h_reweight_zvertex60_mbdup, "h_reweight_zvertex60_mbdup", h_calibjet_pt_zvertex60_mbdup, h_measure_unweighted_zvertex60_mbdup);
  TH1D* h_reweight_zvertex60_mbddown; get_reweight_hist(f_out, h_reweight_zvertex60_mbddown, "h_reweight_zvertex60_mbddown", h_calibjet_pt_zvertex60_mbddown, h_measure_unweighted_zvertex60_mbddown);
}