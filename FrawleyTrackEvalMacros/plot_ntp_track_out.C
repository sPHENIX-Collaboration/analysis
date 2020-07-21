#include "TH2D.h"
#include "TH1D.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TLegend.h"
#include <iostream>

void plot_ntp_track_out()
{
  gROOT->SetStyle("Plain");
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(1);
  gStyle->SetOptTitle(0);

  double ptmax;

  TCanvas *ctemp0 = new TCanvas("ctemp0","ctemp0",5,5,800,800);
  ctemp0->Divide(2,2);

  double slice_low = 5.0;
  double slice_high = 6.0;

  TFile *fin = new TFile("root_files/ntp_track_out.root"); 
  TH2D *hpt2d = 0;
  fin->GetObject("h1",hpt2d);
  hpt2d->FitSlicesY();
  TH1D*hptres = (TH1D*)gDirectory->Get("h1_2");
  TH1D*hptcent = (TH1D*)gDirectory->Get("h1_1");
  ctemp0->cd(1);
  hpt2d->Draw("colz");

  int binlow = hpt2d->GetXaxis()->FindBin(slice_low);
  int binhigh = hpt2d->GetXaxis()->FindBin(slice_high);
  TH1D *hptslice = hpt2d->ProjectionY("hptslice",binlow, binhigh);

  TFile *fin2 = 0; 
  TH2D *hpt2d_2 = 0;
 TH1D *hptres2 = 0;
 TH1D *hptcent2 = 0;
 TH1D *hptslice_2 = 0;
  bool add_2nd = false;
  if(add_2nd)
    {
      fin2 = new TFile("root_files/genfit_electrons_ntp_track_out.root");     
 
      fin2->GetObject("h1",hpt2d_2);
      hpt2d_2->FitSlicesY();
      hptres2 = (TH1D*)gDirectory->Get("h1_2");
      hptcent2 = (TH1D*)gDirectory->Get("h1_1");
      ctemp0->cd(2);
      hpt2d_2->Draw("colz");
	
      int binlow = hpt2d_2->GetXaxis()->FindBin(slice_low);
      int binhigh = hpt2d_2->GetXaxis()->FindBin(slice_high);
      hptslice_2 = hpt2d_2->ProjectionY("hptslice_2",binlow, binhigh);
      hptslice_2->SetLineColor(kRed);      
    }

  TFile *fin3 = 0; 
  TH2D *hpt2d_3 = 0;
  TH1D *hptres3 = 0;
  TH1D *hptcent3 = 0;
 TH1D *hptslice_3 = 0;
  bool add_3rd = false;
  if(add_3rd)
    {
      fin3 = new TFile("root_files/genfit_pions_ntp_track_out.root");     
      fin3->GetObject("h1",hpt2d_3);
      hpt2d_3->FitSlicesY();
      hptres3 = (TH1D*)gDirectory->Get("h1_2");
      hptcent3 = (TH1D*)gDirectory->Get("h1_1");
      ctemp0->cd(3);
      hpt2d_3->Draw("colz");

      int binlow = hpt2d_3->GetXaxis()->FindBin(slice_low);
      int binhigh = hpt2d_3->GetXaxis()->FindBin(slice_high);
      hptslice_3 = hpt2d_3->ProjectionY("hptslice_2",binlow, binhigh);
      hptslice_3->SetLineColor(kBlue);      
    }

  TFile *fin4 = 0; 
  TH2D *hpt2d_4 = 0;
  TH1D *hptres4 = 0;
  TH1D *hptcent4 = 0;
  bool add_4th = false;
  if(add_4th)
    {
      fin4 = new TFile("root_files/acts_geantino_rerun_ntp_track_out.root");   
      //fin4 = new TFile("root_files/acts_geantinos_rerun_neg_ntp_track_out.root");   
      fin4->GetObject("h1",hpt2d_4);
      hpt2d_4->FitSlicesY();
      hptres4 = (TH1D*)gDirectory->Get("h1_2");
      hptcent4 = (TH1D*)gDirectory->Get("h1_1");
      ctemp0->cd(4);
      hpt2d_4->Draw("colz");
    }

  TCanvas *cslice = new TCanvas("cslice","cslice",5,5,1600,800);
  cslice->Divide(3,1);
  cslice->cd(1);
  hptslice->Draw();
  hptslice->Fit("gaus");

  if(add_2nd)
    {
      cslice->cd(2);
      hptslice_2->Draw("same");
      hptslice_2->Fit("gaus","","", -0.03, 0.05);
    }
    if(add_3rd)
      {
	cslice->cd(3);
	hptslice_3->Draw("same");
	hptslice_3->Fit("gaus");
      }

  TCanvas *cpt = new TCanvas("cpt","cpt",5,5,1500,800); 
  cpt->Divide(2,1);

  cpt->cd(1);
  gPad->SetLeftMargin(0.2);
  hptres->GetYaxis()->SetTitleOffset(2.1);
  hptres->GetXaxis()->SetTitleOffset(1.2);
  hptres->SetMarkerStyle(20);
  hptres->SetMarkerSize(1.2);
  hptres->SetMarkerColor(kRed);
  hptres->GetXaxis()->SetRangeUser(0, 20.0);
  hptres->GetYaxis()->SetRangeUser(0.0, 0.10);
  hptres->SetTitle(";p_{T} [GeV/c];#frac{#Delta p_{T}}{p_{T}} (resolution)");
  hptres->DrawCopy("p");

  cpt->cd(2);
  gPad->SetLeftMargin(0.2);
  hptcent->GetYaxis()->SetTitleOffset(2.1);
  hptcent->GetXaxis()->SetTitleOffset(1.2);
  hptcent->SetMarkerStyle(20);
  hptcent->SetMarkerSize(1.2);
  hptcent->SetMarkerColor(kRed);
  hptcent->GetXaxis()->SetRangeUser(0, 20.0);
  hptcent->GetYaxis()->SetRangeUser(-0.1, 0.1);
  hptcent->SetTitle(";p_{T} [GeV/c];#frac{#Delta p_{T}}{p_{T}} (offset)");
  hptcent->DrawCopy("p");

  TLegend *lpd = new TLegend(0.5, 0.6, 0.7, 0.75, "", "NDC");
  lpd->SetBorderSize(1);
  lpd->SetFillColor(kWhite);
  lpd->SetFillStyle(1001);
  lpd->AddEntry(hptres,"Genfit geantino","p");

  if(add_2nd)
    {
      cpt->cd(1);
      hptres2->SetMarkerStyle(20);
      hptres2->SetMarkerSize(1.2);
      hptres2->SetMarkerColor(kBlue);
      hptres2->DrawCopy("same p");

      cpt->cd(2);
      hptcent2->SetMarkerStyle(20);
      hptcent2->SetMarkerSize(1.2);
      hptcent2->SetMarkerColor(kBlue);
      hptcent2->DrawCopy("same p");

      lpd->AddEntry(hptres2,"Genfit electron","p");
    }

  if(add_3rd)
    {
      cpt->cd(1);
      hptres3->SetMarkerSize(1.2);
      hptres3->SetMarkerStyle(24);
      hptres3->SetMarkerColor(kMagenta);
      hptres3->DrawCopy("same p");

      cpt->cd(2);
      hptcent3->SetMarkerSize(1.2);
      hptcent3->SetMarkerStyle(24);
      hptcent3->SetMarkerColor(kMagenta);
      hptcent3->DrawCopy("same p");

      lpd->AddEntry(hptres3,"Genfit pion","p");
    }

  if(add_4th)
    {
      cpt->cd(1);
      hptres4->SetMarkerSize(1.2);
      hptres4->SetMarkerStyle(24);
      hptres4->SetMarkerColor(kBlue);
      hptres4->DrawCopy("same p");

      cpt->cd(2);
      hptcent4->SetMarkerSize(1.2);
      hptcent4->SetMarkerStyle(24);
      hptcent4->SetMarkerColor(kBlue);
      hptcent4->DrawCopy("same p");

      lpd->AddEntry(hptres4,"Acts geantino","p");
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
  heff->DrawCopy("p");


  TH1D *heff2 = 0;
  if(add_2nd)
    {
      fin2->GetObject("h3_eff",heff2);
      heff2->SetMarkerStyle(20);
      heff2->SetMarkerSize(1.2);
      heff2->SetMarkerColor(kBlue);
      heff2->DrawCopy("same p");
    }

  TH1D *heff3 = 0;
  if(add_3rd)
    {
      fin3->GetObject("h3_eff",heff3);
      heff3->SetMarkerStyle(24);
      heff3->SetMarkerSize(1.2);
      heff3->SetMarkerColor(kMagenta);
      heff3->DrawCopy("same p");
    }

  TH1D *heff4 = 0;
  if(add_4th)
    {
      fin4->GetObject("h3_eff",heff4);
      heff4->SetMarkerStyle(24);
      heff4->SetMarkerSize(1.2);
      heff4->SetMarkerColor(kBlue);
      heff4->DrawCopy("same p");
    }
  lpd->Draw();

  TCanvas *ctemp1 = new TCanvas("ctemp1","ctemp1",5,5,800,800);
  ctemp1->Divide(2,2);

  TH2D *hdca2d = 0;
  fin->GetObject("h2",hdca2d);
  ctemp1->cd(1);
  hdca2d->Draw("colz");

  TH2D *hdca2d2 = 0;
  if(add_2nd)
    {
      fin2->GetObject("h2",hdca2d2);
      ctemp1->cd(2);
      hdca2d2->Draw("colz");
    }

  TH2D *hdca2d3 = 0;
  if(add_3rd)
    {
      fin3->GetObject("h2",hdca2d3);
      ctemp1->cd(3);
      hdca2d3->Draw("colz");
    }

  TH2D *hdca2d4 = 0;
  if(add_4th)
    {
      fin4->GetObject("h2",hdca2d4);
      ctemp1->cd(4);
      hdca2d4->Draw("colz");
    }

  TCanvas *cdca = new TCanvas("cdca","cdca",5,5,1200,800); 
  cdca->SetLeftMargin(0.2);
  gPad->SetGrid();

  hdca2d->FitSlicesY();
  TH1D*hdcares = (TH1D*)gDirectory->Get("h2_2");
  hdcares->GetYaxis()->SetTitleOffset(2.1);
  hdcares->GetYaxis()->SetTitle("DCA(r#phi) (cm)");
  hdcares->GetXaxis()->SetTitleOffset(1.2);
  hdcares->GetXaxis()->SetTitle("p_{T} (GeV/c)");
  hdcares->SetMarkerStyle(20);
  hdcares->SetMarkerSize(1.2);
  hdcares->SetMarkerColor(kRed);
  hdcares->GetYaxis()->SetRangeUser(0, 0.005);
  hdcares->DrawCopy("p");


  TH1D *hdcares2 = 0;
  if(add_2nd)
    {
      hdca2d2->FitSlicesY();
      hdcares2 = (TH1D*)gDirectory->Get("h2_2");     
      hdcares2->SetMarkerStyle(20);
      hdcares2->SetMarkerSize(1.2);
      hdcares2->SetMarkerColor(kBlue);
      hdcares2->DrawCopy("same p");
    }

  TH1D *hdcares3 = 0;
  if(add_3rd)
    {
      hdca2d3->FitSlicesY();
      hdcares3 = (TH1D*)gDirectory->Get("h2_2");
      hdcares3->SetMarkerSize(1.2);
      hdcares3->SetMarkerStyle(24);
      hdcares3->SetMarkerColor(kMagenta);
      hdcares3->DrawCopy("same p");  
   }

  TH1D *hdcares4 = 0;
  if(add_4th)
    {
      hdca2d4->FitSlicesY();
      hdcares4 = (TH1D*)gDirectory->Get("h2_2");
      hdcares4->SetMarkerSize(1.2);
      hdcares4->SetMarkerStyle(24);
      hdcares4->SetMarkerColor(kBlue);
      hdcares4->DrawCopy("same p");      
   }

  lpd->Draw();

  // 2D dcaZ plots
  //===========

  TCanvas *ctemp2 = new TCanvas("ctemp2","ctemp2",5,5,800,800);
  ctemp2->Divide(2,2);

  TH2D *hdcaZ2d = 0;
  fin->GetObject("h3",hdcaZ2d);
  ctemp2->cd(1);
  hdcaZ2d->Draw("colz");

  TH2D *hdcaZ2d2 = 0;
  if(add_2nd)
    {
      fin2->GetObject("h3",hdcaZ2d2);
      ctemp2->cd(2);
      hdcaZ2d2->Draw("colz");
    }

  TH2D *hdcaZ2d3 = 0;
  if(add_3rd)
    {
      fin3->GetObject("h3",hdcaZ2d3);      
      ctemp2->cd(3);
      hdcaZ2d3->Draw("colz");
    }

  TH2D *hdcaZ2d4 = 0;
  if(add_4th)
    {
      fin4->GetObject("h3",hdcaZ2d4);      
      ctemp2->cd(4);
      hdcaZ2d4->Draw("colz");
    }

  // 1D dcaZ plots
  //===========

  TCanvas *cdcaZ = new TCanvas("cdcaZ","cdcaZ",5,5,1200,800); 
  cdcaZ->SetLeftMargin(0.2);
  gPad->SetGrid();

  hdcaZ2d->FitSlicesY();
  TH1D*hdcaZres = (TH1D*)gDirectory->Get("h3_2");

  hdcaZres->GetYaxis()->SetTitleOffset(2.1);
  hdcaZres->GetYaxis()->SetTitle("DCA(Z) (cm)");
  hdcaZres->GetXaxis()->SetTitleOffset(1.2);
  hdcaZres->GetXaxis()->SetTitle("p_{T} (GeV/c");
  hdcaZres->SetMarkerStyle(20);
  hdcaZres->SetMarkerSize(1.2);
  hdcaZres->SetMarkerColor(kRed);
  hdcaZres->GetYaxis()->SetRangeUser(0, 0.005);

  hdcaZres->DrawCopy("p");

  TH1D *hdcaZres2 = 0;
  if(add_2nd)
    {
      hdcaZ2d2->FitSlicesY();
      hdcaZres2 = (TH1D*)gDirectory->Get("h3_2");
      hdcaZres2->SetMarkerStyle(20);
      hdcaZres2->SetMarkerSize(1.2);
      hdcaZres2->SetMarkerColor(kBlue);
      hdcaZres2->DrawCopy("same p");
    }

  TH1D *hdcaZres3 = 0;
  if(add_3rd)
    {
      hdcaZ2d3->FitSlicesY();
      hdcaZres3 = (TH1D*)gDirectory->Get("h3_2");
      hdcaZres3->SetMarkerStyle(24);
      hdcaZres3->SetMarkerSize(1.2);
      hdcaZres3->SetMarkerColor(kMagenta);
      hdcaZres3->DrawCopy("same p");
    }

  TH1D *hdcaZres4 = 0;
  if(add_4th)
    {
      fin4->GetObject("h3",hdcaZ2d4);      
      hdcaZ2d4->FitSlicesY();
      hdcaZres4 = (TH1D*)gDirectory->Get("h3_2");
      hdcaZres4->SetMarkerStyle(24);
      hdcaZres4->SetMarkerSize(1.2);
      hdcaZres4->SetMarkerColor(kBlue);
      hdcaZres4->DrawCopy("same p");
    }

  lpd->Draw();

}

