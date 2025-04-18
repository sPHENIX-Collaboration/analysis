#ifndef INTTReadTree_h
#define INTTReadTree_h

#include "../INTTDSTchain.C"
#include "PrivateCluReader.C"
#include "../private_cluster_gen/InttConversion_new.h"
#include "../private_cluster_gen/InttClustering.h"
#include "ReadMCselfgen/MCSelfGenReader.C"
#include "ReadF4ANtupleMC/ReadF4ANtupleMC.C"
#include "ReadF4ANtupleData/ReadF4ANtupleData.C"

class INTTReadTree
{
    public : 
        vector<clu_info> temp_sPH_inner_nocolumn_vec; 
        vector<clu_info> temp_sPH_outer_nocolumn_vec; 
        vector<vector<double>> temp_sPH_nocolumn_vec;
        vector<vector<double>> temp_sPH_nocolumn_rz_vec;
        
        INTTReadTree(
            int data_type, 
            string input_directory, 
            string MC_list_name, 
            string tree_name, 
            int clu_size_cut, 
            int clu_sum_adc_cut, 
            int random_seed = 65539, 
            int N_ladder = 14, 
            double offset_range = 0.2, 
            vector<string> included_ladder_vec = {}, 
            int apply_geo_offset = 1, 
            vector<vector<double>> ladder_offset_vec = {}
        );
        void EvtInit(long long event_i);
        void EvtSetCluGroup();
        long long GetNEvt();
        unsigned long GetEvtNClus();
        unsigned long GetEvtNClusPost();
        double GetTrigZvtxMC();
        vector<double> GetTrigvtxMC();
        vector<vector<float>> GetTrueTrackInfo();
        bool GetPhiCheckTag();
        float GetCentralityBin();
        int GetNvtxMC();
        string GetRunType();
        Long64_t GetBCOFull();
        double GetMBDRecoZ();
        int GetIsMinBias();
        int GetIsMinBiasWoZDC();
        double GetMBDNorthChargeSum();
        double GetMBDSouthChargeSum();
        void EvtClear();
        map<string, vector<double>> GetLadderOffsetMap();
        void EndRun();
        void gen_ladder_offset();
        void clear_ladder_offset_map() {ladder_offset_map.clear();}
        void set_random_seed(int random_seed_in) {random_seed = random_seed_in;}
        void set_ladder_offset(map<string, vector<double>> input_map) {ladder_offset_map = input_map;}

    private : 
        string data_type_list[9] = {
            "MC",
            "data_DST",
            "data_private", 
            "data_private_geo1", 
            "MC_geo_test", 
            "MC_inner_phi_rotation", 
            "MC_selfgen", 
            "data_F4A", 
            "data_F4A_inner_phi_rotation"
        };
        long long N_event;

        string input_directory; 
        string MC_list_name;
        string tree_name;
        double clu_sum_adc_cut;
        double offset_range;
        int apply_geo_offset;
        int random_seed;
        int clu_size_cut;
        int data_type;   
        int N_ladder;
        int NvtxMC; 
        int is_min_bias;  // note : for data and MC
        int is_min_bias_wozdc; // note : only for data
        double MBD_south_charge_sum, MBD_north_charge_sum;
        double MBD_reco_z;
        double TrigXvtxMC;
        double TrigYvtxMC;
        double TrigZvtxMC;
        float  Centrality_bimp;
        float  Centrality_mbd;
        Long64_t bco_full;

        vector<vector<float>>  true_track_info;
        vector<string> included_ladder_vec;
        vector<vector<double>> ladder_offset_vec;
        map<string, vector<double>> ladder_offset_map;

        TChain * chain_in;
        PrivateCluReader * inttCluData; // note : the class to read the private gen cluster file
        InttConversion * inttConv; // note : the class to conver the server_module into ladder name
        MCSelfGenReader * inttMCselfgen; // note : the class to read the self generated MC sample
        TFile * file_in = nullptr;
        TTree * tree;

        // note : the class to read the MC sample 
        // INTTDSTchain * inttDSTMC;    // note : read with TChain
        ReadF4ANtupleMC * inttDSTMC; // note : read with TTree, this is the latest
        ReadF4ANtupleData * inttDSTData; // note : read with TTree, this is the latest, for the data produced by F4A

        vector<int> MBin_NClus_cut_vec;

        string run_type_out;

        // vector<string> included_ladder_vec = {
        //     "B0L000S","B0L002S","B0L003S", "B0L005S", "B0L006S", "B0L008S", "B0L009S", 
        //     "B1L000S","B1L003S","B1L004S", "B1L007S", "B1L008S", "B1L011S", "B1L012S"
        // };// todo : should be updated in the future when the progress is made and we moved to the next geoemtry correction step 
        
        unsigned long evt_length;

        void TChainInit_MC();
        void TTreeInit_private();
        void TTreeInit_private_geo1();
        void TChainInit_MC_geo_test();
        void TTreeInit_MC_selfgen();
        void TTreeInit_data_F4A();
        double get_radius(double x, double y);
        pair<double,double> rotatePoint(double x, double y);
        vector<double> offset_correction(map<string,vector<double>> input_map);
        void read_centrality_cut();        // note : for the selfgen MC
        int get_centrality_bin(int NClus); // note : for the selfgen MC

};

INTTReadTree::INTTReadTree(int data_type, string input_directory, string MC_list_name, string tree_name, int clu_size_cut, int clu_sum_adc_cut, int random_seed, int N_ladder, double offset_range, vector<string> included_ladder_vec, int apply_geo_offset, vector<vector<double>> ladder_offset_vec)
:data_type(data_type), input_directory(input_directory), MC_list_name(MC_list_name), tree_name(tree_name), clu_size_cut(clu_size_cut), clu_sum_adc_cut(clu_sum_adc_cut), random_seed(random_seed), N_ladder(N_ladder), offset_range(offset_range), included_ladder_vec(included_ladder_vec), apply_geo_offset(apply_geo_offset), ladder_offset_vec(ladder_offset_vec)
{
    temp_sPH_inner_nocolumn_vec.clear();
    temp_sPH_outer_nocolumn_vec.clear();
    
    temp_sPH_nocolumn_vec.clear();
    temp_sPH_nocolumn_vec = vector<vector<double>> (2);

    temp_sPH_nocolumn_rz_vec.clear();
    temp_sPH_nocolumn_rz_vec = vector<vector<double>> (2);

    ladder_offset_map.clear();

    true_track_info.clear();

    MBin_NClus_cut_vec.clear();

    if (data_type_list[data_type] == "MC" || data_type_list[data_type] == "MC_inner_phi_rotation") 
    {
        run_type_out = "MC";
        std::cout<<"--- INTTReadTree -> input data type: MC ---"<<std::endl;
        TChainInit_MC();
        std::cout<<"--- INTTReadTree -> Initialization done ---"<<std::endl;

        if (data_type_list[data_type] == "MC_inner_phi_rotation") {std::cout<<"--- INTTReadTree -> note, the clusters in the inner barrel will be rotated ---"<<std::endl;}
    }
    else if (data_type_list[data_type] == "data_private")
    {
        run_type_out = "data_private";
        std::cout<<"--- INTTReadTree -> input data type: private gen cluster ---"<<std::endl;
        TTreeInit_private();
        std::cout<<"--- INTTReadTree -> Initialization done ---"<<std::endl;
    }
    else if (data_type_list[data_type] == "data_private_geo1")
    {
        run_type_out = "data_private_geo1";
        std::cout<<"--- INTTReadTree -> input data type: private gen cluster with geo correction 1 ---"<<std::endl;
        TTreeInit_private_geo1();
        std::cout<<"--- INTTReadTree -> Initialization done ---"<<std::endl;
    }
    else if (data_type_list[data_type] == "MC_geo_test")
    {
        run_type_out = "MC";
        // std::cout<<"--- INTTReadTree -> input data type: MC geometry test ---"<<std::endl; // note : was used
        TChainInit_MC_geo_test();
        // std::cout<<"--- INTTReadTree -> Initialization done ---"<<std::endl; // note : was used
    }
    else if (data_type_list[data_type] == "MC_selfgen")
    {
        run_type_out = "MC";
        read_centrality_cut();
        TTreeInit_MC_selfgen();
    }
    else if (data_type_list[data_type] == "data_F4A" || data_type_list[data_type] == "data_F4A_inner_phi_rotation")
    {
        run_type_out = "data";
        std::cout<<"--- INTTReadTree -> input data type: data F4A ---"<<std::endl;
        TTreeInit_data_F4A();
        std::cout<<"--- INTTReadTree -> Initialization done ---"<<std::endl;
    }

}

void INTTReadTree::read_centrality_cut()
{
    TFile * file_MBin_cut = TFile::Open(Form("%s/MBin_NClus_cut.root", input_directory.c_str()));
    TTree * tree_MBin_cut = (TTree*)file_MBin_cut->Get("tree");
    vector<int> *MBinCut_vec; MBinCut_vec = 0;
    tree_MBin_cut -> SetBranchAddress("MBinCut", &MBinCut_vec);
    tree_MBin_cut -> GetEntry(0);
    cout<<"check MBinCut_vec size : "<<MBinCut_vec->size()<<endl;
    for (int i = 0; i < MBinCut_vec->size(); i++) {MBin_NClus_cut_vec.push_back(MBinCut_vec->at(i));}

    file_MBin_cut -> Close();
    return;
}

int INTTReadTree::get_centrality_bin(int NClus)
{
    int MBin_id = -1;
    for (int i = 0; i < MBin_NClus_cut_vec.size(); i++)
    {
        if (NClus > MBin_NClus_cut_vec[i]) {MBin_id = i; break;}
    }

    if (MBin_id == -1) { MBin_id = MBin_NClus_cut_vec.size(); }

    return MBin_id;
}

void INTTReadTree::TTreeInit_MC_selfgen()
{
    file_in = TFile::Open(Form("%s/%s.root", input_directory.c_str(), MC_list_name.c_str()));
    tree = (TTree *)file_in->Get(tree_name.c_str());   
    N_event = tree -> GetEntries();
    inttMCselfgen = new MCSelfGenReader(tree);
}

void INTTReadTree::TTreeInit_data_F4A()
{
    file_in = TFile::Open(Form("%s/%s.root", input_directory.c_str(), MC_list_name.c_str()));
    tree = (TTree *)file_in->Get(tree_name.c_str());   
    N_event = tree -> GetEntries();
    inttDSTData = new ReadF4ANtupleData(tree);
}

void INTTReadTree::TChainInit_MC()
{
    // note : this is the latest format, probably fixed
    // note : the idea is that, for the avgVTXxy, we are going to read the merged file
    // note : for the evtVTXz, and evtTracklet, we are going to read the separated files for each condor job
    // note : which means, we might not necessarily need to use TChain
    file_in = TFile::Open(Form("%s/%s.root", input_directory.c_str(), MC_list_name.c_str()));
    tree = (TTree *)file_in->Get(tree_name.c_str());   
    N_event = tree -> GetEntries();
    inttDSTMC = new ReadF4ANtupleMC(tree);

    // chain_in = new TChain(tree_name.c_str());
    // inttDSTMC = new INTTDSTchain(chain_in, input_directory, MC_list_name);
    // N_event = chain_in->GetEntries();
    // std::printf("inttDSTMC N event : %lli \n", N_event); // note : was used
}

void INTTReadTree::TChainInit_MC_geo_test()
{
    // note : this is the latest format, probably fixed
    // note : the idea is that, for the avgVTXxy, we are going to read the merged file
    // note : for the evtVTXz, and evtTracklet, we are going to read the separated files for each condor job
    // note : which means, we might not necessarily need to use TChain
    file_in = TFile::Open(Form("%s/%s.root", input_directory.c_str(), MC_list_name.c_str()));
    tree = (TTree *)file_in->Get(tree_name.c_str());   
    N_event = tree -> GetEntries();
    inttDSTMC = new ReadF4ANtupleMC(tree);

    // chain_in = new TChain(tree_name.c_str());
    // inttDSTMC = new INTTDSTchain(chain_in, input_directory, MC_list_name);
    // N_event = chain_in->GetEntries();
    // std::printf("inttDSTMC N event : %lli \n", N_event); // note : was used
    // gen_ladder_offset();
}

void INTTReadTree::TTreeInit_private()
{
    file_in = TFile::Open(Form("%s/%s.root", input_directory.c_str(), MC_list_name.c_str()));
    tree = (TTree *)file_in->Get(tree_name.c_str());
    inttCluData = new PrivateCluReader(tree);
    N_event = tree -> GetEntries();
    // std::printf("private gen tree, N event : %lli \n", N_event); // note : was used

}

void INTTReadTree::TTreeInit_private_geo1()
{
    file_in = TFile::Open(Form("%s/%s.root", input_directory.c_str(), MC_list_name.c_str()));
    // file_in = new TFile(Form("%s/%s.root", input_directory.c_str(), MC_list_name.c_str()),"read");
    tree = (TTree *)file_in->Get(tree_name.c_str());
    inttCluData = new PrivateCluReader(tree);
    N_event = tree -> GetEntries();
    // std::printf("private gen tree, N event : %lli \n", N_event); // note : was used
    inttConv = new InttConversion();

    if (N_ladder != included_ladder_vec.size()) {cout<<"In INTTReadTree, there is an error in ladder offset generation"<<endl; exit(1);}    
    gen_ladder_offset();
}

void INTTReadTree::EvtInit(long long event_i)
{
    if (data_type_list[data_type] == "MC" || data_type_list[data_type] == "MC_geo_test" || data_type_list[data_type] == "MC_inner_phi_rotation")
    {
        inttDSTMC->LoadTree(event_i);
        inttDSTMC->GetEntry(event_i);

        evt_length           = inttDSTMC->NClus;
        bco_full             = -1;
        is_min_bias          = inttDSTMC->is_min_bias;
        is_min_bias_wozdc    = inttDSTMC->is_min_bias_wozdc; 
        MBD_south_charge_sum = inttDSTMC->MBD_south_charge_sum;
        MBD_north_charge_sum = inttDSTMC->MBD_north_charge_sum;
        MBD_reco_z           = inttDSTMC->MBD_z_vtx * 10.; // note : from cm to mm
        NvtxMC               = inttDSTMC->NTruthVtx;
        TrigXvtxMC           = inttDSTMC->TruthPV_trig_x;
        TrigYvtxMC           = inttDSTMC->TruthPV_trig_y;
        TrigZvtxMC           = inttDSTMC->TruthPV_trig_z;
        Centrality_bimp      = inttDSTMC->MBD_centrality; // note : same thing for both
        Centrality_mbd       = inttDSTMC->MBD_centrality; // note : same thing for both
        for (int track_i = 0; track_i < inttDSTMC->PrimaryG4P_Eta->size(); track_i++) {true_track_info.push_back({inttDSTMC->PrimaryG4P_Eta->at(track_i),inttDSTMC->PrimaryG4P_Phi->at(track_i),float(inttDSTMC->PrimaryG4P_PID->at(track_i))});}
    }
    else if (data_type_list[data_type] == "data_private" || data_type_list[data_type] == "data_private_geo1")
    {
        inttCluData->LoadTree(event_i);
        inttCluData->GetEntry(event_i);

        evt_length           = inttCluData->x->size();
        bco_full             = inttCluData->bco_full;
        is_min_bias          = 1;
        is_min_bias_wozdc    = 1;
        MBD_south_charge_sum = -1000;
        MBD_north_charge_sum = -1000;
        MBD_reco_z           = -10000;
        NvtxMC               = -1;
        TrigXvtxMC           = -1000.;
        TrigYvtxMC           = -1000.;
        TrigZvtxMC           = -1000.;
    }
    else if (data_type_list[data_type] == "MC_selfgen")
    {
        inttMCselfgen->LoadTree(event_i);
        inttMCselfgen->GetEntry(event_i);

        evt_length           = inttMCselfgen->NClus;
        bco_full             = -1;
        NvtxMC               = 1;
        is_min_bias          = 1;
        is_min_bias_wozdc    = 1;
        MBD_south_charge_sum = -1000;
        MBD_north_charge_sum = -1000;
        MBD_reco_z           = -10000;
        TrigXvtxMC           = inttMCselfgen->TruthPV_x;
        TrigYvtxMC           = inttMCselfgen->TruthPV_y;
        TrigZvtxMC           = inttMCselfgen->TruthPV_z;
        Centrality_bimp = get_centrality_bin(inttMCselfgen->NClus);
        Centrality_mbd = get_centrality_bin(inttMCselfgen->NClus);

        // cout<<"inttMCselfgen->NClus : "<<inttMCselfgen->NClus<<" Centrality_bimp : "<<Centrality_bimp<<endl;
        
        for (int track_i = 0; track_i < inttMCselfgen->PrimaryG4P_Eta->size(); track_i++) {
            true_track_info.push_back({
                inttMCselfgen->PrimaryG4P_Eta->at(track_i),
                inttMCselfgen->PrimaryG4P_Phi->at(track_i),
                float(inttMCselfgen->PrimaryG4P_PID->at(track_i))
            });
        }
    }
    else if (data_type_list[data_type] == "data_F4A" || data_type_list[data_type] == "data_F4A_inner_phi_rotation") 
    {
        inttDSTData->LoadTree(event_i);
        inttDSTData->GetEntry(event_i);

        evt_length           = inttDSTData->NClus;
        bco_full             = inttDSTData->INTT_BCO;
        NvtxMC               = 1;
        is_min_bias          = inttDSTData->is_min_bias;
        is_min_bias_wozdc    = inttDSTData->is_min_bias_wozdc;
        MBD_south_charge_sum = inttDSTData->MBD_south_charge_sum;
        MBD_north_charge_sum = inttDSTData->MBD_north_charge_sum;
        MBD_reco_z           = inttDSTData->MBD_z_vtx * 10.; // note : cm to mm
        TrigXvtxMC           = -1000;
        TrigYvtxMC           = -1000;
        TrigZvtxMC           = -1000;
        Centrality_bimp      = inttDSTData->MBD_centrality;
        Centrality_mbd       = inttDSTData->MBD_centrality;
    }
}

long long INTTReadTree::GetNEvt() { return N_event; }
unsigned long INTTReadTree::GetEvtNClus() { return evt_length; }
double INTTReadTree::GetTrigZvtxMC() {return TrigZvtxMC;}
vector<double> INTTReadTree::GetTrigvtxMC() {return {TrigXvtxMC,TrigYvtxMC,TrigZvtxMC};}
int INTTReadTree::GetNvtxMC() {return NvtxMC;}
string INTTReadTree::GetRunType() { return run_type_out; }
Long64_t INTTReadTree::GetBCOFull() {return bco_full;}
map<string, vector<double>> INTTReadTree::GetLadderOffsetMap() {return ladder_offset_map;}
float INTTReadTree::GetCentralityBin() {return Centrality_bimp;}
vector<vector<float>> INTTReadTree::GetTrueTrackInfo() {return true_track_info;}
double INTTReadTree::GetMBDRecoZ() {return MBD_reco_z;}
int INTTReadTree::GetIsMinBias() {return is_min_bias;}
int INTTReadTree::GetIsMinBiasWoZDC() {return is_min_bias_wozdc;}
double INTTReadTree::GetMBDNorthChargeSum() {return MBD_north_charge_sum;}
double INTTReadTree::GetMBDSouthChargeSum() {return MBD_south_charge_sum;}

unsigned long INTTReadTree::GetEvtNClusPost() 
{ 
    return temp_sPH_inner_nocolumn_vec.size() + temp_sPH_outer_nocolumn_vec.size(); 
}

vector<double> INTTReadTree::offset_correction(map<string,vector<double>> input_map)
{
    double N_pair = 0;
    double sum_x = 0;
    double sum_y = 0;
    double sum_z = 0;

    for (const auto& pair : input_map)
    {
        N_pair += 1;
        sum_x += pair.second[0];
        sum_y += pair.second[1];
        sum_z += pair.second[2];
    }

    return {sum_x/N_pair, sum_y/N_pair, sum_z/N_pair};
}

void INTTReadTree::EvtSetCluGroup()
{
    if (data_type_list[data_type] == "MC" || data_type_list[data_type] == "MC_inner_phi_rotation"){
        for (int clu_i = 0; clu_i < evt_length; clu_i++)
        {
            if (int(inttDSTMC -> ClusPhiSize -> at(clu_i)) > clu_size_cut) continue; 
            if (int(inttDSTMC -> ClusAdc -> at(clu_i)) <= clu_sum_adc_cut) continue;

            // double clu_x = inttDSTMC -> ClusX -> at(clu_i) * 10; // note : change the unit from cm to mm
            // double clu_y = inttDSTMC -> ClusY -> at(clu_i) * 10;

            int    clu_layer = (inttDSTMC -> ClusLayer -> at(clu_i) == 3 || inttDSTMC -> ClusLayer -> at(clu_i) == 4) ? 0 : 1;
            // note : this is for rotating the clusters in the inner barrel by 180 degrees.
            double clu_x = (data_type_list[data_type] == "MC_inner_phi_rotation" && clu_layer == 0)? rotatePoint(inttDSTMC -> ClusX -> at(clu_i) * 10, inttDSTMC -> ClusY -> at(clu_i) * 10).first  : inttDSTMC -> ClusX -> at(clu_i) * 10; // note : change the unit from cm to mm
            double clu_y = (data_type_list[data_type] == "MC_inner_phi_rotation" && clu_layer == 0)? rotatePoint(inttDSTMC -> ClusX -> at(clu_i) * 10, inttDSTMC -> ClusY -> at(clu_i) * 10).second : inttDSTMC -> ClusY -> at(clu_i) * 10;
            double clu_z = inttDSTMC -> ClusZ -> at(clu_i) * 10;
            double clu_phi = (clu_y < 0) ? atan2(clu_y,clu_x) * (180./TMath::Pi()) + 360 : atan2(clu_y,clu_x) * (180./TMath::Pi());
            double clu_radius = get_radius(clu_x, clu_y);

            temp_sPH_nocolumn_vec[0].push_back( clu_x );
            temp_sPH_nocolumn_vec[1].push_back( clu_y );
            
            temp_sPH_nocolumn_rz_vec[0].push_back( clu_z );
            temp_sPH_nocolumn_rz_vec[1].push_back( ( clu_phi > 180 ) ? clu_radius * -1 : clu_radius );
            

            if (clu_layer == 0) {// note : inner
                temp_sPH_inner_nocolumn_vec.push_back({
                    -1, 
                    -1, 
                    int(inttDSTMC -> ClusAdc -> at(clu_i)), 
                    int(inttDSTMC -> ClusAdc -> at(clu_i)), 
                    int(inttDSTMC -> ClusPhiSize -> at(clu_i)), 
                    clu_x, 
                    clu_y, 
                    clu_z, 
                    inttDSTMC -> ClusLayer -> at(clu_i), // note : should be 3 or 4, for the inner layer, this is for the mega cluster search
                    clu_phi,
                    int(inttDSTMC -> ClusLayer       -> at(clu_i)), // note : raw_layer_id
                    int(inttDSTMC -> ClusLadderPhiId -> at(clu_i)), // note : raw_ladder_id
                    int(inttDSTMC -> ClusLadderZId   -> at(clu_i)), // note : raw_Z_id
                    int(inttDSTMC -> ClusPhiSize     -> at(clu_i)), // note : raw_phi_size
                    int(inttDSTMC -> ClusZSize       -> at(clu_i))  // note : raw_z_size
                });
            }
            
            if (clu_layer == 1) {// note : outer
                temp_sPH_outer_nocolumn_vec.push_back({
                    -1, 
                    -1, 
                    int(inttDSTMC -> ClusAdc -> at(clu_i)), 
                    int(inttDSTMC -> ClusAdc -> at(clu_i)), 
                    int(inttDSTMC -> ClusPhiSize -> at(clu_i)), 
                    clu_x, 
                    clu_y, 
                    clu_z, 
                    inttDSTMC -> ClusLayer -> at(clu_i), // note : should be 5 or 6, for the outer layer, this is for the mega cluster search
                    clu_phi,
                    int(inttDSTMC -> ClusLayer       -> at(clu_i)), // note : raw_layer_id
                    int(inttDSTMC -> ClusLadderPhiId -> at(clu_i)), // note : raw_ladder_id
                    int(inttDSTMC -> ClusLadderZId   -> at(clu_i)), // note : raw_Z_id
                    int(inttDSTMC -> ClusPhiSize     -> at(clu_i)), // note : raw_phi_size
                    int(inttDSTMC -> ClusZSize       -> at(clu_i))  // note : raw_z_size
                });            
            }        
        }
    }

    else if (data_type_list[data_type] == "data_private"){
        for (int clu_i = 0; clu_i < evt_length; clu_i++)
        {
            if (int(inttCluData -> size -> at(clu_i)) > clu_size_cut) continue; 
            if (int(inttCluData -> sum_adc_conv -> at(clu_i)) <= clu_sum_adc_cut) continue;

            double clu_x = inttCluData -> x -> at(clu_i);
            double clu_y = inttCluData -> y -> at(clu_i);
            double clu_z = inttCluData -> z -> at(clu_i);
            double clu_phi = (clu_y < 0) ? atan2(clu_y,clu_x) * (180./TMath::Pi()) + 360 : atan2(clu_y,clu_x) * (180./TMath::Pi());
            int    clu_layer = inttCluData -> layer -> at(clu_i); // note : should be 0 or 1
            double clu_radius = get_radius(clu_x, clu_y);

            temp_sPH_nocolumn_vec[0].push_back( clu_x );
            temp_sPH_nocolumn_vec[1].push_back( clu_y );
            
            temp_sPH_nocolumn_rz_vec[0].push_back( clu_z );
            temp_sPH_nocolumn_rz_vec[1].push_back( ( clu_phi > 180 ) ? clu_radius * -1 : clu_radius );
            

            if (clu_layer == 0) {// note : inner
                temp_sPH_inner_nocolumn_vec.push_back({
                    -1, 
                    -1, 
                    int(inttCluData -> sum_adc_conv -> at(clu_i)), 
                    int(inttCluData -> sum_adc_conv -> at(clu_i)), 
                    int(inttCluData -> size -> at(clu_i)), 
                    clu_x, 
                    clu_y, 
                    clu_z, 
                    clu_layer, 
                    clu_phi
                });
            }
            
            if (clu_layer == 1) {// note : outer
                temp_sPH_outer_nocolumn_vec.push_back({
                    -1, 
                    -1, 
                    int(inttCluData -> sum_adc_conv -> at(clu_i)), 
                    int(inttCluData -> sum_adc_conv -> at(clu_i)), 
                    int(inttCluData -> size -> at(clu_i)), 
                    clu_x, 
                    clu_y, 
                    clu_z, 
                    clu_layer, 
                    clu_phi
                });            
            }        
        }
    }

    // else if (data_type_list[data_type] == "data_private_geo1"){
    //     for (int clu_i = 0; clu_i < evt_length; clu_i++)
    //     {
    //         if (int(inttCluData -> size -> at(clu_i)) > clu_size_cut) {continue;} 
    //         if (int(inttCluData -> sum_adc_conv -> at(clu_i)) <= clu_sum_adc_cut) {continue;}

    //         string ladder_name = inttConv -> GetLadderName(Form("intt%i_%i", inttCluData -> server -> at(clu_i) - 3001, inttCluData -> module -> at(clu_i)));

    //         // note : the following selections only keeps a small portion of INTT clusters
    //         // if (inttCluData -> server -> at(clu_i) - 3001 > 3) {continue;} // note : only the 3001, 3002, 3003, 3004 are included in the study (south)
    //         if ((ladder_name).substr(0,4) == "B0L1") {continue;}
    //         if ((ladder_name).substr(0,4) == "B1L1") {continue;}
    //         // if (inttCluData -> column -> at(clu_i) > 5) {continue;} // note : only the column 1, 2, 3, 4, 5 are included in the study (south type B sensor only)

    //         if ( (ladder_name).substr(0,6) == "B0L001") {continue;}
    //         if ( (ladder_name).substr(0,6) == "B0L004") {continue;}
    //         if ( (ladder_name).substr(0,6) == "B0L007") {continue;}
    //         if ( (ladder_name).substr(0,6) == "B0L010") {continue;}

    //         if ( (ladder_name).substr(0,6) == "B1L001") {continue;}
    //         if ( (ladder_name).substr(0,6) == "B1L002") {continue;}
    //         if ( (ladder_name).substr(0,6) == "B1L005") {continue;}
    //         if ( (ladder_name).substr(0,6) == "B1L006") {continue;}
    //         if ( (ladder_name).substr(0,6) == "B1L009") {continue;}
    //         if ( (ladder_name).substr(0,6) == "B1L010") {continue;}
    //         if ( (ladder_name).substr(0,6) == "B1L013") {continue;}
    //         if ( (ladder_name).substr(0,6) == "B1L014") {continue;}

    //         // note : because the corresponding hald-ladder of this one, the B1L015S, has no data
    //         if ( (ladder_name).substr(0,6) == "B0L011") {continue;}
    //         if ( (ladder_name).substr(0,6) == "B1L015") {continue;}

    //         // note : for the hypothesis test
    //         // if ( (ladder_name).substr(0,6) == "B1L004") {continue;}
    //         // if ( (ladder_name).substr(0,6) == "B0L003") {continue;} // note : pair
    //         // if ( (ladder_name).substr(0,6) == "B1L007") {continue;}
    //         // if ( (ladder_name).substr(0,6) == "B0L005") {continue;} // note : pair
    //         // if ( (ladder_name).substr(0,6) == "B1L011") {continue;}
    //         // if ( (ladder_name).substr(0,6) == "B0L008") {continue;} // note : pair
    //         // if ( (ladder_name).substr(0,6) == "B1L012") {continue;}
    //         // if ( (ladder_name).substr(0,6) == "B0L009") {continue;} // note : pair
    //         // if ( (ladder_name).substr(0,6) == "B0L000") {continue;}
    //         // if ( (ladder_name).substr(0,6) == "B1L000") {continue;} // note : pair

    //         double clu_x_offset = ladder_offset_map[(ladder_name).substr(0,6)].first;
    //         double clu_y_offset = ladder_offset_map[(ladder_name).substr(0,6)].second;

    //         double clu_x = inttCluData -> x -> at(clu_i) + clu_x_offset;
    //         double clu_y = inttCluData -> y -> at(clu_i) + clu_y_offset;
    //         double clu_z = inttCluData -> z -> at(clu_i);
    //         double clu_phi = (clu_y < 0) ? atan2(clu_y,clu_x) * (180./TMath::Pi()) + 360 : atan2(clu_y,clu_x) * (180./TMath::Pi());
    //         int    clu_layer = inttCluData -> layer -> at(clu_i); // note : should be 0 or 1
    //         double clu_radius = get_radius(clu_x, clu_y);

    //         temp_sPH_nocolumn_vec[0].push_back( clu_x );
    //         temp_sPH_nocolumn_vec[1].push_back( clu_y );
            
    //         temp_sPH_nocolumn_rz_vec[0].push_back( clu_z );
    //         temp_sPH_nocolumn_rz_vec[1].push_back( ( clu_phi > 180 ) ? clu_radius * -1 : clu_radius );
            

    //         if (clu_layer == 0) {// note : inner
    //             temp_sPH_inner_nocolumn_vec.push_back({
    //                 -1, 
    //                 -1, 
    //                 int(inttCluData -> sum_adc_conv -> at(clu_i)), 
    //                 int(inttCluData -> sum_adc_conv -> at(clu_i)), 
    //                 int(inttCluData -> size -> at(clu_i)), 
    //                 clu_x, 
    //                 clu_y, 
    //                 clu_z, 
    //                 clu_layer, 
    //                 clu_phi
    //             });
    //         }
            
    //         if (clu_layer == 1) {// note : outer
    //             temp_sPH_outer_nocolumn_vec.push_back({
    //                 -1, 
    //                 -1, 
    //                 int(inttCluData -> sum_adc_conv -> at(clu_i)), 
    //                 int(inttCluData -> sum_adc_conv -> at(clu_i)), 
    //                 int(inttCluData -> size -> at(clu_i)), 
    //                 clu_x, 
    //                 clu_y, 
    //                 clu_z, 
    //                 clu_layer, 
    //                 clu_phi
    //             });            
    //         }        
    //     }
    // }

    else if (data_type_list[data_type] == "MC_geo_test"){
        for (int clu_i = 0; clu_i < evt_length; clu_i++)
        {
            // if (inttDSTMC -> ClusLadderZId -> at(clu_i) != 0) continue; // note : only the south-side sensor B is included in the study
            if (int(inttDSTMC -> ClusPhiSize -> at(clu_i)) > clu_size_cut) continue; 
            if (int(inttDSTMC -> ClusAdc -> at(clu_i)) <= clu_sum_adc_cut) continue;

            double clu_x_offset = ladder_offset_map[Form("%i_%i",inttDSTMC -> ClusLayer -> at(clu_i), inttDSTMC -> ClusLadderPhiId -> at(clu_i))][0];
            double clu_y_offset = ladder_offset_map[Form("%i_%i",inttDSTMC -> ClusLayer -> at(clu_i), inttDSTMC -> ClusLadderPhiId -> at(clu_i))][1];
            double clu_z_offset = ladder_offset_map[Form("%i_%i",inttDSTMC -> ClusLayer -> at(clu_i), inttDSTMC -> ClusLadderPhiId -> at(clu_i))][2];

            double clu_x = inttDSTMC -> ClusX -> at(clu_i) * 10 + clu_x_offset; // note : change the unit from cm to mm
            double clu_y = inttDSTMC -> ClusY -> at(clu_i) * 10 + clu_y_offset;
            double clu_z = inttDSTMC -> ClusZ -> at(clu_i) * 10 + clu_z_offset;
            double clu_phi = (clu_y < 0) ? atan2(clu_y,clu_x) * (180./TMath::Pi()) + 360 : atan2(clu_y,clu_x) * (180./TMath::Pi());
            int    clu_layer = (inttDSTMC -> ClusLayer -> at(clu_i) == 3 || inttDSTMC -> ClusLayer -> at(clu_i) == 4) ? 0 : 1;
            double clu_radius = get_radius(clu_x, clu_y);

            temp_sPH_nocolumn_vec[0].push_back( clu_x );
            temp_sPH_nocolumn_vec[1].push_back( clu_y );
            
            temp_sPH_nocolumn_rz_vec[0].push_back( clu_z );
            temp_sPH_nocolumn_rz_vec[1].push_back( ( clu_phi > 180 ) ? clu_radius * -1 : clu_radius );
            

            if (clu_layer == 0) {// note : inner
                temp_sPH_inner_nocolumn_vec.push_back({
                    -1, 
                    -1, 
                    int(inttDSTMC -> ClusAdc -> at(clu_i)), 
                    int(inttDSTMC -> ClusAdc -> at(clu_i)), 
                    int(inttDSTMC -> ClusPhiSize -> at(clu_i)), 
                    clu_x, 
                    clu_y, 
                    clu_z, 
                    clu_layer, 
                    clu_phi
                });
            }
            
            if (clu_layer == 1) {// note : outer
                temp_sPH_outer_nocolumn_vec.push_back({
                    -1, 
                    -1, 
                    int(inttDSTMC -> ClusAdc -> at(clu_i)), 
                    int(inttDSTMC -> ClusAdc -> at(clu_i)), 
                    int(inttDSTMC -> ClusPhiSize -> at(clu_i)), 
                    clu_x, 
                    clu_y, 
                    clu_z, 
                    clu_layer, 
                    clu_phi
                });            
            }        
        }
    }

    else if (data_type_list[data_type] == "MC_selfgen"){
        for (int clu_i = 0; clu_i < evt_length; clu_i++)
        {
            if (int(inttMCselfgen -> phi_size -> at(clu_i)) > clu_size_cut) continue; 
            if (int(inttMCselfgen -> adc -> at(clu_i)) <= clu_sum_adc_cut) continue;

            // double clu_x = inttMCselfgen -> X -> at(clu_i) * 10; // note : change the unit from cm to mm
            // double clu_y = inttMCselfgen -> Y -> at(clu_i) * 10;

            int    clu_layer = (inttMCselfgen -> layer -> at(clu_i) == 3 || inttMCselfgen -> layer -> at(clu_i) == 4) ? 0 : 1;
            // note : this is for rotating the clusters in the inner barrel by 180 degrees.
            double clu_x = inttMCselfgen -> X -> at(clu_i) * 10; // note : change the unit from cm to mm
            double clu_y = inttMCselfgen -> Y -> at(clu_i) * 10;
            double clu_z = inttMCselfgen -> Z -> at(clu_i) * 10;
            double clu_phi = (clu_y < 0) ? atan2(clu_y,clu_x) * (180./TMath::Pi()) + 360 : atan2(clu_y,clu_x) * (180./TMath::Pi());
            double clu_radius = get_radius(clu_x, clu_y);

            temp_sPH_nocolumn_vec[0].push_back( clu_x );
            temp_sPH_nocolumn_vec[1].push_back( clu_y );
            
            temp_sPH_nocolumn_rz_vec[0].push_back( clu_z );
            temp_sPH_nocolumn_rz_vec[1].push_back( ( clu_phi > 180 ) ? clu_radius * -1 : clu_radius );
            

            if (clu_layer == 0) {// note : inner
                temp_sPH_inner_nocolumn_vec.push_back({
                    -1, 
                    -1, 
                    int(inttMCselfgen -> adc -> at(clu_i)), 
                    int(inttMCselfgen -> adc -> at(clu_i)), 
                    int(inttMCselfgen -> phi_size -> at(clu_i)), 
                    clu_x, 
                    clu_y, 
                    clu_z, 
                    inttMCselfgen -> layer -> at(clu_i), // note : should be 3 or 4, for the inner layer, this is for the mega cluster search
                    clu_phi
                });
            }
            
            if (clu_layer == 1) {// note : outer
                temp_sPH_outer_nocolumn_vec.push_back({
                    -1, 
                    -1, 
                    int(inttMCselfgen -> adc -> at(clu_i)), 
                    int(inttMCselfgen -> adc -> at(clu_i)), 
                    int(inttMCselfgen -> phi_size -> at(clu_i)), 
                    clu_x, 
                    clu_y, 
                    clu_z, 
                    inttMCselfgen -> layer -> at(clu_i), // note : should be 5 or 6, for the outer layer, this is for the mega cluster search
                    clu_phi
                });            
            }        
        }
    }

    else if (data_type_list[data_type] == "data_F4A" || data_type_list[data_type] == "data_F4A_inner_phi_rotation"){
        for (int clu_i = 0; clu_i < evt_length; clu_i++)
        {
            if (int(inttDSTData -> ClusPhiSize -> at(clu_i)) > clu_size_cut) continue; 
            if (int(inttDSTData -> ClusAdc -> at(clu_i)) <= clu_sum_adc_cut) continue;

            // double clu_x = inttDSTData -> ClusX -> at(clu_i) * 10; // note : change the unit from cm to mm
            // double clu_y = inttDSTData -> ClusY -> at(clu_i) * 10;

            int    clu_layer = (inttDSTData -> ClusLayer -> at(clu_i) == 3 || inttDSTData -> ClusLayer -> at(clu_i) == 4) ? 0 : 1;
            // note : this is for rotating the clusters in the inner barrel by 180 degrees.
            double clu_x = (data_type_list[data_type] == "data_F4A_inner_phi_rotation" && clu_layer == 0)? rotatePoint(inttDSTData -> ClusX -> at(clu_i) * 10, inttDSTData -> ClusY -> at(clu_i) * 10).first  : inttDSTData -> ClusX -> at(clu_i) * 10; // note : change the unit from cm to mm
            double clu_y = (data_type_list[data_type] == "data_F4A_inner_phi_rotation" && clu_layer == 0)? rotatePoint(inttDSTData -> ClusX -> at(clu_i) * 10, inttDSTData -> ClusY -> at(clu_i) * 10).second : inttDSTData -> ClusY -> at(clu_i) * 10;
            double clu_z = inttDSTData -> ClusZ -> at(clu_i) * 10;
            double clu_phi = (clu_y < 0) ? atan2(clu_y,clu_x) * (180./TMath::Pi()) + 360 : atan2(clu_y,clu_x) * (180./TMath::Pi());
            double clu_radius = get_radius(clu_x, clu_y);

            temp_sPH_nocolumn_vec[0].push_back( clu_x );
            temp_sPH_nocolumn_vec[1].push_back( clu_y );
            
            temp_sPH_nocolumn_rz_vec[0].push_back( clu_z );
            temp_sPH_nocolumn_rz_vec[1].push_back( ( clu_phi > 180 ) ? clu_radius * -1 : clu_radius );
            

            if (clu_layer == 0) {// note : inner
                temp_sPH_inner_nocolumn_vec.push_back({
                    -1, 
                    -1, 
                    int(inttDSTData -> ClusAdc -> at(clu_i)), 
                    int(inttDSTData -> ClusAdc -> at(clu_i)), 
                    int(inttDSTData -> ClusPhiSize -> at(clu_i)), 
                    clu_x, 
                    clu_y, 
                    clu_z, 
                    inttDSTData -> ClusLayer -> at(clu_i), // note : should be 3 or 4, for the inner layer, this is for the mega cluster search
                    clu_phi,
                    int(inttDSTData -> ClusLayer       -> at(clu_i)), // note : raw_layer_id
                    int(inttDSTData -> ClusLadderPhiId -> at(clu_i)), // note : raw_ladder_id
                    int(inttDSTData -> ClusLadderZId   -> at(clu_i)), // note : raw_Z_id
                    int(inttDSTData -> ClusPhiSize     -> at(clu_i)), // note : raw_phi_size
                    int(inttDSTData -> ClusZSize       -> at(clu_i))  // note : raw_z_size
                });
            }
            
            if (clu_layer == 1) {// note : outer
                temp_sPH_outer_nocolumn_vec.push_back({
                    -1, 
                    -1, 
                    int(inttDSTData -> ClusAdc -> at(clu_i)), 
                    int(inttDSTData -> ClusAdc -> at(clu_i)), 
                    int(inttDSTData -> ClusPhiSize -> at(clu_i)), 
                    clu_x, 
                    clu_y, 
                    clu_z, 
                    inttDSTData -> ClusLayer -> at(clu_i), // note : should be 5 or 6, for the outer layer, this is for the mega cluster search
                    clu_phi,
                    int(inttDSTData -> ClusLayer       -> at(clu_i)), // note : raw_layer_id
                    int(inttDSTData -> ClusLadderPhiId -> at(clu_i)), // note : raw_ladder_id
                    int(inttDSTData -> ClusLadderZId   -> at(clu_i)), // note : raw_Z_id
                    int(inttDSTData -> ClusPhiSize     -> at(clu_i)), // note : raw_phi_size
                    int(inttDSTData -> ClusZSize       -> at(clu_i))  // note : raw_z_size
                });            
            }        
        }
    }
    
}

void INTTReadTree::gen_ladder_offset()
{
    TRandom * rand = new TRandom3();
    rand -> SetSeed(random_seed);

    if (apply_geo_offset == 1)
    {
        for (int ladder_i = 0; ladder_i < N_ladder; ladder_i++)
        {
            ladder_offset_map[included_ladder_vec[ladder_i].c_str()] = {rand -> Uniform(-offset_range, offset_range), rand -> Uniform(-offset_range, offset_range)};    
        }
    }
    else if (apply_geo_offset == 0) // note : no geometry offset applied
    {
        for (int ladder_i = 0; ladder_i < N_ladder; ladder_i++)
        {
            ladder_offset_map[included_ladder_vec[ladder_i].c_str()] = {0, 0};    
        }
    }
    else if (apply_geo_offset == 2) // note : the geometry offset is given from outside
    {
        for (int ladder_i = 0; ladder_i < N_ladder; ladder_i++)
        {
            ladder_offset_map[included_ladder_vec[ladder_i].c_str()] = ladder_offset_vec[ladder_i];    
        }
    }
    else if (apply_geo_offset == 3) // note : the geometry offset for the MC
    {
        for (int layer_i = 3; layer_i < 7; layer_i++)
        {
            double N_layer_ladder = (layer_i == 3 || layer_i == 4) ? 12 : 16;

            for (int phi_i = 0; phi_i < N_layer_ladder; phi_i++)
            {
                ladder_offset_map[Form("%i_%i", layer_i, phi_i)] = {rand -> Uniform(-offset_range, offset_range), rand -> Uniform(-offset_range, offset_range), rand -> Uniform(-offset_range, offset_range)};
                // ladder_offset_map[Form("%i_%i", layer_i, phi_i)] = {100., 100.};
            }
        }

        // note : it's possible that the whole system has a systematic offset, so we need to correct it
        vector<double> XYZ_correction = offset_correction(ladder_offset_map);
        for (const auto& pair : ladder_offset_map)
        {
            ladder_offset_map[pair.first] = {pair.second[0] - XYZ_correction[0], pair.second[1] - XYZ_correction[1], pair.second[2] - XYZ_correction[2]};
        }
        
    }
    


    rand -> Delete();
}

double INTTReadTree::get_radius(double x, double y)
{
    return sqrt(pow(x,2)+pow(y,2));
}

bool INTTReadTree::GetPhiCheckTag()
{
    int inner_1_check = 0;
    int inner_2_check = 0;
    int inner_3_check = 0;
    int inner_4_check = 0;
    for (int inner_i = 0; inner_i < temp_sPH_inner_nocolumn_vec.size(); inner_i++) {
        if (temp_sPH_inner_nocolumn_vec[inner_i].phi >= 0 && temp_sPH_inner_nocolumn_vec[inner_i].phi < 90)    inner_1_check = 1;
        if (temp_sPH_inner_nocolumn_vec[inner_i].phi >= 90 && temp_sPH_inner_nocolumn_vec[inner_i].phi < 180)  inner_2_check = 1;
        if (temp_sPH_inner_nocolumn_vec[inner_i].phi >= 180 && temp_sPH_inner_nocolumn_vec[inner_i].phi < 270) inner_3_check = 1;
        if (temp_sPH_inner_nocolumn_vec[inner_i].phi >= 270 && temp_sPH_inner_nocolumn_vec[inner_i].phi < 360) inner_4_check = 1;

        if ( (inner_1_check + inner_2_check + inner_3_check + inner_4_check) == 4 ) break;
    }

    int outer_1_check = 0;
    int outer_2_check = 0;
    int outer_3_check = 0;
    int outer_4_check = 0;
    for (int outer_i = 0; outer_i < temp_sPH_outer_nocolumn_vec.size(); outer_i++) {
        if (temp_sPH_outer_nocolumn_vec[outer_i].phi >= 0 && temp_sPH_outer_nocolumn_vec[outer_i].phi < 90)    outer_1_check = 1;
        if (temp_sPH_outer_nocolumn_vec[outer_i].phi >= 90 && temp_sPH_outer_nocolumn_vec[outer_i].phi < 180)  outer_2_check = 1;
        if (temp_sPH_outer_nocolumn_vec[outer_i].phi >= 180 && temp_sPH_outer_nocolumn_vec[outer_i].phi < 270) outer_3_check = 1;
        if (temp_sPH_outer_nocolumn_vec[outer_i].phi >= 270 && temp_sPH_outer_nocolumn_vec[outer_i].phi < 360) outer_4_check = 1;

        if ( (outer_1_check + outer_2_check + outer_3_check + outer_4_check) == 4 ) break;
    }

    if ( (inner_1_check + inner_2_check + inner_3_check + inner_4_check + outer_1_check + outer_2_check + outer_3_check + outer_4_check) != 8 ) {return false;}
    else { return true; }
}

void INTTReadTree::EvtClear()
{
    temp_sPH_inner_nocolumn_vec.clear();
    temp_sPH_outer_nocolumn_vec.clear();

    temp_sPH_nocolumn_vec.clear();
    temp_sPH_nocolumn_vec = vector<vector<double>> (2);
    
    temp_sPH_nocolumn_rz_vec.clear();
    temp_sPH_nocolumn_rz_vec = vector<vector<double>> (2);   

    true_track_info.clear();

    evt_length = 0;
    NvtxMC     = 0;
}

void INTTReadTree::EndRun()
{
    ladder_offset_map.clear();
    if (file_in != nullptr){
        cout<<"closing the file_in"<<endl;
        file_in -> Close();
        delete file_in;
    }


}

// note : rotate the ClusX and ClusY
pair<double,double> INTTReadTree::rotatePoint(double x, double y)
{
    // Convert the rotation angle from degrees to radians
    double rotation = 180.; // todo rotation is here
    double angleRad = rotation * M_PI / 180.0;

    // Perform the rotation
    double xOut = x * cos(angleRad) - y * sin(angleRad);
    double yOut = x * sin(angleRad) + y * cos(angleRad);

    xOut = (fabs(xOut) < 0.00000001) ? 0 : xOut;
    yOut = (fabs(yOut) < 0.00000001) ? 0 : yOut;

    // cout<<"Post rotation: "<<xOut<<" "<<yOut<<endl;

    return {xOut,yOut};
}

#endif