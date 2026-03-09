#include "../../../INTTReadTree.h"
#include "../../../INTTZvtx.h"
#include "../../../ana_map_folder/ana_map_v1.h"
namespace ana_map_version = ANA_MAP_V3;

void evt_vtxZ_geoUnc(int random_seed)
{
    string input_directory  = ana_map_version::MC_input_directory;
    string tree_name        = ana_map_version::MC_tree_name;
    string MC_list_name     = "ntuple_merged";

    string out_random_seed = to_string(random_seed); out_random_seed = string(6 - out_random_seed.length(), '0') + out_random_seed;
    TFile * file_geo_XY = TFile::Open( Form("%s/Geo_test/avg_vtxXY/complete_file/geo_scan_avgXY_%s.root", input_directory.c_str(), out_random_seed.c_str()) );
    TH1F * angle_diff_correction_degree_hist = (TH1F *) file_geo_XY -> Get("angle_diff_correction_degree"); // note : special_tag
    TTree * tree_geo_XY = (TTree*) file_geo_XY -> Get("tree_geo_scan");

    // cout<<"test 1"<<endl;

    vector<double> * in_ladder_offset_x_vec; in_ladder_offset_x_vec = 0;
    vector<double> * in_ladder_offset_y_vec; in_ladder_offset_y_vec = 0;
    vector<double> * in_ladder_offset_z_vec; in_ladder_offset_z_vec = 0;
    double in_total_offset_x;
    double in_total_offset_y;
    double in_total_offset_z;
    double final_avgX;
    double final_avgY;
    int in_random_seed;

    // cout<<"test 2"<<endl;

    tree_geo_XY -> SetBranchAddress("offset_x_vec", &in_ladder_offset_x_vec);
    tree_geo_XY -> SetBranchAddress("offset_y_vec", &in_ladder_offset_y_vec);
    tree_geo_XY -> SetBranchAddress("offset_z_vec", &in_ladder_offset_z_vec);
    tree_geo_XY -> SetBranchAddress("total_offset_x", &in_total_offset_x);
    tree_geo_XY -> SetBranchAddress("total_offset_y", &in_total_offset_y);
    tree_geo_XY -> SetBranchAddress("total_offset_z", &in_total_offset_z);
    tree_geo_XY -> SetBranchAddress("vtxX", &final_avgX); // note : unit : mm
    tree_geo_XY -> SetBranchAddress("vtxY", &final_avgY); // note : unit : mm
    tree_geo_XY -> SetBranchAddress("random_seed", &in_random_seed);

    tree_geo_XY -> GetEntry(0);

    // double unit_correction = 10.;
    final_avgX = final_avgX;
    final_avgY = final_avgY;

    // cout<<"test 3"<<endl;

    if (random_seed != in_random_seed) { cout<<"random_seed not match"<<endl; return; }

    // cout<<"total offset X : "<<in_total_offset_x<<" total offset Y : "<<in_total_offset_y<<endl;
    string out_folder_directory_evtZ = input_directory + "/Geo_test/evt_vtxZ" + "/evtZ_" + out_random_seed;
    system(Form("if [ ! -d %s ]; then mkdir %s; fi", out_folder_directory_evtZ.c_str(), out_folder_directory_evtZ.c_str()));

    // cout<<"test 4"<<endl;

    int index_i = 0;
    map<string, vector<double>> ladder_offset_map; ladder_offset_map.clear();
    // cout<<"test-2"<<endl;
    for (int layer_i = 3; layer_i < 7; layer_i++)
    {
        double N_layer_ladder = (layer_i == 3 || layer_i == 4) ? 12 : 16;

        for (int phi_i = 0; phi_i < N_layer_ladder; phi_i++)
        {
            ladder_offset_map[Form("%i_%i", layer_i, phi_i)] = {in_ladder_offset_x_vec -> at(index_i), in_ladder_offset_y_vec -> at(index_i), in_ladder_offset_z_vec -> at(index_i)};
            index_i += 1;
        }
    }

    // cout<<"test 5"<<endl;

    pair<double, double> beam_origin = {final_avgX, final_avgY};
    cout<<"random_seed : "<<in_random_seed<<" beam_origin : "<<beam_origin.first<<" "<<beam_origin.second<<endl;

    double phi_diff_cut                = ana_map_version::evt_vtxZ_phi_diff_cut;
    pair<double, double> DCA_cut       = ana_map_version::evt_vtxZ_DCA_cut;
    pair<double, double> zvtx_QA_width = ana_map_version::evt_vtxZ_zvtx_QA_width; 
    double zvtx_QA_ratio               = ana_map_version::evt_vtxZ_zvtx_QA_ratio;
    int N_clu_cut                      = ana_map_version::evt_vtxZ_N_clu_cut;
    int N_clu_cutl                     = ana_map_version::evt_vtxZ_N_clu_cutl;
    int clu_sum_adc_cut                = ana_map_version::clu_sum_adc_cut;
    int clu_size_cut                   = ana_map_version::clu_size_cut;
    
    bool draw_event_display = false;
    double peek = 3.324;
    int data_type = 4; // note : MC //todo : change to data_type
    int geo_mode_id = 1; 
    int zvtx_cal_require = ana_map_version::evt_vtxZ_zvtx_cal_require;

    INTTReadTree * INTTClu = new INTTReadTree(data_type, input_directory, MC_list_name, tree_name, clu_size_cut, clu_sum_adc_cut);
    INTTClu -> set_ladder_offset(ladder_offset_map);
    INTTZvtx     * MCz     = new INTTZvtx(INTTClu -> GetRunType(), out_folder_directory_evtZ, beam_origin, geo_mode_id, phi_diff_cut, DCA_cut, N_clu_cutl, N_clu_cut, zvtx_cal_require, zvtx_QA_width, zvtx_QA_ratio, draw_event_display, peek, false, angle_diff_correction_degree_hist); // note : special_tag

    // cout<<"Total event : "<<INTTClu -> GetNEvt()<<endl;

    double out_dist_mean;
    double out_dist_width;
    double out_vtxX;
    double out_vtxY;
    TFile * file_out = new TFile(Form("%s/zvtx_dist_info_%i.root",out_folder_directory_evtZ.c_str(),in_random_seed), "RECREATE");
    TTree * tree_out = new TTree("tree","info. of zvtx dist");
    tree_out -> Branch("reco_avgX", &out_vtxX);
    tree_out -> Branch("reco_avgY", &out_vtxY);
    tree_out -> Branch("zvtx_dist_mean", &out_dist_mean);
    tree_out -> Branch("zvtx_dist_width", &out_dist_width);

    out_vtxX = beam_origin.first;
    out_vtxY = beam_origin.second;

    for (int event_i = 0; event_i < 40000; event_i ++)
    {
        if (event_i % 5000 == 0) {cout<<event_i<<endl;}
        INTTClu -> EvtInit(event_i);
        INTTClu -> EvtSetCluGroup();

        MCz     -> ProcessEvt(
            event_i, 
            INTTClu -> temp_sPH_inner_nocolumn_vec, 
            INTTClu -> temp_sPH_outer_nocolumn_vec, 
            INTTClu -> temp_sPH_nocolumn_vec, 
            INTTClu -> temp_sPH_nocolumn_rz_vec, 
            INTTClu -> GetNvtxMC(), 
            INTTClu -> GetTrigZvtxMC(), 
            INTTClu -> GetPhiCheckTag(), 
            INTTClu -> GetBCOFull(), 
            INTTClu -> GetCentralityBin(),  // note : no bco_full for MC
            INTTClu -> GetMBDRecoZ(),
            INTTClu -> GetIsMinBias(),
            INTTClu -> GetIsMinBiasWoZDC(),
            INTTClu -> GetMBDNorthChargeSum(),
            INTTClu -> GetMBDSouthChargeSum()
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

    // system(Form("mv %s/INTT_zvtx.root %s/INTT_zvtx_%i.root", out_folder_directory_evtZ.c_str(), out_folder_directory_evtZ.c_str(), in_random_seed));
    system(Form("cp %s/INTT_zvtx.root %s/INTT_zvtx_%s.root", out_folder_directory_evtZ.c_str(), (input_directory + "/Geo_test/evt_vtxZ/complete_file").c_str(), out_random_seed.c_str()));
    system(Form("cp %s/zvtx_dist_info_%i.root %s", out_folder_directory_evtZ.c_str(), in_random_seed, (input_directory + "/Geo_test/evt_vtxZ/complete_file").c_str()));
    system(Form("rm -r %s", out_folder_directory_evtZ.c_str()));
    
    system(Form("rm /sphenix/user/ChengWei/INTT/INTT_dNdeta_repo/DST_MC/control_files/GeoUncStudy/evt_vtxZ/run_root_sub_evtZ_%i.sh", random_seed));
    system(Form("rm %s/CW_log/condor_output_%i.err", (input_directory + "/Geo_test/evt_vtxZ").c_str(), random_seed));
    system(Form("rm %s/CW_log/condor_output_%i.log", (input_directory + "/Geo_test/evt_vtxZ").c_str(), random_seed));
    system(Form("mv %s/Geo_test/avg_vtxXY/complete_file/geo_scan_avgXY_%s.root %s/Geo_test/avg_vtxXY/complete_file/zvtx_used", input_directory.c_str(), out_random_seed.c_str(), input_directory.c_str()));
    // system(Form("mv %s %s", out_folder_directory.c_str(), (input_directory + "/Geo_Z_scan_trial_1/complete_file/folder_"+to_string(core_i)).c_str()));

    cout<<" seed : "<<in_random_seed<<" run finished"<<endl;

    return;
}