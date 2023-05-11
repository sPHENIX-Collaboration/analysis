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

void plot_comparisons_clusters()
{

  SetsPhenixStyle();

  /*
  gROOT->SetStyle("Plain");
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);
  gStyle->SetOptTitle(0);
  */

  static const int NCONFIGS = 2;

  TFile *fin[NCONFIGS];
  int tpc_layer[NCONFIGS][4];

  char label[NCONFIGS][500]; 

  fin[0] = new TFile("root_files/pileup_fix_inner16_200khz_cluster_resolution_out.root");  
  fin[1] = new TFile("root_files/pileup_fix_inner16_100khz_cluster_resolution_out.root");  

  sprintf(label[0], "NeCF4_400 16 inner layers 200 kHz");
  sprintf(label[1], "NeCF4_400 16 inner layers 100 kHz");

  /*
  // 8 inner layers
  tpc_layer[0][0] = 7;  // 1st layer in TPC - inner radius 30 cm, center = 30.625
  tpc_layer[0][1] = 15; // 1st layer of mid TPC - inner radius 40 cm, center = 40.625
  tpc_layer[0][2] = 31; // 1st layer of outer TPC - inner radius 60 cm, center = 60.625
  tpc_layer[0][3] = 46; // last layer of outer TPC - inner radius 78.75 cm, center = 79.375
  */

  // 16 inner layers
  tpc_layer[0][0] = 7; // 1st layer in TPC - inner radius 30 cm, center = 30.3125
  tpc_layer[0][1] = 23; // 1st layer of mid TPC - inner radius 40 cm, center = 40.625
  tpc_layer[0][2] = 39;  // 1st layer of outer TPC - inner radius 60 cm,  center = 60.625
  tpc_layer[0][3] = 54; // last layer of outer TPC - inner radius 78.75 cm, center 79.375

  // 16 inner layers
  tpc_layer[1][0] = 7; // 1st layer in TPC - inner radius 30 cm, center = 30.3125
  tpc_layer[1][1] = 23; // 1st layer of mid TPC - inner radius 40 cm, center = 40.625
  tpc_layer[1][2] = 39;  // 1st layer of outer TPC - inner radius 60 cm,  center = 60.625
  tpc_layer[1][3] = 54; // last layer of outer TPC - inner radius 78.75 cm, center 79.375

  double layer_radius[4] = {30.0, 40.0, 60.0, 79.0};

  bool use_last_inner = true;
  if(use_last_inner)
    {
      tpc_layer[0][1] = 22; // last layer of inner TPC - inner radius 38.75 cm, center = 39.375
      tpc_layer[1][1] = 22; // last layer of inner TPC - inner radius 39.375,  center = 39.6875
      layer_radius[1] = 39.0;
    }

  TGraph *gzsize[4][NCONFIGS];
  TGraph *gocc[4][NCONFIGS];

  int col[4] = {kRed, kBlue, kBlack, kViolet};

  for(int i=0;i<NCONFIGS;i++)
    {
      if(!fin[i])
	{
	  cout << "Did not find file " << i << "  quit!" << endl;
	  exit(1);
	}

      for(int ilayer = 0;ilayer < 4; ilayer++)
	{
	  int layer = tpc_layer[i][ilayer];
	  char name1[500];
	  sprintf(name1,"goccz%i",layer);
	  cout << "Fetching object " << name1 << " for layer " << layer << " from file " << i  << endl;
	  fin[i]->GetObject(name1,gzsize[ilayer][i]);
	  char name2[500];
	  sprintf(name2,"gocc_clus%i",layer);
	  cout << "Fetching object " << name2 << " for layer " << layer << " from file " << i  << endl;
	  fin[i]->GetObject(name2,gocc[ilayer][i]);
	  
	  if(!gzsize[ilayer][i])
	    {
	      cout << "Failed to find gzsize named  " << name1  << endl;
	      exit(1);
	    }
	  if(!gocc[ilayer][i])
	    {
	      cout << "Failed to find gocc named " << name2  << endl;
	      exit(1);
	    }
	}
    }

  // Plot Z size
  //=========

  double hmaxz[4] = {3.5, 3.5, 3.5, 3.5};
  
  TCanvas *cz = new TCanvas("cz","cz",50,50,1200,800);
  cz->Divide(2,2);
  
  TH1F *hdz = new TH1F("hdz","hdz",100, -110.0, 110.0);
  hdz->SetMinimum(0.0);
  hdz->GetXaxis()->SetTitle("Z (cm)");
  hdz->GetXaxis()->SetTitleOffset(1.15);
  hdz->GetYaxis()->SetTitle("Cluster Z size");
  hdz->GetYaxis()->SetTitleOffset(1.3);

  for(int ilayer = 0;ilayer<4;ilayer++)
    {  
      cz->cd(ilayer+1);

      hdz->SetMaximum(hmaxz[ilayer]);
      hdz->DrawCopy();
      for(int i=0;i<NCONFIGS;i++)
	{	  
	  gzsize[ilayer][i]->SetMarkerColor(col[i]);
	  
	  if(i==0)
	    gzsize[ilayer][i]->Draw("p");
	  else
	    gzsize[ilayer][i]->Draw("same p");
	}

      char label[500];
      sprintf(label,"radius = %.1f cm",layer_radius[ilayer]);
      TLatex *l3 = new TLatex(0.25,0.25,label);
      l3->SetNDC(1);
      l3->Draw();
    }

  // Plot occupancy
  //=============
  //double hmax[4] = {0.3, 0.25, 0.15, 0.10};
  double hmax[4] = {0.45, 0.35, 0.25, 0.18};
  
  TCanvas *cocc = new TCanvas("cocc","cocc",50,50,1200,800);
  cocc->Divide(2,2);
  
  TH1F *hd = new TH1F("hd","hd",100, -110.0, 110.0);
  hd->SetMinimum(0.0);
  hd->GetXaxis()->SetTitle("Z (cm)");
  hd->GetXaxis()->SetTitleOffset(1.15);
  hd->GetYaxis()->SetTitle("Cluster occupancy");
  hd->GetYaxis()->SetTitleOffset(1.3);

  for(int ilayer = 0;ilayer<4;ilayer++)
    {  
      cocc->cd(ilayer+1);

      hd->SetMaximum(hmax[ilayer]);
      hd->DrawCopy();
      for(int i=0;i<NCONFIGS;i++)
	{	  
	  gocc[ilayer][i]->SetMarkerColor(col[i]);
	  
	  if(i==0)
	    gocc[ilayer][i]->Draw("p");
	  else
	    gocc[ilayer][i]->Draw("same p");
	}
      char label[500];
      sprintf(label,"radius = %.1f cm",layer_radius[ilayer]);
      TLatex *l3 = new TLatex(0.25,0.25,label);
      l3->SetNDC(1);
      l3->Draw();
    }
  
  TLegend *lpd = new TLegend(0.20, 0.70, 0.9, 0.90, "", "NDC");
  lpd->SetBorderSize(1);
  lpd->SetFillColor(0);
  lpd->SetFillStyle(0);
  for(int i=0;i<NCONFIGS;i++)
    {
      lpd->AddEntry(gocc[0][i],label[i], "p");
    }
  cocc->cd(4);
  lpd->Draw();

  cz->cd(4);
  lpd->Draw();
  
  cout << "Done" << endl;
}


