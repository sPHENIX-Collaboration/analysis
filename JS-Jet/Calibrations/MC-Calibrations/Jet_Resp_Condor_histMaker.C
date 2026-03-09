#include "sPhenixStyle.h"
#include "sPhenixStyle.C"
#include "respFitterDef.h"

// This macro analyzes output from the jet validation sub module of the 
// JS-Jet module. To first order, it simply computes the jet energy scale (JES)
// and jet energy resolution (JER), but also compute the jet energy linearity
// necessary for the numerical inversion which is used to calibrate the MC
// to itself.
int getEtaBin(float tJeteta, int nEtaBins, std::vector<float> etaBins);
bool isInRange(float truthJetPt, float mcWeight);
float getLeadJetPt(std::vector<float> *tJetPt);


void Jet_Resp_Condor_histMaker(float jetR = 4, int applyCorr = 0, int isLin = 0, const char* fin= "", int segment = 0, int trig = 0) 
{
  //jetR: jet radius
  //
  //applyCorr: determines whether or not to apply calibrations. 
  //
  //isLin: measure the jet linearity instead of the response
  //if variables appear undefined, see respFitterDef.h
  vector<float> etaBins;
  for(float i = etaStart; i <= etaEnd; i += etaGap)
    {
      etaBins.push_back(i);
    }
  const int nEtaBins = etaBins.size() - 1 + extraBins; //see response fitter defs 
  std::cout << "nEtaBins " << nEtaBins << std::endl;
  float maxDist = 0.1*0.75*jetR;
  
  SetsPhenixStyle();
  TH1::SetDefaultSumw2();
  TH2::SetDefaultSumw2();

  TChain * ct = new TChain("T");
  ct -> Add(fin);
  TH1F *nEvents = new TH1F("nEvents","nEvents",3,0.5,3.5);
  //if(trig == 0) nEvents -> SetBinContent(1,ct->GetEntries());
  //else if(trig == 10) nEvents -> SetBinContent(2,ct->GetEntries());
  // else nEvents -> SetBinContent(3,ct -> GetEntries());
  
  TH1F *recoDist = new TH1F("recoDist","distance between reco jets",200,0,5);
  TH1F *truthDist = new TH1F("truthDist","distance between truth jets",200,0,5);

  TFile *f_out = new TFile(Form("/gpfs/mnt/gpfs02/sphenix/user/ahodges/macros_git/analysis/JS-JetOrig/JetValidation/offline/respFitting/output/hists_R0%g_Corr%d_isLin%d_2D_trig%d_seg%d.root",jetR,applyCorr,isLin,trig,segment),"RECREATE");
  
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
  Float_t rWeight, tWeight, RawWeight, SubWeight;
  float mcWeight;
  
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
  ct->SetBranchAddress("mcWeight",&mcWeight);
    
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

  
  float resp_bins[resp_N+1];
  for(int i = 0; i < resp_N+1; i++){
    resp_bins[i] = 2.0/resp_N * i;
  }

  Float_t pt_range_reco[pt_N_reco+1];
  for(int i = 0; i < pt_N_reco+1; i++)
    {
      pt_range_reco[i] = 80./pt_N_reco*i;
    }
  
  Float_t pt_range_truth[pt_N_truth];
  for(int i = 0; i < pt_N_truth+1; i++)
    {
      //pt_range_truth[i] = 10+70./pt_N_truth * i;
      pt_range_truth[i] = 5+75./pt_N_truth * i;
    }


  
  TH2D *h_MC_Reso_pt[nEtaBins];

  if(isLin) 
    {
      for(int i = 0; i < nEtaBins; i++)
      	{
      	  h_MC_Reso_pt[i] = new TH2D(Form("h_MC_Reso_eta%d",i), "" , pt_N_truth, pt_range_truth, pt_N_reco, pt_range_reco);
      	  h_MC_Reso_pt[i] -> GetXaxis()->SetTitle("p_{T}^{truth} [GeV]");
      	  h_MC_Reso_pt[i] -> GetYaxis()->SetTitle("p_{T}^{reco}");
      	}
    }
  else
    {
      for(int i = 0; i < nEtaBins; i++) 
	{
	  h_MC_Reso_pt[i]= new TH2D(Form("h_MC_Reso_eta%d",i),Form("h_MC_Reso_eta%d",i) , pt_N, pt_range, resp_N, resp_bins);
	  h_MC_Reso_pt[i] -> GetXaxis()->SetTitle("p_{T}^{truth} [GeV]");
	  h_MC_Reso_pt[i] -> GetYaxis()->SetTitle("p_{T}^{reco}/p_{T}^{truth}");
	}
    }

 
  TH1F *h_pt_true_lead = new TH1F("h_pt_true_lead","",pt_N,pt_range);
  h_pt_true_lead->GetXaxis()->SetTitle("p_{T} [GeV]");
  TH1F *h_pt_true_lead_preCut = new TH1F("h_pt_true_lead_preCut","",50,0,100);
  h_pt_true_lead_preCut->GetXaxis()->SetTitle("p_{T} [GeV]");

  
  Float_t subet_bins[subet_N+1];
  for(int i = 0; i < subet_N+1; i++){
    subet_bins[i] = i*0.5;
  }

  TH1F *h_pt_true_matched[nEtaBins-1];
  for(int i = 0; i < nEtaBins-1; i++)
    {
      h_pt_true_matched[i] = new TH1F(Form("h_pt_true_matched_eta%d",i),"",subet_N,subet_bins);
      h_pt_true_matched[i]->GetXaxis()->SetTitle("p_{T} [GeV]");
    }
  
  
  TFile *corrFile = new TFile("JES_IsoCorr_NumInv.root");
  TF1 *correction[nEtaBins-1];//last bin doesn't have a unique calibration
  TF1 *correctionExtrap[nEtaBins-1];
  for(int i = 0; i < nEtaBins-1; i++) correction[i] = new TF1(Form("jes_Corr_eta%d",i),"1",0,80);
  if(applyCorr && corrFile)
    {
      corrFile -> cd();
      for(int i = 0; i<nEtaBins-1; i++)correction[i] = (TF1*)corrFile -> Get(Form("corrFit_eta%d",i));
      for(int i = 0; i<nEtaBins-1; i++)correctionExtrap[i] = (TF1*)corrFile -> Get(Form("corrFit_eta%d_extrap",i));
    }

  int nentries = ct->GetEntries();
  std::cout << "about to run over " << nentries << " events" << std::endl;
  float centileIterator = 0.1;
  for(int i = 0; i < nentries; i++){
    ct->GetEntry(i);
    if(i == (int)floor(nentries*centileIterator))
      {
	std::cout << centileIterator*100.  << "% of the way done" << std::endl;
	centileIterator += 0.1;
      }
    if(trig == 0)nEvents -> Fill(1);
    else if(trig == 10) nEvents -> Fill(2);
    else if(trig == 30) nEvents -> Fill(3);
    int njets = truthPt->size();
    int nrecojets = pt->size();
    for(int tj = 0; tj < njets; tj++){
      //fill truth hists
      // to avoid statistical artifacts from double counting, the datasets can't overlap
      float maxTJetPt = getLeadJetPt(truthPt);
      h_pt_true_lead_preCut->Fill(maxTJetPt);

      if(!isInRange(maxTJetPt, mcWeight))
	{
	  if(verbosity)std::cout << "ERRCODE0: max jet pt out of range: " << std::endl;
	  continue;
	}
      h_pt_true_lead->Fill(maxTJetPt, mcWeight);
      
      
      
      //do reco to truth jet matching
      float matchEta, matchPhi, matchPt, matchE, matchsubtracted_et, dR;
      float dRMax = 100;
      int recoMatchJet = -9999;

      for(int rj = 0; rj < nrecojets; rj++){
	if(abs(eta->at(rj)) > 1.0-jetR*0.1)
	  {
	    if(verbosity)std::cout << "ERRCODE1: reco jet outside eta range" << std::endl;
	    continue;
	  }

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
	  recoMatchJet = rj;
	}
      }
      int etaBin = getEtaBin(matchEta, nEtaBins-extraBins, etaBins);
      if(etaBin == -1) 
       	{
	  if(verbosity)std::cout << "ERRCODE2: match eta: " << matchEta << "; match eta bin: " << etaBin << std::endl;
	  continue; //outside acceptance
	}
      if(dRMax > maxDist)
	{
	  if(verbosity)std::cout << "ERRCODE3: drMax > maxDist: " << dRMax << std::endl;
	  continue;
	}
     
      if(!isLin)
	{
	  float corr = 1;
	  if(applyCorr)
	    {
	      if(correctionExtrap[etaBin] -> Eval(matchPt) > 1)
		{
		  corr = correctionExtrap[etaBin] -> Eval(matchPt);
		}
	      else
		{
		  corr = correction[etaBin] -> Eval(matchPt);
		}
	    }
	  h_MC_Reso_pt[etaBin]->Fill(truthPt->at(tj),corr*matchPt/truthPt->at(tj), mcWeight);//eta differential JES
	  
	  h_MC_Reso_pt[nEtaBins-2]->Fill(truthPt->at(tj),corr*matchPt/truthPt->at(tj), mcWeight);//inclusive JES with eta differential correction

	  if(applyCorr)corr = correction[nEtaBins - 2] -> Eval(matchPt);
	  h_MC_Reso_pt[nEtaBins-1]->Fill(truthPt->at(tj),corr*matchPt/truthPt->at(tj), mcWeight);//inclusive JES with eta inclusive correction
	  
	}
      else 
	{
	  //We're looking at the linearity, fill with just the matchpt
	  h_MC_Reso_pt[etaBin]->Fill(truthPt->at(tj),matchPt, mcWeight);
	  h_MC_Reso_pt[nEtaBins-2]->Fill(truthPt->at(tj),matchPt, mcWeight);
	}
      
      h_pt_true_matched[etaBin]->Fill(truthPt->at(tj));
      h_pt_true_matched[nEtaBins-2]->Fill(truthPt->at(tj));
   
    }
  }
  f_out -> cd();
  for(int i = 0; i < nEtaBins; i++) 
    {
      h_MC_Reso_pt[i] -> Write();
    }
  recoDist -> Write();
  truthDist -> Write();
  for(int i = 0; i < nEtaBins-1; i++)
    {
      h_pt_true_matched[i]->Write();
    }
  h_pt_true_lead -> Write();
  h_pt_true_lead_preCut -> Write();
  nEvents -> Write();
  f_out -> Close();
  std::cout << "All done!" << std::endl;
}

int getEtaBin(float jetEta, int nEtaBins, std::vector<float> etaBins)
{
  //std::cout << "jetEta: " << jetEta << std::endl;
  int etaBin = -1;
  if(jetEta > etaBins.at(nEtaBins)) return -1;
  if(jetEta < etaBins.at(0)) return -1;
  for(int i = 0; i < nEtaBins; i++)
    {
      if(jetEta > etaBins.at(i) && jetEta <= etaBins.at(i+1))
	{
	  etaBin = i;
	  //return etaBin;
	}
    }
  //std::cout << "etaBin: " << etaBin << std::endl;
  return etaBin;
}

bool isInRange(float truthJetPt, float mcWeight)
{
  float ptCutOff1 = -1; 
  float ptCutOff2 = -1;
  
  
  if(abs(mcWeight/39.06e-3 - 1) < 1e-7)//Minimum bias
    {
      //std::cout << "MB event found" << std::endl;
      ptCutOff1 = 0;
      ptCutOff2 = 14;
    }
  else if(abs(mcWeight/3.210e-6 - 1) < 1e-7)
    {
      //std::cout << "10GeV event found" << std::endl;
      ptCutOff1 = 14;
      ptCutOff2 = 37;
    }
  else if(abs(mcWeight/2.178e-9 - 1) < 1e-7)
    {
      //std::cout << "30GeV event found" << std::endl;
      ptCutOff1 = 37;
      ptCutOff2 = 3000;
    }

  if(truthJetPt < ptCutOff2 && truthJetPt >= ptCutOff1) return true;
  return false;
}
 
float getLeadJetPt(std::vector<float> *tJetPt)
{
  float maxpt = 0;
  for(int i = 0; i < tJetPt->size(); i++)
    {
      if(tJetPt->at(i) > maxpt) maxpt = tJetPt->at(i);
    }
  
  return maxpt;
}
