#include "sPhenixStyle.h"
#include "sPhenixStyle.C"

const int nIso = 1;
const int nIter = 6;

const float isoParams[] = {0, 1, 1.5, 2, 2.5};

string generateFit(int order, string function);

void numericInverse(int save = 0, int isUESubtracted = 1, int is10GeV = 0)
{
  SetsPhenixStyle();
  
  TGraphErrors *lin[nIso];

  TFile *fin[nIso];
  TCanvas *c1 = new TCanvas("linearity","linearity");
  TCanvas *cUnity = new TCanvas("UnityDiff","UnityDiff");
  TCanvas *cDiff = new TCanvas("fitDiff","FitDiff");
  float fitStart = 15;
  float fitEnd;
  if(!is10GeV) fitEnd = 60;
  else fitEnd = 45;
  TLegend *leg = new TLegend(0.6,0.2,0.9,0.5);
  leg -> SetFillStyle(0);
  leg -> SetBorderSize(0);
  //draw and fit our jet energy linearity
  int colors[] = {1, 2, 4, kGreen +2, kViolet, kCyan, kOrange, kMagenta+2, kAzure-2};
  TF1 *linFit[nIso];
  TH2F *pt_true_matched[nIso];
  TF1 *unity = new TF1("unity","x",0,80);
  unity -> SetLineColor(1);
  unity -> SetLineStyle(8);
  TGraphErrors *unityDiff[nIso];
  TGraphErrors *chi2NDF[nIso];
  TGraphErrors *fitDiff[nIso][3];
  for(int i = 0; i < nIso; i++)
    {

      linFit[i] = new TF1(Form("linFit_iso%d",i),"[0] + [1]*x",fitStart,fitEnd);
      linFit[i] -> SetLineColor(colors[i]);
      fin[i] = new TFile(Form("hists_R04_dR%g_Corr0_isLin1.root",isoParams[i]));
      pt_true_matched[i] = (TH2F*)fin[i] -> Get("h_pt_true_matched");

      lin[i] = (TGraphErrors*)fin[i] -> Get("g_jes_cent0");
      lin[i] -> SetMarkerColor(colors[i]);
      lin[i] -> GetYaxis() -> SetRangeUser(0,70);
      lin[i] -> GetXaxis() -> SetLimits(0,80);
      lin[i] -> SetTitle(";p_{T,truth} [GeV/c];#LTp_{T,reco}#GT [GeV/c]");
      c1 -> cd();
      if(i == 0) lin[i] -> Draw("ap");
      else lin[i] -> Draw("samep");
      unityDiff[i] = new TGraphErrors();
      for(int j = 0; j < lin[i] -> GetN(); j++)
	{
	  Double_t x, y;
	  lin[i] -> GetPoint(j,x,y);
	  
	  unityDiff[i] -> SetPoint(j,x,y-x);
	}
      lin[i] -> Fit(linFit[i],"RQ");
      fitDiff[i][0] = new TGraphErrors();
      for(int n = 0; n < lin[i] -> GetN(); n++)
	{
	  Double_t x,y; 
	  lin[i] -> GetPoint(n,x,y);
		      
	  if(x < fitEnd)
	    { 
	      fitDiff[i][0] -> SetPoint(n, x, (y - linFit[i] -> Eval(x))/y);
	    }
	}
      fitDiff[i][0] -> SetTitle(";p_{T,truth} [GeV/c];#frac{(Data - Fit)}{Data}");
      fitDiff[i][0] -> SetMarkerColor(colors[i]);
      fitDiff[i][0] -> GetYaxis() -> SetRangeUser(-1,1);
      cUnity -> cd();
      unityDiff[i] -> SetTitle(";p_{T,truth} [GeV/c];#LTp_{T,reco}#GT - p_{T,Truth} [GeV/c]");
      unityDiff[i] -> GetYaxis() -> SetRangeUser(-30,5);
      unityDiff[i] -> SetMarkerColor(colors[i]);
      if(i == 0)unityDiff[i] -> Draw("ap");
      else unityDiff[i] -> Draw("samep");
      leg -> AddEntry(lin[i],Form("#DeltaR = %gR",isoParams[i]),"p");
      
      cDiff -> cd();
      if(i == 0) fitDiff[i][0] -> Draw("ap");
      else fitDiff[i][0] -> Draw("samep");
      
    }
  c1 -> cd(); leg -> Draw("same"); unity -> Draw("same");
  cUnity -> cd(); leg -> Draw("same"); 
  cDiff -> cd(); leg -> Draw("same");
  if(save)
    {
      c1 -> SaveAs("plots/lin_isoStack.pdf");
      cUnity -> SaveAs("plots/unityDiff.pdf");
      cDiff -> SaveAs("plots/fitDiff0.pdf");
    }
  
  //for the numerical inversion, first we must compute
  //the function R, which is the avaerage JES
  
  TGraphErrors *r_lin[nIso];
  const int nPoints = lin[0] -> GetN();
  TF1 *rFit[nIso][nIter]; 
  TCanvas *cChi2 = new TCanvas("chi2","resolution");
  TCanvas *c2 = new TCanvas("resolution","resolution");
  int bestIter[nIso] = {-1};
  TLine *bestIterLn[nIso];
  
  for(int i = 0; i < nIso; i++)
    {
      r_lin[i] = new TGraphErrors();
      c2 -> cd();
     
      for(int j = 0; j < nPoints; j++)
	{
	  Double_t x, y, erry, errx;
	  lin[i] -> GetPoint(j, x, y);
	  erry = lin[i] -> GetErrorY(j);
	  errx = lin[i] -> GetErrorX(j);
	  TH1F *pt_true_proj = (TH1F*)pt_true_matched[i] -> ProjectionX();
	  pt_true_proj -> GetXaxis() -> SetRangeUser(x - errx, x + errx);
	  x = pt_true_proj -> GetMean();
		  
	  r_lin[i] -> SetPoint(j, x, y/x);
	  float err = y/x * sqrt(pow(erry/y,2) /*+ pow((errx/2)/x,2)*/);
	  r_lin[i] -> SetPointError(j, errx, err);
	  
	}
      r_lin[i] -> SetMarkerColor(colors[i]);
      r_lin[i] -> SetTitle(";p_{T,truth} [GeV/c];#LTp_{T,reco}#GT/#LTp_{T,truth}#GT");
      r_lin[i] -> GetYaxis() -> SetRangeUser(0.5,1);
      
      string fit = "[0]";
      chi2NDF[i] = new TGraphErrors();
      chi2NDF[i] -> SetTitle(";Iterations;chi2/NDF");
      chi2NDF[i] -> SetMarkerColor(colors[i]);
      float minChi2NDF = 9999;
      //Double_t xstart, ystart;
      //r_lin[i] -> GetPoint(1,xstart,ystart);
      for(int l = 0; l < nIter; l++)
	{
	  rFit[i][l] = new TF1("polyFitBase",fit.c_str(),fitStart,fitEnd);
	  rFit[i][l] -> SetLineColor(colors[i]);
		    
	  r_lin[i] -> Fit(rFit[i][l],"Q0","0",fitStart,fitEnd);
	  r_lin[i] -> GetYaxis() -> SetRangeUser(0.5,1);
	  float chi2, ndf;
	  chi2 = rFit[i][l] -> GetChisquare();
	  ndf = rFit[i][l] -> GetNDF();
	  chi2NDF[i] -> SetPoint(l,l,chi2/ndf);
	  if(chi2/ndf < minChi2NDF)
	    {
	      minChi2NDF = chi2/ndf; 
	      bestIter[i] = l;
	    }
	      
	      
	  fit = generateFit(l+1,fit);
	}
      
      fitDiff[i][1] = new TGraphErrors();
      for(int n = 0; n < r_lin[i] -> GetN(); n++)
	{
	  Double_t x,y;
	  r_lin[i] -> GetPoint(n,x,y);
	  if(x < fitEnd)
	    {
	      fitDiff[i][1] -> SetPoint(n, x, (y - rFit[i][bestIter[i]] -> Eval(x))/y);
	    }
	}
      fitDiff[i][1] -> SetMarkerColor(colors[i]);
      fitDiff[i][1] -> SetTitle(";p_{T,truth} [GeV/c];#frac{(Data - Fit)}{Data}");
      fitDiff[i][1] -> GetYaxis() -> SetRangeUser(-1,1);

      bestIterLn[i] = new TLine(bestIter[i],0,bestIter[i],20);
      bestIterLn[i] -> SetLineColor(colors[i]);
      bestIterLn[i] -> SetLineStyle(8);
      
      
      c2 -> cd();
      if(i == 0)
	{
	  r_lin[i] -> GetXaxis() -> SetLimits(0,80);
	  r_lin[i] -> Draw("ap");
	}
      else r_lin[i] -> Draw("samep");
      rFit[i][bestIter[i]] -> Draw("same");
      
      cChi2 -> cd(); 
      if(i == 0)
	{
	  chi2NDF[i] -> GetYaxis() -> SetRangeUser(0,20);
	  chi2NDF[i] -> Draw("ap");
	    
	}
      else chi2NDF[i] -> Draw("samep");
      bestIterLn[i] -> Draw("same");

      cDiff -> cd();
      if(i == 0)fitDiff[i][1] -> Draw("ap");
      else fitDiff[i][1] -> Draw("samep");
    }
  TLine *thirty = new TLine(30,0.5,30,1);
  thirty -> SetLineStyle(8);
  thirty -> SetLineColor(1);
  c2 -> cd();
  thirty -> Draw("same");
  leg -> Draw("same");
  cChi2 -> cd();
  //gPad -> SetLogy();
  leg -> Draw("same");
  cDiff -> cd(); leg -> Draw("same");
  
  if(save)
    {
      c2 -> SaveAs("plots/fMeanScaled.pdf");
      cChi2 -> SaveAs("plots/chi2NdffMean.pdf");
      cDiff -> SaveAs("plots/fitDiff1.pdf");
    }
  //Now, in order to get the calibration factor, we have to compute R^-1
  //Which is defined by R^-1(y) = R(f^-1(y))
  
  TGraphErrors *rTilde[nIso];
  TGraphErrors *gfInv[nIso];
  TF1 *correctionFit[nIso][nIter];
  TCanvas *c3 = new TCanvas();
  TCanvas *c4 = new TCanvas();
  TCanvas *cChi22 = new TCanvas();

  for(int i = 0; i < nIso; i++)
    {
      rTilde[i] = new TGraphErrors();
      gfInv[i] = new TGraphErrors();
      //if(!isUESubtracted)correctionFit[i] = new TF1(Form("corrFit_Iso%g",isoParams[i]),"[0] + [1]*x");
      //else{ correctionFit[i] = new TF1(Form("corrFit_Iso%g",isoParams[i]),"[0]*exp([1]*x) + [2]");
      for(int j = 0; j < nPoints; j++)
	{
	  Double_t x, y;
	  lin[i] -> GetPoint(j, x, y);
	  if(x < fitStart || x > fitEnd) continue;
	  float fInv = linFit[i] -> GetX(y);
	  gfInv[i] -> SetPoint(j, y, fInv);
	  //gfInv[i] -> SetPointError(j, lin
	  float R = rFit[i][bestIter[i]] -> Eval(fInv);
	  //if(y <= fitStart)continue;
	  rTilde[i] -> SetPoint(j, y, 1/R);
	}
      rTilde[i] -> SetMarkerColor(colors[i]);
      gfInv[i] -> SetMarkerColor(colors[i]);

      rTilde[i] -> SetTitle(";#LTp_{T,reco}#GT [GeV/c];1/#tilde{R}(p_{T,reco})");
      gfInv[i] -> SetTitle(";#LTp_{T,reco}#GT [GeV/c];p_{T,truth} [GeV/c]");

      rTilde[i] -> GetYaxis() -> SetRangeUser(0,2);
      string fit = "[0]";
      chi2NDF[i] = new TGraphErrors();
      chi2NDF[i] -> SetTitle(";Iterations;chi2/NDF");
      chi2NDF[i] -> SetMarkerColor(colors[i]);
      float minChi2NDF = 9999;
      for(int l = 0; l < nIter; l++)
	{
	  correctionFit[i][l] = new TF1("polyFitBas2",fit.c_str(),fitStart,52);
	  
	  rTilde[i] -> Fit(correctionFit[i][l],"Q0","",5,70);
	  float chi2, ndf;
	  chi2 = correctionFit[i][l] -> GetChisquare();
	  ndf = correctionFit[i][l] -> GetNDF();
	  chi2NDF[i] -> SetPoint(l,l,chi2/ndf);
	  if(chi2/ndf < minChi2NDF)
	    {
	      minChi2NDF = chi2/ndf;
	      bestIter[i] = l;
	    }
	  
	  fit = generateFit(l+1,fit);
	}
    
      correctionFit[i][bestIter[i]] -> SetLineColor(colors[i]);
      fitDiff[i][2] = new TGraphErrors();
      for(int n = 0; n < rTilde[i] -> GetN(); n++)
	{
	  Double_t x,y;
	  rTilde[i] -> GetPoint(n,x,y);
	  if(x < fitEnd)
	    {
	      fitDiff[i][2] -> SetPoint(n, x, (y - correctionFit[i][bestIter[i]] -> Eval(x))/y);
	    }
	}
      
      fitDiff[i][2] -> SetTitle(";p_{T,reco} [GeV/c];#frac{(Data - Fit)}{Data}");
      fitDiff[i][2] -> SetMarkerColor(colors[i]);
      fitDiff[i][2] -> GetYaxis() -> SetRangeUser(-1,1);

      c3 -> cd();
      if(i == 0)rTilde[i] -> Draw("ap");
      else rTilde[i] -> Draw("samep");
      correctionFit[i][bestIter[i]] -> Draw("same");

      c4 -> cd();
      if(i == 0)gfInv[i] -> Draw("ap");
      else gfInv[i] -> Draw("samep");

      bestIterLn[i] = new TLine(bestIter[i],-0.005,bestIter[i],0.03);
      bestIterLn[i] -> SetLineColor(colors[i]);
      bestIterLn[i] -> SetLineStyle(8);
      
      cChi22 -> cd(); 
      if(i == 0)
	{
	  chi2NDF[i] -> GetYaxis() -> SetRangeUser(-0.005,0.03);
	  chi2NDF[i] -> GetXaxis() -> SetLimits(-1,14.5);

	  chi2NDF[i] -> Draw("ap");
	}
      else chi2NDF[i] -> Draw("samep");
      bestIterLn[i] -> Draw("same");

      cDiff -> cd();
      if(i == 0)fitDiff[i][2] -> Draw("ap");
      else fitDiff[i][2] -> Draw("samep");
    }
  c3 -> cd();
  leg -> Draw("same");
  c4 -> cd();
  leg -> Draw("same");
  cDiff -> cd();
  leg -> Draw("same");
  if(save)
    {
      c3 -> SaveAs("plots/jesCorrection.pdf");
      c4 -> SaveAs("plots/jesInv.pdf");
      cChi22 -> SaveAs("plots/chi2NdfCorrFit.pdf");
      cDiff -> SaveAs("plots/fitDiff2.pdf");
      
      TFile *corrOut = new TFile("JES_IsoCorr_NumInv.root","RECREATE");
      corrOut -> cd();
      for(int i = 0; i < nIso; i++)
	{
	  correctionFit[i][bestIter[i]] -> SetName(Form("corrFit_Iso%g",isoParams[i]));
	  correctionFit[i][bestIter[i]] -> Write();
	}
      corrOut -> Close();
    }
}
  
string generateFit(int order, string function)
{
  string nextTerm = Form("+ [%d]*pow(log(x),%d)", order, order);
  
  function += nextTerm;
  
  //TF1 *fit = new TF1("polyLog",function.c_str(),11,60);
  
  return function;
}
