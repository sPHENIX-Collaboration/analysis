#include "sPhenixStyle.C"
#include "sPhenixStyle.h"

TPad* getPad(int j, int k, int l, float cw, float ch, const int  nEBins, const int nCutBins);

//void drawCanvas_invMass(TCanvas *c, TH1F *hInvMass, int pad_x, int pad_y, TPad *pad, int isEta);
void drawCanvas_invMass(TCanvas *c, TH1F *hCorr, int pad_x, int pad_y, TPad *pad, int isEta, float peakPos, float peakW);

void SetHistoStyle(TH1F *histo, int cutBin, int eBin, float low, float hi, int isEta);

void DrawCutLabel(int x, int y, int isEta);

void GetSubtractedDist(TH1F *histOrig, TH1F *histSub, TF1 *invMassFit, TF1 *invMassBG);

//float eBins[] = {1,3,5,7,9,11,13,15,17,20};
float eBins[] = {1,2,3,4,5,6,7,8,9,10,11,12,13};
float eBinsEta[] = {12,14,16,18,20};
float eCuts[] = {0.5,0.6,0.7,0.8,0.9,1.,1.1/*,1.2,1.3,1.4,1.5*/};
float eCutsEta[] = {3,3.5,4,4.5,5,5.5,6};
const int nEtaBins = 5;
void draw_hists_3D(const char *input)
{

  SetsPhenixStyle();
  //TH1::SetDefaultSumw2();
  //TH2::SetDefaultSumw2();
  //TH3::SetDefaultSumw2();
  
  TFile *fin = new TFile(input);
 
  string th1List[] = {"photonE_Reco","isoPhotonE_Reco_Tru","truthPi0E","truthDphoE","pi0ETruthReco"};

  string th1xAxis[] = {"E_{#gamma}","E_{#gamma}^{Iso}","E_{#pi^{0}}^{Truth}","E_{#gamma_{Dir}}^{Truth}","E_{#pi^{0}}^{Reco}/E_{#pi^{0}}^{Truth}"};
  
  string th2List[] = {"clusterChi2","clusterProbPhoton","isoPhotonChi2","isoPhotonProb2","tspE","tspEiso","asymEtruthpi0","deltaREtruthpi0"};
  
  string th2xAxis[] = {"Cluster #xhi^{2}","Cluster Photon Prob","Iso Cluster #chi^{2}","Iso Cluster Prob","TSP","TSP","#frac{|E_{1} - E_{2}|}{E_{#pi^0}}","#Delta R [rad]"};
  
  string th2yAxis[] = {"E_{Cluster}","E_{Cluster}","E_{Cluster}","E_{Cluster}","E_{Cluster}","E_{Cluster}","E_{#pi^{0}}","E_{#pi^{0}}"};
  
  string th3List[] = {"deltaREinvMass","asymEinvMass","invMassEtaE","dphoChi2","dphoProb","pi0Chi2","pi0Prob","etaChi2","etaProb","eChi2","eProb","hChi2","hProb","ePi0InvMassEcut0"};

  string th3xAxis[] = {"#DeltaR","#frac{|E_{1} - E_{2}|}{E_{#pi^0}}","M_{#gamma#gamma}","#chi^{2}","Cluster Photon Prob","#chi^{2}","Cluster Photon Prob","#chi^{2}","Cluster Photon Prob","#chi^{2}","Cluster Photon Prob","#chi^{2}","Cluster Photon Prob","E_{#pi^{0}}^{Reco}"};

  string th3yAxis[] = {"E_{#pi^{0}}^{Reco}","E_{#pi^{0}}^{Reco}","#eta"," E_{Cluster}^{#gamma} [GeV]","E_{Cluster}^{#gamma}[GeV]","E_{Cluster}^{#pi^{0}}[GeV]","E_{Cluster}^{#pi^{0}}[GeV]","E_{Cluster}^{#eta}[GeV]","E_{Cluster}^{#eta}[GeV]","E_{Cluster}^{e^{#pm}}[GeV]","E_{Cluster}^{e^{#pm}}[GeV]","E_{Cluster}^{h^{#pm}}[GeV]","E_{Cluster}^{h^{#pm}}[GeV]","M_{#gamma#gamma}"};

  string th3zAxis[] = {"M_{#gamma#gamma}[GeV/c^{2}]","M_{#gamma#gamma}[GeV/c^{2}]","E^{#gamma}_{Reco}[GeV]","E^{#gamma}_{True}[GeV]","E^{#gamma}_{True}[GeV]","E^{#pi^{0}}_{True}[GeV]","E^{#pi^{0}}_{True}[GeV]","E^{#eta}_{True}[GeV]","E^{#eta}_{True}[GeV]","E^{e^{#pm}}_{True}[GeV]","E_{e^{#pm}}_{True}[GeV]","E_{#pi^{#pm}}_{True}[GeV]","E^{#pi^{#pm}}_{True}[GeV]","E_{Cut}[GeV]"};
 
  
  //loop over and draw the TH1F's
  /*TCanvas *c1 = new TCanvas();
  const int nTh1 = (int)sizeof(th1List)/sizeof(th1List[0]);
  for(int i = 0; i < nTh1; i++)
    {
      TH1F *hist = (TH1F*)fin -> Get(th1List[i].c_str());
      hist -> GetXaxis() -> SetTitle(th1xAxis[i].c_str());
      gPad -> SetLogy();
      hist -> Draw();
      c1 -> SaveAs(Form("plots/%s.pdf",th1List[i].c_str()));
    }
  gPad -> SetLogy(0);
  const int nTh2 = (int)sizeof(th2List)/sizeof(th2List[0]);
  for(int i = 0; i < nTh2; i++)
    {
      gPad -> SetLogz();
      TH2F *hist = (TH2F*)fin -> Get(th2List[i].c_str());
      hist -> GetYaxis() -> SetTitle(th2yAxis[i].c_str());
      hist -> GetXaxis() -> SetTitle(th2xAxis[i].c_str());
      hist -> Draw("colz");
      c1 -> SaveAs(Form("plots/%s.pdf",th2List[i].c_str()));
      }*/
  
  
  const int nTh3 = (int)sizeof(th3List)/sizeof(th3List[0]);
  const int nEBins = (int)sizeof(eBins)/sizeof(eBins[0]);
  const int nCutBins = (int)sizeof(eCuts)/sizeof(eCuts[0]);
  const int nCutBinsEta = (int)sizeof(eCutsEta)/sizeof(eCutsEta[0]);
  const int nEBinsEta = (int)sizeof(eBinsEta)/sizeof(eBinsEta[0]);
  
  TGraphErrors *chi2E[5][2];
  //[5]: dir pho, pi0, eta, electron, pi+/-
  //[2]: vs. truth energy, vs. cluster energy
  TGraphErrors *probE[5][2];

  /*
  for(int i = 0; i < nTh3 - 1; i++)
    {
      gPad -> SetLogy(0);
      TH3F *hist = (TH3F*)fin -> Get(th3List[i].c_str());
      hist -> GetXaxis() -> SetTitle(th3xAxis[i].c_str());
      hist -> GetYaxis() -> SetTitle(th3yAxis[i].c_str());
      hist -> GetZaxis() -> SetTitle(th3zAxis[i].c_str());
      
      TH2F *hist2D = (TH2F*)hist -> Project3D("xy");
      c1 -> cd();
      hist2D -> Draw("colz");
      c1 -> SaveAs(Form("plots/%s_projxy.pdf",th3List[i].c_str()));
      
      TCanvas *cSec = new TCanvas();

      if(i > 2 && i < nTh3 - 1)
	{
	  for(int j = 0; j < nEBins - 1; j++)
	    {
	      TH1F *hist1D = (TH1F*)hist2D->ProjectionY(Form("%s_truthE_%g_%g",th3List[i].c_str(), eBins[j], eBins[j+1]), hist2D -> GetXaxis() -> FindBin(eBins[j]), hist2D -> GetXaxis() -> FindBin(eBins[j+1]));
	      //hist1D -> Fit("gaus",
	      //gPad -> WaitPrimitive();
	      hist1D -> Draw();
	      TLatex energy;
	      gPad -> SetLogy();
	      energy.DrawLatexNDC(0.5,0.7,Form("%g < %s < %g",eBins[j], th3yAxis[i].c_str(), eBins[j+1]));
	      cSec -> SaveAs(Form("plots/%s_ClusterE_%g_%g.pdf",th3List[i].c_str(), eBins[j], eBins[j+1]));
	      
	    }
	}

      hist2D = (TH2F*)hist -> Project3D("xz");
      c1 -> cd();
      hist2D -> Draw("colz");
      c1 -> SaveAs(Form("plots/%s_projxz.pdf",th3List[i].c_str()));
      
       if(i > 2 && i < nTh3 - 1)
	{
	  for(int j = 0; j < nEBins - 1; j++)
	    {
	      TH1F *hist1D = (TH1F*)hist2D->ProjectionY(Form("%s_truthE_%g_%g",th3List[i].c_str(), eBins[j], eBins[j+1]), hist2D -> GetXaxis() -> FindBin(eBins[j]), hist2D -> GetXaxis() -> FindBin(eBins[j+1]));
	      cSec -> cd();
	      hist1D -> Draw();
	      TLatex energy;
	      gPad -> SetLogy();
	      energy.DrawLatexNDC(0.5,0.7,Form("%g < %s < %g",eBins[j], th3zAxis[i].c_str(), eBins[j+1]));
	      //hist1D -> Fit("gaus",
	      //gPad -> WaitPrimitive();
	      cSec -> SaveAs(Form("plots/%s_TruthE_%g_%g.pdf",th3List[i].c_str(), eBins[j], eBins[j+1]));
	      
	    }
	}
      
	}*/
  
  //going to handle the last TH3 separately since
  //it's going on a stupid big canvas. 
  
  TH3F *hist[nEtaBins]; 
  TH3F *histMatched[nEtaBins];
  float cw = 2*966, ch = 1.3*637;
  
  TCanvas *cPi0Cut[nEtaBins];
  TPad *pad_invMass[nEBins-1][nCutBins][nEtaBins];

  TCanvas *cSubMass[nEtaBins];
  TPad *pad_invMassSub[nEBins-1][nCutBins][nEtaBins];
  
  TCanvas *cMatchMass[nEtaBins];
  TPad *pad_matchMass[nEBins-1][nCutBins][nEtaBins];
  

  TGraphErrors *mass[nCutBins][nEtaBins];
  TGraphErrors *width[nCutBins][nEtaBins];
  TGraphErrors *yields[nCutBins][nEtaBins];
  TGraphErrors *yieldsMatched[nCutBins][nEtaBins];
  TGraphErrors *matchZeroFrac[nCutBins][nEtaBins];
  TF1 *invMassFit, *invMassFitBG;
  int colors[] = {1,2,4,kGreen+2, kViolet,kCyan,kOrange+2,kMagenta+2,kAzure-2};
  TLegend *leggy = new TLegend(.161, .729, .722, .939 );
  leggy -> SetNColumns(3);
  leggy -> SetFillStyle(0);
  leggy -> SetBorderSize(0);
  
  float xEndFit = 0.25;
  float xStartFit = 0.07;
  float xEndHist = 0.25;
  float xStartHist = 0.07;
  float invMassWindow[2] = {0.112, 0.162};
  float peakPos[nEBins - 1][nCutBins][nEtaBins];
  float peakW[nEBins - 1][nCutBins][nEtaBins];
  float sigma = 3.;
  TLatex marks;
  
  for(int k = 0; k < nEtaBins; k++)
    {
      cMatchMass[k] = new TCanvas(Form("cMatchMass_Eta%d",k),"Mass from Matched Cluster",cw,ch);
      cMatchMass[k] -> Range(0,0,1,1);

      cSubMass[k]= new TCanvas(Form("cMassSubPi0_Eta%d",k),"Subtracted Mass Dist",cw,ch);
      cSubMass[k] -> Range(0,0,1,1);

      cPi0Cut[k] = new TCanvas(Form("cMassPi0_Eta%d",k),"Invariant Mass",cw,ch);
      cPi0Cut[k] -> Range(0,0,1,1);
	    
      hist[k] = (TH3F*)fin -> Get(Form("ePi0InvMassEcut_0Match_Eta%d",k));
      histMatched[k] = (TH3F*)fin -> Get(Form("ePi0InvMassEcut_1Match_Eta%d",k));
      
      for(int i = 0; i < nEBins - 1; i++)
	{
	  for(int j = 0; j < nCutBins; j++)
	    {
	      if(i == 0)
		{
		  mass[j][k] = new TGraphErrors();
		  mass[j][k] -> SetMarkerColor(colors[j]);

		  width[j][k] = new TGraphErrors();
		  width[j][k] -> SetMarkerColor(colors[j]);

		  yields[j][k] = new TGraphErrors();
		  yields[j][k] -> SetMarkerColor(colors[j]);

		  yieldsMatched[j][k] = new TGraphErrors();
		  yieldsMatched[j][k] -> SetMarkerColor(colors[j]);

		  matchZeroFrac[j][k] = new TGraphErrors();
		  matchZeroFrac[j][k] -> SetMarkerColor(colors[j]);
		}
	 
	      pad_invMass[i][j][k] = getPad(i,j,k,cw,ch,nEBins , nCutBins);//for unsubtracted invariant mass distributions
	      pad_invMass[i][j][k] -> cd();

	      pad_invMassSub[i][j][k] = getPad(i,j,k,cw,ch,nEBins , nCutBins);//for subtracted mass distributions

	      pad_matchMass[i][j][k] = getPad(i,j,k,cw,ch,nEBins, nCutBins);//for distributions composed of clusters matched to truth decay photons

	      TH1F *hist1D = (TH1F*)hist[k] -> ProjectionY(Form("invMass_E%d_Cut%d_Eta%d",i,j,k), hist[k] -> GetXaxis() -> FindBin(eBins[i] + 0.001), hist[k] -> GetXaxis() -> FindBin(eBins[i+1] - 0.0001), hist[k] -> GetZaxis() -> FindBin(eCuts[j] + 0.0001), hist[k] -> GetNbinsZ());
	  
	      TH1F *hist1DMatched = (TH1F*)histMatched[k] -> ProjectionY(Form("invMass_E%d_Cut%d_eta%d_Matched",i,j,k), histMatched[k] -> GetXaxis() -> FindBin(eBins[i] + 0.001), histMatched[k] -> GetXaxis() -> FindBin(eBins[i+1] - 0.0001), histMatched[k] -> GetZaxis() -> FindBin(eCuts[j] + 0.0001), histMatched[k] -> GetNbinsZ());
	  
 	  
	      TH1F *invMassSub = (TH1F*)hist1D -> Clone();
	      if(eBins[i+1] <= 13) 
		{
		  invMassFit = new TF1(Form("invMassFit_E%d_Cut%d_Eta%d",i,j,k),"[0] + [1]*x + [2]*pow(x,2)+ [3]*pow(x,3)+  [4]*pow(x,4)+ [8]*pow(x,6)  +[5]*exp(-(([7]-x)^2)/(2*[6]^2))");
	      
	      
		  invMassFitBG = new TF1(Form("invMassFit_E%d_Cut%d_Eta%dBG",i,j,k),"[0] + [1]*x + [2]*pow(x,2)+ [3]*pow(x,3)+  [4]*pow(x,4)+ [8]*pow(x,6)  +[5]*exp(-(([7]-x)^2)/(2*[6]^2)) - [5]*exp(-(([7]-x)^2)/(2*[6]^2))"); //hehe
	      
		  invMassFit -> FixParameter(8,0);
	      
		  invMassFit -> SetLineWidth(1);
		  invMassFit -> SetParameter(6,0.015);
		  
		  invMassFit -> SetLineColor(2);
		  invMassFit -> SetParameter(7,0.135);
		  invMassFit -> SetParLimits(7, 0.135 - 0.07, 0.135 + 0.07);
		  if((eCuts[j] >= 0.9) && (eBins[i] == 3 || eBins[i] == 4)) 
		    {
		      invMassFit -> SetParameter(5, hist1D -> GetMaximum()*2);
		  
		    }
		  else invMassFit -> SetParameter(5,hist1D -> GetMaximum());
		  hist1D -> Fit(invMassFit,"Q","",xStartFit, xEndFit);
		  GetSubtractedDist(hist1D, invMassSub, invMassFit, invMassFitBG);
		  invMassFitBG -> SetLineColor(4);
	     
		  float peakPosition = invMassFit -> GetParameter(7);
		  peakPos[i][j][k] = peakPosition;
		  
		  float peakWidth = invMassFit -> GetParameter(6);
		  peakW[i][j][k] = peakWidth;
		  Double_t yieldErr, yieldErrMatched;
	  
		  float yield;
		  if(hist1D -> Integral() == 0) yield = 0;
		  else yield = invMassSub -> IntegralAndError(invMassSub -> GetXaxis() -> FindBin((peakPosition - sigma*peakWidth)+0.0001), invMassSub -> GetXaxis() -> FindBin((peakPosition + sigma*peakWidth)+0.0001), yieldErr, "");

		  float yieldMatched = hist1DMatched -> IntegralAndError(hist1DMatched -> FindFirstBinAbove(0), hist1DMatched -> GetNbinsX(), yieldErrMatched, "");
		  if(yield != 0)
		    {
		      mass[j][k] -> SetPoint(i, (eBins[i]+eBins[i+1])/2, invMassFit -> GetParameter(7));
		      mass[j][k] -> SetPointError(i,  0, invMassFit -> GetParError(7));
	
		      width[j][k] -> SetPoint(i, (eBins[i]+eBins[i+1])/2, invMassFit -> GetParameter(6));
		      width[j][k] -> SetPointError(i, 0, invMassFit -> GetParError(6));
	
		      yields[j][k] -> SetPoint(i, (eBins[i]+eBins[i+1])/2, yield);
		      yields[j][k] -> SetPointError(i, 0, yieldErr);
		      
		     
		    }
		  yieldsMatched[j][k] -> SetPoint(i, (eBins[i]+eBins[i+1])/2, yieldMatched);
		   
		  yieldsMatched[j][k] -> SetPointError(i, 0, yieldErrMatched);
		      
		  float zeroMatchYield = hist1DMatched -> GetBinContent(hist1DMatched -> FindFirstBinAbove(0));
		  float zeroMatchYieldErr = hist1DMatched -> GetBinError(46);
		  float matchYieldRatio = zeroMatchYield/yieldMatched;
		  float zeroMatchYieldRatErr = matchYieldRatio*sqrt(pow(yieldErrMatched/yieldMatched,2) + pow(zeroMatchYieldErr/zeroMatchYieldErr,2));
		    
		  matchZeroFrac[j][k] -> SetPoint(i, (eBins[i]+eBins[i+1])/2, matchYieldRatio);
		  matchZeroFrac[j][k] -> SetPointError(i, 0, zeroMatchYieldRatErr);
		}
	      hist1D -> GetXaxis() -> SetRangeUser(xStartHist,xEndHist);
	  
	      TCanvas *cMassFitCheck = new TCanvas();
	      hist1D -> SetTitle(";M_{#gamma#gamma} [GeV/c^{2}];");
	      hist1D -> Draw("epx0");
	      invMassFitBG -> SetLineWidth(1);

	      invMassFitBG -> Draw("same");

	      marks.DrawLatexNDC(0.5,0.3,Form("#splitline{E_{Min} > %g GeV}{%g > E_{#pi^{0}} > %g}",eCuts[j], eBins[i], eBins[i+1]));
	      cMassFitCheck -> SaveAs(Form("plots/invMassFitCheck_%dE_%dCut_Sigma%g_Eta%d.pdf",i,j,sigma,k));


	      TCanvas *cMassMatchCheck = new TCanvas();
	      hist1DMatched -> SetTitle(";M_{#gamma#gamma} [GeV/c^{2}];");
	      hist1DMatched -> Draw("epx0");
	      gPad -> SetLogy();
	      marks.DrawLatexNDC(0.5,0.85,Form("#splitline{E_{Min} > %g GeV}{%g > E_{#pi^{0}} > %g}",eCuts[j], eBins[i], eBins[i+1]));
	      cMassMatchCheck -> SaveAs(Form("plots/invMassMatchCheck_%dE_%dCut_Sigma%g_Eta%dMatch.pdf",i,j,sigma,k));

	      drawCanvas_invMass(cPi0Cut[k], hist1D, j, i, pad_invMass[i][j][k], 0, peakPos[i][j][k], sigma*peakW[i][j][k]);
	      invMassFitBG -> Draw("same");
	      pad_invMassSub[i][j][k] -> cd();

	      invMassSub -> GetXaxis() -> SetRangeUser(xStartHist, xEndHist);
	      drawCanvas_invMass(cSubMass[k], invMassSub, j, i, pad_invMassSub[i][j][k], 0, peakPos[i][j][k], sigma*peakW[i][j][k]);

	      pad_matchMass[i][j][k] -> cd();
	      drawCanvas_invMass(cMatchMass[k], hist1DMatched, j, i, pad_matchMass[i][j][k], 1, peakPos[i][j][k], sigma*peakW[i][j][k]);
	  
	    }
	}
    
      cPi0Cut[k] -> SaveAs(Form("plots/invMassCuts_Sigma%g_Eta%d.pdf",sigma, k));
      cSubMass[k] -> SaveAs(Form("plots/invMassCutsSubtracted_Sigma%g_Eta%d.pdf",sigma, k));
      cMatchMass[k] -> SaveAs(Form("plots/invMassMatched_Sigma%g_Eta%d.pdf",sigma,k));
    }
  
  
  TCanvas *cMass[nEtaBins];
  for(int k = 0; k < nEtaBins; k++)
    {
      cMass[k] = new TCanvas();
      for(int i = 0; i < nCutBins; i++)
	{
	  if(i == 0)
	    {
	      mass[i][k] -> Draw("ap");
	      mass[i][k] -> GetYaxis() -> SetRangeUser(0.130,0.180);
	      mass[i][k] -> GetXaxis() -> SetLimits(0,13);
	      mass[i][k] -> GetXaxis() -> SetTitle("E_{#pi^{0}}^{Reco} [GeV]");
	      mass[i][k] -> GetYaxis() -> SetTitle("Mass Peak Location [GeV/c^{2}]");
	    }
	  else mass[i][k] -> Draw("samep");
	  if(k==0)leggy -> AddEntry(mass[i][k], Form("E_{min} > %g",eCuts[i]),"p");
	}
 
      leggy -> Draw("same");
      cMass[k] -> SaveAs(Form("plots/invMassGraphPeakPos_Sigma%g_Eta%d.pdf",sigma,k));
    }

  TCanvas *cWidth[nEtaBins];
  for(int k = 0; k < nEtaBins; k++)
    {
      cWidth[k] = new TCanvas();

      for(int i = 0; i < nCutBins; i++)
	{
	  if(i == 0)
	    {
	      width[i][k] -> Draw("ap");
	      width[i][k] -> GetYaxis() -> SetRangeUser(0.013, 0.028);
	      width[i][k] -> GetXaxis() -> SetLimits(0,13);
	      width[i][k] -> GetXaxis() -> SetTitle("E_{#pi^{0}}^{Reco} [GeV]");
	      width[i][k] -> GetYaxis() -> SetTitle("Mass Peak Width [GeV/c^{2}]");
	    }
	  else width[i][k] -> Draw("samep");
	}
 
      leggy -> Draw("same");
      cWidth[k] -> SaveAs(Form("plots/invMassGraphPeakWidth_Sigma%g_Eta%d.pdf",sigma,k));
    }

  TCanvas *cYields[nEtaBins];
  for(int k = 0; k < nEtaBins; k++)
    {
      cYields[k] = new TCanvas();
      for(int i = 0; i < nCutBins; i++)
	{
	  if(i == 0)
	    {
	      yields[i][k] -> Draw("ap");
	      yields[i][k] -> GetYaxis() -> SetRangeUser(1e3,1e7); 
	      yields[i][k] -> GetXaxis() -> SetTitle("E_{#pi^{0}}^{Reco} [GeV]");
	      yields[i][k] -> GetYaxis() -> SetTitle("N_{#pi^{0}}");
	    }
	  else yields[i][k] -> Draw("samep");
	  gPad -> SetLogy();
	}
  
      leggy -> Draw("same");
      cYields[k] -> SaveAs(Form("plots/invMassYield_Sigma%g_Eta%d.pdf",sigma,k));
    }

  TCanvas *cYieldsMatch[nEtaBins];
  for(int k = 0; k < nEtaBins; k++)
    {
      cYieldsMatch[k] = new TCanvas();
      for(int i = 0; i < nCutBins; i++)
	{
	  if(i == 0)
	    {
	      yieldsMatched[i][k] -> Draw("ap");
	      yieldsMatched[i][k] -> GetYaxis() -> SetRangeUser(1e3,1e7); 
	      yieldsMatched[i][k] -> GetXaxis() -> SetTitle("E_{#pi^{0}}^{Reco} [GeV]");
	      yieldsMatched[i][k] -> GetYaxis() -> SetTitle("N_{#pi^{0}}");
	    }
	  else yields[i][k] -> Draw("samep");
	  gPad -> SetLogy();
	}
  
      leggy -> Draw("same");
      cYields[k] -> SaveAs(Form("plots/invMassYield_Sigma%g_Eta%d_Matched.pdf",sigma,k));
    }
  TH1F *pi0TruSpec[nEtaBins];
  //for(int i = 0; i < nEtaBins; i++) pi0TruSpec[i] = (TH1F*)fin -> Get(th1List[2].c_str());
  for(int i = 0; i < nEtaBins; i++) pi0TruSpec[i] = (TH1F*)fin -> Get(Form("truthPi0E_Eta%d",i));
  TGraphErrors *pi0Eff[nCutBins][nEtaBins];
  TCanvas *cEff[nEtaBins];
  float scaleMax;
  float scaleMin;
  
  for(int k = 0; k < nEtaBins; k++)
    {
      cEff[k] = new TCanvas();
      scaleMax = -9999;
      scaleMin = 9999;
      for(int i = 0; i < nCutBins ; i++)
	{
	  pi0Eff[i][k] = new TGraphErrors();
	  for(int j = 0; j < nEBins; j++)
	    {
	      Double_t pi0TruthYieldErr;
	      float pi0TruthYield = pi0TruSpec[k] -> IntegralAndError(pi0TruSpec[k] -> GetXaxis() -> FindBin(eBins[j]+0.001), pi0TruSpec[k] -> GetXaxis() -> FindBin(eBins[j+1]-0.001), pi0TruthYieldErr, "");
	  
	  
	      Double_t x, pi0YieldMeas, pi0YieldErr;
	      yields[i][k] -> GetPoint(j, x, pi0YieldMeas);
	      pi0YieldErr = yields[i][k] -> GetErrorY(j);
	      float eff = pi0YieldMeas/pi0TruthYield;
	      float err = eff*sqrt(pow(pi0YieldErr/pi0YieldMeas,2) + pow(pi0TruthYieldErr/pi0TruthYield,2));
	      if(eff <  scaleMin) scaleMin = eff;
	      if(eff > scaleMax) scaleMax = eff;
	      pi0Eff[i][k] -> SetPoint(j, (eBins[j]+eBins[j+1])/2, eff);
	      pi0Eff[i][k] -> SetPointError(j, 0, err);
	    }
	  pi0Eff[i][k] -> SetMarkerColor(colors[i]);
	  if(i == 0)
	    {
	      pi0Eff[i][k] -> Draw("ap");
	      pi0Eff[i][k] -> GetYaxis() -> SetRangeUser(-0.01,scaleMax*1.4);
	      pi0Eff[i][k] -> GetXaxis() -> SetTitle("E_{#pi^{0}}^{Reco} [GeV]");
	      pi0Eff[i][k] -> GetYaxis() -> SetTitle("#epsilon");
	    }
	  else pi0Eff[i][k] -> Draw("samep");
      
	}
      leggy -> Draw("same");
      cEff[k] -> SaveAs(Form("plots/pi0Eff_Sigma%g_Eta%d.pdf",sigma,k));
    }

  TGraphErrors *pi0EffMatch[nCutBins][nEtaBins];
  TCanvas *cEffMatch[nEtaBins];
  
  for(int k = 0; k < nEtaBins; k++)
    {
      scaleMax = -9999;
      scaleMin = 9999;
      cEffMatch[k] = new TCanvas();
      for(int i = 0; i < nCutBins ; i++)
	{
	  pi0EffMatch[i][k] = new TGraphErrors();
	  for(int j = 0; j < nEBins - 1; j++)
	    {
	      Double_t pi0TruthYieldErr;
	      float pi0TruthYield = pi0TruSpec[k] -> IntegralAndError(pi0TruSpec[k] -> GetXaxis() -> FindBin(eBins[j]+0.001), pi0TruSpec [k]-> GetXaxis() -> FindBin(eBins[j+1]-0.001), pi0TruthYieldErr, "");
	  
	      Double_t x, pi0YieldMeas, pi0YieldErr;
	      yieldsMatched[i][k] -> GetPoint(j, x, pi0YieldMeas);
	      pi0YieldErr = yieldsMatched[i][k] -> GetErrorY(j);
	      float eff = pi0YieldMeas/pi0TruthYield;
	      float err = eff*sqrt(pow(pi0YieldErr/pi0YieldMeas,2) + pow(pi0TruthYieldErr/pi0TruthYield,2));
	      if(eff <  scaleMin) scaleMin = eff;
	      if(eff > scaleMax) scaleMax = eff;
	      pi0EffMatch[i][k] -> SetPoint(j, (eBins[j]+eBins[j+1])/2, eff);
	      pi0EffMatch[i][k] -> SetPointError(j, 0, err);
	    }
	  pi0EffMatch[i][k] -> SetMarkerColor(colors[i]);
	  if(i == 0)
	    {
	      pi0EffMatch[i][k] -> Draw("ap");
	      pi0EffMatch[i][k] -> GetYaxis() -> SetRangeUser(-0.01,scaleMax*1.25);
	      pi0EffMatch[i][k] -> GetXaxis() -> SetTitle("E_{#pi^{0}}^{Reco} [GeV]");
	      pi0EffMatch[i][k] -> GetYaxis() -> SetTitle("#epsilon");
	    }
	  else pi0EffMatch[i][k] -> Draw("samep");
      
	}
      leggy -> Draw("same");
      cEffMatch[k] -> SaveAs(Form("plots/pi0Eff_Sigma%g_Eta%d_Matched.pdf",sigma,k));
    }

   TCanvas *cZeroFrac[nEtaBins];
  for(int k = 0; k < nEtaBins; k++)
    {
      cZeroFrac[k] = new TCanvas();
      for(int j = 0; j < nCutBins; j++)
	{
	  if(j == 0)
	    {
	      matchZeroFrac[j][k] -> Draw("ap");
	      //matchZeroFrac[j][k] -> GetYaxis() -> SetRangeUser(-0.1,1.1);
	      matchZeroFrac[j][k] -> SetTitle(";E_{#pi^{0}}^{Truth} [GeV]; N^{Unmatched}/N^{Matched}");
	    }
	  else  matchZeroFrac[j][k] -> Draw("samep");
	}
      leggy -> Draw("same");
      cZeroFrac[k] -> SaveAs(Form("plots/matchedZeroYieldFrac_Sigma%g_Eta%d.pdf",sigma,k));
    }
	      

      
  TCanvas *cMiss = new TCanvas();
  cMiss -> Divide(4);
  cMiss -> cd();
  
  TH3F *unmatchedLocale = (TH3F*)fin -> Get("unmatchedLocale");
 
  float eBinsMerge[] = {1,4,5,9,13};
  
  for(int i = 0; i < 4; i++)
    {
      unmatchedLocale -> GetZaxis() -> SetRangeUser(eBinsMerge[i], eBinsMerge[i+1]);
      TH2F *locationProj = (TH2F*)unmatchedLocale -> Project3D("yx");
      locationProj -> SetTitle(Form("%g < E^{#pi^{0}}_{Truth} < %g;#eta;#phi [rad]", eBinsMerge[i], eBinsMerge[i+1]));
      locationProj -> Draw("colz");
      marks.DrawLatexNDC(0.2,0.5,Form("%g < E^{#pi^{0}}_{Truth} < %g", eBinsMerge[i], eBinsMerge[i+1]));

      cMiss -> SaveAs(Form("plots/missLocationBin_%d.pdf",i));
    }

  TCanvas *cMassScale = new TCanvas();
  TH3F *invMassPhoPi0 = (TH3F*)fin -> Get("invMassPhoPi0");
  
  float massZones[] = {0,0.100,0.200,1};
  
  for(int i = 0; i < 3; i++)
    {
      TH1F *pi0EScale = (TH1F*)invMassPhoPi0 -> ProjectionZ(Form("pi0EScale_%d",i),invMassPhoPi0 -> GetXaxis() -> FindBin(massZones[i] + 0.0001), invMassPhoPi0 -> GetXaxis() -> FindBin(massZones[i + 1] - 0.0001), 1, invMassPhoPi0 -> GetNbinsY());
      pi0EScale -> SetTitle(Form("%g < M_{#gamma#gamma} < %g; E_{Reco}^{#pi^{0}}/E_{True}^{#pi^{0}};",massZones[i], massZones[i+1]));
      pi0EScale -> Draw();
      marks.DrawLatexNDC(0.7,0.85,Form("%g < M_{#gamma#gamma} < %g",massZones[i], massZones[i+1]));
      cMassScale -> SaveAs(Form("plots/pi0EScale_MassBin_%d.pdf",i));
      
      
      TH1F *phoEScale = (TH1F*)invMassPhoPi0 -> ProjectionY(Form("phoEScale_%d",i),invMassPhoPi0 -> GetXaxis() -> FindBin(massZones[i] + 0.0001), invMassPhoPi0 -> GetXaxis() -> FindBin(massZones[i + 1] - 0.0001), 1, invMassPhoPi0 -> GetNbinsZ());
      phoEScale -> SetTitle(Form("%g < M_{#gamma#gamma} < %g; E_{Reco}^{#gamma}/E_{True}^{#gamma};",massZones[i], massZones[i+1]));
      phoEScale -> Draw();
      marks.DrawLatexNDC(0.7,0.85,Form("%g < M_{#gamma#gamma} < %g",massZones[i], massZones[i+1]));

      cMassScale -> SaveAs(Form("plots/phoEScale_MassBin_%d.pdf",i));
    }
  
  TCanvas *cMassPhi = new TCanvas();
  TH3F *invMassEPhi = (TH3F*)fin -> Get("invMassEPhi");
  
  for(int i = 0; i < 4; i++)
    {
      invMassEPhi -> GetYaxis() -> SetRangeUser(eBinsMerge[i],eBinsMerge[i+1]); 
      TH2F *invMassPhi = (TH2F*)invMassEPhi -> Project3D("xz");
      
      invMassPhi -> SetTitle(Form("%g <  E^{#pi^{0}}_{Truth} < %g;  #phi [rad]; M_{#gamma#gamma};",eBinsMerge[i], eBinsMerge[i+1] ));
      invMassPhi -> Draw("colz");
      marks.DrawLatexNDC(0.2,0.18,Form("%g <  E^{#pi^{0}}_{Truth} < %g",eBinsMerge[i], eBinsMerge[i+1] ));

      cMassPhi -> SaveAs(Form("plots/invMassPhi_EBin_%d.pdf",i));
      }
  
 
      
  
  /*
  TCanvas *cEtaCut = new TCanvas("cMassEta","Invariant Mass Eta Cuts",cw,ch);
  cEtaCut -> Range(0,0,1,1);
  TPad *pad_invMassEta[nEBinsEta - 1][nCutBinsEta];
    
  for(int i = 0; i < nEBinsEta - 1; i++)
    {
      for(int j = 0; j < nCutBinsEta; j++)
	{
	  pad_invMassEta[i][j] = getPad(i,j,cw,ch,nEBinsEta , nCutBinsEta);
	  pad_invMassEta[i][j] -> cd();
	  TH1F *hist1D = (TH1F*)hist -> ProjectionY(Form("invMass_EtaE%d_Cut%d",i,j), hist -> GetXaxis() -> FindBin(eBinsEta[i] + 0.001), hist -> GetXaxis() -> FindBin(eBinsEta[i+1] - 0.0001), hist -> GetZaxis() -> FindBin(eCutsEta[j] + 0.0001), hist -> GetNbinsZ());
	  drawCanvas_invMass(cEtaCut, hist1D, j, i, pad_invMassEta[i][j], 2);
	}
    }
  
    cEtaCut -> SaveAs("plots/invMassCutsEta.pdf");*/
		   
} 
  
TPad* getPad(int j, int k, int l, float cw, float ch, const int nEBins, const int nCutBins)
{
  float xpos[nCutBins+1], ypos[nEBins];
  int nCutOffset = 1;
  float marginLeft = 0.07;
  float marginRight = 0.035;

  float marginTop = 0.03;
  float marginBottom = 0.06;
  
  float marginColumn = 0.04;
  
  float w = (1 - marginLeft - marginRight - marginColumn*(nCutBins-2 + nCutOffset))/(nCutBins - 1+ nCutOffset);
  float h = (1 - marginTop - marginBottom)/(nEBins - 1);

  for(int m = 0; m < nCutBins + nCutOffset; m++)
    {
      if(m == 0) xpos[m] = 0;
      else if(m == nCutBins - 1+ nCutOffset) xpos[m] = 1.;
      else xpos[m] = marginLeft + m*w + (m-1)*marginColumn;
    }
  
  for(int m = 0; m < nEBins; m++)
    {
      if(m==0) ypos[nEBins -1 -m] = 0;
      else if (m == nEBins - 1) ypos[nEBins-1-m] = 1.;
      else ypos[nEBins - 1 -m] = marginBottom +m*h;
    }
  
  TPad *myPad = new TPad(Form("pad%d%d%d",j,k,l),Form("pad%d%d%d",j,k,l),xpos[k],ypos[j+1],xpos[k+1],ypos[j]);
  
  if(k==0) myPad->SetLeftMargin(marginLeft/(xpos[k+1] - xpos[k]));
  else myPad -> SetLeftMargin(marginColumn/(xpos[k+1] - xpos[k]));
  
  if(k == nCutBins - 2+ nCutOffset) myPad -> SetRightMargin(marginRight/(xpos[k+1]-xpos[k]));
  else myPad -> SetRightMargin(0.012);
  
  if(j == 0) myPad -> SetTopMargin(marginTop/(ypos[j] - ypos[j+1]));
  else myPad -> SetTopMargin(0);
  
  if(j == nEBins - 2) myPad -> SetBottomMargin(marginBottom/(ypos[j] - ypos[j+1]));
  else myPad -> SetBottomMargin(0);
  
  return myPad;
}
  
void drawCanvas_invMass(TCanvas *c, TH1F *hCorr, int pad_x, int pad_y, TPad *pad, int mode, float peakPos, float peakW)
{
  
  float range = hCorr -> GetMaximum() - hCorr -> GetMinimum();
  SetHistoStyle(hCorr, pad_x, pad_y, hCorr -> GetMinimum(), hCorr->GetMaximum() + 0.2*range, mode);
  c -> cd(0);
  /*TLine *pi0Mass = new TLine(0.135,0,0.135,hCorr->GetMaximum());
  pi0Mass -> SetLineStyle(7);
  pi0Mass -> SetLineColor(2);
  TLine *etaMass = new TLine(0.548,0,0.548,hCorr->GetMaximum());
  etaMass -> SetLineStyle(7);
  etaMass -> SetLineColor(2);*/
  TLine *massWindow[2];
  //massWindow[0] = new TLine(0.112,0,0.112,hCorr->GetMaximum());
  pad -> Draw();
  pad -> cd();
  
  hCorr -> Draw("epX0");
  DrawCutLabel(pad_x,pad_y,mode);
  if(mode == 1) gPad -> SetLogy();
  if(peakPos)
    {
      massWindow[0] = new TLine(peakPos - peakW,0,peakPos - peakW,hCorr->GetMaximum());
      massWindow[0] -> SetLineStyle(7);
      massWindow[0] -> SetLineColor(2);
  
      //massWindow[1] = new TLine(0.162,0,0.162,hCorr->GetMaximum());
      massWindow[1] = new TLine(peakPos + peakW,0,peakPos + peakW,hCorr->GetMaximum());
      massWindow[1] -> SetLineStyle(7);
      massWindow[1] -> SetLineColor(2);
      massWindow[0] -> Draw("same");
      massWindow[1] -> Draw("same");
    }
  
  
  //etaMass -> Draw("same");
  //pi0Mass -> Draw("same");
  
}

void SetHistoStyle(TH1F *histo, int cutBin, int eBin, float low, float hi, int mode)
{
  if(hi > low) histo -> GetYaxis() -> SetRangeUser(low,hi);
  if(mode == 1)
    {
      histo -> GetYaxis() -> SetRangeUser(0.1,10*hi);
    }
  
  histo -> SetTitle(";;");
  int xTitleBin[3][2] = {{11,3},{11,3},{3,3}};
  int yTitleBin[3][2] = {{5,0},{5,0},{2,0}};

  float yTitleOffset[3] = {0.3, 0.4, 1};
  float xTitleOffset[3] = {0.7, 0.7, 0.7};

  float yTitleSize[3] = {0.5, 0.5, 0.15};
  float xTitleSize[3] = {0.28, 0.28, 0.13}; 

  if(eBin == xTitleBin[mode][0] && cutBin == xTitleBin[mode][1]) histo -> GetXaxis() -> SetTitle("M_{#gamma#gamma}");
  if(eBin == yTitleBin[mode][0] && cutBin == yTitleBin[mode][1]) histo -> GetYaxis() -> SetTitle("N_{#gamma#gamma}");
  
  histo -> GetXaxis() -> SetTitleSize(xTitleSize[mode]);
  histo -> GetXaxis() -> SetTitleOffset(xTitleOffset[mode]);
  histo -> GetXaxis() -> CenterTitle();
  
  histo -> GetXaxis() -> SetLabelFont(43);
  histo -> GetXaxis() -> SetLabelSize(18);
  histo -> GetXaxis() -> SetLabelOffset(0.0);
  histo -> SetTickLength(0.05,"X");
  histo -> SetNdivisions(505,"x");

  
  histo -> GetYaxis() -> SetNdivisions(505);
  histo -> GetYaxis() -> SetTitleSize(yTitleSize[mode]);
  histo -> GetYaxis() -> SetTitleOffset(yTitleOffset[mode]);
  histo -> GetYaxis() -> CenterTitle();
  
  histo -> GetYaxis() -> SetLabelFont(43);
  histo -> GetYaxis() -> SetLabelSize(19);
  histo -> GetYaxis() -> SetLabelOffset(0.003);
  histo -> GetYaxis() -> SetNoExponent();
  histo -> SetTickLength(0.04,"Y");
  
  histo -> SetStats(0);
  //histo -> SetLineWidth(1.4);
  histo -> SetLineColor(1);
  histo -> SetMarkerColor(1);
  histo -> SetMarkerStyle(20);
  histo -> SetMarkerSize(0.4);
}

void DrawCutLabel(int x, int y, int mode)
{
  gPad -> cd();
  gPad -> Update();
  float Y2 = gPad -> GetUymax();
  float Y1 = gPad -> GetUymin();
  float rangeY = Y2 - Y1;
  float xtop[3] = {0.8, -0.1, 0};
  float xbottom[3] = {0.273152, 1.01, 1.1};
  double ytop[3] = {Y2+0.01*rangeY, Y2+1*rangeY, Y2+0.01*rangeY};
  double ybottom[3] = {  0.98*Y2,  0.98*Y2,  0.9*Y2};
  
  TText *textCut;
  if(y == 0)
    {
      // if(mode < 2)
      // 	{
      // 	  if(x <= 4)textCut = new TText(0.08, Y2+0.01*rangeY, Form("Min E > %g GeV", eCuts[x]));
      // 	  else textCut = new TText(0.08, Y2+0.01*rangeY, Form("Min E > %g GeV", eCuts[x]));
      // 	}
      //else textCut = new TText(0, Y2+0.01*rangeY, Form("Min E > %g GeV", eCutsEta[x]));
      textCut = new TText(xtop[mode], ytop[mode], Form("Min E > %g GeV", eCuts[x]));
      textCut -> SetTextFont(43);
      textCut -> SetTextSize(18);
      textCut -> Draw();
    }
  if(x == sizeof(eCuts)/sizeof(eCuts[0]) - 1)
    {
      //if(mode < 2)textCut = new TText(0.273152, 0.98*Y2, Form("%g-%gGeV",eBins[y], eBins[y+1]));
      //else textCut = new TText(1.1, 0.9*Y2, Form("%g-%gGeV",eBinsEta[y], eBinsEta[y+1]));
      if(mode < 2)textCut = new TText(xbottom[mode], ybottom[mode], Form("%g-%gGeV",eBins[y], eBins[y+1]));
      else textCut = new TText(xbottom[mode], ybottom[mode], Form("%g-%gGeV",eBinsEta[y], eBinsEta[y+1]));
      textCut -> SetTextAngle(270);
      textCut -> SetTextFont(43);
      textCut -> SetTextSize(18);
      textCut -> Draw();
    }
  
}

void GetSubtractedDist(TH1F *histOrig, TH1F *histSub, TF1 *invMassFit, TF1 *invMassBG)
{
  
 
  for(int i = 0; i < invMassFit -> GetNpar(); i++)
    {
      invMassBG -> SetParameter(i, invMassFit -> GetParameter(i));
    }
  
  for(int i = 1; i < histSub -> GetNbinsX() + 1; i++)
    {
      histSub -> SetBinContent(i, histOrig -> GetBinContent(i) - invMassBG -> Eval(histOrig -> GetBinCenter(i)));
    }
  
  
} 
