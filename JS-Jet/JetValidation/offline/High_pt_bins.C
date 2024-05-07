#include "sPhenixStyle.h"
#include "sPhenixStyle.C"
#include <iostream>
#include "utilities.h"
#include <TGraphErrors.h>
#include <TRandom3.h>

void High_pt_bins() 
{


  SetsPhenixStyle();
  TH1::SetDefaultSumw2();
  TH2::SetDefaultSumw2();

  using namespace std;{

    gStyle->SetOptStat();

  
    TFile * fin = TFile::Open("../macro/condorTest/output_data3cut.root");  
    TH1F * _h_R04_z_g_10_20 = (TH1F*)fin->Get("R04_z_g_10_20");
    _h_R04_z_g_10_20->Sumw2(); // Enable Sumw2 for this histogram
    TH1F * _h_R04_theta_g_10_20 = (TH1F*)fin->Get("R04_theta_g_10_20");
    _h_R04_theta_g_10_20->Sumw2(); // Enable Sumw2 for this histogram
    //    TFile * fun = TFile::Open("../macro/condorTest/EMTree_0000.root");  
    TH1F * _h_R04_z_sj_10_20 = (TH1F*)fin->Get("R04_z_sj_10_20");
    _h_R04_z_sj_10_20->Sumw2(); // Enable Sumw2 for this histogram
    TH1F * _h_R04_theta_sj_10_20 = (TH1F*)fin->Get("R04_theta_sj_10_20");
    _h_R04_theta_sj_10_20->Sumw2(); // Enable Sumw2 for this histogram
    TH2D * correlation_theta_10_20 = (TH2D*)fin->Get("correlation_theta_10_20");
    TH2D * correlation_z_10_20 = (TH2D*)fin->Get("correlation_z_10_20");

    std::cout << "number of jets:" << _h_R04_z_sj_10_20->Integral() << std::endl;
    // Normalize 10-20 hists                                                                                                                                            
    _h_R04_z_sj_10_20->Scale(1./_h_R04_z_sj_10_20->Integral());                                                                                                         
    _h_R04_theta_sj_10_20->Scale(1./_h_R04_theta_sj_10_20->Integral());
    _h_R04_z_sj_10_20->Scale(1./0.05);  
    _h_R04_theta_sj_10_20->Scale(1./0.05);                                                                                                                                 
    //SoftDrop Normalization                                                                                                                                            
    // Normalize 10-20 hists                                                                                                                                            
    _h_R04_z_g_10_20->Scale(1./_h_R04_z_g_10_20->Integral());                                                                                                           
    _h_R04_theta_g_10_20->Scale(1./_h_R04_theta_g_10_20->Integral());                                                                                                  
    _h_R04_z_g_10_20->Scale(1./0.05);   
    _h_R04_theta_g_10_20->Scale(1./0.05); 

  TCanvas * Spectra04 = new TCanvas("Spectra04", " ", 500, 500);

  gStyle->SetErrorX();
  gPad->SetTickx();
  gPad->SetTicky();
   
    _h_R04_z_sj_10_20->SetMarkerStyle(20);
    _h_R04_z_sj_10_20->SetMarkerColor(4);
    _h_R04_z_sj_10_20->SetTitle("Momentum Fraction z_{sj} Between Subjets");
    _h_R04_z_sj_10_20->SetXTitle("z_{sj}");
    _h_R04_z_sj_10_20->SetYTitle("(1/N_{jet}) dN/dz_{sj}");
    _h_R04_z_sj_10_20->GetXaxis()->SetTitleSize(0.05);
    _h_R04_z_sj_10_20->GetXaxis()->CenterTitle(true);
    _h_R04_z_sj_10_20->GetXaxis()->SetLabelSize(0.03);
    _h_R04_z_sj_10_20->GetYaxis()->SetLabelSize(0.03);
    _h_R04_z_sj_10_20->GetXaxis()->SetTitleSize(0.05);
    _h_R04_z_sj_10_20->SetAxisRange(0, 15, "Y");
    _h_R04_z_sj_10_20->SetAxisRange(0, 0.5, "X");
    _h_R04_z_sj_10_20->SetMarkerSize(1.0);
    _h_R04_z_sj_10_20->Draw("p E1 same");
    //    _h_R04_z_sj_10_20->Draw(" ");
  
    drawText("Au+Au #sqrt{s} = 200 GeV", 0.25, 0.87, 14);
    drawText("anti-k_{T} R=0.4 jets, |#eta_{jet}| < 0.6", 0.25, 0.84, 14);
    drawText("R_{sj}=0.1", 0.25, 0.81, 14);
    drawText("10 < p_{T} < 20 [GeV/c]", 0.25, 0.77, 14);
    drawText("#it{#bf{sPHENIX}} Preliminary", 0.25, 0.74, 14);
    // Create a TGraphErrors for error bars
    int nbins30 = _h_R04_z_sj_10_20->GetNbinsX();
    double* x30 = new double[nbins30];
    double* y30 = new double[nbins30];
    double* ex30 = new double[nbins30];
    double* ey30 = new double[nbins30];
    for (int i = 1; i <= nbins30; ++i) {
      x30[i - 1] = _h_R04_z_sj_10_20->GetBinCenter(i);
      y30[i - 1] = _h_R04_z_sj_10_20->GetBinContent(i);
      ex30[i - 1] = 0.0; // Assumes uniform bin width
      ey30[i - 1] = _h_R04_z_sj_10_20->GetBinError(i);
    }
    TGraphErrors* errorGraph30 = new TGraphErrors(nbins30, x30, y30, ex30, ey30);
    errorGraph30->SetMarkerStyle(20);
    errorGraph30->SetMarkerColor(4);
    errorGraph30->Draw("p same");
    Spectra04->SaveAs("Momentum_Frac_10_20_R04_data.pdf", "RECREATE");



    TCanvas * Theta_sj = new TCanvas("Theta_sj", " ", 500, 500);
   
    gStyle->SetErrorX(0);
    gPad->SetTickx();
    gPad->SetTicky();
    _h_R04_theta_sj_10_20->SetMarkerStyle(20);
    _h_R04_theta_sj_10_20->SetMarkerColor(4);
    _h_R04_theta_sj_10_20->SetMarkerSize(1.0);
    _h_R04_theta_sj_10_20->SetTitle("Opening Angle #theta_{sj} Between Subjets ");
    _h_R04_theta_sj_10_20->SetYTitle("(1/N_{jet}) dN/d#theta_{sj}");
    _h_R04_theta_sj_10_20->SetXTitle("#theta_{sj}");
    _h_R04_theta_sj_10_20->GetXaxis()->SetTitleSize(0.05);
    _h_R04_theta_sj_10_20->GetXaxis()->CenterTitle(true);
    _h_R04_theta_sj_10_20->GetXaxis()->SetLabelSize(0.03);
    _h_R04_theta_sj_10_20->GetYaxis()->SetLabelSize(0.03);
    _h_R04_theta_sj_10_20->GetXaxis()->SetTitleSize(0.05);
    _h_R04_theta_sj_10_20->SetAxisRange(0, 6.5, "Y");
    _h_R04_theta_sj_10_20->SetAxisRange(0, 0.5, "X");
    _h_R04_theta_sj_10_20->Draw("p E1 same");
    // _h_R04_theta_sj_10_20->Draw(" ");
    drawText("Au+Au #sqrt{s} = 200 GeV", 0.25, 0.87, 14);
    drawText("anti-k_{T} R=0.4 jets, |#eta_{jet}| < 0.6", 0.25, 0.84, 14);
    drawText("R_{sj}=0.1", 0.25, 0.81, 14);
    drawText("10 < p_{T} < 20 [GeV/c]", 0.25, 0.77, 14);
    drawText("#it{#bf{sPHENIX}} Preliminary", 0.25, 0.74, 14);
    // Create a TGraphErrors for error bars
    int nbins30t = _h_R04_theta_sj_10_20->GetNbinsX();
    double* x30t = new double[nbins30t];
    double* y30t = new double[nbins30t];
    double* ex30t = new double[nbins30t];
    double* ey30t = new double[nbins30t];
    for (int i = 1; i <= nbins30t; ++i) {
      x30t[i - 1] = _h_R04_theta_sj_10_20->GetBinCenter(i);
      y30t[i - 1] = _h_R04_theta_sj_10_20->GetBinContent(i);
      ex30t[i - 1] = 0.0; // Assumes uniform bin width
      ey30t[i - 1] = _h_R04_theta_sj_10_20->GetBinError(i);
    }
    TGraphErrors* errorGraph30t = new TGraphErrors(nbins30t, x30t, y30t, ex30t, ey30t);
    errorGraph30t->SetMarkerStyle(20);
    errorGraph30t->SetMarkerColor(4);
    errorGraph30t->Draw("p same");

  
    Theta_sj->SaveAs("Radius_sj_10_20_R04_data.pdf", "RECREATE");

    TCanvas * Groomed_frac = new TCanvas("Groomed_frac"," ", 500, 500);
    
    gStyle->SetErrorX(0);
    gPad->SetTickx();
    gPad->SetTicky();
    _h_R04_z_g_10_20->SetMarkerStyle(20);
    _h_R04_z_g_10_20->SetMarkerColor(2);
    _h_R04_z_g_10_20->SetTitle("Groomed Momentum Fraction Between Subjets");
    _h_R04_z_g_10_20->SetXTitle("z_{g}");
    _h_R04_z_g_10_20->SetYTitle("(1/N_{jet}) dN/dz_{g}");;
    _h_R04_z_g_10_20->GetXaxis()->SetTitleSize(0.05);
    _h_R04_z_g_10_20->GetXaxis()->CenterTitle(true);
    _h_R04_z_g_10_20->GetXaxis()->SetLabelSize(0.03);
    _h_R04_z_g_10_20->GetYaxis()->SetLabelSize(0.03);
    _h_R04_z_g_10_20->GetXaxis()->SetTitleSize(0.05);
    _h_R04_z_g_10_20->SetAxisRange(0, 4.5, "Y");
    _h_R04_z_g_10_20->SetAxisRange(0, 0.5, "X");
    _h_R04_z_g_10_20->SetMarkerSize(1.0);
    _h_R04_z_g_10_20->Draw("p E1 same");
    // _h_R04_z_g_10_20->Draw(" ");
  
    drawText("Au+Au #sqrt{s} = 200 GeV", 0.25, 0.87, 14);
    drawText("anti-k_{T} R=0.4 jets, |#eta_{jet}| < 0.6", 0.25, 0.84, 14);
    drawText("#beta=2.0, z_{cut}=0.3", 0.25, 0.81, 14);
    drawText("10 < p_{T} < 20 [GeV/c]", 0.25, 0.77, 14);
    drawText("#it{#bf{sPHENIX}} Preliminary", 0.25, 0.74, 14);
    // Create a TGraphErrors for error bars
    int nbins30zg = _h_R04_z_g_10_20->GetNbinsX();
    double* x30zg = new double[nbins30zg];
    double* y30zg = new double[nbins30zg];
    double* ex30zg = new double[nbins30zg];
    double* ey30zg = new double[nbins30zg];
    for (int i = 1; i <= nbins30zg; ++i) {
      x30zg[i - 1] = _h_R04_z_g_10_20->GetBinCenter(i);
      y30zg[i - 1] = _h_R04_z_g_10_20->GetBinContent(i);
      ex30zg[i - 1] = 0.0; // Assumes uniform bin width
      ey30zg[i - 1] = _h_R04_z_g_10_20->GetBinError(i);
    }
    TGraphErrors* errorGraph30zg = new TGraphErrors(nbins30zg, x30zg, y30zg, ex30zg, ey30zg);
    errorGraph30zg->SetMarkerStyle(20);
    errorGraph30zg->SetMarkerColor(2);
    errorGraph30zg->Draw("p same");

    Groomed_frac->SaveAs("Groomed_frac_10_20_R04_data.pdf","RECREATE");


    TCanvas * Groomed_rad = new TCanvas("Groomed_rad"," ", 500, 500);
    gStyle->SetErrorX(0);
    gPad->SetTickx();
    gPad->SetTicky();
    _h_R04_theta_g_10_20->SetMarkerStyle(20);
    _h_R04_theta_g_10_20->SetMarkerColor(2);
    _h_R04_theta_g_10_20->SetMarkerSize(1.0);
    _h_R04_theta_g_10_20->SetTitle("Groomed Radius R_{g} Between Subjets");
    _h_R04_theta_g_10_20->SetYTitle("(1/N_{jet}) dN/d#theta_{g}");
    _h_R04_theta_g_10_20->SetXTitle("R_{g}");
    _h_R04_theta_g_10_20->GetXaxis()->SetTitleSize(0.05);
    _h_R04_theta_g_10_20->GetXaxis()->CenterTitle(true);
    _h_R04_theta_g_10_20->GetXaxis()->SetLabelSize(0.03);
    _h_R04_theta_g_10_20->GetYaxis()->SetLabelSize(0.03);
    _h_R04_theta_g_10_20->GetXaxis()->SetTitleSize(0.05);
    _h_R04_theta_g_10_20->SetAxisRange(0, 6.5, "Y");
    _h_R04_theta_g_10_20->SetAxisRange(0, 0.5, "X");
    _h_R04_theta_g_10_20->Draw("p E1 same");
     // _h_R04_theta_g_10_20->Draw(" ");
    drawText("Au+Au #sqrt{s} = 200 GeV", 0.25, 0.87, 14);
    drawText("anti-k_{T} R=0.4 jets, |#eta_{jet}| < 0.6", 0.25, 0.84, 14);
    drawText("#beta=2.0, z_{cut}=0.3", 0.25, 0.81, 14);
    drawText("10 < p_{T} < 20 GeV/c", 0.25, 0.77, 14);
    drawText("#it{#bf{sPHENIX}} Preliminary", 0.25, 0.74, 14);

    // Create a TGraphErrors for error bars
    int nbins30tgt = _h_R04_theta_g_10_20->GetNbinsX();
    double* x30gt = new double[nbins30tgt];
    double* y30gt = new double[nbins30tgt];
    double* ex30gt = new double[nbins30tgt];
    double* ey30gt = new double[nbins30tgt];
    for (int i = 1; i <= nbins30tgt; ++i) {
      x30gt[i - 1] = _h_R04_theta_g_10_20->GetBinCenter(i);
      y30gt[i - 1] = _h_R04_theta_g_10_20->GetBinContent(i);
      // ex30tt[i - 1] = _h_R04_theta_g_10_20->GetBinWidth(i) / 2.0; // Assumes uniform bin width
      ex30gt[i - 1] = 0.0;
      ey30gt[i - 1] = _h_R04_theta_g_10_20->GetBinError(i);
    }
    TGraphErrors* errorGraph30gt = new TGraphErrors(nbins30tgt, x30gt, y30gt, ex30gt, ey30gt);
    errorGraph30gt->SetMarkerStyle(20);
    errorGraph30gt->SetMarkerColor(2);
    errorGraph30gt->Draw("p same");
    Groomed_rad->SaveAs("Groomed_rad_10_20_R04_data.pdf", "RECREATE");

    TCanvas * canvas = new TCanvas("canvas", " ", 500, 500);
    gStyle->SetErrorX(0);
    gPad->SetTickx();
    gPad->SetTicky();   
    correlation_z_10_20->SetMarkerStyle(20);
    correlation_z_10_20->SetMarkerColor(2);
    correlation_z_10_20->SetMarkerSize(1.0);
    correlation_z_10_20->GetXaxis()->SetTitleSize(0.05);
    correlation_z_10_20->GetXaxis()->CenterTitle(true);
    correlation_z_10_20->GetXaxis()->SetLabelSize(0.03);
    correlation_z_10_20->GetYaxis()->SetLabelSize(0.03);
    correlation_z_10_20->GetXaxis()->SetTitleSize(0.05);
    correlation_z_10_20->SetAxisRange(0, 0.6, "Y");
    correlation_z_10_20->SetAxisRange(0, 0.5, "X");
    correlation_z_10_20->SetXTitle("z_{g}");
    correlation_z_10_20->SetYTitle("z_{sj}");
    correlation_z_10_20->SetTitle("Correlation Between z_{sj} & z_{g} ");
    correlation_z_10_20->Draw("colz"); // Use "colz" option to draw the color map
    drawText("Au+Au #sqrt{s} = 200 GeV", 0.2, 0.30, 14);
    drawText("anti-k_{T} R=0.4 jets, |#eta_{jet}| < 0.6", 0.2, 0.27, 14);
    drawText("#beta=2.0, z_{cut}=0.3", 0.2, 0.24, 14);
    drawText("10 < p_{T} < 20 GeV/c", 0.2, 0.21, 14);
    drawText("#it{#bf{sPHENIX}} Preliminary", 0.2, 0.17, 14);

    canvas->SaveAs("Correlation_moment_fracR04_data.pdf","RECREATE");

    TCanvas * canvas1 = new TCanvas("canvas1"," ", 500, 500);
    gStyle->SetErrorX(0);
    gPad->SetTickx();
    gPad->SetTicky();
   
    correlation_theta_10_20->SetMarkerStyle(20);
    correlation_theta_10_20->SetMarkerColor(2);
    correlation_theta_10_20->SetMarkerSize(1.0);
    correlation_theta_10_20->SetTitle(" ");
    correlation_theta_10_20->GetXaxis()->SetTitleSize(0.05);
    correlation_theta_10_20->GetXaxis()->CenterTitle(true);
    correlation_theta_10_20->GetXaxis()->SetLabelSize(0.03);
    correlation_theta_10_20->GetYaxis()->SetLabelSize(0.03);
    correlation_theta_10_20->SetAxisRange(0, 0.5, "Y");
    correlation_theta_10_20->SetAxisRange(0, 0.5, "X");
    correlation_theta_10_20->SetXTitle("R_{g}");
    // correlation_theta_10_20->SetYTitle("#theta_{sj}");
    correlation_theta_10_20->SetTitle("Correlation Between #theta_{sj} & R_{g}");
    correlation_theta_10_20->Draw("colz"); // Use "colz" option to draw the color map
    drawText("Au+Au #sqrt{s} = 200 GeV", 0.2, 0.89, 14);
    drawText("anti-k_{T} R=0.4 jets, |#eta_{jet}| < 0.6", 0.2, 0.86, 14);
    drawText("#beta=2.0, z_{cut}=0.3", 0.2, 0.83, 14);
    drawText("10 < p_{T} < 20 GeV/c", 0.2, 0.80, 14);
    drawText("#it{#bf{sPHENIX}} Preliminary", 0.2, 0.76, 14);
   
    canvas1->SaveAs("Corellation_radialR04_data.pdf", "RECREATE");
   
  }

}

