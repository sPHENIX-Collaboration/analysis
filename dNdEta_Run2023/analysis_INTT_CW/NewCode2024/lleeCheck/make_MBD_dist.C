#include "/sphenix/user/ChengWei/INTT/INTT_dNdeta_repo/NewCode2024/Constants.cpp"

int make_MBD_dist(bool isData = true)
{
    string input_directory; 
    string input_filename;
    string output_directory;
    string output_filename;
    int Nfiles;

    if (isData == true){
        input_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280_HR_Feb102025/Run6_EvtZFitWidthChange/EvtVtxZ/completed";
        input_filename = "Data_EvtVtxZProtoTracklet_FieldOff_BcoFullDiff_VtxZReco_00054280_";
        
        output_directory = input_directory;
        output_filename = Form("MBD_z_vtx.root");

        Nfiles = 1000;
    }
    else {
        input_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/sPH_dNdeta/Run24AuAuMC/Sim_HIJING_MDC2_ana472_20250307/Run7/EvtVtxZ/completed";
        input_filename = "MC_EvtVtxZProtoTracklet_FieldOff_VtxZReco_";
        
        output_directory = input_directory;
        output_filename = Form("MC_MBin70_NClus.root");

        Nfiles = 146;
    }
 

    // TFile * file_in = TFile::Open(Form("%s/%s", input_directory.c_str(), input_filename.c_str()));
    // TTree * tree_in = (TTree*)file_in->Get("EventTree");     

    TChain * tree_in = new TChain("EventTree");
    // tree_in -> Add(Form("%s/%s000{00..24}.root", input_directory.c_str(), input_filename.c_str()));

    for (int i = 0; i < Nfiles; i++)
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

    tree_in -> SetBranchStatus("NClus", 1);
    tree_in -> SetBranchStatus("ClusAdc", 1);
    tree_in -> SetBranchStatus("ClusPhiSize", 1);

    if (isData){tree_in -> SetBranchStatus("InttBcoFullDiff_next",1);}

    if (!isData){tree_in -> SetBranchStatus("NTruthVtx", 1);}
    if (!isData){tree_in -> SetBranchStatus("PrimaryG4P_Pt", 1);}
    if (!isData){tree_in -> SetBranchStatus("PrimaryG4P_Eta", 1);}
    if (!isData){tree_in -> SetBranchStatus("PrimaryG4P_Phi", 1);}
    if (!isData){tree_in -> SetBranchStatus("PrimaryG4P_E", 1);}
    if (!isData){tree_in -> SetBranchStatus("PrimaryG4P_PID", 1);}
    if (!isData){tree_in -> SetBranchStatus("PrimaryG4P_isChargeHadron", 1);}

    if (!isData){tree_in -> SetBranchStatus("TruthPV_trig_z", 1);}

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
    int InttBcoFullDiff_next;

    // note : trigger tag
    // int MBDNSg2;

    // std::vector<int> *ClusLayer = 0;
    // std::vector<double> *ClusEta_INTTz = 0;

    int NClus;
    std::vector<int> *ClusLayer = 0;
    std::vector<int> *ClusAdc = 0;
    std::vector<float> *ClusPhiSize = 0;

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
    if(isData){tree_in -> SetBranchAddress("InttBcoFullDiff_next", &InttBcoFullDiff_next);}

    // tree_in -> SetBranchAddress("MBDNSg2", &MBDNSg2);

    // tree_in -> SetBranchAddress("ClusLayer", &ClusLayer);
    // tree_in -> SetBranchAddress("ClusEta_INTTz", &ClusEta_INTTz);

    tree_in -> SetBranchAddress("NClus",&NClus);
    tree_in -> SetBranchAddress("ClusAdc",&ClusAdc);
    tree_in -> SetBranchAddress("ClusPhiSize",&ClusPhiSize);

    tree_in -> SetBranchAddress("INTTvtxZ", &INTTvtxZ);
    tree_in -> SetBranchAddress("INTTvtxZError", &INTTvtxZError);
    tree_in -> SetBranchAddress("NgroupTrapezoidal", &NgroupTrapezoidal);
    tree_in -> SetBranchAddress("NgroupCoarse", &NgroupCoarse);
    tree_in -> SetBranchAddress("TrapezoidalFitWidth", &TrapezoidalFitWidth);
    tree_in -> SetBranchAddress("TrapezoidalFWHM", &TrapezoidalFWHM);

    // if (!isData){tree_in -> SetBranchAddress("NTruthVtx", &NTruthVtx);}
    // if (!isData){tree_in -> SetBranchAddress("PrimaryG4P_Pt", &PrimaryG4P_Pt);}
    // if (!isData){tree_in -> SetBranchAddress("PrimaryG4P_Eta", &PrimaryG4P_Eta);}
    // if (!isData){tree_in -> SetBranchAddress("PrimaryG4P_Phi", &PrimaryG4P_Phi);}
    // if (!isData){tree_in -> SetBranchAddress("PrimaryG4P_E", &PrimaryG4P_E);}
    // if (!isData){tree_in -> SetBranchAddress("PrimaryG4P_PID", &PrimaryG4P_PID);}
    // if (!isData){tree_in -> SetBranchAddress("PrimaryG4P_isChargeHadron", &PrimaryG4P_isChargeHadron);}
    // if (!isData){tree_in -> SetBranchAddress("TruthPV_trig_z", &TruthPV_trig_z);}

    // std::cout<<"cccc"<<std::endl;
    // tree_in->GetEntry(0);
    // std::cout<<"cccc"<<std::endl;


    // Division : ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    // std::string job_index = std::to_string( process_id );
    // int job_index_len = 5;
    // job_index.insert(0, job_index_len - job_index.size(), '0');

    // std::string output_filename = Form("INTTz_New_QuickCheck_%s.root", job_index.c_str());

    std::cout<<111<<std::endl;

    vector<double> centrality_edges = Constants::centrality_edges;
    // TH2D * h2D_Mbin_NClus = new TH2D("h2D_Mbin_NClus","h2D_Mbin_NClus;Centrality;NClus",centrality_edges.size() - 1, &centrality_edges[0], 500, 0, 10000);

    TH1D * h1D_mbd_z_vtx = new TH1D("h1D_mbd_z_vtx", "h1D_mbd_z_vtx;MBD_z_vtx [cm];Counts", 120, -60, 60);

    TFile * file_out = new TFile(Form("%s/%s", output_directory.c_str(), output_filename.c_str()), "recreate");

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
        if (isData && InttBcoFullDiff_next <= 61) {continue;}

        if (MBD_centrality > 70) {continue;}

        h1D_mbd_z_vtx -> Fill(MBD_z_vtx);
    }    

    file_out -> cd();
    h1D_mbd_z_vtx -> Write();

    file_out -> Close();

    // system(Form("mv %s/%s %s/completed/%s", output_directory.c_str(), output_filename.c_str(), output_directory.c_str(), output_filename.c_str()));

    return 0;
    
}


int make_MBD_dist_2(bool isData = true) // note : for field ON
{
    string input_directory; 
    string input_filename;
    string output_directory;
    string output_filename;
    int Nfiles;

    if (isData == true){
        input_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54912/completed";
        input_filename = "testNtuple_"; // note : testNtuple_00000.root
        
        output_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54912/lltoee_check";
        output_filename = Form("MBD_z_vtx.root");

        Nfiles = 1;
    }
    else {
        input_directory = "/sphenix/user/ChengWei/sPH_dNdeta/Run24AuAuMC/WrongStrangeIncrease_FieldOn_Sim_HIJING_strangeness_MDC2_ana467_20250226/per5k";
        input_filename = "ntuple_per5k_";
        
        output_directory = input_directory;
        output_filename = Form("MBD_z_vtx.root");

        Nfiles = 196;
    }
 

    // TFile * file_in = TFile::Open(Form("%s/%s", input_directory.c_str(), input_filename.c_str()));
    // TTree * tree_in = (TTree*)file_in->Get("EventTree");     

    TChain * tree_in = new TChain("EventTree");
    // tree_in -> Add(Form("%s/%s000{00..24}.root", input_directory.c_str(), input_filename.c_str()));

    for (int i = 0; i < Nfiles; i++)
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

    float MBD_z_vtx;
    bool is_min_bias;
    float MBD_centrality;
    float MBD_south_charge_sum;
    float MBD_north_charge_sum;
    float MBD_charge_sum;
    float MBD_charge_asymm;
    int InttBcoFullDiff_next;


    tree_in -> SetBranchAddress("MBD_z_vtx", &MBD_z_vtx);
    tree_in -> SetBranchAddress("is_min_bias", &is_min_bias);
    tree_in -> SetBranchAddress("MBD_centrality", &MBD_centrality);
    tree_in -> SetBranchAddress("MBD_south_charge_sum", &MBD_south_charge_sum);
    tree_in -> SetBranchAddress("MBD_north_charge_sum", &MBD_north_charge_sum);
    tree_in -> SetBranchAddress("MBD_charge_sum", &MBD_charge_sum);
    tree_in -> SetBranchAddress("MBD_charge_asymm", &MBD_charge_asymm);




    // Division : ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    vector<double> centrality_edges = Constants::centrality_edges;
    // TH2D * h2D_Mbin_NClus = new TH2D("h2D_Mbin_NClus","h2D_Mbin_NClus;Centrality;NClus",centrality_edges.size() - 1, &centrality_edges[0], 500, 0, 10000);

    TH1D * h1D_mbd_z_vtx = new TH1D("h1D_mbd_z_vtx", "h1D_mbd_z_vtx;MBD_z_vtx [cm];Counts", 120, -60, 60);

    TFile * file_out = new TFile(Form("%s/%s", output_directory.c_str(), output_filename.c_str()), "recreate");

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

        // int nHadron_count = 0;

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

        if (MBD_centrality > 70) {continue;}

        h1D_mbd_z_vtx -> Fill(MBD_z_vtx);
    }    

    file_out -> cd();
    h1D_mbd_z_vtx -> Write();

    file_out -> Close();

    // system(Form("mv %s/%s %s/completed/%s", output_directory.c_str(), output_filename.c_str(), output_directory.c_str(), output_filename.c_str()));

    return 0;
    
}