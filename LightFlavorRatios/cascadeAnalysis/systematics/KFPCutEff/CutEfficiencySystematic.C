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

void CutEfficiencySystematic()
{
  std::string weightFileName = "/sphenix/user/rosstom/TrackingAnalysis/Physics_Val_TF/data/analysisMacros/normalized_sWeight_pT_distributions.root";
  TFile* weightFile = new TFile(weightFileName.c_str());
  TH1F* KS0_pT_histo_Normalized = (TH1F*)weightFile->Get("KS0_pT_histo_Normalized");
  TH1F* Xi_pT_histo_Normalized = (TH1F*)weightFile->Get("Xi_pT_histo_Normalized");
  KS0_pT_histo_Normalized->SetDirectory(0);
  Xi_pT_histo_Normalized->SetDirectory(0);

  string sim_inFile_Xi = "/sphenix/user/rosstom/TrackingAnalysis/Physics_Val_TF/sim/pTComp/KFParticle_4Mcascade_nopTCut.root";
  TFile* sim_file_Xi = new TFile(sim_inFile_Xi.c_str());
  sim_file_Xi->cd();
  TTree* sim_tree_Xi = (TTree*)sim_file_Xi->Get("DecayTree");

  const int nBins_pT = 4;
  double pTbins[nBins_pT + 1] = {0.75, 1.07, 1.25, 1.49, 2.0};

  const int nBins_phi = 6;
  double phibins[nBins_phi + 1] = {-M_PI,-2.09439,-1.04719,0.0,1.04721,2.09441,M_PI};

  const int nBins_eta = 6;
  double etabins[nBins_eta + 1] = {-1.1,-0.733333,-0.366666,0.0,0.366668,0.733335,1.1};

  const int nBins_rap = 6;
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

  std::cout << "Writing Xi base histograms" << std::endl;

  sim_tree_Xi->Draw("Ximinus_pT>>+h_pT_truth",truthCut.c_str());
  sim_tree_Xi->Draw("Ximinus_pT>>+h_pT_truthCuts",truthDataCut.c_str());
  sim_tree_Xi->Draw("Ximinus_pseudorapidity>>+h_eta_truth",truthCut.c_str());
  sim_tree_Xi->Draw("Ximinus_pseudorapidity>>+h_eta_truthCuts",truthDataCut.c_str());
  sim_tree_Xi->Draw("Ximinus_phi>>+h_phi_truth",truthCut.c_str());
  sim_tree_Xi->Draw("Ximinus_phi>>+h_phi_truthCuts",truthDataCut.c_str());
  sim_tree_Xi->Draw("Ximinus_rapidity>>+h_y_truth",truthCut.c_str());
  sim_tree_Xi->Draw("Ximinus_rapidity>>+h_y_truthCuts",truthDataCut.c_str());

  std::cout << "Done writing Xi base histograms" << std::endl;
  
  TH1F* h_cutEff_pT_KS0_denom = new TH1F("h_cutEff_pT_KS0_denom",";K_{S}^{0} p_{T} [GeV/c];Cut Efficiency",nBins_pT,pTbins);
  TH1F* h_cutEff_pT_KS0_num = new TH1F("h_cutEff_pT_KS0_num",";K_{S}^{0} p_{T} [GeV/c];Cut Efficiency",nBins_pT,pTbins);
  TH1F* h_cutEff_eta_KS0_denom = new TH1F("h_cutEff_eta_KS0_denom",";K_{S}^{0} #eta;Cut Efficiency",nBins_eta,etabins);
  TH1F* h_cutEff_eta_KS0_num = new TH1F("h_cutEff_eta_KS0_num",";K_{S}^{0} #eta;Cut Efficiency",nBins_eta,etabins);
  TH1F* h_cutEff_phi_KS0_denom = new TH1F("h_cutEff_phi_KS0_denom",";K_{S}^{0} #phi [rad];Cut Efficiency",nBins_phi,phibins);
  TH1F* h_cutEff_phi_KS0_num = new TH1F("h_cutEff_phi_KS0_num",";K_{S}^{0} #phi [rad];Cut Efficiency",nBins_phi,phibins);
  TH1F* h_cutEff_rap_KS0_denom = new TH1F("h_cutEff_rap_KS0_denom",";K_{S}^{0} Rapidity (y);Cut Efficiency",nBins_rap,rapbins);
  TH1F* h_cutEff_rap_KS0_num = new TH1F("h_cutEff_rap_KS0_num",";K_{S}^{0} Rapidity (y);Cut Efficiency",nBins_rap,rapbins);
  
  TH1F* h_cutEff_pT_Xi_denom = new TH1F("h_cutEff_pT_Xi_denom",";#Xi^{-} p_{T} [GeV/c];Cut Efficiency",nBins_pT,pTbins);
  TH1F* h_cutEff_pT_Xi_num = new TH1F("h_cutEff_pT_Xi_num",";#Xi^{-} p_{T} [GeV/c];Cut Efficiency",nBins_pT,pTbins);
  TH1F* h_cutEff_eta_Xi_denom = new TH1F("h_cutEff_eta_Xi_denom",";#Xi^{-} #eta;Cut Efficiency",nBins_eta,etabins);
  TH1F* h_cutEff_eta_Xi_num = new TH1F("h_cutEff_eta_Xi_num",";#Xi^{-} #eta;Cut Efficiency",nBins_eta,etabins);
  TH1F* h_cutEff_phi_Xi_denom = new TH1F("h_cutEff_phi_Xi_denom",";#Xi^{-} #phi [rad];Cut Efficiency",nBins_phi,phibins);
  TH1F* h_cutEff_phi_Xi_num = new TH1F("h_cutEff_phi_Xi_num",";#Xi^{-} #phi [rad];Cut Efficiency",nBins_phi,phibins);
  TH1F* h_cutEff_rap_Xi_denom = new TH1F("h_cutEff_rap_Xi_denom",";#Xi^{-} Rapidity (y);Cut Efficiency",nBins_rap,rapbins);
  TH1F* h_cutEff_rap_Xi_num = new TH1F("h_cutEff_rap_Xi_num",";#Xi^{-} Rapidity (y);Cut Efficiency",nBins_rap,rapbins);

  h_cutEff_pT_KS0_denom->Sumw2();
  h_cutEff_pT_KS0_num->Sumw2();
  h_cutEff_eta_KS0_denom->Sumw2();
  h_cutEff_eta_KS0_num->Sumw2();
  h_cutEff_phi_KS0_denom->Sumw2();
  h_cutEff_phi_KS0_num->Sumw2();
  h_cutEff_rap_KS0_denom->Sumw2();
  h_cutEff_rap_KS0_num->Sumw2();
  h_cutEff_pT_Xi_denom->Sumw2();
  h_cutEff_pT_Xi_num->Sumw2();
  h_cutEff_eta_Xi_denom->Sumw2();
  h_cutEff_eta_Xi_num->Sumw2();
  h_cutEff_phi_Xi_denom->Sumw2();
  h_cutEff_phi_Xi_num->Sumw2();
  h_cutEff_rap_Xi_denom->Sumw2();
  h_cutEff_rap_Xi_num->Sumw2();

  TFile* outputFile = TFile::Open("cutEfficiencyCorrections.root", "RECREATE");  
  outputFile->cd();
  TTree* XiTree_truth = sim_tree_Xi->CopyTree(truthCut.c_str());
  XiTree_truth->SetName("XiTree_truth");
  std::cout << "Copied XiTree_truth" << std::endl;
  TTree* XiTree_truthCuts = sim_tree_Xi->CopyTree(truthDataCut.c_str());
  XiTree_truthCuts->SetName("XiTree_truthCuts");
  std::cout << "Copied XiTree_truthCuts" << std::endl;

  float Ximinus_pT, Ximinus_eta, Ximinus_phi, Ximinus_rap;
  float K_S0_pT, K_S0_eta, K_S0_phi, K_S0_rap;
  float weight;
  XiTree_truth->SetBranchAddress("Ximinus_pT", &Ximinus_pT);
  XiTree_truth->SetBranchAddress("Ximinus_pseudorapidity", &Ximinus_eta);
  XiTree_truth->SetBranchAddress("Ximinus_phi", &Ximinus_phi);
  XiTree_truth->SetBranchAddress("Ximinus_rapidity", &Ximinus_rap);
  for (int i = 0; i < XiTree_truth->GetEntries(); i++)
  {
    XiTree_truth->GetEntry(i);

    if (Ximinus_pT < 0 || Ximinus_pT > 5)
    {
      weight = 0;
    }  
    else
    {
      int binNumber = Xi_pT_histo_Normalized->GetXaxis()->FindBin(Ximinus_pT);
      weight = Xi_pT_histo_Normalized->GetBinContent(binNumber);
    }

    h_cutEff_pT_Xi_denom->Fill(Ximinus_pT, weight); 
    h_cutEff_eta_Xi_denom->Fill(Ximinus_eta, weight); 
    h_cutEff_phi_Xi_denom->Fill(Ximinus_phi, weight); 
    h_cutEff_rap_Xi_denom->Fill(Ximinus_rap, weight); 
  }
  std::cout << "Filled Xi denom" << std::endl;
  
  XiTree_truthCuts->SetBranchAddress("Ximinus_pT", &Ximinus_pT);
  XiTree_truthCuts->SetBranchAddress("Ximinus_pseudorapidity", &Ximinus_eta);
  XiTree_truthCuts->SetBranchAddress("Ximinus_phi", &Ximinus_phi);
  XiTree_truthCuts->SetBranchAddress("Ximinus_rapidity", &Ximinus_rap);
  for (int i = 0; i < XiTree_truthCuts->GetEntries(); i++)
  {
    XiTree_truthCuts->GetEntry(i);
    
    if (Ximinus_pT < 0 || Ximinus_pT > 5)
    {
      weight = 0;
    }  
    else
    {
      int binNumber = Xi_pT_histo_Normalized->GetXaxis()->FindBin(Ximinus_pT);
      weight = Xi_pT_histo_Normalized->GetBinContent(binNumber);
    }

    h_cutEff_pT_Xi_num->Fill(Ximinus_pT, weight); 
    h_cutEff_eta_Xi_num->Fill(Ximinus_eta, weight); 
    h_cutEff_phi_Xi_num->Fill(Ximinus_phi, weight); 
    h_cutEff_rap_Xi_num->Fill(Ximinus_rap, weight); 
  } 
  std::cout << "Filled Xi num" << std::endl;

  string sim_inFile_KS0 = "/sphenix/user/cdean/software/analysis/LightFlavorRatios/geometric_acceptance/simulation/outputKFParticle_Kshort_reco.root";
  TFile* sim_file_KS0 = new TFile(sim_inFile_KS0.c_str());
  sim_file_KS0->cd();
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
  std::cout << "Filled KS0 base" << std::endl;

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

  outputFile->cd();
  TTree* KS0Tree_truth = sim_tree_KS0->CopyTree(truthCut_KS0.c_str());
  KS0Tree_truth->SetName("KS0Tree_truth");
  std::cout << "Copied KS0Tree_truth" << std::endl;
  TTree* KS0Tree_truthCuts = sim_tree_KS0->CopyTree(truthDataCut_KS0.c_str());
  KS0Tree_truthCuts->SetName("KS0Tree_truthCuts");
  std::cout << "Copied KS0Tree_truthCuts" << std::endl;

  KS0Tree_truth->SetBranchAddress("K_S0_pT", &K_S0_pT);
  KS0Tree_truth->SetBranchAddress("K_S0_pseudorapidity", &K_S0_eta);
  KS0Tree_truth->SetBranchAddress("K_S0_phi", &K_S0_phi);
  KS0Tree_truth->SetBranchAddress("K_S0_rapidity", &K_S0_rap);
  for (int i = 0; i < KS0Tree_truth->GetEntries(); i++)
  {
    KS0Tree_truth->GetEntry(i);
    
    if (K_S0_pT < 0 || K_S0_pT > 5)
    {
      weight = 0;
    }  
    else
    {
      int binNumber = KS0_pT_histo_Normalized->GetXaxis()->FindBin(K_S0_pT);
      weight = KS0_pT_histo_Normalized->GetBinContent(binNumber);
    }

    h_cutEff_pT_KS0_denom->Fill(K_S0_pT, weight); 
    h_cutEff_eta_KS0_denom->Fill(K_S0_eta, weight); 
    h_cutEff_phi_KS0_denom->Fill(K_S0_phi, weight); 
    h_cutEff_rap_KS0_denom->Fill(K_S0_rap, weight); 
  }
  std::cout << "Wrote KS0 denom" << std::endl;
  
  KS0Tree_truthCuts->SetBranchAddress("K_S0_pT", &K_S0_pT);
  KS0Tree_truthCuts->SetBranchAddress("K_S0_pseudorapidity", &K_S0_eta);
  KS0Tree_truthCuts->SetBranchAddress("K_S0_phi", &K_S0_phi);
  KS0Tree_truthCuts->SetBranchAddress("K_S0_rapidity", &K_S0_rap);
  for (int i = 0; i < KS0Tree_truthCuts->GetEntries(); i++)
  {
    KS0Tree_truthCuts->GetEntry(i);
    
    if (K_S0_pT < 0 || K_S0_pT > 5)
    {
      weight = 0;
    }  
    else
    {
      int binNumber = KS0_pT_histo_Normalized->GetXaxis()->FindBin(K_S0_pT);
      weight = KS0_pT_histo_Normalized->GetBinContent(binNumber);
    }

    h_cutEff_pT_KS0_num->Fill(K_S0_pT, weight); 
    h_cutEff_eta_KS0_num->Fill(K_S0_eta, weight); 
    h_cutEff_phi_KS0_num->Fill(K_S0_phi, weight); 
    h_cutEff_rap_KS0_num->Fill(K_S0_rap, weight); 
  }
  std::cout << "Wrote KS0 num" << std::endl;

  TH1F* h_cutEff_pT_KS0_max = (TH1F*)h_cutEff_pT_KS0_num->Clone("h_cutEff_pT_KS0_max");
  h_cutEff_pT_KS0_max->Divide(h_cutEff_pT_KS0_num, h_cutEff_pT_KS0_denom, 1., 1.);
  TH1F* h_cutEff_eta_KS0_max = (TH1F*)h_cutEff_eta_KS0_num->Clone("h_cutEff_eta_KS0_max");
  h_cutEff_eta_KS0_max->Divide(h_cutEff_eta_KS0_num, h_cutEff_eta_KS0_denom, 1., 1.);
  TH1F* h_cutEff_phi_KS0_max = (TH1F*)h_cutEff_phi_KS0_num->Clone("h_cutEff_phi_KS0_max");
  h_cutEff_phi_KS0_max->Divide(h_cutEff_phi_KS0_num, h_cutEff_phi_KS0_denom, 1., 1.);
  TH1F* h_cutEff_rap_KS0_max = (TH1F*)h_cutEff_rap_KS0_num->Clone("h_cutEff_rap_KS0_max");
  h_cutEff_rap_KS0_max->Divide(h_cutEff_rap_KS0_num, h_cutEff_rap_KS0_denom, 1., 1.);
  TH1F* h_cutEff_pT_Xi_max = (TH1F*)h_cutEff_pT_Xi_num->Clone("h_cutEff_pT_Xi_max");
  h_cutEff_pT_Xi_max->Divide(h_cutEff_pT_Xi_num, h_cutEff_pT_Xi_denom, 1., 1.);
  TH1F* h_cutEff_eta_Xi_max = (TH1F*)h_cutEff_eta_Xi_num->Clone("h_cutEff_eta_Xi_max");
  h_cutEff_eta_Xi_max->Divide(h_cutEff_eta_Xi_num, h_cutEff_eta_Xi_denom, 1., 1.);
  TH1F* h_cutEff_phi_Xi_max = (TH1F*)h_cutEff_phi_Xi_num->Clone("h_cutEff_phi_Xi_max");
  h_cutEff_phi_Xi_max->Divide(h_cutEff_phi_Xi_num, h_cutEff_phi_Xi_denom, 1., 1.);
  TH1F* h_cutEff_rap_Xi_max = (TH1F*)h_cutEff_rap_Xi_num->Clone("h_cutEff_rap_Xi_max");
  h_cutEff_rap_Xi_max->Divide(h_cutEff_rap_Xi_num, h_cutEff_rap_Xi_denom, 1., 1.);

  outputFile->cd();
  h_cutEff_pT_KS0->Write(); 
  h_cutEff_phi_KS0->Write(); 
  h_cutEff_eta_KS0->Write(); 
  h_cutEff_rap_KS0->Write(); 
  h_cutEff_pT_Xi->Write(); 
  h_cutEff_phi_Xi->Write(); 
  h_cutEff_eta_Xi->Write(); 
  h_cutEff_rap_Xi->Write(); 
  h_cutEff_pT_KS0_max->Write(); 
  h_cutEff_phi_KS0_max->Write(); 
  h_cutEff_eta_KS0_max->Write(); 
  h_cutEff_rap_KS0_max->Write(); 
  h_cutEff_pT_Xi_max->Write(); 
  h_cutEff_phi_Xi_max->Write(); 
  h_cutEff_eta_Xi_max->Write(); 
  h_cutEff_rap_Xi_max->Write(); 
  outputFile->Close();
  std::cout << "Done" << std::endl;
} 
