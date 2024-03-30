#include "INTTReadTree.h"
#include "INTTXYvtx.h"

void data20869_xy_geo1(int loop_i, int core_i)
{
    string input_directory = "/sphenix/user/ChengWei/INTT/INTT_commissioning/ZeroField/20869";
    string file_name = "beam_inttall-00020869-0000_event_base_ana_cluster_full_survey_3.32_excludeR1500_20kEvent_3HotCut";
    string out_folder_directory = input_directory + "/" + "folder_beam_inttall-00020869-0000_event_base_ana_cluster_full_survey_3.32_excludeR1500_20kEvent_3HotCut_VTXxy_geo3";
    string tree_name = "tree_clu";
    
    int N_loop = 20;
    int N_trial = 500;

    // pair<double, double> beam_origin = {-0.015, 0.012};
    // pair<double, double> beam_origin = {-0.0, 0.0};
    pair<double, double> beam_origin = {0.,-2.};
    pair<double, double> DCA_cut ={-0.5, 0.5};
    // pair<double, double> zvtx_QA_width = {35, 70}; 
    // double zvtx_QA_ratio = 0.00005;
    double phi_diff_cut = 5.72;
    int clu_sum_adc_cut = 31;
    int clu_size_cut = 4;
    int N_clu_cut = 350;
    int N_clu_cutl = 20;
    
    bool draw_event_display = false;
    double peek = 3.324;
    int data_type = 3; // note : data with geometry study
    int geo_mode_id = 1; // note : 0 -> perfect geo
    int zvtx_cal_require = 15;
    // int random_seed;
    // int random_seed = 65539;
    int N_ladder = 14;
    double offset_range = 0.2;

    // vector<string> included_ladder_vec = {
    //     "B0L000S","B0L002S","B0L003S", "B0L005S", "B0L006S", "B0L008S", "B0L009S", 
    //     "B1L000S","B1L003S","B1L004S", "B1L007S", "B1L008S", "B1L011S", "B1L012S"
    // };

    vector<string> included_ladder_vec = {
        "B0L000","B0L002","B0L003", "B0L005", "B0L006", "B0L008", "B0L009", 
        "B1L000","B1L003","B1L004", "B1L007", "B1L008", "B1L011", "B1L012"
    };

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

    for (int trial_i = 0; trial_i < N_trial; trial_i++)
    {
        // cout<<" start trial : "<<trial_i<<"-------------------- -------------------- -------------------- --------------------"<<endl;
        // random_seed = trial_i;

        final_random_seed = trial_i + loop_i * N_trial + (N_loop * N_trial) * (core_i + 0);
        random_seed_out = final_random_seed;

        INTTReadTree * INTTClu = new INTTReadTree(data_type, input_directory, file_name, tree_name, clu_size_cut, clu_sum_adc_cut, final_random_seed, N_ladder, offset_range, included_ladder_vec);
        map<string, pair<double,double>> ladder_offset_map = INTTClu -> GetLadderOffsetMap();
        for (int ladder_i = 0; ladder_i < N_ladder; ladder_i++)
        {
            offset_x_vec.push_back( ladder_offset_map[ included_ladder_vec[ladder_i].c_str() ].first );
            offset_y_vec.push_back( ladder_offset_map[ included_ladder_vec[ladder_i].c_str() ].second);
        }

        INTTXYvtx    * MCxy    = new INTTXYvtx(INTTClu -> GetRunType(), out_folder_directory, beam_origin, geo_mode_id, phi_diff_cut, DCA_cut, N_clu_cutl, N_clu_cut, draw_event_display, peek, 0, 3, false);
        // cout<<"Total event : "<<INTTClu -> GetNEvt()<<endl;


        for (int event_i = 0; event_i < 20000; event_i ++)
        {
            INTTClu -> EvtInit(event_i);
            INTTClu -> EvtSetCluGroup();

            MCxy     -> ProcessEvt(
                event_i, 
                INTTClu -> temp_sPH_inner_nocolumn_vec, INTTClu -> temp_sPH_outer_nocolumn_vec, 
                INTTClu -> temp_sPH_nocolumn_vec, INTTClu -> temp_sPH_nocolumn_rz_vec, 
                INTTClu -> GetNvtxMC(), INTTClu -> GetTrigZvtxMC(), INTTClu -> GetPhiCheckTag(), INTTClu -> GetBCOFull()
            );

            MCxy -> ClearEvt();
            INTTClu -> EvtClear();
        }
        // MCxy -> PrintPlots();

        vector<pair<double,double>> out_vtx = MCxy -> MacroVTXSquare(4,10, false);
        // cout<<"The best vertex throughout the scan: "<<out_vtx[0].first<<" "<<out_vtx[0].second<<endl;
        // cout<<"The origin during that scan: "<<out_vtx[1].first<<" "<<out_vtx[1].second<<endl;

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

        MCxy -> TH2F_FakeClone(MCxy -> GetHistFinal()[0], DCA_distance_inner_phi_peak_final);
        MCxy -> TH2F_FakeClone(MCxy -> GetHistFinal()[1], angle_diff_inner_phi_peak_final  );
        MCxy -> TH2F_FakeClone(MCxy -> GetHistFinal()[2], DCA_distance_outer_phi_peak_final);
        MCxy -> TH2F_FakeClone(MCxy -> GetHistFinal()[3], angle_diff_outer_phi_peak_final  );
        
        file_out -> cd();
        // tree_out->SetDirectory(file_out);
        DCA_distance_inner_phi_peak_final -> Write(Form("DCA_distance_inner_phi_peak_final_%d", final_random_seed));
        angle_diff_inner_phi_peak_final   -> Write(Form("angle_diff_inner_phi_peak_final_%d", final_random_seed));
        DCA_distance_outer_phi_peak_final -> Write(Form("DCA_distance_outer_phi_peak_final_%d", final_random_seed));
        angle_diff_outer_phi_peak_final   -> Write(Form("angle_diff_outer_phi_peak_final_%d", final_random_seed));

        tree_out -> Fill();

        // cout<<"test1"<<endl;
        MCxy -> EndRun();
        INTTClu -> EndRun();
        // cout<<"test2"<<endl;
        delete INTTClu;
        delete MCxy;
        // cout<<"test3"<<endl;
        offset_x_vec.clear();
        offset_y_vec.clear();
        // cout<<"test4"<<endl;
        // DCA_distance_inner_phi_peak_final -> Delete();
        // angle_diff_inner_phi_peak_final   -> Delete();
        // cout<<"test5"<<endl;
        // cout<<"test6"<<endl;
    }

    // cout<<"test : "<<endl;

    file_out -> cd();
    // tree_out->SetDirectory(file_out);
    tree_out -> Write("", TObject::kOverwrite);
    file_out -> Close();

    system(Form("mv %s/geo_scan_%i_%i_%i.root %s/complete_file",out_folder_directory.c_str(), core_i, loop_i, loop_i * N_trial + (N_loop * N_trial) * (core_i + 0), out_folder_directory.c_str()));
    
    // MCxy -> MacroVTXxyCorrection(0,1,2,0.,360.); // note : no phi correction, calculate vertexXY
    // MCxy -> MacroVTXxyCorrection(0,1,2,0.,360.);
    
    // MCxy -> MacroVTXxyCorrection_new(50, 250, 21);
    
}


        // cout<< INTTClu -> temp_sPH_inner_nocolumn_vec.size()<<" "
        //     << INTTClu -> temp_sPH_outer_nocolumn_vec.size()<<" "
        //     << INTTClu -> temp_sPH_nocolumn_vec.size()<<" "
        //     << INTTClu -> temp_sPH_nocolumn_rz_vec.size()<<" "
        //     << INTTClu -> GetNvtxMC()<<" "
        //     << INTTClu -> GetTrigZvtxMC()<<" "
        //     << INTTClu -> GetPhiCheckTag()<<" "<<endl;