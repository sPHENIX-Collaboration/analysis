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
  string sim_inFile_Xi = "/sphenix/user/rosstom/TrackingAnalysis/Physics_Val_TF/sim/pTComp/KFParticle_4Mcascade_nopTCut.root";
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

  std::string truthCut = "abs(Lambda0_track_1_true_ID) == 211 && Sum$(abs(Lambda0_track_1_true_track_history_PDG_ID) == 3122) > 0 && Sum$(abs(Lambda0_track_1_true_track_history_PDG_ID) == 3312) > 0 && abs(Lambda0_track_2_true_ID) == 2212 && Sum$(abs(Lambda0_track_2_true_track_history_PDG_ID) == 3122) > 0 && Sum$(abs(Lambda0_track_2_true_track_history_PDG_ID) == 3312) > 0 && abs(track_3_true_ID) == 211 && Sum$(abs(track_3_true_track_history_PDG_ID) == 3312) > 0 && Sum$(abs(track_3_true_track_history_PDG_ID) == 3122) == 0 && abs(Ximinus_rapidity) <= 1.0 && Lambda0_track_1_MVTX_nHits > 0 && Lambda0_track_2_MVTX_nHits > 0 && track_3_MVTX_nHits > 0 && abs(primary_vertex_z) < 10 && Lambda0_track_1_charge == track_3_charge";
  std::string truthDataCut = "abs(Lambda0_track_1_true_ID) == 211 && Sum$(abs(Lambda0_track_1_true_track_history_PDG_ID) == 3122) > 0 && Sum$(abs(Lambda0_track_1_true_track_history_PDG_ID) == 3312) > 0 && abs(Lambda0_track_2_true_ID) == 2212 && Sum$(abs(Lambda0_track_2_true_track_history_PDG_ID) == 3122) > 0 && Sum$(abs(Lambda0_track_2_true_track_history_PDG_ID) == 3312) > 0 && abs(track_3_true_ID) == 211 && Sum$(abs(track_3_true_track_history_PDG_ID) == 3312) > 0 && Sum$(abs(track_3_true_track_history_PDG_ID) == 3122) == 0 && Lambda0_track_1_MVTX_nHits > 0 && Lambda0_track_2_MVTX_nHits > 0 && track_3_MVTX_nHits > 0 && abs(primary_vertex_z) < 10 && Lambda0_track_1_charge == track_3_charge && Lambda0_track_1_INTT_nHits > 0 && Lambda0_track_2_INTT_nHits > 0 && track_3_INTT_nHits > 0 && Lambda0_track_1_TPC_nHits > 19 && Lambda0_track_2_TPC_nHits > 19 && track_3_TPC_nHits > 19 && abs(Lambda0_mass - 1.1157) <= 0.01 && Ximinus_decayLength_xy >= 0.15 && Lambda0_decayLength_xy >= 0.01 && (Lambda0_track_1_chi2/Lambda0_track_1_nDoF) <= 400 && (Lambda0_track_2_chi2/Lambda0_track_2_nDoF) <= 400 && (track_3_chi2/track_3_nDoF) <= 400 && track_1_track_2_DCA <= 0.5 && track_1_track_3_DCA <= 0.5 && track_2_track_3_DCA <= 0.5 && Ximinus_chi2/Ximinus_nDoF <= 50 && Lambda0_chi2/Lambda0_nDoF <= 50 && abs(Ximinus_rapidity) <= 1.0 && Lambda0_track_1_pT >= 0.2 && Lambda0_track_2_pT >= 0.2 && track_3_pT >= 0.2";

  TH1F* h_pT_truth = new TH1F("h_pT_truth",";#Xi^{-} p_{T} [GeV/c];Cut Efficiency",nBins_pT,pTbins);
  TH1F* h_pT_truthCuts = new TH1F("h_pT_truthCuts",";#Xi^{-} p_{T} [GeV/c];Cut Efficiency",nBins_pT,pTbins);
  
  TH1F* h_eta_truth = new TH1F("h_eta_truth",";#Xi^{-} #eta;Cut Efficiency",nBins_eta,etabins);
  TH1F* h_eta_truthCuts = new TH1F("h_eta_truthCuts",";#Xi^{-} #eta;Cut Efficiency",nBins_eta,etabins);
  
  TH1F* h_phi_truth = new TH1F("h_phi_truth",";#Xi^{-} #phi;Cut Efficiency",nBins_phi,phibins);
  TH1F* h_phi_truthCuts = new TH1F("h_phi_truthCuts",";#Xi^{-} #phi;Cut Efficiency",nBins_phi,phibins);
  
  TH1F* h_y_truth = new TH1F("h_y_truth",";#Xi^{-} y;Cut Efficiency",nBins_rap,rapbins);
  TH1F* h_y_truthCuts = new TH1F("h_y_truthCuts",";#Xi^{-} y;Cut Efficiency",nBins_rap,rapbins);
   
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

  string sim_inFile_KS0 = "/sphenix/user/cdean/software/analysis/LightFlavorRatios/geometric_acceptance/simulation/outputKFParticle_Kshort_reco.root";
  TFile* sim_file_KS0 = new TFile(sim_inFile_KS0.c_str());
  TTree* sim_tree_KS0 = (TTree*)sim_file_KS0->Get("DecayTree");

  std::string truthCut_KS0 = "abs(track_1_true_ID) == 211 && Sum$(abs(track_1_true_track_history_PDG_ID) == 310) > 0 && abs(track_2_true_ID) == 211 && Sum$(abs(track_2_true_track_history_PDG_ID) == 310) > 0 && abs(K_S0_rapidity) <= 1.0 && track_1_MVTX_nHits > 0 && track_2_MVTX_nHits > 0 && abs(primary_vertex_z) < 10";
  std::string truthDataCut_KS0 = "abs(track_1_true_ID) == 211 && Sum$(abs(track_1_true_track_history_PDG_ID) == 310) > 0 && abs(track_2_true_ID) == 211 && Sum$(abs(track_2_true_track_history_PDG_ID) == 310) > 0 && abs(K_S0_rapidity) <= 1.0 && track_1_MVTX_nHits > 0 && track_2_MVTX_nHits > 0 && abs(primary_vertex_z) < 10 && track_1_INTT_nHits > 0 && track_2_INTT_nHits > 0 && track_1_TPC_nHits > 19 && track_2_TPC_nHits > 19 && (track_1_chi2/track_1_nDoF) <= 300 && (track_2_chi2/track_2_nDoF) <= 300 && abs(track_1_IP_xy) >= 0.05 && abs(track_2_IP_xy) >= 0.05 && track_1_track_2_DCA <= 0.5 && track_1_track_2_DCA_xy <= 1.0 && K_S0_DIRA >= 0.99 && (K_S0_chi2/K_S0_nDoF) <= 20 && abs(K_S0_mass - 0.5) <= 0.1 && track_1_pT >= 0.2 && track_2_pT >= 0.2";

  TH1F* h_pT_truth_KS0 = new TH1F("h_pT_truth_KS0",";K_{S}^{0} p_{T} [GeV/c];Cut Efficiency",nBins_pT,pTbins);
  TH1F* h_pT_truthCuts_KS0 = new TH1F("h_pT_truthCuts_KS0",";K_{S}^{0} p_{T} [GeV/c];Cut Efficiency",nBins_pT,pTbins);
  
  TH1F* h_eta_truth_KS0 = new TH1F("h_eta_truth_KS0",";K_{S}^{0} #eta;Cut Efficiency",nBins_eta,etabins);
  TH1F* h_eta_truthCuts_KS0 = new TH1F("h_eta_truthCuts_KS0",";K_{S}^{0} #eta;Cut Efficiency",nBins_eta,etabins);
  
  TH1F* h_phi_truth_KS0 = new TH1F("h_phi_truth_KS0",";K_{S}^{0} #phi;Cut Efficiency",nBins_phi,phibins);
  TH1F* h_phi_truthCuts_KS0 = new TH1F("h_phi_truthCuts_KS0",";K_{S}^{0} #phi;Cut Efficiency",nBins_phi,phibins);
  
  TH1F* h_y_truth_KS0 = new TH1F("h_y_truth_KS0",";K_{S}^{0} y;Cut Efficiency",nBins_rap,rapbins);
  TH1F* h_y_truthCuts_KS0 = new TH1F("h_y_truthCuts_KS0",";K_{S}^{0} y;Cut Efficiency",nBins_rap,rapbins);
   
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

  TH1F* h_cutEff_pT_KS0 = (TH1F*)h_pT_truthCuts_KS0->Clone("h_cutEff_pT_KS0");
  h_cutEff_pT_KS0->Divide(h_pT_truthCuts_KS0, h_pT_truth_KS0, 1, 1, "B"); 
  TH1F* h_cutEff_phi_KS0 = (TH1F*)h_phi_truthCuts_KS0->Clone("h_cutEff_phi_KS0");
  h_cutEff_phi_KS0->Divide(h_phi_truthCuts_KS0, h_phi_truth_KS0, 1, 1, "B"); 
  TH1F* h_cutEff_eta_KS0 = (TH1F*)h_eta_truthCuts_KS0->Clone("h_cutEff_eta_KS0");
  h_cutEff_eta_KS0->Divide(h_eta_truthCuts_KS0, h_eta_truth_KS0, 1, 1, "B"); 
  TH1F* h_cutEff_rap_KS0 = (TH1F*)h_y_truthCuts_KS0->Clone("h_cutEff_rap_KS0");
  h_cutEff_rap_KS0->Divide(h_y_truthCuts_KS0, h_y_truth_KS0, 1, 1, "B"); 
  
  TH1F* h_cutEff_pT_Xi = (TH1F*)h_pT_truthCuts->Clone("h_cutEff_pT_Xi");
  h_cutEff_pT_Xi->Divide(h_pT_truthCuts, h_pT_truth, 1, 1, "B"); 
  TH1F* h_cutEff_phi_Xi = (TH1F*)h_phi_truthCuts->Clone("h_cutEff_phi_Xi");
  h_cutEff_phi_Xi->Divide(h_phi_truthCuts, h_phi_truth, 1, 1, "B"); 
  TH1F* h_cutEff_eta_Xi = (TH1F*)h_eta_truthCuts->Clone("h_cutEff_eta_Xi");
  h_cutEff_eta_Xi->Divide(h_eta_truthCuts, h_eta_truth, 1, 1, "B"); 
  TH1F* h_cutEff_rap_Xi = (TH1F*)h_y_truthCuts->Clone("h_cutEff_rap_Xi");
  h_cutEff_rap_Xi->Divide(h_y_truthCuts, h_y_truth, 1, 1, "B"); 

  TFile* outputFile = TFile::Open("cutEfficiencyCorrections.root", "RECREATE");
  h_cutEff_pT_KS0->Write(); 
  h_cutEff_phi_KS0->Write(); 
  h_cutEff_eta_KS0->Write(); 
  h_cutEff_rap_KS0->Write(); 
  h_cutEff_pT_Xi->Write(); 
  h_cutEff_phi_Xi->Write(); 
  h_cutEff_eta_Xi->Write(); 
  h_cutEff_rap_Xi->Write(); 
  outputFile->Close();

  /*
  for (auto* e : {eff_pT_Xi, eff_eta_Xi, eff_phi_Xi, eff_y_Xi,
		  eff_pT_KS0, eff_eta_KS0, eff_phi_KS0, eff_y_KS0})
      e->SetStatisticOption(TEfficiency::kFCP);

  auto makeEffGraph = [](TEfficiency* eff) -> TGraphAsymmErrors*
  {
      int n = eff->GetTotalHistogram()->GetNbinsX();
      TGraphAsymmErrors* g = new TGraphAsymmErrors(n);
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
  ratio_pT_XiOverKS0->GetYaxis()->SetTitle("#Xi^{-}/K_{S}^{0} Cut Efficiency");
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

  string extensions[] = {".png",".root"};
  for (auto extension : extensions)
  {
    string output = "XiKS0ratio_pT_cutEff" + extension;
    c1->SaveAs(output.c_str());
  }
  c1->Close();  

  TCanvas *c2  = new TCanvas("myCanvas", "myCanvas",800,800);
  ratio_eta_XiOverKS0->Draw("AP");
  ratio_eta_XiOverKS0->GetXaxis()->SetTitle("Mother #eta");
  ratio_eta_XiOverKS0->GetYaxis()->SetTitle("#Xi^{-}/K_{S}^{0} Cut Efficiency");
  ratio_eta_XiOverKS0->GetYaxis()->SetRangeUser(0.0, 1.05);
  ratio_eta_XiOverKS0->GetXaxis()->SetLimits(etabins[0], etabins[nBins_eta]);
  c2->Update();
  
  pt->Draw();
  gPad->Modified();

  ptDate->Draw();
  gPad->Modified();

  for (auto extension : extensions)
  {
    string output = "XiKS0ratio_eta_cutEff" + extension;
    c2->SaveAs(output.c_str());
  }
  c2->Close();  
  
  TCanvas *c3  = new TCanvas("myCanvas", "myCanvas",800,800);
  ratio_phi_XiOverKS0->Draw("AP");
  ratio_phi_XiOverKS0->GetXaxis()->SetTitle("Mother #phi [rad]");
  ratio_phi_XiOverKS0->GetYaxis()->SetTitle("#Xi^{-}/K_{S}^{0} Cut Efficiency");
  ratio_phi_XiOverKS0->GetYaxis()->SetRangeUser(0.0, 1.05);
  ratio_phi_XiOverKS0->GetXaxis()->SetLimits(phibins[0], phibins[nBins_phi]);
  c3->Update();
  
  pt->Draw();
  gPad->Modified();

  ptDate->Draw();
  gPad->Modified();

  for (auto extension : extensions)
  {
    string output = "XiKS0ratio_phi_cutEff" + extension;
    c3->SaveAs(output.c_str());
  }
  c3->Close();  

  TCanvas *c4  = new TCanvas("myCanvas", "myCanvas",800,800);
  ratio_y_XiOverKS0->Draw("AP");
  ratio_y_XiOverKS0->GetXaxis()->SetTitle("Mother Rapidity");
  ratio_y_XiOverKS0->GetYaxis()->SetTitle("#Xi^{-}/K_{S}^{0} Cut Efficiency");
  ratio_y_XiOverKS0->GetYaxis()->SetRangeUser(0.0, 1.05);
  ratio_y_XiOverKS0->GetXaxis()->SetLimits(rapbins[0], rapbins[nBins_rap]);
  c4->Update();
  
  pt->Draw();
  gPad->Modified();

  ptDate->Draw();
  gPad->Modified();

  for (auto extension : extensions)
  {
    string output = "XiKS0ratio_y_cutEff" + extension;
    c4->SaveAs(output.c_str());
  }
  c4->Close();  
  */ 

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
