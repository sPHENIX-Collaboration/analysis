#include "INTTReadTree.h"
// #include "INTTZvtx.h"
#include "INTTXYvtxEvt.h"

void test_xyEvt_388_000()
{
    string input_directory = "/sphenix/user/ChengWei/sPH_dNdeta/dNdEta_INTT_MC_388_000";
    string file_name = "MC_ZF_zvtx";
    string out_folder_directory = input_directory + "/test1Evt_" + file_name;
    string MC_list_name = "dst_INTT_dNdEta_388_000.list";
    string tree_name = "EventTree";
    
    // pair<double, double> beam_origin = {-0.015, 0.012};
    // pair<double, double> beam_origin = {-0.0, 0.0};
    pair<double, double> beam_origin = {-0.4, 2.4};
    pair<double, double> DCA_cut = {-1, 1}; //{-0.5, 0.5};
    pair<double, double> zvtx_QA_width = {35, 70}; 
    double zvtx_QA_ratio = 0.00005;
    double phi_diff_cut = 0.3;
    int clu_sum_adc_cut = -1;
    int clu_size_cut = 4;
    int N_clu_cut = 10000;
    int N_clu_cutl = 100;
    
    bool draw_event_display = false;
    double peek = 3.324;
    int data_type = 0; // note : MC
    int geo_mode_id = 0; // note : 0 -> perfect geo
    int zvtx_cal_require = 15;

    INTTReadTree * INTTClu = new INTTReadTree(data_type, input_directory, MC_list_name, tree_name, clu_size_cut, clu_sum_adc_cut);
    // INTTZvtx     * MCz     = new INTTZvtx(INTTClu -> GetRunType(), out_folder_directory, beam_origin, geo_mode_id, phi_diff_cut, DCA_cut, N_clu_cutl, N_clu_cut, zvtx_cal_require, zvtx_QA_width, zvtx_QA_ratio, draw_event_display, peek);
    INTTXYvtxEvt * MCxy    = new INTTXYvtxEvt(INTTClu -> GetRunType(), out_folder_directory, beam_origin, geo_mode_id, phi_diff_cut, DCA_cut, N_clu_cutl, N_clu_cut, draw_event_display, peek, 0, 3, false, 4, 10);

    cout<<"Total event : "<<INTTClu -> GetNEvt()<<endl;

    for (int event_i = 0; event_i < 10000; event_i ++)
    {
        INTTClu -> EvtInit(event_i);
        INTTClu -> EvtSetCluGroup();

        // cout<<"test0 ----------- ----------- ----------- ----------- ----------- ------------------------------------------------------------------------------------------------------------------------------------"<<endl;
        // cout<<"test0 ----------- ----------- ----------- ----------- ----------- ------------------------------------------------------------------------------------------------------------------------------------"<<endl;

        // MCz     -> ProcessEvt(
        //     event_i, 
        //     INTTClu -> temp_sPH_inner_nocolumn_vec, INTTClu -> temp_sPH_outer_nocolumn_vec, 
        //     INTTClu -> temp_sPH_nocolumn_vec, INTTClu -> temp_sPH_nocolumn_rz_vec, 
        //     INTTClu -> GetNvtxMC(), INTTClu -> GetTrigZvtxMC(), INTTClu -> GetPhiCheckTag(), -1  // note : no bco_full for MC
        // );

        // cout<<"test1"<<endl;

        // cout<<"test1.1"<<endl;
        MCxy     -> ProcessEvt(
            event_i, 
            INTTClu -> temp_sPH_inner_nocolumn_vec, INTTClu -> temp_sPH_outer_nocolumn_vec, 
            INTTClu -> temp_sPH_nocolumn_vec, INTTClu -> temp_sPH_nocolumn_rz_vec, 
            INTTClu -> GetNvtxMC(), INTTClu -> GetTrigvtxMC(), INTTClu -> GetPhiCheckTag(), -1, {INTTClu -> GetTrigvtxMC()[2]*10,0}  // note : no bco_full for MC
        );

        // if (MCz -> GetEvtZPeak()[0] == 1) // note : have good zvtx tag
        // {
            
        // }

        // cout<<"test2"<<endl;

        // MCz -> ClearEvt();
        MCxy -> ClearEvt();
        INTTClu -> EvtClear();
    }

    MCxy -> PrintPlots_Evt();
    MCxy -> EndRun();
    // MCz -> PrintPlots();
    // MCz -> EndRun();
}


        // cout<< INTTClu -> temp_sPH_inner_nocolumn_vec.size()<<" "
        //     << INTTClu -> temp_sPH_outer_nocolumn_vec.size()<<" "
        //     << INTTClu -> temp_sPH_nocolumn_vec.size()<<" "
        //     << INTTClu -> temp_sPH_nocolumn_rz_vec.size()<<" "
        //     << INTTClu -> GetNvtxMC()<<" "
        //     << INTTClu -> GetTrigZvtxMC()<<" "
        //     << INTTClu -> GetPhiCheckTag()<<" "<<endl;