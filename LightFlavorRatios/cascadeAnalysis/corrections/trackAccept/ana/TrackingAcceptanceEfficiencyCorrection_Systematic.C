#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TH3F.h>
#include <TGraphAsymmErrors.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TPaveText.h>
#include <TLegend.h>
#include <TMath.h>
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <iomanip>
#include <sstream>

using namespace ROOT::Math;

void TrackingAcceptanceEfficiencyCorrection_Systematic()
{
  gStyle->SetOptStat(0);

  TH1::SetDefaultSumw2();
  TH2::SetDefaultSumw2();
  TH3::SetDefaultSumw2();

  // No dead/hot channel maps applied
  TFile* f_pionSim_noDH = TFile::Open("/sphenix/user/rosstom/analysis/LightFlavorRatios/bachelor_pi_efficiency/sim/noDeadChannels_2M_pions.root", "READ");
  TTree* pionTree_noDH = (TTree*)f_pionSim_noDH->Get("ntp_gtrack");

  TH3F* h_truePions_noDH = new TH3F("h_truePions_noDH",";True #pi Track p_{T} [GeV/c];True #pi Track #eta;True #pi Track #phi",25,0,3,25,-1.1,1.1,25,-M_PI,M_PI);
  TH3F* h_recoPions_noDH = new TH3F("h_recoPions_noDH",";True #pi Track p_{T} [GeV/c];True #pi Track #eta;True #pi Track #phi",25,0,3,25,-1.1,1.1,25,-M_PI,M_PI);

  pionTree_noDH->Draw("gphi:geta:gpt>>h_truePions_noDH","abs(gvz) < 10 && gnmaps > 0 && gnintt > 0 && gntpc > 19");
  pionTree_noDH->Draw("gphi:geta:gpt>>h_recoPions_noDH","abs(gvz) < 10 && gnmaps > 0 && gnintt > 0 && gntpc > 19 && nmaps > 0 && nintt > 0 && ntpc > 19 && quality < 400");

  TH3F *h_pionEff_noDH = (TH3F*)h_recoPions_noDH->Clone("h_pionEff_noDH");
  h_pionEff_noDH->Divide(h_recoPions_noDH,h_truePions_noDH,1.0,1.0,"B");

  // with dead/hot channel maps applied (5 runs) avg 6.14%
  TFile* f_pionSim = TFile::Open("/sphenix/user/rosstom/analysis/LightFlavorRatios/bachelor_pi_efficiency/sim/fiveRuns_5M_pions.root", "READ");
  TTree* pionTree = (TTree*)f_pionSim->Get("ntp_gtrack");

  TH3F* h_truePions = new TH3F("h_truePions",";True #pi Track p_{T} [GeV/c];True #pi Track #eta;True #pi Track #phi",25,0,3,25,-1.1,1.1,25,-M_PI,M_PI);
  TH3F* h_recoPions = new TH3F("h_recoPions",";True #pi Track p_{T} [GeV/c];True #pi Track #eta;True #pi Track #phi",25,0,3,25,-1.1,1.1,25,-M_PI,M_PI);

  pionTree->Draw("gphi:geta:gpt>>h_truePions","abs(gvz) < 10 && gnmaps > 0 && gnintt > 0 && gntpc > 19");
  pionTree->Draw("gphi:geta:gpt>>h_recoPions","abs(gvz) < 10 && gnmaps > 0 && gnintt > 0 && gntpc > 19 && nmaps > 0 && nintt > 0 && ntpc > 19 && quality < 400");

  TH3F *h_pionEff = (TH3F*)h_recoPions->Clone("h_pionEff");
  h_pionEff->Divide(h_recoPions,h_truePions,1.0,1.0,"B");
  
  TH3F *h_deadHot_Eff = (TH3F*)h_pionEff->Clone("h_deadHot_Eff");
  h_deadHot_Eff->Divide(h_pionEff,h_pionEff_noDH,1.0,1.0);

  TH2D* h_eta_pT = (TH2D*)h_deadHot_Eff->Project3D("yx");
  h_eta_pT->SetName("h_eta_pT_proj");
  h_eta_pT->Scale(1./25.);
  h_eta_pT->SetDirectory(0);

  TH2D* h_phi_pT = (TH2D*)h_deadHot_Eff->Project3D("zx");
  h_phi_pT->SetName("h_phi_pT_proj");
  h_phi_pT->Scale(1./25.);
  h_phi_pT->SetDirectory(0);

  TCanvas* c1 = new TCanvas("c1", "Eta vs pT", 800, 600);
  c1->SetRightMargin(0.15);
 
  h_eta_pT->SetTitle(";p_{T} [GeV/c];#eta;Acceptance");
  h_eta_pT->SetContour(99); 
  h_eta_pT->Draw("COLZ");

  //h_eta_pT->Draw("TEXT same");

  //gStyle->SetPaintTextFormat(".2f");
  gPad->Update();

  TPaletteAxis* palette1 = (TPaletteAxis*)h_eta_pT->GetListOfFunctions()->FindObject("palette");
  if (palette1)
  {
    palette1->SetLabelSize(0.03);
    palette1->SetLabelFont(42);
  }

  c1->SaveAs("deadHotEff_eta_pT.png");

  TCanvas* c2 = new TCanvas("c2", "Phi vs pT", 800, 600);
  c2->SetRightMargin(0.15);

  h_phi_pT->SetTitle(";p_{T} [GeV/c];#phi [rad];Acceptance");
  h_phi_pT->SetContour(99);
  h_phi_pT->Draw("COLZ");
  //h_phi_pT->Draw("TEXT same");

  gPad->Update();

  TPaletteAxis* palette2 = (TPaletteAxis*)h_phi_pT->GetListOfFunctions()->FindObject("palette");
  if (palette2)
  {
    palette2->SetLabelSize(0.03);
    palette2->SetLabelFont(42);
  }

  c2->SaveAs("deadHotEff_phi_pT.png"); 

  /*
  // Highest dead fee fraction (79529) 8%
  TFile* f_pionSim_high = TFile::Open("/sphenix/user/rosstom/analysis/LightFlavorRatios/bachelor_pi_efficiency/sim/run_79529_2M_pions.root", "READ");
  TTree* pionTree_high = (TTree*)f_pionSim_high->Get("ntp_gtrack");

  TH3F* h_truePions_high = new TH3F("h_truePions_high",";True #pi Track p_{T} [GeV/c];True #pi Track #eta;True #pi Track #phi",25,0,3,25,-1.1,1.1,25,-M_PI,M_PI);
  TH3F* h_recoPions_high = new TH3F("h_recoPions_high",";True #pi Track p_{T} [GeV/c];True #pi Track #eta;True #pi Track #phi",25,0,3,25,-1.1,1.1,25,-M_PI,M_PI);

  pionTree_high->Draw("gphi:geta:gpt>>h_truePions_high","abs(gvz) < 10 && gnmaps > 0 && gnintt > 0 && gntpc > 19");
  pionTree_high->Draw("gphi:geta:gpt>>h_recoPions_high","abs(gvz) < 10 && gnmaps > 0 && gnintt > 0 && gntpc > 19 && nmaps > 0 && nintt > 0 && ntpc > 19 && quality < 400");

  TH3F *h_pionEff_high = (TH3F*)h_recoPions_high->Clone("h_pionEff_high");
  h_pionEff_high->Divide(h_recoPions_high,h_truePions_high,1.0,1.0,"B");
  
  TH3F *h_deadHot_Eff_high = (TH3F*)h_pionEff_high->Clone("h_deadHot_Eff_high");
  h_deadHot_Eff_high->Divide(h_pionEff_high,h_pionEff_noDH,1.0,1.0);
   
  // Lowest dead fee fraction (79563) 3.5%
  TFile* f_pionSim_low = TFile::Open("/sphenix/user/rosstom/analysis/LightFlavorRatios/bachelor_pi_efficiency/sim/run_79563_2M_pions.root", "READ");
  TTree* pionTree_low = (TTree*)f_pionSim_low->Get("ntp_gtrack");

  TH3F* h_truePions_low = new TH3F("h_truePions_low",";True #pi Track p_{T} [GeV/c];True #pi Track #eta;True #pi Track #phi",25,0,3,25,-1.1,1.1,25,-M_PI,M_PI);
  TH3F* h_recoPions_low = new TH3F("h_recoPions_low",";True #pi Track p_{T} [GeV/c];True #pi Track #eta;True #pi Track #phi",25,0,3,25,-1.1,1.1,25,-M_PI,M_PI);

  pionTree_low->Draw("gphi:geta:gpt>>h_truePions_low","abs(gvz) < 10 && gnmaps > 0 && gnintt > 0 && gntpc > 19");
  pionTree_low->Draw("gphi:geta:gpt>>h_recoPions_low","abs(gvz) < 10 && gnmaps > 0 && gnintt > 0 && gntpc > 19 && nmaps > 0 && nintt > 0 && ntpc > 19 && quality < 400");

  TH3F *h_pionEff_low = (TH3F*)h_recoPions_low->Clone("h_pionEff_low");
  h_pionEff_low->Divide(h_recoPions_low,h_truePions_low,1.0,1.0,"B");
  
  TH3F *h_deadHot_Eff_low = (TH3F*)h_pionEff_low->Clone("h_deadHot_Eff_low");
  h_deadHot_Eff_low->Divide(h_pionEff_low,h_pionEff_noDH,1.0,1.0);

  // Simulated decays
  TFile* f_KshortSim = TFile::Open("/sphenix/user/cdean/software/analysis/LightFlavorRatios/geometric_acceptance/simulation/outputKFParticle_Kshort_reco.root");
  TTree* KshortTree = (TTree*)f_KshortSim->Get("DecayTree");
  
  TFile* f_cascadeSim = TFile::Open("/sphenix/user/rosstom/TrackingAnalysis/Physics_Val_TF/sim/pTComp/KFParticle_4Mcascade_nopTCut.root");
  TTree* cascadeTree = (TTree*)f_cascadeSim->Get("DecayTree");

  const int nBins_pT = 4;
  double pTbins[nBins_pT + 1] = {0.75, 1.07, 1.25, 1.49, 2.0};

  const int nBins_phi = 6;
  //double phibins[nBins_phi + 1] = {-M_PI,-2.14,-1.37,-0.61,0.35,2.21,M_PI};
  double phibins[nBins_phi + 1] = {-M_PI,-2.09439,-1.04719,0.0,1.04721,2.09441,M_PI};

  const int nBins_eta = 6;
  //double etabins[nBins_eta + 1] = {-1.1,-0.82,-0.56,-0.20,0.30,0.72,1.1};
  double etabins[nBins_eta + 1] = {-1.1,-0.733333,-0.366666,0.0,0.366668,0.733335,1.1};

  const int nBins_rap = 6;
  //double rapbins[nBins_rap + 1] = {-1.0,-0.64,-0.41,-0.15,0.21,0.55,1.0};
  double rapbins[nBins_rap + 1] = {-1.0,-0.66,-0.33,0.0,0.33,0.66,1.0};

  TH1F* h_pT_nTrueKS0_perfectAccept = new TH1F("h_pT_nTrueKS0_perfectAccept",";K_{S}^{0} p_{T} [GeV/c];Entries",nBins_pT,pTbins);  
  TH1F* h_phi_nTrueKS0_perfectAccept = new TH1F("h_phi_nTrueKS0_perfectAccept",";K_{S}^{0} #phi [rad];Entries",nBins_phi,phibins);  
  TH1F* h_eta_nTrueKS0_perfectAccept = new TH1F("h_eta_nTrueKS0_perfectAccept",";K_{S}^{0} #eta;Entries",nBins_eta,etabins);  
  TH1F* h_rap_nTrueKS0_perfectAccept = new TH1F("h_rap_nTrueKS0_perfectAccept",";K_{S}^{0} Rapidity;Entries",nBins_rap,rapbins);  
  TH1F* h_pT_nTrueXi_perfectAccept = new TH1F("h_pT_nTrueXi_perfectAccept",";#Xi p_{T} [GeV/c];Entries",nBins_pT,pTbins);  
  TH1F* h_phi_nTrueXi_perfectAccept = new TH1F("h_phi_nTrueXi_perfectAccept",";#Xi #phi [rad];Entries",nBins_phi,phibins);  
  TH1F* h_eta_nTrueXi_perfectAccept = new TH1F("h_eta_nTrueXi_perfectAccept",";#Xi #eta;Entries",nBins_eta,etabins);  
  TH1F* h_rap_nTrueXi_perfectAccept = new TH1F("h_rap_nTrueXi_perfectAccept",";#Xi Rapidity;Entries",nBins_rap,rapbins);  

  TH1F* h_pT_nTrueKS0_deadChannelAccept = new TH1F("h_pT_nTrueKS0_deadChannelAccept",";K_{S}^{0} p_{T} [GeV/c];Entries",nBins_pT,pTbins);  
  TH1F* h_phi_nTrueKS0_deadChannelAccept = new TH1F("h_phi_nTrueKS0_deadChannelAccept",";K_{S}^{0} #phi [rad];Entries",nBins_phi,phibins);  
  TH1F* h_eta_nTrueKS0_deadChannelAccept = new TH1F("h_eta_nTrueKS0_deadChannelAccept",";K_{S}^{0} #eta;Entries",nBins_eta,etabins);  
  TH1F* h_rap_nTrueKS0_deadChannelAccept = new TH1F("h_rap_nTrueKS0_deadChannelAccept",";K_{S}^{0} Rapidity;Entries",nBins_rap,rapbins);  
  TH1F* h_pT_nTrueXi_deadChannelAccept = new TH1F("h_pT_nTrueXi_deadChannelAccept",";#Xi p_{T} [GeV/c];Entries",nBins_pT,pTbins);  
  TH1F* h_phi_nTrueXi_deadChannelAccept = new TH1F("h_phi_nTrueXi_deadChannelAccept",";#Xi #phi [rad];Entries",nBins_phi,phibins);  
  TH1F* h_eta_nTrueXi_deadChannelAccept = new TH1F("h_eta_nTrueXi_deadChannelAccept",";#Xi #eta;Entries",nBins_eta,etabins);  
  TH1F* h_rap_nTrueXi_deadChannelAccept = new TH1F("h_rap_nTrueXi_deadChannelAccept",";#Xi Rapidity;Entries",nBins_rap,rapbins);    
  // highest fees out
  TH1F* h_pT_nTrueKS0_deadChannelAccept_high = new TH1F("h_pT_nTrueKS0_deadChannelAccept_high",";K_{S}^{0} p_{T} [GeV/c];Entries",nBins_pT,pTbins);  
  TH1F* h_phi_nTrueKS0_deadChannelAccept_high = new TH1F("h_phi_nTrueKS0_deadChannelAccept_high",";K_{S}^{0} #phi [rad];Entries",nBins_phi,phibins);  
  TH1F* h_eta_nTrueKS0_deadChannelAccept_high = new TH1F("h_eta_nTrueKS0_deadChannelAccept_high",";K_{S}^{0} #eta;Entries",nBins_eta,etabins);  
  TH1F* h_rap_nTrueKS0_deadChannelAccept_high = new TH1F("h_rap_nTrueKS0_deadChannelAccept_high",";K_{S}^{0} Rapidity;Entries",nBins_rap,rapbins);  
  TH1F* h_pT_nTrueXi_deadChannelAccept_high = new TH1F("h_pT_nTrueXi_deadChannelAccept_high",";#Xi p_{T} [GeV/c];Entries",nBins_pT,pTbins);  
  TH1F* h_phi_nTrueXi_deadChannelAccept_high = new TH1F("h_phi_nTrueXi_deadChannelAccept_high",";#Xi #phi [rad];Entries",nBins_phi,phibins);  
  TH1F* h_eta_nTrueXi_deadChannelAccept_high = new TH1F("h_eta_nTrueXi_deadChannelAccept_high",";#Xi #eta;Entries",nBins_eta,etabins);  
  TH1F* h_rap_nTrueXi_deadChannelAccept_high = new TH1F("h_rap_nTrueXi_deadChannelAccept_high",";#Xi Rapidity;Entries",nBins_rap,rapbins);    
  // lowest fees out
  TH1F* h_pT_nTrueKS0_deadChannelAccept_low = new TH1F("h_pT_nTrueKS0_deadChannelAccept_low",";K_{S}^{0} p_{T} [GeV/c];Entries",nBins_pT,pTbins);  
  TH1F* h_phi_nTrueKS0_deadChannelAccept_low = new TH1F("h_phi_nTrueKS0_deadChannelAccept_low",";K_{S}^{0} #phi [rad];Entries",nBins_phi,phibins);  
  TH1F* h_eta_nTrueKS0_deadChannelAccept_low = new TH1F("h_eta_nTrueKS0_deadChannelAccept_low",";K_{S}^{0} #eta;Entries",nBins_eta,etabins);  
  TH1F* h_rap_nTrueKS0_deadChannelAccept_low = new TH1F("h_rap_nTrueKS0_deadChannelAccept_low",";K_{S}^{0} Rapidity;Entries",nBins_rap,rapbins);  
  TH1F* h_pT_nTrueXi_deadChannelAccept_low = new TH1F("h_pT_nTrueXi_deadChannelAccept_low",";#Xi p_{T} [GeV/c];Entries",nBins_pT,pTbins);  
  TH1F* h_phi_nTrueXi_deadChannelAccept_low = new TH1F("h_phi_nTrueXi_deadChannelAccept_low",";#Xi #phi [rad];Entries",nBins_phi,phibins);  
  TH1F* h_eta_nTrueXi_deadChannelAccept_low = new TH1F("h_eta_nTrueXi_deadChannelAccept_low",";#Xi #eta;Entries",nBins_eta,etabins);  
  TH1F* h_rap_nTrueXi_deadChannelAccept_low = new TH1F("h_rap_nTrueXi_deadChannelAccept_low",";#Xi Rapidity;Entries",nBins_rap,rapbins);   
 
  int track_1_true_ID, track_2_true_ID;
  float track_1_pT, track_1_eta, track_1_phi;
  float track_2_pT, track_2_eta, track_2_phi;
  float track_1_true_px, track_1_true_py, track_1_true_pz;
  float track_2_true_px, track_2_true_py, track_2_true_pz;
  std::vector<int>* track_1_true_track_history_PDG_ID = nullptr;
  std::vector<int>* track_2_true_track_history_PDG_ID = nullptr;
  float K_S0_pT, K_S0_phi, K_S0_eta, K_S0_rapidity, primary_vertex_z;

  KshortTree->SetBranchAddress("track_1_true_ID", &track_1_true_ID);
  KshortTree->SetBranchAddress("track_2_true_ID", &track_2_true_ID);
  KshortTree->SetBranchAddress("track_1_true_px", &track_1_true_px);
  KshortTree->SetBranchAddress("track_2_true_px", &track_2_true_px);
  KshortTree->SetBranchAddress("track_1_true_py", &track_1_true_py);
  KshortTree->SetBranchAddress("track_2_true_py", &track_2_true_py);
  KshortTree->SetBranchAddress("track_1_true_pz", &track_1_true_pz);
  KshortTree->SetBranchAddress("track_2_true_pz", &track_2_true_pz);
  KshortTree->SetBranchAddress("track_1_pT", &track_1_pT);
  KshortTree->SetBranchAddress("track_2_pT", &track_2_pT);
  KshortTree->SetBranchAddress("track_1_pseudorapidity", &track_1_eta);
  KshortTree->SetBranchAddress("track_2_pseudorapidity", &track_2_eta);
  KshortTree->SetBranchAddress("track_1_phi", &track_1_phi);
  KshortTree->SetBranchAddress("track_2_phi", &track_2_phi);
  KshortTree->SetBranchAddress("track_1_true_track_history_PDG_ID", &track_1_true_track_history_PDG_ID);
  KshortTree->SetBranchAddress("track_2_true_track_history_PDG_ID", &track_2_true_track_history_PDG_ID);
  KshortTree->SetBranchAddress("K_S0_pT", &K_S0_pT);
  KshortTree->SetBranchAddress("K_S0_phi", &K_S0_phi);
  KshortTree->SetBranchAddress("K_S0_pseudorapidity", &K_S0_eta);
  KshortTree->SetBranchAddress("K_S0_rapidity", &K_S0_rapidity);
  KshortTree->SetBranchAddress("primary_vertex_z", &primary_vertex_z);

  std::random_device rd; 
  std::mt19937 gen(rd());
  std::uniform_real_distribution<double> dis(0.0, 1.0);

  std::cout << "Calculating KS0 Efficiency Corrections" << std::endl;
  //KS0 Efficiency Calculation
  unsigned int nEntries = KshortTree->GetEntries();
  for (int i = 0; i < nEntries; i++)
  {
    KshortTree->GetEntry(i);

    if (abs(primary_vertex_z) > 10 || abs(K_S0_rapidity) > 1.0) 
    {
      continue;
    } 
    if (abs(track_1_true_ID) != 211 || abs(track_2_true_ID) != 211)
    {
      continue;
    }
    bool track_1_KS0_mother = false;
    bool track_2_KS0_mother = false;
    for (int pdg : *track_1_true_track_history_PDG_ID)
    {
      if (abs(pdg) == 310)
      {
        track_1_KS0_mother = true;
        break;
      }
    }
    for (int pdg : *track_2_true_track_history_PDG_ID)
    {
      if (abs(pdg) == 310)
      {
        track_2_KS0_mother = true;
        break;
      }
    }
    if (track_1_KS0_mother && track_2_KS0_mother)
    {
      float pi_mass = TDatabasePDG::Instance()->GetParticle(track_1_true_ID)->Mass();
      PxPyPzMVector pi1_true_LV = PxPyPzMVector(track_1_true_px, track_1_true_py, track_1_true_pz, pi_mass);
      PxPyPzMVector pi2_true_LV = PxPyPzMVector(track_2_true_px, track_2_true_py, track_2_true_pz, pi_mass);

      float track_1_true_pT = pi1_true_LV.Pt();
      float track_1_true_eta = pi1_true_LV.Eta();
      float track_1_true_phi = pi1_true_LV.Phi();
      float track_2_true_pT = pi2_true_LV.Pt();
      float track_2_true_eta = pi2_true_LV.Eta();
      float track_2_true_phi = pi2_true_LV.Phi();

      PxPyPzMVector mother_true_LV = pi1_true_LV + pi2_true_LV;

      float KS0_true_pT = mother_true_LV.Pt();
      float KS0_true_eta = mother_true_LV.Eta();
      float KS0_true_rapidity = mother_true_LV.Rapidity();
      float KS0_true_phi = mother_true_LV.Phi();

      h_pT_nTrueKS0_perfectAccept->Fill(KS0_true_pT);
      h_phi_nTrueKS0_perfectAccept->Fill(KS0_true_phi);
      h_eta_nTrueKS0_perfectAccept->Fill(KS0_true_eta);
      h_rap_nTrueKS0_perfectAccept->Fill(KS0_true_rapidity);

      if (track_1_true_pT > 3.0) track_1_true_pT = 2.999;
      if (track_1_true_eta > 1.1) track_1_true_eta = 1.0999;
      if (track_1_true_eta < -1.1) track_1_true_eta = -1.0999;
      if (track_2_true_pT > 3.0) track_2_true_pT = 2.999;
      if (track_2_true_eta > 1.1) track_2_true_eta = 1.0999;
      if (track_2_true_eta < -1.1) track_2_true_eta = -1.0999;

      bool t1_accept = true;
      bool t2_accept = true;
      bool t1_accept_high = true;
      bool t2_accept_high = true;
      bool t1_accept_low = true;
      bool t2_accept_low = true;

      int binX = h_deadHot_Eff->GetXaxis()->FindBin(track_1_true_pT);
      int binY = h_deadHot_Eff->GetYaxis()->FindBin(track_1_true_eta);
      int binZ = h_deadHot_Eff->GetZaxis()->FindBin(track_1_true_phi);
      float eff = h_deadHot_Eff->GetBinContent(binX, binY, binZ);
      float eff_high = h_deadHot_Eff_high->GetBinContent(binX, binY, binZ);
      float eff_low = h_deadHot_Eff_low->GetBinContent(binX, binY, binZ);
      if (dis(gen) > eff)
      {
        t1_accept = false;
      }
      if (dis(gen) > eff_high)
      {
        t1_accept_high = false;
      }
      if (dis(gen) > eff_low)
      {
        t1_accept_low = false;
      }
      binX = h_deadHot_Eff->GetXaxis()->FindBin(track_2_true_pT);
      binY = h_deadHot_Eff->GetYaxis()->FindBin(track_2_true_eta);
      binZ = h_deadHot_Eff->GetZaxis()->FindBin(track_2_true_phi);
      eff = h_deadHot_Eff->GetBinContent(binX, binY, binZ);
      eff_high = h_deadHot_Eff_high->GetBinContent(binX, binY, binZ);
      eff_low = h_deadHot_Eff_low->GetBinContent(binX, binY, binZ);
      if (dis(gen) > eff)
      {
        t2_accept = false;
      }
      if (dis(gen) > eff_high)
      {
        t2_accept_high = false;
      }
      if (dis(gen) > eff_low)
      {
        t2_accept_low = false;
      }
      if (t1_accept && t2_accept)
      {
        h_pT_nTrueKS0_deadChannelAccept->Fill(KS0_true_pT);
        h_phi_nTrueKS0_deadChannelAccept->Fill(KS0_true_phi);
        h_eta_nTrueKS0_deadChannelAccept->Fill(KS0_true_eta);
        h_rap_nTrueKS0_deadChannelAccept->Fill(KS0_true_rapidity);
      }
      if (t1_accept_high && t2_accept_high)
      {
        h_pT_nTrueKS0_deadChannelAccept_high->Fill(KS0_true_pT);
        h_phi_nTrueKS0_deadChannelAccept_high->Fill(KS0_true_phi);
        h_eta_nTrueKS0_deadChannelAccept_high->Fill(KS0_true_eta);
        h_rap_nTrueKS0_deadChannelAccept_high->Fill(KS0_true_rapidity);
      }
      if (t1_accept && t2_accept)
      {
        h_pT_nTrueKS0_deadChannelAccept_low->Fill(KS0_true_pT);
        h_phi_nTrueKS0_deadChannelAccept_low->Fill(KS0_true_phi);
        h_eta_nTrueKS0_deadChannelAccept_low->Fill(KS0_true_eta);
        h_rap_nTrueKS0_deadChannelAccept_low->Fill(KS0_true_rapidity);
      }
    }
  }

  TH1F* h_trackAcc_pT_KS0 = (TH1F*)h_pT_nTrueKS0_deadChannelAccept->Clone("h_trackAcc_pT_KS0");
  TH1F* h_trackAcc_phi_KS0 = (TH1F*)h_phi_nTrueKS0_deadChannelAccept->Clone("h_trackAcc_phi_KS0"); 
  TH1F* h_trackAcc_eta_KS0 = (TH1F*)h_eta_nTrueKS0_deadChannelAccept->Clone("h_trackAcc_eta_KS0"); 
  TH1F* h_trackAcc_rap_KS0 = (TH1F*)h_rap_nTrueKS0_deadChannelAccept->Clone("h_trackAcc_rap_KS0"); 
  TH1F* h_trackAcc_pT_KS0_high = (TH1F*)h_pT_nTrueKS0_deadChannelAccept_high->Clone("h_trackAcc_pT_KS0_high"); 
  TH1F* h_trackAcc_phi_KS0_high = (TH1F*)h_phi_nTrueKS0_deadChannelAccept_high->Clone("h_trackAcc_phi_KS0_high"); 
  TH1F* h_trackAcc_eta_KS0_high = (TH1F*)h_eta_nTrueKS0_deadChannelAccept_high->Clone("h_trackAcc_eta_KS0_high"); 
  TH1F* h_trackAcc_rap_KS0_high = (TH1F*)h_rap_nTrueKS0_deadChannelAccept_high->Clone("h_trackAcc_rap_KS0_high"); 
  TH1F* h_trackAcc_pT_KS0_low = (TH1F*)h_pT_nTrueKS0_deadChannelAccept_low->Clone("h_trackAcc_pT_KS0_low"); 
  TH1F* h_trackAcc_phi_KS0_low = (TH1F*)h_phi_nTrueKS0_deadChannelAccept_low->Clone("h_trackAcc_phi_KS0_low"); 
  TH1F* h_trackAcc_eta_KS0_low = (TH1F*)h_eta_nTrueKS0_deadChannelAccept_low->Clone("h_trackAcc_eta_KS0_low"); 
  TH1F* h_trackAcc_rap_KS0_low = (TH1F*)h_rap_nTrueKS0_deadChannelAccept_low->Clone("h_trackAcc_rap_KS0_low"); 
  h_trackAcc_pT_KS0->Divide(h_trackAcc_pT_KS0, h_pT_nTrueKS0_perfectAccept, 1, 1, "B"); 
  h_trackAcc_phi_KS0->Divide(h_trackAcc_phi_KS0, h_phi_nTrueKS0_perfectAccept, 1, 1, "B"); 
  h_trackAcc_eta_KS0->Divide(h_trackAcc_eta_KS0, h_eta_nTrueKS0_perfectAccept, 1, 1, "B"); 
  h_trackAcc_rap_KS0->Divide(h_trackAcc_rap_KS0, h_rap_nTrueKS0_perfectAccept, 1, 1, "B"); 
  h_trackAcc_pT_KS0_high->Divide(h_trackAcc_pT_KS0_high, h_pT_nTrueKS0_perfectAccept, 1, 1, "B"); 
  h_trackAcc_phi_KS0_high->Divide(h_trackAcc_phi_KS0_high, h_phi_nTrueKS0_perfectAccept, 1, 1, "B"); 
  h_trackAcc_eta_KS0_high->Divide(h_trackAcc_eta_KS0_high, h_eta_nTrueKS0_perfectAccept, 1, 1, "B"); 
  h_trackAcc_rap_KS0_high->Divide(h_trackAcc_rap_KS0_high, h_rap_nTrueKS0_perfectAccept, 1, 1, "B"); 
  h_trackAcc_pT_KS0_low->Divide(h_trackAcc_pT_KS0_low, h_pT_nTrueKS0_perfectAccept, 1, 1, "B"); 
  h_trackAcc_phi_KS0_low->Divide(h_trackAcc_phi_KS0_low, h_phi_nTrueKS0_perfectAccept, 1, 1, "B"); 
  h_trackAcc_eta_KS0_low->Divide(h_trackAcc_eta_KS0_low, h_eta_nTrueKS0_perfectAccept, 1, 1, "B"); 
  h_trackAcc_rap_KS0_low->Divide(h_trackAcc_rap_KS0_low, h_rap_nTrueKS0_perfectAccept, 1, 1, "B"); 

  h_trackAcc_pT_KS0->GetYaxis()->SetTitle("K_{S}^{0} Tracking Acceptance");
  h_trackAcc_phi_KS0->GetYaxis()->SetTitle("K_{S}^{0} Tracking Acceptance");
  h_trackAcc_eta_KS0->GetYaxis()->SetTitle("K_{S}^{0} Tracking Acceptance");
  h_trackAcc_rap_KS0->GetYaxis()->SetTitle("K_{S}^{0} Tracking Acceptance");
  h_trackAcc_pT_KS0_high->GetYaxis()->SetTitle("K_{S}^{0} Tracking Acceptance");
  h_trackAcc_phi_KS0_high->GetYaxis()->SetTitle("K_{S}^{0} Tracking Acceptance");
  h_trackAcc_eta_KS0_high->GetYaxis()->SetTitle("K_{S}^{0} Tracking Acceptance");
  h_trackAcc_rap_KS0_high->GetYaxis()->SetTitle("K_{S}^{0} Tracking Acceptance");
  h_trackAcc_pT_KS0_low->GetYaxis()->SetTitle("K_{S}^{0} Tracking Acceptance");
  h_trackAcc_phi_KS0_low->GetYaxis()->SetTitle("K_{S}^{0} Tracking Acceptance");
  h_trackAcc_eta_KS0_low->GetYaxis()->SetTitle("K_{S}^{0} Tracking Acceptance");
  h_trackAcc_rap_KS0_low->GetYaxis()->SetTitle("K_{S}^{0} Tracking Acceptance");

  int Lambda0_track_1_true_ID, Lambda0_track_2_true_ID, track_3_true_ID;
  float Lambda0_track_1_pT, Lambda0_track_1_eta, Lambda0_track_1_phi;
  float Lambda0_track_2_pT, Lambda0_track_2_eta, Lambda0_track_2_phi;
  float track_3_pT, track_3_eta, track_3_phi;
  float Lambda0_track_1_true_px, Lambda0_track_1_true_py, Lambda0_track_1_true_pz;
  float Lambda0_track_2_true_px, Lambda0_track_2_true_py, Lambda0_track_2_true_pz;
  float track_3_true_px, track_3_true_py, track_3_true_pz;
  std::vector<int>* Lambda0_track_1_true_track_history_PDG_ID = nullptr;
  std::vector<int>* Lambda0_track_2_true_track_history_PDG_ID = nullptr;
  std::vector<int>* track_3_true_track_history_PDG_ID = nullptr;
  float Ximinus_pT, Ximinus_phi, Ximinus_eta, Ximinus_rapidity, primary_vertex_z_Xi;

  cascadeTree->SetBranchAddress("Lambda0_track_1_true_ID", &Lambda0_track_1_true_ID);
  cascadeTree->SetBranchAddress("Lambda0_track_2_true_ID", &Lambda0_track_2_true_ID);
  cascadeTree->SetBranchAddress("track_3_true_ID", &track_3_true_ID);
  cascadeTree->SetBranchAddress("Lambda0_track_1_pT", &Lambda0_track_1_pT);
  cascadeTree->SetBranchAddress("Lambda0_track_2_pT", &Lambda0_track_2_pT);
  cascadeTree->SetBranchAddress("track_3_pT", &track_3_pT);
  cascadeTree->SetBranchAddress("Lambda0_track_1_true_px", &Lambda0_track_1_true_px);
  cascadeTree->SetBranchAddress("Lambda0_track_2_true_px", &Lambda0_track_2_true_px);
  cascadeTree->SetBranchAddress("track_3_true_px", &track_3_true_px);
  cascadeTree->SetBranchAddress("Lambda0_track_1_true_py", &Lambda0_track_1_true_py);
  cascadeTree->SetBranchAddress("Lambda0_track_2_true_py", &Lambda0_track_2_true_py);
  cascadeTree->SetBranchAddress("track_3_true_py", &track_3_true_py);
  cascadeTree->SetBranchAddress("Lambda0_track_1_true_pz", &Lambda0_track_1_true_pz);
  cascadeTree->SetBranchAddress("Lambda0_track_2_true_pz", &Lambda0_track_2_true_pz);
  cascadeTree->SetBranchAddress("track_3_true_pz", &track_3_true_pz);
  cascadeTree->SetBranchAddress("Lambda0_track_1_pseudorapidity", &Lambda0_track_1_eta);
  cascadeTree->SetBranchAddress("Lambda0_track_2_pseudorapidity", &Lambda0_track_2_eta);
  cascadeTree->SetBranchAddress("track_3_pseudorapidity", &track_3_eta);
  cascadeTree->SetBranchAddress("Lambda0_track_1_phi", &Lambda0_track_1_phi);
  cascadeTree->SetBranchAddress("Lambda0_track_2_phi", &Lambda0_track_2_phi);
  cascadeTree->SetBranchAddress("track_3_phi", &track_3_phi);
  cascadeTree->SetBranchAddress("Lambda0_track_1_true_track_history_PDG_ID", &Lambda0_track_1_true_track_history_PDG_ID);
  cascadeTree->SetBranchAddress("Lambda0_track_2_true_track_history_PDG_ID", &Lambda0_track_2_true_track_history_PDG_ID);
  cascadeTree->SetBranchAddress("track_3_true_track_history_PDG_ID", &track_3_true_track_history_PDG_ID);
  cascadeTree->SetBranchAddress("Ximinus_pT", &Ximinus_pT);
  cascadeTree->SetBranchAddress("Ximinus_phi", &Ximinus_phi);
  cascadeTree->SetBranchAddress("Ximinus_pseudorapidity", &Ximinus_eta);
  cascadeTree->SetBranchAddress("Ximinus_rapidity", &Ximinus_rapidity);
  cascadeTree->SetBranchAddress("primary_vertex_z", &primary_vertex_z_Xi);

  std::cout << "Calculating Cascade Efficiency Corrections" << std::endl;
  //Ximinus Efficiency Calculation
  nEntries = cascadeTree->GetEntries();
  int n_PV_rap = 0;
  int n_track_IDs = 0;
  int n_track_1_history = 0;
  int n_track_2_history = 0;
  int n_track_3_history = 0;
  for (int i = 0; i < nEntries; i++)
  {
    cascadeTree->GetEntry(i);

    if (abs(primary_vertex_z_Xi) > 10 || abs(Ximinus_rapidity) > 1.0) 
    {
      n_PV_rap++;
      continue;
    } 
    if (abs(Lambda0_track_1_true_ID) != 211 || abs(Lambda0_track_2_true_ID) != 2212 || abs(track_3_true_ID) != 211 || Lambda0_track_1_true_ID != track_3_true_ID)
    {
      n_track_IDs++;
      continue;
    }
    bool track_1_Xi_mother = false;
    bool track_2_Xi_mother = false;
    bool track_3_Xi_mother = false;
    bool track_1_L0_mother = false;
    bool track_2_L0_mother = false;
    for (int pdg : *Lambda0_track_1_true_track_history_PDG_ID)
    {
      if (abs(pdg) == 3122)
      {
        track_1_L0_mother = true;
      }
      else if (abs(pdg) == 3312)
      {
        track_1_Xi_mother = true;
      }
    }
    for (int pdg : *Lambda0_track_2_true_track_history_PDG_ID)
    {
      if (abs(pdg) == 3122)
      {
        track_2_L0_mother = true;
      }
      else if (abs(pdg) == 3312)
      {
        track_2_Xi_mother = true;
      }
    }
    for (int pdg : *track_3_true_track_history_PDG_ID)
    {
      if (abs(pdg) == 3312)
      {
        track_3_Xi_mother = true;
      }
    }
    if (!(track_1_L0_mother && track_1_Xi_mother))
    {
      n_track_1_history++;
    }
    if (!(track_2_L0_mother && track_2_Xi_mother))
    {
      n_track_2_history++;
    }
    if (!(track_3_Xi_mother))
    {
      n_track_3_history++;
    }
    if (track_1_L0_mother && track_2_L0_mother && track_1_Xi_mother && track_2_Xi_mother && track_3_Xi_mother)
    {
      float pi_mass = TDatabasePDG::Instance()->GetParticle(track_3_true_ID)->Mass();
      float proton_mass = TDatabasePDG::Instance()->GetParticle(Lambda0_track_2_true_ID)->Mass();
      PxPyPzMVector pi1_true_LV = PxPyPzMVector(Lambda0_track_1_true_px, Lambda0_track_1_true_py, track_1_true_pz, pi_mass);
      PxPyPzMVector pro2_true_LV = PxPyPzMVector(Lambda0_track_2_true_px, Lambda0_track_2_true_py, Lambda0_track_2_true_pz, proton_mass);
      PxPyPzMVector pi3_true_LV = PxPyPzMVector(track_3_true_px, track_3_true_py, track_3_true_pz, pi_mass);

      float track_1_true_pT = pi1_true_LV.Pt();
      float track_1_true_eta = pi1_true_LV.Eta();
      float track_1_true_phi = pi1_true_LV.Phi();
      float track_2_true_pT = pro2_true_LV.Pt();
      float track_2_true_eta = pro2_true_LV.Eta();
      float track_2_true_phi = pro2_true_LV.Phi();
      float track_3_true_pT = pi3_true_LV.Pt();
      float track_3_true_eta = pi3_true_LV.Eta();
      float track_3_true_phi = pi3_true_LV.Phi();

      PxPyPzMVector mother_true_LV = pi1_true_LV + pro2_true_LV + pi3_true_LV;

      float Xi_true_pT = mother_true_LV.Pt();
      float Xi_true_eta = mother_true_LV.Eta();
      float Xi_true_rapidity = mother_true_LV.Rapidity();
      float Xi_true_phi = mother_true_LV.Phi(); 

      h_pT_nTrueXi_perfectAccept->Fill(Xi_true_pT);
      h_phi_nTrueXi_perfectAccept->Fill(Xi_true_phi);
      h_eta_nTrueXi_perfectAccept->Fill(Xi_true_eta);
      h_rap_nTrueXi_perfectAccept->Fill(Xi_true_rapidity);

      if (track_1_true_pT > 3.0) track_1_true_pT = 2.999;
      if (track_1_true_eta > 1.1) track_1_true_eta = 1.0999;
      if (track_1_true_eta < -1.1) track_1_true_eta = -1.0999;
      if (track_2_true_pT > 3.0) track_2_true_pT = 2.999;
      if (track_2_true_eta > 1.1) track_2_true_eta = 1.0999;
      if (track_2_true_eta < -1.1) track_2_true_eta = -1.0999;
      if (track_3_true_pT > 3.0) track_3_true_pT = 2.999;
      if (track_3_true_eta > 1.1) track_3_true_eta = 1.0999;
      if (track_3_true_eta < -1.1) track_3_true_eta = -1.0999;

      bool t1_accept = true;
      bool t2_accept = true;
      bool t3_accept = true;
      bool t1_accept_high = true;
      bool t2_accept_high = true;
      bool t3_accept_high = true;
      bool t1_accept_low = true;
      bool t2_accept_low = true;
      bool t3_accept_low = true;

      int binX = h_deadHot_Eff->GetXaxis()->FindBin(track_1_true_pT);
      int binY = h_deadHot_Eff->GetYaxis()->FindBin(track_1_true_eta);
      int binZ = h_deadHot_Eff->GetZaxis()->FindBin(track_1_true_phi);
      float eff = h_deadHot_Eff->GetBinContent(binX, binY, binZ);
      float eff_high = h_deadHot_Eff_high->GetBinContent(binX, binY, binZ);
      float eff_low = h_deadHot_Eff_low->GetBinContent(binX, binY, binZ);
      if (dis(gen) > eff)
      {
        t1_accept = false;
      }
      if (dis(gen) > eff_high)
      {
        t1_accept_high = false;
      }
      if (dis(gen) > eff_low)
      {
        t1_accept_low = false;
      }
      binX = h_deadHot_Eff->GetXaxis()->FindBin(track_2_true_pT);
      binY = h_deadHot_Eff->GetYaxis()->FindBin(track_2_true_eta);
      binZ = h_deadHot_Eff->GetZaxis()->FindBin(track_2_true_phi);
      eff = h_deadHot_Eff->GetBinContent(binX, binY, binZ);
      eff_high = h_deadHot_Eff_high->GetBinContent(binX, binY, binZ);
      eff_low = h_deadHot_Eff_low->GetBinContent(binX, binY, binZ);
      if (dis(gen) > eff)
      {
        t2_accept = false;
      }
      if (dis(gen) > eff_high)
      {
        t2_accept_high = false;
      }
      if (dis(gen) > eff_low)
      {
        t2_accept_low = false;
      }
      binX = h_deadHot_Eff->GetXaxis()->FindBin(track_3_true_pT);
      binY = h_deadHot_Eff->GetYaxis()->FindBin(track_3_true_eta);
      binZ = h_deadHot_Eff->GetZaxis()->FindBin(track_3_true_phi);
      eff = h_deadHot_Eff->GetBinContent(binX, binY, binZ);
      eff_high = h_deadHot_Eff_high->GetBinContent(binX, binY, binZ);
      eff_low = h_deadHot_Eff_low->GetBinContent(binX, binY, binZ);
      if (dis(gen) > eff)
      {
        t3_accept = false;
      }
      if (dis(gen) > eff_high)
      {
        t3_accept_high = false;
      }
      if (dis(gen) > eff_low)
      {
        t3_accept_low = false;
      }
     
      if (t1_accept && t2_accept && t3_accept)
      {
        h_pT_nTrueXi_deadChannelAccept->Fill(Xi_true_pT);
        h_phi_nTrueXi_deadChannelAccept->Fill(Xi_true_phi);
        h_eta_nTrueXi_deadChannelAccept->Fill(Xi_true_eta);
        h_rap_nTrueXi_deadChannelAccept->Fill(Xi_true_rapidity);
      }
      if (t1_accept_high && t2_accept_high && t3_accept_high)
      {
        h_pT_nTrueXi_deadChannelAccept_high->Fill(Xi_true_pT);
        h_phi_nTrueXi_deadChannelAccept_high->Fill(Xi_true_phi);
        h_eta_nTrueXi_deadChannelAccept_high->Fill(Xi_true_eta);
        h_rap_nTrueXi_deadChannelAccept_high->Fill(Xi_true_rapidity);
      }
      if (t1_accept_low && t2_accept_low && t3_accept_low)
      {
        h_pT_nTrueXi_deadChannelAccept_low->Fill(Xi_true_pT);
        h_phi_nTrueXi_deadChannelAccept_low->Fill(Xi_true_phi);
        h_eta_nTrueXi_deadChannelAccept_low->Fill(Xi_true_eta);
        h_rap_nTrueXi_deadChannelAccept_low->Fill(Xi_true_rapidity);
      }
    }
  }

  std::cout << "Fails PV or rapidity cut: " << n_PV_rap << std::endl;
  std::cout << "Fails track ID cut: " << n_track_IDs << std::endl;
  std::cout << "Fails Track 1 history: " << n_track_1_history << std::endl;
  std::cout << "Fails Track 2 history: " << n_track_2_history << std::endl; 
  std::cout << "Fails Track 3 history: " << n_track_3_history << std::endl; 

  TH1F* h_trackAcc_pT_Xi = (TH1F*)h_pT_nTrueXi_deadChannelAccept->Clone("h_trackAcc_pT_Xi"); 
  TH1F* h_trackAcc_phi_Xi = (TH1F*)h_phi_nTrueXi_deadChannelAccept->Clone("h_trackAcc_phi_Xi"); 
  TH1F* h_trackAcc_eta_Xi = (TH1F*)h_eta_nTrueXi_deadChannelAccept->Clone("h_trackAcc_eta_Xi"); 
  TH1F* h_trackAcc_rap_Xi = (TH1F*)h_rap_nTrueXi_deadChannelAccept->Clone("h_trackAcc_rap_Xi"); 
  TH1F* h_trackAcc_pT_Xi_high = (TH1F*)h_pT_nTrueXi_deadChannelAccept_high->Clone("h_trackAcc_pT_Xi_high"); 
  TH1F* h_trackAcc_phi_Xi_high = (TH1F*)h_phi_nTrueXi_deadChannelAccept_high->Clone("h_trackAcc_phi_Xi_high"); 
  TH1F* h_trackAcc_eta_Xi_high = (TH1F*)h_eta_nTrueXi_deadChannelAccept_high->Clone("h_trackAcc_eta_Xi_high"); 
  TH1F* h_trackAcc_rap_Xi_high = (TH1F*)h_rap_nTrueXi_deadChannelAccept_high->Clone("h_trackAcc_rap_Xi_high"); 
  TH1F* h_trackAcc_pT_Xi_low = (TH1F*)h_pT_nTrueXi_deadChannelAccept_low->Clone("h_trackAcc_pT_Xi_low"); 
  TH1F* h_trackAcc_phi_Xi_low = (TH1F*)h_phi_nTrueXi_deadChannelAccept_low->Clone("h_trackAcc_phi_Xi_low"); 
  TH1F* h_trackAcc_eta_Xi_low = (TH1F*)h_eta_nTrueXi_deadChannelAccept_low->Clone("h_trackAcc_eta_Xi_low"); 
  TH1F* h_trackAcc_rap_Xi_low = (TH1F*)h_rap_nTrueXi_deadChannelAccept_low->Clone("h_trackAcc_rap_Xi_low"); 
  h_trackAcc_pT_Xi->Divide(h_trackAcc_pT_Xi, h_pT_nTrueXi_perfectAccept, 1, 1, "B"); 
  h_trackAcc_phi_Xi->Divide(h_trackAcc_phi_Xi, h_phi_nTrueXi_perfectAccept, 1, 1, "B"); 
  h_trackAcc_eta_Xi->Divide(h_trackAcc_eta_Xi, h_eta_nTrueXi_perfectAccept, 1, 1, "B"); 
  h_trackAcc_rap_Xi->Divide(h_trackAcc_rap_Xi, h_rap_nTrueXi_perfectAccept, 1, 1, "B"); 
  h_trackAcc_pT_Xi_high->Divide(h_trackAcc_pT_Xi_high, h_pT_nTrueXi_perfectAccept, 1, 1, "B"); 
  h_trackAcc_phi_Xi_high->Divide(h_trackAcc_phi_Xi_high, h_phi_nTrueXi_perfectAccept, 1, 1, "B"); 
  h_trackAcc_eta_Xi_high->Divide(h_trackAcc_eta_Xi_high, h_eta_nTrueXi_perfectAccept, 1, 1, "B"); 
  h_trackAcc_rap_Xi_high->Divide(h_trackAcc_rap_Xi_high, h_rap_nTrueXi_perfectAccept, 1, 1, "B"); 
  h_trackAcc_pT_Xi_low->Divide(h_trackAcc_pT_Xi_low, h_pT_nTrueXi_perfectAccept, 1, 1, "B"); 
  h_trackAcc_phi_Xi_low->Divide(h_trackAcc_phi_Xi_low, h_phi_nTrueXi_perfectAccept, 1, 1, "B"); 
  h_trackAcc_eta_Xi_low->Divide(h_trackAcc_eta_Xi_low, h_eta_nTrueXi_perfectAccept, 1, 1, "B"); 
  h_trackAcc_rap_Xi_low->Divide(h_trackAcc_rap_Xi_low, h_rap_nTrueXi_perfectAccept, 1, 1, "B"); 
  
  h_trackAcc_pT_Xi->GetYaxis()->SetTitle("#Xi^{-} Tracking Acceptance");
  h_trackAcc_phi_Xi->GetYaxis()->SetTitle("#Xi^{-} Tracking Acceptance");
  h_trackAcc_eta_Xi->GetYaxis()->SetTitle("#Xi^{-} Tracking Acceptance");
  h_trackAcc_rap_Xi->GetYaxis()->SetTitle("#Xi^{-} Tracking Acceptance");
  h_trackAcc_pT_Xi_high->GetYaxis()->SetTitle("#Xi^{-} Tracking Acceptance");
  h_trackAcc_phi_Xi_high->GetYaxis()->SetTitle("#Xi^{-} Tracking Acceptance");
  h_trackAcc_eta_Xi_high->GetYaxis()->SetTitle("#Xi^{-} Tracking Acceptance");
  h_trackAcc_rap_Xi_high->GetYaxis()->SetTitle("#Xi^{-} Tracking Acceptance");
  h_trackAcc_pT_Xi_low->GetYaxis()->SetTitle("#Xi^{-} Tracking Acceptance");
  h_trackAcc_phi_Xi_low->GetYaxis()->SetTitle("#Xi^{-} Tracking Acceptance");
  h_trackAcc_eta_Xi_low->GetYaxis()->SetTitle("#Xi^{-} Tracking Acceptance");
  h_trackAcc_rap_Xi_low->GetYaxis()->SetTitle("#Xi^{-} Tracking Acceptance");

  TH1F* h_pT_corrections = (TH1F*)h_trackAcc_pT_Xi->Clone("h_pT_corrections");
  TH1F* h_phi_corrections = (TH1F*)h_trackAcc_phi_Xi->Clone("h_phi_corrections");
  TH1F* h_eta_corrections = (TH1F*)h_trackAcc_eta_Xi->Clone("h_eta_corrections");
  TH1F* h_rap_corrections = (TH1F*)h_trackAcc_rap_Xi->Clone("h_rap_corrections");
  h_pT_corrections->Divide(h_pT_corrections, h_trackAcc_pT_KS0, 1, 1);
  h_phi_corrections->Divide(h_phi_corrections, h_trackAcc_phi_KS0, 1, 1);
  h_eta_corrections->Divide(h_eta_corrections, h_trackAcc_eta_KS0, 1, 1);
  h_rap_corrections->Divide(h_rap_corrections, h_trackAcc_rap_KS0, 1, 1);
  TH1F* h_pT_corrections_high = (TH1F*)h_trackAcc_pT_Xi_high->Clone("h_pT_corrections_high");
  TH1F* h_phi_corrections_high = (TH1F*)h_trackAcc_phi_Xi_high->Clone("h_phi_corrections_high");
  TH1F* h_eta_corrections_high = (TH1F*)h_trackAcc_eta_Xi_high->Clone("h_eta_corrections_high");
  TH1F* h_rap_corrections_high = (TH1F*)h_trackAcc_rap_Xi_high->Clone("h_rap_corrections_high");
  h_pT_corrections_high->Divide(h_pT_corrections_high, h_trackAcc_pT_KS0_high, 1, 1);
  h_phi_corrections_high->Divide(h_phi_corrections_high, h_trackAcc_phi_KS0_high, 1, 1);
  h_eta_corrections_high->Divide(h_eta_corrections_high, h_trackAcc_eta_KS0_high, 1, 1);
  h_rap_corrections_high->Divide(h_rap_corrections_high, h_trackAcc_rap_KS0_high, 1, 1);
  TH1F* h_pT_corrections_low = (TH1F*)h_trackAcc_pT_Xi_low->Clone("h_pT_corrections_low");
  TH1F* h_phi_corrections_low = (TH1F*)h_trackAcc_phi_Xi_low->Clone("h_phi_corrections_low");
  TH1F* h_eta_corrections_low = (TH1F*)h_trackAcc_eta_Xi_low->Clone("h_eta_corrections_low");
  TH1F* h_rap_corrections_low = (TH1F*)h_trackAcc_rap_Xi_low->Clone("h_rap_corrections_low");
  h_pT_corrections_low->Divide(h_pT_corrections_low, h_trackAcc_pT_KS0_low, 1, 1);
  h_phi_corrections_low->Divide(h_phi_corrections_low, h_trackAcc_phi_KS0_low, 1, 1);
  h_eta_corrections_low->Divide(h_eta_corrections_low, h_trackAcc_eta_KS0_low, 1, 1);
  h_rap_corrections_low->Divide(h_rap_corrections_low, h_trackAcc_rap_KS0_low, 1, 1);

  h_pT_corrections->GetXaxis()->SetTitle("Mother p_{T} [GeV/c]");
  h_pT_corrections->GetYaxis()->SetTitle("(#Xi^{-}+#Xi^{+})/2K^{0}_{S} Tracking Acceptance Ratio");
  h_phi_corrections->GetXaxis()->SetTitle("Mother #phi [rad]");
  h_phi_corrections->GetYaxis()->SetTitle("(#Xi^{-}+#Xi^{+})/2K^{0}_{S} Tracking Acceptance Ratio");
  h_eta_corrections->GetXaxis()->SetTitle("Mother #eta");
  h_eta_corrections->GetYaxis()->SetTitle("(#Xi^{-}+#Xi^{+})/2K^{0}_{S} Tracking Acceptance Ratio");
  h_rap_corrections->GetXaxis()->SetTitle("Mother y");
  h_rap_corrections->GetYaxis()->SetTitle("(#Xi^{-}+#Xi^{+})/2K^{0}_{S} Tracking Acceptance Ratio");
  h_pT_corrections_high->GetXaxis()->SetTitle("Mother p_{T} [GeV/c]");
  h_pT_corrections_high->GetYaxis()->SetTitle("(#Xi^{-}+#Xi^{+})/2K^{0}_{S} Tracking Acceptance Ratio");
  h_phi_corrections_high->GetXaxis()->SetTitle("Mother #phi [rad]");
  h_phi_corrections_high->GetYaxis()->SetTitle("(#Xi^{-}+#Xi^{+})/2K^{0}_{S} Tracking Acceptance Ratio");
  h_eta_corrections_high->GetXaxis()->SetTitle("Mother #eta");
  h_eta_corrections_high->GetYaxis()->SetTitle("(#Xi^{-}+#Xi^{+})/2K^{0}_{S} Tracking Acceptance Ratio");
  h_rap_corrections_high->GetXaxis()->SetTitle("Mother y");
  h_rap_corrections_high->GetYaxis()->SetTitle("(#Xi^{-}+#Xi^{+})/2K^{0}_{S} Tracking Acceptance Ratio");
  h_pT_corrections_low->GetXaxis()->SetTitle("Mother p_{T} [GeV/c]");
  h_pT_corrections_low->GetYaxis()->SetTitle("(#Xi^{-}+#Xi^{+})/2K^{0}_{S} Tracking Acceptance Ratio");
  h_phi_corrections_low->GetXaxis()->SetTitle("Mother #phi [rad]");
  h_phi_corrections_low->GetYaxis()->SetTitle("(#Xi^{-}+#Xi^{+})/2K^{0}_{S} Tracking Acceptance Ratio");
  h_eta_corrections_low->GetXaxis()->SetTitle("Mother #eta");
  h_eta_corrections_low->GetYaxis()->SetTitle("(#Xi^{-}+#Xi^{+})/2K^{0}_{S} Tracking Acceptance Ratio");
  h_rap_corrections_low->GetXaxis()->SetTitle("Mother y");
  h_rap_corrections_low->GetYaxis()->SetTitle("(#Xi^{-}+#Xi^{+})/2K^{0}_{S} Tracking Acceptance Ratio");

  std::cout << "Saving output histograms" << std::endl;  

  auto SaveMaxDeviationHist = [](TH1F* hNom, TH1F* hMax, TH1F* hMin, const std::string& saveName, TFile* outFile)
  {
    std::string histName = saveName + "_maxDeviation";
    TH1F* hDev = (TH1F*)hNom->Clone(histName.c_str());
    hDev->Reset();
    hDev->GetYaxis()->SetTitle("Max Deviation Bin Content");

    for (int i = 1; i <= hNom->GetNbinsX(); i++)
    {
        double nom  = hNom->GetBinContent(i);
        double max  = hMax->GetBinContent(i);
        double min  = hMin->GetBinContent(i);

        double diffUp   = std::abs(max - nom);
        double diffDown = std::abs(min - nom);

        // Store the bin content of whichever deviated furthest
        double maxDevContent = (diffUp >= diffDown) ? max : min;
        double maxDevError   = (diffUp >= diffDown) ? hMax->GetBinError(i) : hMin->GetBinError(i);

        hDev->SetBinContent(i, maxDevContent);
        hDev->SetBinError(i, maxDevError);
    }

    outFile->cd();
    hDev->Write();
    delete hDev;
  };

  TFile* fout = new TFile("trackingAcceptanceEfficiencyCorrections_withSystematics.root","RECREATE");
  h_trackAcc_pT_KS0->Write();
  h_trackAcc_phi_KS0->Write();
  h_trackAcc_eta_KS0->Write();
  h_trackAcc_rap_KS0->Write();
  h_trackAcc_pT_Xi->Write();
  h_trackAcc_phi_Xi->Write();
  h_trackAcc_eta_Xi->Write();
  h_trackAcc_rap_Xi->Write();
  h_pT_corrections->Write();
  h_phi_corrections->Write();
  h_eta_corrections->Write();
  h_rap_corrections->Write();
  h_trackAcc_pT_KS0_high->Write();
  h_trackAcc_phi_KS0_high->Write();
  h_trackAcc_eta_KS0_high->Write();
  h_trackAcc_rap_KS0_high->Write();
  h_trackAcc_pT_Xi_high->Write();
  h_trackAcc_phi_Xi_high->Write();
  h_trackAcc_eta_Xi_high->Write();
  h_trackAcc_rap_Xi_high->Write();
  h_pT_corrections_high->Write();
  h_phi_corrections_high->Write();
  h_eta_corrections_high->Write();
  h_rap_corrections_high->Write();
  h_trackAcc_pT_KS0_low->Write();
  h_trackAcc_phi_KS0_low->Write();
  h_trackAcc_eta_KS0_low->Write();
  h_trackAcc_rap_KS0_low->Write();
  h_trackAcc_pT_Xi_low->Write();
  h_trackAcc_phi_Xi_low->Write();
  h_trackAcc_eta_Xi_low->Write();
  h_trackAcc_rap_Xi_low->Write();
  h_pT_corrections_low->Write();
  h_phi_corrections_low->Write();
  h_eta_corrections_low->Write();
  h_rap_corrections_low->Write();
  */
  /*
  SaveMaxDeviationHist(h_Xi_eff_pT,  h_Xi_eff_pT_high,  h_Xi_eff_pT_low,  "Xi_pT",  fout);
  SaveMaxDeviationHist(h_Xi_eff_eta, h_Xi_eff_eta_high, h_Xi_eff_eta_low, "Xi_eta", fout);
  SaveMaxDeviationHist(h_Xi_eff_phi, h_Xi_eff_phi_high, h_Xi_eff_phi_low, "Xi_phi", fout);
  SaveMaxDeviationHist(h_Xi_eff_rap, h_Xi_eff_rap_high, h_Xi_eff_rap_low, "Xi_y",   fout);
  SaveMaxDeviationHist(h_KS0_eff_pT,  h_KS0_eff_pT_high,  h_KS0_eff_pT_low,  "KS0_pT",  fout);
  SaveMaxDeviationHist(h_KS0_eff_eta, h_KS0_eff_eta_high, h_KS0_eff_eta_low, "KS0_eta", fout);
  SaveMaxDeviationHist(h_KS0_eff_phi, h_KS0_eff_phi_high, h_KS0_eff_phi_low, "KS0_phi", fout);
  SaveMaxDeviationHist(h_KS0_eff_rap, h_KS0_eff_rap_high, h_KS0_eff_rap_low, "KS0_y",   fout);
  */

  //fout->Close();

  std::cout << "Completed" << std::endl;  
}
