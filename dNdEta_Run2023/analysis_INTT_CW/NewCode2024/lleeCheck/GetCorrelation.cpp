#include "/sphenix/user/ChengWei/INTT/INTT_dNdeta_repo/NewCode2024/Constants.cpp"

class GetCorrelation
{
    public:
        GetCorrelation(
            string input_directory_in,
            string input_filename_in,
            string output_directory_in,
            string output_filename_in,
            int Nfiles_in,
            string MBD_z_vtx_directory_in,
            
            bool DoSelfCentrality_in,
            bool DoVtxZReWeight_in,
            bool ReadRecoEvtFile_in = false
        );
        

    private: 
        string input_directory;
        string input_filename;
        string output_directory;
        string output_filename;
        int Nfiles;
        string MBD_z_vtx_directory;
        bool DoSelfCentrality;
        bool DoVtxZReWeight;
        bool ReadRecoEvtFile;

        // Division : ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        TH1D * h1D_reweight;
        void GetVtxZReWeight();
        
        // Division : ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        TFile * file_in;
        TChain * tree_in;
        float MBD_z_vtx;
        bool is_min_bias;
        float MBD_centrality;
        float MBD_south_charge_sum;
        float MBD_north_charge_sum;
        float MBD_charge_sum;
        float MBD_charge_asymm;
        int InttBcoFullDiff_next;
        int NClus;
        std::vector<int> *ClusLayer;
        std::vector<int> *ClusAdc;
        std::vector<float> *ClusPhiSize;
        void PrepareTree();

        // Division : ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        int self_MBD_centrality;
        std::vector<double> MBD_charge_sum_vec_percentage_cut;
        void GetSelfCentralitySet();
        int GetSelfCentrality(double MBD_charge_sum_in);

        // Division : ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        TFile * file_out;
        TH2D * h2D_Mbin_NClus;
        TH2D * h2D_Mbin_NClus_Self; // note : self-centrality
        TH1D * h1D_mbd_z_vtx_narrow;
        TH2D * h2D_Centrality_correlation;
        void PrepareOut();

        // Division : ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        void mainAN();

        // Division : ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        void EndRun();
};

GetCorrelation::GetCorrelation(
    string input_directory_in,
    string input_filename_in,
    string output_directory_in,
    string output_filename_in,
    int Nfiles_in,
    string MBD_z_vtx_directory_in,

    bool DoSelfCentrality_in,
    bool DoVtxZReWeight_in,
    bool ReadRecoEvtFile_in
):
    input_directory(input_directory_in),
    input_filename(input_filename_in),
    output_directory(output_directory_in),
    output_filename(output_filename_in),
    Nfiles(Nfiles_in),
    MBD_z_vtx_directory(MBD_z_vtx_directory_in),

    DoSelfCentrality(DoSelfCentrality_in),
    DoVtxZReWeight(DoVtxZReWeight_in),
    ReadRecoEvtFile(ReadRecoEvtFile_in)
{
    if (DoVtxZReWeight){GetVtxZReWeight();}
    PrepareTree();
    PrepareOut();
    if (DoSelfCentrality){GetSelfCentralitySet();}

    mainAN();
    EndRun();
}

void GetCorrelation::GetVtxZReWeight()
{
    std::string Run54280_file_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280_HR_Feb102025/lltoee_check/MBD_z_vtx.root";
    std::string hist_name = "h1D_mbd_z_vtx";

    TFile * file_in = TFile::Open(Run54280_file_directory.c_str());
    TH1D * hist = (TH1D*)file_in->Get(hist_name.c_str());
    hist->Scale(1./hist->Integral());
    hist->Sumw2(true);

    TFile * file_in_2 = TFile::Open(MBD_z_vtx_directory.c_str());
    if (!file_in_2) {std::cout<<"file_in_2 not found"<<std::endl; exit(1);}

    TH1D * hist_2 = (TH1D*)file_in_2->Get(hist_name.c_str());
    hist_2->Scale(1./hist_2->Integral());

    h1D_reweight = (TH1D*)hist->Clone("h1D_reweight");
    h1D_reweight->Reset("ICESM");
    h1D_reweight->Divide(hist, hist_2);
}

void GetCorrelation::PrepareTree()
{
    tree_in = new TChain("EventTree");

    for (int i = 0; i < Nfiles; i++)
    {
        std::string job_index = std::to_string( i );
        int job_index_len = 5;
        job_index.insert(0, job_index_len - job_index.size(), '0');

        std::string input_filename_full = Form("%s/%s%s.root", input_directory.c_str(), input_filename.c_str(), job_index.c_str());
        std::cout<<"input_filename_full: "<<input_filename_full<<std::endl;
        tree_in -> Add(input_filename_full.c_str());
    }

    std::cout<<"tree_in -> GetEntries(): "<<tree_in -> GetEntries()<<std::endl;

    tree_in -> SetBranchStatus("*", 0);

    tree_in -> SetBranchStatus("MBD_z_vtx", 1);
    tree_in -> SetBranchStatus("is_min_bias", 1);
    tree_in -> SetBranchStatus("MBD_centrality", 1);
    tree_in -> SetBranchStatus("MBD_south_charge_sum", 1);
    tree_in -> SetBranchStatus("MBD_north_charge_sum", 1);
    tree_in -> SetBranchStatus("MBD_charge_sum", 1);
    tree_in -> SetBranchStatus("MBD_charge_asymm", 1);
    
    tree_in -> SetBranchStatus("NClus", 1);
    tree_in -> SetBranchStatus("ClusAdc", 1);
    tree_in -> SetBranchStatus("ClusPhiSize", 1);

    if (ReadRecoEvtFile){tree_in -> SetBranchStatus("InttBcoFullDiff_next", 1);}

    
    ClusLayer = 0;
    ClusAdc = 0;
    ClusPhiSize = 0;

    tree_in -> SetBranchAddress("MBD_z_vtx", &MBD_z_vtx);
    tree_in -> SetBranchAddress("is_min_bias", &is_min_bias);
    tree_in -> SetBranchAddress("MBD_centrality", &MBD_centrality);
    tree_in -> SetBranchAddress("MBD_south_charge_sum", &MBD_south_charge_sum);
    tree_in -> SetBranchAddress("MBD_north_charge_sum", &MBD_north_charge_sum);
    tree_in -> SetBranchAddress("MBD_charge_sum", &MBD_charge_sum);
    tree_in -> SetBranchAddress("MBD_charge_asymm", &MBD_charge_asymm);

    tree_in -> SetBranchAddress("NClus",&NClus);
    tree_in -> SetBranchAddress("ClusAdc",&ClusAdc);
    tree_in -> SetBranchAddress("ClusPhiSize",&ClusPhiSize);

    if (ReadRecoEvtFile){tree_in -> SetBranchAddress("InttBcoFullDiff_next", &InttBcoFullDiff_next);}
}

void GetCorrelation::PrepareOut()
{
    vector<double> centrality_edges = Constants::centrality_edges;

    file_out = new TFile(Form("%s/%s", output_directory.c_str(), output_filename.c_str()), "recreate");
    h2D_Mbin_NClus = new TH2D("h2D_Mbin_NClus","h2D_Mbin_NClus;Centrality;NClus",centrality_edges.size() - 1, &centrality_edges[0], 500, 0, 10000);
    h2D_Mbin_NClus_Self = new TH2D("h2D_Mbin_NClus_Self","h2D_Mbin_NClus_Self;Centrality;NClus",centrality_edges.size() - 1, &centrality_edges[0], 500, 0, 10000);
    h1D_mbd_z_vtx_narrow = new TH1D("h1D_mbd_z_vtx_narrow", "h1D_mbd_z_vtx_narrow;MBD_z_vtx [cm];Counts", 120, -60, 60);
    h2D_Centrality_correlation = new TH2D("h2D_Centrality_correlation","h2D_Centrality_correlation;Official Centrality;Self Centrality", 101, -0.5, 100.5, 101, -0.5, 100.5);
}

void GetCorrelation::GetSelfCentralitySet()
{

    std::vector<int> MBD_charge_sum_vec; MBD_charge_sum_vec.clear();

    for (int i = 0; i < tree_in -> GetEntries(); i++)
    {
        tree_in -> GetEntry(i);

        if (i % 10000 == 0)
        {
            std::cout<<"self-centrality, i: "<<i<<std::endl;
        }    

        // note : both data and MC
        if (is_min_bias != 1) {continue;}
        if (MBD_z_vtx != MBD_z_vtx) {continue;}
        if (MBD_centrality != MBD_centrality) {continue;}
        if (MBD_centrality < 0 || MBD_centrality > 100) {continue;}
        if (NClus <= 5) {continue;}
        if (MBD_z_vtx < -10 || MBD_z_vtx >= 10) {continue;}

        if (ReadRecoEvtFile && InttBcoFullDiff_next <= 61) {continue;}

        if (MBD_centrality > 92) {continue;}

        MBD_charge_sum_vec.push_back(MBD_charge_sum);
    }     

    std::sort(MBD_charge_sum_vec.begin(), MBD_charge_sum_vec.end(),std::greater<int>());

    MBD_charge_sum_vec_percentage_cut.clear();


    for (int i = 1; i <= 92; i++)
    {
        int index = (int)(MBD_charge_sum_vec.size() * i / 100);
        MBD_charge_sum_vec_percentage_cut.push_back(MBD_charge_sum_vec[index]);

        std::cout<<"i: "<<i<<", MBD_charge_sum_vec[index]: "<<MBD_charge_sum_vec[index]<<std::endl;
    }
}

int GetCorrelation::GetSelfCentrality(double MBD_charge_sum_in)
{
    for (int i = 0; i < MBD_charge_sum_vec_percentage_cut.size(); i++)
    {
        if (MBD_charge_sum_in >= MBD_charge_sum_vec_percentage_cut[i])
        {
            return i + 1;
        }
    }

    return 0;
}

void GetCorrelation::mainAN()
{
    for (int i = 0; i < tree_in -> GetEntries(); i++)
    {
        tree_in -> GetEntry(i);

        if (i % 10000 == 0)
        {
            std::cout<<"i: "<<i<<std::endl;
        }    

        int nHadron_count = 0;

        // note : both data and MC
        if (is_min_bias != 1) {continue;}
        if (MBD_z_vtx != MBD_z_vtx) {continue;}
        if (MBD_centrality != MBD_centrality) {continue;}
        if (MBD_centrality < 0 || MBD_centrality > 100) {continue;}
        if (NClus <= 5) {continue;}
        if (MBD_z_vtx < -10 || MBD_z_vtx >= 10) {continue;}

        if (ReadRecoEvtFile && InttBcoFullDiff_next <= 61) {continue;}

        if (MBD_centrality > 92) {continue;}

        double INTTvtxZWeighting;
        INTTvtxZWeighting = (DoVtxZReWeight) ? h1D_reweight -> GetBinContent(h1D_reweight -> FindBin(MBD_z_vtx)) : 1.;


        int NClus_count = 0;
        for (int clu_i = 0; clu_i < NClus; clu_i++)
        {
            if (ClusAdc -> at(clu_i) <= 35) {continue;}
            if (ClusPhiSize -> at(clu_i) > 40) {continue;}
            NClus_count++;
        }

        h2D_Mbin_NClus -> Fill(MBD_centrality, NClus_count, INTTvtxZWeighting);
        h1D_mbd_z_vtx_narrow -> Fill(MBD_z_vtx, INTTvtxZWeighting);

        if (DoSelfCentrality)
        {
            self_MBD_centrality = GetSelfCentrality(MBD_charge_sum);
            h2D_Mbin_NClus_Self -> Fill(self_MBD_centrality, NClus_count, INTTvtxZWeighting);
            h2D_Centrality_correlation -> Fill(MBD_centrality, self_MBD_centrality, INTTvtxZWeighting);
        }
    }    
}

void GetCorrelation::EndRun()
{
    file_out -> cd();
    h2D_Mbin_NClus -> Write();
    h1D_mbd_z_vtx_narrow -> Write();
    if (DoVtxZReWeight) {h1D_reweight -> Write("h1D_reweight");}
    if (DoSelfCentrality){h2D_Centrality_correlation -> Write();}
    if (DoSelfCentrality){h2D_Mbin_NClus_Self -> Write();}

    file_out -> Close();
}

void GetCorrelation_main()
{
    string input_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280_HR_Feb102025/Run6_EvtZFitWidthChange/EvtVtxZ/completed";
    string input_filename = "Data_EvtVtxZProtoTracklet_FieldOff_BcoFullDiff_VtxZReco_00054280_";
    
    string output_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280_HR_Feb102025/lltoee_check";
    string output_filename = Form("Data_MBin92_NClus.root");

    int Nfiles = 1000;

    string MBD_z_vtx_directory = "no_map";

    bool DoSelfCentrality = true;
    bool DoVtxZReWeight = false;
    bool ReadRecoEvtFile = true;

    GetCorrelation * GetCorrelation_ = new GetCorrelation(
        input_directory,
        input_filename,
        output_directory,
        output_filename,
        Nfiles,
        MBD_z_vtx_directory,

        DoSelfCentrality,
        DoVtxZReWeight,
        ReadRecoEvtFile
    );
}


void GetCorrelation_MCRun54280()
{
    string input_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/sPH_dNdeta/Run24AuAuMC/Sim_HIJING_MDC2_ana472_20250307/per5k";
    string input_filename = "ntuple_per5k_";
    
    string output_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/sPH_dNdeta/Run24AuAuMC/Sim_HIJING_MDC2_ana472_20250307/lltoee_check";
    string output_filename = Form("MC_MBin92_NClus.root");

    int Nfiles = 146;
    string MBD_z_vtx_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/sPH_dNdeta/Run24AuAuMC/Sim_HIJING_MDC2_ana472_20250307/lltoee_check/MBD_z_vtx.root";

    bool DoSelfCentrality = true;
    bool DoVtxZReWeight = true;
    bool ReadRecoEvtFile = false;

    GetCorrelation * GetCorrelation_ = new GetCorrelation(
        input_directory,
        input_filename,
        output_directory,
        output_filename,
        Nfiles,
        MBD_z_vtx_directory,

        DoSelfCentrality,
        DoVtxZReWeight,
        ReadRecoEvtFile
    );
}


void GetCorrelation_MCFieldON()
{
    string input_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/sPH_dNdeta/Run24AuAuMC/WrongStrangeIncrease_FieldOn_Sim_HIJING_strangeness_MDC2_ana467_20250226/per5k";
    string input_filename = "ntuple_per5k_";
    
    string output_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/sPH_dNdeta/Run24AuAuMC/WrongStrangeIncrease_FieldOn_Sim_HIJING_strangeness_MDC2_ana467_20250226/lltoee_check";
    string output_filename = Form("MC_MBin92_NClus.root");

    int Nfiles = 196;
    string MBD_z_vtx_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/sPH_dNdeta/Run24AuAuMC/WrongStrangeIncrease_FieldOn_Sim_HIJING_strangeness_MDC2_ana467_20250226/lltoee_check/MBD_z_vtx.root";

    bool DoSelfCentrality = true;
    bool DoVtxZReWeight = true;
    bool ReadRecoEvtFile = false;

    GetCorrelation * GetCorrelation_ = new GetCorrelation(
        input_directory,
        input_filename,
        output_directory,
        output_filename,
        Nfiles,
        MBD_z_vtx_directory,

        DoSelfCentrality,
        DoVtxZReWeight,
        ReadRecoEvtFile
    );
}


void GetCorrelation_DataRun54912()
{
    string input_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54912/completed";
    string input_filename = "testNtuple_";
    
    string output_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54912/lltoee_check";
    string output_filename = Form("Data_MBin92_NClus.root");

    int Nfiles = 1;
    string MBD_z_vtx_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54912/lltoee_check/MBD_z_vtx.root";

    bool DoSelfCentrality = true;
    bool DoVtxZReWeight = true;
    bool ReadRecoEvtFile = false;

    GetCorrelation * GetCorrelation_ = new GetCorrelation(
        input_directory,
        input_filename,
        output_directory,
        output_filename,
        Nfiles,
        MBD_z_vtx_directory,

        DoSelfCentrality,
        DoVtxZReWeight,
        ReadRecoEvtFile
    );
}