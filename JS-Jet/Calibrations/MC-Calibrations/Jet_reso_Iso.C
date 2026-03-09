#include "sPhenixStyle.h"
#include "sPhenixStyle.C"

bool isIso(int thisRjet, int thisTjet, vector<float> *rJetpT, vector<float> *tJetpT, vector<float> *rJetEta, vector<float> *tJetEta, vector<float> *rJetPhi, vector<float> *tJetPhi, float minDr, TH1F *&rDist, TH1F *&tDist);
// This macro analyzes output from the jet validation sub module of the 
// JS-Jet module. To first order, it simply computes the jet energy scale (JES)
// and jet energy resolution (JER), but also compute the jet energy linearity
// necessary for the numerical inversion which is used to calibrate the MC
// to itself.
  
void Jet_reso_Iso(float jetR = 4, float isoParam = 1, int applyCorr = 0, int isLin = 0) 
{
  //jetR: jet radius
  //
  //isoParameter: used to compute distance required to consider a jet "isolated" 
  //and is multipled by the jet radius. See minDist variable below. 
  //
  //applyCorr: determines whether or not to apply calibrations. 
  //
  //isLin: measure the jet linearity instead of the response
  float minDist = .1*(jetR*isoParam);
  if(minDist)std::cout << "isolation requirement: dR > " << minDist << " [rad]" << std::endl;
  
  SetsPhenixStyle();
  TH1::SetDefaultSumw2();
  TH2::SetDefaultSumw2();
  TH3::SetDefaultSumw2();
  
  TChain * ct = new TChain("T");
  //ct->Add("run40_30GeV_10GeV_Spliced_RecoJets_test.root");  
  ct->Add("run40_30GeV_10GeV_Spliced_RecoJets.root");
  //if you want to combine multiple files use this
  //for(int i = 0; i < 15000; i++){
  //  ct->Add(Form("../macro/condor/output/jetIso_%d/isoJetCalibOut_%d.root",i,i));
  // }
  
  TH1F *recoDist = new TH1F("recoDist","distance between reco jets",200,0,5);
  TH1F *truthDist = new TH1F("truthDist","distance between truth jets",200,0,5);

  TFile *f_out = new TFile(Form("hists_R0%g_dR%g_Corr%d_isLin%d.root",jetR,isoParam,applyCorr,isLin),"RECREATE");
  
  vector<float> *eta = 0;
  vector<float> *phi = 0;
  vector<float> *pt = 0;
  //vector<float> *e = 0;
  //vector<float> *subtracted_et = 0;
  vector<float> *truthEta = 0;
  vector<float> *truthPhi = 0;
  vector<float> *truthPt = 0;
  //vector<float> *truthE = 0;

  vector<float> *subseedEta = 0;
  vector<float> *subseedPhi = 0;
  vector<float> *subseedPt = 0;
  vector<float> *subseedE = 0;
  vector<int> *subseedCut = 0;
  vector<float> *rawseedEta = 0;
  vector<float> *rawseedPhi = 0;
  vector<float> *rawseedPt = 0;
  //vector<float> *rawseedE = 0;
  //vector<int> *rawseedCut = 0;
  float *totalCalo = 0;
  int cent;
  Float_t rWeight, tWeight, RawWeight, SubWeight;
  

  ct->SetBranchAddress("eta",&eta);
  ct->SetBranchAddress("phi",&phi);
  ct->SetBranchAddress("pt",&pt);
  //ct->SetBranchAddress("e",&e);
  //ct->SetBranchAddress("subtracted_et",&subtracted_et);
  ct->SetBranchAddress("truthEta",&truthEta);
  ct->SetBranchAddress("truthPhi",&truthPhi);
  ct->SetBranchAddress("truthPt",&truthPt);
  //ct->SetBranchAddress("truthE",&truthE);
  //ct->SetBranchAddress("cent", &cent);

  ct->SetBranchAddress("rawseedEta", &rawseedEta);
  ct->SetBranchAddress("rawseedPhi", &rawseedPhi);
  ct->SetBranchAddress("rawseedPt", &rawseedPt);
  //ct->SetBranchAddress("rawseedE", &rawseedE);
  //ct->SetBranchAddress("rawseedCut", &rawseedCut);
  ct->SetBranchAddress("subseedEta", &subseedEta);
  ct->SetBranchAddress("subseedPhi", &subseedPhi);
  ct->SetBranchAddress("subseedPt", &subseedPt);
  //ct->SetBranchAddress("subseedE", &subseedE);
  //ct->SetBranchAddress("subseedCut", &subseedCut);

  ct->SetBranchAddress("tWeight",&tWeight);
  ct->SetBranchAddress("rWeight",&rWeight);
  ct->SetBranchAddress("RawWeight",&RawWeight);
  ct->SetBranchAddress("SubWeight",&SubWeight);
  
  //const Float_t pt_range[]  = {10,15,20,25,30,35,40,45,50,60,80};
  const Float_t pt_range[]  = {15,20,25,30,35,40,45,50,60,80};

  //if(applyCorr)  pt_range[] = {10,15,20,25,30,35,40,45,50,60,80};
  //if(isLin)const Float_t pt_range[] = {10,15,20,25,30,35,40,45,50,55,60,65,70,75,80};
   
  const int pt_N = sizeof(pt_range)/sizeof(float) - 1;
  //const int pt_N_truth = 34;//for including the 10-15 region
  const int pt_N_truth = 37;
  const int resp_N = 100;
  Float_t resp_bins[resp_N];
  for(int i = 0; i < resp_N + 1; i++){
    //resp_bins[i] = 1.2/resp_N * i;
    resp_bins[i] = 2./resp_N * i;
  }


  // Float_t pt_range_reco[pt_N_reco];
  //const int pt_N_reco = 71;
  // for(int i = 0; i < pt_N_reco + 1; i++)
  //   {
  //     pt_range_reco[i] = 80./pt_N_reco * i;
  //   }
  vector<Float_t> pt_range_reco1;
  Float_t ptBin = 0.;
  // while(ptBin < 80)
  //   {
  //     pt_range_reco1.push_back(ptBin);
  //     if(ptBin <= 20)ptBin += 0.1;
  //     else if(ptBin >20 && ptBin <= 30) ptBin += 1;
  //     else if(ptBin > 30 && ptBin < 40) ptBin +=0.1;
  //     else ptBin += 2.5;
  //   }
  while(ptBin < 79.9)
    {
      pt_range_reco1.push_back(ptBin);
      ptBin += 0.1;
    }
  
  const int pt_N_reco = pt_range_reco1.size();//sizeof(pt_range_reco1)/sizeof(float) - 1;
  Float_t pt_range_reco[pt_N_reco];
  for(int i = 0; i < pt_N_reco+1; i++)
    {
      if(i < pt_N_reco)pt_range_reco[i] = pt_range_reco1.at(i);
      else pt_range_reco[i] = 79.9;
    }
  Float_t pt_range_truth[pt_N_truth];
  Float_t rebin[pt_N_truth];
  for(int i = 0; i < pt_N_truth+1; i++)
  {
    //pt_range_truth[i] = 10+70./pt_N_truth * i;
    //pt_range_truth[i] = 10+2*i;
    pt_range_truth[i] = 15+1.5*i;
    //if( pt_range_truth[i] < 15) rebin[i] = 1;
    if(pt_range_truth[i] >= 15 && pt_range_truth[i] < 30) rebin[i] = 4;//4;
    else if(pt_range_truth[i] >= 30 && pt_range_truth[i] < 40) rebin[i] = 1;
    else rebin[i] = 10;
  }
  
  
  const int eta_N = 40;
  Float_t eta_bins[eta_N];
  for(int i = 0; i < eta_N+1; i++){
    eta_bins[i] = -1.1 + 2.2/eta_N * i;
  }
  const int phi_N = 40;
  Float_t phi_bins[phi_N];
  for(int i = 0; i < phi_N+1; i++){
    phi_bins[i] = -TMath::Pi() + 2*TMath::Pi()/phi_N * i;
  }
  const int subet_N = 400;
  Float_t subet_bins[subet_N];
  for(int i = 0; i < subet_N+1; i++){
    subet_bins[i] = i*0.2;
  }
  const float cent_bins[] = {-1, 0, 10, 30, 50, 80}; //the first bin is for inclusive in centrality/pp events
  const int cent_N = 1;//sizeof(cent_bins)/sizeof(float) - 1;

  TH3F *h_MC_Reso_pt = 0;
  if(isLin) h_MC_Reso_pt= new TH3F("h_MC_Reso", "" , pt_N_truth, pt_range_truth, pt_N_reco, pt_range_reco, cent_N, cent_bins);
  else  h_MC_Reso_pt= new TH3F("h_MC_Reso", "" , pt_N, pt_range, resp_N, resp_bins, cent_N, cent_bins);

  h_MC_Reso_pt -> GetXaxis()->SetTitle("p_{T}^{truth} [GeV]");
  if(isLin) h_MC_Reso_pt->GetYaxis()->SetTitle("p_{T}^{reco}");
  else h_MC_Reso_pt->GetYaxis()->SetTitle("p_{T}^{reco}/p_{T}^{truth}");
  TH2F *h_pt_reco = new TH2F("h_pt_reco","",subet_N,subet_bins, cent_N, cent_bins);
  h_pt_reco->GetXaxis()->SetTitle("p_{T} [GeV]");
  TH2F *h_pt_true = new TH2F("h_pt_true","",pt_N,pt_range, cent_N, cent_bins);
  h_pt_true->GetXaxis()->SetTitle("p_{T} [GeV]");
  TH2F *h_pt_true_matched = new TH2F("h_pt_true_matched","",subet_N,subet_bins, cent_N, cent_bins);
  h_pt_true_matched->GetXaxis()->SetTitle("p_{T} [GeV]");
  TH3F *h_eta_phi_reco = new TH3F("h_eta_phi_reco","",eta_N, eta_bins, phi_N, phi_bins, cent_N, cent_bins);
  h_eta_phi_reco->GetXaxis()->SetTitle("#eta");
  h_eta_phi_reco->GetYaxis()->SetTitle("#phi");
  TH3F *h_eta_phi_true = new TH3F("h_eta_phi_true","",eta_N, eta_bins, phi_N, phi_bins, cent_N, cent_bins);
  TH3F *h_subEt_pt = new TH3F("h_subEt_pt","",pt_N,pt_range, subet_N, subet_bins, cent_N, cent_bins);


  TGraphErrors *g_jes[cent_N];
  TGraphErrors *g_jer[cent_N];
  for(int icent = 0; icent < cent_N; icent++){
    g_jes[icent] = new TGraphErrors(pt_N);
    g_jer[icent] = new TGraphErrors(pt_N);
  }

  TFile *corrFile;
  TF1 *correction = new TF1("jet energy correction","1",0,80);
  if(applyCorr)
    {
      corrFile = new TFile("JES_IsoCorr_NumInv.root","READ");
      corrFile -> cd();
      correction = (TF1*)corrFile -> Get(Form("corrFit_Iso%g",isoParam));
    }

  int nentries = ct->GetEntries();
  for(int i = 0; i < nentries; i++){
    ct->GetEntry(i);
    cent = 101;
    int njets = truthPt->size();
    int nrecojets = pt->size();
    if(applyCorr)
      {
	for(int rj = 0; rj < nrecojets; rj++)
	  {
	    pt -> at(rj) *= correction -> Eval(pt -> at(rj));//correct all the jets in one go
	  }
      }
    for(int tj = 0; tj < njets; tj++){
      //fill truth hists
      //if(truthPt -> at(tj) < 10) continue;
      h_pt_true->Fill(truthPt->at(tj), cent);
      h_pt_true->Fill(truthPt->at(tj), -1);
      h_eta_phi_true->Fill(truthEta->at(tj),truthPhi->at(tj), cent);
      h_eta_phi_true->Fill(truthEta->at(tj),truthPhi->at(tj), -1);
      //do reco to truth jet matching
      float matchEta, matchPhi, matchPt, matchE, matchsubtracted_et, dR;
      float dRMax = 100;
      int recoMatchJet = -9999;
      
      for(int rj = 0; rj < nrecojets; rj++){
	float dEta = truthEta->at(tj) - eta->at(rj);
	float dPhi = truthPhi->at(tj) - phi->at(rj);
	while(dPhi > TMath::Pi()) dPhi -= 2*TMath::Pi();
	while(dPhi < -TMath::Pi()) dPhi += 2*TMath::Pi();
	dR = TMath::Sqrt(dEta*dEta + dPhi*dPhi);
	
	if(dR < dRMax){
	  matchEta = eta->at(rj);
	  matchPhi = phi->at(rj);
	  matchPt = pt->at(rj);
	  //matchE = e->at(rj);
	  //matchsubtracted_et = subtracted_et->at(rj);
	  dRMax = dR;
	  recoMatchJet = rj;
	}
      }
      
      if(dRMax > 0.1*0.75*jetR) continue;
      if(minDist && !isIso(recoMatchJet, tj, pt, truthPt, eta, truthEta, phi, truthPhi, minDist, recoDist, truthDist)) continue;
      //if(truthPt->at(tj) >= 30 && rWeight == 1) continue;
      if(!isLin)
	{//if we're applying the correction, we're looking at the JES, fill with ptReco/ptTruth
	  
	  h_MC_Reso_pt->Fill(truthPt->at(tj),matchPt/truthPt->at(tj), cent, rWeight);
	  h_MC_Reso_pt->Fill(truthPt->at(tj),matchPt/truthPt->at(tj), -1, rWeight);
	}
      else 
	{//We're looking at the linearity, fill with just the matchp
	  h_MC_Reso_pt->Fill(truthPt->at(tj),matchPt, cent, rWeight);
	  h_MC_Reso_pt->Fill(truthPt->at(tj),matchPt, -1, rWeight);
	}
      
      h_pt_true_matched->Fill(truthPt->at(tj), cent, tWeight);
      h_eta_phi_reco->Fill(matchEta,matchPhi, cent);
      h_pt_reco->Fill(matchPt, cent, rWeight);
   
      h_pt_true_matched->Fill(truthPt->at(tj), -1, tWeight);
      h_eta_phi_reco->Fill(matchEta,matchPhi, -1);
      h_pt_reco->Fill(matchPt, -1, rWeight);
      //h_subEt_pt->Fill(matchPt, matchsubtracted_et, cent);
      //h_subEt_pt->Fill(matchPt, matchsubtracted_et, -1);
    }
  }

  TCanvas *c = new TCanvas("c","c");
  //c->Print("fits04.pdf("); //uncomment these to get a pdf with all the fits
  TLegend *leg = new TLegend(.6,.9,.9,.9);
  leg->SetFillStyle(0);
  gStyle->SetOptFit(0);
  f_out -> cd();
  int stop;//Sets the number of bins to look at, which are different depending on whether or not you're doing the calibration
  float fitEnd;//The fit is super generic so it really doesn't care if you're fitting from 0-2 (JES) or 0-80 (linearity), but just to be thorough
  float fitStart;
  if(isLin) 
    {
      stop = pt_N_truth;
      fitEnd = 80.;
      fitStart = .1;
    }
  else 
    {
      stop = pt_N;
      fitEnd = 2.;
      fitStart = 0.1;
    }
  for(int icent = 0; icent < cent_N; ++icent){
    for (int i = 0; i < stop; ++i){
      TF1 *func = new TF1("func","gaus",fitStart,fitEnd);
      h_MC_Reso_pt->GetXaxis()->SetRange(i+1,i+1);
      h_MC_Reso_pt->GetZaxis()->SetRange(icent+1,icent+1);
      TH1F *h_temp = (TH1F*) h_MC_Reso_pt->Project3D("y");
      if(isLin)h_temp -> Rebin(rebin[i]);
      func -> SetParameter(1, h_temp -> GetBinCenter(h_temp -> GetMaximumBin()));

      h_temp -> Fit(func,"","",fitStart,fitEnd);
      h_temp -> Write(Form("hFit_cent%d_pt%d_Fit0",icent+1,i));
      func -> SetParameter(1, h_temp -> GetBinCenter(h_temp -> GetMaximumBin()));

      h_temp->Fit(func,"","",func->GetParameter(1)-0.75*func->GetParameter(2),func->GetParameter(1)+0.75*func->GetParameter(2));
      if(isLin)h_temp -> GetXaxis() -> SetRangeUser(func->GetParameter(1)-4*func->GetParameter(2),func->GetParameter(1)+4*func->GetParameter(2));
      func -> SetLineColor(2);
      func -> SetRange(func->GetParameter(1)-func->GetParameter(2),func->GetParameter(1)+func->GetParameter(2));
      h_temp->Draw();
      func -> Draw("same");
      gPad -> SetLogy();
      func->SetLineColor(2);

      //gPad -> WaitPrimitive();
      h_temp -> Write(Form("hFit_cent%d_pt%d_Fit1",icent+1,i));
      //leg->AddEntry("",Form("%2.0f%%-%2.0f%%",h_MC_Reso_pt->GetZaxis()->GetBinLowEdge(icent+1),h_MC_Reso_pt->GetZaxis()->GetBinLowEdge(icent+2)),"");
      leg->AddEntry("",Form("%g < p_T < %g GeV",h_MC_Reso_pt->GetXaxis()->GetBinLowEdge(i+1),h_MC_Reso_pt->GetXaxis()->GetBinLowEdge(i+2)),"");
      leg->Draw("SAME");
      c->Print(Form("plots/hFit_cent%d_pt%d_Fit1_Corr%d_isLin%d_Param%g.pdf",icent+1,i,applyCorr,isLin,isoParam));
      leg->Clear();
      float dsigma = func->GetParError(2);
      float denergy = func->GetParError(1);
      float sigma = func->GetParameter(2);
      float energy = func->GetParameter(1);
	  
      float djer = dsigma/energy + sigma*denergy/pow(energy,2);//correct way to compute error on the resolution.
      if(isLin)
	{
	  g_jes[icent]->SetPoint(i,0.5*(pt_range_truth[i]+pt_range_truth[i+1]),func->GetParameter(1));
	  g_jes[icent]->SetPointError(i,0.5*(pt_range_truth[i+1]-pt_range_truth[i]),func->GetParError(1));
	  
	  g_jer[icent]->SetPoint(i,0.5*(pt_range_truth[i]+pt_range_truth[i+1]),func->GetParameter(2)/func->GetParameter(1));
	  g_jer[icent]->SetPointError(i,0.5*(pt_range_truth[i+1]-pt_range_truth[i]),djer);
	}
      else
	{
	  g_jes[icent]->SetPoint(i,0.5*(pt_range[i]+pt_range[i+1]),func->GetParameter(1));
	  g_jes[icent]->SetPointError(i,0.5*(pt_range[i+1]-pt_range[i]),func->GetParError(1));
	  
	  g_jer[icent]->SetPoint(i,0.5*(pt_range[i]+pt_range[i+1]),func->GetParameter(2)/func->GetParameter(1));
	  g_jer[icent]->SetPointError(i,0.5*(pt_range[i+1]-pt_range[i]),djer);
	}
    }
  }
  //c->Print("fits04.pdf)");



  for(int icent = 0; icent < cent_N; ++icent){
    g_jes[icent]->Write(Form("g_jes_cent%i",icent));
    g_jer[icent]->Write(Form("g_jer_cent%i",icent));
  }
  
  recoDist -> Write();
  truthDist -> Write();
  
  h_pt_true->Write();
  h_eta_phi_true->Write();
  h_pt_true_matched->Write();
  h_eta_phi_reco->Write();
  h_pt_reco->Write();
  h_subEt_pt->Write();
  
  f_out -> Close();
}

bool isIso(int thisRjet, int thisTjet, vector<float> *rJetpT, vector<float> *tJetpT, vector<float> *rJetEta, vector<float> *tJetEta, vector<float> *rJetPhi, vector<float> *tJetPhi, float minDr, TH1F *&rDist, TH1F *&tDist)
{
  if(thisRjet < 0) return false;
  float thisRJetEta = rJetEta->at(thisRjet);
  float thisTJetEta = tJetEta->at(thisTjet);
  float thisRJetPhi = rJetPhi->at(thisRjet);
  float thisTJetPhi = tJetPhi->at(thisTjet);
  float minPt = 3.5;
  float dr = -1;
  
  //uncomment these cout statements if you need to convince yourself this makes sense. 
  //std::cout << "On rJet: " << thisRjet << " with size " <<  rJetpT->size() << std::endl;
  for(int i = 0; i < rJetpT->size(); i++)
    {
      if(i == thisRjet) 
	{
	  //std::cout << "On index: " << i << "; thisRJet: " << thisRjet << std::endl;
	  continue;
	}
      if(rJetpT->at(i) < minPt)
	{
	  //std::cout << "Discarding reco jet with pt: " << rJetpT->at(i) << std::endl;
	  continue;
	}
      float deta = pow(thisRJetEta - rJetEta->at(i),2);
      float dphi = thisRJetPhi - rJetPhi->at(i);
      while(dphi > TMath::Pi()) dphi -= 2*TMath::Pi();
      while(dphi < -TMath::Pi()) dphi += 2*TMath::Pi();
      dphi = pow(dphi,2);

      dr = sqrt(deta + dphi);
     
      if(dr < minDr) return false;
    
    }
 
  if(dr)rDist -> Fill(dr);//dr will stay at -1 if above loop is totally bypassed, which usually happens becuase of the p_T filter. 
  dr = -1;
  for(int i = 0; i < tJetpT->size(); i++)
    {
      if(i == thisTjet) continue;
      //if(tJetpT < minPt) continue;
      
      float deta = pow(thisTJetEta - tJetEta->at(i),2);
      float dphi = pow(thisTJetPhi - tJetPhi->at(i),2);
      while(dphi > TMath::Pi()) dphi -= 2*TMath::Pi();
      while(dphi < -TMath::Pi()) dphi += 2*TMath::Pi();
      dphi = pow(dphi,2);

      dr = sqrt(deta + dphi);
      
      if(dr < minDr) return false;
    }     
  
  if(dr)tDist -> Fill(dr);
  
  return true;
}
