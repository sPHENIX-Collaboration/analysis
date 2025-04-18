#include "../../INTTReadTree.h"
#include "../../INTTXYvtx.h"

void run_xy_398_GeoCheck()
{
    string input_directory = "/sphenix/user/ChengWei/sPH_dNdeta/HIJING_ana398_xvtx-0p04cm_yvtx0p24cm_zvtx-20cm_dummyAlignParams";
    string file_name = "MC_ZF_xyvtx";
    string out_folder_directory = input_directory + "/RunXY_GeoTest_ZeroOffset";
    string MC_list_name = "file_list.txt";
    string tree_name = "EventTree";
    
    // pair<double, double> beam_origin = {-0.015, 0.012};
    // pair<double, double> beam_origin = {-0.0, 0.0};
    // pair<double, double> beam_origin = {1.,-2.};
    pair<double, double> beam_origin = {-0.4, 2.4};
    pair<double, double> DCA_cut ={-0.5, 0.5}; // note : note used
    // pair<double, double> zvtx_QA_width = {35, 70}; 
    // double zvtx_QA_ratio = 0.00005;
    double phi_diff_cut = 0.35; // note : used
    int clu_sum_adc_cut = -1; // note : used
    int clu_size_cut = 4; // note : used
    int N_clu_cut = 350; // note : used
    int N_clu_cutl = 20; // note : used
    
    bool draw_event_display = false;
    double peek = 3.324;
    int data_type = 0; // note : MC
    int geo_mode_id = 0; // note : 0 -> perfect geo
    int zvtx_cal_require = 15;

    INTTReadTree * INTTClu = new INTTReadTree(data_type, input_directory, MC_list_name, tree_name, clu_size_cut, clu_sum_adc_cut);
    INTTXYvtx    * MCxy    = new INTTXYvtx(INTTClu -> GetRunType(), out_folder_directory, beam_origin, geo_mode_id, phi_diff_cut, DCA_cut, N_clu_cutl, N_clu_cut, draw_event_display, peek);

    cout<<"Total event : "<<INTTClu -> GetNEvt()<<endl;

    int count_valid_event = 0;

    for (int event_i = 0; event_i < 20000; event_i ++)
    {
        INTTClu -> EvtInit(event_i);
        INTTClu -> EvtSetCluGroup();

        long total_Nclu = INTTClu -> temp_sPH_inner_nocolumn_vec.size() + INTTClu -> temp_sPH_outer_nocolumn_vec.size();
        if ( total_Nclu < N_clu_cutl || total_Nclu > N_clu_cut ) {MCxy->ClearEvt(); INTTClu->EvtClear(); continue;}

        MCxy     -> ProcessEvt(
            event_i, 
            INTTClu -> temp_sPH_inner_nocolumn_vec, INTTClu -> temp_sPH_outer_nocolumn_vec, 
            INTTClu -> temp_sPH_nocolumn_vec, INTTClu -> temp_sPH_nocolumn_rz_vec, 
            INTTClu -> GetNvtxMC(), INTTClu -> GetTrigZvtxMC(), INTTClu -> GetPhiCheckTag(), -1  // note : no bco_full for MC
        );

        MCxy -> ClearEvt();
        INTTClu -> EvtClear();

        count_valid_event += 1;
        if (count_valid_event >= 2500) {cout<<"got 2500 low-multiplicity events when it's at event : "<<event_i<<endl; break;}
    }
    MCxy -> PrintPlots();

    
    vector<pair<double,double>> out_vtx = MCxy -> MacroVTXSquare(4,10);
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

    // MCxy -> MacroVTXxyCorrection(0,1,2,0.,360.); // note : no phi correction, calculate vertexXY
    // MCxy -> MacroVTXxyCorrection(0,1,2,0.,360.);
    
    // MCxy -> MacroVTXxyCorrection_new(100, 260, 36);
    // vector<pair<double,double>> out_vtx = MCxy -> MacroVTXSquare(3,9);
    // cout<<out_vtx[0].first<<" "<<out_vtx[0].second<<endl;
    // cout<<out_vtx[1].first<<" "<<out_vtx[1].second<<endl;
    MCxy -> EndRun();
}


        // cout<< INTTClu -> temp_sPH_inner_nocolumn_vec.size()<<" "
        //     << INTTClu -> temp_sPH_outer_nocolumn_vec.size()<<" "
        //     << INTTClu -> temp_sPH_nocolumn_vec.size()<<" "
        //     << INTTClu -> temp_sPH_nocolumn_rz_vec.size()<<" "
        //     << INTTClu -> GetNvtxMC()<<" "
        //     << INTTClu -> GetTrigZvtxMC()<<" "
        //     << INTTClu -> GetPhiCheckTag()<<" "<<endl;