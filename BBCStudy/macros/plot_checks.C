#include "get_runstr.h"
#include <iostream>
#include <fstream>

int do_zdc = 0;
int do_emcal = 0;
int do_ohcal = 0;
int do_ihcal = 0;
int calc_gain_corr = 0;

const int MAXRUNS = 20000;
Double_t bqmean[MAXRUNS][128];

TFile *tfile[MAXRUNS];
TH1 *h_bbcqsum[MAXRUNS];
TH1 *h_bbcq[MAXRUNS][128];
TH2 *h2_bbcq[128];

TH1 *h_zdce[MAXRUNS];
TH1 *h_emcale[MAXRUNS];
TH1 *h_ohcale[MAXRUNS];
TH1 *h_ihcale[MAXRUNS];

void plot_checks(const char *flistname = "c.list")
{

  ifstream flist(flistname);

  TString name;
  TString dstfname;
  int nruns = 0;
  while ( flist >> dstfname )
  {
    tfile[nruns] = new TFile(dstfname,"READ");
    h_bbcqsum[nruns] = (TH1*)tfile[nruns]->Get("h_bbcqsum");
    h_zdce[nruns] = (TH1*)tfile[nruns]->Get("h_zdce");
    h_emcale[nruns] = (TH1*)tfile[nruns]->Get("h_emcale");
    h_ohcale[nruns] = (TH1*)tfile[nruns]->Get("h_ohcale");
    h_ihcale[nruns] = (TH1*)tfile[nruns]->Get("h_ihcale");

    for (int ipmt=0; ipmt<128; ipmt++)
    {
      name = "h_bbcq"; name += ipmt;
      h_bbcq[nruns][ipmt] = (TH1*)tfile[nruns]->Get(name);
      bqmean[nruns][ipmt] = h_bbcq[nruns][ipmt]->GetMean();
    }

    nruns++;
  }
  cout << "Processed " << nruns << " runs." << endl;

  // Make vs run index plots
  TH2 *h2_bbcqsum_vs_run = new TH2F("h2_bbcqsum_vs_run","bbc qsum vs run",nruns,0,nruns,3000,0,3000);
  for (int ipmt=0; ipmt<128; ipmt++)
  {
    name = "h2_bbcq"; name += ipmt;
    h2_bbcq[ipmt] = new TH2F(name,name,nruns,0,nruns,1200,0,60);
  }
  TH2 *h2_zdce_vs_run {nullptr};
  TH2 *h2_emcale_vs_run {nullptr};
  TH2 *h2_ohcale_vs_run {nullptr};
  TH2 *h2_ihcale_vs_run {nullptr};
  if ( do_zdc )
  {
    int nbinsx = h_zdce[0]->GetNbinsX();
    Double_t xmin = h_zdce[0]->GetBinLowEdge(1);
    Double_t xmax = h_zdce[0]->GetBinLowEdge(nbinsx+1);
    h2_zdce_vs_run = new TH2F("h2_zdce_vs_run","ZDC E vs run",nruns,0,nruns,nbinsx,xmin,xmax);
  }
  if ( do_emcal )
  {
    int nbinsx = h_emcale[0]->GetNbinsX();
    Double_t xmin = h_emcale[0]->GetBinLowEdge(1);
    Double_t xmax = h_emcale[0]->GetBinLowEdge(nbinsx+1);
    h2_emcale_vs_run = new TH2F("h2_emcale_vs_run","EMC E vs run",nruns,0,nruns,nbinsx,xmin,xmax);
  }
  if ( do_ohcal )
  {
    int nbinsx = h_ohcale[0]->GetNbinsX();
    Double_t xmin = h_ohcale[0]->GetBinLowEdge(1);
    Double_t xmax = h_ohcale[0]->GetBinLowEdge(nbinsx+1);
    h2_ohcale_vs_run = new TH2F("h2_ohcale_vs_run","OHCAL E vs run",nruns,0,nruns,nbinsx,xmin,xmax);
  }
  if ( do_ihcal )
  {
    int nbinsx = h_ihcale[0]->GetNbinsX();
    Double_t xmin = h_ihcale[0]->GetBinLowEdge(1);
    Double_t xmax = h_ihcale[0]->GetBinLowEdge(nbinsx+1);
    h2_ihcale_vs_run = new TH2F("h2_ihcale_vs_run","iHCAL E vs run",nruns,0,nruns,nbinsx,xmin,xmax);
  }


  for (int irun=0; irun<nruns; irun++)
  {
    int nbinsx = h_bbcqsum[irun]->GetNbinsX();
    for (int ibin=1; ibin<=nbinsx; ibin++)
    {
      Float_t val = h_bbcqsum[irun]->GetBinContent(ibin);
      Float_t qsum = h_bbcqsum[irun]->GetBinCenter(ibin);
      h2_bbcqsum_vs_run->Fill( irun, qsum, val );
    }

    for (int ipmt=0; ipmt<128; ipmt++)
    {
      int nbinsx = h_bbcq[irun][ipmt]->GetNbinsX();
      for (int ibin=1; ibin<=nbinsx; ibin++)
      {
        Double_t val = h_bbcq[irun][ipmt]->GetBinContent(ibin);
        Double_t q = h_bbcq[irun][ipmt]->GetBinCenter(ibin);
        h2_bbcq[ipmt]->Fill( (Double_t)irun, q, val );
      }
    }

    if ( do_zdc )
    {
      int nbinsx = h_zdce[irun]->GetNbinsX();
      for (int ibin=1; ibin<=nbinsx; ibin++)
      {
        Float_t val = h_zdce[irun]->GetBinContent(ibin);
        Float_t e = h_zdce[irun]->GetBinCenter(ibin);
        h2_zdce_vs_run->Fill( irun, e, val );
      }
    }

    if ( do_emcal )
    {
      int nbinsx = h_emcale[irun]->GetNbinsX();
      for (int ibin=1; ibin<=nbinsx; ibin++)
      {
        Float_t val = h_emcale[irun]->GetBinContent(ibin);
        Float_t emce = h_emcale[irun]->GetBinCenter(ibin);
        h2_emcale_vs_run->Fill( irun, emce, val );
      }
    }

    if ( do_ohcal )
    {
      int nbinsx = h_ohcale[irun]->GetNbinsX();
      for (int ibin=1; ibin<=nbinsx; ibin++)
      {
        Float_t val = h_ohcale[irun]->GetBinContent(ibin);
        Float_t e = h_ohcale[irun]->GetBinCenter(ibin);
        h2_ohcale_vs_run->Fill( irun, e, val );
      }
    }

    if ( do_ihcal )
    {
      int nbinsx = h_ihcale[irun]->GetNbinsX();
      for (int ibin=1; ibin<=nbinsx; ibin++)
      {
        Float_t val = h_ihcale[irun]->GetBinContent(ibin);
        Float_t e = h_ihcale[irun]->GetBinCenter(ibin);
        h2_ihcale_vs_run->Fill( irun, e, val );
      }
    }


  }

  TCanvas *ac[100];
  int icv = 0;

  ac[icv] = new TCanvas("mbdq_vs_run","MBD Q vs Run",800,600);
  h2_bbcqsum_vs_run->SetMinimum(1e-5);
  h2_bbcqsum_vs_run->DrawCopy("colz");
  gPad->SetLogz(1);

  ac[++icv] = new TCanvas("mbdq_vs_run_low","MBD Q vs Run",1200,800);
  for (int ipmt=0; ipmt<128; ipmt++)
  {
    //h2_bbcq[ipmt]->GetYaxis()->SetRangeUser(0,4);
    h2_bbcq[ipmt]->Draw("colz");
    gPad->SetLogz(1);

    name = h2_bbcq[ipmt]->GetName(); name += ".png";
    cout << name << endl;
    ac[icv]->Print( name );
  }

  int run1 = 0;
  int run2 = 1;
  /*
     h_bbcqsum[run1]->Rebin(8); 
     h_bbcqsum[run2]->Rebin(8); 
     h_bbcqsum[run1]->Draw("hist"); 
     h_bbcqsum[run2]->SetLineColor(2); 
     h_bbcqsum[run2]->Draw("histsame"); 
     gPad->SetLogy(1);
     */

  if ( do_zdc )
  {
    ac[++icv] = new TCanvas("zdce_vs_run","ZDC E vs Run",800,600);
    h2_zdce_vs_run->SetMinimum(1e-6);
    h2_zdce_vs_run->Draw("colz");
    gPad->SetLogz(1);
  }

  if ( do_emcal )
  {
    ac[++icv] = new TCanvas("emce_vs_run","EMC E vs Run",800,600);
    h2_emcale_vs_run->SetMinimum(1e-6);
    h2_emcale_vs_run->Draw("colz");
    gPad->SetLogz(1);

    h_emcale[run1]->Rebin(4); 
    h_emcale[run2]->Rebin(4); 
    h_emcale[run2]->Scale(0.6); 
    h_emcale[run1]->Draw("hist"); 
    h_emcale[run2]->SetLineColor(2); 
    h_emcale[run2]->Draw("histsame"); 
    gPad->SetLogy(1);
  }

  if ( do_ohcal )
  {
    ac[++icv] = new TCanvas("ohcal_vs_run","oHCAL E vs Run",800,600);
    h2_ohcale_vs_run->SetMinimum(1e-6);
    h2_ohcale_vs_run->Draw("colz");
    gPad->SetLogz(1);

    h_ohcale[run1]->Rebin(4); 
    h_ohcale[run2]->Rebin(4); 
    h_ohcale[run2]->Scale(0.6); 
    h_ohcale[run1]->Draw("hist"); 
    h_ohcale[run2]->SetLineColor(2); 
    h_ohcale[run2]->Draw("histsame"); 
    gPad->SetLogy(1);
  }

  if ( do_ihcal )
  {
    ac[++icv] = new TCanvas("ihcal_vs_run","iHCAL E vs Run",800,600);
    h2_ihcale_vs_run->SetMinimum(1e-6);
    h2_ihcale_vs_run->Draw("colz");
    gPad->SetLogz(1);

    h_ihcale[run1]->Rebin(4); 
    h_ihcale[run2]->Rebin(4); 
    h_ihcale[run2]->Scale(0.6); 
    h_ihcale[run1]->Draw("hist"); 
    h_ihcale[run2]->SetLineColor(2); 
    h_ihcale[run2]->Draw("histsame"); 
    gPad->SetLogy(1);
  }


  // find gain corrections
  if ( calc_gain_corr )
  {
    int refrun = 64;  // index of reference run
    flist.clear();
    flist.seekg(0);
    int irun = 0;
    ofstream gaincorr_calfile;
    while ( flist >> dstfname )
    {
      TString dir = "results/";
      dir += get_runstr(dstfname);
      dir += "/";
      name = "mkdir -p " + dir;
      gSystem->Exec( name );
      name = dir + "mbd_gaincorr2.calib";
      gaincorr_calfile.open( name );

      cout << name << endl;

      for (int ipmt=0; ipmt<128; ipmt++)
      {
        double corr = bqmean[irun][ipmt] / bqmean[refrun][ipmt];
        if ( fabs(corr-1.0)>0.01 )
        {
          cout << irun << "\t" << refrun << "\t" << ipmt << "\t" << corr << endl;
        }
        gaincorr_calfile << ipmt << "\t" << corr << endl;
      }

      gaincorr_calfile.close();

      irun++;
    }
  }

}
