// #include "/sphenix/user/ChengWei/INTT/INTT_commissioning/INTT_CW/INTT_commissioning/DAC_Scan/DAC_Scan_ladder.h"
#include "InttConversion_new.h"
#include "InttClustering.h"

// todo : the number of number is given by the adc_setting_run !!!
// todo : also the range of the hist.
// todo : the adc follows the following convention
// todo : 1. the increment has to be 4
// todo : remember to check the "adc_conv"
// vector<vector<int>> adc_setting_run = {	
//     // {8  , 12 , 16 , 20 , 24 , 28 , 32 , 36 },
//     // {28 , 32 , 36 , 40 , 44 , 48 , 52 , 56 },
//     {48 , 52 , 56 , 60 , 64 , 68 , 72 , 76 }, // note : 3
//     {68 , 72 , 76 , 80 , 84 , 88 , 92 , 96 }, // note : 4
//     {88 , 92 , 96 , 100, 104, 108, 112, 116}, // note : 5
//     {108, 112, 116, 120, 124, 128, 132, 136}, // note : 6
//     {128, 132, 136, 140, 144, 148, 152, 156}, // note : 7
//     // {148, 152, 156, 160, 164, 168, 172, 176},
//     // {168, 172, 176, 180, 184, 188, 192, 196},
//     // {188, 192, 196, 200, 204, 208, 212, 216}
// };

vector<vector<int>> adc_setting_run = {	
    {15, 30, 60, 90, 120, 150, 180, 210, 240}
    // {8  , 12 , 16 , 20 , 24 , 28 , 32 , 36 },
    // {28 , 32 , 36 , 40 , 44 , 48 , 52 , 56 },
    // {48 , 52 , 56 , 60 , 64 , 68 , 72 , 76 }, // note : 3
    // {68 , 72 , 76 , 80 , 84 , 88 , 92 , 96 }, // note : 4
    // {88 , 92 , 96 , 100, 104, 108, 112, 116}, // note : 5
    // {108, 112, 116, 120, 124, 128, 132, 136}, // note : 6
    // {128, 132, 136, 140, 144, 148, 152, 156}, // note : 7
    // {148, 152, 156, 160, 164, 168, 172, 176},
    // {168, 172, 176, 180, 184, 188, 192, 196},
    // {188, 192, 196, 200, 204, 208, 212, 216}
};

struct full_hit_info {
    int FC;
    int chip_id;
    int chan_id;
    int adc;
};

struct ladder_info {
    int FC;
    TString Port;
    int ROC;
    int Direction; // note : 0 : south, 1 : north 
};

struct to_ch_info {
    int pid;
    int module;
    int chip;
    int chan;
};


void gen_INTT_cluster_BCO(string sub_folder_string, string file_name, int DAC_run_ID, int Nhit_cut, int geo_mode_id, int run_Nevent, int hot_ch_cut)
{

    // string mother_folder_directory = "/home/phnxrc/INTT/cwshih/DACscan_data/zero_magnet_Takashi_used";
    // string file_name = "beam_inttall-00020869-0000_event_base_ana";

    

    string mother_folder_directory = "/sphenix/user/ChengWei/INTT/INTT_commissioning/" + sub_folder_string;
    // int DAC_run_ID = 0;
    // int Nhit_cut = 1500;
    // int run_Nevent = 36000;
    double peek = 3.32405;

    // vector<int> adc_config = {15, 30, 60, 90, 120, 150, 180, 210, 240};
    vector<vector<int>> adc_setting_run = {	
        {15, 30, 60, 90, 120, 150, 180, 210, 240}, // todo : modify the threshold
        {8  , 12 , 16 , 20 , 24 , 28 , 32 , 36 },
        {28 , 32 , 36 , 40 , 44 , 48 , 52 , 56 }, // note : 2
        {48 , 52 , 56 , 60 , 64 , 68 , 72 , 76 }, // note : 3
        {68 , 72 , 76 , 80 , 84 , 88 , 92 , 96 }, // note : 4
        {88 , 92 , 96 , 100, 104, 108, 112, 116}, // note : 5
        {108, 112, 116, 120, 124, 128, 132, 136}, // note : 6
        {128, 132, 136, 140, 144, 148, 152, 156}, // note : 7
        {148, 152, 156, 160, 164, 168, 172, 176}, // note : 8
        {168, 172, 176, 180, 184, 188, 192, 196},
        {188, 192, 196, 200, 204, 208, 212, 216}
    };

    int N_total_ladder = 14;
    int N_server       = 8;
    vector<string> conversion_mode_BD = {"ideal", "survey_1_XYAlpha_Peek", "full_survey_3.32"};
    string conversion_mode = conversion_mode_BD[geo_mode_id];

    // note : the initiator for the channel position
    InttConversion * ch_pos_DB = new InttConversion(conversion_mode, peek);
    

    vector<int> adc_convert(8,0);
    for (int i=0; i<8; i++) {adc_convert[i] = (adc_setting_run[DAC_run_ID][i]+adc_setting_run[DAC_run_ID][i+1])/2.;} // todo : change the run setting here

    vector<vector<hit_info>> single_event_hit_ladder_vec(N_total_ladder); // note : [ladder]
    vector<vector<vector<hit_info>>> single_event_hit_vec(N_server, single_event_hit_ladder_vec); // note : [server][ladder] 
    // vector<vector<clu_info>> clu_ladder_vec(N_total_ladder); // note : [ladder]
    vector<clu_info> clu_vec; 

    int N_hits;
    int N_cluster_inner = 0;
    int N_cluster_outer = 0;
    Long64_t bco_full_out;
    vector<int> column_out_vec; column_out_vec.clear();
    vector<double> avg_chan_out_vec; avg_chan_out_vec.clear();
    vector<int> sum_adc_out_vec; sum_adc_out_vec.clear();
    vector<int> sum_adc_conv_out_vec; sum_adc_conv_out_vec.clear();
    vector<int> size_out_vec; size_out_vec.clear();
    vector<double> x_out_vec; x_out_vec.clear();
    vector<double> y_out_vec; y_out_vec.clear();
    vector<double> z_out_vec; z_out_vec.clear();
    vector<int> layer_out_vec; layer_out_vec.clear();
    vector<double> phi_out_vec; phi_out_vec.clear();
    vector<vector<double>> bco_diff_out_vec; bco_diff_out_vec.clear(); // note : it should be integer, but somehow it doesn't work well with ROOT
    vector<int>server_out_vec; server_out_vec.clear();
    vector<int>module_out_vec; module_out_vec.clear();

    int pid_hot, module_hot, chip_id_hot, chan_id_hot;
    vector<to_ch_info> hot_ch_vec; hot_ch_vec.clear();
    
    TFile * file_hot;
    TTree * tree_hot;
    if (hot_ch_cut != 0){
        file_hot = TFile::Open(Form("%s/PreCheck_%s/hot_ch_map_criterion_%i.root",mother_folder_directory.c_str(),file_name.c_str(),hot_ch_cut));
        tree_hot = (TTree *)file_hot->Get("tree");
        long long N_event_hot = tree_hot -> GetEntries();
        cout<<"N hot channel in the file : "<<Form("PreCheck_%s/hot_ch_map_criterion_%i.root",file_name.c_str(),hot_ch_cut)<<" : "<<N_event_hot<<endl;
        tree_hot -> SetBranchAddress("pid",    &pid_hot);
        tree_hot -> SetBranchAddress("module", &module_hot);
        tree_hot -> SetBranchAddress("chip_id",&chip_id_hot);
        tree_hot -> SetBranchAddress("chan_id",&chan_id_hot);
        for (int i = 0; i < N_event_hot; i++) {
            tree_hot -> GetEntry(i);
            hot_ch_vec.push_back({pid_hot,module_hot,chip_id_hot,chan_id_hot});
        }
        file_hot -> Close();
    }
    else {
        cout<<"no hot channel file is used"<<endl;
    }
    

    TFile * file_in = TFile::Open(Form("%s/%s.root",mother_folder_directory.c_str(),file_name.c_str()));
    TTree * tree = (TTree *)file_in->Get("tree");
    long long N_event = tree -> GetEntries();
    cout<<Form("N_event in file %s : %lli",file_name.c_str(), N_event)<<endl;

    if (run_Nevent > N_event) run_Nevent = N_event; // = (N_event/1000)*1000;

    int fNhits;
    int pid[100000];
    int module[100000];
    int chip_id[100000];
    int chan_id[100000];
    int adc[100000];
    int bco[100000];
    Long64_t bco_full[100000];

    
    tree -> SetBranchStatus("*",0);
    tree -> SetBranchStatus("fNhits",1);
    tree -> SetBranchStatus("fhitArray.pid",1);
    tree -> SetBranchStatus("fhitArray.module",1);
    tree -> SetBranchStatus("fhitArray.chip_id",1);
    tree -> SetBranchStatus("fhitArray.chan_id",1);
    tree -> SetBranchStatus("fhitArray.adc",1);
    tree -> SetBranchStatus("fhitArray.bco",1);
    tree -> SetBranchStatus("fhitArray.bco_full",1);

    tree -> SetBranchAddress("fNhits",&fNhits);
    tree -> GetEntry(0); // note : actually I really don't know why this line is necessary.
    tree -> SetBranchAddress("fhitArray.pid",&pid[0]);
    tree -> SetBranchAddress("fhitArray.module",&module[0]);
    tree -> SetBranchAddress("fhitArray.chip_id",&chip_id[0]);
    tree -> SetBranchAddress("fhitArray.chan_id",&chan_id[0]);
    tree -> SetBranchAddress("fhitArray.adc",&adc[0]);
    tree -> SetBranchAddress("fhitArray.bco",&bco[0]);
    tree -> SetBranchAddress("fhitArray.bco_full",&bco_full[0]);

    string out_file_name = Form("%s_cluster_%s",file_name.c_str(),conversion_mode.c_str());
    if (conversion_mode == "survey_1_XYAlpha_Peek") out_file_name += Form("_%.2fmm",peek);
    out_file_name += Form("_excludeR%i",Nhit_cut);
    out_file_name += Form("_%1.fkEvent",run_Nevent/1000.);
    out_file_name += Form("_%iHotCut",hot_ch_cut);

    TFile * out_file = new TFile(Form("%s/%s.root",mother_folder_directory.c_str(),out_file_name.c_str()),"RECREATE");

    TTree * tree_out =  new TTree ("tree_clu", "clustering info.");
    tree_out -> Branch("nhits",&N_hits);
    tree_out -> Branch("nclu_inner",&N_cluster_inner);
    tree_out -> Branch("nclu_outer",&N_cluster_outer);
    tree_out -> Branch("bco_full",&bco_full_out);

    tree_out -> Branch("column", &column_out_vec);
    tree_out -> Branch("avg_chan", &avg_chan_out_vec);
    tree_out -> Branch("sum_adc", &sum_adc_out_vec);
    tree_out -> Branch("sum_adc_conv", &sum_adc_conv_out_vec);
    tree_out -> Branch("size", &size_out_vec);
    tree_out -> Branch("x", &x_out_vec);
    tree_out -> Branch("y", &y_out_vec);
    tree_out -> Branch("z", &z_out_vec);
    tree_out -> Branch("layer", &layer_out_vec);
    tree_out -> Branch("phi", &phi_out_vec);
    tree_out -> Branch("bco_diff_vec",&bco_diff_out_vec);
    tree_out -> Branch("server",&server_out_vec);
    tree_out -> Branch("module",&module_out_vec);


    for (int i = 0; i < run_Nevent; i++ ) // note : event
    {
        tree -> GetEntry(i);
        if (i % 10 == 0){cout<<"clustering : "<<i<<endl;}
        // cout<<"=================================== =================================== ==================================="<<endl;
        // cout<<"event ID : "<<i<<endl;

        N_hits = fNhits;
        bco_full_out = bco_full[0];

        bool check_bco_tag = (bco_full_out == 881787581811) ? true : false;

        // todo : the fNhits cut is here 
        if ( fNhits > Nhit_cut ) {
            clu_vec.clear();
            single_event_hit_vec.clear(); single_event_hit_vec = vector<vector<vector<hit_info>>>(N_server, single_event_hit_ladder_vec);
            

            N_cluster_inner = -1;
            N_cluster_outer = -1;
            tree_out -> Fill();

            N_cluster_inner = 0;
            N_cluster_outer = 0;
            column_out_vec.clear();
            avg_chan_out_vec.clear();
            sum_adc_out_vec.clear();
            sum_adc_conv_out_vec.clear();
            size_out_vec.clear();
            x_out_vec.clear();
            y_out_vec.clear();
            z_out_vec.clear();
            layer_out_vec.clear();
            phi_out_vec.clear();

            server_out_vec.clear();
            module_out_vec.clear();
            bco_diff_out_vec.clear();

            continue;
        }

        for (int i1 = 0; i1 < fNhits; i1++) // note : N hit in one event 
        {

            double hot_ch_tag = 0;

            // note : pid 3001 to 3008
            if (pid[i1] > 3000 && pid[i1] < 3009 && module[i1] > -1 && module[i1] < 14 && chip_id[i1] > 0 && chip_id[i1] < 27 && chan_id[i1] > -1 && chan_id[i1] < 128 && adc[i1] > -1 && adc[i1] < 8) 
            {
                if (check_bco_tag) 
                {
                    cout<<"test event : "<<chip_id[i1]<<" "<<chan_id[i1]<<" "<<adc[i1]<<" "<<adc_convert[adc[i1]]<<endl;
                }

                if (pid[i1] == 3001 && module[i1] == 1)  continue; // todo : this half-ladder is masked.
                if (pid[i1] == 3003 && module[i1] == 11) continue; // todo : this half-ladder is masked. for the beam run , intt2_11, B1L010S

                // todo : change the BCO cut
                int bco_diff = ( ((bco_full[i1]&0x7F) - bco[i1]) < 0 ) ? ((bco_full[i1]&0x7F) - bco[i1]) + 128 : ((bco_full[i1]&0x7F) - bco[i1]);

                // todo : bco_diff cut for the run 20869
                bool bco_tag_3001 = (bco_diff == 21 || bco_diff == 22)                   ? true : false;
                bool bco_tag_3002 = (bco_diff == 21 || bco_diff == 22 || bco_diff == 23) ? true : false;
                bool bco_tag_3003 = (bco_diff == 81)                                     ? true : false;
                bool bco_tag_3004 = (bco_diff == 113)                                    ? true : false;
                bool bco_tag_3005 = (bco_diff == 63 || bco_diff == 64)                   ? true : false;
                bool bco_tag_3006 = (bco_diff == 39 || bco_diff == 40)                   ? true : false;
                bool bco_tag_3007 = (bco_diff == 82)                                     ? true : false;
                bool bco_tag_3008 = (bco_diff == 20)                                     ? true : false;

                // todo : bco_diff cut for the run 20888
                // bool bco_tag_3001 = (bco_diff == 50 || bco_diff == 51 || bco_diff == 52 || bco_diff == 53)                   ? true : false;
                // bool bco_tag_3002 = (bco_diff == 48 || bco_diff == 49 || bco_diff == 50 || bco_diff == 51)                   ? true : false;
                // bool bco_tag_3003 = (bco_diff == 50 || bco_diff == 51 || bco_diff == 52 || bco_diff == 53 || bco_diff == 54) ? true : false;
                // bool bco_tag_3004 = (bco_diff == 62 || bco_diff == 63 || bco_diff == 64)                                     ? true : false;
                // bool bco_tag_3005 = (bco_diff == 114 || bco_diff == 115 || bco_diff == 116 || bco_diff == 117)               ? true : false;
                // bool bco_tag_3006 = (bco_diff == 97 || bco_diff == 98 || bco_diff == 99 || bco_diff == 100)                  ? true : false;
                // bool bco_tag_3007 = (bco_diff == 98 || bco_diff == 99 || bco_diff == 100 )                                   ? true : false;
                // bool bco_tag_3008 = (bco_diff == 87 || bco_diff == 88 || bco_diff == 89)                                     ? true : false;

                // todo : the bco_diff cut, remember to activate these lines
                if (pid[i1] == 3001 && bco_tag_3001 == false) continue;
                if (pid[i1] == 3002 && bco_tag_3002 == false) continue;
                if (pid[i1] == 3003 && bco_tag_3003 == false) continue;
                if (pid[i1] == 3004 && bco_tag_3004 == false) continue;
                if (pid[i1] == 3005 && bco_tag_3005 == false) continue;
                if (pid[i1] == 3006 && bco_tag_3006 == false) continue;
                if (pid[i1] == 3007 && bco_tag_3007 == false) continue;
                if (pid[i1] == 3008 && bco_tag_3008 == false) continue;


                if (hot_ch_cut != 0){
                    for (int i2 = 0; i2 < hot_ch_vec.size(); i2++)
                    {
                        // cout<<"test : "<<hot_ch_vec[i2].pid<<" "<<hot_ch_vec[i2].module<<" "<<hot_ch_vec[i2].chip<<" "<<hot_ch_vec[i2].chan<<endl;

                        if ( pid[i1] == hot_ch_vec[i2].pid && module[i1] == hot_ch_vec[i2].module && chip_id[i1] == hot_ch_vec[i2].chip && chan_id[i1] == hot_ch_vec[i2].chan ){
                            hot_ch_tag = 1;
                            break;
                        }
                    }    
                }
                
                if (hot_ch_tag == 1) continue;
                
                // cout<<i<<" hit bco_diff : "<<bco_diff<<endl;
                // cout<<i<<" pid : "<<pid[i1]<<" module : "<<module[i1]<<" chip : "<<chip_id[i1]<<" chan "<<chan_id[i1]<<" adc "<<adc[i1]<<" adc_conv "<<adc_convert[adc[i1]]<<" hit bco_diff : "<<bco_diff<<" bco_full : "<<bco_full[i1]<<" bco : "<<bco[i1]<<endl;
                
                
                // if (bco_diff < 24 || bco_diff > 30) continue;

                single_event_hit_vec[ pid[i1] - 3001 ][ module[i1] ].push_back({chip_id[i1], chan_id[i1], adc[i1], adc_convert[adc[i1]], bco_diff});
            }
                
        } // note : end of the hit in one event

        for (int i1 = 0; i1 < N_server; i1++) // note : server, one event 
        {
            for (int i2 = 0; i2 < N_total_ladder; i2++) // note : ladder, one event 
            {
                
                if ( single_event_hit_vec[i1][i2].size() > 0 )
                {
                    clu_vec = InttClustering::clustering(Form("intt%i",i1), i2, single_event_hit_vec[i1][i2], ch_pos_DB);
                    // cout<<"-------> eID : "<<i<<" Nhit : "<<fNhits<<" serverID : "<<i1<<" ladderID : "<<i2<<" clu_vec size : "<<clu_vec.size()<<" inner_nclu : "<<N_cluster_inner<<" outer_nclu : "<<N_cluster_outer<<endl;

                    for (int clu_i = 0; clu_i < clu_vec.size(); clu_i++)
                    {
                        
                        if (check_bco_tag)
                        {
                            cout<<"after cluster : "<<clu_vec[clu_i].column<<" "<<clu_vec[clu_i].sum_adc_conv<<" "<<clu_vec[clu_i].size<<" "<<clu_vec[clu_i].x<<" "<<clu_vec[clu_i].y<<" "<<clu_vec[clu_i].z<<" "<<clu_vec[clu_i].layer<<" "<<clu_vec[clu_i].phi<<endl;
                        }

                        column_out_vec.push_back( clu_vec[clu_i].column );
                        avg_chan_out_vec.push_back( clu_vec[clu_i].avg_chan );
                        sum_adc_out_vec.push_back( clu_vec[clu_i].sum_adc );
                        sum_adc_conv_out_vec.push_back( clu_vec[clu_i].sum_adc_conv );
                        size_out_vec.push_back( clu_vec[clu_i].size );
                        x_out_vec.push_back( clu_vec[clu_i].x );
                        y_out_vec.push_back( clu_vec[clu_i].y );
                        z_out_vec.push_back( clu_vec[clu_i].z );
                        layer_out_vec.push_back( clu_vec[clu_i].layer );
                        phi_out_vec.push_back( clu_vec[clu_i].phi );

                        if (clu_vec[clu_i].layer == 0) {N_cluster_inner += 1;}
                        else if (clu_vec[clu_i].layer == 1) {N_cluster_outer += 1;}

                        server_out_vec.push_back( i1 + 3001 ); // note : pid
                        module_out_vec.push_back( i2 ); // note : module ID
                        bco_diff_out_vec.push_back( clu_vec[clu_i].bco_diff_vec );
                    }
                }                

                clu_vec.clear();
            } // note : end of ladder
        } // note : end of server

        // cout<<"test : "<<i<<" Nhit : "<<fNhits<<" nclu_inner : "<<N_cluster_inner<<" nclu_outer : "<<N_cluster_outer<<endl; 

        tree_out -> Fill();

        N_cluster_inner = 0;
        N_cluster_outer = 0;
        column_out_vec.clear();
        avg_chan_out_vec.clear();
        sum_adc_out_vec.clear();
        sum_adc_conv_out_vec.clear();
        size_out_vec.clear();
        x_out_vec.clear();
        y_out_vec.clear();
        z_out_vec.clear();
        layer_out_vec.clear();
        phi_out_vec.clear();
        server_out_vec.clear();
        module_out_vec.clear();
        bco_diff_out_vec.clear();

        clu_vec.clear();
        single_event_hit_vec.clear(); single_event_hit_vec = vector<vector<vector<hit_info>>>(N_server, single_event_hit_ladder_vec);
    }// note : end of event


    // file_in -> Close();

    
    tree_out->SetDirectory(out_file);
    tree_out -> Write("", TObject::kOverwrite);

    cout<<"conversion done, file : "<<out_file_name<<endl;
	out_file -> Close();

}