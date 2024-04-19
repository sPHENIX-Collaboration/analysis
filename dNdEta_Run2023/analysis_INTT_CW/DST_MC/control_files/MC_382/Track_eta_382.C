#include "INTTReadTree.h"
#include "INTTEta.h"

void Track_eta_382()
{
    string input_directory = "/sphenix/user/ChengWei/sPH_dNdeta/dNdEta_INTT_MC_382";
    string file_name = "MC_ZF_zvtx";
    // string out_folder_directory = input_directory + "/CodeTest1_combine_FullZ_TrackEta_" + file_name;
    string out_folder_directory = input_directory + "/MegaTrackFinderClassTest1_combine_FullZ_TrackEta_" + file_name;
    string MC_list_name = "dst_INTTdNdEta_382_small.list";
    string tree_name = "EventTree";
    
    // pair<double, double> beam_origin = {-0.015, 0.012};
    // pair<double, double> beam_origin = {-0.0, 0.0};
    // pair<double, double> beam_origin = {-0.4, 2.4};
    // pair<double, double> beam_origin = {-0.394532, 2.40234}; // note : line fill + quadrant method average
    pair<double, double> beam_origin = {0,0}; // note : line fill + quadrant method average
    pair<double, double> DCA_cut = {-3, 3}; // {-1, 1};
    pair<double, double> zvtx_QA_width = {35, 70}; 
    double zvtx_QA_ratio = 0.00005;
    double phi_diff_cut = 3; // 0.3;
    int clu_sum_adc_cut = -1;
    int clu_size_cut = 8;
    int N_clu_cut = 10000;
    int N_clu_cutl = 20;
    
    bool draw_event_display = false;
    double peek = 3.324;
    int data_type = 0; // note : MC
    // int data_type = 5; // note : MC, rotate the cluster of the inner barrel by 180 degrees
    int geo_mode_id = 0; // note : 0 -> perfect geo
    int zvtx_cal_require = 15;

    INTTReadTree * INTTClu = new INTTReadTree(data_type, input_directory, MC_list_name, tree_name, clu_size_cut, clu_sum_adc_cut);
    INTTEta     * MCEta     = new INTTEta(INTTClu -> GetRunType(), out_folder_directory, beam_origin, geo_mode_id, phi_diff_cut, DCA_cut, N_clu_cutl, N_clu_cut, draw_event_display, peek, 0, 3, false);

    cout<<"Total event : "<<INTTClu -> GetNEvt()<<endl;

    for (int event_i = 0; event_i < 500; event_i ++)
    {
        INTTClu -> EvtInit(event_i);
        INTTClu -> EvtSetCluGroup();

        MCEta     -> ProcessEvt(
            event_i, 
            INTTClu -> temp_sPH_inner_nocolumn_vec, INTTClu -> temp_sPH_outer_nocolumn_vec, 
            INTTClu -> temp_sPH_nocolumn_vec, INTTClu -> temp_sPH_nocolumn_rz_vec, 
            INTTClu -> GetNvtxMC(), INTTClu -> GetTrigvtxMC(), INTTClu -> GetPhiCheckTag(), -1, {INTTClu -> GetTrigvtxMC()[2]*10,2}, INTTClu->GetCentralityBin(), INTTClu->GetTrueTrackInfo() // note : no bco_full for MC
        );

        MCEta -> ClearEvt();
        INTTClu -> EvtClear();
    }

    MCEta -> PrintPlots();
    MCEta -> EndRun();
}


        // cout<< INTTClu -> temp_sPH_inner_nocolumn_vec.size()<<" "
        //     << INTTClu -> temp_sPH_outer_nocolumn_vec.size()<<" "
        //     << INTTClu -> temp_sPH_nocolumn_vec.size()<<" "
        //     << INTTClu -> temp_sPH_nocolumn_rz_vec.size()<<" "
        //     << INTTClu -> GetNvtxMC()<<" "
        //     << INTTClu -> GetTrigZvtxMC()<<" "
        //     << INTTClu -> GetPhiCheckTag()<<" "<<endl;