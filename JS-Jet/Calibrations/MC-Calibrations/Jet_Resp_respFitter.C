#include "sPhenixStyle.h"
#include "sPhenixStyle.C"
#include "respFitterDef.h"
// This macro analyzes output from the jet validation sub module of the 
// JS-Jet module. To first order, it simply computes the jet energy scale (JES)
// and jet energy resolution (JER), but also compute the jet energy linearity
// necessary for the numerical inversion which is used to calibrate the MC
// to itself.

void Jet_Resp_respFitter(const char* fin = "", int isLin = 0) 
{
  //old definitions we need to make things work
  vector<float> etaBins;
  for(float i = etaStart; i <= etaEnd; i+=etaGap)
    {
      etaBins.push_back(i);
    }
  const int nEtaBins = etaBins.size() - 1 + extraBins; //one inclusive bin


  gSystem -> CopyFile(fin,Form("calibHists_%s",fin),kTRUE);
  TFile *f = new TFile(Form("calibHists_%s",fin),"UPDATE");
  TH2D *h_MC_Reso_pt[nEtaBins];
  for(int i = 0; i < nEtaBins; i++)
    {
      h_MC_Reso_pt[i] = (TH2D*)f -> Get(Form("h_MC_Reso_eta%d",i));
    }
  

  Float_t pt_range_truth[pt_N_truth];
  for(int i = 0; i < pt_N_truth+1; i++)
  {
    pt_range_truth[i] = 5+75./pt_N_truth * i;
  }

  TLegend *leg = new TLegend(.25,.2,.6,.5);
  leg->SetFillStyle(0);
  gStyle->SetOptFit(1);
 
  TGraphErrors *g_jes[nEtaBins];
  TGraphErrors *g_jer[nEtaBins];
  for(int i = 0; i < nEtaBins; i++)
    {
      g_jes[i] = new TGraphErrors(pt_N);
      g_jer[i] = new TGraphErrors(pt_N);
    }

  int stop;//Sets the number of bins to look at, which are different depending on whether or not you're doing the calibration
  float fitEnd;//The fit is super generic so it really doesn't care if you're fitting from 0-2 (JES) or 0-80 (linearity), but just to be thorough
  float fitStart;
  f->cd();
  if(isLin) 
    {
      stop = pt_N_truth-1;
      fitEnd = 80;
      fitStart = 1.;
    }
  else 
    {
      stop = pt_N;
      fitEnd = 2.;
      fitStart = 0.1;
    }
  for (int i = 0; i < stop; i++)
    {
      for(int j = 0; j < nEtaBins; j++)
	{
	  
	  TF1 *func = new TF1("func","gaus",0,fitEnd);
	  //h_MC_Reso_pt[j]->GetXaxis()->SetRange(i+1,i+1);
	  TH1F *h_temp = (TH1F*) h_MC_Reso_pt[j]->ProjectionY("proj",i+1,i+1);
	  
	  func -> SetParameter(1, h_temp -> GetBinCenter(h_temp -> GetMaximumBin()));
	  h_temp->Fit(func,"Lm","",fitStart,fitEnd);
	  if(isLin)h_temp -> SetTitle(Form("%g < p_{T}^{Truth} < %g",pt_range_truth[i],pt_range_truth[i+1]));
	  else h_temp -> SetTitle(Form("%g < p_{T}^{Truth} < %g", pt_range[i],  pt_range[i+1]));
	  h_temp -> Write(Form("hFit_pt%d_eta%d_Fit0",i,j));
	  float meanGuess = func -> GetParameter(1);
	  float rangeCut = 1.0;
	  if(isLin) rangeCut = nSigmaLin;
	  else rangeCut = nSigmaRat;
	  func -> SetParameter(1, h_temp -> GetBinCenter(h_temp -> GetMaximumBin()));
	  func -> SetParameter(2,1.0*meanGuess);
	  h_temp->Fit(func,"","",func->GetParameter(1)-rangeCut*func->GetParameter(2),func->GetParameter(1)+rangeCut*func->GetParameter(2));
	  func->SetLineColor(kRed);
	      
	  h_temp -> Write(Form("hFit_pt%d_eta%d_Fit1",i,j));
	      
	  leg->Clear();
	  float dsigma = func->GetParError(2);
	  float denergy = func->GetParError(1);
	  float sigma = func->GetParameter(2);
	  float energy = func->GetParameter(1);
	  
	  float djer = dsigma/energy + sigma*denergy/pow(energy,2);//correct way to compute error on the resolution.
	  if(!isLin)
	    {
	      g_jes[j]->SetPoint(i,0.5*(pt_range[i]+pt_range[i+1]),func->GetParameter(1));
	      g_jes[j]->SetPointError(i,0.5*(pt_range[i+1]-pt_range[i]),func->GetParError(1));
	  
	      g_jer[j]->SetPoint(i,0.5*(pt_range[i]+pt_range[i+1]),func->GetParameter(2)/func->GetParameter(1));
	      g_jer[j]->SetPointError(i,0.5*(pt_range[i+1]-pt_range[i]),djer);
	    }
	  else
	    {
	      g_jes[j]->SetPoint(i,0.5*(pt_range_truth[i]+pt_range_truth[i+1]),func->GetParameter(1));
	      g_jes[j]->SetPointError(i,0.5*(pt_range_truth[i+1]-pt_range_truth[i]),func->GetParError(1));
	      g_jer[j]->SetPoint(i,0.5*(pt_range_truth[i]+pt_range_truth[i+1]),func->GetParameter(2)/func->GetParameter(1));
	      g_jer[j]->SetPointError(i,0.5*(pt_range_truth[i+1]-pt_range_truth[i]),func->GetParError(2));
	    }
	
	}
    }
  for(int i = 0; i < nEtaBins; i++)
    {
      g_jes[i]->Write(Form("g_jes_eta%d",i));
      g_jer[i]->Write(Form("g_jer_eta%d",i));
    }
  std::cout << "All done!" << std::endl;
}


