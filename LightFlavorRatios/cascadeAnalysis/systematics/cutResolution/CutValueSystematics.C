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

void CutValueSystematics()
{
  string sim_inFile_Xi = "/sphenix/user/rosstom/TrackingAnalysis/Physics_Val_TF/sim/pTComp/KFParticle_4Mcascade_nopTCut.root";
  TFile* sim_file_Xi = new TFile(sim_inFile_Xi.c_str());
  TTree* sim_tree_Xi = (TTree*)sim_file_Xi->Get("DecayTree");

  const int nBins_pT = 4;
  double pTbins[nBins_pT + 1] = {0.75, 1.07, 1.25, 1.49, 2.0};

  const int nBins_phi = 6;
  double phibins[nBins_phi + 1] = {-M_PI,-2.09439,-1.04719,0.0,1.04721,2.09441,M_PI};

  const int nBins_eta = 6;
  double etabins[nBins_eta + 1] = {-1.1,-0.733333,-0.366666,0.0,0.366668,0.733335,1.1};

  const int nBins_rap = 6;
  double rapbins[nBins_rap + 1] = {-1.0,-0.66,-0.33,0.0,0.33,0.66,1.0};

  float KS0_track_1_sigma = 0.0023;
  float KS0_track_1_sigma_xy = 0.0017;
  float KS0_track_2_sigma = 0.0025;
  float KS0_track_2_sigma_xy = 0.0017;
  float Xi_track_1_sigma = 0.0037;
  float Xi_track_1_sigma_xy = 0.0029;
  float Xi_track_2_sigma = 0.0035;
  float Xi_track_2_sigma_xy = 0.0029;
  float Xi_track_3_sigma = 0.0035;
  float Xi_track_3_sigma_xy = 0.0027;

  float KS0_PV_sigma = 0.0049;
  float KS0_PV_sigma_xy = 0.0035;
  float KS0_SV_sigma = 0.0027;
  float KS0_SV_sigma_xy = 0.0021;
  float Xi_PV_sigma = 0.0055;
  float Xi_PV_sigma_xy = 0.0039;
  float Xi_SV_sigma = 0.0037;
  float Xi_SV_sigma_xy = 0.0033;
  float L0_SV_sigma = 0.0051;
  float L0_SV_sigma_xy = 0.0039;

  std::string truthDataCut = "abs(Lambda0_track_1_true_ID) == 211 && Sum$(abs(Lambda0_track_1_true_track_history_PDG_ID) == 3122) > 0 && Sum$(abs(Lambda0_track_1_true_track_history_PDG_ID) == 3312) > 0 && abs(Lambda0_track_2_true_ID) == 2212 && Sum$(abs(Lambda0_track_2_true_track_history_PDG_ID) == 3122) > 0 && Sum$(abs(Lambda0_track_2_true_track_history_PDG_ID) == 3312) > 0 && abs(track_3_true_ID) == 211 && Sum$(abs(track_3_true_track_history_PDG_ID) == 3312) > 0 && Sum$(abs(track_3_true_track_history_PDG_ID) == 3122) == 0 && Lambda0_track_1_MVTX_nHits > 0 && Lambda0_track_2_MVTX_nHits > 0 && track_3_MVTX_nHits > 0 && abs(primary_vertex_z) < 10 && Lambda0_track_1_charge == track_3_charge && Lambda0_track_1_INTT_nHits > 0 && Lambda0_track_2_INTT_nHits > 0 && track_3_INTT_nHits > 0 && Lambda0_track_1_TPC_nHits > 19 && Lambda0_track_2_TPC_nHits > 19 && track_3_TPC_nHits > 19 && abs(Lambda0_mass - 1.1157) <= 0.01 && Ximinus_decayLength_xy >= 0.15 && Lambda0_decayLength_xy >= 0.01 && (Lambda0_track_1_chi2/Lambda0_track_1_nDoF) <= 400 && (Lambda0_track_2_chi2/Lambda0_track_2_nDoF) <= 400 && (track_3_chi2/track_3_nDoF) <= 400 && track_1_track_2_DCA <= 0.5 && track_1_track_3_DCA <= 0.5 && track_2_track_3_DCA <= 0.5 && Ximinus_chi2/Ximinus_nDoF <= 50 && Lambda0_chi2/Lambda0_nDoF <= 50 && abs(Ximinus_rapidity) <= 1.0 && Lambda0_track_1_pT >= 0.2 && Lambda0_track_2_pT >= 0.2 && track_3_pT >= 0.2";
  std::string truthDataCut_min = "abs(Lambda0_track_1_true_ID) == 211 && Sum$(abs(Lambda0_track_1_true_track_history_PDG_ID) == 3122) > 0 && Sum$(abs(Lambda0_track_1_true_track_history_PDG_ID) == 3312) > 0 && abs(Lambda0_track_2_true_ID) == 2212 && Sum$(abs(Lambda0_track_2_true_track_history_PDG_ID) == 3122) > 0 && Sum$(abs(Lambda0_track_2_true_track_history_PDG_ID) == 3312) > 0 && abs(track_3_true_ID) == 211 && Sum$(abs(track_3_true_track_history_PDG_ID) == 3312) > 0 && Sum$(abs(track_3_true_track_history_PDG_ID) == 3122) == 0 && Lambda0_track_1_MVTX_nHits > 0 && Lambda0_track_2_MVTX_nHits > 0 && track_3_MVTX_nHits > 0 && abs(primary_vertex_z) < 10 && Lambda0_track_1_charge == track_3_charge && Lambda0_track_1_INTT_nHits > 0 && Lambda0_track_2_INTT_nHits > 0 && track_3_INTT_nHits > 0 && Lambda0_track_1_TPC_nHits > 19 && Lambda0_track_2_TPC_nHits > 19 && track_3_TPC_nHits > 19 && abs(Lambda0_mass - 1.1157) <= 0.01 && (Ximinus_decayLength_xy - " + std::to_string(Xi_PV_sigma_xy) + " - " +std::to_string( Xi_SV_sigma_xy) + ") >= 0.15 && (Lambda0_decayLength_xy - " + std::to_string(Xi_SV_sigma_xy) + " - " + std::to_string(L0_SV_sigma_xy) + ") >= 0.01 && (Lambda0_track_1_chi2/Lambda0_track_1_nDoF) <= 400 && (Lambda0_track_2_chi2/Lambda0_track_2_nDoF) <= 400 && (track_3_chi2/track_3_nDoF) <= 400 && (track_1_track_2_DCA + " + std::to_string(Xi_track_1_sigma) + " + " + std::to_string(Xi_track_2_sigma) + ") <= 0.5 && (track_1_track_3_DCA + " + std::to_string(Xi_track_1_sigma) + " + " + std::to_string(Xi_track_3_sigma) + ") <= 0.5 && (track_2_track_3_DCA + " + std::to_string(Xi_track_2_sigma) + " + " + std::to_string(Xi_track_3_sigma) + ") <= 0.5 && Ximinus_chi2/Ximinus_nDoF <= 50 && Lambda0_chi2/Lambda0_nDoF <= 50 && abs(Ximinus_rapidity) <= 1.0 && Lambda0_track_1_pT >= 0.2 && Lambda0_track_2_pT >= 0.2 && track_3_pT >= 0.2";
  std::string truthDataCut_max = "abs(Lambda0_track_1_true_ID) == 211 && Sum$(abs(Lambda0_track_1_true_track_history_PDG_ID) == 3122) > 0 && Sum$(abs(Lambda0_track_1_true_track_history_PDG_ID) == 3312) > 0 && abs(Lambda0_track_2_true_ID) == 2212 && Sum$(abs(Lambda0_track_2_true_track_history_PDG_ID) == 3122) > 0 && Sum$(abs(Lambda0_track_2_true_track_history_PDG_ID) == 3312) > 0 && abs(track_3_true_ID) == 211 && Sum$(abs(track_3_true_track_history_PDG_ID) == 3312) > 0 && Sum$(abs(track_3_true_track_history_PDG_ID) == 3122) == 0 && Lambda0_track_1_MVTX_nHits > 0 && Lambda0_track_2_MVTX_nHits > 0 && track_3_MVTX_nHits > 0 && abs(primary_vertex_z) < 10 && Lambda0_track_1_charge == track_3_charge && Lambda0_track_1_INTT_nHits > 0 && Lambda0_track_2_INTT_nHits > 0 && track_3_INTT_nHits > 0 && Lambda0_track_1_TPC_nHits > 19 && Lambda0_track_2_TPC_nHits > 19 && track_3_TPC_nHits > 19 && abs(Lambda0_mass - 1.1157) <= 0.01 && (Ximinus_decayLength_xy + " + std::to_string(Xi_PV_sigma_xy) + " + " + std::to_string(Xi_SV_sigma_xy) + ") >= 0.15 && (Lambda0_decayLength_xy + " + std::to_string(Xi_SV_sigma_xy) + " + " + std::to_string(L0_SV_sigma_xy) + ") >= 0.01 && (Lambda0_track_1_chi2/Lambda0_track_1_nDoF) <= 400 && (Lambda0_track_2_chi2/Lambda0_track_2_nDoF) <= 400 && (track_3_chi2/track_3_nDoF) <= 400 && (track_1_track_2_DCA - " + std::to_string(Xi_track_1_sigma) + " - " + std::to_string(Xi_track_2_sigma) + ") <= 0.5 && (track_1_track_3_DCA - " + std::to_string(Xi_track_1_sigma) + " - " + std::to_string(Xi_track_3_sigma) + ") <= 0.5 && (track_2_track_3_DCA - " + std::to_string(Xi_track_2_sigma) + " - " + std::to_string(Xi_track_3_sigma) + ") <= 0.5 && Ximinus_chi2/Ximinus_nDoF <= 50 && Lambda0_chi2/Lambda0_nDoF <= 50 && abs(Ximinus_rapidity) <= 1.0 && Lambda0_track_1_pT >= 0.2 && Lambda0_track_2_pT >= 0.2 && track_3_pT >= 0.2";

  //TH1F* h_pT_truth = new TH1F("h_pT_truth",";#Xi^{-} p_{T} [GeV/c];Cut Efficiency",nBins_pT,pTbins);
  TH1F* h_pT_base_Xi = new TH1F("h_pT_base_Xi",";#Xi^{-} p_{T} [GeV/c];Entries",nBins_pT,pTbins);
  TH1F* h_pT_base_Xi_max = new TH1F("h_pT_base_Xi_max",";#Xi^{-} p_{T} [GeV/c];Entries",nBins_pT,pTbins);
  TH1F* h_pT_base_Xi_min = new TH1F("h_pT_base_Xi_min",";#Xi^{-} p_{T} [GeV/c];Entries",nBins_pT,pTbins);
  
  //TH1F* h_eta_truth = new TH1F("h_eta_truth",";#Xi^{-} #eta;Cut Efficiency",nBins_eta,etabins);
  TH1F* h_eta_base_Xi = new TH1F("h_eta_base_Xi",";#Xi^{-} #eta;Entries",nBins_eta,etabins);
  TH1F* h_eta_base_Xi_max = new TH1F("h_eta_base_Xi_max",";#Xi^{-} #eta;Entries",nBins_eta,etabins);
  TH1F* h_eta_base_Xi_min = new TH1F("h_eta_base_Xi_min",";#Xi^{-} #eta;Entries",nBins_eta,etabins);
  
  //TH1F* h_phi_truth = new TH1F("h_phi_truth",";#Xi^{-} #phi;Cut Efficiency",nBins_phi,phibins);
  TH1F* h_phi_base_Xi = new TH1F("h_phi_base_Xi",";#Xi^{-} #phi;Entries",nBins_phi,phibins);
  TH1F* h_phi_base_Xi_max = new TH1F("h_phi_base_Xi_max",";#Xi^{-} #phi;Entries",nBins_phi,phibins);
  TH1F* h_phi_base_Xi_min = new TH1F("h_phi_base_Xi_min",";#Xi^{-} #phi;Entries",nBins_phi,phibins);
  
  //TH1F* h_y_truth = new TH1F("h_y_truth",";#Xi^{-} y;Cut Efficiency",nBins_rap,rapbins);
  TH1F* h_rap_base_Xi = new TH1F("h_rap_base_Xi",";#Xi^{-} y;Entries",nBins_rap,rapbins);
  TH1F* h_rap_base_Xi_max = new TH1F("h_rap_base_Xi_max",";#Xi^{-} y;Entries",nBins_rap,rapbins);
  TH1F* h_rap_base_Xi_min = new TH1F("h_rap_base_Xi_min",";#Xi^{-} y;Entries",nBins_rap,rapbins);
   
  //h_pT_truth->Sumw2();
  h_pT_base_Xi->Sumw2();
  h_pT_base_Xi_max->Sumw2();
  h_pT_base_Xi_min->Sumw2();
  //h_eta_truth->Sumw2();
  h_eta_base_Xi->Sumw2();
  h_eta_base_Xi_max->Sumw2();
  h_eta_base_Xi_min->Sumw2();
  //h_phi_truth->Sumw2();
  h_phi_base_Xi->Sumw2();
  h_phi_base_Xi_max->Sumw2();
  h_phi_base_Xi_min->Sumw2();
  //h_y_truth->Sumw2();
  h_rap_base_Xi->Sumw2();
  h_rap_base_Xi_max->Sumw2();
  h_rap_base_Xi_min->Sumw2();

  //sim_tree_Xi->Draw("Ximinus_pT>>+h_pT_truth",truthCut.c_str());
  sim_tree_Xi->Draw("Ximinus_pT>>+h_pT_base_Xi",truthDataCut.c_str());
  sim_tree_Xi->Draw("Ximinus_pT>>+h_pT_base_Xi_max",truthDataCut_max.c_str());
  sim_tree_Xi->Draw("Ximinus_pT>>+h_pT_base_Xi_min",truthDataCut_min.c_str());
  //sim_tree_Xi->Draw("Ximinus_pseudorapidity>>+h_eta_truth",truthCut.c_str());
  sim_tree_Xi->Draw("Ximinus_pseudorapidity>>+h_eta_base_Xi",truthDataCut.c_str());
  sim_tree_Xi->Draw("Ximinus_pseudorapidity>>+h_eta_base_Xi_max",truthDataCut_max.c_str());
  sim_tree_Xi->Draw("Ximinus_pseudorapidity>>+h_eta_base_Xi_min",truthDataCut_min.c_str());
  //sim_tree_Xi->Draw("Ximinus_phi>>+h_phi_truth",truthCut.c_str());
  sim_tree_Xi->Draw("Ximinus_phi>>+h_phi_base_Xi",truthDataCut.c_str());
  sim_tree_Xi->Draw("Ximinus_phi>>+h_phi_base_Xi_max",truthDataCut_max.c_str());
  sim_tree_Xi->Draw("Ximinus_phi>>+h_phi_base_Xi_min",truthDataCut_min.c_str());
  //sim_tree_Xi->Draw("Ximinus_rapidity>>+h_y_truth",truthCut.c_str());
  sim_tree_Xi->Draw("Ximinus_rapidity>>+h_rap_base_Xi",truthDataCut.c_str());
  sim_tree_Xi->Draw("Ximinus_rapidity>>+h_rap_base_Xi_max",truthDataCut_max.c_str());
  sim_tree_Xi->Draw("Ximinus_rapidity>>+h_rap_base_Xi_min",truthDataCut_min.c_str());

  string sim_inFile_KS0 = "/sphenix/user/cdean/software/analysis/LightFlavorRatios/geometric_acceptance/simulation/outputKFParticle_Kshort_reco.root";
  TFile* sim_file_KS0 = new TFile(sim_inFile_KS0.c_str());
  TTree* sim_tree_KS0 = (TTree*)sim_file_KS0->Get("DecayTree");

  std::string truthDataCut_KS0 = "abs(track_1_true_ID) == 211 && Sum$(abs(track_1_true_track_history_PDG_ID) == 310) > 0 && abs(track_2_true_ID) == 211 && Sum$(abs(track_2_true_track_history_PDG_ID) == 310) > 0 && abs(K_S0_rapidity) <= 1.0 && track_1_MVTX_nHits > 0 && track_2_MVTX_nHits > 0 && abs(primary_vertex_z) < 10 && track_1_INTT_nHits > 0 && track_2_INTT_nHits > 0 && track_1_TPC_nHits > 19 && track_2_TPC_nHits > 19 && (track_1_chi2/track_1_nDoF) <= 300 && (track_2_chi2/track_2_nDoF) <= 300 && abs(track_1_IP_xy) >= 0.05 && abs(track_2_IP_xy) >= 0.05 && track_1_track_2_DCA <= 0.5 && track_1_track_2_DCA_xy <= 1.0 && K_S0_DIRA >= 0.99 && (K_S0_chi2/K_S0_nDoF) <= 20 && abs(K_S0_mass - 0.5) <= 0.1 && track_1_pT >= 0.2 && track_2_pT >= 0.2";
  std::string truthDataCut_KS0_min = "abs(track_1_true_ID) == 211 && Sum$(abs(track_1_true_track_history_PDG_ID) == 310) > 0 && abs(track_2_true_ID) == 211 && Sum$(abs(track_2_true_track_history_PDG_ID) == 310) > 0 && abs(K_S0_rapidity) <= 1.0 && track_1_MVTX_nHits > 0 && track_2_MVTX_nHits > 0 && abs(primary_vertex_z) < 10 && track_1_INTT_nHits > 0 && track_2_INTT_nHits > 0 && track_1_TPC_nHits > 19 && track_2_TPC_nHits > 19 && (track_1_chi2/track_1_nDoF) <= 300 && (track_2_chi2/track_2_nDoF) <= 300 && (abs(track_1_IP_xy) - " + std::to_string(KS0_PV_sigma_xy) + " - " + std::to_string(KS0_track_1_sigma_xy) + ") >= 0.05 && (abs(track_2_IP_xy) - " + std::to_string(KS0_PV_sigma_xy) + " - " + std::to_string(KS0_track_2_sigma_xy) + ") >= 0.05 && (track_1_track_2_DCA + " + std::to_string(KS0_track_1_sigma) + " + " + std::to_string(KS0_track_2_sigma) + ") <= 0.5 && (track_1_track_2_DCA_xy + " + std::to_string(KS0_track_1_sigma_xy) + " + " + std::to_string(KS0_track_2_sigma_xy) + ") <= 1.0 && cos(acos(K_S0_DIRA) + atan(" + std::to_string(KS0_PV_sigma) + "/K_S0_decayLength) + atan(" + std::to_string(KS0_SV_sigma) + "/K_S0_decayLength)) >= 0.99 && (K_S0_chi2/K_S0_nDoF) <= 20 && abs(K_S0_mass - 0.5) <= 0.1 && track_1_pT >= 0.2 && track_2_pT >= 0.2";
  std::string truthDataCut_KS0_max = "abs(track_1_true_ID) == 211 && Sum$(abs(track_1_true_track_history_PDG_ID) == 310) > 0 && abs(track_2_true_ID) == 211 && Sum$(abs(track_2_true_track_history_PDG_ID) == 310) > 0 && abs(K_S0_rapidity) <= 1.0 && track_1_MVTX_nHits > 0 && track_2_MVTX_nHits > 0 && abs(primary_vertex_z) < 10 && track_1_INTT_nHits > 0 && track_2_INTT_nHits > 0 && track_1_TPC_nHits > 19 && track_2_TPC_nHits > 19 && (track_1_chi2/track_1_nDoF) <= 300 && (track_2_chi2/track_2_nDoF) <= 300 && (abs(track_1_IP_xy) + " + std::to_string(KS0_PV_sigma_xy) + " + " + std::to_string(KS0_track_1_sigma_xy) + ") >= 0.05 && (abs(track_2_IP_xy) + " + std::to_string(KS0_PV_sigma_xy) + " + " + std::to_string(KS0_track_2_sigma_xy) + ") >= 0.05 && (track_1_track_2_DCA - " + std::to_string(KS0_track_1_sigma) + " - " + std::to_string(KS0_track_2_sigma) + ") <= 0.5 && (track_1_track_2_DCA_xy - " + std::to_string(KS0_track_1_sigma_xy) + " - " + std::to_string(KS0_track_2_sigma_xy) + ") <= 1.0 && cos(acos(K_S0_DIRA) - atan(" + std::to_string(KS0_PV_sigma) + "/K_S0_decayLength) - atan(" + std::to_string(KS0_SV_sigma) + "/K_S0_decayLength)) >= 0.99 && (K_S0_chi2/K_S0_nDoF) <= 20 && abs(K_S0_mass - 0.5) <= 0.1 && track_1_pT >= 0.2 && track_2_pT >= 0.2";

  //TH1F* h_pT_truth_KS0 = new TH1F("h_pT_truth_KS0",";K_{S}^{0} p_{T} [GeV/c];Cut Efficiency",nBins_pT,pTbins);
  TH1F* h_pT_base_KS0 = new TH1F("h_pT_base_KS0",";K_{S}^{0} p_{T} [GeV/c];Cut Efficiency",nBins_pT,pTbins);
  TH1F* h_pT_base_KS0_max = new TH1F("h_pT_base_KS0_max",";K_{S}^{0} p_{T} [GeV/c];Entries",nBins_pT,pTbins);
  TH1F* h_pT_base_KS0_min = new TH1F("h_pT_base_KS0_min",";K_{S}^{0} p_{T} [GeV/c];Entries",nBins_pT,pTbins);
  
  //TH1F* h_eta_truth_KS0 = new TH1F("h_eta_truth_KS0",";K_{S}^{0} #eta;Cut Efficiency",nBins_eta,etabins);
  TH1F* h_eta_base_KS0 = new TH1F("h_eta_base_KS0",";K_{S}^{0} #eta;Entries",nBins_eta,etabins);
  TH1F* h_eta_base_KS0_max = new TH1F("h_eta_base_KS0_max",";K_{S}^{0} #eta;Entries",nBins_eta,etabins);
  TH1F* h_eta_base_KS0_min = new TH1F("h_eta_base_KS0_min",";K_{S}^{0} #eta;Entries",nBins_eta,etabins);
  
  //TH1F* h_phi_truth_KS0 = new TH1F("h_phi_truth_KS0",";K_{S}^{0} #phi;Cut Efficiency",nBins_phi,phibins);
  TH1F* h_phi_base_KS0 = new TH1F("h_phi_base_KS0",";K_{S}^{0} #phi;Entries",nBins_phi,phibins);
  TH1F* h_phi_base_KS0_max = new TH1F("h_phi_base_KS0_max",";K_{S}^{0} #phi;Entries",nBins_phi,phibins);
  TH1F* h_phi_base_KS0_min = new TH1F("h_phi_base_KS0_min",";K_{S}^{0} #phi;Entries",nBins_phi,phibins);
  
  //TH1F* h_y_truth_KS0 = new TH1F("h_y_truth_KS0",";K_{S}^{0} y;Cut Efficiency",nBins_rap,rapbins);
  TH1F* h_rap_base_KS0 = new TH1F("h_rap_base_KS0",";K_{S}^{0} y;Entries",nBins_rap,rapbins);
  TH1F* h_rap_base_KS0_max = new TH1F("h_rap_base_KS0_max",";K_{S}^{0} y;Entries",nBins_rap,rapbins);
  TH1F* h_rap_base_KS0_min = new TH1F("h_rap_base_KS0_min",";K_{S}^{0} y;Entries",nBins_rap,rapbins);
   
  //h_pT_truth_KS0->Sumw2();
  h_pT_base_KS0->Sumw2();
  h_pT_base_KS0_max->Sumw2();
  h_pT_base_KS0_min->Sumw2();
  //h_eta_truth_KS0->Sumw2();
  h_eta_base_KS0->Sumw2();
  h_eta_base_KS0_max->Sumw2();
  h_eta_base_KS0_min->Sumw2();
  //h_phi_truth_KS0->Sumw2();
  h_phi_base_KS0->Sumw2();
  h_phi_base_KS0_max->Sumw2();
  h_phi_base_KS0_min->Sumw2();
  //h_y_truth_KS0->Sumw2();
  h_rap_base_KS0->Sumw2();
  h_rap_base_KS0_max->Sumw2();
  h_rap_base_KS0_min->Sumw2();

  //sim_tree_KS0->Draw("K_S0_pT>>+h_pT_truth_KS0",truthCut_KS0.c_str());
  sim_tree_KS0->Draw("K_S0_pT>>+h_pT_base_KS0",truthDataCut_KS0.c_str());
  sim_tree_KS0->Draw("K_S0_pT>>+h_pT_base_KS0_max",truthDataCut_KS0_max.c_str());
  sim_tree_KS0->Draw("K_S0_pT>>+h_pT_base_KS0_min",truthDataCut_KS0_min.c_str());
  //sim_tree_KS0->Draw("K_S0_pseudorapidity>>+h_eta_truth_KS0",truthCut_KS0.c_str());
  sim_tree_KS0->Draw("K_S0_pseudorapidity>>+h_eta_base_KS0",truthDataCut_KS0.c_str());
  sim_tree_KS0->Draw("K_S0_pseudorapidity>>+h_eta_base_KS0_max",truthDataCut_KS0_max.c_str());
  sim_tree_KS0->Draw("K_S0_pseudorapidity>>+h_eta_base_KS0_min",truthDataCut_KS0_min.c_str());
  //sim_tree_KS0->Draw("K_S0_phi>>+h_phi_truth_KS0",truthCut_KS0.c_str());
  sim_tree_KS0->Draw("K_S0_phi>>+h_phi_base_KS0",truthDataCut_KS0.c_str());
  sim_tree_KS0->Draw("K_S0_phi>>+h_phi_base_KS0_max",truthDataCut_KS0_max.c_str());
  sim_tree_KS0->Draw("K_S0_phi>>+h_phi_base_KS0_min",truthDataCut_KS0_min.c_str());
  //sim_tree_KS0->Draw("K_S0_rapidity>>+h_y_truth_KS0",truthCut_KS0.c_str());
  sim_tree_KS0->Draw("K_S0_rapidity>>+h_rap_base_KS0",truthDataCut_KS0.c_str());
  sim_tree_KS0->Draw("K_S0_rapidity>>+h_rap_base_KS0_max",truthDataCut_KS0_max.c_str());
  sim_tree_KS0->Draw("K_S0_rapidity>>+h_rap_base_KS0_min",truthDataCut_KS0_min.c_str());

  TFile* fout = new TFile("CutSystematicsHistograms.root","RECREATE");
  h_pT_base_Xi->Write();
  h_pT_base_KS0->Write();
  h_phi_base_Xi->Write();
  h_phi_base_KS0->Write();
  h_eta_base_Xi->Write();
  h_eta_base_KS0->Write();
  h_rap_base_Xi->Write();
  h_rap_base_KS0->Write();
  h_pT_base_Xi_max->Write();
  h_pT_base_KS0_max->Write();
  h_phi_base_Xi_max->Write();
  h_phi_base_KS0_max->Write();
  h_eta_base_Xi_max->Write();
  h_eta_base_KS0_max->Write();
  h_rap_base_Xi_max->Write();
  h_rap_base_KS0_max->Write();
  h_pT_base_Xi_min->Write();
  h_pT_base_KS0_min->Write();
  h_phi_base_Xi_min->Write();
  h_phi_base_KS0_min->Write();
  h_eta_base_Xi_min->Write();
  h_eta_base_KS0_min->Write();
  h_rap_base_Xi_min->Write();
  h_rap_base_KS0_min->Write(); 
} 
