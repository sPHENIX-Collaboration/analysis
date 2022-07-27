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


void CBfitter()
{
  gROOT->SetStyle("Plain");
  gStyle->SetOptStat(0);
  gStyle->SetOptTitle(1);

  bool ups1s = true;
  bool ups2s = true;
  bool ups3s = true;

  TFile *file1S, *file2S, *file3S;
  TH1 *recomass1S, *recomass2S, *recomass3S;

  if(ups1s)
    {
      file1S = new TFile("root_files/unsuppressed_auau_24B_ntp_quarkonium_out.root");
      recomass1S = (TH1 *)file1S->Get("recomass0");
    }

  if(ups2s)
    {
      //file2S = new TFile(" root_files/ups2s_80ns_100pions_pp.root ");
      recomass2S = (TH1 *)file1S->Get("recomass1");
    }
  if(ups3s)
    {
      //file3S = new TFile(" root_files/ups3s_80ns_100pions_pp.root");
      recomass3S = (TH1 *)file1S->Get("recomass2");
    }

  TFile *massout = new TFile("upsilon_mass_histos_out.root","recreate");

  TH1 *recomass = (TH1*)recomass1S->Clone("recomass");
  recomass->Sumw2();
  if(ups2s)
    recomass->Add(recomass2S);
  if(ups3s)
    recomass->Add(recomass3S);
  int nrebin = 1;  // a rebin of 2 is needed to match background histo binning, use 1 if want best resolution
  recomass->Rebin(nrebin);
  
  //TCanvas *cups = new TCanvas("cups","cups",5,5,800,600);
  TCanvas *cups = new TCanvas("cups","cups",5,5,800,800);
  recomass->SetTitle("Y(1S,2S,3S) #rightarrow e^{+}e^{-}");
  recomass->GetYaxis()->SetNdivisions(205);
  recomass->GetXaxis()->SetTitle("Invariant mass (GeV/c^{2})");
  recomass->SetMarkerStyle(20);
  recomass->SetMarkerSize(1);
  recomass->SetLineStyle(kSolid);
  recomass->SetLineWidth(2);
  //recomass->SetMaximum(810);
  recomass->DrawCopy("p");

 
  TF1 *fitups = new TF1("fitups",Upscalc,7,11,11);
  fitups->SetParameter(0, 1.05);
  fitups->SetParameter(1, 1.5);
  fitups->SetParameter(2, 0.09);
  fitups->SetParameter(3, 9.44);
  fitups->SetParameter(4, 10.0);       
  fitups->SetParameter(5, 10.3);
  fitups->SetParameter(6, 900.0);
  fitups->SetParameter(7, 200.0);
  fitups->SetParameter(8, 100.0);
  //fitups->SetParameter(9, 10.0);
  fitups->FixParameter(9, 0.0);
  fitups->FixParameter(10, -0.2);
  fitups->SetLineColor(kBlack);  
  fitups->SetParNames("alpha1S","n1S","sigma1S","m1S","m2S","m3S","N1S","N2S","N3S","Nexp","expSlope");

  recomass->Fit(fitups);
  //fitups->Draw("same");

  double sigma1s = 1000.0 * fitups->GetParameter(2);
  double sigma1s_err = 1000.0 * fitups->GetParError(2);
  cout << " sigma1s " << sigma1s << " par2 " << fitups->GetParameter(2) << endl; 
  // Now draw the individual states

  TF1 *f1S = new TF1("f1S",CBcalc,7,11,5);
  f1S->SetParameter(0,fitups->GetParameter(0));
  f1S->SetParameter(1,fitups->GetParameter(1));
  f1S->SetParameter(2,fitups->GetParameter(3));
  f1S->SetParameter(3,fitups->GetParameter(2));
  f1S->SetParameter(4,fitups->GetParameter(6));
  f1S->SetParNames("alpha1S","n1S","m1S","sigma1S","N1S");
  f1S->SetLineColor(kBlue);
  f1S->SetLineWidth(3);
  f1S->SetLineStyle(kDashed);

  f1S->Draw("same");

  TF1 *f2S = new TF1("f2S",CBcalc,7,11,5);
  f2S->SetParameter(0,fitups->GetParameter(0));
  f2S->SetParameter(1,fitups->GetParameter(1));
  f2S->SetParameter(2,fitups->GetParameter(4));
  f2S->SetParameter(3,fitups->GetParameter(2));
  f2S->SetParameter(4,fitups->GetParameter(7));
  f2S->SetParNames("alpha1S","n1S","m2S","sigma1S","N2S");
  f2S->SetLineColor(kRed);
  f2S->SetLineWidth(3);
  f2S->SetLineStyle(kDashed);

  f2S->Draw("same");

  TF1 *f3S = new TF1("f3S",CBcalc,7,11,5);
  f3S->SetParameter(0,fitups->GetParameter(0));
  f3S->SetParameter(1,fitups->GetParameter(1));
  f3S->SetParameter(2,fitups->GetParameter(5));
  f3S->SetParameter(3,fitups->GetParameter(2));
  f3S->SetParameter(4,fitups->GetParameter(8));
  f3S->SetParNames("alpha1S","n1S","m3S","sigma1S","N3S");
  f3S->SetLineColor(kMagenta);
  f3S->SetLineWidth(3);
  f3S->SetLineStyle(kDashed);

  f3S->Draw("same");

  TF1 *fexp = new TF1("fexp","[0]*exp([1]*x)",7,11);
  fexp->SetParameter(0,fitups->GetParameter(9));
  fexp->SetParameter(1,fitups->GetParameter(10));

  fexp->SetLineColor(kGreen);
  fexp->SetLineWidth(3);
  fexp->SetLineStyle(kDashed);
  fexp->Draw("same");

  double binw = recomass->GetBinWidth(1);
  double renorm = 1.0/binw;   // (1 / (bin_width of data in GeV) )
  //double renorm = 25.0;   // (1 / (bin_width of data in GeV) )
  cout << "renorm = " << renorm << endl;

  cout << "Area of f1S is " << renorm * f1S->Integral(7,11) << endl;
  cout << "Area of f2S is " << renorm * f2S->Integral(7,11) << endl;
  cout << "Area of f3S is " << renorm * f3S->Integral(7,11) << endl;
  cout << "Area of fexp is " << renorm * fexp->Integral(7,11) << endl;

  bool pp = true;
  bool AuAu_20pc = false;

  bool do_subtracted = false;

  TLatex *lab;

  if(pp)
    lab = new TLatex(0.15,0.75,"#splitline{p+p, 197 pb^{-1}}{signal only}");
  else if(AuAu_20pc)
    lab = new TLatex(0.20,0.75,"#splitline{Au+Au 0-20%}{10B events}");
  else
    lab = new TLatex(0.20,0.75,"#splitline{Au+Au MB}{50B events}");

  lab->SetNDC();
  lab->SetTextSize(0.05);
  //lab->Draw();

  char resstr[500];
  sprintf(resstr,"#sigma_{1S} = %.0f #pm %.1f MeV",sigma1s,sigma1s_err);
  TLatex *res = new TLatex(0.13,0.55,resstr);
  res->SetNDC();
  res->SetTextSize(0.05);
  //res->Draw();

  recomass->Write();
  f1S->Write();
  f2S->Write();
  f3S->Write();

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
	file_all = new TFile("SplusBtotal_rej90_to80.root");
      
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
	lab2 = new TLatex(0.20,0.75,"#splitline{Au+Au 0-20%}{10B events}");
      
      lab2->SetNDC();
      lab2->SetTextSize(0.05);
      lab2->Draw();
           
    }
}
