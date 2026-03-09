#include "../../../INTTReadTree.h"
#include "../../../INTTXYvtx.h"
#include "../../../ana_map_folder/ana_map_v1.h"
namespace ana_map_version = ANA_MAP_V3;


map<string, vector<double>> run_avgXY_run_sample(
    int random_seed,
    int core_i,
    pair<vector<double>, vector<pair<double, double>>> &angle_diff_correction_degree_vec,
    bool run_angle_diff_test = false,
    pair<double, double> beam_origin_in = ana_map_version::MC_beam_origin
)
{
    if (run_angle_diff_test == true) {core_i = 999;}

    string input_directory = ana_map_version::MC_input_directory;
    string file_name       = ana_map_version::MC_file_name;
    string tree_name       = ana_map_version::MC_tree_name;

    string out_random_seed = to_string(random_seed); out_random_seed = string(6 - out_random_seed.length(), '0') + out_random_seed;
    string out_folder_directory_mother = input_directory + "/Geo_test/avg_vtxXY/avgXY_" + out_random_seed;

    string out_folder_index = to_string(core_i); out_folder_index = string(3 - out_folder_index.length(), '0') + out_folder_index;
    string out_folder_directory = out_folder_directory_mother + "/runXY_" + out_folder_index;

    system(Form("if [ ! -d %s ]; then mkdir %s; fi", out_folder_directory_mother.c_str(), out_folder_directory_mother.c_str()));
    
    int Nevt_per_core   = ana_map_version::avg_vtxXY_Nevt_per_core;
    int clu_sum_adc_cut = ana_map_version::clu_sum_adc_cut;      // note : used 
    int clu_size_cut    = ana_map_version::clu_size_cut;         // note : used 
    int N_clu_cut       = ana_map_version::avg_vtxXY_N_clu_cut;  // note : used 
    int N_clu_cutl      = ana_map_version::avg_vtxXY_N_clu_cutl; // note : used 
    int data_type       = 4;         // note : used, MC geotest -----------------------------------------------
    bool draw_event_display = false;

    pair<double, double> beam_origin = beam_origin_in; // note : not used 
    pair<double, double> DCA_cut ={-0.5, 0.5};   // note : not used 
    double phi_diff_cut = 5.72; // note : not used
    double peek = 3.324;        // note : not used
    int geo_mode_id = 1;        // note : not used

    int N_ladder = 56; // note : not used
    double offset_range = 0.25;        // note : ----------------------------------------------------------------------------------------------
    int apply_geo_offset_mode = 3;
    vector<string> included_ladder_vec = {}; // note : not used

    // INTTReadTree * INTTClu = new INTTReadTree(data_type, input_directory, file_name, tree_name, clu_size_cut, clu_sum_adc_cut, false);
    INTTReadTree * INTTClu = new INTTReadTree(data_type, input_directory, file_name, tree_name, clu_size_cut, clu_sum_adc_cut, random_seed, N_ladder, offset_range, included_ladder_vec, apply_geo_offset_mode);
    INTTClu -> gen_ladder_offset();

    map<string, vector<double>> ladder_offset_map = INTTClu -> GetLadderOffsetMap();

    int start_evt = core_i * Nevt_per_core;
    int end_evt   = (core_i + 1) * Nevt_per_core;

    if (run_angle_diff_test == true)
    {
        start_evt = 0;
        end_evt = INTTClu -> GetNEvt();
    }

    if (start_evt >= INTTClu -> GetNEvt()) { return {}; }
    if (fabs(start_evt - INTTClu -> GetNEvt()) < Nevt_per_core) { return {}; } // note : because the previous will be handling the remaining events                                
    if (fabs(end_evt - INTTClu -> GetNEvt()) <= Nevt_per_core) { end_evt = INTTClu -> GetNEvt();}
    
    system(Form("mkdir %s",out_folder_directory.c_str()));

    if (run_angle_diff_test == true)
    {
        cout<<"in angle diff test"<<endl;
        cout<<"final running event confirmation : "<<start_evt<<" "<<end_evt<<endl;
    }

    INTTXYvtx    * MCxy    = new INTTXYvtx(INTTClu -> GetRunType(), out_folder_directory, beam_origin, geo_mode_id, phi_diff_cut, DCA_cut, N_clu_cutl, N_clu_cut, draw_event_display, peek);

    cout<<"Total event : "<<INTTClu -> GetNEvt()<<endl;

    long out_start_evt;
    long out_end_evt;
    double out_quadrant_corner_X;
    double out_quadrant_corner_Y;
    double out_quadrant_center_X;
    double out_quadrant_center_Y;
    double out_line_filled_mean_X;
    double out_line_filled_mean_Y;
    double out_line_filled_meanE_X;
    double out_line_filled_meanE_Y;
    double out_line_filled_stddev_X;
    double out_line_filled_stddev_Y;
    double out_line_filled_variance_X;
    double out_line_filled_variance_Y;
    double out_line_filled_covariance;

    TFile * file_out;
    TTree * tree_out;

    if (run_angle_diff_test == false){
        file_out = new TFile(Form("%s/run_XY_tree.root",out_folder_directory.c_str()),"RECREATE");
        file_out -> cd();
        tree_out = new TTree("tree", "tree avg VtxXY");
        tree_out -> Branch("start_evt",&out_start_evt);
        tree_out -> Branch("end_evt",&out_end_evt);
        tree_out -> Branch("quadrant_corner_X",&out_quadrant_corner_X);
        tree_out -> Branch("quadrant_corner_Y",&out_quadrant_corner_Y);
        tree_out -> Branch("quadrant_center_X",&out_quadrant_center_X);
        tree_out -> Branch("quadrant_center_Y",&out_quadrant_center_Y);
        tree_out -> Branch("line_filled_mean_X",&out_line_filled_mean_X);
        tree_out -> Branch("line_filled_mean_Y",&out_line_filled_mean_Y);
        tree_out -> Branch("line_filled_meanE_X",&out_line_filled_meanE_X);
        tree_out -> Branch("line_filled_meanE_Y",&out_line_filled_meanE_Y);
        tree_out -> Branch("line_filled_stddev_X",&out_line_filled_stddev_X);
        tree_out -> Branch("line_filled_stddev_Y",&out_line_filled_stddev_Y);
        tree_out -> Branch("line_filled_variance_X",&out_line_filled_variance_X);
        tree_out -> Branch("line_filled_variance_Y",&out_line_filled_variance_Y);
        tree_out -> Branch("line_filled_covariance",&out_line_filled_covariance);
    }

    for (int event_i = start_evt; event_i < end_evt; event_i ++)
    {
        INTTClu -> EvtInit(event_i);
        INTTClu -> EvtSetCluGroup();

        if (INTTClu -> GetMBDRecoZ() != INTTClu -> GetMBDRecoZ()) {MCxy -> ClearEvt(); INTTClu -> EvtClear(); continue;}
        if (INTTClu -> GetCentralityBin() != INTTClu -> GetCentralityBin()) {MCxy -> ClearEvt(); INTTClu -> EvtClear(); continue;}

        MCxy     -> ProcessEvt(
            event_i, 
            INTTClu -> temp_sPH_inner_nocolumn_vec, 
            INTTClu -> temp_sPH_outer_nocolumn_vec, 
            INTTClu -> temp_sPH_nocolumn_vec, 
            INTTClu -> temp_sPH_nocolumn_rz_vec, 
            INTTClu -> GetNvtxMC(), 
            INTTClu -> GetTrigZvtxMC(), 
            INTTClu -> GetPhiCheckTag(), 
            INTTClu -> GetBCOFull(),
            INTTClu -> GetIsMinBias(),
            INTTClu -> GetIsMinBiasWoZDC()
        );

        MCxy -> ClearEvt();
        INTTClu -> EvtClear();
    }
    // MCxy -> PrintPlots();
    // MCxy -> MacroVTXxyCorrection(0,1,2,0.,360.); // note : no phi correction, calculate vertexXY
    // MCxy -> MacroVTXxyCorrection(0,1,2,0.,360.);
    
    // MCxy -> MacroVTXxyCorrection_new(50, 250, 21);
    // vector<pair<double,double>> out_vtx = MCxy -> MacroVTXSquare(3,9);
    // cout<<"The best vertex throughout the scan: "<<out_vtx[0].first<<" "<<out_vtx[0].second<<endl;
    // cout<<"The origin during that scan: "<<out_vtx[1].first<<" "<<out_vtx[1].second<<endl;
    // MCxy -> EndRun();

    if (run_angle_diff_test == true)
    {
        MCxy -> RunDeltaPhiStudy();
        TProfile * angle_diff_correction_degree = MCxy -> Get_final_angle_diff_inner_phi_degree_coarseX_peak_profile();

        angle_diff_correction_degree_vec.first = {
            double(angle_diff_correction_degree -> GetNbinsX()), 
            angle_diff_correction_degree -> GetXaxis() -> GetXmin(), 
            angle_diff_correction_degree -> GetXaxis() -> GetXmax()
        };

        for (int i = 0; i < angle_diff_correction_degree -> GetNbinsX(); i++)
        {
            angle_diff_correction_degree_vec.second.push_back({angle_diff_correction_degree -> GetBinCenter(i+1), angle_diff_correction_degree -> GetBinContent(i+1)});
        }

        MCxy -> EndRun();
        return {};
    }

    vector<pair<double,double>> out_vtx = MCxy -> MacroVTXSquare(ana_map_version::avg_vtxXY_quadrant_window_size,ana_map_version::avg_vtxXY_quadrant_Niteration, true); // note : special_tag
    cout<<" "<<endl;
    cout<<"The best vertex throughout the scan: "<<out_vtx[0].first<<" "<<out_vtx[0].second<<endl;
    cout<<"The origin during that scan: "<<out_vtx[1].first<<" "<<out_vtx[1].second<<endl;
    cout<<"Fit error, DCA and angle diff: "<<out_vtx[2].first<<" "<<out_vtx[2].second<<endl;
    cout<<"fit pol0 pos Y, DCA and angle diff: "<<out_vtx[3].first<<" "<<out_vtx[3].second<<endl;

    vector<pair<double,double>> out_vtx_line = MCxy -> FillLine_FindVertex({(out_vtx[0].first + out_vtx[1].first)/2., (out_vtx[0].second + out_vtx[1].second)/2.}, 0.001);
    cout<<" "<<endl;
    cout<<"By fill-line method,"<<endl;
    cout<<"Reco Run Vertex XY: "<<out_vtx_line[0].first<<" "<<out_vtx_line[0].second<<endl;
    cout<<"Reco Run Vertex XY Error: "<<out_vtx_line[1].first<<" "<<out_vtx_line[1].second<<endl;

    vector<double> covariance_matrix = MCxy -> LineFill_bkgrm_Get_covariance();
    cout<<" "<<endl;
    cout<<"Mean X : "<<covariance_matrix[0]<<" Mean Y : "<<covariance_matrix[1]<<endl;
    cout<<"Variance X : "<<covariance_matrix[2]<<" Variance Y : "<<covariance_matrix[3]<<endl;
    cout<<"Covariance : "<<covariance_matrix[4]<<endl;

    out_start_evt = start_evt;
    out_end_evt = end_evt - 1.;
    out_quadrant_corner_X = out_vtx[0].first;
    out_quadrant_corner_Y = out_vtx[0].second;
    out_quadrant_center_X = out_vtx[1].first;
    out_quadrant_center_Y = out_vtx[1].second;
    out_line_filled_mean_X = out_vtx_line[0].first;
    out_line_filled_mean_Y = out_vtx_line[0].second;
    out_line_filled_meanE_X = out_vtx_line[1].first;
    out_line_filled_meanE_Y = out_vtx_line[1].second;
    out_line_filled_stddev_X = out_vtx_line[2].first;
    out_line_filled_stddev_Y = out_vtx_line[2].second;

    out_line_filled_variance_X = covariance_matrix[2];
    out_line_filled_variance_Y = covariance_matrix[3];
    out_line_filled_covariance = covariance_matrix[4];

    TH2F * out_line_filled_2D = MCxy -> GetHistFinal().first[4];
    TH2F * out_line_filled_bkg_2D = MCxy -> GetHistFinal().first[5];

    tree_out -> Fill();

    file_out -> cd();
    // tree_out -> SetDirectory(file_out);
    out_line_filled_2D -> Write(Form("line_filled_2D_%i", core_i));
    out_line_filled_bkg_2D -> Write(Form("line_filled_bkg_2D_%i", core_i));
    tree_out -> Write();

    file_out -> Close();

    MCxy -> EndRun();

    return ladder_offset_map;
}

void avg_vtxXY_geoUnc(int random_seed)
{   
    string input_directory = ana_map_version::MC_input_directory;
    string out_random_seed = to_string(random_seed); out_random_seed = string(6 - out_random_seed.length(), '0') + out_random_seed;
    string out_folder_directory_mother_mother = input_directory + "/Geo_test/avg_vtxXY";
    string out_folder_directory_mother = out_folder_directory_mother_mother + "/avgXY_" + out_random_seed;

    system(Form("if [ -d %s ]; then rm -r %s; fi", out_folder_directory_mother.c_str(), out_folder_directory_mother.c_str()));
    sleep(2);

    pair<vector<double>,vector<pair<double,double>>> angle_diff_correction_degree_vec;
    angle_diff_correction_degree_vec.first.clear();
    angle_diff_correction_degree_vec.second.clear();

    // todo : the number of cores is hardwritten here, 6 for 80k events, as one core is handling 15k events
    map<string, vector<double>> ladder_offset_map;
    vector<map<string, vector<double>>> ladder_offset_map_all; ladder_offset_map_all.clear();
    for (int i = 0; i < 6; i++)
    {   
        if (i == 0) {
            ladder_offset_map = run_avgXY_run_sample(random_seed, i, angle_diff_correction_degree_vec);
            ladder_offset_map_all.push_back(ladder_offset_map);
        }
        else {
            ladder_offset_map_all.push_back(run_avgXY_run_sample(random_seed, i, angle_diff_correction_degree_vec));
        }
    }

    // for (int layer_i = 3; layer_i < 7; layer_i++)
    // {
    //     double N_layer_ladder = (layer_i == 3 || layer_i == 4) ? 12 : 16;

    //     for (int phi_i = 0; phi_i < N_layer_ladder; phi_i++)
    //     {
    //         for (int i = 0; i < ladder_offset_map_all.size() - 1; i++)
    //         {
    //             cout<<"ladder : "<<Form("%i_%i", layer_i, phi_i)<<" offset X, file : "<<i<<" "<<ladder_offset_map_all[i][Form("%i_%i", layer_i, phi_i)][0]<<endl;
    //         }

    //         for (int i = 0; i < ladder_offset_map_all.size() - 1; i++)
    //         {
    //             cout<<"ladder : "<<Form("%i_%i", layer_i, phi_i)<<" offset Y, file : "<<i<<" "<<ladder_offset_map_all[i][Form("%i_%i", layer_i, phi_i)][1]<<endl;
    //         }

    //         for (int i = 0; i < ladder_offset_map_all.size() - 1; i++)
    //         {
    //             cout<<"ladder : "<<Form("%i_%i", layer_i, phi_i)<<" offset Z, file : "<<i<<" "<<ladder_offset_map_all[i][Form("%i_%i", layer_i, phi_i)][2]<<endl;
    //         }

    //     }
    // }
    
    system(Form("/sphenix/user/ChengWei/INTT/INTT_dNdeta_repo/DST_MC/runXY_stability/run_avg_vtxXY_merge_result 1 %s", out_folder_directory_mother.c_str()));

    TFile * file_in = TFile::Open(Form("%s/merged_result/determined_avg_vtxXY.root",out_folder_directory_mother.c_str()));
    TTree * tree_in = (TTree*)file_in->Get("tree");
    double in_final_num_avgX;
    double in_final_num_avgY;
    tree_in -> SetBranchAddress("final_num_avgX", &in_final_num_avgX); // note : unit : cm
    tree_in -> SetBranchAddress("final_num_avgY", &in_final_num_avgY); // note : unit : cm
    tree_in -> GetEntry(0);


    TFile * file_out = new TFile(Form("%s/geo_scan_avgXY.root",out_folder_directory_mother.c_str()), "recreate");
    TTree * tree_out = new TTree("tree_geo_scan", "tree_geo_scan");

    vector<double> offset_x_vec; offset_x_vec.clear();
    vector<double> offset_y_vec; offset_y_vec.clear();
    vector<double> offset_z_vec; offset_z_vec.clear();
    double total_offset_x = 0;
    double total_offset_y = 0;
    double total_offset_z = 0;

    double vtxX = in_final_num_avgX * 10.; // note : unit : mm 
    double vtxY = in_final_num_avgY * 10.; // note : unit : mm

    tree_out -> Branch("offset_x_vec", &offset_x_vec);
    tree_out -> Branch("offset_y_vec", &offset_y_vec);
    tree_out -> Branch("offset_z_vec", &offset_z_vec);
    tree_out -> Branch("total_offset_x", &total_offset_x);
    tree_out -> Branch("total_offset_y", &total_offset_y);
    tree_out -> Branch("total_offset_z", &total_offset_z);
    tree_out -> Branch("vtxX", &vtxX);
    tree_out -> Branch("vtxY", &vtxY);
    tree_out -> Branch("random_seed", &random_seed);

    for (int layer_i = 3; layer_i < 7; layer_i++)
    {
        double N_layer_ladder = (layer_i == 3 || layer_i == 4) ? 12 : 16;

        for (int phi_i = 0; phi_i < N_layer_ladder; phi_i++)
        {
            offset_x_vec.push_back( ladder_offset_map[Form("%i_%i", layer_i, phi_i)][0] );
            offset_y_vec.push_back( ladder_offset_map[Form("%i_%i", layer_i, phi_i)][1] );
            offset_z_vec.push_back( ladder_offset_map[Form("%i_%i", layer_i, phi_i)][2] );

            total_offset_x += fabs(ladder_offset_map[Form("%i_%i", layer_i, phi_i)][0]);
            total_offset_y += fabs(ladder_offset_map[Form("%i_%i", layer_i, phi_i)][1]);
            total_offset_z += fabs(ladder_offset_map[Form("%i_%i", layer_i, phi_i)][2]);
        }
    }
    
    tree_out -> Fill();
    
    

    angle_diff_correction_degree_vec.first.clear();
    angle_diff_correction_degree_vec.second.clear();
    run_avgXY_run_sample(random_seed, 999, angle_diff_correction_degree_vec, true, {vtxX, vtxY});

    TH1F * angle_diff_correction_degree = new TH1F(
        "angle_diff_correction_degree", 
        "angle_diff_correction_degree", 
        int(angle_diff_correction_degree_vec.first[0]), 
        angle_diff_correction_degree_vec.first[1], 
        angle_diff_correction_degree_vec.first[2]
    );

    for (int i = 0; i < angle_diff_correction_degree_vec.second.size(); i++)
    {
        angle_diff_correction_degree -> SetBinContent(
            angle_diff_correction_degree -> FindBin(angle_diff_correction_degree_vec.second[i].first), 
            angle_diff_correction_degree_vec.second[i].second
        );

        // cout<<"angle diff correction degree : "<<angle_diff_correction_degree_vec.second[i].first<<" "<<angle_diff_correction_degree_vec.second[i].second<<endl;
    }

    file_out -> cd();
    // tree_out->SetDirectory(file_out);
    angle_diff_correction_degree -> Write("angle_diff_correction_degree");
    tree_out -> Write("", TObject::kOverwrite);
    file_out -> Close();

    system(Form("cp %s/geo_scan_avgXY.root %s/complete_file/geo_scan_avgXY_%s.root", out_folder_directory_mother.c_str(), out_folder_directory_mother_mother.c_str(), out_random_seed.c_str()));
    system(Form("rm /sphenix/user/ChengWei/INTT/INTT_dNdeta_repo/DST_MC/control_files/GeoUncStudy/avg_vtxXY/run_root_sub_avgXY_%i.sh", random_seed));
    system(Form("rm %s/CW_log/condor_output_%i.err", out_folder_directory_mother_mother.c_str(), random_seed));
    system(Form("rm %s/CW_log/condor_output_%i.log", out_folder_directory_mother_mother.c_str(), random_seed));
    

}