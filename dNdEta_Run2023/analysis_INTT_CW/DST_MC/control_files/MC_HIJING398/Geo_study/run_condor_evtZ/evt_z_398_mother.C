#include "../../../../INTTReadTree.h"
#include "../../../../INTTZvtx.h"

void evt_z_398_mother(int core_i, int loop_i)
{
    string input_directory = "/sphenix/user/ChengWei/sPH_dNdeta/HIJING_ana398_xvtx-0p04cm_yvtx0p24cm_zvtx-20cm_dummyAlignParams";
    string file_name = "MC_ZF_zvtx";
    string MC_list_name = "file_list.txt";
    string tree_name = "EventTree";

    TFile * file_geo_XY = TFile::Open(Form("%s/Geo_XY_scan_trial_2/complete_file/full_merged_file/geo_scan_%i_merged.root", input_directory.c_str(),core_i));
    TTree * tree_geo_XY = (TTree*) file_geo_XY -> Get("tree_geo_scan");

    if (loop_i >= tree_geo_XY -> GetEntries()){
        cout<<"loop_i >= tree_geo_XY -> GetEntries()"<<endl;
        return;
    }

    // cout<<"test 1"<<endl;

    vector<double> * in_ladder_offset_x_vec; in_ladder_offset_x_vec = 0;
    vector<double> * in_ladder_offset_y_vec; in_ladder_offset_y_vec = 0;
    double in_cornerX;
    double in_cornerY;
    double in_centerX;
    double in_centerY;
    double in_total_offset_x;
    double in_total_offset_y;
    double Line_filled_X;
    double Line_filled_Y;
    int in_random_seed;

    // cout<<"test 2"<<endl;

    tree_geo_XY -> SetBranchAddress("offset_x_vec", &in_ladder_offset_x_vec);
    tree_geo_XY -> SetBranchAddress("offset_y_vec", &in_ladder_offset_y_vec);
    tree_geo_XY -> SetBranchAddress("total_offset_x", &in_total_offset_x);
    tree_geo_XY -> SetBranchAddress("total_offset_y", &in_total_offset_y);
    tree_geo_XY -> SetBranchAddress("vtxX", &in_cornerX);
    tree_geo_XY -> SetBranchAddress("vtxY", &in_cornerY);
    tree_geo_XY -> SetBranchAddress("trial_originX", &in_centerX);
    tree_geo_XY -> SetBranchAddress("trial_originY", &in_centerY);
    tree_geo_XY -> SetBranchAddress("random_seed", &in_random_seed);
    tree_geo_XY -> SetBranchAddress("Line_filled_X", &Line_filled_X);
    tree_geo_XY -> SetBranchAddress("Line_filled_Y", &Line_filled_Y);
    tree_geo_XY -> GetEntry(loop_i);

    // cout<<"test 3"<<endl;

    // cout<<"total offset X : "<<in_total_offset_x<<" total offset Y : "<<in_total_offset_y<<endl;
    string out_folder_directory = input_directory + "/Geo_Z_scan_trial_3"+"/random_seed_"+to_string(in_random_seed);

    // cout<<"test 4"<<endl;

    int index_i = 0;
    map<string, pair<double,double>> ladder_offset_map; ladder_offset_map.clear();
    // cout<<"test-2"<<endl;
    for (int layer_i = 3; layer_i < 7; layer_i++)
    {
        double N_layer_ladder = (layer_i == 3 || layer_i == 4) ? 12 : 16;

        for (int phi_i = 0; phi_i < N_layer_ladder; phi_i++)
        {
            ladder_offset_map[Form("%i_%i", layer_i, phi_i)] = {in_ladder_offset_x_vec -> at(index_i), in_ladder_offset_y_vec -> at(index_i)};
            index_i += 1;
        }
    }

    // cout<<"test 5"<<endl;

    pair<double, double> beam_origin = {(((in_cornerX+in_centerX)/2.) + Line_filled_X)/2., (((in_cornerY+in_centerY)/2.) + Line_filled_Y)/2.};
    cout<<"random_seed : "<<in_random_seed<<" beam_origin : "<<beam_origin.first<<" "<<beam_origin.second<<endl;
    pair<double, double> DCA_cut ={-0.5, 0.5};
    pair<double, double> zvtx_QA_width = {35, 70}; 
    double zvtx_QA_ratio = 0.00005;
    double phi_diff_cut = 0.3; // todo : it was 0.2 degree
    int clu_sum_adc_cut = -1;
    int clu_size_cut = 4;
    int N_clu_cut = 10000;
    int N_clu_cutl = 20;
    
    bool draw_event_display = false;
    double peek = 3.324;
    int data_type = 4; // note : MC
    int geo_mode_id = 0; // note : 0 -> perfect geo
    int zvtx_cal_require = 15;

    INTTReadTree * INTTClu = new INTTReadTree(data_type, input_directory, MC_list_name, tree_name, clu_size_cut, clu_sum_adc_cut);
    INTTClu -> set_ladder_offset(ladder_offset_map);
    INTTZvtx     * MCz     = new INTTZvtx(INTTClu -> GetRunType(), out_folder_directory, beam_origin, geo_mode_id, phi_diff_cut, DCA_cut, N_clu_cutl, N_clu_cut, zvtx_cal_require, zvtx_QA_width, zvtx_QA_ratio, draw_event_display, peek, false);

    // cout<<"Total event : "<<INTTClu -> GetNEvt()<<endl;

    double out_dist_mean;
    double out_dist_width;
    double out_vtxX;
    double out_vtxY;
    TFile * file_out = new TFile(Form("%s/zvtx_dist_info_%i.root",out_folder_directory.c_str(),in_random_seed), "RECREATE");
    TTree * tree_out = new TTree("tree","info. of zvtx dist");
    tree_out -> Branch("reco_avgX", &out_vtxX);
    tree_out -> Branch("reco_avgY", &out_vtxY);
    tree_out -> Branch("zvtx_dist_mean", &out_dist_mean);
    tree_out -> Branch("zvtx_dist_width", &out_dist_width);

    out_vtxX = beam_origin.first;
    out_vtxY = beam_origin.second;

    for (int event_i = 0; event_i < 20000; event_i ++)
    {
        INTTClu -> EvtInit(event_i);
        INTTClu -> EvtSetCluGroup();

        MCz     -> ProcessEvt(
            event_i, 
            INTTClu -> temp_sPH_inner_nocolumn_vec, INTTClu -> temp_sPH_outer_nocolumn_vec, 
            INTTClu -> temp_sPH_nocolumn_vec, INTTClu -> temp_sPH_nocolumn_rz_vec, 
            INTTClu -> GetNvtxMC(), INTTClu -> GetTrigZvtxMC(), INTTClu -> GetPhiCheckTag(), -1, INTTClu->GetCentralityBin()  // note : no bco_full for MC
        );

        MCz -> ClearEvt();
        INTTClu -> EvtClear();
    }

    // MCz -> PrintPlots();
    MCz -> EndRun();

    out_dist_mean  = MCz -> GetZdiffPeakMC();
    out_dist_width = MCz -> GetZdiffWidthMC();

    tree_out -> Fill();

    file_out -> cd();
    tree_out -> Write();
    file_out -> Close();

    system(Form("mv %s/INTT_zvtx.root %s/INTT_zvtx_%i.root", out_folder_directory.c_str(), out_folder_directory.c_str(), in_random_seed));
    system(Form("mv %s/INTT_zvtx_%i.root %s", out_folder_directory.c_str(), in_random_seed, (input_directory + "/Geo_Z_scan_trial_3/complete_file/folder_"+to_string(core_i)).c_str()));
    system(Form("mv %s/zvtx_dist_info_%i.root %s", out_folder_directory.c_str(), in_random_seed, (input_directory + "/Geo_Z_scan_trial_3/complete_file/folder_"+to_string(core_i)).c_str()));
    system(Form("rm -r %s", out_folder_directory.c_str()));
    
    // system(Form("mv %s %s", out_folder_directory.c_str(), (input_directory + "/Geo_Z_scan_trial_1/complete_file/folder_"+to_string(core_i)).c_str()));

    cout<<"core : "<<core_i<<" seed : "<<in_random_seed<<" run finished"<<endl;

    return;
}