#include <TF1.h>
#include <TMath.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TGraph.h>
#include <TLegend.h>
#include <TStyle.h>
#include <TROOT.h>
#include <TLatex.h>

Double_t CBcalc(Double_t *xx, Double_t *par)
{
  double f;
  double x = xx[0];

  // The four parameters (alpha, n, x_mean, sigma) plus normalization (N) are:
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

  double Nexp = par[5];
  double slope = par[6];
  
  TF1 *fexp = new TF1("fexp","[0]*exp([1]*x)",7,11);
  fexp->SetParameter(0,Nexp);
  fexp->SetParameter(1,slope);

  f = f + fexp->Eval(x);
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


void CBfitter_1state()
{
  gROOT->SetStyle("Plain");
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);
  gStyle->SetOptTitle(0);

  TFile *file1S, *file2S, *file3S;
  TH1 *recomass1S, *recomass2S, *recomass3S;

  bool draw_gauss = false;

  // if all are false, do AuAu 0-10%
  bool pp = false;
  bool pAu = false;
  bool AuAu_20pc = false;
  bool AuAu_10pc = true;

  //bool do_subtracted = true;
 
  file1S = new TFile("root_files/ntp_quarkonium_out.root");

  if(!file1S)
    {
      cout << "Failed to open file  " << endl;
      exit(1); 
    }

  recomass1S = (TH1 *)file1S->Get("recomass0");

  TH1 *recomass = (TH1*)recomass1S->Clone("recomass1");
  if(!recomass)
    {
      cout << "Failed to get recomass histogram  " << endl;
      exit(1); 
    }

  recomass->GetXaxis()->SetTitle("invariant mass (GeV/c^{2})");
  recomass->Sumw2();

  int nrebin = 1;  // set to 2 to match background histo binning, but this worsens resolution slightly. Use 1 for signal fit
  //int nrebin = 2;  // set to 2 to match background histo binning, but this worsens resolution slightly. Use 1 for signal fit
  recomass->Rebin(nrebin);
  
  TCanvas *cups = new TCanvas("cups","cups",5,5,800,800);
  recomass->SetTitle("Y(1S,2S,3S) #rightarrow e^{+}e^{-}");
  recomass->SetMarkerStyle(20);
  recomass->SetMarkerSize(1);
  recomass->SetLineStyle(kSolid);
  recomass->SetLineWidth(2);
  //  recomass->SetMaximum(700);
  recomass->DrawCopy("p");


  TF1 *f1S = new TF1("f1S",CBcalc,7,11,7);
  f1S->SetParameter(0, 1.0);     // alpha
  f1S->SetParameter(1, 1.0);      // n
  f1S->SetParameter(2, 9.46);      // xmean
  f1S->SetParameter(3, 0.08);     // sigma
  f1S->SetParameter(4, 2000.0);    // N
  //f1S->SetParameter(4, 200.0);    // N
  f1S->SetParameter(5,3.5);
  f1S->SetParameter(6,0.05);

  f1S->SetParNames("alpha1S","n1S","m1S","sigma1S","N1S");
  f1S->SetLineColor(kBlue);
  f1S->SetLineWidth(3);
  f1S->SetLineStyle(kDashed);

  recomass->Fit(f1S);
  f1S->Draw("same");
  cout << "f1S pars " <<  f1S->GetParameter(3) << "   " << f1S->GetParError(3) << endl;

  char resstr[500];
  sprintf(resstr,"#sigma_{1S} = %.1f #pm %.1f MeV", f1S->GetParameter(3)*1000, f1S->GetParError(3)*1000);
  TLatex *res = new TLatex(0.13,0.55,resstr);
  res->SetNDC();
  res->SetTextSize(0.05);
  res->Draw();


  double binw = recomass->GetBinWidth(1);
  double renorm = 1.0/binw;   // (1 / (bin_width of data in GeV) )
  cout << "renorm = " << renorm << endl;

  cout << "Area of f1S is " << renorm * f1S->Integral(7,11) << endl;

  // Extract ratio of yield in central gaussian to total

  TF1 *fgauss = new TF1("fgauss","gaus(0)",7,11);
  fgauss->SetParameter(0, f1S->GetParameter(4));
  fgauss->SetParameter(1, f1S->GetParameter(2));
  fgauss->SetParameter(2, f1S->GetParameter(3));
  fgauss->SetLineColor(kRed);
  if(draw_gauss) fgauss->Draw("same");

  // calculate fraction of yield in gaussian
  double area_fgauss =  fgauss->Integral(7,11) * renorm;
  double area_f1S = f1S->Integral(7,11) * renorm;
  double fraction = area_fgauss / area_f1S;


  cout << "Parameters of fgauss = " << fgauss->GetParameter(0) << "  " << fgauss->GetParameter(1) << "  " << fgauss->GetParameter(2) << " Area of fgauss is " << renorm * fgauss->Integral(7,11) << " fraction in fgauss " << area_fgauss / area_f1S << endl;

  char labfrac[500];
  sprintf(labfrac, "Gauss fraction %.2f", fraction);
  TLatex *lab = new TLatex(0.13,0.75,labfrac);
  lab->SetNDC();
  lab->SetTextSize(0.05);
  if(draw_gauss)  lab->Draw();


  /*
  TLatex *lab;

  if(pp)
    lab = new TLatex(0.15,0.75,"p+p, 10 weeks");
  else if(pAu)
    lab = new TLatex(0.20,0.75,"#splitline{p+Au 0-20%}{10 weeks}");
  else if(AuAu_20pc)
    lab = new TLatex(0.20,0.75,"#splitline{Au+Au 0-20%}{20B events}");
  else
    lab = new TLatex(0.20,0.75,"#splitline{Au+Au 0-10%}{10B events}");

  lab->SetNDC();
  lab->SetTextSize(0.05);
  lab->Draw();

  char resstr[500];
  sprintf(resstr,"#sigma_{1S} = %.0f #pm %.0f MeV",sigma1s,sigma1s_err);
  TLatex *res = new TLatex(0.13,0.55,resstr);
  res->SetNDC();
  res->SetTextSize(0.05);
  res->Draw();

  */
  /*
  if (do_subtracted)
    {
      //========================================================
      // Now make the background_subtracted spectrum and show the 
      // CB fits on it
      
      //=======================================
      // Read in the subtracted mass spectrum
      
      // There is only one file to read, it was written by upsilon_mass_plus_background(_pp).C                                          
      TFile *file_all;
      if(pp)
	file_all = new TFile("SplusBtotal_pp_to80.root");
      else
	file_all = new TFile("SplusBtotal_10Bevts_0_10pc_rej90_eff70.root");
      
      TH1 *hsplusb, *hsub, *comb_bgd, *corr_bgd;
      hsplusb = (TH1 *)file_all->Get("hSplusB");
      hsub = (TH1 *)file_all->Get("hsub");
      corr_bgd =  (TH1 *)file_all->Get("hdy_new");
      if(!pp)
	comb_bgd =  (TH1 *)file_all->Get("hfake_new_bgd");
      
      // Fit the correlated background with a function
      
      //TCanvas *c = new TCanvas("c","c",30,30,900,600);  
      TCanvas *c = new TCanvas("c","c",30,30,800,800);  
      
      TF1 *fbgd = new TF1("fbgd","[0]*exp([1]+[2]*x)",7,11);
      fbgd->SetLineColor(kRed);
      fbgd->SetLineWidth(3);
      fbgd->SetLineStyle(kDashed);
      corr_bgd->Fit(fbgd);
      
      hsub->SetMarkerStyle(20);
      hsub->SetMarkerSize(1);
      hsub->GetXaxis()->SetRangeUser(7.8,11.0);
      hsub->Draw("p");
      f1S->Draw("same");
      f2S->Draw("same");
      f3S->Draw("same");
      //fexp->Draw("same");
      fbgd->Draw("same");
      
      // Now want to make the total of all of the fit functions
      
      static const int NSTEP = 200;
      double xm[NSTEP];
      double ym[NSTEP];
      
      double step = 4.0 / (double) NSTEP;
      for(int i=0;i<NSTEP;i++)
	{
	  double x = 7.0 + (double) i * step;
	  double y = f1S->Eval(x);
	  y += f2S->Eval(x);
	  y += f3S->Eval(x);
	  y += fbgd->Eval(x);
	  y += fexp->Eval(x);
	  
	  xm[i] = x;
	  ym[i] = y;
	}
      TGraph *gtot = new TGraph(NSTEP,xm,ym);
      gtot->SetLineWidth(3);
      gtot->SetLineStyle(kSolid);
      gtot->SetLineColor(kBlue);
      gtot->Draw("l");
      
      TLegend *leg = new TLegend(0.62,0.58,0.9,0.88,"");
      leg->SetBorderSize(0);
      leg->SetFillColor(0);
      leg->SetFillStyle(0);
      leg->AddEntry(hsub,"++,-- subtracted","p");
      leg->AddEntry(f1S,"Y(1S)","l");
      leg->AddEntry(f2S,"Y(2S)","l");
      leg->AddEntry(f3S,"Y(3S)","l");
      leg->AddEntry(fbgd,"correlated bkg","l");
      
      leg->Draw();
      
      TLatex *lab2;
      
      if(pp)
	lab2 = new TLatex(0.15,0.75,"p+p, 10 weeks");
      else
	lab2 = new TLatex(0.20,0.75,"#splitline{Au+Au 0-10%}{10B events}");
      
      lab2->SetNDC();
      lab2->SetTextSize(0.05);
      lab2->Draw();
           
    }
  */
}
