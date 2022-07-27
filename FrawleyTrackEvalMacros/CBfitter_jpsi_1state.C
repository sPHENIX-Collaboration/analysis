#include <TF1.h>
#include <TMath.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TH2.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TLegend.h>
#include <TStyle.h>
#include <TROOT.h>
#include <TLatex.h>

TH1D *recomass;
TH1D *recomassFG;
TH1D *recomassBG;
 
Double_t CBcalc(Double_t *xx, Double_t *par)
{
  // Crystal Ball lineshape plus exponential background
  // Used for chisq fit to mixed event background data

  double f;
  double x = xx[0];

  // The four CB parameters (alpha, n, x_mean, sigma) plus normalization (N) are:
  double alpha = par[0];
  double n = par[1];
  double x_mean = par[2];
  double sigma = par[3];
  double N = par[4];

  // we need:
  double A = pow( (n/TMath::Abs(alpha)),n) * exp(-pow(alpha,2)/2.0);
  double B = n/TMath::Abs(alpha) - TMath::Abs(alpha);

  // The Crystal Ball function is:
  if( (x-x_mean)/sigma > -alpha)
    {
      f = N * exp( -pow(x-x_mean,2) / (2.0*pow(sigma,2)));
    }
  else
    {
      f = N * A * pow(B - (x-x_mean)/sigma, -n);
    }

  return f;
}

Double_t CBcalc_exp(Double_t *xx, Double_t *par)
{
  // Crystal Ball lineshape plus exponential background
  // Used for chisq fit to mixed event background data

  double f;
  double x = xx[0];

  // The four CB parameters (alpha, n, x_mean, sigma) plus normalization (N) are:
  double alpha = par[0];
  double n = par[1];
  double x_mean = par[2];
  double sigma = par[3];
  double N = par[4];
  // and we add an exponential background
  double Nexp = par[5];
  double slope = par[6];

  // we need:
  double A = pow( (n/TMath::Abs(alpha)),n) * exp(-pow(alpha,2)/2.0);
  double B = n/TMath::Abs(alpha) - TMath::Abs(alpha);

  // The Crystal Ball function is:
  if( (x-x_mean)/sigma > -alpha)
    {
      f = N * exp( -pow(x-x_mean,2) / (2.0*pow(sigma,2)));
    }
  else
    {
      f = N * A * pow(B - (x-x_mean)/sigma, -n);
    }

  f += Nexp * exp(slope * x);

  return f;
}

Double_t CBcalc_LL(Double_t *xx, Double_t *par)
{
  // Crystal Ball lineshape plus exponential background plus mixed event combinatorial background
  // Used for LL fit to foreground data
  double f;
  double x = xx[0];

  // The four CB parameters (alpha, n, x_mean, sigma) plus normalization (N) are:
  double alpha = par[0];
  double n = par[1];
  double x_mean = par[2];
  double sigma = par[3];
  double N = par[4];
  // and we add an exponential background
  double Nexp = par[5];
  double slope = par[6];

  /*
  TF1 *fcbexp = new TF1("fcbexp",CBcalc_exp,2.0,4.0,7);
  fcbexp->SetParameter(0,alpha);
  fcbexp->SetParameter(1,n);
  fcbexp->SetParameter(2,x_mean);
  fcbexp->SetParameter(3,sigma);
  fcbexp->SetParameter(4, N);
  fcbexp->SetParameter(5,Nexp);
  fcbexp->SetParameter(6,slope);
  f = fcbexp->Eval(x);
  */

  // we need:
  double A = pow( (n/TMath::Abs(alpha)),n) * exp(-pow(alpha,2)/2.0);
  double B = n/TMath::Abs(alpha) - TMath::Abs(alpha);

  // The Crystal Ball function is:
  if( (x-x_mean)/sigma > -alpha)
    {
      f = N * exp( -pow(x-x_mean,2) / (2.0*pow(sigma,2)));
    }
  else
    {
      f = N * A * pow(B - (x-x_mean)/sigma, -n);
    }

  f += Nexp * exp(slope * x);
 
  // Add the mixed event background to the fit function
  int ibin = recomassBG->FindBin(x);
 
  double bg12 = recomassBG->GetBinContent(ibin);
  f += bg12;
 

  return f;
}

Double_t Upscalc(Double_t *xx, Double_t *par)
{
  double x = xx[0];
  
  // The input parameters are:
  double alpha1s = par[0];
  double n1s = par[1];
  double sigma1s = par[2];
  double m1s = par[3];
  double m2s = par[4];
  double m3s = par[5];
  double N1s = par[6];
  double N2s = par[7];
  double N3s = par[8];
  double Nexp = par[9];
  double slope = par[10];

  // Construct the Y(1S) CB shape

  TF1 *f1 = new TF1("f1",CBcalc,7,11,5);
  f1->SetParameter(0,alpha1s);
  f1->SetParameter(1,n1s);
  f1->SetParameter(2,m1s);
  f1->SetParameter(3,sigma1s);
  f1->SetParameter(4,N1s);
  
  TF1 *f2 = new TF1("f2",CBcalc,7,11,5);
  f2->SetParameter(0,alpha1s);
  f2->SetParameter(1,n1s);
  f2->SetParameter(2,m2s);
  f2->SetParameter(3,sigma1s);
  f2->SetParameter(4,N2s);

  TF1 *f3 = new TF1("f3",CBcalc,7,11,5);
  f3->SetParameter(0,alpha1s);
  f3->SetParameter(1,n1s);
  f3->SetParameter(2,m3s);
  f3->SetParameter(3,sigma1s);
  f3->SetParameter(4,N3s);

  TF1 *fexp = new TF1("fexp","[0]*exp([1]*x)",7,11);
  fexp->SetParameter(0,Nexp);
  fexp->SetParameter(1,slope);
  
 
  double mass = f1->Eval(x) + f2->Eval(x) + f3->Eval(x) + fexp->Eval(x);

  return mass;
}

void CBfitter_jpsi_1state()
{
  gROOT->SetStyle("Plain");
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);
  gStyle->SetOptTitle(0);

  bool LL_fit = true;

  double maxmass = 3.7;
  double minmass = 2.4;

  TFile *file1S;
  file1S = new TFile("Runcuts_DIMU_MUTRONLY_Run15pp200_JPSISIM_NOEMBED_C_PTBIN0_0_NONE.root");  
  if(!file1S)
    {
      cout << " Failed to open input root file" << endl;
      return;
    }

  TH2D *recomass2D[2];
  TH1D *recomass[2][4];
  for(int iarm = 0; iarm < 2; ++iarm)
    {
      // Get the 2D histos 
      char hname[500];
      sprintf(hname, "mass_y_same_arm%i_chg0", iarm);
      recomass2D[iarm] = (TH2D *)file1S->Get(hname);

      if(!recomass2D[iarm])
	cout << "Failed to get 2D histogram " << endl; 

      // and project the four rapidity bins      
      for(int irap =0; irap < 4; ++irap)
	{
	  char h1dname[500];
	  sprintf(h1dname, "recomass%i%i", iarm, irap);
	  recomass[iarm][irap] = recomass2D[iarm]->ProjectionX(h1dname, irap+1, irap+1);
	}
    }

  TF1 *f1S[2][4];
  TCanvas *cups = new TCanvas("cups","cups",5,5,1600,800);
  cups->Divide(4,2);
  for(int iarm = 0; iarm<2; ++iarm)
    {
      for(int irap = 0; irap < 4; ++irap)
	{
	  cout << "Fit for iarm = " << iarm << " irap = " << irap << endl;

	  cups->cd(irap + iarm*4 + 1);
	  recomass[iarm][irap]->Draw();

	  char fname[500];
	  sprintf(fname, "f1S%i%i", iarm, irap);
	  f1S[iarm][irap] = new TF1(fname,CBcalc, minmass, maxmass, 5);
  
	  f1S[iarm][irap]->SetParameter(0, 1.5);     // alpha
	  f1S[iarm][irap]->SetParameter(1, 100.0);      // n
	  f1S[iarm][irap]->SetParameter(2, 3.12);     // mass
	  f1S[iarm][irap]->SetParameter(3, 0.13);     // sigma
	  f1S[iarm][irap]->SetParameter(4, 300);    // N
	  f1S[iarm][irap]->SetParNames("alpha1S","n1S","m1S","sigma1S","N1S");
	  f1S[iarm][irap]->SetLineColor(kBlue);
	  f1S[iarm][irap]->SetLineWidth(3);
	  f1S[iarm][irap]->SetLineStyle(kDashed);

	  recomass[iarm][irap]->GetXaxis()->SetRangeUser(2.2,3.8);
	  recomass[iarm][irap]->Fit(f1S[iarm][irap]);
	  //recomass[iarm][irap]->Fit(f1S[iarm][irap],"L");
	  recomass[iarm][irap]->DrawCopy();
	  f1S[iarm][irap]->Draw("same");
	}
    }

  // Get the mass and width so we can plot them

  TCanvas *cgr = new TCanvas("cgr", "cgr", 5, 5, 1600, 800);
  cgr->Divide(2,2);

  TLatex *lab[2];
  double rap[2][4] = {-2.075, -1.825, -1.575, -1.325, 1.325, 1.575, 1.825, 2.075};
  double rap_err[4] = {0};
  //TGraph *grmean[2];
  //TGraph *grsigma[2]; 
  TGraphErrors *grmean[2];
  TGraphErrors *grsigma[2]; 
  double mean[2][4];
  double sigma[2][4];
  double mean_err[2][4];
  double sigma_err[2][4];
  for(int iarm = 0; iarm < 2; ++iarm)
    {
      for(int irap = 0; irap < 4; ++irap)
	{
	  mean[iarm][irap] =  f1S[iarm][irap]->GetParameter(2);
	  mean_err[iarm][irap] =  f1S[iarm][irap]->GetParError(2);
	  sigma[iarm][irap] =  f1S[iarm][irap]->GetParameter(3);
	  sigma_err[iarm][irap] =  f1S[iarm][irap]->GetParError(3);
	}
      grmean[iarm] = new TGraphErrors(4, rap[iarm], mean[iarm], rap_err, mean_err[iarm]);
      grsigma[iarm] = new TGraphErrors(4, rap[iarm], sigma[iarm], rap_err, sigma_err[iarm]);

      cgr->cd(1+iarm);
      grmean[iarm]->SetMarkerStyle(20);
      grmean[iarm]->SetMarkerSize(2);
      grmean[iarm]->GetYaxis()->SetTitle("mass (GeV/c^{2})");
      grmean[iarm]->GetYaxis()->SetTitleSize(0.05);
      grmean[iarm]->GetXaxis()->SetTitle("y");
      grmean[iarm]->GetXaxis()->SetTitleSize(0.05);
      grmean[iarm]->Draw();

      char arm[500];
      sprintf(arm,"arm %i",iarm);
      lab[iarm] = new TLatex(0.2, 0.350, arm);
      lab[iarm]->SetNDC();
      lab[iarm]->SetTextSize(0.1);
      lab[iarm]->Draw();

      cgr->cd(3+iarm);
      grsigma[iarm]->SetMarkerStyle(20);
      grsigma[iarm]->SetMarkerSize(2);
      grsigma[iarm]->GetYaxis()->SetTitle("width (GeV/c^{2})");
      grsigma[iarm]->GetYaxis()->SetTitleSize(0.05);
      grsigma[iarm]->GetXaxis()->SetTitle("y");
      grsigma[iarm]->GetYaxis()->SetTitleSize(0.05);
      grsigma[iarm]->Draw();
      lab[iarm]->Draw();
    }



}





