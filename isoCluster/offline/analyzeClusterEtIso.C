#include <sPhenixStyle.h>
#include <sPhenixStyle.C>

/*
  This file analyzes the output of the primary
  isoCluster module, specifically the correlation
  between the cluster tranverse energy and the 
  likelihood of clusters to be direct photons
  based on their Et values

  Anthony Hodges, UIUC, February 15th, 2023
*/

void analyzeClusterEtIso(int pass)
{

  SetsPhenixStyle();

  TFile *fin = new TFile(Form("../macro/condor/aggregation/gammaJet_pass%d.root",pass));
  
  TTree *T = (TTree*)fin -> Get("T");
  
  vector<float> *clusterPhi = 0;
  vector<float> *clusterEta = 0;
  vector<float> *clusterE = 0;
  vector<float> *clusterPt = 0;
  vector<float> *clusterEtIso = 0;
  vector<float> *clusterchisq = 0;
  vector<float> *photonPhi = 0;
  vector<float> *photonEta = 0;
  vector<float> *photonE = 0;
  vector<float> *photonPt = 0;

  T -> SetBranchAddress("clusterPhi",&clusterPhi);
  T -> SetBranchAddress("clusterEta",&clusterEta);
  T -> SetBranchAddress("clusterE",&clusterE);
  T -> SetBranchAddress("clusterPt",&clusterPt);
  T -> SetBranchAddress("clusterEtIso",&clusterEtIso);
  T -> SetBranchAddress("photonPhi",&photonPhi);
  T -> SetBranchAddress("photonEta",&photonEta);
  T -> SetBranchAddress("photonE",&photonE);
  T -> SetBranchAddress("photonPt",&photonPt);
  
  int nEvents = T -> GetEntries();
  
  std::cout << "Analyzing " << nEvents << " entries" << std::endl;
  
  TH2F *isoEtE = new TH2F("isoEtE","isoEtE",500,20,100,500,-10,10);
  isoEtE -> SetTitle(";E [GeV]; E_T [GeV]");
  TH2F *isoEtMatchFrac = new TH2F("isoEtMatchFrac","isoEtMatchFrac",500,-50,50,100,0,1);
  isoEtMatchFrac -> SetTitle("E_T [GeV]; Purity");
  
  //eTIso calibration. Determine sigmalized values for Et based on clusterE
  float eBins[] = {25,30,35,40,45,50,55,60};
  int nBins = sizeof(eBins)/sizeof(eBins[0]);
  float fitStart = -60; float fitEnd = 60;

  for(int i = 0; i < nEvents; i++)
    {
      T -> GetEntry(i);
      
      for(int j = 0; j < clusterEtIso -> size(); j++)
	{
	  isoEtE -> Fill(clusterE -> at(j), clusterEtIso -> at(j));
	  
	}
    }
  
  TCanvas *c1 = new TCanvas();
  isoEtE -> Draw("colz");

  TGraphErrors *sigma = new TGraphErrors();
  TGraphErrors *mu = new TGraphErrors();

  for(int i = 0; i < nBins-1; i++)
    {
      TH1F *isoEtEProj = (TH1F*)isoEtE -> ProjectionY("projection",isoEtE -> GetXaxis() -> FindBin(eBins[i] + 0.001), isoEtE -> GetXaxis() -> FindBin(eBins[i+1] - 0.001));
      
      TF1 *func = new TF1("func","gaus",fitStart, fitEnd);
      func -> SetLineColor(2);
      isoEtEProj -> Fit(func,"Q0","",fitStart,fitEnd);
      TCanvas *cTemp = new TCanvas();

      isoEtEProj -> Fit(func,"Q","",func -> GetParameter(1) - func -> GetParameter(2), func -> GetParameter(1) + func -> GetParameter(2));
      sigma -> SetPoint(i, (eBins[i+1] + eBins[i])/2, func -> GetParameter(2));
      sigma -> SetPointError(i, 0, func -> GetParError(2));
      
      mu ->  SetPoint(i, (eBins[i+1] + eBins[i])/2, func -> GetParameter(1));
      mu -> SetPointError(i, 0, func -> GetParError(1));
    }

  TCanvas *c2 = new TCanvas();
  sigma -> SetTitle(";Cluster E [GeV]; Iso E_{T} #sigma [GeV]");
  sigma -> Draw("ap");
  TF1 *sigmaFit = new TF1("sigmaFit","pol1",eBins[0],eBins[nBins-1]);
  sigmaFit -> SetLineColor(2);
  sigma -> Fit(sigmaFit,"RQ","");
  
  TCanvas *c3 = new TCanvas();
  mu -> SetTitle(";Cluster E [GeV];Iso E_{T} #mu [GeV]");
  mu -> Draw("ap");
  //TF1 *muFit = new TF1("muFit","pol8",eBins[0],eBins[nBins-1]);
  TF1 *muFit = new TF1("muFit","[0]*exp([1]*pow(x,[2]) + [3]) + [4]",(eBins[0] + eBins[1])/2,(eBins[nBins-2]+eBins[nBins-1])/2);
  muFit -> SetParameter(0, 0.00017);
  

  muFit -> SetLineColor(2);
  mu -> Fit(muFit,"RQ","");
  //Now we're going to go through the clusters again and assess
  //the correlation between eT and direct photon purity
  float sigmas[] = {0.5, 1 , 1.5, 2, 2.5, 3};
  const int nSigmas = sizeof(sigmas)/sizeof(sigmas[0]);
  int colors[] = {1,2,4,kGreen+2, kViolet,kCyan,kOrange+2,kMagenta+2,kAzure-2};
  
  TGraphErrors *gPurity[nSigmas];
  TGraphErrors *gEfficiency[nSigmas];

  TCanvas *c4 = new TCanvas();
  TCanvas *c5 = new TCanvas();
  TH1F *hdrMin = new TH1F("drMin","drMin",100,0,0.6);
  float dRMax = 0.12;

  TH1F *hEResp = new TH1F("eResp","eResp",200,0,2);
  float respMin = 0.1;
  TLegend *leg = new TLegend(0.5,0.5,0.9,0.9);
  leg -> SetFillStyle(0);
  leg -> SetBorderSize(0);
  
  for(int s = 0; s < nSigmas; s++)
    {
      gPurity[s] = new TGraphErrors();
      gPurity[s] -> SetMarkerColor(colors[s]);

      gEfficiency[s] = new TGraphErrors();
      gEfficiency[s] -> SetMarkerColor(colors[s]);
      for(int e = 0; e < nBins; e++)
	{
	  
	  int nMatch = 0;
	  int nIsoCluster = 0;
	  int nDirPhotons = 0;
	  for(int i = 0; i < nEvents; i++)
	    {
	      T -> GetEntry(i);
	  
	      if(photonE -> size() == 0) continue;
	      nDirPhotons += photonE -> size();
	      int matchIDCluster = -1;
	      int matchIDPhoton = -1;
	      int checkOnce = 0;
	      for(int k = 0; k < clusterE -> size(); k++)
		{
		  if(clusterE -> at(k) < eBins[e] || clusterE -> at(k) > eBins[e+1]) continue;
		  if(abs(clusterEtIso -> at(k)) > muFit -> Eval(clusterE -> at(k)) + sigmas[s]*sigmaFit -> Eval(clusterE -> at(k))) continue;
		  nIsoCluster++;
		  float drMin = 1000;
		  
		  for(int l = 0; l < photonE -> size(); l++)
		    {
		      if((photonE -> at(l) <  eBins[e] || photonE-> at(l) > eBins[e+1]) && !checkOnce)
			{
			  checkOnce = 1;
			  nDirPhotons++;
			}
		      float dPhi = clusterPhi -> at(k) - photonPhi -> at(l);
		      float dEta = clusterEta -> at(k) - photonEta -> at(l);
	      
		      while(dPhi > M_PI) dPhi -= 2*M_PI;
		      while(dPhi < -M_PI) dPhi += 2*M_PI;
	      
		      float dr = sqrt(pow(dPhi,2) + pow(dEta,2));
	      
		      if(dr < drMin) 
			{
			  drMin = dr;
			  matchIDCluster = k;
			  matchIDPhoton = l;
			}
		    }//direct photon loop
		  
		  hdrMin -> Fill(drMin);

		  if(!matchIDCluster || !matchIDPhoton) continue;

		  float response =  clusterE -> at(matchIDCluster)/photonE -> at(matchIDPhoton);
		  hEResp -> Fill(response);

		  if(drMin < dRMax && abs(1 - response) < respMin) nMatch++;
      
		}//cluster loop
	  
	 
	    }//event loop
     
	  float purity = 0;
	  if(nIsoCluster != 0) purity = (float)nMatch/(float)nIsoCluster;
	  float efficiency = (float)nMatch/(float)nDirPhotons;
	  
	  
	  // std::cout << Form("nMatch for sigma %g: %d with %d nIsoCluster",sigmas[s],nMatch,nIsoCluster) << std::endl;
	  // std::cout << "filling with purity: " << purity << std::endl;
	  gPurity[s] -> SetPoint(e, (eBins[e] + eBins[e+1])/2, purity);
	  gEfficiency[s] -> SetPoint(e, (eBins[e] + eBins[e+1])/2, efficiency);
      
	  
	}//energy loop
      leg -> AddEntry(gPurity[s], Form("E_{T} Iso #sigma < %g",sigmas[s]),"p");
      if(s == 0) 
	{
	  c4 -> cd();
	  gPurity[s] -> Draw("ap");
	  gPurity[s] -> SetTitle(";Cluster E [GeV]; Purity");
	  gPurity[s] -> GetYaxis() -> SetRangeUser(0,0.02);
	  
	  c5 -> cd();
	  gEfficiency[s] -> Draw("ap");
	  gEfficiency[s] -> SetTitle(";Cluster E [GeV]; Efficiency");
	  gEfficiency[s] -> GetYaxis() -> SetRangeUser(0,0.02);
	}
      else 
	{
	  c4 -> cd();
	  gPurity[s] -> Draw("samep");
	  c5 -> cd();
	  gEfficiency[s] -> Draw("samep");
	}
    }//sigma loop
  leg -> Draw("same");
  
  TCanvas *cDrMin = new TCanvas();
  hdrMin -> Draw("epx0");
  hdrMin -> GetYaxis() -> SetRangeUser(1e-1, 10*hdrMin -> GetMaximum());
  TLine *ldrMax = new TLine(dRMax, 1e-1, dRMax, hdrMin -> GetMaximum());
  ldrMax -> SetLineStyle(7);
  ldrMax -> SetLineColor(2);
  ldrMax -> Draw("same");
  gPad -> SetLogy();
  
  TCanvas *cResp = new TCanvas();
  hEResp -> Draw("epx0");
  TLine *lRespMin1 = new TLine(1-respMin, 0, 1-respMin, hEResp -> GetMaximum());
  lRespMin1 -> SetLineStyle(7);
  lRespMin1 -> SetLineColor(2);
  lRespMin1 -> Draw("same");

  TLine *lRespMin2 = new TLine(1+respMin, 0, 1+respMin, hEResp -> GetMaximum());
  lRespMin2 -> SetLineStyle(7);
  lRespMin2 -> SetLineColor(2);
  lRespMin2 -> Draw("same");
      
}
  
			
 
      
	  
      
	   
	      
	      
				    
      
