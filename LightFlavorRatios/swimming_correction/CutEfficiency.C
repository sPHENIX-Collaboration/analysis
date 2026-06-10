#include "../util/binning.h"

std::string getDate()
{
    std::time_t t = std::time(0);   // get time now
    std::tm* now = std::localtime(&t);

    std::stringstream date;
    date << (now->tm_mon + 1) << '/'
         <<  now->tm_mday << '/'
         << (now->tm_year + 1900);
    return date.str();
}

/*
void CutEfficiency()
{
  string sim_inFile_Xi = "/sphenix/user/rosstom/TrackingAnalysis/Physics_Val_TF/sim/pTComp/KFParticle_2Mcascade_nopTCut.root";
  TFile* sim_file_Xi = new TFile(sim_inFile_Xi.c_str());
  TTree* sim_tree_Xi = (TTree*)sim_file_Xi->Get("DecayTree");

  const int nBins_pT = 5;
  double pTbins[nBins_pT + 1] = {0.75, 0.909375, 1.05792, 1.23212, 1.46559, 2.0};

  const int nBins_phi = 10;
  double phibins[nBins_phi + 1] = {-M_PI,-2.51177,-2.0196,-1.55623,-1.0106,-0.530045,0.0583016,1.05245,2.12079,2.59601,M_PI};

  const int nBins_eta = 10;
  double etabins[nBins_eta + 1] = {-1.1,-0.916345,-0.755196,-0.585922,-0.390321,-0.159045,0.122585,0.394623,0.637892,0.849511,1.1};

  std::string truthCut = "abs(Lambda0_track_1_true_ID) == 211 && Sum$(abs(Lambda0_track_1_true_track_history_PDG_ID) == 3122) > 0 && Sum$(abs(Lambda0_track_1_true_track_history_PDG_ID) == 3312) > 0 && abs(Lambda0_track_2_true_ID) == 2212 && Sum$(abs(Lambda0_track_2_true_track_history_PDG_ID) == 3122) > 0 && Sum$(abs(Lambda0_track_2_true_track_history_PDG_ID) == 3312) > 0 && abs(track_3_true_ID) == 211 && Sum$(abs(track_3_true_track_history_PDG_ID) == 3312) > 0 && Sum$(abs(track_3_true_track_history_PDG_ID) == 3122) == 0 && abs(Ximinus_rapidity) <= 1.0 && Lambda0_track_1_MVTX_nHits > 0 && Lambda0_track_2_MVTX_nHits > 0 && track_3_MVTX_nHits > 0";
  //std::string truthDataCut = "abs(Lambda0_track_1_true_ID) == 211 && Sum$(abs(Lambda0_track_1_true_track_history_PDG_ID) == 3122) > 0 && Sum$(abs(Lambda0_track_1_true_track_history_PDG_ID) == 3312) > 0 && abs(Lambda0_track_2_true_ID) == 2212 && Sum$(abs(Lambda0_track_2_true_track_history_PDG_ID) == 3122) > 0 && Sum$(abs(Lambda0_track_2_true_track_history_PDG_ID) == 3312) > 0 && abs(track_3_true_ID) == 211 && Sum$(abs(track_3_true_track_history_PDG_ID) == 3312) > 0 && Sum$(abs(track_3_true_track_history_PDG_ID) == 3122) == 0 && Lambda0_track_1_MVTX_nStates > 0 && Lambda0_track_2_MVTX_nStates > 0 && track_3_MVTX_nStates > 0 && Lambda0_track_1_INTT_nStates > 0 && Lambda0_track_2_INTT_nStates > 0 && track_3_INTT_nStates > 0 && Lambda0_track_1_TPC_nStates > 19 && Lambda0_track_2_TPC_nStates > 19 && track_3_TPC_nStates > 19 && abs(Lambda0_mass - 1.1157) <= 0.01 && Ximinus_decayLength_xy >= 0.15 && Lambda0_decayLength_xy >= 0.01 && (Lambda0_track_1_chi2/Lambda0_track_1_nDoF) <= 400 && (Lambda0_track_2_chi2/Lambda0_track_2_nDoF) <= 400 && (track_3_chi2/track_3_nDoF) <= 400 && track_1_track_2_DCA <= 0.5 && Ximinus_chi2/Ximinus_nDoF <= 50 && track_1_track_3_DCA <= 0.5 && track_2_track_3_DCA <= 0.5 && Lambda0_chi2/Lambda0_nDoF <= 50";
  std::string truthDataCut = "abs(Lambda0_track_1_true_ID) == 211 && Sum$(abs(Lambda0_track_1_true_track_history_PDG_ID) == 3122) > 0 && Sum$(abs(Lambda0_track_1_true_track_history_PDG_ID) == 3312) > 0 && abs(Lambda0_track_2_true_ID) == 2212 && Sum$(abs(Lambda0_track_2_true_track_history_PDG_ID) == 3122) > 0 && Sum$(abs(Lambda0_track_2_true_track_history_PDG_ID) == 3312) > 0 && abs(track_3_true_ID) == 211 && Sum$(abs(track_3_true_track_history_PDG_ID) == 3312) > 0 && Sum$(abs(track_3_true_track_history_PDG_ID) == 3122) == 0 && Lambda0_track_1_MVTX_nHits > 0 && Lambda0_track_2_MVTX_nHits > 0 && track_3_MVTX_nHits > 0 && Lambda0_track_1_INTT_nHits > 0 && Lambda0_track_2_INTT_nHits > 0 && track_3_INTT_nHits > 0 && Lambda0_track_1_TPC_nHits > 19 && Lambda0_track_2_TPC_nHits > 19 && track_3_TPC_nHits > 19 && abs(Lambda0_mass - 1.1157) <= 0.01 && Ximinus_decayLength_xy >= 0.15 && Lambda0_decayLength_xy >= 0.01 && (Lambda0_track_1_chi2/Lambda0_track_1_nDoF) <= 400 && (Lambda0_track_2_chi2/Lambda0_track_2_nDoF) <= 400 && (track_3_chi2/track_3_nDoF) <= 400 && track_1_track_2_DCA <= 0.5 && track_1_track_3_DCA <= 0.5 && track_2_track_3_DCA <= 0.5 && Ximinus_chi2/Ximinus_nDoF <= 50 && Lambda0_chi2/Lambda0_nDoF <= 50 && abs(Ximinus_rapidity) <= 1.0";

  TH1F* h_pT_truth = new TH1F("h_pT_truth",";#Xi^{-} p_{T} [GeV/c];Cut Efficiency",nBins_pT,pTbins);
  TH1F* h_pT_truthCuts = new TH1F("h_pT_truthCuts",";#Xi^{-} p_{T} [GeV/c];Cut Efficiency",nBins_pT,pTbins);
  
  TH1F* h_eta_truth = new TH1F("h_eta_truth",";#Xi^{-} #eta;Cut Efficiency",nBins_eta,etabins);
  TH1F* h_eta_truthCuts = new TH1F("h_eta_truthCuts",";#Xi^{-} #eta;Cut Efficiency",nBins_eta,etabins);
  
  TH1F* h_phi_truth = new TH1F("h_phi_truth",";#Xi^{-} #phi;Cut Efficiency",nBins_phi,phibins);
  TH1F* h_phi_truthCuts = new TH1F("h_phi_truthCuts",";#Xi^{-} #phi;Cut Efficiency",nBins_phi,phibins);
  
  TH1F* h_y_truth = new TH1F("h_y_truth",";#Xi^{-} y;Cut Efficiency",10,-1.0,1.0);
  TH1F* h_y_truthCuts = new TH1F("h_y_truthCuts",";#Xi^{-} y;Cut Efficiency",10,-1.0,1.0);
   
  h_pT_truth->Sumw2();
  h_pT_truthCuts->Sumw2();
  h_eta_truth->Sumw2();
  h_eta_truthCuts->Sumw2();
  h_phi_truth->Sumw2();
  h_phi_truthCuts->Sumw2();
  h_y_truth->Sumw2();
  h_y_truthCuts->Sumw2();

  sim_tree_Xi->Draw("Ximinus_pT>>+h_pT_truth",truthCut.c_str());
  sim_tree_Xi->Draw("Ximinus_pT>>+h_pT_truthCuts",truthDataCut.c_str());
  sim_tree_Xi->Draw("Ximinus_pseudorapidity>>+h_eta_truth",truthCut.c_str());
  sim_tree_Xi->Draw("Ximinus_pseudorapidity>>+h_eta_truthCuts",truthDataCut.c_str());
  sim_tree_Xi->Draw("Ximinus_phi>>+h_phi_truth",truthCut.c_str());
  sim_tree_Xi->Draw("Ximinus_phi>>+h_phi_truthCuts",truthDataCut.c_str());
  sim_tree_Xi->Draw("Ximinus_rapidity>>+h_y_truth",truthCut.c_str());
  sim_tree_Xi->Draw("Ximinus_rapidity>>+h_y_truthCuts",truthDataCut.c_str());

  TEfficiency* h_pT_eff = new TEfficiency(*h_pT_truthCuts, *h_pT_truth);
  TEfficiency* h_eta_eff = new TEfficiency(*h_eta_truthCuts, *h_eta_truth);
  TEfficiency* h_phi_eff = new TEfficiency(*h_phi_truthCuts, *h_phi_truth);
  TEfficiency* h_y_eff = new TEfficiency(*h_y_truthCuts, *h_y_truth);
  h_pT_eff->SetStatisticOption(TEfficiency::kFCP);
  h_pT_eff->SetMarkerStyle(20);
  h_pT_eff->SetMarkerSize(1.2);
  h_eta_eff->SetStatisticOption(TEfficiency::kFCP);
  h_eta_eff->SetMarkerStyle(20);
  h_eta_eff->SetMarkerSize(1.2);
  h_phi_eff->SetStatisticOption(TEfficiency::kFCP);
  h_phi_eff->SetMarkerStyle(20);
  h_phi_eff->SetMarkerSize(1.2);
  h_y_eff->SetStatisticOption(TEfficiency::kFCP);
  h_y_eff->SetMarkerStyle(20);
  h_y_eff->SetMarkerSize(1.2);
  //h_pT_eff->GetXaxis()->SetTitle("#Xi^{-} p_{T} [GeV/c]");
  //h_pT_eff->GetYaxis()->SetTitle("Cut Efficiency");  

  TCanvas *c1  = new TCanvas("myCanvas", "myCanvas",1600,1600);
  c1->Divide(2,2);
  //pT 
  c1->cd(1);
  h_pT_eff->Draw("AP");
  c1->Update();

  TGraph* g = h_pT_eff->GetPaintedGraph();
  if (g)
  {
    g->GetXaxis()->SetTitle("#Xi^{-} p_{T} [GeV/c]");
    //g->GetXaxis()->SetTitle("#Xi^{-} #eta");
    //g->GetXaxis()->SetTitle("#Xi^{-} #phi");
    //g->GetXaxis()->SetTitle("#Xi^{-} y");
    g->GetYaxis()->SetTitle("Cut Efficiency");
    g->GetYaxis()->SetRangeUser(0.0, 1.05);
    g->GetXaxis()->SetLimits(pTbins[0], pTbins[nBins_pT]);
    //g->GetXaxis()->SetLimits(etabins[0], etabins[nBins_eta]);
    //g->GetXaxis()->SetLimits(phibins[0], phibins[nBins_phi]);
    //g->GetXaxis()->SetLimits(-1.5,1.5);
  }
  h_pT_eff->Draw("AP"); 
 
  TPaveText *pt;
  pt = new TPaveText(0.15,0.75,0.65,1., "NDC");
  pt->SetFillColor(0);
  pt->SetFillStyle(0);
  pt->SetTextFont(42);
  string label = "#it{#bf{sPHENIX}} Simulation";
  TText *pt_LaTex = pt->AddText(label.c_str());
  pt->SetBorderSize(0);
  pt->Draw();
  gPad->Modified();

  TPaveText *ptDate;
  ptDate = new TPaveText(0.67,0.79,0.85,0.95, "NDC");
  ptDate->SetFillColor(0);
  ptDate->SetFillStyle(0);
  ptDate->SetTextFont(42);
  std::string compilation_date = getDate();
  TText *pt_LaTexDate = ptDate->AddText(compilation_date.c_str());
  ptDate->SetBorderSize(0);
  ptDate->Draw();
  gPad->Modified();
 
  //eta
  c1->cd(2);
  h_eta_eff->Draw("AP");
  c1->Update();

  g = h_eta_eff->GetPaintedGraph();
  if (g)
  {
    g->GetXaxis()->SetTitle("#Xi^{-} #eta");
    g->GetYaxis()->SetTitle("Cut Efficiency");
    g->GetYaxis()->SetRangeUser(0.0, 1.05);
    g->GetXaxis()->SetLimits(etabins[0], etabins[nBins_eta]);
  }
  h_eta_eff->Draw("AP"); 
 
  pt->Draw();
  gPad->Modified();

  ptDate->Draw();
  gPad->Modified();
  
  //phi
  c1->cd(3);
  h_phi_eff->Draw("AP");
  c1->Update();

  g = h_phi_eff->GetPaintedGraph();
  if (g)
  {
    g->GetXaxis()->SetTitle("#Xi^{-} #phi [rad]");
    g->GetYaxis()->SetTitle("Cut Efficiency");
    g->GetYaxis()->SetRangeUser(0.0, 1.05);
    g->GetXaxis()->SetLimits(phibins[0], phibins[nBins_phi]);
  }
  h_phi_eff->Draw("AP"); 
 
  pt->Draw();
  gPad->Modified();

  ptDate->Draw();
  gPad->Modified();
  
  //rapidity
  c1->cd(4);
  h_y_eff->Draw("AP");
  c1->Update();

  g = h_y_eff->GetPaintedGraph();
  if (g)
  {
    g->GetXaxis()->SetTitle("#Xi^{-} y");
    g->GetYaxis()->SetTitle("Cut Efficiency");
    g->GetYaxis()->SetRangeUser(0.0, 1.05);
    g->GetXaxis()->SetLimits(-1.0,1.0);
  }
  h_y_eff->Draw("AP"); 
 
  pt->Draw();
  gPad->Modified();

  ptDate->Draw();
  gPad->Modified();
  
  string extensions[] = {".png"};
  for (auto extension : extensions)
  {
    string output = "Xi_all_cutEff" + extension;
    //string output = "Xi_pT_cutEff" + extension;
    //string output = "Xi_eta_cutEff" + extension;
    //string output = "Xi_phi_cutEff" + extension;
    //string output = "Xi_y_cutEff" + extension;
    c1->SaveAs(output.c_str());
  }
} 
*/
/*
void CutEfficiency()
{
  string sim_inFile_KS0 = "/sphenix/user/cdean/software/analysis/LightFlavorRatios/geometric_acceptance/simulation/outputKFParticle_Kshort_reco.root";
  TFile* sim_file_KS0 = new TFile(sim_inFile_KS0.c_str());
  TTree* sim_tree_KS0 = (TTree*)sim_file_KS0->Get("DecayTree");

  const int nBins_pT = 5;
  double pTbins[nBins_pT + 1] = {0.75, 0.909375, 1.05792, 1.23212, 1.46559, 2.0};

  const int nBins_phi = 10;
  double phibins[nBins_phi + 1] = {-M_PI,-2.51177,-2.0196,-1.55623,-1.0106,-0.530045,0.0583016,1.05245,2.12079,2.59601,M_PI};

  const int nBins_eta = 10;
  double etabins[nBins_eta + 1] = {-1.1,-0.916345,-0.755196,-0.585922,-0.390321,-0.159045,0.122585,0.394623,0.637892,0.849511,1.1};

  std::string truthCut_KS0 = "abs(track_1_true_ID) == 211 && Sum$(abs(track_1_true_track_history_PDG_ID) == 310) > 0 && abs(track_2_true_ID) == 211 && Sum$(abs(track_2_true_track_history_PDG_ID) == 310) > 0 && abs(K_S0_rapidity) <= 1.0 && track_1_MVTX_nHits > 0 && track_2_MVTX_nHits > 0";
  std::string truthDataCut_KS0 = "abs(track_1_true_ID) == 211 && Sum$(abs(track_1_true_track_history_PDG_ID) == 310) > 0 && abs(track_2_true_ID) == 211 && Sum$(abs(track_2_true_track_history_PDG_ID) == 310) > 0 && abs(K_S0_rapidity) <= 1.0 && track_1_MVTX_nHits > 0 && track_2_MVTX_nHits > 0 && track_1_INTT_nHits > 0 && track_2_INTT_nHits > 0 && track_1_TPC_nHits > 19 && track_2_TPC_nHits > 19 && (track_1_chi2/track_1_nDoF) <= 300 && (track_2_chi2/track_2_nDoF) <= 300 && abs(track_1_IP_xy) >= 0.05 && abs(track_2_IP_xy) >= 0.05 && track_1_track_2_DCA <= 0.5 && track_1_track_2_DCA_xy <= 1.0 && K_S0_DIRA >= 0.99 && (K_S0_chi2/K_S0_nDoF) <= 20 && abs(K_S0_mass - 0.5) <= 0.1";

  TH1F* h_pT_truth_KS0 = new TH1F("h_pT_truth_KS0",";K_{S}^{0} p_{T} [GeV/c];Cut Efficiency",nBins_pT,pTbins);
  TH1F* h_pT_truthCuts_KS0 = new TH1F("h_pT_truthCuts_KS0",";K_{S}^{0} p_{T} [GeV/c];Cut Efficiency",nBins_pT,pTbins);
  
  TH1F* h_eta_truth_KS0 = new TH1F("h_eta_truth_KS0",";K_{S}^{0} #eta;Cut Efficiency",nBins_eta,etabins);
  TH1F* h_eta_truthCuts_KS0 = new TH1F("h_eta_truthCuts_KS0",";K_{S}^{0} #eta;Cut Efficiency",nBins_eta,etabins);
  
  TH1F* h_phi_truth_KS0 = new TH1F("h_phi_truth_KS0",";K_{S}^{0} #phi;Cut Efficiency",nBins_phi,phibins);
  TH1F* h_phi_truthCuts_KS0 = new TH1F("h_phi_truthCuts_KS0",";K_{S}^{0} #phi;Cut Efficiency",nBins_phi,phibins);
  
  TH1F* h_y_truth_KS0 = new TH1F("h_y_truth_KS0",";K_{S}^{0} y;Cut Efficiency",10,-1.0,1.0);
  TH1F* h_y_truthCuts_KS0 = new TH1F("h_y_truthCuts_KS0",";K_{S}^{0} y;Cut Efficiency",10,-1.0,1.0);
   
  h_pT_truth_KS0->Sumw2();
  h_pT_truthCuts_KS0->Sumw2();
  h_eta_truth_KS0->Sumw2();
  h_eta_truthCuts_KS0->Sumw2();
  h_phi_truth_KS0->Sumw2();
  h_phi_truthCuts_KS0->Sumw2();
  h_y_truth_KS0->Sumw2();
  h_y_truthCuts_KS0->Sumw2();

  sim_tree_KS0->Draw("K_S0_pT>>+h_pT_truth_KS0",truthCut_KS0.c_str());
  sim_tree_KS0->Draw("K_S0_pT>>+h_pT_truthCuts_KS0",truthDataCut_KS0.c_str());
  sim_tree_KS0->Draw("K_S0_pseudorapidity>>+h_eta_truth_KS0",truthCut_KS0.c_str());
  sim_tree_KS0->Draw("K_S0_pseudorapidity>>+h_eta_truthCuts_KS0",truthDataCut_KS0.c_str());
  sim_tree_KS0->Draw("K_S0_phi>>+h_phi_truth_KS0",truthCut_KS0.c_str());
  sim_tree_KS0->Draw("K_S0_phi>>+h_phi_truthCuts_KS0",truthDataCut_KS0.c_str());
  sim_tree_KS0->Draw("K_S0_rapidity>>+h_y_truth_KS0",truthCut_KS0.c_str());
  sim_tree_KS0->Draw("K_S0_rapidity>>+h_y_truthCuts_KS0",truthDataCut_KS0.c_str());

  TEfficiency* h_pT_eff_KS0 = new TEfficiency(*h_pT_truthCuts_KS0, *h_pT_truth_KS0);
  TEfficiency* h_eta_eff_KS0 = new TEfficiency(*h_eta_truthCuts_KS0, *h_eta_truth_KS0);
  TEfficiency* h_phi_eff_KS0 = new TEfficiency(*h_phi_truthCuts_KS0, *h_phi_truth_KS0);
  TEfficiency* h_y_eff_KS0 = new TEfficiency(*h_y_truthCuts_KS0, *h_y_truth_KS0);
  h_pT_eff_KS0->SetStatisticOption(TEfficiency::kFCP);
  h_pT_eff_KS0->SetMarkerStyle(20);
  h_pT_eff_KS0->SetMarkerSize(1.2);
  h_eta_eff_KS0->SetStatisticOption(TEfficiency::kFCP);
  h_eta_eff_KS0->SetMarkerStyle(20);
  h_eta_eff_KS0->SetMarkerSize(1.2);
  h_phi_eff_KS0->SetStatisticOption(TEfficiency::kFCP);
  h_phi_eff_KS0->SetMarkerStyle(20);
  h_phi_eff_KS0->SetMarkerSize(1.2);
  h_y_eff_KS0->SetStatisticOption(TEfficiency::kFCP);
  h_y_eff_KS0->SetMarkerStyle(20);
  h_y_eff_KS0->SetMarkerSize(1.2);

  TCanvas *c1  = new TCanvas("myCanvas", "myCanvas",1600,1600);
  c1->Divide(2,2);
  //pT 
  c1->cd(1);
  h_pT_eff_KS0->Draw("AP");
  c1->Update();

  TGraph* g = h_pT_eff_KS0->GetPaintedGraph();
  if (g)
  {
    g->GetXaxis()->SetTitle("K_{S}^{0} p_{T} [GeV/c]");
    g->GetYaxis()->SetTitle("Cut Efficiency");
    g->GetYaxis()->SetRangeUser(0.0, 1.05);
    g->GetXaxis()->SetLimits(pTbins[0], pTbins[nBins_pT]);
  }
  h_pT_eff_KS0->Draw("AP"); 
 
  TPaveText *pt;
  pt = new TPaveText(0.15,0.75,0.65,1., "NDC");
  pt->SetFillColor(0);
  pt->SetFillStyle(0);
  pt->SetTextFont(42);
  string label = "#it{#bf{sPHENIX}} Simulation";
  TText *pt_LaTex = pt->AddText(label.c_str());
  pt->SetBorderSize(0);
  pt->Draw();
  gPad->Modified();

  TPaveText *ptDate;
  ptDate = new TPaveText(0.67,0.79,0.85,0.95, "NDC");
  ptDate->SetFillColor(0);
  ptDate->SetFillStyle(0);
  ptDate->SetTextFont(42);
  std::string compilation_date = getDate();
  TText *pt_LaTexDate = ptDate->AddText(compilation_date.c_str());
  ptDate->SetBorderSize(0);
  ptDate->Draw();
  gPad->Modified();
 
  //eta
  c1->cd(2);
  h_eta_eff_KS0->Draw("AP");
  c1->Update();

  g = h_eta_eff_KS0->GetPaintedGraph();
  if (g)
  {
    g->GetXaxis()->SetTitle("K_{S}^{0} #eta");
    g->GetYaxis()->SetTitle("Cut Efficiency");
    g->GetYaxis()->SetRangeUser(0.0, 1.05);
    g->GetXaxis()->SetLimits(etabins[0], etabins[nBins_eta]);
  }
  h_eta_eff_KS0->Draw("AP"); 
 
  pt->Draw();
  gPad->Modified();

  ptDate->Draw();
  gPad->Modified();
  
  //phi
  c1->cd(3);
  h_phi_eff_KS0->Draw("AP");
  c1->Update();

  g = h_phi_eff_KS0->GetPaintedGraph();
  if (g)
  {
    g->GetXaxis()->SetTitle("K_{S}^{0} #phi [rad]");
    g->GetYaxis()->SetTitle("Cut Efficiency");
    g->GetYaxis()->SetRangeUser(0.0, 1.05);
    g->GetXaxis()->SetLimits(phibins[0], phibins[nBins_phi]);
  }
  h_phi_eff_KS0->Draw("AP"); 
 
  pt->Draw();
  gPad->Modified();

  ptDate->Draw();
  gPad->Modified();
  
  //rapidity
  c1->cd(4);
  h_y_eff_KS0->Draw("AP");
  c1->Update();

  g = h_y_eff_KS0->GetPaintedGraph();
  if (g)
  {
    g->GetXaxis()->SetTitle("K_{S}^{0} y");
    g->GetYaxis()->SetTitle("Cut Efficiency");
    g->GetYaxis()->SetRangeUser(0.0, 1.05);
    g->GetXaxis()->SetLimits(-1.0,1.0);
  }
  h_y_eff_KS0->Draw("AP"); 
 
  pt->Draw();
  gPad->Modified();

  ptDate->Draw();
  gPad->Modified();
  
  string extensions[] = {".png"};
  for (auto extension : extensions)
  {
    string output = "KS0_all_cutEff" + extension;
    c1->SaveAs(output.c_str());
  }
} 
*/
void CutEfficiency()
{
  //string sim_inFile_Xi = "/sphenix/tg/tg01/hf/mjpeters/LightFlavorResults/outputKFParticle_Lambda_reco.root";
  string sim_inFile_Xi = "/gpfs/mnt/gpfs02/sphenix/user/cdean/software/analysis/LightFlavorRatios/geometric_acceptance/simulation/outputKFParticle_Lambda2ppi_reco_Usman_patch.root";
  TFile* sim_file_Xi = new TFile(sim_inFile_Xi.c_str());
  TTree* sim_tree_Xi = (TTree*)sim_file_Xi->Get("DecayTree");

  /*
  const int nBins_pT = 5;
  double pTbins[nBins_pT + 1] = {0.75, 0.909375, 1.05792, 1.23212, 1.46559, 2.0};

  const int nBins_phi = 10;
  double phibins[nBins_phi + 1] = {-M_PI,-2.51177,-2.0196,-1.55623,-1.0106,-0.530045,0.0583016,1.05245,2.12079,2.59601,M_PI};

  const int nBins_eta = 10;
  double etabins[nBins_eta + 1] = {-1.1,-0.916345,-0.755196,-0.585922,-0.390321,-0.159045,0.122585,0.394623,0.637892,0.849511,1.1};
  */

  HistogramInfo pTbininfo = BinInfo::final_pt_bins;
  HistogramInfo phibininfo = BinInfo::final_phi_bins;
  HistogramInfo etabininfo = BinInfo::final_eta_bins;
  HistogramInfo ybininfo = BinInfo::final_rapidity_bins;

  const int nBins_pT = pTbininfo.bins.size()-1;
  std::vector<float> pTbins = pTbininfo.bins;

  const int nBins_phi = phibininfo.bins.size()-1;
  std::vector<float> phibins = phibininfo.bins;

  const int nBins_eta = etabininfo.bins.size()-1;
  std::vector<float> etabins = etabininfo.bins;

  const int nBins_rap = ybininfo.bins.size()-1;
  std::vector<float> rapbins = ybininfo.bins;

  double Lambda_PV_sigma_xy = .003553;
  double Lambda_track_1_sigma_xy = .002853;
  double Lambda_track_2_sigma_xy = .002841;
  double Lambda_track_1_sigma = .003507;
  double Lambda_track_2_sigma = .003192;
  double Lambda_PV_sigma = pow(9.756e-8,1./3.);
  double Lambda_SV_sigma = pow(2.663e-6,1./3.);

  double KS0_PV_sigma_xy = .003477; // sqrt(C_00 + C_11 - 2*C_01) from primary_vertex_Covariance
  double KS0_track_1_sigma_xy = .002584; // sqrt(C_00 + C_11 - 2*C_01) from track_1_Covariance
  double KS0_track_2_sigma_xy = .002585; // above but for track_2_Covariance
  double KS0_track_1_sigma = .002985; // track_1_IPErr
  double KS0_track_2_sigma = .002959; // track_2_IPErr
  double KS0_PV_sigma = pow(9.087e-8,1./3.); // (mother vertex volume)^1/3
  double KS0_SV_sigma = pow(7.531e-6,1./3.); // (primary vertex volume)^1/3

  std::string truthCut = "((abs(track_1_true_ID) == 211 && abs(track_2_true_ID) == 2212) || (abs(track_1_true_ID) == 2212 && abs(track_2_true_ID) == 211)) && Sum$(abs(track_1_true_track_history_PDG_ID) == 3122) > 0 && Sum$(abs(track_2_true_track_history_PDG_ID) == 3122) > 0 && abs(Lambda0_rapidity) <= 1.0 && track_1_MVTX_nHits > 0 && track_2_MVTX_nHits > 0 && abs(primary_vertex_z) < 10";

  std::string truthDataCut = "((abs(track_1_true_ID) == 211 && abs(track_2_true_ID) == 2212) || (abs(track_1_true_ID) == 2212 && abs(track_2_true_ID) == 211)) && Sum$(abs(track_1_true_track_history_PDG_ID) == 3122) > 0 && Sum$(abs(track_2_true_track_history_PDG_ID) == 3122) > 0 && abs(Lambda0_rapidity) <= 1.0 && track_1_MVTX_nHits > 0 && track_2_MVTX_nHits > 0 && abs(primary_vertex_z) < 10 && track_1_INTT_nHits > 0 && track_2_INTT_nHits > 0 && track_1_TPC_nHits > 19 && track_2_TPC_nHits > 19 && (track_1_chi2/track_1_nDoF) <= 300 && (track_2_chi2/track_2_nDoF) <= 300 && abs(track_1_IP_xy) >= 0.05 && abs(track_2_IP_xy) >= 0.05 && Lambda0_DIRA >= 0.99 && abs(Lambda0_mass - 1.115) <= 0.1 && Lambda0_decayLength>=0.05 && track_1_pT >= 0.2 && track_2_pT >= 0.2";

  std::string truthDataCut_min = "((abs(track_1_true_ID) == 211 && abs(track_2_true_ID) == 2212) || (abs(track_1_true_ID) == 2212 && abs(track_2_true_ID) == 211)) && Sum$(abs(track_1_true_track_history_PDG_ID) == 3122) > 0 && Sum$(abs(track_2_true_track_history_PDG_ID) == 3122) > 0 && abs(Lambda0_rapidity) <= 1.0 && track_1_MVTX_nHits > 0 && track_2_MVTX_nHits > 0 && abs(primary_vertex_z) < 10 && track_1_INTT_nHits > 0 && track_2_INTT_nHits > 0 && track_1_TPC_nHits > 19 && track_2_TPC_nHits > 19 && (track_1_chi2/track_1_nDoF) <= 300 && (track_2_chi2/track_2_nDoF) <= 300 && (abs(track_1_IP_xy) - " + std::to_string(Lambda_PV_sigma_xy) + " - " + std::to_string(Lambda_track_1_sigma_xy) + ") >= 0.05 && (abs(track_2_IP_xy) - " + std::to_string(Lambda_PV_sigma_xy) + " - " + std::to_string(Lambda_track_2_sigma_xy) + ") >= 0.05 && (track_1_track_2_DCA + " + std::to_string(Lambda_track_1_sigma) + " + " + std::to_string(Lambda_track_2_sigma) + ") <= 0.5 && (track_1_track_2_DCA_xy + " + std::to_string(Lambda_track_1_sigma_xy) + " + " + std::to_string(Lambda_track_2_sigma_xy) + ") <= 1.0 && cos(acos(Lambda0_DIRA) + atan(" + std::to_string(Lambda_PV_sigma) + "/Lambda0_decayLength) + atan(" + std::to_string(Lambda_SV_sigma) + "/Lambda0_decayLength)) >= 0.99 && abs(Lambda0_mass - 1.115) <= 0.1 && track_1_pT >= 0.2 && track_2_pT >= 0.2";
  
  std::string truthDataCut_max = " ((abs(track_1_true_ID) == 211 && abs(track_2_true_ID) == 2212) || (abs(track_1_true_ID) == 2212 && abs(track_2_true_ID) == 211)) && Sum$(abs(track_1_true_track_history_PDG_ID) == 3122) > 0 && Sum$(abs(track_2_true_track_history_PDG_ID) == 3122) > 0 && abs(Lambda0_rapidity) <= 1.0 && track_1_MVTX_nHits > 0 && track_2_MVTX_nHits > 0 && abs(primary_vertex_z) < 10 && track_1_INTT_nHits > 0 && track_2_INTT_nHits > 0 && track_1_TPC_nHits > 19 && track_2_TPC_nHits > 19 && (track_1_chi2/track_1_nDoF) <= 300 && (track_2_chi2/track_2_nDoF) <= 300 && (abs(track_1_IP_xy) + " + std::to_string(Lambda_PV_sigma_xy) + " + " + std::to_string(Lambda_track_1_sigma_xy) + ") >= 0.05 && (abs(track_2_IP_xy) + " + std::to_string(Lambda_PV_sigma_xy) + " + " + std::to_string(Lambda_track_2_sigma_xy) + ") >= 0.05 && (track_1_track_2_DCA - " + std::to_string(Lambda_track_1_sigma) + " - " + std::to_string(Lambda_track_2_sigma) + ") <= 0.5 && (track_1_track_2_DCA_xy - " + std::to_string(Lambda_track_1_sigma_xy) + " - " + std::to_string(Lambda_track_2_sigma_xy) + ") <= 1.0 && cos(acos(Lambda0_DIRA) - atan(" + std::to_string(Lambda_PV_sigma) + "/Lambda0_decayLength) - atan(" + std::to_string(Lambda_SV_sigma) + "/Lambda0_decayLength)) >= 0.99 && abs(Lambda0_mass - 1.115) <= 0.1 && track_1_pT >= 0.2 && track_2_pT >= 0.2";

  //std::string truthCut = "abs(Lambda0_track_1_true_ID) == 211 && Sum$(abs(Lambda0_track_1_true_track_history_PDG_ID) == 3122) > 0 && Sum$(abs(Lambda0_track_1_true_track_history_PDG_ID) == 3312) > 0 && abs(Lambda0_track_2_true_ID) == 2212 && Sum$(abs(Lambda0_track_2_true_track_history_PDG_ID) == 3122) > 0 && Sum$(abs(Lambda0_track_2_true_track_history_PDG_ID) == 3312) > 0 && abs(track_3_true_ID) == 211 && Sum$(abs(track_3_true_track_history_PDG_ID) == 3312) > 0 && Sum$(abs(track_3_true_track_history_PDG_ID) == 3122) == 0 && abs(Ximinus_rapidity) <= 1.0 && Lambda0_track_1_MVTX_nHits > 0 && Lambda0_track_2_MVTX_nHits > 0 && track_3_MVTX_nHits > 0 && abs(primary_vertex_z) < 10 && Lambda0_track_1_charge == track_3_charge";
  //std::string truthDataCut = "abs(Lambda0_track_1_true_ID) == 211 && Sum$(abs(Lambda0_track_1_true_track_history_PDG_ID) == 3122) > 0 && Sum$(abs(Lambda0_track_1_true_track_history_PDG_ID) == 3312) > 0 && abs(Lambda0_track_2_true_ID) == 2212 && Sum$(abs(Lambda0_track_2_true_track_history_PDG_ID) == 3122) > 0 && Sum$(abs(Lambda0_track_2_true_track_history_PDG_ID) == 3312) > 0 && abs(track_3_true_ID) == 211 && Sum$(abs(track_3_true_track_history_PDG_ID) == 3312) > 0 && Sum$(abs(track_3_true_track_history_PDG_ID) == 3122) == 0 && Lambda0_track_1_MVTX_nHits > 0 && Lambda0_track_2_MVTX_nHits > 0 && track_3_MVTX_nHits > 0 && abs(primary_vertex_z) < 10 && Lambda0_track_1_charge == track_3_charge && Lambda0_track_1_INTT_nHits > 0 && Lambda0_track_2_INTT_nHits > 0 && track_3_INTT_nHits > 0 && Lambda0_track_1_TPC_nHits > 19 && Lambda0_track_2_TPC_nHits > 19 && track_3_TPC_nHits > 19 && abs(Lambda0_mass - 1.1157) <= 0.01 && Ximinus_decayLength_xy >= 0.15 && Lambda0_decayLength_xy >= 0.01 && (Lambda0_track_1_chi2/Lambda0_track_1_nDoF) <= 400 && (Lambda0_track_2_chi2/Lambda0_track_2_nDoF) <= 400 && (track_3_chi2/track_3_nDoF) <= 400 && track_1_track_2_DCA <= 0.5 && track_1_track_3_DCA <= 0.5 && track_2_track_3_DCA <= 0.5 && Ximinus_chi2/Ximinus_nDoF <= 50 && Lambda0_chi2/Lambda0_nDoF <= 50 && abs(Ximinus_rapidity) <= 1.0 && Lambda0_track_1_pT >= 0.2 && Lambda0_track_2_pT >= 0.2 && track_3_pT >= 0.2";

  TH1F* h_pT_truth = new TH1F("h_pT_truth",";#Lambda p_{T} [GeV/c];Cut Efficiency",nBins_pT,pTbins.data());
  TH1F* h_pT_truthCuts = new TH1F("h_pT_truthCuts",";#Lambda p_{T} [GeV/c];Cut Efficiency",nBins_pT,pTbins.data());
  TH1F* h_pT_truthCuts_min = new TH1F("h_pT_truthCuts_min","#Lambda p_{T} [GeV/c];Cut Efficiency",nBins_pT,pTbins.data());
  TH1F* h_pT_truthCuts_max = new TH1F("h_pT_truthCuts_max","#Lambda p_{T} [GeV/c];Cut Efficiency",nBins_pT,pTbins.data());
  
  TH1F* h_eta_truth = new TH1F("h_eta_truth",";#Lambda #eta;Cut Efficiency",nBins_eta,etabins.data());
  TH1F* h_eta_truthCuts = new TH1F("h_eta_truthCuts",";#Lambda #eta;Cut Efficiency",nBins_eta,etabins.data());
  TH1F* h_eta_truthCuts_min = new TH1F("h_eta_truthCuts_min",";#Lambda #eta;Cut Efficiency",nBins_eta,etabins.data());
  TH1F* h_eta_truthCuts_max = new TH1F("h_eta_truthCuts_max",";#Lambda #eta;Cut Efficiency",nBins_eta,etabins.data());
  
  TH1F* h_phi_truth = new TH1F("h_phi_truth",";#Lambda #phi;Cut Efficiency",nBins_phi,phibins.data());
  TH1F* h_phi_truthCuts = new TH1F("h_phi_truthCuts",";#Lambda #phi;Cut Efficiency",nBins_phi,phibins.data());
  TH1F* h_phi_truthCuts_min = new TH1F("h_phi_truthCuts_min",";#Lambda #phi;Cut Efficiency",nBins_phi,phibins.data());
  TH1F* h_phi_truthCuts_max = new TH1F("h_phi_truthCuts_max",";#Lambda #phi;Cut Efficiency",nBins_phi,phibins.data());
  
  TH1F* h_y_truth = new TH1F("h_y_truth",";#Lambda y;Cut Efficiency",nBins_rap,rapbins.data());
  TH1F* h_y_truthCuts = new TH1F("h_y_truthCuts",";#Lambda y;Cut Efficiency",nBins_rap,rapbins.data());
  TH1F* h_y_truthCuts_min = new TH1F("h_y_truthCuts_min",";#Lambda y;Cut Efficiency",nBins_rap,rapbins.data());
  TH1F* h_y_truthCuts_max = new TH1F("h_y_truthCuts_max",";#Lambda y;Cut Efficiency",nBins_rap,rapbins.data());
   
  h_pT_truth->Sumw2();
  h_pT_truthCuts->Sumw2();
  h_pT_truthCuts_min->Sumw2();
  h_pT_truthCuts_max->Sumw2();
  h_eta_truth->Sumw2();
  h_eta_truthCuts->Sumw2();
  h_eta_truthCuts_min->Sumw2();
  h_eta_truthCuts_max->Sumw2();
  h_phi_truth->Sumw2();
  h_phi_truthCuts->Sumw2();
  h_phi_truthCuts_min->Sumw2();
  h_phi_truthCuts_max->Sumw2();
  h_y_truth->Sumw2();
  h_y_truthCuts->Sumw2();
  h_y_truthCuts_min->Sumw2();
  h_y_truthCuts_max->Sumw2();

  sim_tree_Xi->Draw("Lambda0_pT>>+h_pT_truth",truthCut.c_str());
  sim_tree_Xi->Draw("Lambda0_pT>>+h_pT_truthCuts",truthDataCut.c_str());
  sim_tree_Xi->Draw("Lambda0_pT>>+h_pT_truthCuts_min",truthDataCut_min.c_str());
  sim_tree_Xi->Draw("Lambda0_pT>>+h_pT_truthCuts_max",truthDataCut_max.c_str());
  sim_tree_Xi->Draw("Lambda0_pseudorapidity>>+h_eta_truth",truthCut.c_str());
  sim_tree_Xi->Draw("Lambda0_pseudorapidity>>+h_eta_truthCuts",truthDataCut.c_str());
  sim_tree_Xi->Draw("Lambda0_pseudorapidity>>+h_eta_truthCuts_min",truthDataCut_min.c_str());
  sim_tree_Xi->Draw("Lambda0_pseudorapidity>>+h_eta_truthCuts_max",truthDataCut_max.c_str());
  sim_tree_Xi->Draw("Lambda0_phi>>+h_phi_truth",truthCut.c_str());
  sim_tree_Xi->Draw("Lambda0_phi>>+h_phi_truthCuts",truthDataCut.c_str());
  sim_tree_Xi->Draw("Lambda0_phi>>+h_phi_truthCuts_min",truthDataCut_min.c_str());
  sim_tree_Xi->Draw("Lambda0_phi>>+h_phi_truthCuts_max",truthDataCut_max.c_str());
  sim_tree_Xi->Draw("Lambda0_rapidity>>+h_y_truth",truthCut.c_str());
  sim_tree_Xi->Draw("Lambda0_rapidity>>+h_y_truthCuts",truthDataCut.c_str());
  sim_tree_Xi->Draw("Lambda0_rapidity>>+h_y_truthCuts_min",truthDataCut_min.c_str());
  sim_tree_Xi->Draw("Lambda0_rapidity>>+h_y_truthCuts_max",truthDataCut_max.c_str());

  //string sim_inFile_KS0 = "/sphenix/user/cdean/software/analysis/LightFlavorRatios/geometric_acceptance/simulation/outputKFParticle_Kshort_reco.root";
  string sim_inFile_KS0 = "/gpfs/mnt/gpfs02/sphenix/user/cdean/software/analysis/LightFlavorRatios/geometric_acceptance/simulation/outputKFParticle_Kshort2pipi_reco_Usman_patch.root";
  TFile* sim_file_KS0 = new TFile(sim_inFile_KS0.c_str());
  TTree* sim_tree_KS0 = (TTree*)sim_file_KS0->Get("DecayTree");

  std::string truthCut_KS0 = "abs(track_1_true_ID) == 211 && Sum$(abs(track_1_true_track_history_PDG_ID) == 310) > 0 && abs(track_2_true_ID) == 211 && Sum$(abs(track_2_true_track_history_PDG_ID) == 310) > 0 && abs(K_S0_rapidity) <= 1.0 && track_1_MVTX_nHits > 0 && track_2_MVTX_nHits > 0 && abs(primary_vertex_z) < 10";
  std::string truthDataCut_KS0 = "abs(track_1_true_ID) == 211 && Sum$(abs(track_1_true_track_history_PDG_ID) == 310) > 0 && abs(track_2_true_ID) == 211 && Sum$(abs(track_2_true_track_history_PDG_ID) == 310) > 0 && abs(K_S0_rapidity) <= 1.0 && track_1_MVTX_nHits > 0 && track_2_MVTX_nHits > 0 && abs(primary_vertex_z) < 10 && track_1_INTT_nHits > 0 && track_2_INTT_nHits > 0 && track_1_TPC_nHits > 19 && track_2_TPC_nHits > 19 && (track_1_chi2/track_1_nDoF) <= 300 && (track_2_chi2/track_2_nDoF) <= 300 && abs(track_1_IP_xy) >= 0.05 && abs(track_2_IP_xy) >= 0.05 && K_S0_DIRA >= 0.99 && abs(K_S0_mass - 0.5) <= 0.1 && K_S0_decayLength >= 0.05 && track_1_pT >= 0.2 && track_2_pT >= 0.2";
  std::string truthDataCut_KS0_min = "abs(track_1_true_ID) == 211 && Sum$(abs(track_1_true_track_history_PDG_ID) == 310) > 0 && abs(track_2_true_ID) == 211 && Sum$(abs(track_2_true_track_history_PDG_ID) == 310) > 0 && abs(K_S0_rapidity) <= 1.0 && track_1_MVTX_nHits > 0 && track_2_MVTX_nHits > 0 && abs(primary_vertex_z) < 10 && track_1_INTT_nHits > 0 && track_2_INTT_nHits > 0 && track_1_TPC_nHits > 19 && track_2_TPC_nHits > 19 && (track_1_chi2/track_1_nDoF) <= 300 && (track_2_chi2/track_2_nDoF) <= 300 && (abs(track_1_IP_xy) - " + std::to_string(KS0_PV_sigma_xy) + " - " + std::to_string(KS0_track_1_sigma_xy) + ") >= 0.05 && (abs(track_2_IP_xy) - " + std::to_string(KS0_PV_sigma_xy) + " - " + std::to_string(KS0_track_2_sigma_xy) + ") >= 0.05 && (track_1_track_2_DCA + " + std::to_string(KS0_track_1_sigma) + " + " + std::to_string(KS0_track_2_sigma) + ") <= 0.5 && (track_1_track_2_DCA_xy + " + std::to_string(KS0_track_1_sigma_xy) + " + " + std::to_string(KS0_track_2_sigma_xy) + ") <= 1.0 && cos(acos(K_S0_DIRA) + atan(" + std::to_string(KS0_PV_sigma) + "/K_S0_decayLength) + atan(" + std::to_string(KS0_SV_sigma) + "/K_S0_decayLength)) >= 0.99 && abs(K_S0_mass - 0.5) <= 0.1 && track_1_pT >= 0.2 && track_2_pT >= 0.2";
  std::string truthDataCut_KS0_max = "abs(track_1_true_ID) == 211 && Sum$(abs(track_1_true_track_history_PDG_ID) == 310) > 0 && abs(track_2_true_ID) == 211 && Sum$(abs(track_2_true_track_history_PDG_ID) == 310) > 0 && abs(K_S0_rapidity) <= 1.0 && track_1_MVTX_nHits > 0 && track_2_MVTX_nHits > 0 && abs(primary_vertex_z) < 10 && track_1_INTT_nHits > 0 && track_2_INTT_nHits > 0 && track_1_TPC_nHits > 19 && track_2_TPC_nHits > 19 && (track_1_chi2/track_1_nDoF) <= 300 && (track_2_chi2/track_2_nDoF) <= 300 && (abs(track_1_IP_xy) + " + std::to_string(KS0_PV_sigma_xy) + " + " + std::to_string(KS0_track_1_sigma_xy) + ") >= 0.05 && (abs(track_2_IP_xy) + " + std::to_string(KS0_PV_sigma_xy) + " + " + std::to_string(KS0_track_2_sigma_xy) + ") >= 0.05 && (track_1_track_2_DCA - " + std::to_string(KS0_track_1_sigma) + " - " + std::to_string(KS0_track_2_sigma) + ") <= 0.5 && (track_1_track_2_DCA_xy - " + std::to_string(KS0_track_1_sigma_xy) + " - " + std::to_string(KS0_track_2_sigma_xy) + ") <= 1.0 && cos(acos(K_S0_DIRA) - atan(" + std::to_string(KS0_PV_sigma) + "/K_S0_decayLength) - atan(" + std::to_string(KS0_SV_sigma) + "/K_S0_decayLength)) >= 0.99 && abs(K_S0_mass - 0.5) <= 0.1 && track_1_pT >= 0.2 && track_2_pT >= 0.2";

  TH1F* h_pT_truth_KS0 = new TH1F("h_pT_truth_KS0",";K_{S}^{0} p_{T} [GeV/c];Cut Efficiency",nBins_pT,pTbins.data());
  TH1F* h_pT_truthCuts_KS0 = new TH1F("h_pT_truthCuts_KS0",";K_{S}^{0} p_{T} [GeV/c];Cut Efficiency",nBins_pT,pTbins.data());
  TH1F* h_pT_truthCuts_KS0_min = new TH1F("h_pT_truthCuts_KS0_min",";K_{S}^{0} p_{T} [GeV/c];Cut Efficiency",nBins_pT,pTbins.data());
  TH1F* h_pT_truthCuts_KS0_max = new TH1F("h_pT_truthCuts_KS0_max",";K_{S}^{0} p_{T} [GeV/c];Cut Efficiency",nBins_pT,pTbins.data());
  
  TH1F* h_eta_truth_KS0 = new TH1F("h_eta_truth_KS0",";K_{S}^{0} #eta;Cut Efficiency",nBins_eta,etabins.data());
  TH1F* h_eta_truthCuts_KS0 = new TH1F("h_eta_truthCuts_KS0",";K_{S}^{0} #eta;Cut Efficiency",nBins_eta,etabins.data());
  TH1F* h_eta_truthCuts_KS0_min = new TH1F("h_eta_truthCuts_KS0_min",";K_{S}^{0} #eta;Cut Efficiency",nBins_eta,etabins.data());
  TH1F* h_eta_truthCuts_KS0_max = new TH1F("h_eta_truthCuts_KS0_max",";K_{S}^{0} #eta;Cut Efficiency",nBins_eta,etabins.data());
  
  TH1F* h_phi_truth_KS0 = new TH1F("h_phi_truth_KS0",";K_{S}^{0} #phi;Cut Efficiency",nBins_phi,phibins.data());
  TH1F* h_phi_truthCuts_KS0 = new TH1F("h_phi_truthCuts_KS0",";K_{S}^{0} #phi;Cut Efficiency",nBins_phi,phibins.data());
  TH1F* h_phi_truthCuts_KS0_min = new TH1F("h_phi_truthCuts_KS0_min",";K_{S}^{0} #phi;Cut Efficiency",nBins_phi,phibins.data());
  TH1F* h_phi_truthCuts_KS0_max = new TH1F("h_phi_truthCuts_KS0_max",";K_{S}^{0} #phi;Cut Efficiency",nBins_phi,phibins.data());
  
  TH1F* h_y_truth_KS0 = new TH1F("h_y_truth_KS0",";K_{S}^{0} y;Cut Efficiency",nBins_rap,rapbins.data());
  TH1F* h_y_truthCuts_KS0 = new TH1F("h_y_truthCuts_KS0",";K_{S}^{0} y;Cut Efficiency",nBins_rap,rapbins.data());
  TH1F* h_y_truthCuts_KS0_min = new TH1F("h_y_truthCuts_KS0_min",";K_{S}^{0} y;Cut Efficiency",nBins_rap,rapbins.data());
  TH1F* h_y_truthCuts_KS0_max = new TH1F("h_y_truthCuts_KS0_max",";K_{S}^{0} y;Cut Efficiency",nBins_rap,rapbins.data());
   
  h_pT_truth_KS0->Sumw2();
  h_pT_truthCuts_KS0->Sumw2();
  h_pT_truthCuts_KS0_min->Sumw2();
  h_pT_truthCuts_KS0_max->Sumw2();
  h_eta_truth_KS0->Sumw2();
  h_eta_truthCuts_KS0->Sumw2();
  h_eta_truthCuts_KS0_min->Sumw2();
  h_eta_truthCuts_KS0_max->Sumw2();
  h_phi_truth_KS0->Sumw2();
  h_phi_truthCuts_KS0->Sumw2();
  h_phi_truthCuts_KS0_min->Sumw2();
  h_phi_truthCuts_KS0_max->Sumw2();
  h_y_truth_KS0->Sumw2();
  h_y_truthCuts_KS0->Sumw2();
  h_y_truthCuts_KS0_min->Sumw2();
  h_y_truthCuts_KS0_max->Sumw2();

  sim_tree_KS0->Draw("K_S0_pT>>+h_pT_truth_KS0",truthCut_KS0.c_str());
  sim_tree_KS0->Draw("K_S0_pT>>+h_pT_truthCuts_KS0",truthDataCut_KS0.c_str());
  sim_tree_KS0->Draw("K_S0_pT>>+h_pT_truthCuts_KS0_min",truthDataCut_KS0_min.c_str());
  sim_tree_KS0->Draw("K_S0_pT>>+h_pT_truthCuts_KS0_max",truthDataCut_KS0_max.c_str());
  sim_tree_KS0->Draw("K_S0_pseudorapidity>>+h_eta_truth_KS0",truthCut_KS0.c_str());
  sim_tree_KS0->Draw("K_S0_pseudorapidity>>+h_eta_truthCuts_KS0",truthDataCut_KS0.c_str());
  sim_tree_KS0->Draw("K_S0_pseudorapidity>>+h_eta_truthCuts_KS0_min",truthDataCut_KS0_min.c_str());
  sim_tree_KS0->Draw("K_S0_pseudorapidity>>+h_eta_truthCuts_KS0_max",truthDataCut_KS0_max.c_str());
  sim_tree_KS0->Draw("K_S0_phi>>+h_phi_truth_KS0",truthCut_KS0.c_str());
  sim_tree_KS0->Draw("K_S0_phi>>+h_phi_truthCuts_KS0",truthDataCut_KS0.c_str());
  sim_tree_KS0->Draw("K_S0_phi>>+h_phi_truthCuts_KS0_min",truthDataCut_KS0_min.c_str());
  sim_tree_KS0->Draw("K_S0_phi>>+h_phi_truthCuts_KS0_max",truthDataCut_KS0_max.c_str());
  sim_tree_KS0->Draw("K_S0_rapidity>>+h_y_truth_KS0",truthCut_KS0.c_str());
  sim_tree_KS0->Draw("K_S0_rapidity>>+h_y_truthCuts_KS0",truthDataCut_KS0.c_str());
  sim_tree_KS0->Draw("K_S0_rapidity>>+h_y_truthCuts_KS0_min",truthDataCut_KS0_min.c_str());
  sim_tree_KS0->Draw("K_S0_rapidity>>+h_y_truthCuts_KS0_max",truthDataCut_KS0_max.c_str());

  TH1F* hEff_pT = (TH1F*)h_pT_truth->Clone();
  TH1F* hEff_eta = (TH1F*)h_eta_truth->Clone();
  TH1F* hEff_phi = (TH1F*)h_phi_truth->Clone();
  TH1F* hEff_y = (TH1F*)h_y_truth->Clone();

  TH1F* hEff_pT_min = (TH1F*)h_pT_truth->Clone();
  TH1F* hEff_eta_min = (TH1F*)h_eta_truth->Clone();
  TH1F* hEff_phi_min = (TH1F*)h_phi_truth->Clone();
  TH1F* hEff_y_min = (TH1F*)h_y_truth->Clone();

  TH1F* hEff_pT_max = (TH1F*)h_pT_truth->Clone();
  TH1F* hEff_eta_max = (TH1F*)h_eta_truth->Clone();
  TH1F* hEff_phi_max = (TH1F*)h_phi_truth->Clone();
  TH1F* hEff_y_max = (TH1F*)h_y_truth->Clone();

  TH1F* hEff_pT_KS0 = (TH1F*)h_pT_truth_KS0->Clone();
  TH1F* hEff_eta_KS0 = (TH1F*)h_eta_truth_KS0->Clone();
  TH1F* hEff_phi_KS0 = (TH1F*)h_phi_truth_KS0->Clone();
  TH1F* hEff_y_KS0 = (TH1F*)h_y_truth_KS0->Clone();

  TH1F* hEff_pT_KS0_min = (TH1F*)h_pT_truth_KS0->Clone();
  TH1F* hEff_eta_KS0_min = (TH1F*)h_eta_truth_KS0->Clone();
  TH1F* hEff_phi_KS0_min = (TH1F*)h_phi_truth_KS0->Clone();
  TH1F* hEff_y_KS0_min = (TH1F*)h_y_truth_KS0->Clone();

  TH1F* hEff_pT_KS0_max = (TH1F*)h_pT_truth_KS0->Clone();
  TH1F* hEff_eta_KS0_max = (TH1F*)h_eta_truth_KS0->Clone();
  TH1F* hEff_phi_KS0_max = (TH1F*)h_phi_truth_KS0->Clone();
  TH1F* hEff_y_KS0_max = (TH1F*)h_y_truth_KS0->Clone();

  hEff_pT->SetName("eff_pT");
  hEff_eta->SetName("eff_eta");
  hEff_phi->SetName("eff_phi");
  hEff_y->SetName("eff_y");

  hEff_pT_min->SetName("eff_pT_min");
  hEff_eta_min->SetName("eff_eta_min");
  hEff_phi_min->SetName("eff_phi_min");
  hEff_y_min->SetName("eff_y_min");

  hEff_pT_max->SetName("eff_pT_max");
  hEff_eta_max->SetName("eff_eta_max");
  hEff_phi_max->SetName("eff_phi_max");
  hEff_y_max->SetName("eff_y_max");

  hEff_pT_KS0->SetName("eff_pT_KS0");
  hEff_eta_KS0->SetName("eff_eta_KS0");
  hEff_phi_KS0->SetName("eff_phi_KS0");
  hEff_y_KS0->SetName("eff_y_KS0");

  hEff_pT_KS0_min->SetName("eff_pT_KS0_min");
  hEff_eta_KS0_min->SetName("eff_eta_KS0_min");
  hEff_phi_KS0_min->SetName("eff_phi_KS0_min");
  hEff_y_KS0_min->SetName("eff_y_KS0_min");

  hEff_pT_KS0_max->SetName("eff_pT_KS0_max");
  hEff_eta_KS0_max->SetName("eff_eta_KS0_max");
  hEff_phi_KS0_max->SetName("eff_phi_KS0_max");
  hEff_y_KS0_max->SetName("eff_y_KS0_max");

  hEff_pT->Divide(h_pT_truthCuts,h_pT_truth);
  hEff_eta->Divide(h_eta_truthCuts,h_eta_truth);
  hEff_phi->Divide(h_phi_truthCuts,h_phi_truth);
  hEff_y->Divide(h_y_truthCuts,h_y_truth);

  hEff_pT_min->Divide(h_pT_truthCuts_min,h_pT_truth);
  hEff_eta_min->Divide(h_eta_truthCuts_min,h_eta_truth);
  hEff_phi_min->Divide(h_phi_truthCuts_min,h_phi_truth);
  hEff_y_min->Divide(h_y_truthCuts_min,h_y_truth);

  hEff_pT_max->Divide(h_pT_truthCuts,h_pT_truth);
  hEff_eta_max->Divide(h_eta_truthCuts,h_eta_truth);
  hEff_phi_max->Divide(h_phi_truthCuts,h_phi_truth);
  hEff_y_max->Divide(h_y_truthCuts,h_y_truth);

  hEff_pT_KS0->Divide(h_pT_truthCuts_KS0,h_pT_truth_KS0);
  hEff_eta_KS0->Divide(h_eta_truthCuts_KS0,h_eta_truth_KS0);
  hEff_phi_KS0->Divide(h_phi_truthCuts_KS0,h_phi_truth_KS0);
  hEff_y_KS0->Divide(h_y_truthCuts_KS0,h_y_truth_KS0);

  hEff_pT_KS0_min->Divide(h_pT_truthCuts_KS0_min,h_pT_truth_KS0);
  hEff_eta_KS0_min->Divide(h_eta_truthCuts_KS0_min,h_eta_truth_KS0);
  hEff_phi_KS0_min->Divide(h_phi_truthCuts_KS0_min,h_phi_truth_KS0);
  hEff_y_KS0_min->Divide(h_y_truthCuts_KS0_min,h_y_truth_KS0);

  hEff_pT_KS0_max->Divide(h_pT_truthCuts_KS0_max,h_pT_truth_KS0);
  hEff_eta_KS0_max->Divide(h_eta_truthCuts_KS0_max,h_eta_truth_KS0);
  hEff_phi_KS0_max->Divide(h_phi_truthCuts_KS0_max,h_phi_truth_KS0);
  hEff_y_KS0_max->Divide(h_y_truthCuts_KS0_max,h_y_truth_KS0);

  TH1F* hEffRatio_pT = (TH1F*)hEff_pT->Clone();
  TH1F* hEffRatio_eta = (TH1F*)hEff_eta->Clone();
  TH1F* hEffRatio_phi = (TH1F*)hEff_phi->Clone();
  TH1F* hEffRatio_y = (TH1F*)hEff_y->Clone();

  TH1F* hEffRatio_pT_min = (TH1F*)hEff_pT_min->Clone();
  TH1F* hEffRatio_eta_min = (TH1F*)hEff_eta_min->Clone();
  TH1F* hEffRatio_phi_min = (TH1F*)hEff_phi_min->Clone();
  TH1F* hEffRatio_y_min = (TH1F*)hEff_y_min->Clone();

  TH1F* hEffRatio_pT_max = (TH1F*)hEff_pT_max->Clone();
  TH1F* hEffRatio_eta_max = (TH1F*)hEff_eta_max->Clone();
  TH1F* hEffRatio_phi_max = (TH1F*)hEff_phi_max->Clone();
  TH1F* hEffRatio_y_max = (TH1F*)hEff_y_max->Clone();

  hEffRatio_pT->SetName("hEffRatio_pT");
  hEffRatio_eta->SetName("hEffRatio_eta");
  hEffRatio_phi->SetName("hEffRatio_phi");
  hEffRatio_y->SetName("hEffRatio_y");

  hEffRatio_pT_min->SetName("hEffRatio_pT_min");
  hEffRatio_eta_min->SetName("hEffRatio_eta_min");
  hEffRatio_phi_min->SetName("hEffRatio_phi_min");
  hEffRatio_y_min->SetName("hEffRatio_y_min");

  hEffRatio_pT_max->SetName("hEffRatio_pT_max");
  hEffRatio_eta_max->SetName("hEffRatio_eta_max");
  hEffRatio_phi_max->SetName("hEffRatio_phi_max");
  hEffRatio_y_max->SetName("hEffRatio_y_max");

  hEffRatio_pT->Divide(hEff_pT,hEff_pT_KS0);
  hEffRatio_eta->Divide(hEff_eta,hEff_eta_KS0);
  hEffRatio_phi->Divide(hEff_phi,hEff_phi_KS0);
  hEffRatio_y->Divide(hEff_y,hEff_y_KS0);

  hEffRatio_pT_min->Divide(hEff_pT_min,hEff_pT_KS0_min);
  hEffRatio_eta_min->Divide(hEff_eta_min,hEff_eta_KS0_min);
  hEffRatio_phi_min->Divide(hEff_phi_min,hEff_phi_KS0_min);
  hEffRatio_y_min->Divide(hEff_y_min,hEff_y_KS0_min);

  hEffRatio_pT_max->Divide(hEff_pT_max,hEff_pT_KS0_max);
  hEffRatio_eta_max->Divide(hEff_eta_max,hEff_eta_KS0_max);
  hEffRatio_phi_max->Divide(hEff_phi_max,hEff_phi_KS0_max);
  hEffRatio_y_max->Divide(hEff_y_max,hEff_y_KS0_max);

  TFile* fout = new TFile("LamdbaKsCutEfficiency_200MeV_hists.root","RECREATE");

  hEff_pT->Write();
  hEff_eta->Write();
  hEff_phi->Write();
  hEff_y->Write();

  hEff_pT_KS0->Write();
  hEff_eta_KS0->Write();
  hEff_phi_KS0->Write();
  hEff_y_KS0->Write();

  hEffRatio_pT->Write();
  hEffRatio_eta->Write();
  hEffRatio_phi->Write();
  hEffRatio_y->Write();

  hEff_pT_min->Write();
  hEff_eta_min->Write();
  hEff_phi_min->Write();
  hEff_y_min->Write();

  hEff_pT_KS0_min->Write();
  hEff_eta_KS0_min->Write();
  hEff_phi_KS0_min->Write();
  hEff_y_KS0_min->Write();

  hEffRatio_pT_min->Write();
  hEffRatio_eta_min->Write();
  hEffRatio_phi_min->Write();
  hEffRatio_y_min->Write();

  hEff_pT_max->Write();
  hEff_eta_max->Write();
  hEff_phi_max->Write();
  hEff_y_max->Write();

  hEff_pT_KS0_max->Write();
  hEff_eta_KS0_max->Write();
  hEff_phi_KS0_max->Write();
  hEff_y_KS0_max->Write();

  hEffRatio_pT_max->Write();
  hEffRatio_eta_max->Write();
  hEffRatio_phi_max->Write();
  hEffRatio_y_max->Write();

  TH1F* syserr_pT = new TH1F("cuteff_syserr_pT","systematic error from cut efficiency",nBins_pT,pTbins.data());
  TH1F* syserr_eta = new TH1F("cuteff_syserr_eta","systematic error from cut efficiency",nBins_eta,etabins.data());
  TH1F* syserr_phi = new TH1F("cuteff_syserr_phi","systematic error from cut efficiency",nBins_phi,phibins.data());
  TH1F* syserr_y = new TH1F("cuteff_syserr_y","systematic error from cut efficiency",nBins_rap,rapbins.data());

  for(int i=1;i<=syserr_pT->GetNbinsX();i++)
  {
    std::pair<double,double> extremes = std::minmax({hEffRatio_pT_min->GetBinContent(i),hEffRatio_pT->GetBinContent(i),hEffRatio_pT_max->GetBinContent(i)});
    syserr_pT->SetBinContent(i,0.5*fabs(extremes.second-extremes.first));
  }

  for(int i=1;i<=syserr_eta->GetNbinsX();i++)
  {
    std::pair<double,double> extremes = std::minmax({hEffRatio_eta_min->GetBinContent(i),hEffRatio_eta->GetBinContent(i),hEffRatio_eta_max->GetBinContent(i)});
    syserr_eta->SetBinContent(i,0.5*fabs(extremes.second-extremes.first));
  }

  for(int i=1;i<=syserr_phi->GetNbinsX();i++)
  {
    std::pair<double,double> extremes = std::minmax({hEffRatio_phi_min->GetBinContent(i),hEffRatio_phi->GetBinContent(i),hEffRatio_phi_max->GetBinContent(i)});
    syserr_phi->SetBinContent(i,0.5*fabs(extremes.second-extremes.first));
  }

  for(int i=1;i<=syserr_y->GetNbinsX();i++)
  {
    std::pair<double,double> extremes = std::minmax({hEffRatio_y_min->GetBinContent(i),hEffRatio_y->GetBinContent(i),hEffRatio_y_max->GetBinContent(i)});
    syserr_y->SetBinContent(i,0.5*fabs(extremes.second-extremes.first));
  }

  syserr_pT->Write();
  syserr_eta->Write();
  syserr_phi->Write();
  syserr_y->Write();

  fout->Close();

  // -------------------------------------------------------------------------
  // Build TEfficiency objects for Xi and KS0
  // -------------------------------------------------------------------------
  TEfficiency* eff_pT_Xi  = new TEfficiency(*h_pT_truthCuts,  *h_pT_truth);
  TEfficiency* eff_eta_Xi = new TEfficiency(*h_eta_truthCuts, *h_eta_truth);
  TEfficiency* eff_phi_Xi = new TEfficiency(*h_phi_truthCuts, *h_phi_truth);
  TEfficiency* eff_y_Xi   = new TEfficiency(*h_y_truthCuts,   *h_y_truth);

  TEfficiency* eff_pT_KS0  = new TEfficiency(*h_pT_truthCuts_KS0,  *h_pT_truth_KS0);
  TEfficiency* eff_eta_KS0 = new TEfficiency(*h_eta_truthCuts_KS0, *h_eta_truth_KS0);
  TEfficiency* eff_phi_KS0 = new TEfficiency(*h_phi_truthCuts_KS0, *h_phi_truth_KS0);
  TEfficiency* eff_y_KS0   = new TEfficiency(*h_y_truthCuts_KS0,   *h_y_truth_KS0);

  TEfficiency* eff_pT_Xi_min  = new TEfficiency(*h_pT_truthCuts_min,  *h_pT_truth);
  TEfficiency* eff_eta_Xi_min = new TEfficiency(*h_eta_truthCuts_min, *h_eta_truth);
  TEfficiency* eff_phi_Xi_min = new TEfficiency(*h_phi_truthCuts_min, *h_phi_truth);
  TEfficiency* eff_y_Xi_min   = new TEfficiency(*h_y_truthCuts_min,   *h_y_truth);

  TEfficiency* eff_pT_KS0_min  = new TEfficiency(*h_pT_truthCuts_KS0_min,  *h_pT_truth_KS0);
  TEfficiency* eff_eta_KS0_min = new TEfficiency(*h_eta_truthCuts_KS0_min, *h_eta_truth_KS0);
  TEfficiency* eff_phi_KS0_min = new TEfficiency(*h_phi_truthCuts_KS0_min, *h_phi_truth_KS0);
  TEfficiency* eff_y_KS0_min   = new TEfficiency(*h_y_truthCuts_KS0_min,   *h_y_truth_KS0);

  TEfficiency* eff_pT_Xi_max  = new TEfficiency(*h_pT_truthCuts_max,  *h_pT_truth);
  TEfficiency* eff_eta_Xi_max = new TEfficiency(*h_eta_truthCuts_max, *h_eta_truth);
  TEfficiency* eff_phi_Xi_max = new TEfficiency(*h_phi_truthCuts_max, *h_phi_truth);
  TEfficiency* eff_y_Xi_max   = new TEfficiency(*h_y_truthCuts_max,   *h_y_truth);

  TEfficiency* eff_pT_KS0_max  = new TEfficiency(*h_pT_truthCuts_KS0_max,  *h_pT_truth_KS0);
  TEfficiency* eff_eta_KS0_max = new TEfficiency(*h_eta_truthCuts_KS0_max, *h_eta_truth_KS0);
  TEfficiency* eff_phi_KS0_max = new TEfficiency(*h_phi_truthCuts_KS0_max, *h_phi_truth_KS0);
  TEfficiency* eff_y_KS0_max   = new TEfficiency(*h_y_truthCuts_KS0_max,   *h_y_truth_KS0);

  for (auto* e : {eff_pT_Xi, eff_eta_Xi, eff_phi_Xi, eff_y_Xi,
		  eff_pT_KS0, eff_eta_KS0, eff_phi_KS0, eff_y_KS0,
                  eff_pT_Xi_min, eff_eta_Xi_min, eff_phi_Xi_min, eff_y_Xi_min,
                  eff_pT_KS0_min, eff_eta_KS0_min, eff_phi_KS0_min, eff_y_KS0_min,
                  eff_pT_Xi_max, eff_eta_Xi_max, eff_phi_Xi_max, eff_y_Xi_max,
                  eff_pT_KS0_max, eff_eta_KS0_max, eff_phi_KS0_max, eff_y_KS0_max})
      e->SetStatisticOption(TEfficiency::kFCP);

  auto makeEffGraph = [](TEfficiency* eff) -> TGraphAsymmErrors*
  {
      int n = eff->GetTotalHistogram()->GetNbinsX();
      TGraphAsymmErrors* g = new TGraphAsymmErrors(n);
      g->SetTitle("");
      for (int i = 1; i <= n; ++i)
      {
	  double x    = eff->GetTotalHistogram()->GetBinCenter(i);
	  double exlo = eff->GetTotalHistogram()->GetBinCenter(i) - eff->GetTotalHistogram()->GetBinLowEdge(i);
	  double exhi = eff->GetTotalHistogram()->GetBinLowEdge(i+1) - eff->GetTotalHistogram()->GetBinCenter(i);
	  double y    = eff->GetEfficiency(i);
	  double eylo = eff->GetEfficiencyErrorLow(i);
	  double eyhi = eff->GetEfficiencyErrorUp(i);
	  g->SetPoint(i-1, x, y);
	  g->SetPointError(i-1, exlo, exhi, eylo, eyhi);
      }
      return g;
  };

  auto makeRatioGraph = [](TGraphAsymmErrors* gXi, TGraphAsymmErrors* gKS0,
			   bool XiOverKS0) -> TGraphAsymmErrors*
  {
      int n = gXi->GetN();
      TGraphAsymmErrors* ratio = new TGraphAsymmErrors(n);
      ratio->SetTitle("");
      for (int i = 0; i < n; ++i)
      {
	  double x, yXi, yKS0;
	  gXi ->GetPoint(i, x, yXi);
	  double dummy;
	  gKS0->GetPoint(i, dummy, yKS0);

	  double exlo = gXi->GetErrorXlow(i);
	  double exhi = gXi->GetErrorXhigh(i);

	  // Numerator and denominator depend on which ratio we want
	  double num    = XiOverKS0 ? yXi  : yKS0;
	  double den    = XiOverKS0 ? yKS0 : yXi;

	  double numElo = XiOverKS0 ? gXi->GetErrorYlow(i)  : gKS0->GetErrorYlow(i);
	  double numEhi = XiOverKS0 ? gXi->GetErrorYhigh(i) : gKS0->GetErrorYhigh(i);
	  double denElo = XiOverKS0 ? gKS0->GetErrorYlow(i) : gXi->GetErrorYlow(i);
	  double denEhi = XiOverKS0 ? gKS0->GetErrorYhigh(i): gXi->GetErrorYhigh(i);

	  if (den <= 0.)
	  {
	      ratio->SetPoint(i, x, 0.);
	      ratio->SetPointError(i, exlo, exhi, 0., 0.);
	      continue;
	  }

	  double r = num / den;

	  // Error propagation for ratio f = a/b:
	  // delta_f = f * sqrt((delta_a/a)^2 + (delta_b/b)^2)
	  // Use asymmetric errors: propagate lo and hi separately
	  double eylo = (num > 0.) ? r * sqrt(pow(numElo/num, 2) + pow(denEhi/den, 2)) : 0.;
	  double eyhi = (num > 0.) ? r * sqrt(pow(numEhi/num, 2) + pow(denElo/den, 2)) : 0.;

	  ratio->SetPoint(i, x, r);
	  ratio->SetPointError(i, exlo, exhi, eylo, eyhi);
      }
      return ratio;
  };

  TGraphAsymmErrors* g_pT_Xi  = makeEffGraph(eff_pT_Xi);
  TGraphAsymmErrors* g_pT_KS0 = makeEffGraph(eff_pT_KS0);
  TGraphAsymmErrors* g_eta_Xi  = makeEffGraph(eff_eta_Xi);
  TGraphAsymmErrors* g_eta_KS0 = makeEffGraph(eff_eta_KS0);
  TGraphAsymmErrors* g_phi_Xi  = makeEffGraph(eff_phi_Xi);
  TGraphAsymmErrors* g_phi_KS0 = makeEffGraph(eff_phi_KS0);
  TGraphAsymmErrors* g_y_Xi    = makeEffGraph(eff_y_Xi);
  TGraphAsymmErrors* g_y_KS0   = makeEffGraph(eff_y_KS0);

  // Xi/KS0 ratios
  TGraphAsymmErrors* ratio_pT_XiOverKS0  = makeRatioGraph(g_pT_Xi,  g_pT_KS0,  true);
  TGraphAsymmErrors* ratio_eta_XiOverKS0 = makeRatioGraph(g_eta_Xi, g_eta_KS0, true);
  TGraphAsymmErrors* ratio_phi_XiOverKS0 = makeRatioGraph(g_phi_Xi, g_phi_KS0, true);
  TGraphAsymmErrors* ratio_y_XiOverKS0   = makeRatioGraph(g_y_Xi,   g_y_KS0,   true);

  // KS0/Xi ratios
  TGraphAsymmErrors* ratio_pT_KS0OverXi  = makeRatioGraph(g_pT_Xi,  g_pT_KS0,  false);
  TGraphAsymmErrors* ratio_eta_KS0OverXi = makeRatioGraph(g_eta_Xi, g_eta_KS0, false);
  TGraphAsymmErrors* ratio_phi_KS0OverXi = makeRatioGraph(g_phi_Xi, g_phi_KS0, false);
  TGraphAsymmErrors* ratio_y_KS0OverXi   = makeRatioGraph(g_y_Xi,   g_y_KS0,   false);
 
  TCanvas *c1  = new TCanvas("myCanvas", "myCanvas",800,800);
  ratio_pT_XiOverKS0->Draw("AP");
  ratio_pT_XiOverKS0->GetXaxis()->SetTitle("Mother p_{T} [GeV/c]");
  ratio_pT_XiOverKS0->GetYaxis()->SetTitle("#Lambda/K_{S}^{0} Cut Efficiency");
  ratio_pT_XiOverKS0->GetYaxis()->SetRangeUser(0.0, 1.05);
  ratio_pT_XiOverKS0->GetXaxis()->SetLimits(pTbins[0], pTbins[nBins_pT]);
  c1->Update();
  
  TPaveText *pt;
  pt = new TPaveText(0.15,0.75,0.65,1., "NDC");
  pt->SetFillColor(0);
  pt->SetFillStyle(0);
  pt->SetTextFont(42);
  string label = "#it{#bf{sPHENIX}} Simulation";
  TText *pt_LaTex = pt->AddText(label.c_str());
  pt->SetBorderSize(0);
  pt->Draw();
  gPad->Modified();

  TPaveText *ptDate;
  ptDate = new TPaveText(0.67,0.79,0.85,0.95, "NDC");
  ptDate->SetFillColor(0);
  ptDate->SetFillStyle(0);
  ptDate->SetTextFont(42);
  std::string compilation_date = getDate();
  TText *pt_LaTexDate = ptDate->AddText(compilation_date.c_str());
  ptDate->SetBorderSize(0);
  ptDate->Draw();
  gPad->Modified();

  string extensions[] = {"_250MeV.png","_250MeV.root"};
  for (auto extension : extensions)
  {
    string output = "LambdaKS0ratio_pT_cutEff" + extension;
    c1->SaveAs(output.c_str());
  }
  c1->Close();  

  TCanvas *c2  = new TCanvas("myCanvas", "myCanvas",800,800);
  ratio_eta_XiOverKS0->Draw("AP");
  ratio_eta_XiOverKS0->GetXaxis()->SetTitle("Mother #eta");
  ratio_eta_XiOverKS0->GetYaxis()->SetTitle("#Lambda/K_{S}^{0} Cut Efficiency");
  ratio_eta_XiOverKS0->GetYaxis()->SetRangeUser(0.0, 1.05);
  ratio_eta_XiOverKS0->GetXaxis()->SetLimits(etabins[0], etabins[nBins_eta]);
  c2->Update();
  
  pt->Draw();
  gPad->Modified();

  ptDate->Draw();
  gPad->Modified();

  for (auto extension : extensions)
  {
    string output = "LambdaKS0ratio_eta_cutEff" + extension;
    c2->SaveAs(output.c_str());
  }
  c2->Close();  
  
  TCanvas *c3  = new TCanvas("myCanvas", "myCanvas",800,800);
  ratio_phi_XiOverKS0->Draw("AP");
  ratio_phi_XiOverKS0->GetXaxis()->SetTitle("Mother #phi [rad]");
  ratio_phi_XiOverKS0->GetYaxis()->SetTitle("#Lambda/K_{S}^{0} Cut Efficiency");
  ratio_phi_XiOverKS0->GetYaxis()->SetRangeUser(0.0, 1.05);
  ratio_phi_XiOverKS0->GetXaxis()->SetLimits(phibins[0], phibins[nBins_phi]);
  c3->Update();
  
  pt->Draw();
  gPad->Modified();

  ptDate->Draw();
  gPad->Modified();

  for (auto extension : extensions)
  {
    string output = "LambdaKS0ratio_phi_cutEff" + extension;
    c3->SaveAs(output.c_str());
  }
  c3->Close();  

  TCanvas *c4  = new TCanvas("myCanvas", "myCanvas",800,800);
  ratio_y_XiOverKS0->Draw("AP");
  ratio_y_XiOverKS0->GetXaxis()->SetTitle("Mother Rapidity");
  ratio_y_XiOverKS0->GetYaxis()->SetTitle("#Lambda/K_{S}^{0} Cut Efficiency");
  ratio_y_XiOverKS0->GetYaxis()->SetRangeUser(0.0, 1.05);
  ratio_y_XiOverKS0->GetXaxis()->SetLimits(rapbins[0], rapbins[nBins_rap]);
  c4->Update();
  
  pt->Draw();
  gPad->Modified();

  ptDate->Draw();
  gPad->Modified();

  for (auto extension : extensions)
  {
    string output = "LambdaKS0ratio_y_cutEff" + extension;
    c4->SaveAs(output.c_str());
  }
  c4->Close();  
 
  /*
  c1->Divide(2,2);
  //pT 
  c1->cd(1);
  
  //ratio_pT_XiOverKS0->Draw("AP");
  //ratio_pT_XiOverKS0->GetXaxis()->SetTitle("Mother p_{T} [GeV/c]");
  //ratio_pT_XiOverKS0->GetYaxis()->SetTitle("#Xi^{-}/K_{S}^{0} Cut Efficiency");
  //ratio_pT_XiOverKS0->GetYaxis()->SetRangeUser(0.0, 1.05);
  //ratio_pT_XiOverKS0->GetXaxis()->SetLimits(pTbins[0], pTbins[nBins_pT]);
 
  ratio_pT_KS0OverXi->Draw("AP");
  ratio_pT_KS0OverXi->GetXaxis()->SetTitle("Mother p_{T} [GeV/c]");
  ratio_pT_KS0OverXi->GetYaxis()->SetTitle("K_{S}^{0}/#Xi^{-} Cut Efficiency");
  ratio_pT_KS0OverXi->GetYaxis()->SetRangeUser(0.0, 5.00);
  ratio_pT_KS0OverXi->GetXaxis()->SetLimits(pTbins[0], pTbins[nBins_pT]);
  c1->Update();

  TPaveText *pt;
  pt = new TPaveText(0.15,0.75,0.65,1., "NDC");
  pt->SetFillColor(0);
  pt->SetFillStyle(0);
  pt->SetTextFont(42);
  string label = "#it{#bf{sPHENIX}} Simulation";
  TText *pt_LaTex = pt->AddText(label.c_str());
  pt->SetBorderSize(0);
  pt->Draw();
  gPad->Modified();

  TPaveText *ptDate;
  ptDate = new TPaveText(0.67,0.79,0.85,0.95, "NDC");
  ptDate->SetFillColor(0);
  ptDate->SetFillStyle(0);
  ptDate->SetTextFont(42);
  std::string compilation_date = getDate();
  TText *pt_LaTexDate = ptDate->AddText(compilation_date.c_str());
  ptDate->SetBorderSize(0);
  ptDate->Draw();
  gPad->Modified();
 
  //eta
  c1->cd(2);
  ratio_eta_KS0OverXi->Draw("AP");
  ratio_eta_KS0OverXi->GetXaxis()->SetTitle("Mother #eta");
  ratio_eta_KS0OverXi->GetYaxis()->SetTitle("K_{S}^{0}/#Xi^{-} Cut Efficiency");
  ratio_eta_KS0OverXi->GetYaxis()->SetRangeUser(0.0, 5.0);
  ratio_eta_KS0OverXi->GetXaxis()->SetLimits(etabins[0], etabins[nBins_eta]);
  c1->Update();

  pt->Draw();
  gPad->Modified();

  ptDate->Draw();
  gPad->Modified();
  
  //phi
  c1->cd(3);
  ratio_phi_KS0OverXi->Draw("AP");
  ratio_phi_KS0OverXi->GetXaxis()->SetTitle("Mother #phi [rad]");
  ratio_phi_KS0OverXi->GetYaxis()->SetTitle("K_{S}^{0}/#Xi^{-} Cut Efficiency");
  ratio_phi_KS0OverXi->GetYaxis()->SetRangeUser(0.0, 5.0);
  ratio_phi_KS0OverXi->GetXaxis()->SetLimits(phibins[0], phibins[nBins_phi]);
  c1->Update();

  pt->Draw();
  gPad->Modified();

  ptDate->Draw();
  gPad->Modified();
  
  //rapidity
  c1->cd(4);
  ratio_y_KS0OverXi->Draw("AP");
  ratio_y_KS0OverXi->GetXaxis()->SetTitle("Mother Rapidity");
  ratio_y_KS0OverXi->GetYaxis()->SetTitle("K_{S}^{0}/#Xi^{-} Cut Efficiency");
  ratio_y_KS0OverXi->GetYaxis()->SetRangeUser(0.0, 5.0);
  ratio_y_KS0OverXi->GetXaxis()->SetLimits(-1.,1.);
  c1->Update();

  pt->Draw();
  gPad->Modified();

  ptDate->Draw();
  gPad->Modified();
   

  string extensions[] = {".png",".root"};
  for (auto extension : extensions)
  {
    //string output = "XiKS0ratio_all_cutEff" + extension;
    string output = "KS0Xiratio_all_cutEff" + extension;
    c1->SaveAs(output.c_str());
  }
  */
} 
