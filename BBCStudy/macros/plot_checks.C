#include "get_runstr.h"
#include <iostream>
#include <fstream>
#include <TGraphErrors.h>
#include <TString.h>

int do_zdc = 0;
int do_emcal = 0;
int do_ohcal = 0;
int do_ihcal = 0;
int calc_gain_corr = 1;

const int MAXRUNS = 20000;

int nruns = 0;

void fit_across_runs(TGraphErrors *gainvals[], const char *flist);
void plot_bz();

Double_t bqmeansum[MAXRUNS];            // bq mean in sum of two arms
Double_t bqmeansumerr[MAXRUNS];
Double_t bqmeantot[2][MAXRUNS];         // bq mean in each arm
Double_t bqmeantoterr[2][MAXRUNS];
Double_t bqmean[128][MAXRUNS];
Double_t bqmeanerr[128][MAXRUNS];
Double_t runindex[MAXRUNS];
TGraphErrors *g_meansum {nullptr};
TGraphErrors *g_meantot[2] {nullptr};
TGraphErrors *g_mean[128] {nullptr};

TFile *tfile[MAXRUNS];
TH1 *h_bbcqsum[MAXRUNS];
TH1 *h_bbcqtot[2][MAXRUNS];
TH1 *h_bbcq[MAXRUNS][128];
TH2 *h2_bbcq[128];

TH1 *h_zdce[MAXRUNS];
TH1 *h_emcale[MAXRUNS];
TH1 *h_ohcale[MAXRUNS];
TH1 *h_ihcale[MAXRUNS];

TH1 *h_bz[MAXRUNS];

TCanvas *cv[100];
int icv = 0;

// Get mean gain for each channel across runs, and write out the values
void fit_across_runs(TGraphErrors *gainvals[], const char *flist)
{
  TString meangainfname = "results/"; meangainfname.Append( flist );
  meangainfname.ReplaceAll(".list","_qmean.calib");
  ofstream meangainfile( meangainfname );
  TF1 *f_meangain[128] {nullptr};
  Double_t grp_mean[128];
  Double_t grp_meanerr[128];
  for (int ipmt=0; ipmt<128; ipmt++)
  {
    TString name = "f_meangain"; name += ipmt;
    f_meangain[ipmt] = new TF1(name,"pol0",0,1e9);
    gainvals[ipmt]->Fit(f_meangain[ipmt]);
    gPad->Update();
    gPad->Modified();
    grp_mean[ipmt] = f_meangain[ipmt]->GetParameter(0);
    grp_meanerr[ipmt] = f_meangain[ipmt]->GetParError(0);

    meangainfile << ipmt << "\t" << grp_mean[ipmt] << "\t" << grp_meanerr[ipmt] << endl;
    cout << ipmt << "\t" << grp_mean[ipmt] << "\t" << grp_meanerr[ipmt] << endl;
  }
  meangainfile.close();
}


TH2 *h2_bzvsirun{nullptr};
void plot_bz()
{
  int nbinsx = h_bz[0]->GetNbinsX();
  double xmin = h_bz[0]->GetBinLowEdge(1);
  double xmax = h_bz[0]->GetBinLowEdge(nbinsx+1);
  h2_bzvsirun = new TH2F("h2_bzvsirun","MBD_z vs run idx",nruns,0,nruns,nbinsx,xmin,xmax);

  for (int irun=0; irun<nruns; irun++)
  {
    if ( h_bz[irun]->Integral() == 0 )
    {
      cout << irun << endl;
    }
    for (int ibin=1; ibin<=nbinsx; ibin++)
    {
      float val = h_bz[irun]->GetBinContent(ibin);
      h2_bzvsirun->SetBinContent(irun+1,ibin,val);
    }
  }

  cv[icv] = new TCanvas("bz","bz",1000,600);
  h2_bzvsirun->Draw("colz");
}

void plot_checks(const char *flistname = "c.list")
{

  ifstream flist(flistname);

  TString dir = "results/plot_checks_"; dir += flistname;
  dir.ReplaceAll(".list","/");
  TString name = "mkdir -p " + dir;
  cout << name << endl;
  gSystem->Exec( name );

  TString dstfname;
  while ( flist >> dstfname )
  {
    tfile[nruns] = new TFile(dstfname,"READ");

    h_bz[nruns] = (TH1*)tfile[nruns]->Get("h_bz");

    h_bbcqsum[nruns] = (TH1*)tfile[nruns]->Get("h_bbcqsum");
    h_bbcqtot[0][nruns] = (TH1*)tfile[nruns]->Get("h_bbcqtot0");
    h_bbcqtot[1][nruns] = (TH1*)tfile[nruns]->Get("h_bbcqtot1");
    bqmeansum[nruns] = h_bbcqsum[nruns]->GetMean();
    bqmeansumerr[nruns] = h_bbcqsum[nruns]->GetMeanError();
    bqmeantot[0][nruns] = h_bbcqtot[0][nruns]->GetMean();
    bqmeantoterr[0][nruns] = h_bbcqtot[0][nruns]->GetMeanError();
    bqmeantot[1][nruns] = h_bbcqtot[0][nruns]->GetMean();
    bqmeantoterr[1][nruns] = h_bbcqtot[0][nruns]->GetMeanError();

    h_zdce[nruns] = (TH1*)tfile[nruns]->Get("h_zdce");
    h_emcale[nruns] = (TH1*)tfile[nruns]->Get("h_emcale");
    h_ohcale[nruns] = (TH1*)tfile[nruns]->Get("h_ohcale");
    h_ihcale[nruns] = (TH1*)tfile[nruns]->Get("h_ihcale");

    for (int ipmt=0; ipmt<128; ipmt++)
    {
      name = "h_q"; name += ipmt;
      h_bbcq[nruns][ipmt] = (TH1*)tfile[nruns]->Get(name);
      bqmean[ipmt][nruns] = h_bbcq[nruns][ipmt]->GetMean();
      bqmeanerr[ipmt][nruns] = h_bbcq[nruns][ipmt]->GetMeanError();
    }

    runindex[nruns] = nruns;
    nruns++;
  }
  cout << "Processed " << nruns << " runs." << endl;

  plot_bz();
  return;

  TCanvas *bc = new TCanvas("bc","mean fits",800,600);
  TString title;
  TString meangainfname = dir + flistname;
  meangainfname.ReplaceAll(".list","_qmean.calib");
  ofstream meanfile( meangainfname );
  TF1 *meanfit = new TF1("meanfit","pol0",0,nruns);
  double runmean[128];
  double runmeanerr[128];
  double chi2ndf[128];
  for (int ipmt=0; ipmt<128; ipmt++)
  {
    g_mean[ipmt] = new TGraphErrors(nruns,runindex,bqmean[ipmt],0,bqmeanerr[ipmt]);
    name = "q_mean"; name += ipmt;
    title = name;
    g_mean[ipmt]->SetName( name );
    g_mean[ipmt]->SetTitle( title );

    g_mean[ipmt]->Draw("ap");
    meanfit->SetParameter(0,100);
    g_mean[ipmt]->Fit(meanfit,"R");
    gPad->Modified();
    gPad->Update();

    runmean[ipmt] = meanfit->GetParameter(0);
    runmeanerr[ipmt] = meanfit->GetParError(0);
    Double_t chi2 = meanfit->GetChisquare();
    Double_t ndf = meanfit->GetNDF();
    chi2ndf[ipmt] = chi2/ndf;

    meanfile << ipmt << "\t" << runmean[ipmt] << "\t" << runmeanerr[ipmt] << "\t" << chi2ndf[ipmt] << endl;
    cout << ipmt << "\t" << runmean[ipmt] << "\t" << runmeanerr[ipmt] << "\t" << chi2ndf[ipmt] << endl;
  }
  meanfile.close();

  // Make vs run index plots
  TH2 *h2_bbcqsum_vs_run = new TH2F("h2_bbcqsum_vs_run","bbc qsum vs run",nruns,0,nruns,3000,0,3000);
  TH2 *h2_bbcqs_vs_run = new TH2F("h2_bbcqs_vs_run","bbc south qsum vs run",nruns,0,nruns,1400,0,1400);
  TH2 *h2_bbcqn_vs_run = new TH2F("h2_bbcqn_vs_run","bbc north qsum vs run",nruns,0,nruns,1400,0,1400);
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

      val = h_bbcqtot[0][irun]->GetBinContent(ibin);
      qsum = h_bbcqtot[0][irun]->GetBinCenter(ibin);
      h2_bbcqs_vs_run->Fill( irun, qsum, val );

      val = h_bbcqtot[1][irun]->GetBinContent(ibin);
      qsum = h_bbcqtot[1][irun]->GetBinCenter(ibin);
      h2_bbcqn_vs_run->Fill( irun, qsum, val );
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

  ac[icv] = new TCanvas("mbdq_vs_run2d","MBD Q vs Run",800,600);
  h2_bbcqsum_vs_run->SetMinimum(1e-5);
  h2_bbcqsum_vs_run->DrawCopy("colz");
  gPad->SetLogz(1);

  ac[++icv] = new TCanvas("mbdqs_vs_run2d","MBD Q.S vs Run",800,600);
  h2_bbcqs_vs_run->SetMinimum(1e-6);
  h2_bbcqs_vs_run->DrawCopy("colz");
  gPad->SetLogz(1);

  ac[++icv] = new TCanvas("mbdqn_vs_run2d","MBD Q.N vs Run",800,600);
  h2_bbcqn_vs_run->SetMinimum(1e-6);
  h2_bbcqn_vs_run->DrawCopy("colz");
  gPad->SetLogz(1);

  ac[++icv] = new TCanvas("mbdq_vs_run","MBD Qsum vs Run",800,600);
  g_meansum = new TGraphErrors(nruns,runindex,bqmeansum,0,bqmeansumerr);
  name = "q_meansum";
  title = name;
  g_meansum->SetName( name );
  g_meansum->SetTitle( title );
  g_meansum->SetMarkerStyle(20);
  g_meansum->Draw("ap");

  ac[++icv] = new TCanvas("mbdqs_vs_run","MBD Q.S vs Run",800,600);
  g_meantot[0] = new TGraphErrors(nruns,runindex,bqmeantot[0],0,bqmeantoterr[0]);
  name = "q_meantot0";
  title = name;
  g_meantot[0]->SetName( name );
  g_meantot[0]->SetTitle( title );
  g_meantot[0]->SetMarkerStyle(20);
  g_meantot[0]->Draw("ap");

  ac[++icv] = new TCanvas("mbdqn_vs_run","MBD Q.N vs Run",800,600);
  g_meantot[1] = new TGraphErrors(nruns,runindex,bqmeantot[1],0,bqmeantoterr[1]);
  name = "q_meantot1";
  title = name;
  g_meantot[1]->SetName( name );
  g_meantot[1]->SetTitle( title );
  g_meantot[1]->SetMarkerStyle(20);
  g_meantot[1]->Draw("ap");

  ac[++icv] = new TCanvas("mbdq_vs_run_ch","MBD Q vs Run",1200,800);
  for (int ipmt=0; ipmt<128; ipmt++)
  {
    h2_bbcq[ipmt]->Draw("colz");
    gPad->SetLogz(1);

    name = dir; name += h2_bbcq[ipmt]->GetName(); name += ".png";
    //cout << name << endl;
    ac[icv]->Print( name );
  }

  /*
  ac[++icv] = new TCanvas("mbdq_vs_run_ch_low","MBD Q vs Run, Low",1200,800);
  for (int ipmt=0; ipmt<128; ipmt++)
  {
    h2_bbcq[ipmt]->GetYaxis()->SetRangeUser(0,4);
    h2_bbcq[ipmt]->Draw("colz");

    name = dir; name += h2_bbcq[ipmt]->GetName(); name += "_low.png";
    //cout << name << endl;
    ac[icv]->Print( name );
  }
  */

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
    //int refrun = 0;  // index of reference run
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
      name = dir + "mbd_gaincorrmean.calib";
      gaincorr_calfile.open( name );

      cout << name << endl;

      for (int ipmt=0; ipmt<128; ipmt++)
      {
        double corr = bqmean[ipmt][irun] / runmean[ipmt];
        //double corr = bqmean[ipmt][irun] / bqmean[refun][ipmt];
        if ( fabs(corr-1.0)>0.01 )
        {
          cout << irun << "\t" << ipmt << "\t" << corr << endl;
        }
        gaincorr_calfile << ipmt << "\t" << corr << endl;
      }

      gaincorr_calfile.close();

      irun++;
    }
  }

}
