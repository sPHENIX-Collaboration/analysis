#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TMath.h>
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include "RooUnfold.h"
#include "RooUnfoldResponse.h"
#include "RooUnfoldBayes.h"
#include "/sphenix/user/hanpuj/plotstyle/AtlasStyle.C"
#include "/sphenix/user/hanpuj/plotstyle/AtlasUtils.C"
#include "unfold_Def.h"

void do_unfolding_closure() {
  //********** Files **********//
  TFile *f_out = new TFile("output_closure_r04.root", "RECREATE");
  TFile *f_in_rm = new TFile("output_sim_r04.root", "READ");

  //********** Input histograms **********//
  TH1D *h_fullclosure_truth_zvertex60 = (TH1D*)f_in_rm->Get("h_fullclosure_truth_zvertex60"); // h_truth
  TH1D *h_fullclosure_measure_zvertex60 = (TH1D*)f_in_rm->Get("h_fullclosure_measure_zvertex60"); // h_measure
  TH2D *h_fullclosure_respmatrix_zvertex60 = (TH2D*)f_in_rm->Get("h_fullclosure_respmatrix_zvertex60"); // h_respmatrix
  TH1D *h_fullclosure_fake_zvertex60 = (TH1D*)f_in_rm->Get("h_fullclosure_fake_zvertex60"); // h_fake
  TH1D *h_fullclosure_miss_zvertex60 = (TH1D*)f_in_rm->Get("h_fullclosure_miss_zvertex60"); // h_miss

  TH1D *h_halfclosure_inputmeasure_zvertex60 = (TH1D*)f_in_rm->Get("h_halfclosure_inputmeasure_zvertex60"); // h_inputmeasure
  h_halfclosure_inputmeasure_zvertex60->SetBinContent(h_halfclosure_inputmeasure_zvertex60->FindBin(70),0.3);
  TH1D *h_halfclosure_truth_zvertex60 = (TH1D*)f_in_rm->Get("h_halfclosure_truth_zvertex60"); // h_truth
  TH1D *h_halfclosure_measure_zvertex60 = (TH1D*)f_in_rm->Get("h_halfclosure_measure_zvertex60"); // h_measure
  TH2D *h_halfclosure_respmatrix_zvertex60 = (TH2D*)f_in_rm->Get("h_halfclosure_respmatrix_zvertex60"); // h_respmatrix
  TH1D *h_halfclosure_fake_zvertex60 = (TH1D*)f_in_rm->Get("h_halfclosure_fake_zvertex60"); // h_fake
  TH1D *h_halfclosure_miss_zvertex60 = (TH1D*)f_in_rm->Get("h_halfclosure_miss_zvertex60"); // h_miss

  //********** Get purity & efficiency **********//
  // Calculate purity: h_purity = (h_measure – h_fake) / h_measure
  TH1D *h_fullclosure_purity_zvertex60 = (TH1D*)h_fullclosure_measure_zvertex60->Clone("h_fullclosure_purity_zvertex60");
  h_fullclosure_purity_zvertex60->Add(h_fullclosure_fake_zvertex60, -1);
  h_fullclosure_purity_zvertex60->Divide(h_fullclosure_purity_zvertex60, h_fullclosure_measure_zvertex60, 1, 1, "B");

  TH1D *h_halfclosure_purity_zvertex60 = (TH1D*)h_halfclosure_measure_zvertex60->Clone("h_halfclosure_purity_zvertex60");
  h_halfclosure_purity_zvertex60->Add(h_halfclosure_fake_zvertex60, -1);
  h_halfclosure_purity_zvertex60->Divide(h_halfclosure_purity_zvertex60, h_halfclosure_measure_zvertex60, 1, 1, "B");

  // Calculate efficiency: h_efficiency = (h_truth – h_miss) / h_truth
  TH1D *h_fullclosure_efficiency_zvertex60 = (TH1D*)h_fullclosure_truth_zvertex60->Clone("h_fullclosure_efficiency_zvertex60");
  h_fullclosure_efficiency_zvertex60->Add(h_fullclosure_miss_zvertex60, -1);
  h_fullclosure_efficiency_zvertex60->Divide(h_fullclosure_efficiency_zvertex60, h_fullclosure_truth_zvertex60, 1, 1, "B");

  TH1D *h_halfclosure_efficiency_zvertex60 = (TH1D*)h_halfclosure_truth_zvertex60->Clone("h_halfclosure_efficiency_zvertex60");
  h_halfclosure_efficiency_zvertex60->Add(h_halfclosure_miss_zvertex60, -1);
  h_halfclosure_efficiency_zvertex60->Divide(h_halfclosure_efficiency_zvertex60, h_halfclosure_truth_zvertex60, 1, 1, "B");

  //********** Full closure test 1 **********//
  // Apply purity correction: h_measure_puritycorrected = h_measure * h_purity
  TH1D *h_fullclosure_puritycorrected_measure_zvertex60 = (TH1D*)h_fullclosure_measure_zvertex60->Clone("h_fullclosure_puritycorrected_measure_zvertex60");
  h_fullclosure_puritycorrected_measure_zvertex60->Multiply(h_fullclosure_purity_zvertex60);

  TH1D *h_halfclosure_puritycorrected_measure_zvertex60 = (TH1D*)h_halfclosure_inputmeasure_zvertex60->Clone("h_halfclosure_puritycorrected_measure_zvertex60");
  h_halfclosure_puritycorrected_measure_zvertex60->Multiply(h_halfclosure_purity_zvertex60);

  // Check whether purity corrected h_measure equal the reco-axis projection of h_respmatrix
  TH1D *h_fullclosure_recoaxis_projection_zvertex60 = (TH1D*)h_fullclosure_respmatrix_zvertex60->ProjectionX("h_fullclosure_recoaxis_projection_zvertex60");

  TH1D *h_halfclosure_recoaxis_projection_zvertex60 = (TH1D*)h_halfclosure_respmatrix_zvertex60->ProjectionX("h_halfclosure_recoaxis_projection_zvertex60");

  // Save the ratio to h_fullclosure_test1_zvertex60 for plotting
  TH1D *h_fullclosure_test1_zvertex60 = (TH1D*)h_fullclosure_puritycorrected_measure_zvertex60->Clone("h_fullclosure_test1_zvertex60");
  h_fullclosure_test1_zvertex60->Divide(h_fullclosure_recoaxis_projection_zvertex60);

  TH1D *h_halfclosure_test1_zvertex60 = (TH1D*)h_halfclosure_puritycorrected_measure_zvertex60->Clone("h_halfclosure_test1_zvertex60");
  h_halfclosure_test1_zvertex60->Divide(h_halfclosure_recoaxis_projection_zvertex60);

  //********** Full closure test 2 **********//
  // Form response matrix object without fake and miss
  TH1D *h_fullclosure_rm_measure_zvertex60 = (TH1D*)h_fullclosure_respmatrix_zvertex60->ProjectionX("h_fullclosure_rm_measure_zvertex60");
  TH1D *h_fullclosure_rm_truth_zvertex60 = (TH1D*)h_fullclosure_respmatrix_zvertex60->ProjectionY("h_fullclosure_rm_truth_zvertex60");
  RooUnfoldResponse *rm_fullclosure_zvertex60 = new RooUnfoldResponse(h_fullclosure_rm_measure_zvertex60, h_fullclosure_rm_truth_zvertex60, h_fullclosure_respmatrix_zvertex60, "rm_fullclosure_zvertex60", "");

  TH1D *h_halfclosure_rm_measure_zvertex60 = (TH1D*)h_halfclosure_respmatrix_zvertex60->ProjectionX("h_halfclosure_rm_measure_zvertex60");
  TH1D *h_halfclosure_rm_truth_zvertex60 = (TH1D*)h_halfclosure_respmatrix_zvertex60->ProjectionY("h_halfclosure_rm_truth_zvertex60");
  RooUnfoldResponse *rm_halfclosure_zvertex60 = new RooUnfoldResponse(h_halfclosure_rm_measure_zvertex60, h_halfclosure_rm_truth_zvertex60, h_halfclosure_respmatrix_zvertex60, "rm_halfclosure_zvertex60", "");

  // Unfold h_measure_puritycorrected with 1 iteration
  RooUnfoldBayes fullclosure_unfold_iter1(rm_fullclosure_zvertex60, h_fullclosure_puritycorrected_measure_zvertex60, 1);
  TH1D *h_fullclosure_unfolded_iter1_zvertex60 = (TH1D*)fullclosure_unfold_iter1.Hunfold(RooUnfold::kErrors);
  h_fullclosure_unfolded_iter1_zvertex60->SetName("h_fullclosure_unfolded_iter1_zvertex60");

  RooUnfoldBayes halfclosure_unfold_iter1(rm_halfclosure_zvertex60, h_halfclosure_puritycorrected_measure_zvertex60, 1);
  TH1D *h_halfclosure_unfolded_iter1_zvertex60 = (TH1D*)halfclosure_unfold_iter1.Hunfold(RooUnfold::kErrors);
  h_halfclosure_unfolded_iter1_zvertex60->SetName("h_halfclosure_unfolded_iter1_zvertex60");

  // Unfold h_measure_puritycorrected with 5 iterations
  RooUnfoldBayes fullclosure_unfold_iter5(rm_fullclosure_zvertex60, h_fullclosure_puritycorrected_measure_zvertex60, 5);
  TH1D *h_fullclosure_unfolded_iter5_zvertex60 = (TH1D*)fullclosure_unfold_iter5.Hunfold(RooUnfold::kErrors);
  h_fullclosure_unfolded_iter5_zvertex60->SetName("h_fullclosure_unfolded_iter5_zvertex60");

  RooUnfoldBayes halfclosure_unfold_iter5(rm_halfclosure_zvertex60, h_halfclosure_puritycorrected_measure_zvertex60, 5);
  TH1D *h_halfclosure_unfolded_iter5_zvertex60 = (TH1D*)halfclosure_unfold_iter5.Hunfold(RooUnfold::kErrors);
  h_halfclosure_unfolded_iter5_zvertex60->SetName("h_halfclosure_unfolded_iter5_zvertex60");

  // Check whether the output equal the truth-axis projection of h_respmatrix
  TH1D *h_fullclosure_truthaxis_projection_zvertex60 = (TH1D*)h_fullclosure_respmatrix_zvertex60->ProjectionY("h_fullclosure_truthaxis_projection_zvertex60");

  TH1D *h_halfclosure_truthaxis_projection_zvertex60 = (TH1D*)h_halfclosure_respmatrix_zvertex60->ProjectionY("h_halfclosure_truthaxis_projection_zvertex60");

  // Save the ratio to h_fullclosure_test2_iter1_zvertex60 and h_fullclosure_test2_iter5_zvertex60 for plotting
  TH1D *h_fullclosure_test2_iter1_zvertex60 = (TH1D*)h_fullclosure_unfolded_iter1_zvertex60->Clone("h_fullclosure_test2_iter1_zvertex60");
  h_fullclosure_test2_iter1_zvertex60->Divide(h_fullclosure_truthaxis_projection_zvertex60);
  TH1D *h_fullclosure_test2_iter5_zvertex60 = (TH1D*)h_fullclosure_unfolded_iter5_zvertex60->Clone("h_fullclosure_test2_iter5_zvertex60");
  h_fullclosure_test2_iter5_zvertex60->Divide(h_fullclosure_truthaxis_projection_zvertex60);

  TH1D *h_halfclosure_test2_iter1_zvertex60 = (TH1D*)h_halfclosure_unfolded_iter1_zvertex60->Clone("h_halfclosure_test2_iter1_zvertex60");
  h_halfclosure_test2_iter1_zvertex60->Divide(h_halfclosure_truthaxis_projection_zvertex60);
  TH1D *h_halfclosure_test2_iter5_zvertex60 = (TH1D*)h_halfclosure_unfolded_iter5_zvertex60->Clone("h_halfclosure_test2_iter5_zvertex60");
  h_halfclosure_test2_iter5_zvertex60->Divide(h_halfclosure_truthaxis_projection_zvertex60);

  //********** Full closure test 3 **********//
  // Apply efficiency correction: h_unfolded_efficiencycorrected = h_unfolded / h_efficiency
  TH1D *h_fullclosure_efficiencycorrected_unfolded_iter1_zvertex = (TH1D*)h_fullclosure_unfolded_iter1_zvertex60->Clone("h_fullclosure_efficiencycorrected_unfolded_iter1_zvertex60");
  h_fullclosure_efficiencycorrected_unfolded_iter1_zvertex->Divide(h_fullclosure_efficiency_zvertex60);

  TH1D *h_halfclosure_efficiencycorrected_unfolded_iter1_zvertex = (TH1D*)h_halfclosure_unfolded_iter1_zvertex60->Clone("h_halfclosure_efficiencycorrected_unfolded_iter1_zvertex60");
  h_halfclosure_efficiencycorrected_unfolded_iter1_zvertex->Divide(h_halfclosure_efficiency_zvertex60);

  // Check whether efficiency corrected h_unfolded equal h_truth and save the ratio to h_fullclosure_test3_iter1_zvertex60 for plotting
  TH1D *h_fullclosure_test3_iter1_zvertex60 = (TH1D*)h_fullclosure_efficiencycorrected_unfolded_iter1_zvertex->Clone("h_fullclosure_test3_iter1_zvertex60");
  h_fullclosure_test3_iter1_zvertex60->Divide(h_fullclosure_truth_zvertex60);

  TH1D *h_halfclosure_test3_iter1_zvertex60 = (TH1D*)h_halfclosure_efficiencycorrected_unfolded_iter1_zvertex->Clone("h_halfclosure_test3_iter1_zvertex60");
  h_halfclosure_test3_iter1_zvertex60->Divide(h_halfclosure_truth_zvertex60);

  //********** Writing to output **********//
  f_out->cd();
  // Input histograms
  h_fullclosure_truth_zvertex60->Write();
  h_fullclosure_measure_zvertex60->Write();
  h_fullclosure_respmatrix_zvertex60->Write();
  h_fullclosure_fake_zvertex60->Write();
  h_fullclosure_miss_zvertex60->Write();

  h_halfclosure_inputmeasure_zvertex60->Write();
  h_halfclosure_truth_zvertex60->Write();
  h_halfclosure_measure_zvertex60->Write();
  h_halfclosure_respmatrix_zvertex60->Write();
  h_halfclosure_fake_zvertex60->Write();
  h_halfclosure_miss_zvertex60->Write();

  // Purity and efficiency histograms
  h_fullclosure_purity_zvertex60->Write();
  h_fullclosure_efficiency_zvertex60->Write();

  h_halfclosure_purity_zvertex60->Write();
  h_halfclosure_efficiency_zvertex60->Write();

  // Full closure test 1
  h_fullclosure_puritycorrected_measure_zvertex60->Write();
  h_fullclosure_recoaxis_projection_zvertex60->Write();
  h_fullclosure_test1_zvertex60->Write();

  h_halfclosure_puritycorrected_measure_zvertex60->Write();
  h_halfclosure_recoaxis_projection_zvertex60->Write();
  h_halfclosure_test1_zvertex60->Write();

  // Full closure test 2
  h_fullclosure_unfolded_iter1_zvertex60->Write();
  h_fullclosure_unfolded_iter5_zvertex60->Write();
  h_fullclosure_truthaxis_projection_zvertex60->Write();
  h_fullclosure_test2_iter1_zvertex60->Write();
  h_fullclosure_test2_iter5_zvertex60->Write();

  h_halfclosure_unfolded_iter1_zvertex60->Write();
  h_halfclosure_unfolded_iter5_zvertex60->Write();
  h_halfclosure_truthaxis_projection_zvertex60->Write();
  h_halfclosure_test2_iter1_zvertex60->Write();
  h_halfclosure_test2_iter5_zvertex60->Write();

  // Full closure test 3
  h_fullclosure_efficiencycorrected_unfolded_iter1_zvertex->Write();
  h_fullclosure_test3_iter1_zvertex60->Write();

  h_halfclosure_efficiencycorrected_unfolded_iter1_zvertex->Write();
  h_halfclosure_test3_iter1_zvertex60->Write();
  f_out->Close();

  //********** Plotting **********//
  SetAtlasStyle();
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);
/*
  // Plot response matrix
  TH2D *draw_respmatrix = (TH2D*)h_fullclosure_respmatrix_zvertex60->Clone("draw_respmatrix");
  TCanvas *can_respmatrix = new TCanvas("can_respmatrix", "", 800, 700);
  gStyle->SetPalette(57);
  can_respmatrix->SetTopMargin(0.12);
  can_respmatrix->SetLeftMargin(0.15);
  can_respmatrix->SetBottomMargin(0.15);
  can_respmatrix->SetRightMargin(0.2);
  can_respmatrix->SetLogz();
  draw_respmatrix->GetXaxis()->SetTitle("p_{T}^{reco} [GeV]");
  draw_respmatrix->GetYaxis()->SetTitle("p_{T}^{truth} [GeV]");
  draw_respmatrix->GetZaxis()->SetTitle(" ");
  draw_respmatrix->GetZaxis()->SetRangeUser(1e-6, 2e5);
  draw_respmatrix->GetXaxis()->SetTitleSize(0.065);
  draw_respmatrix->GetYaxis()->SetTitleSize(0.065);
  draw_respmatrix->GetZaxis()->SetTitleSize(0.06);
  draw_respmatrix->GetXaxis()->SetTitleOffset(1.04);
  draw_respmatrix->GetYaxis()->SetTitleOffset(1.06);
  draw_respmatrix->GetZaxis()->SetTitleOffset(1.22);
  draw_respmatrix->GetXaxis()->SetLabelSize(0.055);
  draw_respmatrix->GetYaxis()->SetLabelSize(0.055);
  draw_respmatrix->GetZaxis()->SetLabelSize(0.055);
  draw_respmatrix->GetXaxis()->CenterTitle();
  draw_respmatrix->GetYaxis()->CenterTitle();
  draw_respmatrix->GetZaxis()->CenterTitle();
  draw_respmatrix->GetYaxis()->SetRangeUser(truthptbins[0], truthptbins[truthnpt-1]);
  draw_respmatrix->Draw("colz");
  myText(0.03, 0.97, 1, "#bf{#it{sPHENIX}} Simulation    PYTHIA8 p+p#sqrt{s} = 200 GeV", 0.05);
  myText(0.03, 0.91, 1, "anti-k_{t} #kern[-0.5]{#it{R}} = 0.4,  |#eta^{jet}| < 0.7,  |z| < 60 cm", 0.05);
  can_respmatrix->SaveAs("figure_closure/fullclosure_respmatrix.png");

  // Plot purity and efficiency
  TH1D *draw_measure = (TH1D*)h_fullclosure_measure_zvertex60->Clone("draw_measure");
  TH1D *draw_fake = (TH1D*)h_fullclosure_fake_zvertex60->Clone("draw_fake");
  TH1D *draw_purity = (TH1D*)h_fullclosure_purity_zvertex60->Clone("draw_purity");
  TCanvas *can_purity = new TCanvas("can_purity", "", 800, 963);
  can_purity->Divide(1, 2);
  gStyle->SetPalette(57);
  TPad *pad_purity_1 = (TPad*)can_purity->cd(1);
  pad_purity_1->SetPad(0, 0.4, 1, 1);
  pad_purity_1->SetTopMargin(0.03);
  pad_purity_1->SetLeftMargin(0.15);
  pad_purity_1->SetBottomMargin(0.035);
  pad_purity_1->SetRightMargin(0.08);
  pad_purity_1->SetLogy();
  draw_measure->SetMarkerStyle(53);
  draw_measure->SetMarkerSize(2);
  draw_measure->SetMarkerColor(kBlack);
  draw_measure->SetLineColor(kBlack);
  draw_fake->SetMarkerStyle(54);
  draw_fake->SetMarkerSize(2);
  draw_fake->SetMarkerColor(kRed);
  draw_fake->SetLineColor(kRed);
  draw_measure->GetXaxis()->SetTitle("p_{T}^{reco} [GeV]");
  draw_measure->GetYaxis()->SetTitle("Arb. Unit");
  draw_measure->GetXaxis()->SetRangeUser(15, 75);
  draw_measure->GetYaxis()->SetRangeUser(5e-2, 5e5);
  draw_measure->GetXaxis()->SetTitleSize(0.065);
  draw_measure->GetYaxis()->SetTitleSize(0.065);
  draw_measure->GetXaxis()->SetTitleOffset(1.04);
  draw_measure->GetYaxis()->SetTitleOffset(1.06);
  draw_measure->GetXaxis()->SetLabelSize(0.055);
  draw_measure->GetYaxis()->SetLabelSize(0.055);
  draw_measure->GetXaxis()->SetLabelOffset(2);
  draw_measure->GetXaxis()->CenterTitle();
  draw_measure->GetYaxis()->CenterTitle();
  draw_measure->Draw();
  draw_fake->Draw("same");
  myText(0.58, 0.9, 1, "#bf{#it{sPHENIX}} Simulation", 0.05);
  myText(0.58, 0.84, 1, "anti-k_{t} #kern[-0.5]{#it{R}} = 0.4", 0.05);
  myText(0.58, 0.78, 1, "|#eta^{jet}| < 0.7", 0.05);
  myText(0.58, 0.72, 1, "|z| < 60 cm", 0.05);
  myMarkerLineText(0.25, 0.2, 2, kBlack, 53, kBlack, 1, "h_measure", 0.05, true);
  myMarkerLineText(0.25, 0.12, 2, kRed, 54, kRed, 1, "h_fake", 0.05, true);
  TPad *pad_purity_2 = (TPad*)can_purity->cd(2);
  pad_purity_2->SetPad(0, 0, 1, 0.4);
  pad_purity_2->SetTopMargin(0.03);
  pad_purity_2->SetLeftMargin(0.15);
  pad_purity_2->SetBottomMargin(0.25);
  pad_purity_2->SetRightMargin(0.08);
  draw_purity->SetMarkerStyle(53);
  draw_purity->SetMarkerSize(2);
  draw_purity->SetMarkerColor(kBlack);
  draw_purity->SetLineColor(kBlack);
  draw_purity->GetXaxis()->SetTitle("p_{T}^{reco} [GeV]");
  draw_purity->GetYaxis()->SetTitle("Purity");
  draw_purity->GetXaxis()->SetRangeUser(15, 75);
  draw_purity->GetYaxis()->SetRangeUser(0.85, 1.05);
  draw_purity->GetXaxis()->CenterTitle();
  draw_purity->GetYaxis()->CenterTitle();
  draw_purity->GetXaxis()->SetTitleOffset(0.95);
  draw_purity->GetYaxis()->SetTitleOffset(draw_measure->GetYaxis()->GetTitleOffset()*4/6.);
  draw_purity->GetXaxis()->SetLabelSize(draw_measure->GetXaxis()->GetLabelSize()*6/4.);
  draw_purity->GetYaxis()->SetLabelSize(draw_measure->GetYaxis()->GetLabelSize()*6/4.);
  draw_purity->GetXaxis()->SetTitleSize(draw_measure->GetXaxis()->GetTitleSize()*6/4.);
  draw_purity->GetYaxis()->SetTitleSize(draw_measure->GetYaxis()->GetTitleSize()*6/4.);
  draw_purity->Draw();
  myText(0.22, 0.9, 1, "h_purity = (h_measure - h_fake) / h_measure", 0.075);
  TLine *line_purity;
  line_purity = new TLine(15, 1, 75, 1);
  line_purity->SetLineColor(kBlack);
  line_purity->SetLineStyle(3);
  line_purity->Draw("same");
  can_purity->SaveAs("figure_closure/fullclosure_purity.png");

  TH1D *draw_truth = (TH1D*)h_fullclosure_truth_zvertex60->Clone("draw_truth");
  TH1D *draw_miss = (TH1D*)h_fullclosure_miss_zvertex60->Clone("draw_miss");
  TH1D *draw_efficiency = (TH1D*)h_fullclosure_efficiency_zvertex60->Clone("draw_efficiency");
  TCanvas *can_efficiency = new TCanvas("can_efficiency", "", 800, 963);
  can_efficiency->Divide(1, 2);
  gStyle->SetPalette(57);
  TPad *pad_efficiency_1 = (TPad*)can_efficiency->cd(1);
  pad_efficiency_1->SetPad(0, 0.4, 1, 1);
  pad_efficiency_1->SetTopMargin(0.03);
  pad_efficiency_1->SetLeftMargin(0.15);
  pad_efficiency_1->SetBottomMargin(0.035);
  pad_efficiency_1->SetRightMargin(0.08);
  pad_efficiency_1->SetLogy();
  draw_truth->SetMarkerStyle(53);
  draw_truth->SetMarkerSize(2);
  draw_truth->SetMarkerColor(kBlack);
  draw_truth->SetLineColor(kBlack);
  draw_miss->SetMarkerStyle(54);
  draw_miss->SetMarkerSize(2);
  draw_miss->SetMarkerColor(kRed);
  draw_miss->SetLineColor(kRed);
  draw_truth->GetXaxis()->SetTitle("p_{T}^{truth} [GeV]");
  draw_truth->GetYaxis()->SetTitle("Arb. Unit");
  draw_truth->GetXaxis()->SetRangeUser(7, 86);
  draw_truth->GetYaxis()->SetRangeUser(5e-4, 5e7);
  draw_truth->GetXaxis()->SetTitleSize(0.065);
  draw_truth->GetYaxis()->SetTitleSize(0.065);
  draw_truth->GetXaxis()->SetTitleOffset(1.04);
  draw_truth->GetYaxis()->SetTitleOffset(1.06);
  draw_truth->GetXaxis()->SetLabelSize(0.055);
  draw_truth->GetYaxis()->SetLabelSize(0.055);
  draw_truth->GetXaxis()->SetLabelOffset(2);
  draw_truth->GetXaxis()->CenterTitle();
  draw_truth->GetYaxis()->CenterTitle();
  draw_truth->Draw();
  draw_miss->Draw("same");
  myText(0.58, 0.9, 1, "#bf{#it{sPHENIX}} Simulation", 0.05);
  myText(0.58, 0.84, 1, "anti-k_{t} #kern[-0.5]{#it{R}} = 0.4", 0.05);
  myText(0.58, 0.78, 1, "|#eta^{jet}| < 0.7", 0.05);
  myText(0.58, 0.72, 1, "|z| < 60 cm", 0.05);
  myMarkerLineText(0.25, 0.2, 2, kBlack, 53, kBlack, 1, "h_truth", 0.05, true);
  myMarkerLineText(0.25, 0.12, 2, kRed, 54, kRed, 1, "h_miss", 0.05, true);
  TPad *pad_efficiency_2 = (TPad*)can_efficiency->cd(2);
  pad_efficiency_2->SetPad(0, 0, 1, 0.4);
  pad_efficiency_2->SetTopMargin(0.03);
  pad_efficiency_2->SetLeftMargin(0.15);
  pad_efficiency_2->SetBottomMargin(0.25);
  pad_efficiency_2->SetRightMargin(0.08);
  draw_efficiency->SetMarkerStyle(53);
  draw_efficiency->SetMarkerSize(2);
  draw_efficiency->SetMarkerColor(kBlack);
  draw_efficiency->SetLineColor(kBlack);
  draw_efficiency->GetXaxis()->SetTitle("p_{T}^{truth} [GeV]");
  draw_efficiency->GetYaxis()->SetTitle("Efficiency");
  draw_efficiency->GetXaxis()->SetRangeUser(7, 86);
  draw_efficiency->GetYaxis()->SetRangeUser(0, 1.2);
  draw_efficiency->GetXaxis()->CenterTitle();
  draw_efficiency->GetYaxis()->CenterTitle();
  draw_efficiency->GetXaxis()->SetTitleOffset(0.95);
  draw_efficiency->GetYaxis()->SetTitleOffset(draw_truth->GetYaxis()->GetTitleOffset()*4/6.);
  draw_efficiency->GetXaxis()->SetLabelSize(draw_truth->GetXaxis()->GetLabelSize()*6/4.);
  draw_efficiency->GetYaxis()->SetLabelSize(draw_truth->GetYaxis()->GetLabelSize()*6/4.);
  draw_efficiency->GetXaxis()->SetTitleSize(draw_truth->GetXaxis()->GetTitleSize()*6/4.);
  draw_efficiency->GetYaxis()->SetTitleSize(draw_truth->GetYaxis()->GetTitleSize()*6/4.);
  draw_efficiency->Draw();
  myText(0.22, 0.9, 1, "h_efficiency = (h_truth - h_miss) / h_truth", 0.075);
  TLine *line_efficiency;
  line_efficiency = new TLine(7, 1, 86, 1);
  line_efficiency->SetLineColor(kBlack);
  line_efficiency->SetLineStyle(3);
  line_efficiency->Draw("same");
  can_efficiency->SaveAs("figure_closure/fullclosure_efficiency.png");

  // Plot full closure test 1
  TH1D *draw_correctedmeasure = (TH1D*)h_fullclosure_puritycorrected_measure_zvertex60->Clone("draw_correctedmeasure");
  TH1D *draw_recoprojection = (TH1D*)h_fullclosure_recoaxis_projection_zvertex60->Clone("draw_recoprojection"); 
  TH1D *draw_test1 = (TH1D*)h_fullclosure_test1_zvertex60->Clone("draw_test1");
  TCanvas *can_test1 = new TCanvas("can_test1", "", 800, 963);
  can_test1->Divide(1, 2);
  gStyle->SetPalette(57);
  TPad *pad_test1_1 = (TPad*)can_test1->cd(1);
  pad_test1_1->SetPad(0, 0.4, 1, 1);
  pad_test1_1->SetTopMargin(0.03);
  pad_test1_1->SetLeftMargin(0.15);
  pad_test1_1->SetBottomMargin(0.035);
  pad_test1_1->SetRightMargin(0.08);
  pad_test1_1->SetLogy();
  draw_correctedmeasure->SetMarkerStyle(53);
  draw_correctedmeasure->SetMarkerSize(2);
  draw_correctedmeasure->SetMarkerColor(kBlack);
  draw_correctedmeasure->SetLineColor(kBlack);
  draw_recoprojection->SetMarkerStyle(54);
  draw_recoprojection->SetMarkerSize(2);
  draw_recoprojection->SetMarkerColor(kRed);
  draw_recoprojection->SetLineColor(kRed);
  draw_correctedmeasure->GetXaxis()->SetTitle("p_{T}^{reco} [GeV]");
  draw_correctedmeasure->GetYaxis()->SetTitle("Arb. Unit");
  draw_correctedmeasure->GetXaxis()->SetRangeUser(15, 75);
  draw_correctedmeasure->GetYaxis()->SetRangeUser(1, 5e5);
  draw_correctedmeasure->GetXaxis()->SetTitleSize(0.065);
  draw_correctedmeasure->GetYaxis()->SetTitleSize(0.065);
  draw_correctedmeasure->GetXaxis()->SetTitleOffset(1.04);
  draw_correctedmeasure->GetYaxis()->SetTitleOffset(1.06);
  draw_correctedmeasure->GetXaxis()->SetLabelSize(0.055);
  draw_correctedmeasure->GetYaxis()->SetLabelSize(0.055);
  draw_correctedmeasure->GetXaxis()->SetLabelOffset(2);
  draw_correctedmeasure->GetXaxis()->CenterTitle();
  draw_correctedmeasure->GetYaxis()->CenterTitle();
  draw_correctedmeasure->Draw();
  draw_recoprojection->Draw("same");
  myText(0.58, 0.9, 1, "#bf{#it{sPHENIX}} Simulation", 0.05);
  myText(0.58, 0.84, 1, "anti-k_{t} #kern[-0.5]{#it{R}} = 0.4", 0.05);
  myText(0.58, 0.78, 1, "|#eta^{jet}| < 0.7", 0.05);
  myText(0.58, 0.72, 1, "|z| < 60 cm", 0.05);
  myMarkerLineText(0.25, 0.2, 2, kBlack, 53, kBlack, 1, "h_measure #times  h_purity", 0.05, true);
  myMarkerLineText(0.25, 0.12, 2, kRed, 54, kRed, 1, "Reco axis projection of response matrix", 0.05, true);
  TPad *pad_test1_2 = (TPad*)can_test1->cd(2);
  pad_test1_2->SetPad(0, 0, 1, 0.4);
  pad_test1_2->SetTopMargin(0.03);
  pad_test1_2->SetLeftMargin(0.15);
  pad_test1_2->SetBottomMargin(0.25);
  pad_test1_2->SetRightMargin(0.08);
  draw_test1->SetMarkerStyle(53);
  draw_test1->SetMarkerSize(2);
  draw_test1->SetMarkerColor(kBlack);
  draw_test1->SetLineColor(kBlack);
  draw_test1->GetXaxis()->SetTitle("p_{T}^{reco} [GeV]");
  draw_test1->GetYaxis()->SetTitle("Ratio");
  draw_test1->GetXaxis()->SetRangeUser(15, 75);
  draw_test1->GetYaxis()->SetRangeUser(0.85, 1.15);
  draw_test1->GetXaxis()->CenterTitle();
  draw_test1->GetYaxis()->CenterTitle();
  draw_test1->GetXaxis()->SetTitleOffset(0.95);
  draw_test1->GetYaxis()->SetTitleOffset(draw_correctedmeasure->GetYaxis()->GetTitleOffset()*4/6.);
  draw_test1->GetXaxis()->SetLabelSize(draw_correctedmeasure->GetXaxis()->GetLabelSize()*6/4.);
  draw_test1->GetYaxis()->SetLabelSize(draw_correctedmeasure->GetYaxis()->GetLabelSize()*6/4.);
  draw_test1->GetXaxis()->SetTitleSize(draw_correctedmeasure->GetXaxis()->GetTitleSize()*6/4.);
  draw_test1->GetYaxis()->SetTitleSize(draw_correctedmeasure->GetYaxis()->GetTitleSize()*6/4.);
  draw_test1->Draw();
  TLine *line_test1;
  line_test1 = new TLine(15, 1, 75, 1);
  line_test1->SetLineColor(kBlack);
  line_test1->SetLineStyle(3);
  line_test1->Draw("same");
  can_test1->SaveAs("figure_closure/fullclosure_test1.png");

  // Plot full closure test 2
  TH1D *draw_unfolded_iter1 = (TH1D*)h_fullclosure_unfolded_iter1_zvertex60->Clone("draw_unfolded_iter1");
  TH1D *draw_unfolded_iter5 = (TH1D*)h_fullclosure_unfolded_iter5_zvertex60->Clone("draw_unfolded_iter5");
  TH1D *draw_truthprojection = (TH1D*)h_fullclosure_truthaxis_projection_zvertex60->Clone("draw_truthprojection");
  TH1D *draw_test2_iter1 = (TH1D*)h_fullclosure_test2_iter1_zvertex60->Clone("draw_test2_iter1");
  TH1D *draw_test2_iter5 = (TH1D*)h_fullclosure_test2_iter5_zvertex60->Clone("draw_test2_iter5");

  TCanvas *can_test2_iter1 = new TCanvas("can_test2_iter1", "", 800, 963);
  can_test2_iter1->Divide(1, 2);
  gStyle->SetPalette(57);
  TPad *pad_test2_iter1_1 = (TPad*)can_test2_iter1->cd(1);
  pad_test2_iter1_1->SetPad(0, 0.4, 1, 1);
  pad_test2_iter1_1->SetTopMargin(0.03);
  pad_test2_iter1_1->SetLeftMargin(0.15);
  pad_test2_iter1_1->SetBottomMargin(0.035);
  pad_test2_iter1_1->SetRightMargin(0.08);
  pad_test2_iter1_1->SetLogy();
  draw_unfolded_iter1->SetMarkerStyle(53);
  draw_unfolded_iter1->SetMarkerSize(2);
  draw_unfolded_iter1->SetMarkerColor(kBlack);
  draw_unfolded_iter1->SetLineColor(kBlack);
  draw_truthprojection->SetMarkerStyle(54);
  draw_truthprojection->SetMarkerSize(2);
  draw_truthprojection->SetMarkerColor(kRed);
  draw_truthprojection->SetLineColor(kRed);
  draw_unfolded_iter1->GetXaxis()->SetTitle("p_{T}^{truth} [GeV]");
  draw_unfolded_iter1->GetYaxis()->SetTitle("Arb. Unit");
  draw_unfolded_iter1->GetXaxis()->SetRangeUser(7, 86);
  draw_unfolded_iter1->GetYaxis()->SetRangeUser(5e-4, 5e5);
  draw_unfolded_iter1->GetXaxis()->SetTitleSize(0.065);
  draw_unfolded_iter1->GetYaxis()->SetTitleSize(0.065);
  draw_unfolded_iter1->GetXaxis()->SetTitleOffset(1.04);
  draw_unfolded_iter1->GetYaxis()->SetTitleOffset(1.06);
  draw_unfolded_iter1->GetXaxis()->SetLabelSize(0.055);
  draw_unfolded_iter1->GetYaxis()->SetLabelSize(0.055);
  draw_unfolded_iter1->GetXaxis()->SetLabelOffset(2);
  draw_unfolded_iter1->GetXaxis()->CenterTitle();
  draw_unfolded_iter1->GetYaxis()->CenterTitle();
  draw_unfolded_iter1->Draw();
  draw_truthprojection->Draw("same");
  myText(0.58, 0.9, 1, "#bf{#it{sPHENIX}} Simulation", 0.05);
  myText(0.58, 0.84, 1, "anti-k_{t} #kern[-0.5]{#it{R}} = 0.4", 0.05);
  myText(0.58, 0.78, 1, "|#eta^{jet}| < 0.7", 0.05);
  myText(0.58, 0.72, 1, "|z| < 60 cm", 0.05);
  myMarkerLineText(0.25, 0.2, 2, kBlack, 53, kBlack, 1, "h_unfolded (1 iteration)", 0.05, true);
  myMarkerLineText(0.25, 0.12, 2, kRed, 54, kRed, 1, "Truth axis projection of response matrix", 0.05, true);
  TPad *pad_test2_iter1_2 = (TPad*)can_test2_iter1->cd(2);
  pad_test2_iter1_2->SetPad(0, 0, 1, 0.4);
  pad_test2_iter1_2->SetTopMargin(0.03);
  pad_test2_iter1_2->SetLeftMargin(0.15);
  pad_test2_iter1_2->SetBottomMargin(0.25);
  pad_test2_iter1_2->SetRightMargin(0.08);
  draw_test2_iter1->SetMarkerStyle(53);
  draw_test2_iter1->SetMarkerSize(2);
  draw_test2_iter1->SetMarkerColor(kBlack);
  draw_test2_iter1->SetLineColor(kBlack);
  draw_test2_iter1->GetXaxis()->SetTitle("p_{T}^{truth} [GeV]");
  draw_test2_iter1->GetYaxis()->SetTitle("Ratio");
  draw_test2_iter1->GetXaxis()->SetRangeUser(7, 86);
  draw_test2_iter1->GetYaxis()->SetRangeUser(0.85, 1.15);
  draw_test2_iter1->GetXaxis()->CenterTitle();
  draw_test2_iter1->GetYaxis()->CenterTitle();
  draw_test2_iter1->GetXaxis()->SetTitleOffset(0.95);
  draw_test2_iter1->GetYaxis()->SetTitleOffset(draw_unfolded_iter1->GetYaxis()->GetTitleOffset()*4/6.);
  draw_test2_iter1->GetXaxis()->SetLabelSize(draw_unfolded_iter1->GetXaxis()->GetLabelSize()*6/4.);
  draw_test2_iter1->GetYaxis()->SetLabelSize(draw_unfolded_iter1->GetYaxis()->GetLabelSize()*6/4.);
  draw_test2_iter1->GetXaxis()->SetTitleSize(draw_unfolded_iter1->GetXaxis()->GetTitleSize()*6/4.);
  draw_test2_iter1->GetYaxis()->SetTitleSize(draw_unfolded_iter1->GetYaxis()->GetTitleSize()*6/4.);
  draw_test2_iter1->Draw();
  TLine *line_test2_iter1;
  line_test2_iter1 = new TLine(7, 1, 86, 1);
  line_test2_iter1->SetLineColor(kBlack);
  line_test2_iter1->SetLineStyle(3);
  line_test2_iter1->Draw("same");
  can_test2_iter1->SaveAs("figure_closure/fullclosure_test2_iter1.png");

  TCanvas *can_test2_iter5 = new TCanvas("can_test2_iter5", "", 800, 963);
  can_test2_iter5->Divide(1, 2);
  gStyle->SetPalette(57);
  TPad *pad_test2_iter5_1 = (TPad*)can_test2_iter5->cd(1);
  pad_test2_iter5_1->SetPad(0, 0.4, 1, 1);
  pad_test2_iter5_1->SetTopMargin(0.03);
  pad_test2_iter5_1->SetLeftMargin(0.15);
  pad_test2_iter5_1->SetBottomMargin(0.035);
  pad_test2_iter5_1->SetRightMargin(0.08);
  pad_test2_iter5_1->SetLogy();
  draw_unfolded_iter5->SetMarkerStyle(53);
  draw_unfolded_iter5->SetMarkerSize(2);
  draw_unfolded_iter5->SetMarkerColor(kBlack);
  draw_unfolded_iter5->SetLineColor(kBlack);
  draw_truthprojection->SetMarkerStyle(54);
  draw_truthprojection->SetMarkerSize(2);
  draw_truthprojection->SetMarkerColor(kRed);
  draw_truthprojection->SetLineColor(kRed);
  draw_unfolded_iter5->GetXaxis()->SetTitle("p_{T}^{truth} [GeV]");
  draw_unfolded_iter5->GetYaxis()->SetTitle("Arb. Unit");
  draw_unfolded_iter5->GetXaxis()->SetRangeUser(7, 86);
  draw_unfolded_iter5->GetYaxis()->SetRangeUser(5e-4, 5e5);
  draw_unfolded_iter5->GetXaxis()->SetTitleSize(0.065);
  draw_unfolded_iter5->GetYaxis()->SetTitleSize(0.065);
  draw_unfolded_iter5->GetXaxis()->SetTitleOffset(1.04);
  draw_unfolded_iter5->GetYaxis()->SetTitleOffset(1.06);
  draw_unfolded_iter5->GetXaxis()->SetLabelSize(0.055);
  draw_unfolded_iter5->GetYaxis()->SetLabelSize(0.055);
  draw_unfolded_iter5->GetXaxis()->SetLabelOffset(2);
  draw_unfolded_iter5->GetXaxis()->CenterTitle();
  draw_unfolded_iter5->GetYaxis()->CenterTitle();
  draw_unfolded_iter5->Draw();
  draw_truthprojection->Draw("same");
  myText(0.58, 0.9, 1, "#bf{#it{sPHENIX}} Simulation", 0.05);
  myText(0.58, 0.84, 1, "anti-k_{t} #kern[-0.5]{#it{R}} = 0.4", 0.05);
  myText(0.58, 0.78, 1, "|#eta^{jet}| < 0.7", 0.05);
  myText(0.58, 0.72, 1, "|z| < 60 cm", 0.05);
  myMarkerLineText(0.25, 0.2, 2, kBlack, 53, kBlack, 1, "h_unfolded (5 iteration)", 0.05, true);
  myMarkerLineText(0.25, 0.12, 2, kRed, 54, kRed, 1, "Truth axis projection of response matrix", 0.05, true);
  TPad *pad_test2_iter5_2 = (TPad*)can_test2_iter5->cd(2);
  pad_test2_iter5_2->SetPad(0, 0, 1, 0.4);
  pad_test2_iter5_2->SetTopMargin(0.03);
  pad_test2_iter5_2->SetLeftMargin(0.15);
  pad_test2_iter5_2->SetBottomMargin(0.25);
  pad_test2_iter5_2->SetRightMargin(0.08);
  draw_test2_iter5->SetMarkerStyle(53);
  draw_test2_iter5->SetMarkerSize(2);
  draw_test2_iter5->SetMarkerColor(kBlack);
  draw_test2_iter5->SetLineColor(kBlack);
  draw_test2_iter5->GetXaxis()->SetTitle("p_{T}^{truth} [GeV]");
  draw_test2_iter5->GetYaxis()->SetTitle("Ratio");
  draw_test2_iter5->GetXaxis()->SetRangeUser(7, 86);
  draw_test2_iter5->GetYaxis()->SetRangeUser(0.85, 1.15);
  draw_test2_iter5->GetXaxis()->CenterTitle();
  draw_test2_iter5->GetYaxis()->CenterTitle();
  draw_test2_iter5->GetXaxis()->SetTitleOffset(0.95);
  draw_test2_iter5->GetYaxis()->SetTitleOffset(draw_unfolded_iter5->GetYaxis()->GetTitleOffset()*4/6.);
  draw_test2_iter5->GetXaxis()->SetLabelSize(draw_unfolded_iter5->GetXaxis()->GetLabelSize()*6/4.);
  draw_test2_iter5->GetYaxis()->SetLabelSize(draw_unfolded_iter5->GetYaxis()->GetLabelSize()*6/4.);
  draw_test2_iter5->GetXaxis()->SetTitleSize(draw_unfolded_iter5->GetXaxis()->GetTitleSize()*6/4.);
  draw_test2_iter5->GetYaxis()->SetTitleSize(draw_unfolded_iter5->GetYaxis()->GetTitleSize()*6/4.);
  draw_test2_iter5->Draw();
  TLine *line_test2_iter5;
  line_test2_iter5 = new TLine(7, 1, 86, 1);
  line_test2_iter5->SetLineColor(kBlack);
  line_test2_iter5->SetLineStyle(3);
  line_test2_iter5->Draw("same");
  can_test2_iter5->SaveAs("figure_closure/fullclosure_test2_iter5.png");

  // Plot full closure test 3
  TH1D *draw_efficiencycorrected_unfolded = (TH1D*)h_fullclosure_efficiencycorrected_unfolded_iter1_zvertex->Clone("draw_efficiencycorrected_unfolded");
  TH1D *draw_truth_final = (TH1D*)h_fullclosure_truth_zvertex60->Clone("draw_truth_final");
  TH1D *draw_test3 = (TH1D*)h_fullclosure_test3_iter1_zvertex60->Clone("draw_test3");
  TCanvas *can_test3 = new TCanvas("can_test3", "", 800, 963);
  can_test3->Divide(1, 2);
  gStyle->SetPalette(57);
  TPad *pad_test3_1 = (TPad*)can_test3->cd(1);
  pad_test3_1->SetPad(0, 0.4, 1, 1);
  pad_test3_1->SetTopMargin(0.03);
  pad_test3_1->SetLeftMargin(0.15);
  pad_test3_1->SetBottomMargin(0.035);
  pad_test3_1->SetRightMargin(0.08);
  pad_test3_1->SetLogy();
  draw_efficiencycorrected_unfolded->SetMarkerStyle(53);
  draw_efficiencycorrected_unfolded->SetMarkerSize(2);
  draw_efficiencycorrected_unfolded->SetMarkerColor(kBlack);
  draw_efficiencycorrected_unfolded->SetLineColor(kBlack);
  draw_truth_final->SetMarkerStyle(54);
  draw_truth_final->SetMarkerSize(2);
  draw_truth_final->SetMarkerColor(kRed);
  draw_truth_final->SetLineColor(kRed);
  draw_efficiencycorrected_unfolded->GetXaxis()->SetTitle("p_{T}^{truth} [GeV]");
  draw_efficiencycorrected_unfolded->GetYaxis()->SetTitle("Arb. Unit");
  draw_efficiencycorrected_unfolded->GetXaxis()->SetRangeUser(7, 86);
  draw_efficiencycorrected_unfolded->GetYaxis()->SetRangeUser(5e-4, 1e8);
  draw_efficiencycorrected_unfolded->GetXaxis()->SetTitleSize(0.065);
  draw_efficiencycorrected_unfolded->GetYaxis()->SetTitleSize(0.065);
  draw_efficiencycorrected_unfolded->GetXaxis()->SetTitleOffset(1.04);
  draw_efficiencycorrected_unfolded->GetYaxis()->SetTitleOffset(1.06);
  draw_efficiencycorrected_unfolded->GetXaxis()->SetLabelSize(0.055);
  draw_efficiencycorrected_unfolded->GetYaxis()->SetLabelSize(0.055);
  draw_efficiencycorrected_unfolded->GetXaxis()->SetLabelOffset(2);
  draw_efficiencycorrected_unfolded->GetXaxis()->CenterTitle();
  draw_efficiencycorrected_unfolded->GetYaxis()->CenterTitle();
  draw_efficiencycorrected_unfolded->Draw();
  draw_truth_final->Draw("same");
  myText(0.58, 0.9, 1, "#bf{#it{sPHENIX}} Simulation", 0.05);
  myText(0.58, 0.84, 1, "anti-k_{t} #kern[-0.5]{#it{R}} = 0.4", 0.05);
  myText(0.58, 0.78, 1, "|#eta^{jet}| < 0.7", 0.05);
  myText(0.58, 0.72, 1, "|z| < 60 cm", 0.05);
  myMarkerLineText(0.25, 0.2, 2, kBlack, 53, kBlack, 1, "h_unfolded (1 iteration) / h_efficiency", 0.05, true);
  myMarkerLineText(0.25, 0.12, 2, kRed, 54, kRed, 1, "h_truth", 0.05, true);
  TPad *pad_test3_2 = (TPad*)can_test3->cd(2);
  pad_test3_2->SetPad(0, 0, 1, 0.4);
  pad_test3_2->SetTopMargin(0.03);
  pad_test3_2->SetLeftMargin(0.15);
  pad_test3_2->SetBottomMargin(0.25);
  pad_test3_2->SetRightMargin(0.08);
  draw_test3->SetMarkerStyle(53);
  draw_test3->SetMarkerSize(2);
  draw_test3->SetMarkerColor(kBlack);
  draw_test3->SetLineColor(kBlack);
  draw_test3->GetXaxis()->SetTitle("p_{T}^{truth} [GeV]");
  draw_test3->GetYaxis()->SetTitle("Ratio");
  draw_test3->GetXaxis()->SetRangeUser(7, 86);
  draw_test3->GetYaxis()->SetRangeUser(0.85, 1.15);
  draw_test3->GetXaxis()->CenterTitle();
  draw_test3->GetYaxis()->CenterTitle();
  draw_test3->GetXaxis()->SetTitleOffset(0.95);
  draw_test3->GetYaxis()->SetTitleOffset(draw_efficiencycorrected_unfolded->GetYaxis()->GetTitleOffset()*4/6.);
  draw_test3->GetXaxis()->SetLabelSize(draw_efficiencycorrected_unfolded->GetXaxis()->GetLabelSize()*6/4.);
  draw_test3->GetYaxis()->SetLabelSize(draw_efficiencycorrected_unfolded->GetYaxis()->GetLabelSize()*6/4.);
  draw_test3->GetXaxis()->SetTitleSize(draw_efficiencycorrected_unfolded->GetXaxis()->GetTitleSize()*6/4.);
  draw_test3->GetYaxis()->SetTitleSize(draw_efficiencycorrected_unfolded->GetYaxis()->GetTitleSize()*6/4.);
  draw_test3->Draw();
  TLine *line_test3;
  line_test3 = new TLine(7, 1, 86, 1);
  line_test3->SetLineColor(kBlack);
  line_test3->SetLineStyle(3);
  line_test3->Draw("same");
  can_test3->SaveAs("figure_closure/fullclosure_test3.png");
*/


  //***** Half closure */
  // Plot response matrix
  TH2D *draw_respmatrix = (TH2D*)h_halfclosure_respmatrix_zvertex60->Clone("draw_respmatrix");
  TCanvas *can_respmatrix = new TCanvas("can_respmatrix", "", 800, 700);
  gStyle->SetPalette(57);
  can_respmatrix->SetTopMargin(0.12);
  can_respmatrix->SetLeftMargin(0.15);
  can_respmatrix->SetBottomMargin(0.15);
  can_respmatrix->SetRightMargin(0.2);
  can_respmatrix->SetLogz();
  draw_respmatrix->GetXaxis()->SetTitle("p_{T}^{reco} [GeV]");
  draw_respmatrix->GetYaxis()->SetTitle("p_{T}^{truth} [GeV]");
  draw_respmatrix->GetZaxis()->SetTitle(" ");
  draw_respmatrix->GetZaxis()->SetRangeUser(1e-6, 2e5);
  draw_respmatrix->GetXaxis()->SetTitleSize(0.065);
  draw_respmatrix->GetYaxis()->SetTitleSize(0.065);
  draw_respmatrix->GetZaxis()->SetTitleSize(0.06);
  draw_respmatrix->GetXaxis()->SetTitleOffset(1.04);
  draw_respmatrix->GetYaxis()->SetTitleOffset(1.06);
  draw_respmatrix->GetZaxis()->SetTitleOffset(1.22);
  draw_respmatrix->GetXaxis()->SetLabelSize(0.055);
  draw_respmatrix->GetYaxis()->SetLabelSize(0.055);
  draw_respmatrix->GetZaxis()->SetLabelSize(0.055);
  draw_respmatrix->GetXaxis()->CenterTitle();
  draw_respmatrix->GetYaxis()->CenterTitle();
  draw_respmatrix->GetZaxis()->CenterTitle();
  draw_respmatrix->GetYaxis()->SetRangeUser(truthptbins[0], truthptbins[truthnpt-1]);
  draw_respmatrix->Draw("colz");
  myText(0.03, 0.97, 1, "#bf{#it{sPHENIX}} Simulation    PYTHIA8 p+p#sqrt{s} = 200 GeV", 0.05);
  myText(0.03, 0.91, 1, "anti-k_{t} #kern[-0.5]{#it{R}} = 0.4,  |#eta^{jet}| < 0.7,  |z| < 60 cm", 0.05);
  can_respmatrix->SaveAs("figure_closure/halfclosure_respmatrix.png");

  // Plot input
  TH1D *draw_measure_resp = (TH1D*)h_halfclosure_measure_zvertex60->Clone("draw_measure_resp");
  TH1D *draw_measure_input = (TH1D*)h_halfclosure_inputmeasure_zvertex60->Clone("draw_measure_input");
  TH1D *draw_measure_ratio = (TH1D*)h_halfclosure_inputmeasure_zvertex60->Clone("draw_measure_ratio");
  draw_measure_ratio->Divide(h_halfclosure_inputmeasure_zvertex60, h_halfclosure_measure_zvertex60, 1, 1, "B");
  TCanvas *can_measure = new TCanvas("can_measure", "", 800, 963);
  can_measure->Divide(1, 2);
  gStyle->SetPalette(57);
  TPad *pad_measure_1 = (TPad*)can_measure->cd(1);
  pad_measure_1->SetPad(0, 0.4, 1, 1);
  pad_measure_1->SetTopMargin(0.03);
  pad_measure_1->SetLeftMargin(0.15);
  pad_measure_1->SetBottomMargin(0.035);
  pad_measure_1->SetRightMargin(0.08);
  pad_measure_1->SetLogy();
  draw_measure_resp->SetMarkerStyle(53);
  draw_measure_resp->SetMarkerSize(2);
  draw_measure_resp->SetMarkerColor(kBlack);
  draw_measure_resp->SetLineColor(kBlack);
  draw_measure_input->SetMarkerStyle(54);
  draw_measure_input->SetMarkerSize(2);
  draw_measure_input->SetMarkerColor(kRed);
  draw_measure_input->SetLineColor(kRed);
  draw_measure_resp->GetXaxis()->SetTitle("p_{T}^{calib} [GeV]");
  draw_measure_resp->GetYaxis()->SetTitle("Arb. Unit");
  draw_measure_resp->GetXaxis()->SetRangeUser(15, 75);
  draw_measure_resp->GetYaxis()->SetRangeUser(5e-2, 5e5);
  draw_measure_resp->GetXaxis()->SetTitleSize(0.065);
  draw_measure_resp->GetYaxis()->SetTitleSize(0.065);
  draw_measure_resp->GetXaxis()->SetTitleOffset(1.04);
  draw_measure_resp->GetYaxis()->SetTitleOffset(1.06);
  draw_measure_resp->GetXaxis()->SetLabelSize(0.055);
  draw_measure_resp->GetYaxis()->SetLabelSize(0.055);
  draw_measure_resp->GetXaxis()->SetLabelOffset(2);
  draw_measure_resp->GetXaxis()->CenterTitle();
  draw_measure_resp->GetYaxis()->CenterTitle();
  draw_measure_resp->Draw();
  draw_measure_input->Draw("same");
  myText(0.58, 0.9, 1, "#bf{#it{sPHENIX}} Simulation", 0.05);
  myText(0.58, 0.84, 1, "anti-k_{t} #kern[-0.5]{#it{R}} = 0.4", 0.05);
  myText(0.58, 0.78, 1, "|#eta^{jet}| < 0.7", 0.05);
  myText(0.58, 0.72, 1, "|z| < 60 cm", 0.05);
  myMarkerLineText(0.25, 0.2, 2, kBlack, 53, kBlack, 1, "h_measure", 0.05, true);
  myMarkerLineText(0.25, 0.12, 2, kRed, 54, kRed, 1, "h_input", 0.05, true);
  TPad *pad_measure_2 = (TPad*)can_measure->cd(2);
  pad_measure_2->SetPad(0, 0, 1, 0.4);
  pad_measure_2->SetTopMargin(0.03);
  pad_measure_2->SetLeftMargin(0.15);
  pad_measure_2->SetBottomMargin(0.25);
  pad_measure_2->SetRightMargin(0.08);
  draw_measure_ratio->SetMarkerStyle(53);
  draw_measure_ratio->SetMarkerSize(2);
  draw_measure_ratio->SetMarkerColor(kBlack);
  draw_measure_ratio->SetLineColor(kBlack);
  draw_measure_ratio->GetXaxis()->SetTitle("p_{T}^{reco} [GeV]");
  draw_measure_ratio->GetYaxis()->SetTitle("h_input / h_measure");
  draw_measure_ratio->GetXaxis()->SetRangeUser(15, 75);
  draw_measure_ratio->GetYaxis()->SetRangeUser(0.85, 1.05);
  draw_measure_ratio->GetXaxis()->CenterTitle();
  draw_measure_ratio->GetYaxis()->CenterTitle();
  draw_measure_ratio->GetXaxis()->SetTitleOffset(0.95);
  draw_measure_ratio->GetYaxis()->SetTitleOffset(draw_measure_resp->GetYaxis()->GetTitleOffset()*4/6.);
  draw_measure_ratio->GetXaxis()->SetLabelSize(draw_measure_resp->GetXaxis()->GetLabelSize()*6/4.);
  draw_measure_ratio->GetYaxis()->SetLabelSize(draw_measure_resp->GetYaxis()->GetLabelSize()*6/4.);
  draw_measure_ratio->GetXaxis()->SetTitleSize(draw_measure_resp->GetXaxis()->GetTitleSize()*6/4.);
  draw_measure_ratio->GetYaxis()->SetTitleSize(draw_measure_resp->GetYaxis()->GetTitleSize()*6/4.);
  draw_measure_ratio->Draw();
  TLine *line_measure;
  line_measure = new TLine(15, 1, 75, 1);
  line_measure->SetLineColor(kBlack);
  line_measure->SetLineStyle(3);
  line_measure->Draw("same");
  can_measure->SaveAs("figure_closure/halfclosure_measure.png");

  // Plot purity and efficiency
  TH1D *draw_measure = (TH1D*)h_halfclosure_measure_zvertex60->Clone("draw_measure");
  TH1D *draw_fake = (TH1D*)h_halfclosure_fake_zvertex60->Clone("draw_fake");
  TH1D *draw_purity = (TH1D*)h_halfclosure_purity_zvertex60->Clone("draw_purity");
  TCanvas *can_purity = new TCanvas("can_purity", "", 800, 963);
  can_purity->Divide(1, 2);
  gStyle->SetPalette(57);
  TPad *pad_purity_1 = (TPad*)can_purity->cd(1);
  pad_purity_1->SetPad(0, 0.4, 1, 1);
  pad_purity_1->SetTopMargin(0.03);
  pad_purity_1->SetLeftMargin(0.15);
  pad_purity_1->SetBottomMargin(0.035);
  pad_purity_1->SetRightMargin(0.08);
  pad_purity_1->SetLogy();
  draw_measure->SetMarkerStyle(53);
  draw_measure->SetMarkerSize(2);
  draw_measure->SetMarkerColor(kBlack);
  draw_measure->SetLineColor(kBlack);
  draw_fake->SetMarkerStyle(54);
  draw_fake->SetMarkerSize(2);
  draw_fake->SetMarkerColor(kRed);
  draw_fake->SetLineColor(kRed);
  draw_measure->GetXaxis()->SetTitle("p_{T}^{reco} [GeV]");
  draw_measure->GetYaxis()->SetTitle("Arb. Unit");
  draw_measure->GetXaxis()->SetRangeUser(15, 75);
  draw_measure->GetYaxis()->SetRangeUser(5e-2, 5e5);
  draw_measure->GetXaxis()->SetTitleSize(0.065);
  draw_measure->GetYaxis()->SetTitleSize(0.065);
  draw_measure->GetXaxis()->SetTitleOffset(1.04);
  draw_measure->GetYaxis()->SetTitleOffset(1.06);
  draw_measure->GetXaxis()->SetLabelSize(0.055);
  draw_measure->GetYaxis()->SetLabelSize(0.055);
  draw_measure->GetXaxis()->SetLabelOffset(2);
  draw_measure->GetXaxis()->CenterTitle();
  draw_measure->GetYaxis()->CenterTitle();
  draw_measure->Draw();
  draw_fake->Draw("same");
  myText(0.58, 0.9, 1, "#bf{#it{sPHENIX}} Simulation", 0.05);
  myText(0.58, 0.84, 1, "anti-k_{t} #kern[-0.5]{#it{R}} = 0.4", 0.05);
  myText(0.58, 0.78, 1, "|#eta^{jet}| < 0.7", 0.05);
  myText(0.58, 0.72, 1, "|z| < 60 cm", 0.05);
  myMarkerLineText(0.25, 0.2, 2, kBlack, 53, kBlack, 1, "h_measure", 0.05, true);
  myMarkerLineText(0.25, 0.12, 2, kRed, 54, kRed, 1, "h_fake", 0.05, true);
  TPad *pad_purity_2 = (TPad*)can_purity->cd(2);
  pad_purity_2->SetPad(0, 0, 1, 0.4);
  pad_purity_2->SetTopMargin(0.03);
  pad_purity_2->SetLeftMargin(0.15);
  pad_purity_2->SetBottomMargin(0.25);
  pad_purity_2->SetRightMargin(0.08);
  draw_purity->SetMarkerStyle(53);
  draw_purity->SetMarkerSize(2);
  draw_purity->SetMarkerColor(kBlack);
  draw_purity->SetLineColor(kBlack);
  draw_purity->GetXaxis()->SetTitle("p_{T}^{reco} [GeV]");
  draw_purity->GetYaxis()->SetTitle("Purity");
  draw_purity->GetXaxis()->SetRangeUser(15, 75);
  draw_purity->GetYaxis()->SetRangeUser(0.85, 1.05);
  draw_purity->GetXaxis()->CenterTitle();
  draw_purity->GetYaxis()->CenterTitle();
  draw_purity->GetXaxis()->SetTitleOffset(0.95);
  draw_purity->GetYaxis()->SetTitleOffset(draw_measure->GetYaxis()->GetTitleOffset()*4/6.);
  draw_purity->GetXaxis()->SetLabelSize(draw_measure->GetXaxis()->GetLabelSize()*6/4.);
  draw_purity->GetYaxis()->SetLabelSize(draw_measure->GetYaxis()->GetLabelSize()*6/4.);
  draw_purity->GetXaxis()->SetTitleSize(draw_measure->GetXaxis()->GetTitleSize()*6/4.);
  draw_purity->GetYaxis()->SetTitleSize(draw_measure->GetYaxis()->GetTitleSize()*6/4.);
  draw_purity->Draw();
  myText(0.22, 0.9, 1, "h_purity = (h_measure - h_fake) / h_measure", 0.075);
  TLine *line_purity;
  line_purity = new TLine(15, 1, 75, 1);
  line_purity->SetLineColor(kBlack);
  line_purity->SetLineStyle(3);
  line_purity->Draw("same");
  can_purity->SaveAs("figure_closure/halfclosure_purity.png");

  TH1D *draw_truth = (TH1D*)h_halfclosure_truth_zvertex60->Clone("draw_truth");
  TH1D *draw_miss = (TH1D*)h_halfclosure_miss_zvertex60->Clone("draw_miss");
  TH1D *draw_efficiency = (TH1D*)h_halfclosure_efficiency_zvertex60->Clone("draw_efficiency");
  TCanvas *can_efficiency = new TCanvas("can_efficiency", "", 800, 963);
  can_efficiency->Divide(1, 2);
  gStyle->SetPalette(57);
  TPad *pad_efficiency_1 = (TPad*)can_efficiency->cd(1);
  pad_efficiency_1->SetPad(0, 0.4, 1, 1);
  pad_efficiency_1->SetTopMargin(0.03);
  pad_efficiency_1->SetLeftMargin(0.15);
  pad_efficiency_1->SetBottomMargin(0.035);
  pad_efficiency_1->SetRightMargin(0.08);
  pad_efficiency_1->SetLogy();
  draw_truth->SetMarkerStyle(53);
  draw_truth->SetMarkerSize(2);
  draw_truth->SetMarkerColor(kBlack);
  draw_truth->SetLineColor(kBlack);
  draw_miss->SetMarkerStyle(54);
  draw_miss->SetMarkerSize(2);
  draw_miss->SetMarkerColor(kRed);
  draw_miss->SetLineColor(kRed);
  draw_truth->GetXaxis()->SetTitle("p_{T}^{truth} [GeV]");
  draw_truth->GetYaxis()->SetTitle("Arb. Unit");
  draw_truth->GetXaxis()->SetRangeUser(7, 86);
  draw_truth->GetYaxis()->SetRangeUser(5e-4, 5e7);
  draw_truth->GetXaxis()->SetTitleSize(0.065);
  draw_truth->GetYaxis()->SetTitleSize(0.065);
  draw_truth->GetXaxis()->SetTitleOffset(1.04);
  draw_truth->GetYaxis()->SetTitleOffset(1.06);
  draw_truth->GetXaxis()->SetLabelSize(0.055);
  draw_truth->GetYaxis()->SetLabelSize(0.055);
  draw_truth->GetXaxis()->SetLabelOffset(2);
  draw_truth->GetXaxis()->CenterTitle();
  draw_truth->GetYaxis()->CenterTitle();
  draw_truth->Draw();
  draw_miss->Draw("same");
  myText(0.58, 0.9, 1, "#bf{#it{sPHENIX}} Simulation", 0.05);
  myText(0.58, 0.84, 1, "anti-k_{t} #kern[-0.5]{#it{R}} = 0.4", 0.05);
  myText(0.58, 0.78, 1, "|#eta^{jet}| < 0.7", 0.05);
  myText(0.58, 0.72, 1, "|z| < 60 cm", 0.05);
  myMarkerLineText(0.25, 0.2, 2, kBlack, 53, kBlack, 1, "h_truth", 0.05, true);
  myMarkerLineText(0.25, 0.12, 2, kRed, 54, kRed, 1, "h_miss", 0.05, true);
  TPad *pad_efficiency_2 = (TPad*)can_efficiency->cd(2);
  pad_efficiency_2->SetPad(0, 0, 1, 0.4);
  pad_efficiency_2->SetTopMargin(0.03);
  pad_efficiency_2->SetLeftMargin(0.15);
  pad_efficiency_2->SetBottomMargin(0.25);
  pad_efficiency_2->SetRightMargin(0.08);
  draw_efficiency->SetMarkerStyle(53);
  draw_efficiency->SetMarkerSize(2);
  draw_efficiency->SetMarkerColor(kBlack);
  draw_efficiency->SetLineColor(kBlack);
  draw_efficiency->GetXaxis()->SetTitle("p_{T}^{truth} [GeV]");
  draw_efficiency->GetYaxis()->SetTitle("Efficiency");
  draw_efficiency->GetXaxis()->SetRangeUser(7, 86);
  draw_efficiency->GetYaxis()->SetRangeUser(0, 1.2);
  draw_efficiency->GetXaxis()->CenterTitle();
  draw_efficiency->GetYaxis()->CenterTitle();
  draw_efficiency->GetXaxis()->SetTitleOffset(0.95);
  draw_efficiency->GetYaxis()->SetTitleOffset(draw_truth->GetYaxis()->GetTitleOffset()*4/6.);
  draw_efficiency->GetXaxis()->SetLabelSize(draw_truth->GetXaxis()->GetLabelSize()*6/4.);
  draw_efficiency->GetYaxis()->SetLabelSize(draw_truth->GetYaxis()->GetLabelSize()*6/4.);
  draw_efficiency->GetXaxis()->SetTitleSize(draw_truth->GetXaxis()->GetTitleSize()*6/4.);
  draw_efficiency->GetYaxis()->SetTitleSize(draw_truth->GetYaxis()->GetTitleSize()*6/4.);
  draw_efficiency->Draw();
  myText(0.22, 0.9, 1, "h_efficiency = (h_truth - h_miss) / h_truth", 0.075);
  TLine *line_efficiency;
  line_efficiency = new TLine(7, 1, 86, 1);
  line_efficiency->SetLineColor(kBlack);
  line_efficiency->SetLineStyle(3);
  line_efficiency->Draw("same");
  can_efficiency->SaveAs("figure_closure/halfclosure_efficiency.png");

  // Plot half closure test 1
  TH1D *draw_correctedmeasure = (TH1D*)h_halfclosure_puritycorrected_measure_zvertex60->Clone("draw_correctedmeasure");
  TH1D *draw_recoprojection = (TH1D*)h_halfclosure_recoaxis_projection_zvertex60->Clone("draw_recoprojection"); 
  TH1D *draw_test1 = (TH1D*)h_halfclosure_test1_zvertex60->Clone("draw_test1");
  TCanvas *can_test1 = new TCanvas("can_test1", "", 800, 963);
  can_test1->Divide(1, 2);
  gStyle->SetPalette(57);
  TPad *pad_test1_1 = (TPad*)can_test1->cd(1);
  pad_test1_1->SetPad(0, 0.4, 1, 1);
  pad_test1_1->SetTopMargin(0.03);
  pad_test1_1->SetLeftMargin(0.15);
  pad_test1_1->SetBottomMargin(0.035);
  pad_test1_1->SetRightMargin(0.08);
  pad_test1_1->SetLogy();
  draw_correctedmeasure->SetMarkerStyle(53);
  draw_correctedmeasure->SetMarkerSize(2);
  draw_correctedmeasure->SetMarkerColor(kBlack);
  draw_correctedmeasure->SetLineColor(kBlack);
  draw_recoprojection->SetMarkerStyle(54);
  draw_recoprojection->SetMarkerSize(2);
  draw_recoprojection->SetMarkerColor(kRed);
  draw_recoprojection->SetLineColor(kRed);
  draw_correctedmeasure->GetXaxis()->SetTitle("p_{T}^{reco} [GeV]");
  draw_correctedmeasure->GetYaxis()->SetTitle("Arb. Unit");
  draw_correctedmeasure->GetXaxis()->SetRangeUser(15, 75);
  draw_correctedmeasure->GetYaxis()->SetRangeUser(1, 5e5);
  draw_correctedmeasure->GetXaxis()->SetTitleSize(0.065);
  draw_correctedmeasure->GetYaxis()->SetTitleSize(0.065);
  draw_correctedmeasure->GetXaxis()->SetTitleOffset(1.04);
  draw_correctedmeasure->GetYaxis()->SetTitleOffset(1.06);
  draw_correctedmeasure->GetXaxis()->SetLabelSize(0.055);
  draw_correctedmeasure->GetYaxis()->SetLabelSize(0.055);
  draw_correctedmeasure->GetXaxis()->SetLabelOffset(2);
  draw_correctedmeasure->GetXaxis()->CenterTitle();
  draw_correctedmeasure->GetYaxis()->CenterTitle();
  draw_correctedmeasure->Draw();
  draw_recoprojection->Draw("same");
  myText(0.58, 0.9, 1, "#bf{#it{sPHENIX}} Simulation", 0.05);
  myText(0.58, 0.84, 1, "anti-k_{t} #kern[-0.5]{#it{R}} = 0.4", 0.05);
  myText(0.58, 0.78, 1, "|#eta^{jet}| < 0.7", 0.05);
  myText(0.58, 0.72, 1, "|z| < 60 cm", 0.05);
  myMarkerLineText(0.25, 0.2, 2, kBlack, 53, kBlack, 1, "h_measure #times  h_purity", 0.05, true);
  myMarkerLineText(0.25, 0.12, 2, kRed, 54, kRed, 1, "Reco axis projection of response matrix", 0.05, true);
  TPad *pad_test1_2 = (TPad*)can_test1->cd(2);
  pad_test1_2->SetPad(0, 0, 1, 0.4);
  pad_test1_2->SetTopMargin(0.03);
  pad_test1_2->SetLeftMargin(0.15);
  pad_test1_2->SetBottomMargin(0.25);
  pad_test1_2->SetRightMargin(0.08);
  draw_test1->SetMarkerStyle(53);
  draw_test1->SetMarkerSize(2);
  draw_test1->SetMarkerColor(kBlack);
  draw_test1->SetLineColor(kBlack);
  draw_test1->GetXaxis()->SetTitle("p_{T}^{reco} [GeV]");
  draw_test1->GetYaxis()->SetTitle("Ratio");
  draw_test1->GetXaxis()->SetRangeUser(15, 75);
  draw_test1->GetYaxis()->SetRangeUser(0.85, 1.15);
  draw_test1->GetXaxis()->CenterTitle();
  draw_test1->GetYaxis()->CenterTitle();
  draw_test1->GetXaxis()->SetTitleOffset(0.95);
  draw_test1->GetYaxis()->SetTitleOffset(draw_correctedmeasure->GetYaxis()->GetTitleOffset()*4/6.);
  draw_test1->GetXaxis()->SetLabelSize(draw_correctedmeasure->GetXaxis()->GetLabelSize()*6/4.);
  draw_test1->GetYaxis()->SetLabelSize(draw_correctedmeasure->GetYaxis()->GetLabelSize()*6/4.);
  draw_test1->GetXaxis()->SetTitleSize(draw_correctedmeasure->GetXaxis()->GetTitleSize()*6/4.);
  draw_test1->GetYaxis()->SetTitleSize(draw_correctedmeasure->GetYaxis()->GetTitleSize()*6/4.);
  draw_test1->Draw();
  TLine *line_test1;
  line_test1 = new TLine(15, 1, 75, 1);
  line_test1->SetLineColor(kBlack);
  line_test1->SetLineStyle(3);
  line_test1->Draw("same");
  can_test1->SaveAs("figure_closure/halfclosure_test1.png");

  // Plot half closure test 2
  TH1D *draw_unfolded_iter1 = (TH1D*)h_halfclosure_unfolded_iter1_zvertex60->Clone("draw_unfolded_iter1");
  TH1D *draw_unfolded_iter5 = (TH1D*)h_halfclosure_unfolded_iter5_zvertex60->Clone("draw_unfolded_iter5");
  TH1D *draw_truthprojection = (TH1D*)h_halfclosure_truthaxis_projection_zvertex60->Clone("draw_truthprojection");
  TH1D *draw_test2_iter1 = (TH1D*)h_halfclosure_test2_iter1_zvertex60->Clone("draw_test2_iter1");
  TH1D *draw_test2_iter5 = (TH1D*)h_halfclosure_test2_iter5_zvertex60->Clone("draw_test2_iter5");

  TCanvas *can_test2_iter1 = new TCanvas("can_test2_iter1", "", 800, 963);
  can_test2_iter1->Divide(1, 2);
  gStyle->SetPalette(57);
  TPad *pad_test2_iter1_1 = (TPad*)can_test2_iter1->cd(1);
  pad_test2_iter1_1->SetPad(0, 0.4, 1, 1);
  pad_test2_iter1_1->SetTopMargin(0.03);
  pad_test2_iter1_1->SetLeftMargin(0.15);
  pad_test2_iter1_1->SetBottomMargin(0.035);
  pad_test2_iter1_1->SetRightMargin(0.08);
  pad_test2_iter1_1->SetLogy();
  draw_unfolded_iter1->SetMarkerStyle(53);
  draw_unfolded_iter1->SetMarkerSize(2);
  draw_unfolded_iter1->SetMarkerColor(kBlack);
  draw_unfolded_iter1->SetLineColor(kBlack);
  draw_truthprojection->SetMarkerStyle(54);
  draw_truthprojection->SetMarkerSize(2);
  draw_truthprojection->SetMarkerColor(kRed);
  draw_truthprojection->SetLineColor(kRed);
  draw_unfolded_iter1->GetXaxis()->SetTitle("p_{T}^{truth} [GeV]");
  draw_unfolded_iter1->GetYaxis()->SetTitle("Arb. Unit");
  draw_unfolded_iter1->GetXaxis()->SetRangeUser(7, 86);
  draw_unfolded_iter1->GetYaxis()->SetRangeUser(5e-4, 5e5);
  draw_unfolded_iter1->GetXaxis()->SetTitleSize(0.065);
  draw_unfolded_iter1->GetYaxis()->SetTitleSize(0.065);
  draw_unfolded_iter1->GetXaxis()->SetTitleOffset(1.04);
  draw_unfolded_iter1->GetYaxis()->SetTitleOffset(1.06);
  draw_unfolded_iter1->GetXaxis()->SetLabelSize(0.055);
  draw_unfolded_iter1->GetYaxis()->SetLabelSize(0.055);
  draw_unfolded_iter1->GetXaxis()->SetLabelOffset(2);
  draw_unfolded_iter1->GetXaxis()->CenterTitle();
  draw_unfolded_iter1->GetYaxis()->CenterTitle();
  draw_unfolded_iter1->Draw();
  draw_truthprojection->Draw("same");
  myText(0.58, 0.9, 1, "#bf{#it{sPHENIX}} Simulation", 0.05);
  myText(0.58, 0.84, 1, "anti-k_{t} #kern[-0.5]{#it{R}} = 0.4", 0.05);
  myText(0.58, 0.78, 1, "|#eta^{jet}| < 0.7", 0.05);
  myText(0.58, 0.72, 1, "|z| < 60 cm", 0.05);
  myMarkerLineText(0.25, 0.2, 2, kBlack, 53, kBlack, 1, "h_unfolded (1 iteration)", 0.05, true);
  myMarkerLineText(0.25, 0.12, 2, kRed, 54, kRed, 1, "Truth axis projection of response matrix", 0.05, true);
  TPad *pad_test2_iter1_2 = (TPad*)can_test2_iter1->cd(2);
  pad_test2_iter1_2->SetPad(0, 0, 1, 0.4);
  pad_test2_iter1_2->SetTopMargin(0.03);
  pad_test2_iter1_2->SetLeftMargin(0.15);
  pad_test2_iter1_2->SetBottomMargin(0.25);
  pad_test2_iter1_2->SetRightMargin(0.08);
  draw_test2_iter1->SetMarkerStyle(53);
  draw_test2_iter1->SetMarkerSize(2);
  draw_test2_iter1->SetMarkerColor(kBlack);
  draw_test2_iter1->SetLineColor(kBlack);
  draw_test2_iter1->GetXaxis()->SetTitle("p_{T}^{truth} [GeV]");
  draw_test2_iter1->GetYaxis()->SetTitle("Ratio");
  draw_test2_iter1->GetXaxis()->SetRangeUser(7, 86);
  draw_test2_iter1->GetYaxis()->SetRangeUser(0.85, 1.15);
  draw_test2_iter1->GetXaxis()->CenterTitle();
  draw_test2_iter1->GetYaxis()->CenterTitle();
  draw_test2_iter1->GetXaxis()->SetTitleOffset(0.95);
  draw_test2_iter1->GetYaxis()->SetTitleOffset(draw_unfolded_iter1->GetYaxis()->GetTitleOffset()*4/6.);
  draw_test2_iter1->GetXaxis()->SetLabelSize(draw_unfolded_iter1->GetXaxis()->GetLabelSize()*6/4.);
  draw_test2_iter1->GetYaxis()->SetLabelSize(draw_unfolded_iter1->GetYaxis()->GetLabelSize()*6/4.);
  draw_test2_iter1->GetXaxis()->SetTitleSize(draw_unfolded_iter1->GetXaxis()->GetTitleSize()*6/4.);
  draw_test2_iter1->GetYaxis()->SetTitleSize(draw_unfolded_iter1->GetYaxis()->GetTitleSize()*6/4.);
  draw_test2_iter1->Draw();
  TLine *line_test2_iter1;
  line_test2_iter1 = new TLine(7, 1, 86, 1);
  line_test2_iter1->SetLineColor(kBlack);
  line_test2_iter1->SetLineStyle(3);
  line_test2_iter1->Draw("same");
  can_test2_iter1->SaveAs("figure_closure/halfclosure_test2_iter1.png");

  TCanvas *can_test2_iter5 = new TCanvas("can_test2_iter5", "", 800, 963);
  can_test2_iter5->Divide(1, 2);
  gStyle->SetPalette(57);
  TPad *pad_test2_iter5_1 = (TPad*)can_test2_iter5->cd(1);
  pad_test2_iter5_1->SetPad(0, 0.4, 1, 1);
  pad_test2_iter5_1->SetTopMargin(0.03);
  pad_test2_iter5_1->SetLeftMargin(0.15);
  pad_test2_iter5_1->SetBottomMargin(0.035);
  pad_test2_iter5_1->SetRightMargin(0.08);
  pad_test2_iter5_1->SetLogy();
  draw_unfolded_iter5->SetMarkerStyle(53);
  draw_unfolded_iter5->SetMarkerSize(2);
  draw_unfolded_iter5->SetMarkerColor(kBlack);
  draw_unfolded_iter5->SetLineColor(kBlack);
  draw_truthprojection->SetMarkerStyle(54);
  draw_truthprojection->SetMarkerSize(2);
  draw_truthprojection->SetMarkerColor(kRed);
  draw_truthprojection->SetLineColor(kRed);
  draw_unfolded_iter5->GetXaxis()->SetTitle("p_{T}^{truth} [GeV]");
  draw_unfolded_iter5->GetYaxis()->SetTitle("Arb. Unit");
  draw_unfolded_iter5->GetXaxis()->SetRangeUser(7, 86);
  draw_unfolded_iter5->GetYaxis()->SetRangeUser(5e-4, 5e5);
  draw_unfolded_iter5->GetXaxis()->SetTitleSize(0.065);
  draw_unfolded_iter5->GetYaxis()->SetTitleSize(0.065);
  draw_unfolded_iter5->GetXaxis()->SetTitleOffset(1.04);
  draw_unfolded_iter5->GetYaxis()->SetTitleOffset(1.06);
  draw_unfolded_iter5->GetXaxis()->SetLabelSize(0.055);
  draw_unfolded_iter5->GetYaxis()->SetLabelSize(0.055);
  draw_unfolded_iter5->GetXaxis()->SetLabelOffset(2);
  draw_unfolded_iter5->GetXaxis()->CenterTitle();
  draw_unfolded_iter5->GetYaxis()->CenterTitle();
  draw_unfolded_iter5->Draw();
  draw_truthprojection->Draw("same");
  myText(0.58, 0.9, 1, "#bf{#it{sPHENIX}} Simulation", 0.05);
  myText(0.58, 0.84, 1, "anti-k_{t} #kern[-0.5]{#it{R}} = 0.4", 0.05);
  myText(0.58, 0.78, 1, "|#eta^{jet}| < 0.7", 0.05);
  myText(0.58, 0.72, 1, "|z| < 60 cm", 0.05);
  myMarkerLineText(0.25, 0.2, 2, kBlack, 53, kBlack, 1, "h_unfolded (5 iteration)", 0.05, true);
  myMarkerLineText(0.25, 0.12, 2, kRed, 54, kRed, 1, "Truth axis projection of response matrix", 0.05, true);
  TPad *pad_test2_iter5_2 = (TPad*)can_test2_iter5->cd(2);
  pad_test2_iter5_2->SetPad(0, 0, 1, 0.4);
  pad_test2_iter5_2->SetTopMargin(0.03);
  pad_test2_iter5_2->SetLeftMargin(0.15);
  pad_test2_iter5_2->SetBottomMargin(0.25);
  pad_test2_iter5_2->SetRightMargin(0.08);
  draw_test2_iter5->SetMarkerStyle(53);
  draw_test2_iter5->SetMarkerSize(2);
  draw_test2_iter5->SetMarkerColor(kBlack);
  draw_test2_iter5->SetLineColor(kBlack);
  draw_test2_iter5->GetXaxis()->SetTitle("p_{T}^{truth} [GeV]");
  draw_test2_iter5->GetYaxis()->SetTitle("Ratio");
  draw_test2_iter5->GetXaxis()->SetRangeUser(7, 86);
  draw_test2_iter5->GetYaxis()->SetRangeUser(0.85, 1.15);
  draw_test2_iter5->GetXaxis()->CenterTitle();
  draw_test2_iter5->GetYaxis()->CenterTitle();
  draw_test2_iter5->GetXaxis()->SetTitleOffset(0.95);
  draw_test2_iter5->GetYaxis()->SetTitleOffset(draw_unfolded_iter5->GetYaxis()->GetTitleOffset()*4/6.);
  draw_test2_iter5->GetXaxis()->SetLabelSize(draw_unfolded_iter5->GetXaxis()->GetLabelSize()*6/4.);
  draw_test2_iter5->GetYaxis()->SetLabelSize(draw_unfolded_iter5->GetYaxis()->GetLabelSize()*6/4.);
  draw_test2_iter5->GetXaxis()->SetTitleSize(draw_unfolded_iter5->GetXaxis()->GetTitleSize()*6/4.);
  draw_test2_iter5->GetYaxis()->SetTitleSize(draw_unfolded_iter5->GetYaxis()->GetTitleSize()*6/4.);
  draw_test2_iter5->Draw();
  TLine *line_test2_iter5;
  line_test2_iter5 = new TLine(7, 1, 86, 1);
  line_test2_iter5->SetLineColor(kBlack);
  line_test2_iter5->SetLineStyle(3);
  line_test2_iter5->Draw("same");
  can_test2_iter5->SaveAs("figure_closure/halfclosure_test2_iter5.png");

  // Plot half closure test 3
  TH1D *draw_efficiencycorrected_unfolded = (TH1D*)h_halfclosure_efficiencycorrected_unfolded_iter1_zvertex->Clone("draw_efficiencycorrected_unfolded");
  TH1D *draw_truth_final = (TH1D*)h_halfclosure_truth_zvertex60->Clone("draw_truth_final");
  TH1D *draw_test3 = (TH1D*)h_halfclosure_test3_iter1_zvertex60->Clone("draw_test3");
  TCanvas *can_test3 = new TCanvas("can_test3", "", 800, 963);
  can_test3->Divide(1, 2);
  gStyle->SetPalette(57);
  TPad *pad_test3_1 = (TPad*)can_test3->cd(1);
  pad_test3_1->SetPad(0, 0.4, 1, 1);
  pad_test3_1->SetTopMargin(0.03);
  pad_test3_1->SetLeftMargin(0.15);
  pad_test3_1->SetBottomMargin(0.035);
  pad_test3_1->SetRightMargin(0.08);
  pad_test3_1->SetLogy();
  draw_efficiencycorrected_unfolded->SetMarkerStyle(53);
  draw_efficiencycorrected_unfolded->SetMarkerSize(2);
  draw_efficiencycorrected_unfolded->SetMarkerColor(kBlack);
  draw_efficiencycorrected_unfolded->SetLineColor(kBlack);
  draw_truth_final->SetMarkerStyle(54);
  draw_truth_final->SetMarkerSize(2);
  draw_truth_final->SetMarkerColor(kRed);
  draw_truth_final->SetLineColor(kRed);
  draw_efficiencycorrected_unfolded->GetXaxis()->SetTitle("p_{T}^{truth} [GeV]");
  draw_efficiencycorrected_unfolded->GetYaxis()->SetTitle("Arb. Unit");
  draw_efficiencycorrected_unfolded->GetXaxis()->SetRangeUser(7, 86);
  draw_efficiencycorrected_unfolded->GetYaxis()->SetRangeUser(5e-4, 1e8);
  draw_efficiencycorrected_unfolded->GetXaxis()->SetTitleSize(0.065);
  draw_efficiencycorrected_unfolded->GetYaxis()->SetTitleSize(0.065);
  draw_efficiencycorrected_unfolded->GetXaxis()->SetTitleOffset(1.04);
  draw_efficiencycorrected_unfolded->GetYaxis()->SetTitleOffset(1.06);
  draw_efficiencycorrected_unfolded->GetXaxis()->SetLabelSize(0.055);
  draw_efficiencycorrected_unfolded->GetYaxis()->SetLabelSize(0.055);
  draw_efficiencycorrected_unfolded->GetXaxis()->SetLabelOffset(2);
  draw_efficiencycorrected_unfolded->GetXaxis()->CenterTitle();
  draw_efficiencycorrected_unfolded->GetYaxis()->CenterTitle();
  draw_efficiencycorrected_unfolded->Draw();
  draw_truth_final->Draw("same");
  myText(0.58, 0.9, 1, "#bf{#it{sPHENIX}} Simulation", 0.05);
  myText(0.58, 0.84, 1, "anti-k_{t} #kern[-0.5]{#it{R}} = 0.4", 0.05);
  myText(0.58, 0.78, 1, "|#eta^{jet}| < 0.7", 0.05);
  myText(0.58, 0.72, 1, "|z| < 60 cm", 0.05);
  myMarkerLineText(0.25, 0.2, 2, kBlack, 53, kBlack, 1, "h_unfolded (1 iteration) / h_efficiency", 0.05, true);
  myMarkerLineText(0.25, 0.12, 2, kRed, 54, kRed, 1, "h_truth", 0.05, true);
  TPad *pad_test3_2 = (TPad*)can_test3->cd(2);
  pad_test3_2->SetPad(0, 0, 1, 0.4);
  pad_test3_2->SetTopMargin(0.03);
  pad_test3_2->SetLeftMargin(0.15);
  pad_test3_2->SetBottomMargin(0.25);
  pad_test3_2->SetRightMargin(0.08);
  draw_test3->SetMarkerStyle(53);
  draw_test3->SetMarkerSize(2);
  draw_test3->SetMarkerColor(kBlack);
  draw_test3->SetLineColor(kBlack);
  draw_test3->GetXaxis()->SetTitle("p_{T}^{truth} [GeV]");
  draw_test3->GetYaxis()->SetTitle("Ratio");
  draw_test3->GetXaxis()->SetRangeUser(7, 86);
  draw_test3->GetYaxis()->SetRangeUser(0.85, 1.15);
  draw_test3->GetXaxis()->CenterTitle();
  draw_test3->GetYaxis()->CenterTitle();
  draw_test3->GetXaxis()->SetTitleOffset(0.95);
  draw_test3->GetYaxis()->SetTitleOffset(draw_efficiencycorrected_unfolded->GetYaxis()->GetTitleOffset()*4/6.);
  draw_test3->GetXaxis()->SetLabelSize(draw_efficiencycorrected_unfolded->GetXaxis()->GetLabelSize()*6/4.);
  draw_test3->GetYaxis()->SetLabelSize(draw_efficiencycorrected_unfolded->GetYaxis()->GetLabelSize()*6/4.);
  draw_test3->GetXaxis()->SetTitleSize(draw_efficiencycorrected_unfolded->GetXaxis()->GetTitleSize()*6/4.);
  draw_test3->GetYaxis()->SetTitleSize(draw_efficiencycorrected_unfolded->GetYaxis()->GetTitleSize()*6/4.);
  draw_test3->Draw();
  TLine *line_test3;
  line_test3 = new TLine(7, 1, 86, 1);
  line_test3->SetLineColor(kBlack);
  line_test3->SetLineStyle(3);
  line_test3->Draw("same");
  can_test3->SaveAs("figure_closure/halfclosure_test3.png");

}
