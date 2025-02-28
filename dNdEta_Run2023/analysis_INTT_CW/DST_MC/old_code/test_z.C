#include "INTTReadTree.h"
#include "INTTZvtx.h"

void test_z()
{
    string input_directory = "/sphenix/user/ChengWei/sPH_dNdeta/dNdEta_INTT_MC";
    string file_name = "MC_ZF_1_30400";
    string out_folder_directory = input_directory + "/codetest3_" + file_name;
    string MC_list_name = "dst_INTTdNdEta.list";
    string tree_name = "EventTree";
    
    pair<double, double> beam_origin = {-0.015, 0.012};
    pair<double, double> DCA_cut ={-0.5, 0.5};
    pair<double, double> zvtx_QA_width = {42.32, 68.12}; 
    double zvtx_QA_ratio = 0.00005;
    double phi_diff_cut = 0.11;
    int clu_sum_adc_cut = -1;
    int clu_size_cut = 4;
    int N_clu_cut = 10000;
    int N_clu_cutl = 20;
    
    bool draw_event_display = true;
    double peek = 3.324;
    int data_type = 0; // note : MC
    int geo_mode_id = 0; // note : 0 -> perfect geo
    int zvtx_cal_require = 15;

    INTTReadTree * INTTClu = new INTTReadTree(data_type, input_directory, MC_list_name, tree_name, clu_size_cut, clu_sum_adc_cut);
    INTTZvtx     * MCz     = new INTTZvtx(INTTClu -> GetRunType(), out_folder_directory, beam_origin, geo_mode_id, phi_diff_cut, DCA_cut, N_clu_cutl, N_clu_cut, zvtx_cal_require, zvtx_QA_width, zvtx_QA_ratio, draw_event_display, peek);

    cout<<"Total event : "<<INTTClu -> GetNEvt()<<endl;

    for (int event_i = 0; event_i < 1000; event_i ++)
    {
        INTTClu -> EvtInit(event_i);
        INTTClu -> EvtSetCluGroup();

        MCz     -> ProcessEvt(
            event_i, 
            INTTClu -> temp_sPH_inner_nocolumn_vec, INTTClu -> temp_sPH_outer_nocolumn_vec, 
            INTTClu -> temp_sPH_nocolumn_vec, INTTClu -> temp_sPH_nocolumn_rz_vec, 
            INTTClu -> GetNvtxMC(), INTTClu -> GetTrigZvtxMC(), INTTClu -> GetPhiCheckTag(), -1  // note : no bco_full for MC
        );

        MCz -> ClearEvt();
        INTTClu -> EvtClear();
    }

    MCz -> PrintPlots();
    MCz -> EndRun();
}


        // cout<< INTTClu -> temp_sPH_inner_nocolumn_vec.size()<<" "
        //     << INTTClu -> temp_sPH_outer_nocolumn_vec.size()<<" "
        //     << INTTClu -> temp_sPH_nocolumn_vec.size()<<" "
        //     << INTTClu -> temp_sPH_nocolumn_rz_vec.size()<<" "
        //     << INTTClu -> GetNvtxMC()<<" "
        //     << INTTClu -> GetTrigZvtxMC()<<" "
        //     << INTTClu -> GetPhiCheckTag()<<" "<<endl;