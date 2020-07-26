#include "TH2D.h"
#include "TH1D.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TLegend.h"
#include "TF1.h"

#include <iostream>

void plot_ntp_track_out()
{
  gROOT->SetStyle("Plain");
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(1);
  gStyle->SetOptTitle(0);

  double ptmax = 40.0;
  double slice_low = 20.0;
  double slice_high = 21.0;

  std::vector<std::string> finvec;
  std::vector<std::string> legvec;
  std::vector<int> col;

  finvec.push_back("root_files/acts_require2maps_ntp_track_out.root");
  legvec.push_back( "acts MVTX match");
  col.push_back(kBlack);

  finvec.push_back("root_files/acts_require0maps_ntp_track_out.root");
  legvec.push_back("acts MVTX not matched");
  col.push_back(kBlue);

  finvec.push_back("root_files/acts_genfit_compare_track_out.root");
  legvec.push_back("genfit all");
  col.push_back(kMagenta);

  /*
  finvec.push_back("root_files/acts_smoothed_require2maps_ntp_track_out.root");
  legvec.push_back("acts MVTX match smoothed");
  col.push_back(kRed);
  */

  unsigned int nfiles = finvec.size();

  TCanvas *ctemp0 = new TCanvas("ctemp0","ctemp0",5,5,1600,800);
  ctemp0->Divide(nfiles,1);
  TCanvas *cslice = new TCanvas("cslice","cslice",5,5,1600,800);
  cslice->Divide(nfiles,1);
  TCanvas *cpt = new TCanvas("cpt","cpt",5,5,1500,800); 
  cpt->Divide(2,1);
  cpt->cd(1); gPad->SetLeftMargin(0.2);
  cpt->cd(2); gPad->SetLeftMargin(0.2);
  TCanvas *ceff = new TCanvas("ceff","ceff",5,5,1200,800);
  ceff->SetLeftMargin(0.18);
  TCanvas *ctemp1 = new TCanvas("ctemp1","ctemp1",5,5,1200,800);
  ctemp1->Divide(nfiles,1);
  TCanvas *cslicexy = new TCanvas("cslicexy","cslicexy",5,5,1600,800);
  cslicexy->Divide(nfiles,1);
  TCanvas *cdcaxy = new TCanvas("cdcaxy","cdcaxy",5,5,1200,800); 
  cdcaxy->SetLeftMargin(0.2);
  gPad->SetGrid();
  TCanvas *ctemp2 = new TCanvas("ctemp2","ctemp2",5,5,1200,800);
  ctemp2->Divide(nfiles,1);
  TCanvas *csliceZ = new TCanvas("csliceZ","csliceZ",5,5,1600,800);
  csliceZ->Divide(nfiles,1);
  TCanvas *cdcaZ = new TCanvas("cdcaZ","cdcaZ",5,5,1200,800); 
  cdcaZ->SetLeftMargin(0.2);
  gPad->SetGrid();


  TLegend *lpd = new TLegend(0.5, 0.6, 0.8, 0.75, "", "NDC");
  lpd->SetBorderSize(1);
  lpd->SetFillColor(kWhite);
  lpd->SetFillStyle(1001);

  TF1 *fz = new TF1("fz","gaus");
  fz->SetRange(-0.02, 0.02);

  /*
  TFile *fin[nfiles];
  std::string filename[nfiles]=
    {
      "root_files/acts_require2maps_ntp_track_out.root"
      , "root_files/ntp_track_out.root"
      //      , "root_files/acts_require0maps_ntp_track_out.root"     
      , "root_files/acts_genfit_compare_track_out.root"
    };

  std::string lname[3] =
    {
      "acts MVTX match"
      //      , "acts no MVTX match"
      , "acts MVTX match smoothed"
      , " genfit all"
    };
  */


  for(unsigned int i=0; i<finvec.size();++i)
    {
      TFile *fin = new TFile(finvec[i].c_str());

      // pT resolution

      TH2D *hpt2d;
      fin->GetObject("h1",hpt2d); 
      std::cout << hpt2d->GetEntries() << std::endl;
      hpt2d->FitSlicesY();
      TH1D *hptres =  (TH1D*)gDirectory->Get("h1_2");
      TH1D *hptcent = (TH1D*)gDirectory->Get("h1_1");
      ctemp0->cd(i+1);
      hpt2d->Draw("colz");

      int binlow = hpt2d->GetXaxis()->FindBin(slice_low);
      int binhigh = hpt2d->GetXaxis()->FindBin(slice_high);
      char hname[500];
      sprintf(hname,"hptslice%i",i);
      TH1D *hptslice = hpt2d->ProjectionY(hname,binlow, binhigh);
      cslice->cd(i+1);
      hptslice->DrawCopy();
      hptslice->Fit("gaus");

      cpt->cd(1);
      hptres->GetYaxis()->SetTitleOffset(2.1);
      hptres->GetXaxis()->SetTitleOffset(1.2);
      hptres->SetMarkerStyle(20);
      hptres->SetMarkerSize(1.2);
      hptres->SetMarkerColor(col[i]);
      hptres->GetXaxis()->SetRangeUser(0, ptmax);
      hptres->GetYaxis()->SetRangeUser(0.0, 0.18);
      hptres->SetTitle(";p_{T} [GeV/c];#frac{#Delta p_{T}}{p_{T}} (resolution)");
      if(i == 0)
	hptres->DrawCopy("p");
      else
	hptres->DrawCopy("p same");

      lpd->AddEntry(hptres, legvec[i].c_str());

      cpt->cd(2);
      hptcent->GetYaxis()->SetTitleOffset(2.1);
      hptcent->GetXaxis()->SetTitleOffset(1.2);
      hptcent->SetMarkerStyle(20);
      hptcent->SetMarkerSize(1.2);
      hptcent->SetMarkerColor(col[i]);
      hptcent->GetXaxis()->SetRangeUser(0, ptmax);
      hptcent->GetYaxis()->SetRangeUser(-0.1, 0.1);
      hptcent->SetTitle(";p_{T} [GeV/c];#frac{#Delta p_{T}}{p_{T}} (offset)");
      if(i==0)
	hptcent->DrawCopy("p");
      else
	hptcent->DrawCopy("p same");

      // Efficiency
      ceff->cd();
      TH1D *hnum = 0;
      TH1D *hden = 0;
      fin->GetObject("h3_num",hnum);
      fin->GetObject("h3_den",hden);

      TH1D* heff = (TH1D*)hden->Clone("heff");;

      for(int i=1;i<=hden->GetNbinsX();++i)
	{
	  double pass = hnum->GetBinContent(i);
	  double all = hden->GetBinContent(i);
	  double eff = 0;
	  if(all > pass)
	    eff = pass/all;
	  else if(all > 0)
	    eff = 1.;
	  heff->SetBinContent(i, eff);
	}  

      heff->GetYaxis()->SetTitle("Efficiency");
      heff->GetYaxis()->SetTitleOffset(1.5);
      heff->GetXaxis()->SetTitle("p_{T} (GeV/c)");
      heff->GetXaxis()->SetTitleOffset(1.2);
      heff->SetMarkerStyle(20);
      heff->SetMarkerSize(1.2);
      heff->SetMarkerColor(col[i]);
      heff->GetXaxis()->SetRangeUser(0.0, ptmax);
      heff->GetYaxis()->SetRangeUser(0.0, 1.0);
      if(i==0)
	heff->DrawCopy("p");
      else
	heff->DrawCopy("p same");

      // dca xy resolution
      cdcaxy->cd();
      TH2D *hdca2d;
      fin->GetObject("h2",hdca2d);
      ctemp1->cd(i+1);
      hdca2d->Draw("colz");

      cslicexy->cd(i+1);
      binlow = hdca2d->GetXaxis()->FindBin(slice_low);
      binhigh = hdca2d->GetXaxis()->FindBin(slice_high);
      TH1D* hdcaxyslice = hdca2d->ProjectionY("hdcaxyslice",binlow, binhigh);
      hdcaxyslice->SetLineColor(col[i]);      
      cslicexy->cd(i+1);
      hdcaxyslice->DrawCopy();
      hdcaxyslice->Fit("gaus");

      cdcaxy->cd();  
      hdca2d->FitSlicesY();
      TH1D*hdcares = (TH1D*)gDirectory->Get("h2_2");
      hdcares->GetYaxis()->SetTitleOffset(2.1);
      hdcares->GetYaxis()->SetTitle("DCA(r#phi) (cm)");
      hdcares->GetXaxis()->SetTitleOffset(1.2);
      hdcares->GetXaxis()->SetTitle("p_{T} (GeV/c)");
      hdcares->SetMarkerStyle(20);
      hdcares->SetMarkerSize(1.2);
      hdcares->SetMarkerColor(col[i]);
      hdcares->GetYaxis()->SetRangeUser(0, 0.03);
      if(i==0) 
	hdcares->DrawCopy("p");
      else
	hdcares->DrawCopy("p same");

      // dca z resolution
      TH2D *hdcaZ2d;
      fin->GetObject("h3",hdcaZ2d);
      ctemp2->cd(i+1);
      hdcaZ2d->Draw("colz");

      binlow = hdcaZ2d->GetXaxis()->FindBin(slice_low);
      binhigh = hdcaZ2d->GetXaxis()->FindBin(slice_high);
      TH1D* hdcazslice = hdcaZ2d->ProjectionY("hdcazslice",binlow, binhigh);
      hdcazslice->SetLineColor(col[i]);      
      csliceZ->cd(i+1);
      hdcazslice->Draw();
      hdcazslice->Fit("gaus");

      cdcaZ->cd();
      hdcaZ2d->FitSlicesY(fz);
      TH1D* hdcaZres = (TH1D*)gDirectory->Get("h3_2");

      hdcaZres->GetYaxis()->SetTitleOffset(2.1);
      hdcaZres->GetYaxis()->SetTitle("DCA(Z) (cm)");
      hdcaZres->GetXaxis()->SetTitleOffset(1.2);
      hdcaZres->GetXaxis()->SetTitle("p_{T} (GeV/c");
      hdcaZres->SetMarkerStyle(20);
      hdcaZres->SetMarkerSize(1.2);
      hdcaZres->SetMarkerColor(col[i]);
      hdcaZres->GetYaxis()->SetRangeUser(0, 0.02);
      if(i==0)      
	hdcaZres->DrawCopy("p");
      else
	hdcaZres->DrawCopy("p same");
    }

  cpt->cd(2);  lpd->Draw();

  cdcaxy->cd();  lpd->Draw();

  cdcaZ->cd();  lpd->Draw();

  ceff->cd();  lpd->Draw();
}

