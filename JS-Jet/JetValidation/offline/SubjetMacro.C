#include "sPhenixStyle.h"
#include "sPhenixStyle.C"
#include <iostream>
#include <TGraphErrors.h>
#include <TRandom3.h>
#include "utilities.h"

void SubjetMacro() 
{
  SetsPhenixStyle();
  TH1::SetDefaultSumw2();
  TH2::SetDefaultSumw2();

  using namespace std;{

    
    TFile *fin1 = TFile::Open("../macro/condorTest/output_data3cut.root");  
    TH1F *_h_R04_z_g_10_20 = (TH1F*)fin1->Get("R04_z_g_10_20");
    _h_R04_z_g_10_20->Sumw2(); // Enable Sumw2 for this histogram
    TH1F *_h_R04_theta_g_10_20 = (TH1F*)fin1->Get("R04_theta_g_10_20");
    _h_R04_theta_g_10_20->Sumw2(); // Enable Sumw2 for this histogram
    TH1F *_h_R04_z_sj_10_20 = (TH1F*)fin1->Get("R04_z_sj_10_20");
    _h_R04_z_sj_10_20->Sumw2(); // Enable Sumw2 for this histogram
    TH1F *_h_R04_theta_sj_10_20 = (TH1F*)fin1->Get("R04_theta_sj_10_20");
    _h_R04_theta_sj_10_20->Sumw2(); // Enable Sumw2 for this histogram
    TH2D *correlation_theta_10_20 = (TH2D*)fin1->Get("correlation_theta_10_20");
    TH2D *correlation_z_10_20 = (TH2D*)fin1->Get("correlation_z_10_20");
    
    TFile *fin2 = TFile::Open("../macro/condorTest/output_data1cut.root");
    TH1F *_h_R04_z_g_10_20_2 = (TH1F*)fin2->Get("R04_z_g_10_20");
    _h_R04_z_g_10_20_2->Sumw2(); // Enable Sumw2 for this histogram
    TH1F *_h_R04_theta_g_10_20_2 = (TH1F*)fin2->Get("R04_theta_g_10_20");
    _h_R04_theta_g_10_20_2->Sumw2(); // Enable Sumw2 for this histogram
    TH1F *_h_R04_z_sj_10_20_2 = (TH1F*)fin2->Get("R04_z_sj_10_20");
    _h_R04_z_sj_10_20_2->Sumw2(); // Enable Sumw2 for this histogram
    TH1F *_h_R04_theta_sj_10_20_2 = (TH1F*)fin2->Get("R04_theta_sj_10_20");
    _h_R04_theta_sj_10_20_2->Sumw2(); // Enable Sumw2 for this histogram


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

    std::cout << "number of jets:" << _h_R04_z_sj_10_20_2->Integral() << std::endl;

    // Normalize 10-20 hists                                                                                                                                           
    _h_R04_z_sj_10_20_2->Scale(1./_h_R04_z_sj_10_20_2->Integral());
    _h_R04_theta_sj_10_20_2->Scale(1./_h_R04_theta_sj_10_20_2->Integral());
    _h_R04_z_sj_10_20_2->Scale(1./0.05);
    _h_R04_theta_sj_10_20_2->Scale(1./0.05);
    //SoftDrop Normalization                                                                                                                                           
    // Normalize 10-20 hists                                                                                                                                           
    _h_R04_z_g_10_20_2->Scale(1./_h_R04_z_g_10_20_2->Integral());
    _h_R04_theta_g_10_20_2->Scale(1./_h_R04_theta_g_10_20_2->Integral());
    _h_R04_z_g_10_20_2->Scale(1./0.05);
    _h_R04_theta_g_10_20_2->Scale(1./0.05);

    TCanvas * Spectra04 = new TCanvas("Spectra04", " ", 500, 500);
    
    // Set marker colors for histograms from the first file
    _h_R04_z_g_10_20->SetMarkerStyle(20);
    _h_R04_z_g_10_20->SetMarkerColor(kRed);
    _h_R04_theta_g_10_20->SetMarkerStyle(20);
    _h_R04_theta_g_10_20->SetMarkerColor(kRed);
    _h_R04_z_sj_10_20->SetMarkerStyle(20);
    _h_R04_z_sj_10_20->SetMarkerColor(kRed);
    _h_R04_theta_sj_10_20->SetMarkerStyle(20);
    _h_R04_theta_sj_10_20->SetMarkerColor(kRed);
    
    // Set marker colors for histograms from the second file
    _h_R04_z_g_10_20_2->SetMarkerStyle(20);
    _h_R04_z_g_10_20_2->SetMarkerColor(kBlue);
    _h_R04_theta_g_10_20_2->SetMarkerStyle(20);
    _h_R04_theta_g_10_20_2->SetMarkerColor(kBlue);
    _h_R04_z_sj_10_20_2->SetMarkerStyle(20);
    _h_R04_z_sj_10_20_2->SetMarkerColor(kBlue);
    _h_R04_theta_sj_10_20_2->SetMarkerStyle(20);
    _h_R04_theta_sj_10_20_2->SetMarkerColor(kBlue);
   

    _h_R04_z_g_10_20->SetTitle("Momentum Fraction z_{g} Between Subjets");
    _h_R04_z_g_10_20->SetXTitle("z_{g}");
    _h_R04_z_g_10_20->SetYTitle("(1/N_{jet}) dN/dz_{g}");
    _h_R04_z_g_10_20->GetXaxis()->SetTitleSize(0.05);
    _h_R04_z_g_10_20->GetXaxis()->CenterTitle(true);
    _h_R04_z_g_10_20->GetXaxis()->SetLabelSize(0.03);
    _h_R04_z_g_10_20->GetYaxis()->SetLabelSize(0.03);
    _h_R04_z_g_10_20->GetXaxis()->SetTitleSize(0.05);
    _h_R04_z_g_10_20->SetAxisRange(0, 5.0, "Y");
    _h_R04_z_g_10_20->SetAxisRange(0, 0.5, "X");
    _h_R04_z_g_10_20->SetMarkerSize(1.0);
    _h_R04_z_g_10_20->Draw("p E");
   
    _h_R04_z_g_10_20_2->SetAxisRange(0, 0.5, "X");
    _h_R04_z_g_10_20_2->SetMarkerSize(1.0);
    _h_R04_z_g_10_20_2->Draw("p E same");

    int nbins3 = _h_R04_z_g_10_20->GetNbinsX();
    double* ex3 = new double[nbins3];
    double* ey3 = new double[nbins3];

    for (int i = 0; i < nbins3; ++i) {
      ex3[i] = 0.0; // Set x error to 0
      ey3[i] = _h_R04_z_g_10_20->GetBinError(i+1);
    }

    TGraphErrors* errorGraph3 = new TGraphErrors(nbins3, nullptr, ey3, ex3, nullptr);
    errorGraph3->Draw("P same"); // "P" to draw with markers and error bars, "same" to draw on the same canvas

    drawText("Au+Au #sqrt{s} = 200 GeV  Run 23745", 0.25, 0.87, 14);
    drawText("anti-k_{T} R=0.4 jets, |#eta_{jet}| < 0.6", 0.25, 0.84, 14);
    drawText("R_{sj}=0.1", 0.25, 0.81, 14);
    drawText("10 < p_{T} < 20 [GeV/c]", 0.25, 0.775, 14);
    drawText("#it{#bf{sPHENIX}} Preliminary", 0.25, 0.735, 14);
   
    TLegend* legend = new TLegend(0.6, 0.735, 0.8, 0.835); // Specify the position of the legend
    legend->AddEntry(_h_R04_z_sj_10_20, "z_{cut}=0.3, #beta = 2.0", "p"); // Add an entry for the first dataset
    legend->AddEntry(_h_R04_z_sj_10_20_2, "z_{cut}=0.1, #beta = 0.0", "p"); // Add an entry for the second dataset
    legend->SetBorderSize(0); // Remove the border around the legend
    legend->SetEntrySeparation(0.03);
    legend->SetTextSize(0.03); 
    legend->Draw(); // Draw the legend on the canvas
   
    Spectra04->SaveAs("Momentum_Frac_10_20_R04_data.pdf", "RECREATE");
  }
}
