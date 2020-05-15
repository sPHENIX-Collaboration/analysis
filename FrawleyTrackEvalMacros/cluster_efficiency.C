#include "TChain.h"
#include "TString.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TCanvas.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TLine.h"

#include <iostream>

void cluster_efficiency()
{
  gROOT->SetStyle("Plain");
  gStyle->SetOptStat(0);
  gStyle->SetOptTitle(1);
 

  TString good_g4cluster_cut = Form("layer > 6 && gr > 0 && gembed > 0 && gtrackID >= 0");
  TString good_cluster_cut = Form("layer > 6 && r > 0 && gembed > 0 && gtrackID >= 0");

 /*  
  TString good_g4cluster_cut = Form("layer > 6 && gr > 0");
  TString good_cluster_cut = Form("layer > 6 && r > 0");
  */

  TChain* ntp_g4cluster = new TChain("ntp_g4cluster","truth clusters");

  for(int ifile = 0; ifile < 30; ++ifile)
    {
      char name[500];
      sprintf(name,"/sphenix/user/frawley/cluster_efficiency/macros/macros/g4simulations/fiducial.5_search2_eval_output/g4svtx_eval_%i.root_g4svtx_eval.root",ifile);
      //sprintf(name,"/sphenix/user/frawley/cluster_efficiency/macros/macros/g4simulations/fiducial.0_search2_eval_output/g4svtx_eval_%i.root_g4svtx_eval.root",ifile);
      std::cout << "Open file " << name << std::endl;

      ntp_g4cluster->Add(name);
    }
      
  // cluster efficiency vs layer
  
  TCanvas * c3 = new TCanvas("c3","c3",5,5,1200,800);
  TH1D* h3_den = new TH1D("h3_den","; layer; Efficiency",54, 0, 54);
  TH1D* h3_num = (TH1D*)h3_den->Clone("h3_num");;
  TH1D* h3_eff = (TH1D*)h3_den->Clone("h3_eff");;
  
  //std::cout<<__LINE__<<": "<< good_cluster_cut <<std::endl;
  
  ntp_g4cluster->Draw("layer>>h3_den",good_g4cluster_cut.Data());
  ntp_g4cluster->Draw("layer>>h3_num",good_cluster_cut.Data());
  
  for(int i=1;i<=h3_den->GetNbinsX();++i){
    double pass = h3_num->GetBinContent(i);
    double all = h3_den->GetBinContent(i);
    double eff = 0;
    if(all > pass)
      eff = pass/all;
    else if(all > 0)
      eff = 1.;
    //std::cout << " i " << i << " pass " << pass << " all " << all << " eff " << eff << std::endl;	
    //double err = BinomialError(pass, all); 
    h3_eff->SetBinContent(i, eff);
    //h3_eff->SetBinError(i, err);
  }
  
  //h3_eff->Draw("e,text");
  h3_eff->Draw("p");
  h3_eff->SetStats(0);
  h3_eff->SetTitle("truth clusters with reco cluster; layer; cluster efficiency");
  h3_eff->SetMarkerStyle(20);
  h3_eff->SetMarkerColor(kBlack);
  h3_eff->SetLineColor(kBlack);
  //h3_eff->GetYaxis()->SetRangeUser(0.0, 1.1);
  h3_eff->GetYaxis()->SetRangeUser(0.5, 1.04);

  TLine *unit = new TLine(0,1.0,54,1.0);
  unit->SetLineStyle(2);
  unit->Draw();
  
  TCanvas *creco = new TCanvas("creco","creco", 5,5,800,800);
  TH1D* hreco = new TH1D("hreco","reco clusters per truth cluster",8, 0, 8);  
  ntp_g4cluster->Draw("nreco>>hreco",good_g4cluster_cut.Data());  
  double norm = hreco->Integral();
  std::cout << " norm = " << norm << endl;
  hreco->Scale(1/norm);
  hreco->SetMarkerStyle(20);
  hreco->SetMarkerSize(2);
  gPad->SetLogy(1);

  hreco->Draw();  

  cout << " Mean nreco = " << hreco->GetMean() << endl;

  /*  
  // zsize
  
  TCanvas *c2 = new TCanvas("c2","c2", 5,5,800,800); 
  
  TH2D *h21 = new TH2D("h21","h21",800, -110, 110, 5000, 0, 8);
  ntp_g4cluster->Draw("zsize*.98:z>>h21",good_cluster_cut.Data());
  h21->SetMarkerStyle(20);
  h21->SetMarkerSize(0.5);
  h21->GetXaxis()->SetTitle("Z (cm)");
  h21->GetYaxis()->SetTitle("zsize (cm)");
  
  TH2D *h22 = new TH2D("h22","h22",800, -110, 110, 5000, 0, 8);
  ntp_g4cluster->Draw("g4zsize:gz>>h22",good_g4cluster_cut.Data());
  h22->SetMarkerStyle(20);
  h22->SetMarkerSize(0.5);
  h22->SetMarkerColor(kRed);
  h21->GetXaxis()->SetTitle("Z (cm)");
  h21->GetYaxis()->SetTitle("zsize (cm)");
  
  h21->Draw();
  h22->Draw("same");
  
  // phisize
  
  TCanvas *c4 = new TCanvas("c4","c4", 5,5,800,800); 
  
  TH2D *h41 = new TH2D("h41","h41",500, 0, 80.0, 5000, 0, 2.);
  ntp_g4cluster->Draw("phisize*0.98:r>>h41",good_cluster_cut.Data());
  
  h41->SetMarkerStyle(20);
  h41->SetMarkerSize(0.5);
  h41->GetXaxis()->SetTitle("radius (cm)");
  h41->GetYaxis()->SetTitle("radius*phisize (cm)");
  
  TH2D *h42 = new TH2D("h42","h42",500, 0, 80.0, 5000, 0, 2.);
  ntp_g4cluster->Draw("g4phisize:gr>>h42",good_g4cluster_cut.Data());
  h42->SetMarkerStyle(20);
  h42->SetMarkerSize(0.5);
  h42->SetMarkerColor(kRed);
  h42->GetXaxis()->SetTitle("radius (cm)");
  h42->GetYaxis()->SetTitle("radius*phisize (cm)");
  
  
  h41->Draw();
  h42->Draw("same");
  */
  
  //h42->Draw();
  //h41->Draw("same");

}
