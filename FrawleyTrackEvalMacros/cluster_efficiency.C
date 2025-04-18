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
#include "TLatex.h"

#include <iostream>

void cluster_efficiency()
{
  gROOT->SetStyle("Plain");
  gStyle->SetOptStat(0);
  gStyle->SetOptTitle(1);
 
  TCut good_g4cluster_cut = Form("layer >= 0 && gr > 0 && gembed > 0 && gtrackID > 0");
  TCut good_cluster_cut = Form("layer >= 0 && r > 0 && gembed > 0 && gtrackID > 0");
  TCut layer_cut = Form("layer > 6");
  TCut layer_cut_tpc[3];
  layer_cut_tpc[0] = Form("layer > 6 && layer <= 22");
  layer_cut_tpc[1] = Form("layer > 22 && layer <= 38");
  layer_cut_tpc[2] = Form("layer > 38 && layer <= 54");
  TCut layer_cut_intt = Form("layer > 2 && layer < 7");
  TCut layer_cut_mvtx = Form("layer < 3");
  TCut etacut = Form("geta > -1.0");
  //TCut etacut = Form("geta < 0.2 && geta > 0.1");
  //TCut etacut = Form("geta < 1.0 && geta > -1.0");
  //TCut ng4hitscut = Form("ng4hits > 0");
  TCut ng4hitscut = Form("");

  TChain* ntp_g4cluster = new TChain("ntp_g4cluster","truth clusters");
  TChain* ntp_cluster = new TChain("ntp_cluster","reco clusters");

  for(int ifile = 0; ifile < 100; ++ifile)
    {
      char name[500];
      
      //sprintf(name,"/sphenix/user/frawley/tpc_cluster_qa/macros/macros/g4simulations/repo_100_tracks_occ_eval_output/g4svtx_eval_%i.root_g4svtx_eval.root",ifile);     
      //sprintf(name,"/sphenix/user/frawley/tpc_cluster_qa/macros/macros/g4simulations/2k_tracks_occ_eval_output/g4svtx_eval_%i.root_g4svtx_eval.root",ifile);
      //sprintf(name,"/sphenix/user/frawley/tpc_cluster_qa/macros/macros/g4simulations/4k_tracks_occ_eval_output/g4svtx_eval_%i.root_g4svtx_eval.root",ifile);
      //sprintf(name,"/sphenix/user/frawley/tpc_cluster_qa/macros/macros/g4simulations/100muon_eval_output/g4svtx_eval_%i.root_g4svtx_eval.root",ifile);

      sprintf(name,"/sphenix/user/frawley/tpc_cluster_qa/macros/macros/g4simulations/100_tracks_occ_eval_output/g4svtx_eval_%i.root_g4svtx_eval.root",ifile);
      //sprintf(name,"/sphenix/user/frawley/current_repo/macros/macros/g4simulations/genfit_pion_eval_output/g4svtx_eval_%i.root_g4svtx_eval.root",ifile);
      //sprintf(name,"/sphenix/user/frawley/tpc_cluster_qa/macros/macros/g4simulations/eval_output/g4svtx_eval_%i.root_g4svtx_eval.root",ifile);     

      //sprintf(name,"/sphenix/user/frawley/cluster_efficiency/macros/macros/g4simulations/low_occupancy_fiducial0.5_search2_eval_output/g4svtx_eval_%i.root_g4svtx_eval.root",ifile);     

      std::cout << "Open file " << name << std::endl;

      ntp_g4cluster->Add(name);
      ntp_cluster->Add(name);
    }
      
  // cluster efficiency vs layer
  
  TCanvas * ceff = new TCanvas("ceff","ceff",5,5,1200,800);
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
  h3_eff->GetYaxis()->SetRangeUser(0.0, 1.04);

  TLine *unit = new TLine(0,1.0,54,1.0);
  unit->SetLineStyle(2);
  unit->Draw();

  TCanvas *crestpc = new TCanvas("crestpc","crestpc",5,5,1600,800);
  crestpc->Divide(3,1);
  TH1D *hreslayer[3];
  for(int i=0;i<3;++i)
    {
      crestpc->cd(i+1);
      gPad->SetLeftMargin(0.15);
      char name[500];
      sprintf(name,"hreslayer%i",i);
      if(i==0)
	hreslayer[i] = new TH1D(name,"cluster r-phi resolution TPC inner",300,-0.095, 0.095);  
      else if(i==1)
	hreslayer[i] = new TH1D(name,"cluster r-phi resolution TPC mid",300,-0.095, 0.095);  
      else
      hreslayer[i] = new TH1D(name,"Cluster r-phi resolution TPC outer",300,-0.095, 0.095);  
      char drawit[500];
      sprintf(drawit,"gr*(phi-gphi)>>hreslayer%i",i);
      ntp_cluster->Draw(drawit, good_cluster_cut && layer_cut_tpc[i]);
      hreslayer[i]->GetXaxis()->SetTitle("gr*(phi-gphi) (cm)");
      hreslayer[i]->GetXaxis()->SetNdivisions(504);
      hreslayer[i]->Draw("l");

      cout << " RMS for TPC r-phi for i = " << i << "  is " << hreslayer[i]->GetRMS() << endl;;

      char latstr[500];
      sprintf(latstr,"RMS = %.0f (#mum)",  1.0e04 * hreslayer[i]->GetRMS());
      TLatex *lat = new TLatex(0.6, 0.915, latstr);
      lat->SetNDC();
      lat->Draw();
    }

  TCanvas *crestpcz = new TCanvas("crestpcz","crestpcz",5,5,1600,800);
  crestpcz->Divide(3,1);
  TH1D *hreslayerz[3];
  for(int i=0;i<3;++i)
    {
      crestpcz->cd(i+1);
      gPad->SetLeftMargin(0.15);
      char name[500];
      sprintf(name,"hreslayerz%i",i);
      if(i==0)
	hreslayerz[i] = new TH1D(name,"Cluster z resolution TPC inner",300,-0.4, 0.4);  
      else if(i==1)
	hreslayerz[i] = new TH1D(name,"Cluster z resolution TPC mid",300,-0.4, 0.4);  
      else
	hreslayerz[i] = new TH1D(name,"Cluster z resolution TPC outer",300,-0.4, 0.4);  
      char drawit[500];
      sprintf(drawit,"z-gz>>hreslayerz%i",i);
      ntp_g4cluster->Draw(drawit, good_cluster_cut && layer_cut_tpc[i] && etacut && ng4hitscut);
      hreslayerz[i]->GetXaxis()->SetTitle("z-gz (cm)");
      hreslayerz[i]->GetXaxis()->SetNdivisions(504);
      hreslayerz[i]->Draw("l");

      char latstr[500];
      sprintf(latstr,"RMS = %.0f (#mum)",  1.0e04 * hreslayerz[i]->GetRMS());
      TLatex *lat = new TLatex(0.6, 0.915, latstr);
      lat->SetNDC();
      lat->Draw();
    }

  TCanvas *cressil = new TCanvas("cressil","cressil",5,5,1600,800);
  cressil->Divide(2,1);
  TH1D *hreslayer_sil[2];
  for(int i=0;i<2;++i)
    {
      cressil->cd(i+1);
      gPad->SetLeftMargin(0.15);
      char name[500];
      sprintf(name,"hreslayer_sil%i",i);
      if(i==0)
      hreslayer_sil[i] = new TH1D(name,"Cluster resolution INTT",300,-0.0049, 0.0049);  
      else
      hreslayer_sil[i] = new TH1D(name,"Cluster resolution MVTX",300,-0.0025, 0.0025);  
      char drawit[500];
      sprintf(drawit,"gr*(phi-gphi)>>hreslayer_sil%i",i);
      if(i==0)
	ntp_cluster->Draw(drawit, good_cluster_cut && layer_cut_intt);
      else
	ntp_cluster->Draw(drawit, good_cluster_cut && layer_cut_mvtx);
      hreslayer_sil[i]->GetXaxis()->SetTitle("gr*(phi-gphi) (cm)");
      hreslayer_sil[i]->GetXaxis()->SetNdivisions(504);
      hreslayer_sil[i]->Draw("l");

      char latstr[500];
      sprintf(latstr,"RMS = %.1f (#mum)",  1.0e04 * hreslayer_sil[i]->GetRMS());
      TLatex *lat = new TLatex(0.6, 0.915, latstr);
      lat->SetNDC();
      lat->Draw();
    }

  TCanvas *cressilz = new TCanvas("cressilz","cressilz",5,5,1600,800);
  cressilz->Divide(2,1);
  TH1D *hreslayer_silz[2];
  for(int i=0;i<2;++i)
    {
      cressilz->cd(i+1);
      gPad->SetLeftMargin(0.15);
      char name[500];
      sprintf(name,"hreslayer_silz%i",i);
      if(i==0)
	hreslayer_silz[i] = new TH1D(name,"Cluster resolution INTT z",300,-1.0, 1.0);  
      else
	hreslayer_silz[i] = new TH1D(name,"Cluster resolution MVTX z",300,-0.0025, 0.0025);  
      char drawit[500];
      sprintf(drawit,"z-gz>>hreslayer_silz%i",i);
      if(i==0)
	ntp_cluster->Draw(drawit, good_cluster_cut && layer_cut_intt);
      else
	ntp_cluster->Draw(drawit, good_cluster_cut && layer_cut_mvtx);
      hreslayer_silz[i]->GetXaxis()->SetTitle("z-gz (cm)");
      hreslayer_silz[i]->GetXaxis()->SetNdivisions(504);
      hreslayer_silz[i]->Draw("l");

      char latstr[500];
      if(i==0)
	sprintf(latstr,"RMS = %.0f (#mum)",  1.0e04 * hreslayer_silz[i]->GetRMS());
      else 
	sprintf(latstr,"RMS = %.1f (#mum)",  1.0e04 * hreslayer_silz[i]->GetRMS());
      TLatex *lat = new TLatex(0.6, 0.915, latstr);
      lat->SetNDC();
      lat->Draw();
    }

  TCanvas *ctruth = new TCanvas("ctruth","ctruth",5,5,1200,800); 
  ctruth->SetLeftMargin(0.15);
  TH1D *htruth = new TH1D("htruth","Truth clusters per layer",540,0,60);
  ntp_g4cluster->Draw("layer>>htruth", good_g4cluster_cut);  
  htruth->SetMarkerStyle(20);
  htruth->SetMarkerSize(2);
  htruth->GetXaxis()->SetTitle("layer");
  htruth->GetYaxis()->SetTitle("truth clusters / layer");
  htruth->GetYaxis()->SetTitleOffset(1.7);
  htruth->Draw("p");

  TCanvas *cphi = new TCanvas("cphi","cphi", 5,5,800,800);
  TH1D *hphi = new TH1D("hphi","hphi",10000, -4.0, 4.0);
  ntp_g4cluster->Draw("phi>>hphi", good_cluster_cut && layer_cut);
  hphi->Draw();

  TCanvas *ceta = new TCanvas("ceta","ceta", 5,5,800,800);
  TH1D *heta = new TH1D("heta","heta",10000, -1.2, 1.2);
  ntp_g4cluster->Draw("eta>>heta", good_cluster_cut && layer_cut);
  heta->Draw();

  TCanvas * cetaz = new TCanvas("cetaz","cetaz",5,5,800,800);
  TH2D *hetaz = new TH2D("hetaz","hetaz",200, -1.2, 1.2, 200, -0.5, 0.5);
  ntp_g4cluster->Draw("(z-gz):eta>>hetaz", good_cluster_cut && layer_cut);
  hetaz->Draw("colz");

  /*
  TCanvas * cetazsize = new TCanvas("cetazsize","cetazsize",5,5,800,800);
  TH2D *hetazsize = new TH2D("hetazsize","hetazsize",200, -1.2, 1.2, 200, 0, 5.0);
  ntp_g4cluster->Draw("zsize:eta>>hetazsize", good_cluster_cut && layer_cut);
  hetazsize->Draw("colz");
  */


  // zsize
  
  TCanvas *c2 = new TCanvas("c2","c2", 5,5,800,800); 
  
  //TH2D *h21 = new TH2D("h21","h21",800, -110, 110, 5000, 0, 8);
  TH2D *h21 = new TH2D("h21","h21",800, -1.2, 1.2, 5000, 0, 8);
  //ntp_g4cluster->Draw("zsize*.98:z>>h21",good_cluster_cut);
  ntp_g4cluster->Draw("zsize*.98:geta>>h21",good_cluster_cut);
  h21->SetMarkerStyle(20);
  h21->SetMarkerSize(0.5);
  //h21->GetXaxis()->SetTitle("Z (cm)");
  h21->GetXaxis()->SetTitle("#eta");
  h21->GetYaxis()->SetTitle("zsize (cm)");
  
  //TH2D *h22 = new TH2D("h22","h22",800, -110, 110, 5000, 0, 8);
  TH2D *h22 = new TH2D("h22","h22",800, -1.2, 1.2, 5000, 0, 8);
  //ntp_g4cluster->Draw("gzsize:gz>>h22",good_g4cluster_cut);
  ntp_g4cluster->Draw("gzsize:geta>>h22",good_g4cluster_cut);
  h22->SetMarkerStyle(20);
  h22->SetMarkerSize(0.5);
  h22->SetMarkerColor(kRed);
  //h21->GetXaxis()->SetTitle("Z (cm)");
  h21->GetXaxis()->SetTitle("#eta");
  h21->GetYaxis()->SetTitle("zsize (cm)");
  
  h21->Draw();
  h22->Draw("same");
  
  // phisize
  
  TCanvas *c4 = new TCanvas("c4","c4", 5,5,800,800); 
  
  TH2D *h41 = new TH2D("h41","h41",500, 0, 80.0, 5000, 0, 2.);
  ntp_g4cluster->Draw("phisize*0.98:r>>h41",good_cluster_cut);
  
  h41->SetMarkerStyle(20);
  h41->SetMarkerSize(0.5);
  h41->GetXaxis()->SetTitle("radius (cm)");
  h41->GetYaxis()->SetTitle("radius*phisize (cm)");
  
  TH2D *h42 = new TH2D("h42","h42",500, 0, 80.0, 5000, 0, 2.);
  ntp_g4cluster->Draw("gphisize:gr>>h42",good_g4cluster_cut);
  h42->SetMarkerStyle(20);
  h42->SetMarkerSize(0.5);
  h42->SetMarkerColor(kRed);
  h42->GetXaxis()->SetTitle("radius (cm)");
  h42->GetYaxis()->SetTitle("radius*phisize (cm)");
  
  
  h41->Draw();
  h42->Draw("same");

  
  //h42->Draw();
  //h41->Draw("same");

}
