#include "../../../INTTReadTree.h"
#include "../../../INTTXYvtx.h"

void avgXY_mother(int core_i)
{
    string input_directory = "/sphenix/user/ChengWei/sPH_dNdeta/Sim_Ntuple_HIJING_new_20240424_HR";
    string file_name = "ntuple_merged";
    string out_folder_index = to_string(core_i); out_folder_index = string(3 - out_folder_index.length(), '0') + out_folder_index;
    string out_folder_directory = input_directory + "/avg_vtxXY/" + "runXY_" + out_folder_index;
    string tree_name = "EventTree";
    
    // pair<double, double> beam_origin = {-0.015, 0.012};
    // pair<double, double> beam_origin = {-0.0, 0.0};
    pair<double, double> beam_origin = {0.,-2.}; // note : not used 
    pair<double, double> DCA_cut ={-0.5, 0.5};   // note : not used 
    // pair<double, double> zvtx_QA_width = {35, 70}; 
    // double zvtx_QA_ratio = 0.00005;
    double phi_diff_cut = 5.72;
    int clu_sum_adc_cut = 35; // note : used 
    int clu_size_cut = 5;     // note : used 
    int N_clu_cut = 350;      // note : used 
    int N_clu_cutl = 20;      // note : used 
    
    bool draw_event_display = false;
    double peek = 3.324; // note : not used
    int data_type = 0; // note : MC 
    int geo_mode_id = 1; // note : not used
    // int zvtx_cal_require = 15;

    INTTReadTree * INTTClu = new INTTReadTree(data_type, input_directory, file_name, tree_name, clu_size_cut, clu_sum_adc_cut, false);
    
    int start_evt = core_i * 15000;
    int end_evt   = (core_i + 1) * 15000;
    if (start_evt >= INTTClu -> GetNEvt()) { return; }
    if (fabs(start_evt - INTTClu -> GetNEvt()) < 15000) { return; } // note : because the previous will be handling the remaining events
    if (fabs(end_evt - INTTClu -> GetNEvt()) < 15000) { end_evt = INTTClu -> GetNEvt();}
    // if (end_evt   > INTTClu -> GetNEvt()) { end_evt = INTTClu -> GetNEvt();}
    
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
    TFile * file_out = new TFile(Form("%s/run_XY_tree.root",out_folder_directory.c_str()),"RECREATE");
    file_out -> cd();
    TTree * tree_out = new TTree("tree", "tree avg VtxXY");
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

    for (int event_i = start_evt; event_i < end_evt; event_i ++)
    {
        INTTClu -> EvtInit(event_i);
        INTTClu -> EvtSetCluGroup();

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
    MCxy -> PrintPlots();
    // MCxy -> MacroVTXxyCorrection(0,1,2,0.,360.); // note : no phi correction, calculate vertexXY
    // MCxy -> MacroVTXxyCorrection(0,1,2,0.,360.);
    
    // MCxy -> MacroVTXxyCorrection_new(50, 250, 21);
    // vector<pair<double,double>> out_vtx = MCxy -> MacroVTXSquare(3,9);
    // cout<<"The best vertex throughout the scan: "<<out_vtx[0].first<<" "<<out_vtx[0].second<<endl;
    // cout<<"The origin during that scan: "<<out_vtx[1].first<<" "<<out_vtx[1].second<<endl;
    // MCxy -> EndRun();

    vector<pair<double,double>> out_vtx = MCxy -> MacroVTXSquare(4,8);
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
}


        // cout<< INTTClu -> temp_sPH_inner_nocolumn_vec.size()<<" "
        //     << INTTClu -> temp_sPH_outer_nocolumn_vec.size()<<" "
        //     << INTTClu -> temp_sPH_nocolumn_vec.size()<<" "
        //     << INTTClu -> temp_sPH_nocolumn_rz_vec.size()<<" "
        //     << INTTClu -> GetNvtxMC()<<" "
        //     << INTTClu -> GetTrigZvtxMC()<<" "
        //     << INTTClu -> GetPhiCheckTag()<<" "<<endl;