#include "../../INTTReadTree.h"
#include "../../INTTXYvtx.h"

void data20869_xy_GeoCheck()
{
    string input_directory = "/sphenix/user/ChengWei/INTT/INTT_commissioning/ZeroField/20869";
    string file_name = "beam_inttall-00020869-0000_event_base_ana_cluster_full_survey_3.32_excludeR1500_20kEvent_3HotCut";
    string out_folder_directory = input_directory + "/" + "SemiFinal_RunXY_"+ file_name;
    string tree_name = "tree_clu";
    
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
    int data_type = 2; // note : MC
    int geo_mode_id = 1; // note : 0 -> perfect geo
    int zvtx_cal_require = 15;

    INTTReadTree * INTTClu = new INTTReadTree(data_type, input_directory, file_name, tree_name, clu_size_cut, clu_sum_adc_cut, false);
    INTTXYvtx    * MCxy    = new INTTXYvtx(INTTClu -> GetRunType(), out_folder_directory, beam_origin, geo_mode_id, phi_diff_cut, DCA_cut, N_clu_cutl, N_clu_cut, draw_event_display, peek);

    cout<<"Total event : "<<INTTClu -> GetNEvt()<<endl;

    for (int event_i = 0; event_i < 100000; event_i ++)
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
    MCxy -> PrintPlots();
    // MCxy -> MacroVTXxyCorrection(0,1,2,0.,360.); // note : no phi correction, calculate vertexXY
    // MCxy -> MacroVTXxyCorrection(0,1,2,0.,360.);
    
    // MCxy -> MacroVTXxyCorrection_new(50, 250, 21);
    // vector<pair<double,double>> out_vtx = MCxy -> MacroVTXSquare(3,9);
    // cout<<"The best vertex throughout the scan: "<<out_vtx[0].first<<" "<<out_vtx[0].second<<endl;
    // cout<<"The origin during that scan: "<<out_vtx[1].first<<" "<<out_vtx[1].second<<endl;
    // MCxy -> EndRun();

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

    MCxy -> EndRun();
}


        // cout<< INTTClu -> temp_sPH_inner_nocolumn_vec.size()<<" "
        //     << INTTClu -> temp_sPH_outer_nocolumn_vec.size()<<" "
        //     << INTTClu -> temp_sPH_nocolumn_vec.size()<<" "
        //     << INTTClu -> temp_sPH_nocolumn_rz_vec.size()<<" "
        //     << INTTClu -> GetNvtxMC()<<" "
        //     << INTTClu -> GetTrigZvtxMC()<<" "
        //     << INTTClu -> GetPhiCheckTag()<<" "<<endl;