#ifndef INTTReadTree_h
#define INTTReadTree_h

#include "../INTTDSTchain.C"
#include "PrivateCluReader.C"
#include "../private_cluster_gen/InttConversion_new.h"
#include "../private_cluster_gen/InttClustering.h"

class INTTReadTree
{
    public : 
        vector<clu_info> temp_sPH_inner_nocolumn_vec; 
        vector<clu_info> temp_sPH_outer_nocolumn_vec; 
        vector<vector<double>> temp_sPH_nocolumn_vec;
        vector<vector<double>> temp_sPH_nocolumn_rz_vec;
        
        INTTReadTree(int data_type, string input_directory, string MC_list_name, string tree_name, int clu_size_cut, int clu_sum_adc_cut, int random_seed = 65539, int N_ladder = 14, double offset_range = 0.2, vector<string> included_ladder_vec = {}, int apply_geo_offset = 1, vector<pair<double,double>> ladder_offset_vec = {});
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
        void EvtClear();
        map<string, pair<double,double>> GetLadderOffsetMap();
        void EndRun();
        void gen_ladder_offset();
        void clear_ladder_offset_map() {ladder_offset_map.clear();}
        void set_random_seed(int random_seed_in) {random_seed = random_seed_in;}

    private : 
        string data_type_list[6] = {"MC","data_DST","data_private", "data_private_geo1", "MC_geo_test", "MC_inner_phi_rotation"};
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
        double TrigXvtxMC;
        double TrigYvtxMC;
        double TrigZvtxMC;
        float  Centrality_bimp;
        float  Centrality_mbd;
        Long64_t bco_full;

        vector<vector<float>>  true_track_info;
        vector<string> included_ladder_vec;
        vector<pair<double,double>> ladder_offset_vec;
        map<string, pair<double,double>> ladder_offset_map;

        TChain * chain_in;
        PrivateCluReader * inttCluData; // note : the class to read the private gen cluster file
        INTTDSTchain * inttDSTMC; // note : the class to read the MC sample with TChain
        InttConversion * inttConv; // note : the class to conver the server_module into ladder name
        TFile * file_in = nullptr;
        TTree * tree;

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
        double get_radius(double x, double y);
        pair<double,double> rotatePoint(double x, double y);
        pair<double, double> offset_correction(map<string,pair<double,double>> input_map);

};

INTTReadTree::INTTReadTree(int data_type, string input_directory, string MC_list_name, string tree_name, int clu_size_cut, int clu_sum_adc_cut, int random_seed, int N_ladder, double offset_range, vector<string> included_ladder_vec, int apply_geo_offset, vector<pair<double,double>> ladder_offset_vec)
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
        std::cout<<"--- INTTReadTree -> input data type: MC geometry test ---"<<std::endl;
        TChainInit_MC_geo_test();
        std::cout<<"--- INTTReadTree -> Initialization done ---"<<std::endl;
    }

}

void INTTReadTree::TChainInit_MC()
{
    chain_in = new TChain(tree_name.c_str());
    inttDSTMC = new INTTDSTchain(chain_in, input_directory, MC_list_name);
    N_event = chain_in->GetEntries();
    std::printf("inttDSTMC N event : %lli \n", N_event);
}

void INTTReadTree::TChainInit_MC_geo_test()
{
    chain_in = new TChain(tree_name.c_str());
    inttDSTMC = new INTTDSTchain(chain_in, input_directory, MC_list_name);
    N_event = chain_in->GetEntries();
    std::printf("inttDSTMC N event : %lli \n", N_event);
    // gen_ladder_offset();
}

void INTTReadTree::TTreeInit_private()
{
    file_in = TFile::Open(Form("%s/%s.root", input_directory.c_str(), MC_list_name.c_str()));
    tree = (TTree *)file_in->Get(tree_name.c_str());
    inttCluData = new PrivateCluReader(tree);
    N_event = tree -> GetEntries();
    std::printf("private gen tree, N event : %lli \n", N_event);

}

void INTTReadTree::TTreeInit_private_geo1()
{
    file_in = TFile::Open(Form("%s/%s.root", input_directory.c_str(), MC_list_name.c_str()));
    // file_in = new TFile(Form("%s/%s.root", input_directory.c_str(), MC_list_name.c_str()),"read");
    tree = (TTree *)file_in->Get(tree_name.c_str());
    inttCluData = new PrivateCluReader(tree);
    N_event = tree -> GetEntries();
    std::printf("private gen tree, N event : %lli \n", N_event);
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

        evt_length = inttDSTMC->NClus;
        bco_full   = -1;
        NvtxMC     = inttDSTMC->NTruthVtx;
        TrigXvtxMC = inttDSTMC->TruthPV_trig_x;
        TrigYvtxMC = inttDSTMC->TruthPV_trig_y;
        TrigZvtxMC = inttDSTMC->TruthPV_trig_z;
        Centrality_bimp = inttDSTMC->centrality_bimp; // note : the centrality bin information, these two are similar, but a bit different in terms of the definition which leads to the different result, but they more and less have positive correlation
        Centrality_mbd = inttDSTMC->centrality_mbd;   // note : the centrality bin information,
        for (int track_i = 0; track_i < inttDSTMC->UniqueAncG4P_Eta->size(); track_i++) {true_track_info.push_back({inttDSTMC->UniqueAncG4P_Eta->at(track_i),inttDSTMC->UniqueAncG4P_Phi->at(track_i),float(inttDSTMC->UniqueAncG4P_PID->at(track_i))});}
    }
    else if (data_type_list[data_type] == "data_private" || data_type_list[data_type] == "data_private_geo1")
    {
        inttCluData->LoadTree(event_i);
        inttCluData->GetEntry(event_i);

        evt_length = inttCluData->x->size();
        bco_full   = inttCluData->bco_full;
        NvtxMC     = -1;
        TrigXvtxMC = -1000.;
        TrigYvtxMC = -1000.;
        TrigZvtxMC = -1000.;
    }
}

long long INTTReadTree::GetNEvt() { return N_event; }
unsigned long INTTReadTree::GetEvtNClus() { return evt_length; }
double INTTReadTree::GetTrigZvtxMC() {return TrigZvtxMC;}
vector<double> INTTReadTree::GetTrigvtxMC() {return {TrigXvtxMC,TrigYvtxMC,TrigZvtxMC};}
int INTTReadTree::GetNvtxMC() {return NvtxMC;}
string INTTReadTree::GetRunType() { return run_type_out; }
Long64_t INTTReadTree::GetBCOFull() {return bco_full;}
map<string, pair<double,double>> INTTReadTree::GetLadderOffsetMap() {return ladder_offset_map;}
float INTTReadTree::GetCentralityBin() {return Centrality_bimp;}
vector<vector<float>> INTTReadTree::GetTrueTrackInfo() {return true_track_info;}

unsigned long INTTReadTree::GetEvtNClusPost() 
{ 
    return temp_sPH_inner_nocolumn_vec.size() + temp_sPH_outer_nocolumn_vec.size(); 
}

pair<double, double> INTTReadTree::offset_correction(map<string,pair<double,double>> input_map)
{
    double N_pair = 0;
    double sum_x = 0;
    double sum_y = 0;

    for (const auto& pair : input_map)
    {
        N_pair += 1;
        sum_x += pair.second.first;
        sum_y += pair.second.second;
    }

    return {sum_x/N_pair, sum_y/N_pair};
}

void INTTReadTree::EvtSetCluGroup()
{
    if (data_type_list[data_type] == "MC" || data_type_list[data_type] == "MC_inner_phi_rotation"){
        for (int clu_i = 0; clu_i < evt_length; clu_i++)
        {
            if (int(inttDSTMC -> ClusPhiSize -> at(clu_i)) > clu_size_cut) continue; 
            if (int(inttDSTMC -> ClusAdc -> at(clu_i)) < clu_sum_adc_cut) continue;

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
                    inttDSTMC -> ClusLayer -> at(clu_i), // note : should be 5 or 6, for the outer layer, this is for the mega cluster search
                    clu_phi
                });            
            }        
        }
    }

    else if (data_type_list[data_type] == "data_private"){
        for (int clu_i = 0; clu_i < evt_length; clu_i++)
        {
            if (int(inttCluData -> size -> at(clu_i)) > clu_size_cut) continue; 
            if (int(inttCluData -> sum_adc_conv -> at(clu_i)) < clu_sum_adc_cut) continue;

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

    else if (data_type_list[data_type] == "data_private_geo1"){
        for (int clu_i = 0; clu_i < evt_length; clu_i++)
        {
            if (int(inttCluData -> size -> at(clu_i)) > clu_size_cut) {continue;} 
            if (int(inttCluData -> sum_adc_conv -> at(clu_i)) < clu_sum_adc_cut) {continue;}

            string ladder_name = inttConv -> GetLadderName(Form("intt%i_%i", inttCluData -> server -> at(clu_i) - 3001, inttCluData -> module -> at(clu_i)));

            // note : the following selections only keeps a small portion of INTT clusters
            // if (inttCluData -> server -> at(clu_i) - 3001 > 3) {continue;} // note : only the 3001, 3002, 3003, 3004 are included in the study (south)
            if ((ladder_name).substr(0,4) == "B0L1") {continue;}
            if ((ladder_name).substr(0,4) == "B1L1") {continue;}
            // if (inttCluData -> column -> at(clu_i) > 5) {continue;} // note : only the column 1, 2, 3, 4, 5 are included in the study (south type B sensor only)

            if ( (ladder_name).substr(0,6) == "B0L001") {continue;}
            if ( (ladder_name).substr(0,6) == "B0L004") {continue;}
            if ( (ladder_name).substr(0,6) == "B0L007") {continue;}
            if ( (ladder_name).substr(0,6) == "B0L010") {continue;}

            if ( (ladder_name).substr(0,6) == "B1L001") {continue;}
            if ( (ladder_name).substr(0,6) == "B1L002") {continue;}
            if ( (ladder_name).substr(0,6) == "B1L005") {continue;}
            if ( (ladder_name).substr(0,6) == "B1L006") {continue;}
            if ( (ladder_name).substr(0,6) == "B1L009") {continue;}
            if ( (ladder_name).substr(0,6) == "B1L010") {continue;}
            if ( (ladder_name).substr(0,6) == "B1L013") {continue;}
            if ( (ladder_name).substr(0,6) == "B1L014") {continue;}

            // note : because the corresponding hald-ladder of this one, the B1L015S, has no data
            if ( (ladder_name).substr(0,6) == "B0L011") {continue;}
            if ( (ladder_name).substr(0,6) == "B1L015") {continue;}

            // note : for the hypothesis test
            // if ( (ladder_name).substr(0,6) == "B1L004") {continue;}
            // if ( (ladder_name).substr(0,6) == "B0L003") {continue;} // note : pair
            // if ( (ladder_name).substr(0,6) == "B1L007") {continue;}
            // if ( (ladder_name).substr(0,6) == "B0L005") {continue;} // note : pair
            // if ( (ladder_name).substr(0,6) == "B1L011") {continue;}
            // if ( (ladder_name).substr(0,6) == "B0L008") {continue;} // note : pair
            // if ( (ladder_name).substr(0,6) == "B1L012") {continue;}
            // if ( (ladder_name).substr(0,6) == "B0L009") {continue;} // note : pair
            // if ( (ladder_name).substr(0,6) == "B0L000") {continue;}
            // if ( (ladder_name).substr(0,6) == "B1L000") {continue;} // note : pair

            double clu_x_offset = ladder_offset_map[(ladder_name).substr(0,6)].first;
            double clu_y_offset = ladder_offset_map[(ladder_name).substr(0,6)].second;

            double clu_x = inttCluData -> x -> at(clu_i) + clu_x_offset;
            double clu_y = inttCluData -> y -> at(clu_i) + clu_y_offset;
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

    else if (data_type_list[data_type] == "MC_geo_test"){
        for (int clu_i = 0; clu_i < evt_length; clu_i++)
        {
            if (inttDSTMC -> ClusLadderZId -> at(clu_i) != 0) continue; // note : only the south-side sensor B is included in the study
            if (int(inttDSTMC -> ClusPhiSize -> at(clu_i)) > clu_size_cut) continue; 
            if (int(inttDSTMC -> ClusAdc -> at(clu_i)) < clu_sum_adc_cut) continue;

            double clu_x_offset = ladder_offset_map[Form("%i_%i",inttDSTMC -> ClusLayer -> at(clu_i), inttDSTMC -> ClusLadderPhiId -> at(clu_i))].first;
            double clu_y_offset = ladder_offset_map[Form("%i_%i",inttDSTMC -> ClusLayer -> at(clu_i), inttDSTMC -> ClusLadderPhiId -> at(clu_i))].second;

            double clu_x = inttDSTMC -> ClusX -> at(clu_i) * 10 + clu_x_offset; // note : change the unit from cm to mm
            double clu_y = inttDSTMC -> ClusY -> at(clu_i) * 10 + clu_y_offset;
            double clu_z = inttDSTMC -> ClusZ -> at(clu_i) * 10;
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
                ladder_offset_map[Form("%i_%i", layer_i, phi_i)] = {rand -> Uniform(-offset_range, offset_range), rand -> Uniform(-offset_range, offset_range)};
                // ladder_offset_map[Form("%i_%i", layer_i, phi_i)] = {100., 100.};
            }
        }

        // note : it's possible that the whole system has a systematic offset, so we need to correct it
        pair<double,double> XY_correction = offset_correction(ladder_offset_map);
        for (const auto& pair : ladder_offset_map)
        {
            ladder_offset_map[pair.first] = {pair.second.first - XY_correction.first, pair.second.second - XY_correction.second};
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