#include "sPhenixStyle.h"
#include "sPhenixStyle.C"


void Jet_reso() 
{
  SetsPhenixStyle();
  TH1::SetDefaultSumw2();
  TH2::SetDefaultSumw2();

  TChain * ct = new TChain("T");
  //if you want to run one file use this
  ct->Add("../macro/output.root");

  //if you want to combine multiple files use this
  /*for(int i = 0; i < 100; i++){
    ct->Add(Form("../macro/condor/output_%i.root",i));
    }*/

  vector<float> *eta = 0;
  vector<float> *phi = 0;
  vector<float> *pt = 0;
  vector<float> *e = 0;
  vector<float> *subtracted_et = 0;
  vector<float> *truthEta = 0;
  vector<float> *truthPhi = 0;
  vector<float> *truthPt = 0;
  vector<float> *truthE = 0;
  vector<float> *subseedEta = 0;
  vector<float> *subseedPhi = 0;
  vector<float> *subseedPt = 0;
  vector<float> *subseedE = 0;
  vector<int> *subseedCut = 0;
  vector<float> *rawseedEta = 0;
  vector<float> *rawseedPhi = 0;
  vector<float> *rawseedPt = 0;
  vector<float> *rawseedE = 0;
  vector<int> *rawseedCut = 0;
  float *totalCalo = 0;
  int cent;

  ct->SetBranchAddress("eta",&eta);
  ct->SetBranchAddress("phi",&phi);
  ct->SetBranchAddress("pt",&pt);
  ct->SetBranchAddress("e",&e);
  ct->SetBranchAddress("subtracted_et",&subtracted_et);
  ct->SetBranchAddress("truthEta",&truthEta);
  ct->SetBranchAddress("truthPhi",&truthPhi);
  ct->SetBranchAddress("truthPt",&truthPt);
  ct->SetBranchAddress("truthE",&truthE);
  ct->SetBranchAddress("cent", &cent);

  ct->SetBranchAddress("rawseedEta", &rawseedEta);
  ct->SetBranchAddress("rawseedPhi", &rawseedPhi);
  ct->SetBranchAddress("rawseedPt", &rawseedPt);
  ct->SetBranchAddress("rawseedE", &rawseedE);
  ct->SetBranchAddress("rawseedCut", &rawseedCut);
  ct->SetBranchAddress("subseedEta", &subseedEta);
  ct->SetBranchAddress("subseedPhi", &subseedPhi);
  ct->SetBranchAddress("subseedPt", &subseedPt);
  ct->SetBranchAddress("subseedE", &subseedE);
  ct->SetBranchAddress("subseedCut", &subseedCut);

  const Float_t pt_range[] = {10,15,20,25,30,35,40,45,50,60,80};
  const int pt_N = sizeof(pt_range)/sizeof(float) - 1;
  const int resp_N = 60;
  Float_t resp_bins[resp_N+1];
  for(int i = 0; i < resp_N+1; i++){
    resp_bins[i] = 1.2/resp_N * i;
  }
  const int eta_N = 40;
  Float_t eta_bins[eta_N+1];
  for(int i = 0; i < eta_N+1; i++){
    eta_bins[i] = -1.1 + 2.2/eta_N * i;
  }
  const int phi_N = 40;
  Float_t phi_bins[phi_N+1];
  for(int i = 0; i < phi_N+1; i++){
    phi_bins[i] = -TMath::Pi() + 2*TMath::Pi()/phi_N * i;
  }
  const int subet_N = 400;
  Float_t subet_bins[subet_N+1];
  for(int i = 0; i < subet_N+1; i++){
    subet_bins[i] = i*0.5;
  }
  const float cent_bins[] = {-1, 0, 10, 30, 50, 80}; //the first bin is for inclusive in centrality/pp events
  const int cent_N = sizeof(cent_bins)/sizeof(float) - 1;

  TH3F *h_MC_Reso_pt = new TH3F("h_MC_Reso", "" , pt_N, pt_range, resp_N, resp_bins, cent_N, cent_bins);;
  h_MC_Reso_pt->GetXaxis()->SetTitle("p_{T}^{truth} [GeV]");
  h_MC_Reso_pt->GetYaxis()->SetTitle("p_{T}^{reco}/p_{T}^{truth}");

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

  int nentries = ct->GetEntries();
  for(int i = 0; i < nentries; i++){
    ct->GetEntry(i);

    int njets = truthPt->size();
    int nrecojets = pt->size();

    for(int tj = 0; tj < njets; tj++){
      //fill truth hists
      h_pt_true->Fill(truthPt->at(tj), cent);
      h_pt_true->Fill(truthPt->at(tj), -1);
      h_eta_phi_true->Fill(truthEta->at(tj),truthPhi->at(tj), cent);
      h_eta_phi_true->Fill(truthEta->at(tj),truthPhi->at(tj), -1);
      //do reco to truth jet matching
      float matchEta, matchPhi, matchPt, matchE, matchsubtracted_et, dR;
      float dRMax = 100;
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
	  matchE = e->at(rj);
	  matchsubtracted_et = subtracted_et->at(rj);
	  dRMax = dR;
	}
      }

      if(dRMax > 0.3) continue;
      h_MC_Reso_pt->Fill(truthPt->at(tj),matchPt/truthPt->at(tj), cent);
      h_pt_true_matched->Fill(truthPt->at(tj), cent);
      h_eta_phi_reco->Fill(matchEta,matchPhi, cent);
      h_pt_reco->Fill(matchPt, cent);
      h_MC_Reso_pt->Fill(truthPt->at(tj),matchPt/truthPt->at(tj), -1);
      h_pt_true_matched->Fill(truthPt->at(tj), -1);
      h_eta_phi_reco->Fill(matchEta,matchPhi, -1);
      h_pt_reco->Fill(matchPt, -1);
      h_subEt_pt->Fill(matchPt, matchsubtracted_et, cent);
      h_subEt_pt->Fill(matchPt, matchsubtracted_et, -1);
    }
  }

  TCanvas *c = new TCanvas("c","c");
  //c->Print("fits04.pdf("); //uncomment these to get a pdf with all the fits
  TLegend *leg = new TLegend(.25,.2,.6,.5);
  leg->SetFillStyle(0);
  gStyle->SetOptFit(1);
  for(int icent = 0; icent < cent_N; ++icent){
    for (int i = 0; i < pt_N; ++i){
      TF1 *func = new TF1("func","gaus",0,1.2);
      h_MC_Reso_pt->GetXaxis()->SetRange(i+1,i+1);
      h_MC_Reso_pt->GetZaxis()->SetRange(icent+1,icent+1);
      TH1F *h_temp = (TH1F*) h_MC_Reso_pt->Project3D("y");
      h_temp->Fit(func,"","",0,1.2);
      h_temp->Fit(func,"","",func->GetParameter(1)-1.5*func->GetParameter(2),func->GetParameter(1)+1.5*func->GetParameter(2));
      func->SetLineColor(kRed);
      h_temp->Draw();
      leg->AddEntry("",Form("%2.0f%%-%2.0f%%",h_MC_Reso_pt->GetZaxis()->GetBinLowEdge(icent+1),h_MC_Reso_pt->GetZaxis()->GetBinLowEdge(icent+2)),"");
      leg->AddEntry("",Form("%2.0f < p_T < %2.0f GeV",h_MC_Reso_pt->GetXaxis()->GetBinLowEdge(i+1),h_MC_Reso_pt->GetXaxis()->GetBinLowEdge(i+2)),"");
      leg->Draw("SAME");
      /*-------for calculating the JER uncertainty----*/
      float dsigma = func -> GetParError(2);
      float denergy = func -> GetParError(1);
      float sigma = func -> GetParameter(2);
      float energy = func -> GetParameter(1);

      float djer = dsigma/energy + sigma*denergy/pow(energy,2);//correct way to calculate jer
                                                               //accounting for the fact that the 
                                                               //mean response and width are correlated
      //c->Print("fits04.pdf");
      leg->Clear();
      g_jes[icent]->SetPoint(i,0.5*(pt_range[i]+pt_range[i+1]),func->GetParameter(1));
      g_jes[icent]->SetPointError(i,0.5*(pt_range[i+1]-pt_range[i]),func->GetParError(1));
      g_jer[icent]->SetPoint(i,0.5*(pt_range[i]+pt_range[i+1]),func->GetParameter(2)/func->GetParameter(1));
      
      //g_jer[icent]->SetPointError(i,0.5*(pt_range[i+1]-pt_range[i]),func->GetParError(2));
      g_jer[icent]->SetPointError(i,0.5*(pt_range[i+1]-pt_range[i]),djer);
    }
  }
  //c->Print("fits04.pdf)");



  TFile *f_out = new TFile("hists.root","RECREATE");
  for(int icent = 0; icent < cent_N; ++icent){
    g_jes[icent]->Write(Form("g_jes_cent%i",icent));
    g_jer[icent]->Write(Form("g_jer_cent%i",icent));
  }

  h_pt_true->Write();
  h_eta_phi_true->Write();
  h_pt_true_matched->Write();
  h_eta_phi_reco->Write();
  h_pt_reco->Write();
  h_subEt_pt->Write();
}

