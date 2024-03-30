#include "INTTReadTree.h"
#include "INTTXYvtx.h"

void test_xy_388_000_geo(int loop_i, int core_i)
{
    string input_directory = "/sphenix/user/ChengWei/sPH_dNdeta/dNdEta_INTT_MC_388_000";
    string file_name = "MC_ZF_xyvtx";
    string out_folder_directory = input_directory + "/Geo_scan1_" + file_name;
    string MC_list_name = "dst_INTT_dNdEta_388_000_2.list";
    string tree_name = "EventTree";

    int N_loop = 20;
    int N_trial = 500;
    
    // pair<double, double> beam_origin = {-0.015, 0.012};
    // pair<double, double> beam_origin = {-0.0, 0.0};
    // pair<double, double> beam_origin = {1.,-2.};
    pair<double, double> beam_origin = {-0.4, 2.4};
    pair<double, double> DCA_cut ={-0.5, 0.5}; // note : used
    // pair<double, double> zvtx_QA_width = {35, 70}; 
    // double zvtx_QA_ratio = 0.00005;
    double phi_diff_cut = 0.2; // note : used
    int clu_sum_adc_cut = -1; // note : used
    int clu_size_cut = 4; // note : used
    int N_clu_cut = 350; // note : used
    int N_clu_cutl = 20; // note : used
    
    bool draw_event_display = false;
    double peek = 3.324;
    int data_type = 4; // note : MC geo test, which applys the offset to each ladder
    int geo_mode_id = 0; // note : 0 -> perfect geo
    int zvtx_cal_require = 15;

    int N_ladder = 56;
    double offset_range = 0.25;
    vector<string> included_ladder_vec = {};
    vector<pair<double,double>> ladder_offset_vec = {};


    TFile * file_out = new TFile(Form("%s/geo_scan_%i_%i_%i.root",out_folder_directory.c_str(), core_i, loop_i, loop_i * N_trial + (N_loop * N_trial) * (core_i + 0)), "recreate");
    TTree * tree_out = new TTree("tree_geo_scan", "tree_geo_scan");
    vector<double> offset_x_vec;
    vector<double> offset_y_vec;

    double DCA_inner_fitYpos;
    double angle_diff_inner_fitYpos;
    double DCA_inner_fitE;
    double angle_diff_inner_fitE;
    double DCA_outer_fitYpos;
    double angle_diff_outer_fitYpos;
    double DCA_outer_fitE;
    double angle_diff_outer_fitE;
    int    random_seed_out;
    double vtxX;
    double vtxY;
    double trial_originX;
    double trial_originY;

    tree_out -> Branch("offset_x_vec", &offset_x_vec);
    tree_out -> Branch("offset_y_vec", &offset_y_vec);
    tree_out -> Branch("DCA_inner_fitYpos", &DCA_inner_fitYpos);
    tree_out -> Branch("DCA_inner_fitE", &DCA_inner_fitE);
    tree_out -> Branch("angle_diff_inner_fitYpos", &angle_diff_inner_fitYpos);
    tree_out -> Branch("angle_diff_inner_fitE", &angle_diff_inner_fitE);
    tree_out -> Branch("DCA_outer_fitYpos", &DCA_outer_fitYpos);
    tree_out -> Branch("DCA_outer_fitE", &DCA_outer_fitE);
    tree_out -> Branch("angle_diff_outer_fitYpos", &angle_diff_outer_fitYpos);
    tree_out -> Branch("angle_diff_outer_fitE", &angle_diff_outer_fitE);
    tree_out -> Branch("random_seed", &random_seed_out);
    tree_out -> Branch("vtxX", &vtxX);
    tree_out -> Branch("vtxY", &vtxY);
    tree_out -> Branch("trial_originX", &trial_originX);
    tree_out -> Branch("trial_originY", &trial_originY);
    

    TH2F * DCA_distance_inner_phi_peak_final;
    TH2F * angle_diff_inner_phi_peak_final;
    TH2F * DCA_distance_outer_phi_peak_final;
    TH2F * angle_diff_outer_phi_peak_final;

    DCA_distance_inner_phi_peak_final = new TH2F("","DCA_distance_inner_phi_peak_final",100,0,360,100,-10,10);
    DCA_distance_inner_phi_peak_final -> SetStats(0);
    DCA_distance_inner_phi_peak_final -> GetXaxis() -> SetTitle("Inner phi [degree]");
    DCA_distance_inner_phi_peak_final -> GetYaxis() -> SetTitle("DCA [mm]");
    DCA_distance_inner_phi_peak_final -> GetXaxis() -> SetNdivisions(505);

    DCA_distance_outer_phi_peak_final = new TH2F("","DCA_distance_outer_phi_peak_final",100,0,360,100,-10,10);
    DCA_distance_outer_phi_peak_final -> SetStats(0);
    DCA_distance_outer_phi_peak_final -> GetXaxis() -> SetTitle("Outer phi [degree]");
    DCA_distance_outer_phi_peak_final -> GetYaxis() -> SetTitle("DCA [mm]");
    DCA_distance_outer_phi_peak_final -> GetXaxis() -> SetNdivisions(505);

    angle_diff_inner_phi_peak_final = new TH2F("","angle_diff_inner_phi_peak_final",361,0,361,100,-1.5,1.5);
    angle_diff_inner_phi_peak_final -> SetStats(0);
    angle_diff_inner_phi_peak_final -> GetXaxis() -> SetTitle("Inner phi [degree]");
    angle_diff_inner_phi_peak_final -> GetYaxis() -> SetTitle("Inner - Outer [degree]");
    angle_diff_inner_phi_peak_final -> GetXaxis() -> SetNdivisions(505);

    angle_diff_outer_phi_peak_final = new TH2F("","angle_diff_outer_phi_peak_final",361,0,361,100,-1.5,1.5);
    angle_diff_outer_phi_peak_final -> SetStats(0);
    angle_diff_outer_phi_peak_final -> GetXaxis() -> SetTitle("Outer phi [degree]");
    angle_diff_outer_phi_peak_final -> GetYaxis() -> SetTitle("Inner - Outer [degree]");
    angle_diff_outer_phi_peak_final -> GetXaxis() -> SetNdivisions(505);

    int final_random_seed;

    INTTReadTree * INTTClu = new INTTReadTree(data_type, input_directory, MC_list_name, tree_name, clu_size_cut, clu_sum_adc_cut, 9999, N_ladder, offset_range, included_ladder_vec, 3);

    for (int trial_i = 0; trial_i < N_trial; trial_i++)
    {
        // cout<<" start trial : "<<trial_i<<"-------------------- -------------------- -------------------- --------------------"<<endl;
        // random_seed = trial_i;
        // cout<<"test-4"<<endl;
        final_random_seed = trial_i + loop_i * N_trial + (N_loop * N_trial) * (core_i + 0);
        random_seed_out = final_random_seed;
        // cout<<"test-3"<<endl;

        INTTClu -> set_random_seed(final_random_seed);
        INTTClu -> gen_ladder_offset();

        map<string, pair<double,double>> ladder_offset_map = INTTClu -> GetLadderOffsetMap();
        // cout<<"test-2"<<endl;
        for (int layer_i = 3; layer_i < 7; layer_i++)
        {
            double N_layer_ladder = (layer_i == 3 || layer_i == 4) ? 12 : 16;

            for (int phi_i = 0; phi_i < N_layer_ladder; phi_i++)
            {
                offset_x_vec.push_back( ladder_offset_map[Form("%i_%i", layer_i, phi_i)].first );
                offset_y_vec.push_back( ladder_offset_map[Form("%i_%i", layer_i, phi_i)].second );
            }
        }
        INTTXYvtx    * MCxy    = new INTTXYvtx(INTTClu -> GetRunType(), out_folder_directory, beam_origin, geo_mode_id, phi_diff_cut, DCA_cut, N_clu_cutl, N_clu_cut, draw_event_display, peek, 0, 3, false);
        // cout<<"test-1"<<endl;
        for (int event_i = 0; event_i < 2500; event_i ++)
        {
            INTTClu -> EvtInit(event_i);
            INTTClu -> EvtSetCluGroup();

            MCxy     -> ProcessEvt(
                event_i, 
                INTTClu -> temp_sPH_inner_nocolumn_vec, INTTClu -> temp_sPH_outer_nocolumn_vec, 
                INTTClu -> temp_sPH_nocolumn_vec, INTTClu -> temp_sPH_nocolumn_rz_vec, 
                INTTClu -> GetNvtxMC(), INTTClu -> GetTrigZvtxMC(), INTTClu -> GetPhiCheckTag(), -1  // note : no bco_full for MC
            );

            MCxy -> ClearEvt();
            INTTClu -> EvtClear();
        }
        // cout<<"test0"<<endl;
        vector<pair<double,double>> out_vtx = MCxy -> MacroVTXSquare(4,10,false);

        // cout<<"test1"<<endl;

        vtxX = out_vtx[0].first;
        vtxY = out_vtx[0].second;
        trial_originX = out_vtx[1].first;
        trial_originY = out_vtx[1].second;

        DCA_inner_fitE = out_vtx[2].first;
        angle_diff_inner_fitE = out_vtx[2].second;
        DCA_inner_fitYpos = out_vtx[3].first;
        angle_diff_inner_fitYpos = out_vtx[3].second;

        DCA_outer_fitE = out_vtx[4].first;
        angle_diff_outer_fitE = out_vtx[4].second;
        DCA_outer_fitYpos = out_vtx[5].first;
        angle_diff_outer_fitYpos = out_vtx[5].second;

        // cout<<"test2"<<endl;
        MCxy -> TH2F_FakeClone(MCxy -> GetHistFinal()[0], DCA_distance_inner_phi_peak_final);
        MCxy -> TH2F_FakeClone(MCxy -> GetHistFinal()[1], angle_diff_inner_phi_peak_final  );
        MCxy -> TH2F_FakeClone(MCxy -> GetHistFinal()[2], DCA_distance_outer_phi_peak_final);
        MCxy -> TH2F_FakeClone(MCxy -> GetHistFinal()[3], angle_diff_outer_phi_peak_final  );
        // cout<<"test3"<<endl;
        
        file_out -> cd();
        // tree_out->SetDirectory(file_out);
        DCA_distance_inner_phi_peak_final -> Write(Form("DCA_distance_inner_phi_peak_final_%d", final_random_seed));
        angle_diff_inner_phi_peak_final   -> Write(Form("angle_diff_inner_phi_peak_final_%d", final_random_seed));
        DCA_distance_outer_phi_peak_final -> Write(Form("DCA_distance_outer_phi_peak_final_%d", final_random_seed));
        angle_diff_outer_phi_peak_final   -> Write(Form("angle_diff_outer_phi_peak_final_%d", final_random_seed));
        // cout<<"test4"<<endl;
        tree_out -> Fill();
        // cout<<"test5"<<endl;
        // // cout<<"test1"<<endl;
        MCxy -> EndRun();
        INTTClu -> EndRun();
        // // cout<<"test2"<<endl;
        // delete INTTClu;
        delete MCxy;
        // // cout<<"test3"<<endl;
        offset_x_vec.clear();
        offset_y_vec.clear();
        // cout<<"test6"<<endl;

        INTTClu -> clear_ladder_offset_map();
    }

    file_out -> cd();
    // tree_out->SetDirectory(file_out);
    tree_out -> Write("", TObject::kOverwrite);
    file_out -> Close();

    system(Form("mv %s/geo_scan_%i_%i_%i.root %s/complete_file",out_folder_directory.c_str(), core_i, loop_i, loop_i * N_trial + (N_loop * N_trial) * (core_i + 0), out_folder_directory.c_str()));

    
    
    // vector<pair<double,double>> out_vtx_line = MCxy -> FillLine_FindVertex({(out_vtx[0].first + out_vtx[1].first)/2., (out_vtx[0].second + out_vtx[1].second)/2.}, 0.001);
    // cout<<" "<<endl;
    // cout<<"By fill-line method,"<<endl;
    // cout<<"Reco Run Vertex XY: "<<out_vtx_line[0].first<<" "<<out_vtx_line[0].second<<endl;
    // cout<<"Reco Run Vertex XY Error: "<<out_vtx_line[1].first<<" "<<out_vtx_line[1].second<<endl;

    // MCxy -> MacroVTXxyCorrection(0,1,2,0.,360.); // note : no phi correction, calculate vertexXY
    // MCxy -> MacroVTXxyCorrection(0,1,2,0.,360.);
    
    // MCxy -> MacroVTXxyCorrection_new(100, 260, 36);
    // vector<pair<double,double>> out_vtx = MCxy -> MacroVTXSquare(3,9);
    // cout<<out_vtx[0].first<<" "<<out_vtx[0].second<<endl;
    // cout<<out_vtx[1].first<<" "<<out_vtx[1].second<<endl;
}


        // cout<< INTTClu -> temp_sPH_inner_nocolumn_vec.size()<<" "
        //     << INTTClu -> temp_sPH_outer_nocolumn_vec.size()<<" "
        //     << INTTClu -> temp_sPH_nocolumn_vec.size()<<" "
        //     << INTTClu -> temp_sPH_nocolumn_rz_vec.size()<<" "
        //     << INTTClu -> GetNvtxMC()<<" "
        //     << INTTClu -> GetTrigZvtxMC()<<" "
        //     << INTTClu -> GetPhiCheckTag()<<" "<<endl;