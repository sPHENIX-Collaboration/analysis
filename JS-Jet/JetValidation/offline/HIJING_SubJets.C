#include "sPhenixStyle.h"
#include "sPhenixStyle.C"
#include <iostream>
#include "utilities.h"
#include <TGraphErrors.h>
#include <TRandom3.h>

void HIJING_SubJets() 
{


  SetsPhenixStyle();
  TH1::SetDefaultSumw2();
  TH2::SetDefaultSumw2();

  using namespace std;{

    gStyle->SetOptStat(0);

  
    TFile * fin = TFile::Open("../macro/hist_jets.root");  
    TH1F * _h_R04_z_g_15_20 = (TH1F*)fin->Get("R04_z_g_15_20");
    _h_R04_z_g_15_20->Sumw2(); // Enable Sumw2 for this histogram
    TH1F * _h_R04_z_g_20_25 = (TH1F*)fin->Get("R04_z_g_20_25");
    _h_R04_z_g_20_25->Sumw2(); // Enable Sumw2 for this histogram
    TH1F * _h_R04_z_g_25_30 = (TH1F*)fin->Get("R04_z_g_25_30");
    _h_R04_z_g_25_30->Sumw2(); // Enable Sumw2 for this histogram
    TH1F * _h_R04_z_g_30_40 = (TH1F*)fin->Get("R04_z_g_30_40");
    _h_R04_z_g_30_40->Sumw2(); // Enable Sumw2 for this histogram
    TH1F * _h_R04_theta_g_15_20 = (TH1F*)fin->Get("R04_theta_g_15_20");
    _h_R04_theta_g_15_20->Sumw2(); // Enable Sumw2 for this histogram
    TH1F * _h_R04_theta_g_20_25 = (TH1F*)fin->Get("R04_theta_g_20_25");
    _h_R04_theta_g_20_25->Sumw2(); // Enable Sumw2 for this histogram
    TH1F * _h_R04_theta_g_25_30 = (TH1F*)fin->Get("R04_theta_g_25_30");
    _h_R04_theta_g_25_30->Sumw2(); // Enable Sumw2 for this histogram
    TH1F * _h_R04_theta_g_30_40 = (TH1F*)fin->Get("R04_theta_g_30_40");
    _h_R04_theta_g_30_40->Sumw2(); // Enable Sumw2 for this histogram
    TFile * fun = TFile::Open("../macro/hist_jets.root");  
    TH1F * _h_R04_z_sj_15_20 = (TH1F*)fun->Get("R04_z_sj_15_20");
    _h_R04_z_sj_15_20->Sumw2(); // Enable Sumw2 for this histogram
    TH1F * _h_R04_z_sj_20_25 = (TH1F*)fun->Get("R04_z_sj_20_25");
    _h_R04_z_sj_20_25->Sumw2(); // Enable Sumw2 for this histogram
    TH1F * _h_R04_z_sj_25_30 = (TH1F*)fun->Get("R04_z_sj_25_30");
    _h_R04_z_sj_25_30->Sumw2(); // Enable Sumw2 for this histogram
    TH1F * _h_R04_z_sj_30_40 = (TH1F*)fun->Get("R04_z_sj_30_40");
    _h_R04_z_sj_30_40->Sumw2(); // Enable Sumw2 for this histogram
    TH1F * _h_R04_theta_sj_15_20 = (TH1F*)fun->Get("R04_theta_sj_15_20");
    _h_R04_theta_sj_15_20->Sumw2(); // Enable Sumw2 for this histogram
    TH1F * _h_R04_theta_sj_20_25 = (TH1F*)fun->Get("R04_theta_sj_20_25");
    _h_R04_theta_sj_20_25->Sumw2(); // Enable Sumw2 for this histogram
    TH1F * _h_R04_theta_sj_25_30 = (TH1F*)fun->Get("R04_theta_sj_25_30");
    _h_R04_theta_sj_25_30->Sumw2(); // Enable Sumw2 for this histogram
    TH1F * _h_R04_theta_sj_30_40 = (TH1F*)fun->Get("R04_theta_sj_30_40");
    _h_R04_theta_sj_30_40->Sumw2(); // Enable Sumw2 for this histogram
    TH2D * correlation_theta_15_20 = (TH2D*)fun->Get("correlation_theta_15_20");
    TH2D * correlation_theta_20_25 = (TH2D*)fun->Get("correlation_theta_20_25");
    TH2D * correlation_theta_25_30 = (TH2D*)fun->Get("correlation_theta_25_30");
    TH2D * correlation_theta_30_40 = (TH2D*)fun->Get("correlation_theta_30_40");

    TH2D * correlation_z_15_20 = (TH2D*)fun->Get("correlation_z_15_20");
    TH2D * correlation_z_20_25 = (TH2D*)fun->Get("correlation_z_20_25");
    TH2D * correlation_z_25_30 = (TH2D*)fun->Get("correlation_z_25_30");
    TH2D * correlation_z_30_40 = (TH2D*)fun->Get("correlation_z_30_40");

    TCanvas * csubjetR01 = new TCanvas("csubjetR01", " ", 1500, 500);
  
    makeMultiPanelCanvasWithGap(csubjetR01, 4, 2, 0.2, 0.2, 0.2, 0.2, 0.08, 0.01);
 

    // TH1F * hdummy_jetpT = (TH1F*) _h_R04_z_g_15_20->Clone("hdummy_jetpT");
    // hdummy_jetpT->Reset();
    // hdummy_jetpT->SetAxisRange(0, 5, "Y");
  
    csubjetR01->cd(1);
    gStyle->SetErrorX(0);
    gPad->SetTickx();
    gPad->SetTicky();
    _h_R04_z_sj_15_20->SetLineStyle(1);
    _h_R04_z_sj_15_20->SetLineWidth(1);
    _h_R04_z_sj_15_20->SetTitle(" ");
    _h_R04_z_sj_15_20->SetYTitle("(1/N_{jet}) dN/dz_{sj}");
    _h_R04_z_sj_15_20->GetYaxis()->SetTitleOffset(0.8);
    _h_R04_z_sj_15_20->GetYaxis()->SetTitleSize(0.07);
    _h_R04_z_sj_15_20->GetYaxis()->SetLabelSize(0.04);
    _h_R04_z_sj_15_20->GetYaxis()->CenterTitle(true);

    _h_R04_z_sj_15_20->SetXTitle("z_{sj}");
    _h_R04_z_sj_15_20->GetXaxis()->SetTitleSize(0.07);
    _h_R04_z_sj_15_20->GetXaxis()->CenterTitle(true);
    _h_R04_z_sj_15_20->GetXaxis()->SetLabelSize(0.04);
    _h_R04_z_sj_15_20->SetMarkerSize(1.0);
    _h_R04_z_sj_15_20->SetAxisRange(0, 5.5, "Y");
    _h_R04_z_sj_15_20->SetAxisRange(0, 0.5, "X");
    //  _h_R04_z_sj_15_20 ->Draw("p E2 same");
    _h_R04_z_sj_15_20->Draw("hist C same");
    // Create a TGraphErrors for error bars
    int nbins15 = _h_R04_z_sj_15_20->GetNbinsX();
    double* x15 = new double[nbins15];
    double* y15 = new double[nbins15];
    double* ex15 = new double[nbins15];
    double* ey15 = new double[nbins15];
    for (int i = 1; i <= nbins15; ++i) {
      x15[i - 1] = _h_R04_z_sj_15_20 ->GetBinCenter(i);
      y15[i - 1] = _h_R04_z_sj_15_20 ->GetBinContent(i);
      ex15[i - 1] = 0.0; // Assumes uniform bin width
      ey15[i - 1] = _h_R04_z_sj_15_20 ->GetBinError(i);
    }
    TGraphErrors* errorGraph15 = new TGraphErrors(nbins15, x15, y15, ex15, ey15);
    errorGraph15->SetMarkerStyle(29);
    errorGraph15->SetMarkerColor(2);
    errorGraph15->Draw("p same");
    
    
    csubjetR01->cd(2);
    gStyle->SetErrorX(0);
    gPad->SetTickx();
    gPad->SetTicky();
    // cjetSpectra->cd(1)->SetLogy();
    // hdummy_jetpT->Draw();
    _h_R04_z_sj_20_25->SetTitle(" ");
    _h_R04_z_sj_20_25->SetXTitle("z_{sj}");
    _h_R04_z_sj_20_25->GetXaxis()->SetTitleSize(0.07);
    _h_R04_z_sj_20_25->GetXaxis()->CenterTitle(true);
    _h_R04_z_sj_20_25->GetXaxis()->SetLabelSize(0.04);
    _h_R04_z_sj_20_25->SetAxisRange(0, 5.5, "Y");
    _h_R04_z_sj_20_25->SetAxisRange(0, 0.5, "X");
    _h_R04_z_sj_20_25->SetMarkerSize(1.0);
    // _h_R04_z_sj_20_25->Draw("p E1 same");
    _h_R04_z_sj_20_25->Draw("hist C same");

    // Create a TGraphErrors for error bars
    int nbins20 = _h_R04_z_sj_20_25->GetNbinsX();
    double* x20 = new double[nbins20];
    double* y20 = new double[nbins20];
    double* ex20 = new double[nbins20];
    double* ey20 = new double[nbins20];
    for (int i = 1; i <= nbins20; ++i) {
      x20[i - 1] = _h_R04_z_sj_20_25->GetBinCenter(i);
      y20[i - 1] = _h_R04_z_sj_20_25->GetBinContent(i);
      ex20[i - 1] = 0.0; // Assumes uniform bin width
      ey20[i - 1] = _h_R04_z_sj_20_25->GetBinError(i);
    }
    TGraphErrors* errorGraph20 = new TGraphErrors(nbins20, x20, y20, ex20, ey20);
    errorGraph20->SetMarkerStyle(29);
    errorGraph20->SetMarkerColor(2);
    errorGraph20->Draw("p same");

    csubjetR01->cd(3);
    gStyle->SetErrorX(0);
    gPad->SetTickx();
    gPad->SetTicky();
    // cjetSpectra->cd(1)->SetLogy();
    // hdummy_jetpT->Draw();
    _h_R04_z_sj_25_30->SetMarkerStyle(29);
    _h_R04_z_sj_25_30->SetMarkerColor(2);
    _h_R04_z_sj_25_30->SetTitle(" ");
    _h_R04_z_sj_25_30->SetXTitle("z_{sj}");
    _h_R04_z_sj_25_30->GetXaxis()->SetTitleSize(0.07);
    _h_R04_z_sj_25_30->GetXaxis()->CenterTitle(true);
    _h_R04_z_sj_25_30->GetXaxis()->SetLabelSize(0.04);
    _h_R04_z_sj_25_30->SetAxisRange(0, 5.5, "Y");
    _h_R04_z_sj_25_30->SetAxisRange(0, 0.5, "X");
    _h_R04_z_sj_25_30->SetMarkerSize(1.0);
    //  _h_R04_z_sj_25_30->Draw("p E1 same");
    _h_R04_z_sj_25_30->Draw("hist C same");
    // Create a TGraphErrors for error bars
    int nbins25 = _h_R04_z_sj_25_30->GetNbinsX();
    double* x25 = new double[nbins25];
    double* y25 = new double[nbins25];
    double* ex25 = new double[nbins25];
    double* ey25 = new double[nbins25];
    for (int i = 1; i <= nbins25; ++i) {
      x25[i - 1] = _h_R04_z_sj_25_30->GetBinCenter(i);
      y25[i - 1] = _h_R04_z_sj_25_30->GetBinContent(i);
      ex25[i - 1] = 0.0; // Assumes uniform bin width
      ey25[i - 1] = _h_R04_z_sj_25_30->GetBinError(i);
    }
    TGraphErrors* errorGraph25 = new TGraphErrors(nbins25, x25, y25, ex25, ey25);
    errorGraph25->SetMarkerStyle(29);
    errorGraph25->SetMarkerColor(2);
    errorGraph25->Draw("p same");

    csubjetR01->cd(4);
    gStyle->SetErrorX(0);
    gPad->SetTickx();
    gPad->SetTicky();
    _h_R04_z_sj_30_40->SetMarkerStyle(29);
    _h_R04_z_sj_30_40->SetMarkerColor(2);
    _h_R04_z_sj_30_40->SetTitle(" ");
    _h_R04_z_sj_30_40->SetXTitle("z_{sj}");
    _h_R04_z_sj_30_40->GetXaxis()->SetTitleSize(0.07);
    _h_R04_z_sj_30_40->GetXaxis()->CenterTitle(true);
    _h_R04_z_sj_30_40->GetXaxis()->SetLabelSize(0.04);
    _h_R04_z_sj_30_40->GetXaxis()->SetTitleSize(0.07);
    _h_R04_z_sj_30_40->SetAxisRange(0, 5.5, "Y");
    _h_R04_z_sj_30_40->SetAxisRange(0, 0.5, "X");
    _h_R04_z_sj_30_40->SetMarkerSize(1.0);
    //  _h_R04_z_sj_30_40->Draw("p E1 same");
    _h_R04_z_sj_30_40->Draw("hist C same");
  
    drawText("p+p #sqrt{s} = 200 GeV", 0.3, 0.8, 14);
    drawText("anti-k_{T} R=0.4 jets, |#eta_{jet}| < 0.6", 0.3, 0.7, 14);
    drawText("R_{sj}=0.1", 0.3, 0.6, 14);

    // Create a TGraphErrors for error bars
    int nbins30 = _h_R04_z_sj_30_40->GetNbinsX();
    double* x30 = new double[nbins30];
    double* y30 = new double[nbins30];
    double* ex30 = new double[nbins30];
    double* ey30 = new double[nbins30];
    for (int i = 1; i <= nbins30; ++i) {
      x30[i - 1] = _h_R04_z_sj_30_40->GetBinCenter(i);
      y30[i - 1] = _h_R04_z_sj_30_40->GetBinContent(i);
      ex30[i - 1] = 0.0; // Assumes uniform bin width
      ey30[i - 1] = _h_R04_z_sj_30_40->GetBinError(i);
    }
    TGraphErrors* errorGraph30 = new TGraphErrors(nbins15, x30, y30, ex30, ey30);
    errorGraph30->SetMarkerStyle(29);
    errorGraph30->SetMarkerColor(2);
    errorGraph30->Draw("p same");

    csubjetR01->cd(5);
    gStyle->SetErrorX(0);
    gPad->SetTickx();
    gPad->SetTicky();
    _h_R04_theta_sj_15_20->SetMarkerStyle(29);
    _h_R04_theta_sj_15_20->SetMarkerColor(2);
    _h_R04_theta_sj_15_20->SetMarkerSize(1.0);
    _h_R04_theta_sj_15_20->SetTitle(" ");
    _h_R04_theta_sj_15_20->SetYTitle("(1/N_{jet}) dN/d#theta_{sj}");
    _h_R04_theta_sj_15_20->SetAxisRange(0, 9.0, "Y");
    _h_R04_theta_sj_15_20->SetAxisRange(0, 0.5, "X");
    _h_R04_theta_sj_15_20->GetYaxis()->SetTitleOffset(0.8);
    _h_R04_theta_sj_15_20->GetYaxis()->SetTitleSize(0.07);
    _h_R04_theta_sj_15_20->GetYaxis()->SetLabelSize(0.04);
    _h_R04_theta_sj_15_20->GetYaxis()->CenterTitle(true);

    _h_R04_theta_sj_15_20->SetXTitle("#theta_{sj}");
    _h_R04_theta_sj_15_20->GetXaxis()->SetTitleSize(0.07);
    _h_R04_theta_sj_15_20->GetXaxis()->CenterTitle(true);
    _h_R04_theta_sj_15_20->GetXaxis()->SetLabelSize(0.04);

    _h_R04_theta_sj_15_20->Draw("hist C same");
    // Create a TGraphErrors for error bars
    int nbins15t = _h_R04_theta_sj_15_20->GetNbinsX();
    double* x15t = new double[nbins15t];
    double* y15t = new double[nbins15t];
    double* ex15t = new double[nbins15t];
    double* ey15t = new double[nbins15t];
    for (int i = 1; i <= nbins15t; ++i) {
      x15t[i - 1] = _h_R04_theta_sj_15_20->GetBinCenter(i);
      y15t[i - 1] = _h_R04_theta_sj_15_20->GetBinContent(i);
      ex15t[i - 1] = 0.0; // Assumes uniform bin width
      ey15t[i - 1] = _h_R04_theta_sj_15_20->GetBinError(i);
    }
    TGraphErrors* errorGraph15t = new TGraphErrors(nbins15t, x15t, y15t, ex15t, ey15t);
    errorGraph15t->SetMarkerStyle(29);
    errorGraph15t->SetMarkerColor(2);
    errorGraph15t->Draw("p same");

    drawText("15 < p_{T} < 20 [GeV/c]", 0.6, 0.8, 12);
    
    csubjetR01->cd(6);
    gStyle->SetErrorX(0);
    gPad->SetTickx();
    gPad->SetTicky();
    // cjetSpectra->cd(5)->SetLogy();
    // hdummy_jetpT->Draw();
    _h_R04_theta_sj_20_25->SetMarkerStyle(29);
    _h_R04_theta_sj_20_25->SetMarkerColor(2);
    _h_R04_theta_sj_20_25->SetMarkerSize(1.0);
    _h_R04_theta_sj_20_25->SetTitle(" ");
    _h_R04_theta_sj_20_25->SetXTitle("#theta_{sj}");
    _h_R04_theta_sj_20_25->GetXaxis()->SetTitleSize(0.07);
    _h_R04_theta_sj_20_25->GetXaxis()->CenterTitle(true);
    _h_R04_theta_sj_20_25->GetXaxis()->SetLabelSize(0.04);
    _h_R04_theta_sj_20_25->SetAxisRange(0, 9.0, "Y");
    _h_R04_theta_sj_20_25->SetAxisRange(0, 0.5, "X");
    _h_R04_theta_sj_20_25->Draw("hist C same");
    //  _h_R04_theta_sj_20_25->Draw("p E1 same");
    // Create a TGraphErrors for error bars
    int nbins20t = _h_R04_theta_sj_20_25->GetNbinsX();
    double* x20t = new double[nbins20t];
    double* y20t = new double[nbins20t];
    double* ex20t = new double[nbins20t];
    double* ey20t = new double[nbins20t];
    for (int i = 1; i <= nbins15t; ++i) {
      x20t[i - 1] = _h_R04_theta_sj_20_25->GetBinCenter(i);
      y20t[i - 1] = _h_R04_theta_sj_20_25->GetBinContent(i);
      ex20t[i - 1] = 0.0; // Assumes uniform bin width
      ey20t[i - 1] = _h_R04_theta_sj_20_25->GetBinError(i);
    }
    TGraphErrors* errorGraph20t = new TGraphErrors(nbins20t, x20t, y20t, ex20t, ey20t);
    errorGraph20t->SetMarkerStyle(29);
    errorGraph20t->SetMarkerColor(2);
    errorGraph20t->Draw("p same");
    drawText("20 < p_{T} < 25 [GeV/c]", 0.5, 0.8, 12);

    csubjetR01->cd(7);
    gStyle->SetErrorX(0);
    gPad->SetTickx();
    gPad->SetTicky();
    // cjetSpectra->cd(5)->SetLogy();
    // hdummy_jetpT->Draw();
    _h_R04_theta_sj_25_30->SetMarkerStyle(29);
    _h_R04_theta_sj_25_30->SetMarkerColor(2);
    _h_R04_theta_sj_25_30->SetMarkerSize(1.0);
    _h_R04_theta_sj_25_30->SetTitle(" ");
    _h_R04_theta_sj_25_30->SetXTitle("#theta_{sj}");
    _h_R04_theta_sj_25_30->GetXaxis()->SetTitleSize(0.07);
    _h_R04_theta_sj_25_30->GetXaxis()->CenterTitle(true);
    _h_R04_theta_sj_25_30->GetXaxis()->SetLabelSize(0.04);
    _h_R04_theta_sj_25_30->SetAxisRange(0, 9.0, "Y");
    _h_R04_theta_sj_25_30->SetAxisRange(0, 0.5, "X");
    // _h_R04_theta_sj_25_30->Draw("p E1 same");

    _h_R04_theta_sj_25_30->Draw("hist C same");
    // Create a TGraphErrors for error bars
    int nbins25t = _h_R04_theta_sj_25_30->GetNbinsX();
    double* x25t = new double[nbins25t];
    double* y25t = new double[nbins25t];
    double* ex25t = new double[nbins25t];
    double* ey25t = new double[nbins25t];
    for (int i = 1; i <= nbins25t; ++i) {
      x25t[i - 1] = _h_R04_theta_sj_25_30->GetBinCenter(i);
      y25t[i - 1] = _h_R04_theta_sj_25_30->GetBinContent(i);
      ex25t[i - 1] = 0.0; // Assumes uniform bin width
      ey25t[i - 1] = _h_R04_theta_sj_25_30->GetBinError(i);
    }
    TGraphErrors* errorGraph25t = new TGraphErrors(nbins25t, x25t, y25t, ex25t, ey25t);
    errorGraph25t->SetMarkerStyle(29);
    errorGraph25t->SetMarkerColor(2);
    errorGraph25t->Draw("p same");
    drawText("25 < p_{T} < 30 [GeV/c]", 0.5, 0.8, 12);

    csubjetR01->cd(8);
    gStyle->SetErrorX(0);
    gPad->SetTickx();
    gPad->SetTicky();
    _h_R04_theta_sj_30_40->SetMarkerStyle(29);
    _h_R04_theta_sj_30_40->SetMarkerColor(2);
    _h_R04_theta_sj_30_40->SetMarkerSize(1.0);
    _h_R04_theta_sj_30_40->SetTitle(" ");
    _h_R04_theta_sj_30_40->SetXTitle("#theta_{sj}");
    _h_R04_theta_sj_30_40->GetXaxis()->SetTitleSize(0.07);
    _h_R04_theta_sj_30_40->GetXaxis()->CenterTitle(true);
    _h_R04_theta_sj_30_40->GetXaxis()->SetLabelSize(0.04);
    _h_R04_theta_sj_30_40->GetXaxis()->SetTitleSize(0.07);
    _h_R04_theta_sj_30_40->SetAxisRange(0, 9.0, "Y");
    _h_R04_theta_sj_30_40->SetAxisRange(0, 0.5, "X");
    //  _h_R04_theta_sj_30_40->Draw("p E1 same");
    _h_R04_theta_sj_30_40->Draw("hist C same");
    // Create a TGraphErrors for error bars
    int nbins30t = _h_R04_theta_sj_30_40->GetNbinsX();
    double* x30t = new double[nbins30t];
    double* y30t = new double[nbins30t];
    double* ex30t = new double[nbins30t];
    double* ey30t = new double[nbins30t];
    for (int i = 1; i <= nbins30t; ++i) {
      x30t[i - 1] = _h_R04_theta_sj_30_40->GetBinCenter(i);
      y30t[i - 1] = _h_R04_theta_sj_30_40->GetBinContent(i);
      ex30t[i - 1] = 0.0; // Assumes uniform bin width
      ey30t[i - 1] = _h_R04_theta_sj_30_40->GetBinError(i);
    }
    TGraphErrors* errorGraph30t = new TGraphErrors(nbins30t, x30t, y30t, ex30t, ey30t);
    errorGraph30t->SetMarkerStyle(29);
    errorGraph30t->SetMarkerColor(2);
    errorGraph30t->Draw("p same");

    drawText("30 < p_{T} < 40 [GeV/c]", 0.5, 0.8, 12);
  
    csubjetR01->SaveAs("RHIC_subjetR04_AKTB0z1.pdf", "RECREATE");


    TCanvas * csubjetR04 = new TCanvas("csubjetR04", " ", 1500, 500);
  
    makeMultiPanelCanvasWithGap(csubjetR04, 4, 2, 0.2, 0.2, 0.2, 0.2, 0.08, 0.01);
 
    csubjetR04->cd(1);
    gStyle->SetErrorX(0);
    gPad->SetTickx();
    gPad->SetTicky();
    _h_R04_z_g_15_20 ->SetLineStyle(1);
    _h_R04_z_g_15_20->SetLineWidth(1);
    _h_R04_z_g_15_20->SetTitle(" ");
    _h_R04_z_g_15_20->SetYTitle("(1/N_{jet}) dN/dz_{g}");
    _h_R04_z_g_15_20->GetYaxis()->SetTitleOffset(0.8);
    _h_R04_z_g_15_20->GetYaxis()->SetTitleSize(0.07);
    _h_R04_z_g_15_20->GetYaxis()->SetLabelSize(0.04);
    _h_R04_z_g_15_20->GetYaxis()->CenterTitle(true);

    _h_R04_z_g_15_20->SetXTitle("z_{g}");
    _h_R04_z_g_15_20->GetXaxis()->SetTitleSize(0.07);
    _h_R04_z_g_15_20->GetXaxis()->CenterTitle(true);
    _h_R04_z_g_15_20->GetXaxis()->SetLabelSize(0.04);
    _h_R04_z_g_15_20->SetMarkerSize(1.0);
    _h_R04_z_g_15_20->SetAxisRange(0, 6.5, "Y");
    _h_R04_z_g_15_20->SetAxisRange(0, 0.5, "X");
    //  _h_R04_z_g_15_20->Draw("p E2 same");
    _h_R04_z_g_15_20->Draw("hist C same");
    // Create a TGraphErrors for error bars
    int nbins15zg = _h_R04_z_g_15_20->GetNbinsX();
    double* x15zg = new double[nbins15zg];
    double* y15zg = new double[nbins15zg];
    double* ex15zg = new double[nbins15zg];
    double* ey15zg = new double[nbins15zg];
    for (int i = 1; i <= nbins15zg; ++i) {
      x15zg[i - 1] = _h_R04_z_g_15_20->GetBinCenter(i);
      y15zg[i - 1] = _h_R04_z_g_15_20->GetBinContent(i);
      ex15zg[i - 1] = 0.0; // Assumes uniform bin width
      ey15zg[i - 1] = _h_R04_z_g_15_20->GetBinError(i);
    }
    TGraphErrors* errorGraph15zg = new TGraphErrors(nbins15zg, x15zg, y15zg, ex15zg, ey15zg);
    errorGraph15zg->SetMarkerStyle(29);
    errorGraph15zg->SetMarkerColor(2);
    errorGraph15zg->Draw("p same");
  

    csubjetR04->cd(2);
    gStyle->SetErrorX(0);
    gPad->SetTickx();
    gPad->SetTicky();
    // cjetSpectra->cd(1)->SetLogy();
    // hdummy_jetpT->Draw();
    _h_R04_z_g_20_25->SetTitle(" ");
    _h_R04_z_g_20_25->SetXTitle("z_{g}");
    _h_R04_z_g_20_25->GetXaxis()->SetTitleSize(0.07);
    _h_R04_z_g_20_25->GetXaxis()->CenterTitle(true);
    _h_R04_z_g_20_25->GetXaxis()->SetLabelSize(0.04);
    _h_R04_z_g_20_25->SetAxisRange(0, 6.5, "Y");
    _h_R04_z_g_20_25->SetAxisRange(0, 0.5, "X");
    _h_R04_z_g_20_25->SetMarkerSize(1.0);
    // _h_R04_z_g_20_25->Draw("p E1 same");
    _h_R04_z_g_20_25->Draw("hist C same");

    // Create a TGraphErrors for error bars
    int nbins20zg = _h_R04_z_g_20_25->GetNbinsX();
    double* x20zg = new double[nbins20zg];
    double* y20zg = new double[nbins20zg];
    double* ex20zg = new double[nbins20zg];
    double* ey20zg = new double[nbins20zg];
    for (int i = 1; i <= nbins20; ++i) {
      x20zg[i - 1] = _h_R04_z_g_20_25->GetBinCenter(i);
      y20zg[i - 1] = _h_R04_z_g_20_25->GetBinContent(i);
      ex20zg[i - 1] = 0.0; // Assumes uniform bin width
      ey20zg[i - 1] = _h_R04_z_g_20_25->GetBinError(i);
    }
    TGraphErrors* errorGraph20zg = new TGraphErrors(nbins20, x20zg, y20zg, ex20zg, ey20zg);
    errorGraph20zg->SetMarkerStyle(29);
    errorGraph20zg->SetMarkerColor(2);
    errorGraph20zg->Draw("p same");

    csubjetR04->cd(3);
    gStyle->SetErrorX(0);
    gPad->SetTickx();
    gPad->SetTicky();
    // cjetSpectra->cd(1)->SetLogy();
    // hdummy_jetpT->Draw();
    _h_R04_z_g_25_30->SetMarkerStyle(29);
    _h_R04_z_g_25_30->SetMarkerColor(2);
    _h_R04_z_g_25_30->SetTitle(" ");
    _h_R04_z_g_25_30->SetXTitle("z_{g}");
    _h_R04_z_g_25_30->GetXaxis()->SetTitleSize(0.07);
    _h_R04_z_g_25_30->GetXaxis()->CenterTitle(true);
    _h_R04_z_g_25_30->GetXaxis()->SetLabelSize(0.04);
    _h_R04_z_g_25_30->SetAxisRange(0, 6.5, "Y");
    _h_R04_z_g_25_30->SetAxisRange(0, 0.5, "X");
    _h_R04_z_g_25_30->SetMarkerSize(1.0);
    //  _h_R04_z_g_25_30->Draw("p E1 same");
    _h_R04_z_g_25_30->Draw("hist C same");
    // Create a TGraphErrors for error bars
    int nbins25zg = _h_R04_z_g_25_30->GetNbinsX();
    double* x25zg = new double[nbins25zg];
    double* y25zg = new double[nbins25zg];
    double* ex25zg = new double[nbins25zg];
    double* ey25zg = new double[nbins25zg];
    for (int i = 1; i <= nbins25zg; ++i) {
      x25zg[i - 1] = _h_R04_z_g_25_30->GetBinCenter(i);
      y25zg[i - 1] = _h_R04_z_g_25_30->GetBinContent(i);
      ex25zg[i - 1] = 0.0; // Assumes uniform bin width
      ey25zg[i - 1] = _h_R04_z_g_25_30->GetBinError(i);
    }
    TGraphErrors* errorGraph25zg = new TGraphErrors(nbins25zg, x25zg, y25zg, ex25zg, ey25zg);
    errorGraph25zg->SetMarkerStyle(29);
    errorGraph25zg->SetMarkerColor(2);
    errorGraph25zg->Draw("p same");

    csubjetR04->cd(4);
    gStyle->SetErrorX(0);
    gPad->SetTickx();
    gPad->SetTicky();
    _h_R04_z_g_30_40->SetMarkerStyle(29);
    _h_R04_z_g_30_40->SetMarkerColor(2);
    _h_R04_z_g_30_40->SetTitle(" ");
    _h_R04_z_g_30_40->SetXTitle("z_{g}");
    _h_R04_z_g_30_40->GetXaxis()->SetTitleSize(0.07);
    _h_R04_z_g_30_40->GetXaxis()->CenterTitle(true);
    _h_R04_z_g_30_40->GetXaxis()->SetLabelSize(0.04);
    _h_R04_z_g_30_40->GetXaxis()->SetTitleSize(0.07);
    _h_R04_z_g_30_40->SetAxisRange(0, 6.5, "Y");
    _h_R04_z_g_30_40->SetAxisRange(0, 0.5, "X");
    _h_R04_z_g_30_40->SetMarkerSize(1.0);
    //  _h_R04_z_g_30_40->Draw("p E1 same");
    _h_R04_z_g_30_40->Draw("hist C same");
  
    drawText("p+p #sqrt{s} = 200 GeV", 0.4, 0.85, 12);
    drawText("anti-k_{T} R=0.4 jets, |#eta_{jet}| < 0.6", 0.4, 0.75, 12);
    drawText("#beta=0.0, z_{cut}=0.1", 0.4, 0.65, 12);
   
    // Create a TGraphErrors for error bars
    int nbins30zg = _h_R04_z_g_30_40->GetNbinsX();
    double* x30zg = new double[nbins30zg];
    double* y30zg = new double[nbins30zg];
    double* ex30zg = new double[nbins30zg];
    double* ey30zg = new double[nbins30zg];
    for (int i = 1; i <= nbins20zg; ++i) {
      x30zg[i - 1] = _h_R04_z_g_30_40->GetBinCenter(i);
      y30zg[i - 1] = _h_R04_z_g_30_40->GetBinContent(i);
      ex30zg[i - 1] = 0.0; // Assumes uniform bin width
      ey30zg[i - 1] = _h_R04_z_g_30_40->GetBinError(i);
    }
    TGraphErrors* errorGraph30zg = new TGraphErrors(nbins15zg, x30zg, y30zg, ex30zg, ey30zg);
    errorGraph30zg->SetMarkerStyle(29);
    errorGraph30zg->SetMarkerColor(2);
    errorGraph30zg->Draw("p same");

    csubjetR04->cd(5);
    gStyle->SetErrorX(0);
    gPad->SetTickx();
    gPad->SetTicky();
    _h_R04_theta_g_15_20->SetMarkerStyle(29);
    _h_R04_theta_g_15_20->SetMarkerColor(2);
    _h_R04_theta_g_15_20->SetMarkerSize(1.0);
    _h_R04_theta_g_15_20->SetTitle(" ");
    _h_R04_theta_g_15_20->SetYTitle("(1/N_{jet}) dN/dR_{g}");
    _h_R04_theta_g_15_20->SetAxisRange(0, 9.0, "Y");
    _h_R04_theta_g_15_20->SetAxisRange(0, 0.5, "X");
    _h_R04_theta_g_15_20->GetYaxis()->SetTitleOffset(0.8);
    _h_R04_theta_g_15_20->GetYaxis()->SetTitleSize(0.07);
    _h_R04_theta_g_15_20->GetYaxis()->SetLabelSize(0.04);
    _h_R04_theta_g_15_20->GetYaxis()->CenterTitle(true);

    _h_R04_theta_g_15_20->SetXTitle("R_{g}");
    _h_R04_theta_g_15_20->GetXaxis()->SetTitleSize(0.07);
    _h_R04_theta_g_15_20->GetXaxis()->CenterTitle(true);
    _h_R04_theta_g_15_20->GetXaxis()->SetLabelSize(0.04);

    _h_R04_theta_g_15_20->Draw("hist C same");
    // Create a TGraphErrors for error bars
    int nbins15tgt = _h_R04_theta_g_15_20->GetNbinsX();
    double* x15gt = new double[nbins15tgt];
    double* y15gt = new double[nbins15tgt];
    double* ex15gt = new double[nbins15tgt];
    double* ey15gt = new double[nbins15tgt];
    for (int i = 1; i <= nbins15tgt; ++i) {
      x15gt[i - 1] = _h_R04_theta_g_15_20->GetBinCenter(i);
      y15gt[i - 1] = _h_R04_theta_g_15_20->GetBinContent(i);
      ex15gt[i - 1] = 0.0; // Assumes uniform bin width
      ey15gt[i - 1] = _h_R04_theta_g_15_20->GetBinError(i);
    }
    TGraphErrors* errorGraph15gt = new TGraphErrors(nbins15tgt, x15gt, y15gt, ex15gt, ey15gt);
    errorGraph15gt->SetMarkerStyle(29);
    errorGraph15gt->SetMarkerColor(2);
    errorGraph15gt->Draw("p same");

    drawText("15 < p_{T} < 20 GeV/c", 0.7, 0.8, 12);
    
    csubjetR04->cd(6);
    gStyle->SetErrorX(0);
    gPad->SetTickx();
    gPad->SetTicky();
    // cjetSpectra->cd(5)->SetLogy();
    // hdummy_jetpT->Draw();
    _h_R04_theta_g_20_25->SetMarkerStyle(29);
    _h_R04_theta_g_20_25->SetMarkerColor(2);
    _h_R04_theta_g_20_25->SetMarkerSize(1.0);
    _h_R04_theta_g_20_25->SetTitle(" ");
    _h_R04_theta_g_20_25->SetXTitle("R_{g}");
    _h_R04_theta_g_20_25->GetXaxis()->SetTitleSize(0.07);
    _h_R04_theta_g_20_25->GetXaxis()->CenterTitle(true);
    _h_R04_theta_g_20_25->GetXaxis()->SetLabelSize(0.04);
    _h_R04_theta_g_20_25->SetAxisRange(0, 9.0, "Y");
    _h_R04_theta_g_20_25->SetAxisRange(0, 0.5, "X");
    _h_R04_theta_g_20_25->Draw("hist C same");
    //  _h_R04_theta_g_20_25->Draw("p E1 same");
    // Create a TGraphErrors for error bars
    int nbins20tgt = _h_R04_theta_g_20_25->GetNbinsX();
    double* x20gt = new double[nbins20tgt];
    double* y20gt = new double[nbins20tgt];
    double* ex20gt = new double[nbins20tgt];
    double* ey20gt = new double[nbins20tgt];
    for (int i = 1; i <= nbins15tgt; ++i) {
      x20gt[i - 1] = _h_R04_theta_g_20_25->GetBinCenter(i);
      y20gt[i - 1] = _h_R04_theta_g_20_25->GetBinContent(i);
      ex20gt[i - 1] =0.0; // Assumes uniform bin width
      ey20gt[i - 1] = _h_R04_theta_g_20_25->GetBinError(i);
    }
    TGraphErrors* errorGraph20gt = new TGraphErrors(nbins20tgt, x20gt, y20gt, ex20gt, ey20gt);
    errorGraph20gt->SetMarkerStyle(29);
    errorGraph20gt->SetMarkerColor(2);
    errorGraph20gt->Draw("p same");
    drawText("20 < p_{T} < 25 GeV/c", 0.6, 0.8, 12);

    csubjetR04->cd(7);
    gStyle->SetErrorX(0);
    gPad->SetTickx();
    gPad->SetTicky();
    // cjetSpectra->cd(5)->SetLogy();
    // hdummy_jetpT->Draw();
    _h_R04_theta_g_25_30->SetMarkerStyle(29);
    _h_R04_theta_g_25_30->SetMarkerColor(2);
    _h_R04_theta_g_25_30->SetMarkerSize(1.0);
    _h_R04_theta_g_25_30->SetTitle(" ");
    _h_R04_theta_g_25_30->SetXTitle("R_{g}");
    _h_R04_theta_g_25_30->GetXaxis()->SetTitleSize(0.07);
    _h_R04_theta_g_25_30->GetXaxis()->CenterTitle(true);
    _h_R04_theta_g_25_30->GetXaxis()->SetLabelSize(0.04);
    _h_R04_theta_g_25_30->SetAxisRange(0, 9.0, "Y");
    _h_R04_theta_g_25_30->SetAxisRange(0, 0.5, "X");
    // _h_R04_theta_g_25_30->Draw("p E1 same");

    _h_R04_theta_g_25_30->Draw("hist C same");
    // Create a TGraphErrors for error bars
    int nbins25tgt = _h_R04_theta_g_25_30->GetNbinsX();
    double* x25gt = new double[nbins25tgt];
    double* y25gt = new double[nbins25tgt];
    double* ex25gt = new double[nbins25tgt];
    double* ey25gt = new double[nbins25tgt];
    for (int i = 1; i <= nbins25tgt; ++i) {
      x25gt[i - 1] = _h_R04_theta_g_25_30->GetBinCenter(i);
      y25gt[i - 1] = _h_R04_theta_g_25_30->GetBinContent(i);
      ex25gt[i - 1] = 0.0;
      //ex25t[i - 1] = _h_R04_theta_g_25_30->GetBinWidth(i) / 2.0; // Assumes uniform bin width
      ey25gt[i - 1] = _h_R04_theta_g_25_30->GetBinError(i);
    }
    TGraphErrors* errorGraph25gt = new TGraphErrors(nbins25tgt, x25gt, y25gt, ex25gt, ey25gt);
    errorGraph25gt->SetMarkerStyle(29);
    errorGraph25gt->SetMarkerColor(2);
    errorGraph25gt->Draw("p same");
    drawText("25 < p_{T} < 30 GeV/c", 0.6, 0.8, 12);

    csubjetR04->cd(8);
    gStyle->SetErrorX(0);
    gPad->SetTickx();
    gPad->SetTicky();
    _h_R04_theta_g_30_40->SetMarkerStyle(29);
    _h_R04_theta_g_30_40->SetMarkerColor(2);
    _h_R04_theta_g_30_40->SetMarkerSize(1.0);
    _h_R04_theta_g_30_40->SetTitle(" ");
    _h_R04_theta_g_30_40->SetXTitle("R_{g}");
    _h_R04_theta_g_30_40->GetXaxis()->SetTitleSize(0.07);
    _h_R04_theta_g_30_40->GetXaxis()->CenterTitle(true);
    _h_R04_theta_g_30_40->GetXaxis()->SetLabelSize(0.04);
    _h_R04_theta_g_30_40->GetXaxis()->SetTitleSize(0.07);
    _h_R04_theta_g_30_40->SetAxisRange(0, 9.0, "Y");
    _h_R04_theta_g_30_40->SetAxisRange(0, 0.5, "X");
    //  _h_R04_theta_g_30_40->Draw("p E1 same");
    _h_R04_theta_g_30_40->Draw("hist C same");
    // Create a TGraphErrors for error bars
    int nbins30tgt = _h_R04_theta_g_30_40->GetNbinsX();
    double* x30gt = new double[nbins30tgt];
    double* y30gt = new double[nbins30tgt];
    double* ex30gt = new double[nbins30tgt];
    double* ey30gt = new double[nbins30tgt];
    for (int i = 1; i <= nbins20tgt; ++i) {
      x30gt[i - 1] = _h_R04_theta_g_30_40->GetBinCenter(i);
      y30gt[i - 1] = _h_R04_theta_g_30_40->GetBinContent(i);
      // ex30tt[i - 1] = _h_R04_theta_g_30_40->GetBinWidth(i) / 2.0; // Assumes uniform bin width
      ex30gt[i - 1] = 0.0;
      ey30gt[i - 1] = _h_R04_theta_g_30_40->GetBinError(i);
    }
    TGraphErrors* errorGraph30gt = new TGraphErrors(nbins20tgt, x30gt, y30gt, ex30gt, ey30gt);
    errorGraph30gt->SetMarkerStyle(29);
    errorGraph30gt->SetMarkerColor(2);
    errorGraph30gt->Draw("p same");

    drawText("30 < p_{T} < 40 GeV/c", 0.55, 0.8, 12);
  
    csubjetR04->SaveAs("RHIC_subjetR04_CAB0z1.pdf", "RECREATE");


    TCanvas * canvas = new TCanvas("canvas", " ", 1500, 500);
   
    makeMultiPanelCanvasWithGap(canvas, 4, 2, 0.2, 0.2, 0.2, 0.2, 0.09, 0.01);

    canvas->cd(1);
    gStyle->SetErrorX(0);
    gPad->SetTickx();
    gPad->SetTicky();
   
    correlation_z_15_20->SetMarkerStyle(29);
    correlation_z_15_20->SetMarkerColor(2);
    correlation_z_15_20->SetMarkerSize(1.0);
    correlation_z_15_20->GetXaxis()->SetTitleSize(0.07);
    correlation_z_15_20->GetYaxis()->SetTitleSize(0.07);
    correlation_z_15_20->GetXaxis()->CenterTitle(true);
    correlation_z_15_20->GetXaxis()->SetLabelSize(0.04);
    correlation_z_15_20->GetXaxis()->SetTitleSize(0.07);
    correlation_z_15_20->SetAxisRange(0, 0.6, "Y");
    correlation_z_15_20->SetAxisRange(0, 0.5, "X");
    correlation_z_15_20->SetXTitle("z_{g}");
    correlation_z_15_20->SetYTitle("z_{sj}");
    correlation_z_15_20->GetYaxis()->SetLabelSize(0.04);
    correlation_z_15_20->SetTitle(" ");
    correlation_z_15_20->Draw("colz"); // Use "colz" option to draw the color map
    drawText("15 < p_{T} < 20 GeV/c", 0.4, 0.75, 12);

   
    canvas->cd(2);
    gStyle->SetErrorX(0);
    gPad->SetTickx();
    gPad->SetTicky();
   
    correlation_z_20_25->SetMarkerStyle(29);
    correlation_z_20_25->SetMarkerColor(2);
    correlation_z_20_25->SetMarkerSize(1.0);
    correlation_z_20_25->GetXaxis()->SetTitleSize(0.07);
    correlation_z_20_25->GetXaxis()->CenterTitle(true);
    correlation_z_20_25->GetXaxis()->SetLabelSize(0.04);
    correlation_z_20_25->GetXaxis()->SetTitleSize(0.07);
    correlation_z_20_25->SetAxisRange(0, 0.6, "Y");
    correlation_z_20_25->SetAxisRange(0, 0.5, "X");
    correlation_z_20_25->SetXTitle("z_{g}");
    // correlation_z_20_25->SetYTitle("z_{sj}");
    correlation_z_20_25->SetTitle(" ");
    correlation_z_20_25->Draw("colz"); // Use "colz" option to draw the color map
    drawText("20 < p_{T} < 25 GeV/c", 0.3, 0.75, 12);

   
    canvas->cd(3);
    gStyle->SetErrorX(0);
    gPad->SetTickx();
    gPad->SetTicky();
   
    correlation_z_25_30->SetMarkerStyle(29);
    correlation_z_25_30->SetMarkerColor(2);
    correlation_z_25_30->SetMarkerSize(1.0);
    correlation_z_25_30->GetXaxis()->SetTitleSize(0.07);
    correlation_z_25_30->GetXaxis()->CenterTitle(true);
    correlation_z_25_30->GetXaxis()->SetLabelSize(0.04);
    correlation_z_25_30->GetXaxis()->SetTitleSize(0.07);
    correlation_z_25_30->SetAxisRange(0, 0.6, "Y");
    correlation_z_25_30->SetAxisRange(0, 0.5, "X");
    correlation_z_25_30->SetXTitle("z_{g}");
    // correlation_z_25_30->SetYTitle("z_{sj}");
    correlation_z_25_30->SetTitle(" ");
    correlation_z_25_30->Draw("colz"); // Use "colz" option to draw the color map

    drawText("25 < p_{T} < 30 GeV/c", 0.3, 0.75, 12);
    canvas->cd(4);
    gStyle->SetErrorX(0);
    gPad->SetTickx();
    gPad->SetTicky();
   
    correlation_z_30_40->SetMarkerStyle(29);
    correlation_z_30_40->SetMarkerColor(2);
    correlation_z_30_40->SetMarkerSize(1.0);
    correlation_z_30_40->GetXaxis()->SetTitleSize(0.07);
    correlation_z_30_40->GetXaxis()->CenterTitle(true);
    correlation_z_30_40->GetXaxis()->SetLabelSize(0.04);
    correlation_z_30_40->GetXaxis()->SetTitleSize(0.07);
    correlation_z_30_40->SetAxisRange(0, 0.6, "Y");
    correlation_z_30_40->SetAxisRange(0, 0.5, "X");
    correlation_z_30_40->SetXTitle("z_{g}");
    // correlation_z_30_40->SetYTitle("z_{sj}");
    correlation_z_30_40->SetTitle(" ");
    correlation_z_30_40->Draw("colz"); // Use "colz" option to draw the color map
   
    drawText("30 < p_{T} < 40 GeV/c", 0.3, 0.75, 12);

    canvas->cd(5);
    gStyle->SetErrorX(0);
    gPad->SetTickx();
    gPad->SetTicky();
   
   
    correlation_theta_15_20->SetMarkerStyle(29);
    correlation_theta_15_20->SetMarkerColor(2);
    correlation_theta_15_20->SetMarkerSize(1.0);
    correlation_theta_15_20->SetTitle(" ");
    correlation_theta_15_20->SetXTitle("#theta_{g}");
    correlation_theta_15_20->GetXaxis()->SetTitleSize(0.07);
    correlation_theta_15_20->GetXaxis()->CenterTitle(true);
    correlation_theta_15_20->GetXaxis()->SetLabelSize(0.04);
    correlation_theta_15_20->GetYaxis()->SetTitleSize(0.07);
    correlation_theta_15_20->SetAxisRange(0, 0.5, "Y");
    correlation_theta_15_20->SetAxisRange(0, 0.5, "X");
    correlation_theta_15_20->SetXTitle("R_{g}");
    correlation_theta_15_20->SetYTitle("#theta_{sj}");
    correlation_theta_15_20->GetYaxis()->SetLabelSize(0.04);
    correlation_theta_15_20->SetTitle(" ");
    correlation_theta_15_20->Draw("colz"); // Use "colz" option to draw the color map
   
    canvas->cd(6);
    gStyle->SetErrorX(0);
    gPad->SetTickx();
    gPad->SetTicky();

    correlation_theta_20_25->SetMarkerStyle(29);
    correlation_theta_20_25->SetMarkerColor(2);
    correlation_theta_20_25->SetMarkerSize(1.0);
    correlation_theta_20_25->SetTitle(" ");
    correlation_theta_20_25->SetXTitle("#theta_{g}");
    correlation_theta_20_25->GetXaxis()->SetTitleSize(0.07);
    correlation_theta_20_25->GetXaxis()->CenterTitle(true);
    correlation_theta_20_25->GetXaxis()->SetLabelSize(0.04);
    correlation_theta_20_25->GetXaxis()->SetTitleSize(0.07);
    correlation_theta_20_25->SetAxisRange(0, 0.5, "Y");
    correlation_theta_20_25->SetAxisRange(0, 0.5, "X");
    correlation_theta_20_25->SetXTitle("R_{g}");
    // correlation_theta_20_25->SetYTitle("#theta_{sj}");
    correlation_theta_20_25->SetTitle(" ");
    correlation_theta_20_25->Draw("colz"); // Use "colz" option to draw the color map
    drawText("p+p #sqrt{s} = 200 GeV", 0.05, 0.9, 12);
   
    canvas->cd(7);
    gStyle->SetErrorX(0);
    gPad->SetTickx();
    gPad->SetTicky();
   
    correlation_theta_25_30->SetMarkerStyle(29);
    correlation_theta_25_30->SetMarkerColor(2);
    correlation_theta_25_30->SetMarkerSize(1.0);
    correlation_theta_25_30->SetTitle(" ");
    correlation_theta_25_30->SetXTitle("#theta_{g}");
    correlation_theta_25_30->GetXaxis()->SetTitleSize(0.07);
    correlation_theta_25_30->GetXaxis()->CenterTitle(true);
    correlation_theta_25_30->GetXaxis()->SetLabelSize(0.04);
    correlation_theta_25_30->GetXaxis()->SetTitleSize(0.07);
    correlation_theta_25_30->SetAxisRange(0, 0.5, "Y");
    correlation_theta_25_30->SetAxisRange(0, 0.5, "X");
    correlation_theta_25_30->SetXTitle("R_{g}");
    // correlation_theta_25_30->SetYTitle("#theta_{sj}");
    correlation_theta_25_30->SetTitle(" ");
    correlation_theta_25_30->Draw("colz"); // Use "colz" option to draw the color map
    drawText("anti-k_{T} R=0.4 jets, |#eta_{jet}| < 0.6", 0.03, 0.9, 12);

    canvas->cd(8);
    gStyle->SetErrorX(0);
    gPad->SetTickx();
    gPad->SetTicky();
   
    correlation_theta_30_40->SetMarkerStyle(29);
    correlation_theta_30_40->SetMarkerColor(2);
    correlation_theta_30_40->SetMarkerSize(1.0);
    correlation_theta_30_40->SetTitle(" ");
    correlation_theta_30_40->GetXaxis()->SetTitleSize(0.07);
    correlation_theta_30_40->GetXaxis()->CenterTitle(true);
    correlation_theta_30_40->GetXaxis()->SetLabelSize(0.04);
    correlation_theta_30_40->SetAxisRange(0, 0.5, "Y");
    correlation_theta_30_40->SetAxisRange(0, 0.5, "X");
    correlation_theta_30_40->SetXTitle("R_{g}");
    // correlation_theta_30_40->SetYTitle("#theta_{sj}");
    correlation_theta_30_40->SetTitle(" ");
    correlation_theta_30_40->Draw("colz"); // Use "colz" option to draw the color map
    drawText("#beta=0.0, z_{cut}=0.1", 0.05, 0.9, 12);
   
   
    canvas->SaveAs("CorellBeta0z1.pdf", "RECREATE");
   
  }

}

