#include "TH2D.h"
#include "TH1D.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TLegend.h"


void plot_ntp_track_out()
{
  gROOT->SetStyle("Plain");
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);
  gStyle->SetOptTitle(0);

  double ptmax;

  TFile *fin = new TFile("root_files/fiducial0.0_search2_ntp_track_out.root"); 

  TFile *fin2; 
  bool add_2nd = true;

  TFile *fin3; 
  bool add_3rd = false;
  
  TH2D *hpt2d = 0;
  fin->GetObject("h1",hpt2d);

  TCanvas *ctemp0 = new TCanvas("ctemp0","ctemp0",5,5,800,800);
  hpt2d->Draw();


  hpt2d->FitSlicesY();
  TH1D*hptres = (TH1D*)gDirectory->Get("h1_2");
  
  TCanvas *cpt = new TCanvas("cpt","cpt",5,5,1200,800); 
  cpt->SetLeftMargin(0.2);

  hptres->GetYaxis()->SetTitleOffset(2.1);
  hptres->GetXaxis()->SetTitleOffset(1.2);
  hptres->SetMarkerStyle(20);
  hptres->SetMarkerSize(1.2);
  hptres->SetMarkerColor(kRed);
  hptres->GetXaxis()->SetRangeUser(0, 20.0);
  hptres->GetYaxis()->SetRangeUser(0.0, 0.04);
  hptres->SetTitle(";p_{T} [GeV/c];#frac{#Delta p_{T}}{p_{T}}");
  hptres->Draw("p");

  TLegend *lpd = new TLegend(0.50, 0.5, 0.85, 0.7, "", "NDC");
  lpd->SetBorderSize(1);
  lpd->SetFillColor(kWhite);
  lpd->SetFillStyle(1001);
  lpd->AddEntry(hptres,"sector fiducial 0","p");
  TH1D *hptres2 = 0;
  if(add_2nd)
    {
      fin2 = new TFile("root_files/fiducial0.5_search2_ntp_track_out.root"); 

      TH2D *hpt2d_2 = 0;
      fin2->GetObject("h1",hpt2d_2);
      hpt2d_2->FitSlicesY();
  
      TH1D*hptres2 = (TH1D*)gDirectory->Get("h1_2");
      
      hptres2->SetMarkerStyle(25);
      hptres2->SetMarkerSize(2);
      hptres2->SetMarkerColor(kBlue);
      hptres2->Draw("same p");

      lpd->AddEntry(hptres2,"sector fiducial 0.5","p");
    }

  TH1D *hptres3 = 0;
  if(add_3rd)
    {
      fin3 = new TFile("root_files/track_out_0111.root"); 
      fin3->GetObject("h1_2",hptres3);
      lpd->AddEntry(hptres3,"0111","p");
      hptres3->SetMarkerSize(1.2);
      hptres3->SetMarkerColor(kMagenta);
      hptres3->Draw("same p");
    }

  lpd->Draw();

  TH1D *hnum = 0;
  TH1D *hden = 0;
  fin->GetObject("h3_num",hnum);
  fin->GetObject("h3_den",hden);

  TH1D* heff = (TH1D*)hden->Clone("heff");;

  for(int i=1;i<=hden->GetNbinsX();++i){
    double pass = hnum->GetBinContent(i);
    double all = hden->GetBinContent(i);
    double eff = 0;
    if(all > pass)
      eff = pass/all;
    else if(all > 0)
      eff = 1.;
    //cout << " i " << i << " eff " << eff << endl; 
    //double err = BinomialError(pass, all); 
    heff->SetBinContent(i, eff);
  }
  
  TCanvas *ceff = new TCanvas("ceff","ceff",5,5,1200,800);
  ceff->SetLeftMargin(0.18);

  heff->GetYaxis()->SetTitle("Efficiency");
  heff->GetYaxis()->SetTitleOffset(1.5);
  heff->GetXaxis()->SetTitle("p_{T} (GeV/c)");
  heff->GetXaxis()->SetTitleOffset(1.2);
  heff->SetMarkerStyle(20);
  heff->SetMarkerSize(1.2);
  heff->SetMarkerColor(kRed);
  heff->GetXaxis()->SetRangeUser(0.0, 20.0);
  heff->GetYaxis()->SetRangeUser(0.0, 1.0);
  heff->Draw("p");


  TH1D *heff2 = 0;
  if(add_2nd)
    {
      fin2->GetObject("h3_eff",heff2);
      heff2->SetMarkerStyle(25);
      heff2->SetMarkerSize(1.2);
      heff2->SetMarkerColor(kBlue);
      heff2->Draw("same p");
    }

  TH1D *heff3 = 0;
  if(add_3rd)
    {
      fin3->GetObject("h3_eff",heff3);
      heff3->SetMarkerSize(1.2);
      heff3->SetMarkerColor(kMagenta);
      heff3->Draw("same p");
    }
  lpd->Draw();


  TH2D *hdca2d = 0;
  fin->GetObject("h2",hdca2d);

  TCanvas *ctemp1 = new TCanvas("ctemp1","ctemp1",5,5,800,800);
  hdca2d->Draw();

  hdca2d->FitSlicesY();

  TH1D*hdcares = (TH1D*)gDirectory->Get("h2_2");
  hdcares->Scale(1.0e4);  // converts from cm to microns

  TCanvas *cdca = new TCanvas("cdca","cdca",5,5,1200,800); 
  cdca->SetLeftMargin(0.2);

  //gPad->SetLogy(1);
  gPad->SetGrid();

  hdcares->GetYaxis()->SetTitleOffset(2.1);
  hdcares->GetYaxis()->SetTitle("DCA(r#phi) (#mu m)");
  hdcares->GetXaxis()->SetTitleOffset(1.2);
  hdcares->GetXaxis()->SetTitle("p_{T} (GeV/c)");
  hdcares->SetMarkerStyle(20);
  hdcares->SetMarkerSize(1.2);
  hdcares->SetMarkerColor(kRed);
  //hdcares->GetXaxis()->SetRangeUser(0.0, 20.0);
  //hdcares->GetYaxis()->SetRangeUser(1, 70);
  //hdcares->GetYaxis()->SetNdivisions(505);

  hdcares->DrawCopy("p");


  TH1D *hdcares2 = 0;
  if(add_2nd)
    {
      fin2->GetObject("h2_2",hdcares2);
      //hdcares2->Scale(1.0e4);  // converts from cm to microns
     
      hdcares2->SetMarkerStyle(25);
      hdcares2->SetMarkerSize(2);
      hdcares2->SetMarkerColor(kBlue);
      hdcares2->Draw("same p");
      //  hdcares2->Draw("p");
    }

  TH1D *hdcares3 = 0;
  if(add_3rd)
    {
      fin3->GetObject("h2_2",hdcares3);
      hdcares3->Scale(1.0e4);  // converts from cm to microns
     
      hdcares3->SetMarkerSize(2);
      hdcares3->SetMarkerColor(kMagenta);
      hdcares3->GetYaxis()->SetRangeUser(0.0004,0.10);
      hdcares3->Draw("same p");
    }

  lpd->Draw();

  TH2D *hdcaZ2d = 0;
  fin->GetObject("h3",hdcaZ2d);

  TCanvas *ctemp2 = new TCanvas("ctemp2","ctemp2",5,5,800,800);
  hdcaZ2d->Draw();

  hdcaZ2d->FitSlicesY();
  TH1D*hdcaZres = (TH1D*)gDirectory->Get("h3_2");
  hdcaZres->Scale(1.0e4);  // converts from cm to microns
  TCanvas *cdcaZ = new TCanvas("cdcaZ","cdcaZ",5,5,1200,800); 
  cdcaZ->SetLeftMargin(0.2);

  //gPad->SetLogy();
  gPad->SetGrid();

  hdcaZres->GetYaxis()->SetTitleOffset(2.1);
  hdcaZres->GetYaxis()->SetTitle("DCA(Z) (#mu m)");
  hdcaZres->GetXaxis()->SetTitleOffset(1.2);
  hdcaZres->GetXaxis()->SetTitle("p_{T} (GeV/c");
  hdcaZres->SetMarkerStyle(20);
  hdcaZres->SetMarkerSize(1.2);
  hdcaZres->SetMarkerColor(kRed);
  hdcaZres->GetXaxis()->SetRangeUser(0.1, 20);
  hdcaZres->GetYaxis()->SetRangeUser(0, 70);
  //hdcaZres->GetYaxis()->SetNdivisions(505);

  hdcaZres->DrawCopy("p");

  TH1D *hdcaZres2 = 0;
  if(add_2nd)
    {
      fin2->GetObject("h3_2",hdcaZres2);
      //hdcaZres2->Scale(1.0e4);  // converts from cm to microns
      hdcaZres2->SetMarkerStyle(25);
      hdcaZres2->SetMarkerSize(2);
      hdcaZres2->SetMarkerColor(kBlue);

      hdcaZres2->DrawCopy("same p");
      
    }

  TH1D *hdcaZres3 = 0;
  if(add_3rd)
    {
      fin3->GetObject("h3_2",hdcaZres3);
      hdcaZres3->Scale(1.0e4);  // converts from cm to microns
      hdcaZres3->SetMarkerSize(1.2);
      hdcaZres3->SetMarkerColor(kMagenta);
      hdcaZres3->GetYaxis()->SetRangeUser(0.0002, 0.1);
      hdcaZres3->Draw("same p");
    }

  lpd->Draw();

}

