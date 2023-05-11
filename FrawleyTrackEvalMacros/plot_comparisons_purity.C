#include <TH2D.h>
#include <TFile.h>
#include <TF1.h>
#include <TF2.h>
#include <TGraph.h>
#include  <TCanvas.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TLegend.h>
#include <TLatex.h>
#include <TLine.h>
#include <TColor.h>

#include "sPhenixStyle.C"

void plot_comparisons_purity()
{

  SetsPhenixStyle();

  /*
  gROOT->SetStyle("Plain");
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);
  gStyle->SetOptTitle(0);
  */

  bool toymodel = false;

  static const int NPLOTS = 2;
  double hptmax = 40.0;

  TFile *fin[NPLOTS];

  char label[NPLOTS][500]; 


  //fin[0] = new TFile("root_files/mar1_100pions_1ups_look_purity_out.root");  
 



  /*
  fin[0] = new TFile("root_files/mar1_100pions_1ups_look_purity_out.root");  
  fin[1] = new TFile("root_files/mar3_100pions_nomvtx_primvtxfit_look_purity_out.root");  
  sprintf(label[0], "100 pions only");
  sprintf(label[1], "100 pions only, no MVTX");
  */

  fin[0] = new TFile("root_files/look_purity_out_100pions_80ns_baseline_noMVTX.root");  
  fin[1] = new TFile("root_files/look_purity_out_central_167khz_80ns_1-5");  
  sprintf(label[0], "100pions");
  sprintf(label[1], "Central+store average lumi+100pions");



   
 
  TGraph *gdca[NPLOTS];
  TGraph *gdcaZ[NPLOTS];
  TGraph *geff[NPLOTS];
  TGraph *grdpt[NPLOTS];

  int col[3] = {kRed, kBlue, kBlack};

  for(int i=0;i<NPLOTS;i++)
    {
      if(!fin[i])
	{
	  cout << "Did not find file " << i << "  quit!" << endl;
	  exit(1);
	}

      fin[i]->GetObject("single_track_efficiency",geff[i]);
      fin[i]->GetObject("dca2d_resolution",gdca[i]);
      fin[i]->GetObject("dcaZ_resolution",gdcaZ[i]);
      fin[i]->GetObject("pt_resolution",grdpt[i]);
      
      if(!geff[i])
	{
	  cout << "Failed to find geff for " << i  << endl;
	  //exit(1);
	}

      if(!gdca[i])
	{
	  cout << "Failed to find gdca for " << i  << endl;
	  exit(1);
	}
      if(!gdcaZ[i])
	{
	  cout << "Failed to find gdcaZ for " << i  << endl;
	  exit(1);
	}
      if(!grdpt[i])
	{
	  cout << "Failed to find grdpt for " << i  << endl;
	  exit(1);
	}
    }


  char lab[500];
  //sprintf(lab,"Au+Au HIJING b=0-12 fm, 100 kHz pileup");
  //sprintf(lab,"Au+Au HIJING b=0-12 fm");
  sprintf(lab,"100 pion events");
  
  
  TCanvas *ceff = new TCanvas("ceff","ceff",50,50,800,800);
  //ceff->SetLeftMargin(0.12);
  
  TH1F *hd = new TH1F("hd","hd",100, 0.0, hptmax);
  hd->SetMinimum(0.0);
  hd->SetMaximum(1.1);
  hd->GetXaxis()->SetTitle("p_{T} (GeV/c)");
  hd->GetXaxis()->SetTitleOffset(1.15);
  hd->GetYaxis()->SetTitle("Single track efficiency");
  hd->GetYaxis()->SetTitleOffset(1.3);
  hd->Draw();
  
  for(int i=0;i<NPLOTS;i++)
    {
      geff[i]->SetMarkerColor(col[i]);
      
      if(i==0)
	geff[i]->Draw("p");
      else
	geff[i]->Draw("same p");
    }
  
  TLegend *lpd = new TLegend(0.30, 0.45, 0.95, 0.65, "", "NDC");
  lpd->SetBorderSize(0);
  lpd->SetFillColor(0);
  lpd->SetFillStyle(0);
  for(int i=0;i<NPLOTS;i++)
    {
      lpd->AddEntry(geff[i],label[i], "p");
    }
  lpd->Draw();

  TLine *unit = new TLine(0,1.0,40,1.0);
  unit->SetLineStyle(3);
  unit->SetLineWidth(1);
  unit->Draw();

  //for(int i=0;i<2;i++)
  //lmax[i]->Draw();
  
  TCanvas *cdca = new TCanvas("cdca","cdca",50,50,800,800);
  //cdca->SetLeftMargin(0.15);
  
  
  TH1F *hdca = new TH1F("hdca","hdca",100, 0.0, hptmax);
  hdca->SetMinimum(0.0003);
  hdca->SetMaximum(0.02);
  hdca->GetXaxis()->SetTitle("p_{T} (GeV/c)");
  hdca->GetXaxis()->SetTitleOffset(1.25);
  hdca->GetYaxis()->SetTitle("DCA(r#phi) resolution (cm)");
  hdca->GetYaxis()->SetTitleOffset(1.4);
  hdca->Draw();
  
  gPad->SetLogy(1);
  
  for(int i=0;i<NPLOTS;i++)
    {
      gdca[i]->SetMarkerColor(col[i]);
      
      if(i==0)
	gdca[i]->Draw("p");
      else
	gdca[i]->Draw("same p");
    }
  
  TLegend *lpd1 = new TLegend(0.30, 0.7, 0.95, 0.92, "", "NDC");
  lpd1->SetBorderSize(0);
  lpd1->SetFillColor(0);
  lpd1->SetFillStyle(0);
  for(int i=0;i<NPLOTS;i++)
    {
      lpd1->AddEntry(gdca[i],label[i], "p");
    }

  lpd1->Draw();
  
  TCanvas *cdcaz = new TCanvas("cdcaz","cdcaz",50,50,800,800);
  //cdcaz->SetLeftMargin(0.15);
  
  TH1F *hdcaz = new TH1F("hdcaz","hdcaz",100, 0.0, hptmax);
  hdcaz->SetMinimum(0.0003);
  hdcaz->SetMaximum(0.02);
  hdcaz->GetXaxis()->SetTitle("p_{T} (GeV/c)");
  hdcaz->GetXaxis()->SetTitleOffset(1.25);
  hdcaz->GetYaxis()->SetTitle("DCA(Z) resolution (cm)");
  hdcaz->GetYaxis()->SetTitleOffset(1.4);
  hdcaz->Draw();
  
  gPad->SetLogy(1);
  
  for(int i=0;i<NPLOTS;i++)
    {
      gdcaZ[i]->SetMarkerColor(col[i]);
      
      if(i==0)
	gdcaZ[i]->Draw("p");
      else
	gdcaZ[i]->Draw("same p");
    }
  
  TLegend *lpd1z = new TLegend(0.30, 0.70, 0.95, 0.92, "", "NDC");
  lpd1z->SetBorderSize(0);
  lpd1z->SetFillColor(0);
  lpd1z->SetFillStyle(0);
  lpd1z->AddEntry(geff[0],label[0], "p");
  lpd1z->AddEntry(geff[1], label[1], "p");
  lpd1z->Draw();


  double hdptmax = 40.0;
  //double hdptmax = 39.0;
   
  TCanvas *crdpt = new TCanvas("crdpt","crdpt",50,50,800,800);
  //crdpt->SetLeftMargin(0.15);
  
  TH1F *hrdpt = new TH1F("hrdpt","hrdpt",100, 0.0, hdptmax);
  hrdpt->SetMinimum(0.0);
  hrdpt->SetMaximum(0.11);
  hrdpt->GetXaxis()->SetTitle("p_{T} (GeV/c)");
  hrdpt->GetXaxis()->SetTitleOffset(1.25);
  hrdpt->GetYaxis()->SetTitle("#Delta p_{T} / p_{T}");
  hrdpt->GetYaxis()->SetTitleOffset(1.5);
  hrdpt->Draw();
  
  for(int i=0;i<NPLOTS;i++)
    {
      grdpt[i]->SetMarkerColor(col[i]);

      if(i==0)
	grdpt[i]->Draw("p");
      else
	grdpt[i]->Draw("same p");
    }

  TLegend *lpd2 = new TLegend(0.15, 0.72, 0.85, 0.87, "", "NDC");
  lpd2->SetBorderSize(0);
  lpd2->SetFillColor(0);
  lpd2->SetFillStyle(0);
  lpd2->AddEntry(grdpt[0], label[0], "p");
  lpd2->AddEntry(grdpt[1], label[1], "p");
  lpd2->Draw();

  /*
  // draw the ratio of momentum resolution histograms

  TCanvas *cdptratio = new TCanvas("cdptratio","cdptratio",50,50,800,600);
  //cdptratio->SetLeftMargin(0.15);

  TH1F *hdptratio = new TH1F("hdptratio","hdptratio",100, 0.0, hptmax);
  hdptratio->SetMinimum(0.8);
  hdptratio->SetMaximum(1.4);
  hdptratio->GetXaxis()->SetTitle("p_{T} (GeV/c)");
  hdptratio->GetXaxis()->SetTitleOffset(1.15);
  hdptratio->GetYaxis()->SetTitle("#Delta p_{T} ratio");
  hdptratio->GetYaxis()->SetTitleOffset(1.8);
  hdptratio->Draw();

  int N = grdpt[0]->GetN();
  int N2 = grdpt[1]->GetN();
  cout << "grdpt[0] has " << N << " points, " << " grdpt[1] has " << N2 <<  " points" << endl;

  TGraph *gratio = new TGraph(N-1);
  gratio->SetMarkerStyle(20);
  gratio->SetMarkerSize(1);
  for(int i=0;i<N-1;i++)
    {
      double x1 = 0;
      double y1=0;
      grdpt[0]->GetPoint(i, x1, y1);
      double x2 = 0;
      double y2=0;
      grdpt[1]->GetPoint(i, x2, y2);
      double ratio = y2/y1;
      gratio->SetPoint(i,x1,ratio);
    }

  gratio->Draw("p");  
  */

  /*

  // now the upsilon spectra

  TFile *fin_ups[NPLOTS_UPS];

  fin_ups[0] = new TFile("look_quarkonia_histos_2mpas+4maps_out.root");  
  fin_ups[1] = new TFile("look_quarkonia_histos_2pixels+4maps_out.root");  

  TH1D *recomass[NPLOTS_UPS];

  for(int i=0;i<NPLOTS_UPS;i++)
    {
      fin_ups[i]->GetObject("recomass",recomass[i]);
    }

  TCanvas *cups = new TCanvas("cups","cups",50,50,1000,600);
  cups->SetLeftMargin(0.12);

  for(int i=0;i<NPLOTS_UPS;i++)
    {
      recomass[i]->SetMarkerColor(col[i]);
      recomass[i]->SetLineColor(col[i]);
      recomass[i]->SetMarkerStyle(20);
      recomass[i]->SetMarkerSize(1.2);

      if(i==0)
	recomass[i]->Draw("p");
      else
	recomass[i]->Draw("same p");
    }

  TLegend *lpq = new TLegend(0.15, 0.55, 0.40, 0.80,"Inner barrel","NDC");
  lpq->SetBorderSize(0);
  lpq->SetFillColor(0);
  lpq->SetFillStyle(0);
  lpq->AddEntry(recomass[0], "2 maps", "p");
  lpq->AddEntry(recomass[1], "2 pixels", "p");
  
  lpq->Draw();
  */

}
