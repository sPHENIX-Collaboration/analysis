#include "INTTReadTree.h"
#include "INTTXYvtx.h"

void data20869_xy_geo1_origin()
{
    string input_directory = "/sphenix/user/ChengWei/INTT/INTT_commissioning/ZeroField/20869";
    string file_name = "beam_inttall-00020869-0000_event_base_ana_cluster_full_survey_3.32_excludeR1500_20kEvent_3HotCut";
    string out_folder_directory = input_directory + "/" + "folder_beam_inttall-00020869-0000_event_base_ana_cluster_full_survey_3.32_excludeR1500_20kEvent_3HotCut_VTXxy_origin_seed1886027";
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
    int data_type = 3; // note : data, for the geometry study
    int geo_mode_id = 1; // note : 0 -> perfect geo
    int zvtx_cal_require = 15;
    int N_ladder = 14;
    double offset_range = 0.2;

    vector<string> included_ladder_vec = {
        "B0L000S","B0L002S","B0L003S", "B0L005S", "B0L006S", "B0L008S", "B0L009S", 
        "B1L000S","B1L003S","B1L004S", "B1L007S", "B1L008S", "B1L011S", "B1L012S"
    };

    // vector<string> included_ladder_vec = {
    //     "B0L000","B0L002","B0L003", "B0L005", "B0L006", "B0L008", "B0L009", 
    //     "B1L000","B1L003","B1L004", "B1L007", "B1L008", "B1L011", "B1L012"
    // };

    // note : from the random seed 1886027 
    vector<pair<double,double>> ladder_offset_vec = {
        {0.159441, -0.00337591}, 
        {-0.145018, 0.122129}, 
        {-0.18206, 0.196566}, 
        {0.120321, -0.165647}, 
        {-0.153744, -0.0644527}, 
        {0.0798007, -0.106612}, 
        {0.0508071, -0.117757}, 
        {0.162305, -0.189061}, 
        {0.164888, -0.0421905}, 
        {-0.0503242, 0.110501}, 
        {0.124757, -0.192832}, 
        {0.187674, 0.00304799}, 
        {0.137282, 0.0677386}, 
        {0.169483, -0.0842175}
    };

    INTTReadTree * INTTClu = new INTTReadTree(data_type, input_directory, file_name, tree_name, clu_size_cut, clu_sum_adc_cut, 65535, N_ladder, offset_range, included_ladder_vec, 2, ladder_offset_vec); // note : no geometry offset was applied 
    map<string, pair<double,double>> ladder_offset_map = INTTClu -> GetLadderOffsetMap();
    for (int ladder_i = 0; ladder_i < N_ladder; ladder_i++)
    {
        cout<<"applied geometry offset: "<<ladder_offset_map[included_ladder_vec[ladder_i]].first<<" "<<ladder_offset_map[included_ladder_vec[ladder_i]].second<<endl;
    }

    // INTTReadTree * INTTClu = new INTTReadTree(data_type, input_directory, file_name, tree_name, clu_size_cut, clu_sum_adc_cut); 
    INTTXYvtx    * MCxy    = new INTTXYvtx(INTTClu -> GetRunType(), out_folder_directory, beam_origin, geo_mode_id, phi_diff_cut, DCA_cut, N_clu_cutl, N_clu_cut, draw_event_display, peek);

    cout<<"Total event : "<<INTTClu -> GetNEvt()<<endl;

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
    MCxy -> PrintPlots();
    // MCxy -> MacroVTXxyCorrection(0,1,2,0.,360.); // note : no phi correction, calculate vertexXY
    // MCxy -> MacroVTXxyCorrection(0,1,2,0.,360.);
    
    // MCxy -> MacroVTXxyCorrection_new(50, 250, 21);
    vector<pair<double,double>> out_vtx = MCxy -> MacroVTXSquare(3,9);
    cout<<"The best vertex throughout the scan: "<<out_vtx[0].first<<" "<<out_vtx[0].second<<endl;
    cout<<"The origin during that scan: "<<out_vtx[1].first<<" "<<out_vtx[1].second<<endl;
    cout<<"Fit error, DCA and angle diff: "<<out_vtx[2].first<<" "<<out_vtx[2].second<<endl;
    cout<<"fit pol0 pos Y, DCA and angle diff: "<<out_vtx[3].first<<" "<<out_vtx[3].second<<endl;
    MCxy -> EndRun();
}


        // cout<< INTTClu -> temp_sPH_inner_nocolumn_vec.size()<<" "
        //     << INTTClu -> temp_sPH_outer_nocolumn_vec.size()<<" "
        //     << INTTClu -> temp_sPH_nocolumn_vec.size()<<" "
        //     << INTTClu -> temp_sPH_nocolumn_rz_vec.size()<<" "
        //     << INTTClu -> GetNvtxMC()<<" "
        //     << INTTClu -> GetTrigZvtxMC()<<" "
        //     << INTTClu -> GetPhiCheckTag()<<" "<<endl;