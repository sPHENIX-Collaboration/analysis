#include "sPhenixStyle.h"
#include "sPhenixStyle.C"

void drawCorr()
{
  SetsPhenixStyle();
    
  TCanvas *cJer = new TCanvas();
  TCanvas *cJes = new TCanvas();
  TLegend *leg = new TLegend(.4,.5,.9,.9);
  TLegend *corr = new TLegend(0.35,0.5,0.85,0.75);
  corr -> SetFillStyle(0);
  corr -> SetBorderSize(0);
  leg -> SetFillStyle(0);
  leg -> SetBorderSize(0);
  TLatex tag;
  leg->AddEntry("","#it{#bf{sPHENIX}} Internal","");
  leg->AddEntry("","p+p #sqrt{s_{NN}}=200 GeV","");
  leg->AddEntry("","anti-#it{k}_{#it{t}} #it{R} = 0.4, |#eta| < 1.1","");
  string rad[] = {"0","1","1.5","2","2.5"};
  
  const int nCorrStat = 2;//sizeof(corr)/sizeof(corr[0]);
  const int nRads =  sizeof(rad)/sizeof(rad[0]);
  
  TGraphErrors *jes[nCorrStat][nRads];
  TGraphErrors *jer[nCorrStat][nRads];
  
  TFile *fin[nCorrStat][nRads];
 
  
  int colors[] = {1, 2, 4, kGreen +2, kViolet, kCyan, kOrange, kMagenta+2, kAzure-2};

  for(int i = 0; i < nCorrStat; i++)
    {
      for(int j = 0; j < nRads; j++)
	{
	  fin[i][j] = new TFile(Form("hists_R04_dR%s_Corr%d_isLin0.root",rad[j].c_str(),i));
      
	  jes[i][j] = (TGraphErrors*)fin[i][j] -> Get("g_jes_cent0");
	  jes[i][j] -> SetTitle(";p_{T,truth} [GeV]; #LTp_{T,reco}/p_{T,truth}#GT");
	  
	  jer[i][j] = (TGraphErrors*)fin[i][j] -> Get("g_jer_cent0");
	  jer[i][j] -> SetTitle(";p_{T,truth} [GeV];#sigma(p_{T,reco}/p_{T,truth}) / #LTp_{T,reco}/p_{T,truth}#GT");
      
	  
	  jes[i][j] -> SetMarkerColor(colors[j]);
	  jer[i][j] -> SetMarkerColor(colors[j]);
	  if(i)
	    {
	      jes[i][j] -> SetMarkerStyle(20);
	      jer[i][j] -> SetMarkerStyle(20);
	    }
	  else
	    {
	      jes[i][j] -> SetMarkerStyle(4);
	      jer[i][j] -> SetMarkerStyle(4);
	    }
	  if(i == 0)leg -> AddEntry(jes[i][j],Form("#DeltaR = %sR", rad[j].c_str()),"p");
	  else corr -> AddEntry(jes[i][j]," ","p");
	  cJes -> cd();
	  if(i == 0 && j == 0)
	    {
	      jes[i][j] -> Draw("ap");
	      jes[i][j] -> GetYaxis() -> SetRangeUser(0.5,1.8);
	      jes[i][j] -> GetXaxis() -> SetLimits(0,80);
	    }
	  else jes[i][j] -> Draw("samep");
	  
	  cJer -> cd();
	  if(i == 0 && j == 0) jer[i][j] -> Draw("ap");
	  else jer[i][j] -> Draw("samep");
	}
      
    }
  
  TLine *one = new TLine(0,1,80,1);
  one -> SetLineStyle(8);
  cJes -> cd();
  tag.SetTextSize(0.04);
  tag.DrawLatexNDC(0.39,0.75,"W/  W/o");
  leg -> Draw("same");
  corr -> Draw("same");
  one -> Draw("same");
  cJes -> SaveAs("plots/JES_CorrectionComp.pdf");
  
  cJer -> cd();
  tag.SetTextSize(0.04);
  tag.DrawLatexNDC(0.39,0.75,"W/  W/o");
  leg -> Draw("same");
  corr -> Draw("same");
  cJer -> SaveAs("plots/JER_CorrectionComp.pdf");

  //make one with no iso cut so it's cleaner

  TCanvas *cJesClean = new TCanvas();
  jes[0][0] -> GetYaxis() -> SetRangeUser(0.5,1.80);
  jes[0][0] -> Draw("ap");
  jes[1][0] -> Draw("same p");
  jes[1][0] -> SetMarkerStyle(20);
  jes[0][0] -> SetMarkerStyle(20);

  jes[1][0] -> SetMarkerColor(2);

  one -> Draw("same");
  leg -> Clear();
  leg->AddEntry("","#it{#bf{sPHENIX}} Internal","");
  leg->AddEntry("","p+p #sqrt{s_{NN}}=200 GeV","");
  leg->AddEntry("","anti-#it{k}_{#it{t}} #it{R} = 0.4, |#eta| < 1.1","");
  leg -> AddEntry(jes[0][0],"Uncalibrated","p");
  leg -> AddEntry(jes[1][0],"Calibrated","p");

  leg -> Draw("same");

  
  TCanvas *cJerClean = new TCanvas();
  jer[0][0] -> Draw("ap");
  jer[1][0] -> Draw("same p");
  jer[1][0] -> SetMarkerStyle(20);
  jer[1][0] -> SetMarkerColor(2);

  leg -> Draw("same");
  
  cJerClean -> SaveAs("plots/JER_CorrectionComp_NoIso.pdf");
  cJesClean -> SaveAs("plots/JES_CorrectionComp_NoIso.pdf");
  
  TCanvas *cJesCleanZoom = new TCanvas();
  jes[1][0] -> GetYaxis() -> SetRangeUser(0.98,1.02);
  jes[1][0] -> GetXaxis() -> SetLimits(0,80);
  jes[1][0] -> Draw("aP");
  one -> Draw("same");
  gPad -> SetGridy();
  
  cJesCleanZoom -> SaveAs("plots/JES_CorrectionComp_NoIso_Zoom.pdf");
 

}
      
