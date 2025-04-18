#include "sPhenixStyle.h"
#include "sPhenixStyle.C"
#include "respFitterDef.h"


void drawCorr(int doJesInv = 0)
{
  SetsPhenixStyle();

  vector<float> etaBins;
  for(float i = etaStart; i <= etaEnd; i+=etaGap)
    {
      etaBins.push_back(i);
    }
  const int nEtaBins = etaBins.size() -1 + extraBins;
    
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
  leg->AddEntry("","anti-#it{k}_{#it{t}} #it{R} = 0.4, |#eta_{Jet}| < 0.6","");
  
  const int nCorrStat = 2;//sizeof(corr)/sizeof(corr[0]);
  
  TGraphErrors *jes[nCorrStat][nEtaBins];
  TGraphErrors *jer[nCorrStat][nEtaBins];
  
  TFile *fin[nCorrStat];
 
  
  int colors[] = {1, 2, 4, kGreen +2, kViolet, kCyan, kOrange, kMagenta+2, kAzure-2};

  for(int i = 0; i < nCorrStat; i++)
    {
      if(!doJesInv)fin[i] = new TFile(Form("calibHists_hists_R04_Corr%d_isLin0_2D.root",i));
      else fin[i] = new TFile(Form("calibHists_hists_R04_Corr%d_isLin0_2D.root",i));
      
      for(int j = 0; j < nEtaBins-1; j++)
	{
      
	  jes[i][j] = (TGraphErrors*)fin[i] -> Get(Form("g_jes_eta%d",j));
	  jes[i][j] -> SetTitle(";p_{T,truth} [GeV]; #LTp_{T,reco}/p_{T,truth}#GT");
	  
	  jer[i][j] = (TGraphErrors*)fin[i] -> Get(Form("g_jer_eta%d",j));
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
	 
	  if(i)
	    {
	      if(j <=nEtaBins - 3)leg -> AddEntry(jes[i][j],Form("%g<#eta<%g",etaBins.at(j),etaBins.at(j+1)),"p");
	      else leg -> AddEntry(jes[i][j],"-0.6 < #eta < 0.6","p");
	    }
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
  
  string dirPrefix = "plots";
  if(!doJesInv)dirPrefix=dirPrefix+"_correctionFromLin";
  
  TLine *one = new TLine(0,1,80,1);
  one -> SetLineStyle(8);
  cJes -> cd();
  tag.SetTextSize(0.04);
  tag.DrawLatexNDC(0.39,0.75,"W/  W/o");
  leg -> Draw("same");
  corr -> Draw("same");
  one -> Draw("same");
  cJes -> SaveAs(Form("%s/JES_CorrectionComp.pdf",dirPrefix.c_str()));
  
  cJer -> cd();
  tag.SetTextSize(0.04);
  tag.DrawLatexNDC(0.39,0.75,"W/  W/o");
  leg -> Draw("same");
  corr -> Draw("same");
  cJer -> SaveAs(Form("%s/JER_CorrectionComp.pdf",dirPrefix.c_str()));

  //make one with no iso cut so it's cleaner
  one -> SetX1(5);
  TCanvas *cJesClean = new TCanvas();
  jes[0][4] -> SetMarkerColor(1);
  jes[0][4] -> SetMarkerStyle(20);
  jes[0][4] -> GetYaxis() -> SetRangeUser(0.5,1.80);
  jes[0][4] -> GetXaxis() -> SetRangeUser(0,80);
  jes[1][4] -> GetXaxis() -> SetRangeUser(0,80);
  jes[0][4] -> Draw("ap");
  jes[1][4] -> Draw("same p");
  jes[1][4] -> SetMarkerStyle(20);
  jes[0][4] -> SetMarkerStyle(20);

  jes[1][4] -> SetMarkerColor(2);
  jes[1][4] -> SetMarkerStyle(20);


  one -> Draw("same");
  leg -> Clear();
  leg->AddEntry("","#it{#bf{sPHENIX}} Internal","");
  leg->AddEntry("","p+p #sqrt{s_{NN}}=200 GeV","");
  leg->AddEntry("","anti-#it{k}_{#it{t}} #it{R} = 0.4, |#eta| < 0.6","");
  leg -> AddEntry(jes[0][4],"Uncalibrated","p");
  leg -> AddEntry(jes[1][4],"Calibrated","p");

  leg -> Draw("same");

  cJesClean -> SaveAs(Form("%s/JES_CorrectionComp_EtaIntegrated.pdf",dirPrefix.c_str()));

  TCanvas *cJerClean = new TCanvas();
  jer[0][4] -> SetMarkerColor(1);
  jer[0][4] -> SetMarkerStyle(20);
  jer[0][4] -> Draw("ap");
  jer[1][4] -> Draw("same p");
  jer[1][4] -> SetMarkerStyle(20);
  jer[1][4] -> SetMarkerColor(2);

  leg -> Draw("same");
  
  cJerClean -> SaveAs(Form("%s/JER_CorrectionComp_EtaIntegrated.pdf",dirPrefix.c_str()));
  
  TCanvas *cJesCleanZoom = new TCanvas();
  jes[1][4] -> GetYaxis() -> SetRangeUser(0.98,1.02);
  jes[1][4] -> GetXaxis() -> SetRangeUser(0,80);
  jes[1][4] -> Draw("aP");
  TLine *onep1 = (TLine*)one->Clone();
  onep1 -> SetLineStyle(7);
  onep1 -> SetY1(1.01); onep1->SetY2(1.01);
  onep1 -> Draw("same");
  
  TLine *onep9 = (TLine*)one->Clone();
  onep9 -> SetLineStyle(7);
  onep9 -> SetY1(0.99); onep9 -> SetY2(0.99);
  onep9 -> Draw("same");

  one -> Draw("same");
  //gPad -> SetGridy();
  
  cJesCleanZoom -> SaveAs(Form("%s/JES_CorrectionComp_EtaIntegrated_Zoom.pdf",dirPrefix.c_str()));
 

}
      
