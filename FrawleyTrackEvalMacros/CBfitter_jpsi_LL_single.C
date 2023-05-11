#include <TF1.h>
#include <TMath.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TH2.h>
#include <TGraph.h>
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

void CBfitter_jpsi_LL_single()
{
  gROOT->SetStyle("Plain");
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);
  gStyle->SetOptTitle(0);

  bool LL_fit = true;

  double maxmass = 3.5;

  TFile *file1S;
   file1S = new TFile("Final_Hists_mee_meeBG_Type_VetoB0.root");
  
  if(!file1S)
    {
      cout << " Failed to open input root file" << endl;
      return;
    }
  
  recomassFG = (TH1D *)file1S->Get("Hunlk");
  if(!recomassFG)
    cout << "Failed to get Hunlk" << endl; 

  recomassBG = (TH1D *)file1S->Get("HmxNorm");
  if(!recomassBG)
    cout << "Failed to get HmxNorm" << endl; 

  TCanvas *cups = new TCanvas("cups","cups",5,5,1000,800);
  recomassFG->Draw();
  //recomassBG->Draw("same");

  TF1 *f1S;

  if(LL_fit)
    { 
      // fit FG with CB+exp+BG12 using LL
      f1S = new TF1("f1SLL",CBcalc_LL,1.3,maxmass,7);
    }
  else
    { 
      // fit FG - BG with CB+exp using chisq
      f1S = new TF1("f1S",CBcalc_exp,1.3,maxmass,7);
    }
  
  // These are an error weighted average of the four free fits below in the comments
  // Use these values to describe the J/psi line shape in fits at all cenralities
  
  f1S->SetParameter(0, -0.5);     // alpha
  f1S->SetParameter(1, 2.0);      // n
  f1S->SetParameter(2, 3.07);     // mass
  f1S->SetParameter(3, 0.11);     // sigma
  f1S->SetParameter(4, 850);    // N
  f1S->FixParameter(5, 0.0);    // Nexp
  f1S->FixParameter(6, -1.00);    // slope
  f1S->SetParNames("alpha1S","n1S","m1S","sigma1S","N1S","Nexp","slope");
  
  f1S->SetLineColor(kBlue);
  f1S->SetLineWidth(3);
  f1S->SetLineStyle(kDashed);

  TCanvas *csig = new TCanvas("csig","csig",5,5,1000,800);

  TF1 *fitResult= 0;
  if(LL_fit)
    {
      // log likelihood fit with integer data option is "L"
      //recomassFG->Fit(f1S,"LQ");
      recomassFG->Fit(f1S,"L R");
      recomassFG->DrawCopy();
      fitResult = recomassFG->GetFunction("f1SLL");
    }
  else
    {
      recomassFG->Fit(f1S);
      recomassFG->DrawCopy();
      fitResult = recomassFG->GetFunction("CBcalc_exp");
    }
  if(fitResult)
    {
      cout << " chisquare " <<  fitResult->GetChisquare() << " NDF " <<  fitResult->GetNDF() << endl;
    }
  else
    {
      cout << "Failed to get fitResult" << endl;
      return;
    }
  
  // Plot the fit results
  //======================
  // Make a function that is only the CB lineshape and plot it
  // We want CBcalc for this
  TF1 *fcbonly = new TF1("fcbonly",CBcalc,1.3,4.0,5);
  fcbonly->SetParameter(0,f1S->GetParameter(0));
  fcbonly->SetParameter(1,f1S->GetParameter(1));
  fcbonly->SetParameter(2,f1S->GetParameter(2));
  fcbonly->SetParameter(3,f1S->GetParameter(3));
  fcbonly->SetParameter(4,f1S->GetParameter(4));
  fcbonly->SetLineColor(kMagenta);
  fcbonly->Draw("same");
  
  // make a function that is only the exponential lineshape and plot it
  TF1 *fexp = new TF1("fexp","[0]*exp([1]*x)",1.3,4.0);
  fexp->SetParameter(0,f1S->GetParameter(5));
  fexp->SetParameter(1,f1S->GetParameter(6));
  
  fexp->SetLineColor(kGreen);
  fexp->SetLineWidth(3);
  fexp->SetLineStyle(kDashed);
  fexp->Draw("same");
  
  // Finally, plot the mixed event background
  if(LL_fit)
    {
      recomassBG->SetLineColor(kRed);
      recomassBG->DrawCopy("same");
    }
  
  // calculate yield uncertainty from fit
  double frac_yerror = f1S->GetParError(4) / f1S->GetParameter(4);
  
  /*
  CB_yield[icent][ipt] = renorm * fcbonly->Integral(2.8,3.4) ;
  CB_yield_err[icent][ipt] = CB_yield[icent][ipt] * frac_yerror;
  
  best_CB_yield[ialpha][in1][im1][isig] = CB_yield[icent][ipt];
  best_CB_yield_err[ialpha][in1][im1][isig] = CB_yield_err[icent][ipt];
  */

  // On a separate canvas, plot the FG - BG and the CB+exp
  //========================================
  
  recomassFG->DrawCopy("pe");
  recomassBG->DrawCopy("same");
  fexp->Draw("same");
  fcbonly->Draw("same");
  
  
}




