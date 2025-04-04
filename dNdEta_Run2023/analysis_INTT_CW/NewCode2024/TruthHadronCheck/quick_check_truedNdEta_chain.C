#include "/sphenix/user/ChengWei/INTT/INTT_dNdeta_repo/NewCode2024/Constants.cpp"

int quick_check_truedNdEta_chain()
{
    string input_directory = "/sphenix/user/ChengWei/sPH_dNdeta/Run24AuAuMC/Sim_HIJING_ananew_20250131/Run6_EvtZFitWidthChange/EvtVtxZ/completed";
    string input_filename = "MC_EvtVtxZProtoTracklet_FieldOff_VtxZReco_";
    string output_directory = "/sphenix/user/ChengWei/sPH_dNdeta/Run24AuAuMC/Sim_HIJING_ananew_20250131/Run6_EvtZFitWidthChange/EvtVtxZ/completed/TruthHadron";
    std::string output_filename = Form("INTTz_New_QuickCheck_Mbin70.root");

    // TFile * file_in = TFile::Open(Form("%s/%s", input_directory.c_str(), input_filename.c_str()));
    // TTree * tree_in = (TTree*)file_in->Get("EventTree");     

    TChain * tree_in = new TChain("EventTree");
    // tree_in -> Add(Form("%s/%s000{00..24}.root", input_directory.c_str(), input_filename.c_str()));

    for (int i = 0; i < 25; i++)
    {
        std::string job_index = std::to_string( i );
        int job_index_len = 5;
        job_index.insert(0, job_index_len - job_index.size(), '0');

        std::string input_filename_full = Form("%s/%s%s.root", input_directory.c_str(), input_filename.c_str(), job_index.c_str());
        std::cout<<"input_filename_full: "<<input_filename_full<<std::endl;
        tree_in -> Add(input_filename_full.c_str());
    }

    // std::cout<<"aaaa"<<std::endl;
    // tree_in->GetEntry(0);
    // std::cout<<"aaaa"<<std::endl;

    std::cout<<"tree_in -> GetEntries(): "<<tree_in -> GetEntries()<<std::endl;

    tree_in -> SetBranchStatus("*", 0);

    tree_in -> SetBranchStatus("MBD_z_vtx", 1);
    tree_in -> SetBranchStatus("is_min_bias", 1);
    tree_in -> SetBranchStatus("MBD_centrality", 1);
    tree_in -> SetBranchStatus("MBD_south_charge_sum", 1);
    tree_in -> SetBranchStatus("MBD_north_charge_sum", 1);
    tree_in -> SetBranchStatus("MBD_charge_sum", 1);
    tree_in -> SetBranchStatus("MBD_charge_asymm", 1);
    // tree_in -> SetBranchStatus("InttBcoFullDiff_next", 1);
    // tree_in -> SetBranchStatus("MBDNSg2", 1);
    
    // tree_in -> SetBranchStatus("ClusLayer", 1);
    // tree_in -> SetBranchStatus("ClusEta_INTTz", 1);

    tree_in -> SetBranchStatus("INTTvtxZ", 1);
    tree_in -> SetBranchStatus("INTTvtxZError", 1);
    tree_in -> SetBranchStatus("NgroupTrapezoidal", 1);
    tree_in -> SetBranchStatus("NgroupCoarse", 1);
    tree_in -> SetBranchStatus("TrapezoidalFitWidth", 1);
    tree_in -> SetBranchStatus("TrapezoidalFWHM", 1);

    tree_in -> SetBranchStatus("NTruthVtx", 1);
    tree_in -> SetBranchStatus("PrimaryG4P_Pt", 1);
    tree_in -> SetBranchStatus("PrimaryG4P_Eta", 1);
    tree_in -> SetBranchStatus("PrimaryG4P_Phi", 1);
    tree_in -> SetBranchStatus("PrimaryG4P_E", 1);
    tree_in -> SetBranchStatus("PrimaryG4P_PID", 1);
    tree_in -> SetBranchStatus("PrimaryG4P_isChargeHadron", 1);

    tree_in -> SetBranchStatus("TruthPV_trig_z", 1);

    // std::cout<<"bbbb"<<std::endl;
    // tree_in->GetEntry(0);
    // tree_in->Show(0);
    // std::cout<<"bbbb"<<std::endl;


    float MBD_z_vtx;
    bool is_min_bias;
    float MBD_centrality;
    float MBD_south_charge_sum;
    float MBD_north_charge_sum;
    float MBD_charge_sum;
    float MBD_charge_asymm;
    // int InttBcoFullDiff_next;

    // note : trigger tag
    // int MBDNSg2;

    // std::vector<int> *ClusLayer = 0;
    // std::vector<double> *ClusEta_INTTz = 0;

    // note : INTT vertex Z
    double INTTvtxZ;
    double INTTvtxZError;
    double NgroupTrapezoidal;
    double NgroupCoarse;
    double TrapezoidalFitWidth;
    double TrapezoidalFWHM;

    int NTruthVtx;
    float TruthPV_trig_z;
    std::vector<float> *PrimaryG4P_Pt = 0;
    std::vector<float> *PrimaryG4P_Eta = 0;
    std::vector<float> *PrimaryG4P_Phi = 0;
    std::vector<float> *PrimaryG4P_E = 0;
    std::vector<int> *PrimaryG4P_PID = 0;
    std::vector<bool> *PrimaryG4P_isChargeHadron = 0;

    tree_in -> SetBranchAddress("MBD_z_vtx", &MBD_z_vtx);
    tree_in -> SetBranchAddress("is_min_bias", &is_min_bias);
    tree_in -> SetBranchAddress("MBD_centrality", &MBD_centrality);
    tree_in -> SetBranchAddress("MBD_south_charge_sum", &MBD_south_charge_sum);
    tree_in -> SetBranchAddress("MBD_north_charge_sum", &MBD_north_charge_sum);
    tree_in -> SetBranchAddress("MBD_charge_sum", &MBD_charge_sum);
    tree_in -> SetBranchAddress("MBD_charge_asymm", &MBD_charge_asymm);
    // tree_in -> SetBranchAddress("InttBcoFullDiff_next", &InttBcoFullDiff_next);

    // tree_in -> SetBranchAddress("MBDNSg2", &MBDNSg2);

    // tree_in -> SetBranchAddress("ClusLayer", &ClusLayer);
    // tree_in -> SetBranchAddress("ClusEta_INTTz", &ClusEta_INTTz);

    tree_in -> SetBranchAddress("INTTvtxZ", &INTTvtxZ);
    tree_in -> SetBranchAddress("INTTvtxZError", &INTTvtxZError);
    tree_in -> SetBranchAddress("NgroupTrapezoidal", &NgroupTrapezoidal);
    tree_in -> SetBranchAddress("NgroupCoarse", &NgroupCoarse);
    tree_in -> SetBranchAddress("TrapezoidalFitWidth", &TrapezoidalFitWidth);
    tree_in -> SetBranchAddress("TrapezoidalFWHM", &TrapezoidalFWHM);

    tree_in -> SetBranchAddress("NTruthVtx", &NTruthVtx);
    tree_in -> SetBranchAddress("PrimaryG4P_Pt", &PrimaryG4P_Pt);
    tree_in -> SetBranchAddress("PrimaryG4P_Eta", &PrimaryG4P_Eta);
    tree_in -> SetBranchAddress("PrimaryG4P_Phi", &PrimaryG4P_Phi);
    tree_in -> SetBranchAddress("PrimaryG4P_E", &PrimaryG4P_E);
    tree_in -> SetBranchAddress("PrimaryG4P_PID", &PrimaryG4P_PID);
    tree_in -> SetBranchAddress("PrimaryG4P_isChargeHadron", &PrimaryG4P_isChargeHadron);
    tree_in -> SetBranchAddress("TruthPV_trig_z", &TruthPV_trig_z);

    // std::cout<<"cccc"<<std::endl;
    // tree_in->GetEntry(0);
    // std::cout<<"cccc"<<std::endl;


    // Division : ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    // std::string job_index = std::to_string( process_id );
    // int job_index_len = 5;
    // job_index.insert(0, job_index_len - job_index.size(), '0');

    // std::string output_filename = Form("INTTz_New_QuickCheck_%s.root", job_index.c_str());

    std::cout<<111<<std::endl;

    // system(Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi", output_directory.c_str(), output_filename.c_str(), output_directory.c_str(), output_filename.c_str()));

    TFile * file_out = new TFile(Form("%s/%s", output_directory.c_str(), output_filename.c_str()), "recreate");
    
    TH1D * h1D_NHadronCount = new TH1D("h1D_NHadronCount","h1D_NHadronCount;#eta;Entries",27,-2.7,2.7);
    TH1D * h1D_TrueEvtCount = new TH1D("h1D_TrueEvtCount","h1D_TrueEvtCount;#True vtx Z;Entries",20,-10,10);
    TH1D * h1D_TruedNdEta = new TH1D("h1D_TruedNdEta","h1D_TruedNdEta;#eta;Entries",27,-2.7,2.7);

    TH1D * h1D_NHadronCount_NTrueVtxCut = new TH1D("h1D_NHadronCount_NTrueVtxCut","h1D_NHadronCount_NTrueVtxCut;#eta;Entries",27,-2.7,2.7);
    TH1D * h1D_TrueEvtCount_NTrueVtxCut = new TH1D("h1D_TrueEvtCount_NTrueVtxCut","h1D_TrueEvtCount_NTrueVtxCut;#True vtx Z;Entries",20,-10,10);
    TH1D * h1D_TruedNdEta_NTrueVtxCut = new TH1D("h1D_TruedNdEta_NTrueVtxCut","h1D_TruedNdEta_NTrueVtxCut;#eta;Entries",27,-2.7,2.7);

    std::cout<<222<<std::endl;

    for (int i = 0; i < tree_in -> GetEntries(); i++)
    {
        // std::cout<<333<<std::endl;
        tree_in -> GetEntry(i);
        // std::cout<<444<<std::endl;

        if (i % 1000 == 0)
        {
            std::cout<<"i: "<<i<<std::endl;
        }    

        int nHadron_count = 0;

        // note : for MC
        // if (NTruthVtx != 1) {continue;}

        // if (is_min_bias != 1) {continue;}
        // if (INTTvtxZ != INTTvtxZ) {continue;}
        // if (INTTvtxZ < -10 || INTTvtxZ >= 10) {continue;}
        // if (MBD_centrality < 0) {continue;}
        // if (MBD_centrality > 3) {continue;} // note : [0-3)% centrality interval


         // note : both data and MC
        if (is_min_bias != 1) {continue;}
        if (MBD_z_vtx != MBD_z_vtx) {continue;}
        if (MBD_centrality != MBD_centrality) {continue;}
        if (MBD_centrality < 0 || MBD_centrality > 100) {continue;}
        if (INTTvtxZ != INTTvtxZ) {continue;}
        // =======================================================================================================================================================
        // note : optional cut
        if ((MBD_z_vtx - INTTvtxZ < Constants::cut_vtxZDiff.first || MBD_z_vtx - INTTvtxZ > Constants::cut_vtxZDiff.second) ) {continue;}
        if ((TrapezoidalFitWidth < Constants::cut_TrapezoidalFitWidth.first || TrapezoidalFitWidth > Constants::cut_TrapezoidalFitWidth.second)){continue;}
        if ((TrapezoidalFWHM < Constants::cut_TrapezoidalFWHM.first || TrapezoidalFWHM > Constants::cut_TrapezoidalFWHM.second)){continue;}
        if ((INTTvtxZError < Constants::cut_INTTvtxZError.first || INTTvtxZError > Constants::cut_INTTvtxZError.second)){continue;}
        // =======================================================================================================================================================

        if (INTTvtxZ < -10 || INTTvtxZ >= 10) {continue;}
        if (MBD_centrality < 0) {continue;}
        if (MBD_centrality > 70) {continue;} // note : [0-3)% centrality interval


        // if (MBD_z_vtx < -10 || MBD_z_vtx >= 10) {continue;}

        // note : both data and MC
        // if (MBD_z_vtx != MBD_z_vtx) {continue;}
        // if (MBD_centrality != MBD_centrality) {continue;}
        // if (MBD_centrality < 0 || MBD_centrality > 100) {continue;}
        // if (INTTvtxZ != INTTvtxZ) {continue;}
        // if (MBD_z_vtx < -60 || MBD_z_vtx > 60) {continue;} // todo: the hard cut 60 cm 


        // if ((MBD_z_vtx - INTTvtxZ < Constants::cut_vtxZDiff.first || MBD_z_vtx - INTTvtxZ > Constants::cut_vtxZDiff.second) ) {continue;}
        // if ((TrapezoidalFitWidth < Constants::cut_TrapezoidalFitWidth.first || TrapezoidalFitWidth > Constants::cut_TrapezoidalFitWidth.second)){continue;}
        // if ((TrapezoidalFWHM < Constants::cut_TrapezoidalFWHM.first || TrapezoidalFWHM > Constants::cut_TrapezoidalFWHM.second)){continue;}
        // if ((INTTvtxZError < Constants::cut_INTTvtxZError.first || INTTvtxZError > Constants::cut_INTTvtxZError.second)){continue;}

        // if (TruthPV_trig_z < -10 || TruthPV_trig_z >= 10) {continue;}
        // if (MBD_centrality > 0.7) {continue;}

        // if (MBD_centrality > 3) {continue;} // note : [0-3)% centrality interval

        for (int true_i = 0; true_i < PrimaryG4P_isChargeHadron -> size(); true_i++)
        {
            if (PrimaryG4P_isChargeHadron -> at(true_i) == 1)
            {
                h1D_NHadronCount -> Fill(PrimaryG4P_Eta -> at(true_i));

                if (fabs(PrimaryG4P_Eta->at(true_i)) < 4){
                    nHadron_count++;
                }
            }
        } 

        // std::cout<<"eID: "<<i<<", nHadron_count: "<<nHadron_count<<", TruthPV_trig_z: "<<TruthPV_trig_z<<std::endl;

        h1D_TrueEvtCount -> Fill(INTTvtxZ);


        if (NTruthVtx != 1) {continue;}


        for (int true_i = 0; true_i < PrimaryG4P_isChargeHadron -> size(); true_i++)
        {
            if (PrimaryG4P_isChargeHadron -> at(true_i) == 1)
            {
                h1D_NHadronCount_NTrueVtxCut -> Fill(PrimaryG4P_Eta -> at(true_i));

                // if (fabs(PrimaryG4P_Eta->at(true_i)) < 4){
                //     nHadron_count++;
                // }
            }
        } 

        h1D_TrueEvtCount_NTrueVtxCut -> Fill(INTTvtxZ);
    }    

    h1D_TruedNdEta = (TH1D*) h1D_NHadronCount -> Clone("h1D_TruedNdEta");
    h1D_TruedNdEta -> Sumw2(true);
    h1D_TruedNdEta -> Scale(1./h1D_TrueEvtCount->Integral());
    h1D_TruedNdEta -> Scale(1./ h1D_TruedNdEta->GetBinWidth(1));

    h1D_TruedNdEta_NTrueVtxCut = (TH1D*) h1D_NHadronCount_NTrueVtxCut -> Clone("h1D_TruedNdEta_NTrueVtxCut");
    h1D_TruedNdEta_NTrueVtxCut -> Sumw2(true);
    h1D_TruedNdEta_NTrueVtxCut -> Scale(1./h1D_TrueEvtCount_NTrueVtxCut->Integral());
    h1D_TruedNdEta_NTrueVtxCut -> Scale(1./ h1D_TruedNdEta_NTrueVtxCut->GetBinWidth(1));

    file_out -> cd();
    h1D_NHadronCount -> Write();
    h1D_TrueEvtCount -> Write();
    h1D_TruedNdEta -> Write();

    h1D_NHadronCount_NTrueVtxCut -> Write();
    h1D_TrueEvtCount_NTrueVtxCut -> Write();
    h1D_TruedNdEta_NTrueVtxCut -> Write();

    file_out -> Close();

    // system(Form("mv %s/%s %s/completed/%s", output_directory.c_str(), output_filename.c_str(), output_directory.c_str(), output_filename.c_str()));

    return 0;
    
}