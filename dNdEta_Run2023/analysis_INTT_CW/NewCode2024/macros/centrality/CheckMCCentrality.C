int CheckMCCentrality()
{
    std::string file_directory = "/sphenix/user/ChengWei/sPH_dNdeta/Run24AuAuMC/Sim_HIJING_MDC2_ana472_20250307/per5k";
    std::string file_name = "ntuple_per5k_0*.root";

    // std::string file_directory = "/sphenix/user/ChengWei/sPH_dNdeta/Run24AuAuMC/Sim_HIJING_ananew_20250131/per10k";
    // std::string file_name = "ntuple_per10k_0*.root";

    TChain * tree_in = new TChain("EventTree");
    tree_in -> Add(Form("%s/%s", file_directory.c_str(), file_name.c_str()));

    tree_in -> SetBranchStatus("*", 0);
    tree_in -> SetBranchStatus("MBD_centrality", 1);
    tree_in -> SetBranchStatus("is_min_bias", 1);
    tree_in -> SetBranchStatus("MBD_z_vtx", 1);

    float MBD_centrality;
    bool is_min_bias;
    float MBD_z_vtx;

    tree_in -> SetBranchAddress("MBD_centrality", &MBD_centrality);
    tree_in -> SetBranchAddress("is_min_bias", &is_min_bias);
    tree_in -> SetBranchAddress("MBD_z_vtx", &MBD_z_vtx);

    int n_entries = tree_in -> GetEntries();

    TH1D * h_centrality = new TH1D("h_centrality", "h_centrality", 101, -0.5, 100.5);
    
    std::vector<TF1 *> v_fit; v_fit.clear();
    std::vector<TH1D *> v_h_centrality; v_h_centrality.clear();

    TCanvas * c2 = new TCanvas("c2", "c2", 600, 600);
    c2 -> Print("CheckMCCentrality.pdf[");

    TCanvas * c1 = new TCanvas("c1", "c1", 1400, 600);
    c1 -> Divide(2, 1);

    TGraph * g_eID_fiterror = new TGraph();
    TGraph * g_eID_RChi2 = new TGraph();


    for (int i = 0; i < n_entries; i++)
    {
        tree_in -> GetEntry(i);

        if (is_min_bias == 0) continue;
        if (MBD_z_vtx != MBD_z_vtx) continue;
        if (MBD_centrality != MBD_centrality) continue;

        h_centrality -> Fill(MBD_centrality);

        if (int(h_centrality -> GetEntries()) % 5000 == 0){
            v_fit.push_back(new TF1(Form("f_%.0f", h_centrality -> GetEntries()), "pol0", -10,110));

            v_h_centrality.push_back((TH1D*)h_centrality -> Clone(Form("h_centrality_%.0f", h_centrality -> GetEntries())));
            v_h_centrality.back() -> Scale(1.0 / v_h_centrality.back() -> GetEntries());
            v_h_centrality.back() -> Fit(v_fit.back(), "NQ","",1,92);

            g_eID_fiterror -> SetPoint(g_eID_fiterror -> GetN(), h_centrality -> GetEntries(), v_fit.back() -> GetParError(0));
            g_eID_RChi2    -> SetPoint(g_eID_RChi2 -> GetN(), h_centrality -> GetEntries(), v_fit.back() -> GetChisquare() / double(v_fit.back() -> GetNDF()));

            c2 -> cd();
            v_h_centrality.back() -> Draw("ep");
            v_fit.back() -> Draw("same");

            c2 -> Print("CheckMCCentrality.pdf");
        }
    }

    c2 -> Print("CheckMCCentrality.pdf]");

    g_eID_fiterror -> SetMarkerStyle(20);
    g_eID_fiterror -> SetMarkerSize(0.5);
    g_eID_RChi2 -> SetMarkerStyle(20);
    g_eID_RChi2 -> SetMarkerSize(0.5);

    c1 -> cd(1);
    g_eID_fiterror -> Draw("AP");
    c1 -> cd(2);
    g_eID_RChi2 -> Draw("AP");

    c1 -> Print("CheckMCCentrality_correlation.pdf");

    return 888;
}