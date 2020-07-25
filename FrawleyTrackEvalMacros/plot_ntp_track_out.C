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
  double slice_low = 5.0;
  double slice_high = 6.0;

  static const int nfiles = 3;

  int col[nfiles] = {kBlack, kRed, kBlue};

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

  TFile *fin[nfiles];
  std::string filename[nfiles]=
    {
      "root_files/acts_require2maps_ntp_track_out.root"
      , "root_files/acts_require0maps_ntp_track_out.root"     
      , "root_files/acts_genfit_compare_track_out.root"
    };

  std::string lname[3] =
    {
      "acts MVTX match"
      , "acts no MVTX match"
      , " genfit all"
    };

  TH2D *hpt2d[nfiles];
  TH2D *hdca2d[nfiles];
  TH2D *hdcaZ2d[nfiles];
  TH1D * hptres[nfiles];
  TH1D * hptcent[nfiles];

  for(int i=0; i<nfiles;++i)
    {
      fin[i] = new TFile(filename[i].c_str());

      // pT resolution
      fin[i]->GetObject("h1",hpt2d[i]); 
      std::cout << hpt2d[i]->GetEntries() << std::endl;
      hpt2d[i]->FitSlicesY();
      hptres[i] =  (TH1D*)gDirectory->Get("h1_2");
      hptcent[i] = (TH1D*)gDirectory->Get("h1_1");
      ctemp0->cd(i+1);
      hpt2d[i]->Draw("colz");

      int binlow = hpt2d[i]->GetXaxis()->FindBin(slice_low);
      int binhigh = hpt2d[i]->GetXaxis()->FindBin(slice_high);
      char hname[500];
      sprintf(hname,"hptslice%i",i);
      TH1D *hptslice = hpt2d[i]->ProjectionY(hname,binlow, binhigh);
      cslice->cd(i+1);
      hptslice->DrawCopy();
      hptslice->Fit("gaus");

      cpt->cd(1);
      hptres[i]->GetYaxis()->SetTitleOffset(2.1);
      hptres[i]->GetXaxis()->SetTitleOffset(1.2);
      hptres[i]->SetMarkerStyle(20);
      hptres[i]->SetMarkerSize(1.2);
      hptres[i]->SetMarkerColor(col[i]);
      hptres[i]->GetXaxis()->SetRangeUser(0, ptmax);
      hptres[i]->GetYaxis()->SetRangeUser(0.0, 0.18);
      hptres[i]->SetTitle(";p_{T} [GeV/c];#frac{#Delta p_{T}}{p_{T}} (resolution)");
      if(i == 0)
	hptres[i]->DrawCopy("p");
      else
	hptres[i]->DrawCopy("p same");

      lpd->AddEntry(hptres[i],lname[i].c_str());

      cpt->cd(2);
      //gPad->SetLeftMargin(0.2);
      hptcent[i]->GetYaxis()->SetTitleOffset(2.1);
      hptcent[i]->GetXaxis()->SetTitleOffset(1.2);
      hptcent[i]->SetMarkerStyle(20);
      hptcent[i]->SetMarkerSize(1.2);
      hptcent[i]->SetMarkerColor(col[i]);
      hptcent[i]->GetXaxis()->SetRangeUser(0, ptmax);
      hptcent[i]->GetYaxis()->SetRangeUser(-0.1, 0.1);
      hptcent[i]->SetTitle(";p_{T} [GeV/c];#frac{#Delta p_{T}}{p_{T}} (offset)");
      if(i==0)
	hptcent[i]->DrawCopy("p");
      else
	hptcent[i]->DrawCopy("p same");

      // Efficiency
      ceff->cd();
      TH1D *hnum = 0;
      TH1D *hden = 0;
      fin[i]->GetObject("h3_num",hnum);
      fin[i]->GetObject("h3_den",hden);

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
	  //cout << " i " << i << " eff " << eff << endl; 
	  //double err = BinomialError(pass, all); 
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
      fin[i]->GetObject("h2",hdca2d[i]);
      ctemp1->cd(i+1);
      hdca2d[i]->Draw("colz");

      cslicexy->cd(i+1);
      binlow = hdca2d[i]->GetXaxis()->FindBin(slice_low);
      binhigh = hdca2d[i]->GetXaxis()->FindBin(slice_high);
      TH1D* hdcaxyslice = hdca2d[i]->ProjectionY("hdcaxyslice",binlow, binhigh);
      hdcaxyslice->SetLineColor(col[i]);      
      cslicexy->cd(i+1);
      hdcaxyslice->DrawCopy();
      hdcaxyslice->Fit("gaus");

      cdcaxy->cd();  
      hdca2d[i]->FitSlicesY();
      TH1D*hdcares = (TH1D*)gDirectory->Get("h2_2");
      hdcares->GetYaxis()->SetTitleOffset(2.1);
      hdcares->GetYaxis()->SetTitle("DCA(r#phi) (cm)");
      hdcares->GetXaxis()->SetTitleOffset(1.2);
      hdcares->GetXaxis()->SetTitle("p_{T} (GeV/c)");
      hdcares->SetMarkerStyle(20);
      hdcares->SetMarkerSize(1.2);
      hdcares->SetMarkerColor(col[i]);
      //hdcares->GetYaxis()->SetRangeUser(0, 0.02);
      hdcares->GetYaxis()->SetRangeUser(0, 0.03);
      if(i==0) 
	hdcares->DrawCopy("p");
      else
	hdcares->DrawCopy("p same");

      // dca z resolution
      fin[i]->GetObject("h3",hdcaZ2d[i]);
      ctemp2->cd(i+1);
      hdcaZ2d[i]->Draw("colz");

      binlow = hdcaZ2d[i]->GetXaxis()->FindBin(slice_low);
      binhigh = hdcaZ2d[i]->GetXaxis()->FindBin(slice_high);
      TH1D* hdcazslice = hdcaZ2d[i]->ProjectionY("hdcazslice",binlow, binhigh);
      hdcazslice->SetLineColor(col[i]);      
      csliceZ->cd(i+1);
      hdcazslice->Draw();
      hdcazslice->Fit("gaus");

      cdcaZ->cd();
      hdcaZ2d[i]->FitSlicesY(fz);
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

