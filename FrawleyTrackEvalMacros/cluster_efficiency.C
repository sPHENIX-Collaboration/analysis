#include "TChain.h"
#include "TString.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TCanvas.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TLine.h"
#include "TProfile.h"
#include "TCut.h"
#include "TLegend.h"

#include <iostream>

void cluster_efficiency()
{
  gROOT->SetStyle("Plain");
  gStyle->SetOptStat(0);
  gStyle->SetOptTitle(1);
 

  TCut good_g4cluster_cut = Form("layer > 6 && gr > 0 && gembed > 0 && gtrackID >= 0");
  TCut good_cluster_cut = Form("layer > 6 && r > 0 && gembed > 0 && gtrackID >= 0");
  TCut sector_side_same_cut = Form("sector_side_same == 1");
  TCut side_change_cut = Form("side_change == 1");
  TCut layer_cut = Form("layer > 6");

  TChain* ntp_g4cluster = new TChain("ntp_g4cluster","truth clusters");

  for(int ifile = 0; ifile < 300; ++ifile)
    {
      char name[500];
      sprintf(name,"/sphenix/user/frawley/cluster_efficiency/macros/macros/g4simulations/eval_output/g4svtx_eval_%i.root_g4svtx_eval.root",ifile);
      //sprintf(name,"/sphenix/user/frawley/cluster_efficiency/macros/macros/g4simulations/high_occupancy_fiducial0.5_search2_eval_output/g4svtx_eval_%i.root_g4svtx_eval.root",ifile);
      //sprintf(name,"/sphenix/user/frawley/cluster_efficiency/macros/macros/g4simulations/low_occupancy_fiducial0.5_search2_eval_output/g4svtx_eval_%i.root_g4svtx_eval.root",ifile);

      std::cout << "Open file " << name << std::endl;

      ntp_g4cluster->Add(name);
    }
      
  // cluster efficiency vs layer
  
  TCanvas * c3 = new TCanvas("c3","c3",5,5,1200,800);
  TH1D* h3_den = new TH1D("h3_den","; layer; Efficiency",54, 0, 54);
  TH1D* h3_num = (TH1D*)h3_den->Clone("h3_num");;
  TH1D* h3_eff = (TH1D*)h3_den->Clone("h3_eff");;
  
  ntp_g4cluster->Draw("layer>>h3_den",good_g4cluster_cut);
  ntp_g4cluster->Draw("layer>>h3_num",good_cluster_cut);
  
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
  h3_eff->SetTitle("truth clusters with matched reco cluster; layer; cluster efficiency");
  h3_eff->SetMarkerStyle(20);
  h3_eff->SetMarkerColor(kBlack);
  h3_eff->SetLineColor(kBlack);
  //h3_eff->GetYaxis()->SetRangeUser(0.0, 1.1);
  h3_eff->GetYaxis()->SetRangeUser(0.5, 1.04);

  TLine *unit = new TLine(0,1.0,54,1.0);
  unit->SetLineStyle(2);
  unit->Draw();
  
  TCanvas *creco = new TCanvas("creco","creco", 5,5,800,800);
  TH1D* hnreco = new TH1D("hnreco","number of reco clusters per truth cluster",100, -0.5, 6.5);  
  ntp_g4cluster->Draw("nreco>>hnreco",good_g4cluster_cut);  
  double norm = hnreco->Integral();
  double ntrue = h3_den->GetEntries();
  std::cout << " norm = " << norm << " ntrue = " << ntrue << endl;
  hnreco->Scale(1/norm);
  hnreco->SetMarkerStyle(20);
  hnreco->SetMarkerSize(2);
  hnreco->GetYaxis()->SetRangeUser(1e-7, 2.0);
  hnreco->GetXaxis()->SetTitle("reco clusters / truth cluster");;

  gPad->SetLogy(1);
  gPad->SetGridy(1);

  hnreco->DrawCopy();  

  cout << " Mean nreco = " << hnreco->GetMean() << endl;

  TCanvas *crl = new TCanvas("crl","crl",5,5,1200,800);
  TH2D *hrecolayer = new TH2D("hrecolayer","Reco clusters per truth cluster",54,0,54,300,0,8);  
  ntp_g4cluster->Draw("nreco:layer>>hrecolayer",good_g4cluster_cut );
  TProfile *hrl = hrecolayer->ProfileX();
  hrl->GetYaxis()->SetTitle("reco clusters per truth cluster");
  hrl->GetXaxis()->SetTitle("Layer");

  hrl->SetMarkerStyle(20);
  hrl->SetMarkerSize(1);

  hrl->Draw("p");

  unit->Draw();

  TCanvas *css = new TCanvas("css","Cluster duplication type",5,5,800,800);
  gPad->SetGridy(1);
  TH1D* hreco_ss = new TH1D("hreco_ss","Failed reco clusters type",100, -0.5, 6.5);  
  TH1D* hreco_sc = new TH1D("hreco_sc","Failed reco clusters type",100, -0.5, 6.5);  
  //ntp_g4cluster->Draw("nreco>>hreco_sc",good_g4cluster_cut && side_change_cut);  // does not require reco
  //ntp_g4cluster->Draw("nreco>>hreco_ss", good_g4cluster_cut && sector_side_same_cut);  
  ntp_g4cluster->Draw("nreco>>hreco_sc",good_cluster_cut && side_change_cut);  // requires reco
  ntp_g4cluster->Draw("nreco>>hreco_ss", good_cluster_cut && sector_side_same_cut);  

  hreco_ss->Scale(1/norm);
  hreco_ss->SetMarkerStyle(28);
  hreco_ss->SetMarkerSize(2);
  hreco_ss->SetMarkerColor(kBlue);

  hreco_sc->Scale(1/norm);
  hreco_sc->SetMarkerStyle(24);
  hreco_sc->SetMarkerSize(2);
  hreco_sc->SetMarkerColor(2);

  gPad->SetLogy(1);

  hnreco->SetTitle("Failed clusters by type");
  hnreco->Draw();
  hreco_ss->Draw("same p");  
  hreco_sc->Draw("same p");  

  TLegend *leg = new TLegend(0.55, 0.65, 0.85, 0.88, "", "NDC");
  leg->SetBorderSize(1);
  leg->SetFillColor(kWhite);
  leg->SetFillStyle(1001);
  leg->AddEntry(hnreco,"All clusters","p");
  leg->AddEntry(hreco_ss,"No side/sector change","p");
  leg->AddEntry(hreco_sc,"Side change","p");
  leg->Draw();

  TCanvas *ctruth = new TCanvas("ctruth","ctruth",5,5,1200,800); 
  ctruth->SetLeftMargin(0.15);
  TH1D *htruth = new TH1D("htruth","Truth clusters per layer 30,000 tracks",540,0,60);
  ntp_g4cluster->Draw("layer>>htruth", good_g4cluster_cut);  
  htruth->SetMarkerStyle(20);
  htruth->SetMarkerSize(2);
  htruth->GetXaxis()->SetTitle("layer");
  htruth->GetYaxis()->SetTitle("truth clusters / 30000 tracks");
  htruth->GetYaxis()->SetTitleOffset(1.7);
  htruth->Draw("p");

  TCanvas *cphi = new TCanvas("cphi","cphi", 5,5,800,800);
  TH1D *hphi = new TH1D("hphi","hphi",10000, -5.0, 5.0);
  ntp_g4cluster->Draw("r*phi>>hphi", good_cluster_cut && layer_cut);
  hphi->Draw();


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
